#include "StdAfx.h"

#if defined(_MSC_VER)
# pragma warning(disable:4786)
#endif

#include <ATOM_utils.h>
#include "mem.h"
#include "memlog.h"
#include "logger.h"
#include "assertion.h"
#include "callstack.h"
#include "utils.h"
#include "stacktrace.h"

/* prime numbers
  53ul,
  97ul,
  193ul,
  389ul,
  769ul,
  1543ul,
  3079ul,
  6151ul,
  12289ul,
  24593ul,
  49157ul,
  98317ul,
  196613ul,
  393241ul,
  786433ul,
  1572869ul,
  3145739ul,
  6291469ul,
  12582917ul,
  25165843ul,
  50331653ul,
  100663319ul,
  201326611ul,
  402653189ul,
  805306457ul,
  1610612741ul,
  3221225473ul,
  4294967291ul
*/

static const unsigned long num_buckets = 12289ul;
extern volatile long UseSymServer;

struct ModuleName
{
  char _modulename[MAX_PATH];

  ModuleName (void)
  {
    char buffer[MAX_PATH];
    ::GetModuleFileNameA (NULL, buffer, MAX_PATH);
    char *p = strrchr (buffer, '\\');
    strcpy (_modulename, p+1);
    p[1] = '\0';
    strcat (buffer, "memlog");
    _mkdir (buffer);

    strcat (buffer, "\\");
    strcat (buffer, _modulename);
    strcpy (_modulename, buffer);
  }
};

extern int TestBlock (void *p, char *error);

ATOM_MemLog::ATOM_MemLog (void)
{
  m_buckets = new EntryArray[num_buckets];
  m_fperr = 0;
}

ATOM_MemLog::~ATOM_MemLog (void)
{
  for (unsigned i = 0; i < num_buckets; ++i)
  {
    for (unsigned j = 0; j < m_buckets[i].size(); ++j)
    {
      delete m_buckets[i][j];
    }
  }
  delete [] m_buckets;

  if (m_fperr)
  {
    fclose (m_fperr);
  }
}

static inline unsigned long calc_string_hash (const char *str)
{
  unsigned long i = 0; 
  for ( ; *str; ++str)
    i = 5 * i + *str;
  return i;
}

static inline unsigned long calc_hash_value (const char *filename, int lineno)
{
  return calc_string_hash (filename) + lineno;
}

ATOM_AllocEntry *ATOM_MemLog::acquire_entry (ATOM_BlockEntry *block, const char *filename, int lineno)
{
	static int entry_flag = 0;

	++entry_flag;

	unsigned long bucket = calc_hash_value (filename, lineno) % num_buckets;
	EntryArray &array = m_buckets[bucket];
	ATOM_AllocEntry *freeEntry = 0;

	{
		ATOM_FastMutex::ScopeMutex l(m_lock);

		for (EntryArray::iterator i = array.begin(); i != array.end(); ++i)
		{
			ATOM_AllocEntry *iter = *i;

			if (!iter->counter)
			{
				freeEntry = iter;
				continue;
			}

			if (strcmp (iter->filename, filename))
			{
				continue;
			}

			if (iter->lineno != lineno)
			{
				continue;
			}

			iter->counter++;
			if (entry_flag == 1 && iter->counter >= iter->counterAlert)
			{
				ATOM_LOGGER::error ("*** Possible memory leak detected: id=%d <%s@%d> ***\nStackTrace:\n%s\n", iter->id, iter->filename, iter->lineno, ATOM_GetStackTraceString());
				iter->counterAlert += 100;
			}
			iter->total_size += block->size;

			block->next = iter->blocks;
			block->prev = 0;
			iter->blocks->prev = block;
			iter->blocks = block;

			--entry_flag;

			return iter;
		}

		if (!freeEntry)
		{
			freeEntry = new ATOM_AllocEntry;
			array.push_back (freeEntry);
		}

		freeEntry->counter = 1;
		freeEntry->counterAlert = ATOM_GetAllocCounterAlert ();
		freeEntry->total_size = block->size;

		static int allocid = 0;
		freeEntry->id = allocid++;

		freeEntry->filename = filename;
		freeEntry->lineno = lineno;

		block->next = 0;
		block->prev = 0;
		freeEntry->blocks = block;
	}

	--entry_flag;

	return freeEntry;
}

void ATOM_MemLog::release_entry (ATOM_BlockEntry *block, ATOM_AllocEntry *entry)
{
  ATOM_FastMutex::ScopeMutex l(m_lock);
  entry->total_size -= block->size;
  --entry->counter;

  if (entry->blocks == block)
  {
    entry->blocks = block->next;
    if (entry->blocks)
    {
      entry->blocks->prev = 0;
    }
  }
  else
  {
    block->prev->next = block->next;
    if (block->next)
    {
      block->next->prev = block->prev;
    }
  }

  //ATOM_ASSERT(entry->counter >= 0);
  ATOM_ASSERT(entry->counter || entry->total_size == 0);
  ATOM_ASSERT(!entry->counter || entry->total_size > 0);
}

static inline void remove_head_tail_spaces(std::string& str) {
  int n = 0;
  while ( n < int(str.length()) && (str[n] == ' ' || str[n] == '\t'))
    ++n;
  str.erase(0, n);

  if ( str.length() > 0)
  {
    n = int(str.length() - 1);
    while ( n >= 0 && (str[n] == ' ' || str[n] == '\t'))
      --n;
    str.erase(n + 1);
  }
}

static inline bool getline_from_stream (std::istream &in, std::string &out)
{
  static char buffer[2048];
  while (!in.eof())
  {
    in.getline (buffer, 2048);
    out = buffer;
    remove_head_tail_spaces (out);
    if (!out.empty())
    {
      return true;
    }
  }
  return false;
}

struct AllocEntry2
{
  int id;
  int size;
  int count;
};

static void load_memory_state (const char *filename, std::vector<AllocEntry2> &entryset)
{
  std::ifstream s(filename, std::ios_base::in);

  AllocEntry2 entry;
  bool first = true;

  if (s.good())
  {
    std::string line;

    for (;;)
    {
      if (!getline_from_stream (s, line))
      {
        break;
      }

      if (line.find ("==") != std::string::npos)
      {
        if (first)
        {
          first = false;
        }
        else
        {
          entryset.push_back(entry);
        }
      }

      if (line.find("allocid:") != std::string::npos)
      {
        sscanf (line.c_str(), "allocid: %d", &entry.id);
      }
      else if (line.find ("size:") != std::string::npos)
      {
        sscanf (line.c_str(), "size: %d", &entry.size);
      }
      else if (line.find ("count:") != std::string::npos)
      {
        sscanf (line.c_str(), "count: %d", &entry.count);
      }
    }
  }
}

struct CompareEntry
{
  int allocid;
  int size_delta;
  int count_delta;
};

bool operator < (const CompareEntry &ce1, const CompareEntry &ce2)
{
  return ce1.size_delta > ce2.size_delta;
}

bool ATOM_MemLog::dump_difference (const char *filename1, const char *filename2, const char *output_filename) const
{
  FILE *fp = output_filename ? fopen (output_filename, "wt") : stdout;

  if (fp)    
  {
    std::vector<AllocEntry2> entryset1, entryset2;
    std::vector<CompareEntry> compare_entries;

    load_memory_state (filename1, entryset1);
    load_memory_state (filename2, entryset2);

    int total_size1 = 0;
    int total_size2 = 0;

    std::vector<AllocEntry2>::const_iterator it;

    for (it = entryset1.begin(); it != entryset1.end(); ++it)
    {
      CompareEntry ce;
      ce.allocid = it->id;
      ce.size_delta = it->size;
      ce.size_delta *= -1;
      ce.count_delta = it->count;
      ce.count_delta *= -1;

      total_size1 += it->size;

      std::vector<AllocEntry2>::iterator it2;
      for (it2 = entryset2.begin(); it2 != entryset2.end(); ++it2)
      {
        if (it2->id == it->id)
        {
          break;
        }
      }

      if (it2 != entryset2.end ())
      {
        total_size2 += it2->size;

        ce.size_delta += it2->size;
        ce.count_delta += it2->count;
        entryset2.erase (it2);
      }

      compare_entries.push_back (ce);
    }

    for (it = entryset2.begin(); it != entryset2.end(); ++it)
    {
      CompareEntry ce;
      ce.allocid = it->id;
      ce.size_delta = it->size;
      ce.count_delta = it->count;

      total_size2 += it->size;

      compare_entries.push_back (ce);
    }

    std::sort (compare_entries.begin(), compare_entries.end(), std::less<CompareEntry>());

    fprintf (fp, "total size before = %d, total size after = %d, increment = %d\n\n", total_size1, total_size2, total_size2 - total_size1);

    for (unsigned i = 0; i < compare_entries.size(); ++i)
    {
      if (compare_entries[i].size_delta != 0)
      {
        fprintf (fp, "allocid=%d size_delta=%d count_delta=%d\n", compare_entries[i].allocid, compare_entries[i].size_delta, compare_entries[i].count_delta);
      }
    }

    if (fp != stdout)
    {
      fclose (fp);
    }

    return true;
  }

  return false;
}

static bool operator < (const ATOM_AllocEntry &e1, const ATOM_AllocEntry &e2)
{
  return e1.total_size > e2.total_size;
}

struct BackTraceEntry
{
  unsigned size;
  ATOM_ULongLong count;
  unsigned num_frames;
  char *offset[ATOM_MAX_BACKTRACE_DEPTH];
};

inline bool operator < (const BackTraceEntry &b1, const BackTraceEntry &b2)
{
  if (b1.num_frames < b2.num_frames)
  {
    return true;
  }
  else if (b1.num_frames > b2.num_frames)
  {
    return false;
  }

  for (unsigned i = 0; i < b1.num_frames; ++i)
  {
    if (b1.offset[i] < b2.offset[i])
    {
      return true;
    }
    else if (b1.offset[i] > b2.offset[i])
    {
      return false;
    }
  }

  return false;
}

struct DumpEntry
{
  int allocid;
  const char *filename;
  int lineno;
  int total_size;
  ATOM_ULongLong counter;
  std::vector<BackTraceEntry> backtraces;
};

static bool operator < (const DumpEntry &e1, const DumpEntry &e2)
{
  return e1.total_size > e2.total_size;
}

static const char *sep = "====================================================";
static const char *sep2 = "--------------------------------------------";

bool ATOM_MemLog::dump (const char *filename) const
{
  FILE *fp = filename ? fopen (filename, "wt") : stdout;

  std::vector<ATOM_AllocEntry> entries;

  if (fp)
  {
    {
      ATOM_FastMutex::ScopeMutex l(m_lock);

      for (int i = 0; i < num_buckets; ++i)
      {
        for (int j = 0; j < m_buckets[i].size(); ++j)
        {
          const ATOM_AllocEntry *pEntry = m_buckets[i][j];

          if (pEntry->counter > 0)
          {
            entries.push_back (*pEntry);
          }
        }
      }
    }

    std::sort (entries.begin(), entries.end());

    ATOM_ULongLong total_bytes = 0;

    for (int i = 0; i < entries.size(); ++i)
    {
      fprintf (fp, "%s\nallocid:   %d\nfilename:  %s\nline:      %d\nsize:      %d\ncount:     %d\n",
        sep,
        entries[i].id,
        entries[i].filename,
        entries[i].lineno,
        entries[i].total_size,
        entries[i].counter);

      total_bytes += entries[i].total_size;
    }

    fprintf (fp, "%s\n", sep);

    fprintf (fp, "Total bytes allocated: %I64d\n", total_bytes);

    fclose (fp);

    return true;
  }

  return false;
}

static bool extractBacktraces (std::vector<BackTraceEntry> &backtraces, ATOM_BlockEntry *b)
{
  try
  {
    while (b)
    {
      ATOM_BlockEntry *tmp = b->next;
      try
      {
        BackTraceEntry bte;
        bte.num_frames = b->num_frames;
        bte.size = b->size;
        for (unsigned i = 0; i < b->num_frames; ++i)
        {
          bte.offset[i] = (char*)b->backtrace[i];
        }
        backtraces.push_back (bte);
      }
      catch (...)
      {
      }

      b = tmp;
    }
    std::sort (backtraces.begin(), backtraces.end());

    std::vector<BackTraceEntry> backtraces2;
    for (unsigned i = 0; i < backtraces.size(); ++i)
    {
      if (backtraces2.empty())
      {
        backtraces2.push_back (backtraces[i]);
        backtraces2.back().count = 1;
      }
      else
      {
        BackTraceEntry *it = &backtraces2.back();
        bool identical = false;
        if (it->num_frames == backtraces[i].num_frames)
        {
          identical = true;
          for (unsigned n = 0; n < it->num_frames; ++n)
          {
            if (it->offset[n] != backtraces[i].offset[n])
            {
              identical = false;
              break;
            }
          }
		}

        if (identical)
        {
          it->size += backtraces[i].size;
          it->count++;
        }
        else
        {
          backtraces2.push_back (backtraces[i]);
          backtraces2.back().count = 1;
        }
      }
    }
    backtraces.swap (backtraces2);
  }
  catch(...)
  {
    return false;
  }

  return true;
}

static char block_buffer[16384];

bool ATOM_MemLog::dump_block (const char *filename, void *block, bool userblock) const
{
	if (!dump_block_to_memory (block_buffer, block, userblock))
	{
		return false;
	}

	FILE *fp = filename ? fopen (filename, "wt") : stdout;
	if (!fp)
	{
		return false;
	}

	unsigned size = strlen(block_buffer);
	fwrite (block_buffer, 1, size, fp);
	fclose (fp);

	return true;
}

bool ATOM_MemLog::dump_block_to_memory (char *buffer, void *block, bool userblock) const
{
	if (!block)
	{
		return false;
	}

	ATOM_BlockPrefix *prefix = (ATOM_BlockPrefix *)block;
	if (userblock)
	{
		prefix--;
	}

	if (prefix->mark != ATOM_BLOCK_MARK)
	{
		return false;
	}

    m_lock.lock ();
	ATOM_AllocEntry *entry = prefix->entry;
	std::vector<BackTraceEntry> backtraces;
	if (!extractBacktraces (backtraces, entry->blocks))
	{
		m_lock.unlock ();
		return false;
	}

	char tmp[2048];

	sprintf (tmp, "%s\r\n", sep);
	strcpy (buffer, tmp);

    sprintf (tmp, "allocid:    %d\r\n", entry->id);
	strcat (buffer, tmp);

    sprintf (tmp, "filename:   %s\r\n", entry->filename);
	strcat (buffer, tmp);

    sprintf (tmp, "line:       %d\r\n", entry->lineno);
	strcat (buffer, tmp);

    strcat (buffer, "backtraces:\r\n");

	if (backtraces.size() > 0)
	{
		ATOM_CallStack callstack(0, ATOM_CallStack::SYM_BUILDPATH);
		ATOM_RadixSort radixsort;
		radixsort.sort(&backtraces[0].size, backtraces.size(), false, sizeof(backtraces[0]));
		unsigned *indices = radixsort.getIndices();

		for (int j = 0; j < backtraces.size(); ++j)
		{
		  sprintf (tmp, "%s\r\n", sep2);
		  strcat (buffer, tmp);

		  unsigned idx = indices[backtraces.size() - j - 1];

		  for (int k = 0; k < backtraces[idx].num_frames; ++k)
		  {
			ATOM_CallStack::StackEntry e;
			e.offset = backtraces[idx].offset[k];
			callstack.resolveSymbols (e);

			sprintf (tmp, "    !%s(%s@%d)\r\n", 
			  e.und_name,
			  e.line_filename,
			  e.line_number);
			strcat (buffer, tmp);
		  }

		  sprintf (tmp, "    (allocate size:  %u)\r\n", backtraces[idx].size);
		  strcat (buffer, tmp);

		  sprintf (tmp, "    (allocate count: %I64d)\r\n", backtraces[idx].count);
		  strcat (buffer, tmp);
		}
	}

	m_lock.unlock ();
	return true;
}

struct DumpContext
{
	std::string filename;
	std::vector<DumpEntry> entries;
};

static long volatile lastDumpHandle = 0L;

unsigned __stdcall fullDumpThreadFunc (void *param)
{
	DumpContext *dc = (DumpContext*)param;

	for (;;)
	{
		long cur = ::InterlockedCompareExchange (&lastDumpHandle, 1L, 0L);
		if (cur == 0L)
		{
			break;
		}
		::Sleep (100);
	}

	FILE *fp = fopen (dc->filename.c_str(), "wt");
	if (!fp)
	{
		delete dc;
		return 1;
	}

	std::vector<DumpEntry> &entries = dc->entries;

	ATOM_CallStack callstack(0, ATOM_CallStack::SYM_BUILDPATH);

	for (int i = 0; i < entries.size(); ++i)
	{
		std::vector<BackTraceEntry> &backtraces = entries[i].backtraces;
		std::sort (backtraces.begin(), backtraces.end());

		std::vector<BackTraceEntry> backtraces2;
		for (unsigned i = 0; i < backtraces.size(); ++i)
		{
			if (backtraces2.empty())
			{
				backtraces2.push_back (backtraces[i]);
				backtraces2.back().count = 1;
			}
			else
			{
				BackTraceEntry *it = &backtraces2.back();
				bool identical = false;
				if (it->num_frames == backtraces[i].num_frames)
				{
					identical = true;
					for (unsigned n = 0; n < it->num_frames; ++n)
					{
						if (it->offset[n] != backtraces[i].offset[n])
						{
							identical = false;
							break;
						}
					}
				}

				if (identical)
				{
					it->size += backtraces[i].size;
					it->count++;
				}
				else
				{
					backtraces2.push_back (backtraces[i]);
					backtraces2.back().count = 1;
				}
			}
		}
		backtraces.swap (backtraces2);
	}

	std::sort (entries.begin(), entries.end());

	ATOM_ULongLong total_bytes = 0;

	for (int i = 0; i < entries.size(); ++i)
	{
		if (entries[i].counter == 0)
		{
			continue;
		}

		fprintf (fp, "%s\n", sep);
		fprintf (fp, "allocid:    %d\n", entries[i].allocid);
		fprintf (fp, "filename:   %s\n", entries[i].filename);
		fprintf (fp, "line:       %d\n", entries[i].lineno);
		fprintf (fp, "totalsize:  %u\n", entries[i].total_size);
		fprintf (fp, "count:      %u\n", entries[i].counter);

		fprintf (fp, "backtraces:\n");

		if (entries[i].backtraces.size() > 0)
		{
			ATOM_RadixSort radixsort;
			radixsort.sort(&entries[i].backtraces[0].size, entries[i].backtraces.size(), false, sizeof(entries[i].backtraces[0]));
			unsigned *indices = radixsort.getIndices();

			for (int j = 0; j < entries[i].backtraces.size(); ++j)
			{
				fprintf (fp, "%s\n", sep2);

				unsigned idx = indices[entries[i].backtraces.size() - j - 1];

				for (int k = 0; k < entries[i].backtraces[idx].num_frames; ++k)
				{
					ATOM_CallStack::StackEntry e;
					e.offset = entries[i].backtraces[idx].offset[k];
					callstack.resolveSymbols (e);

					fprintf (fp, "    !%s(%s@%d)\n", 
						e.und_name,
						e.line_filename,
						e.line_number);
				}
				fprintf (fp, "    (allocate size:  %u)\n", entries[i].backtraces[idx].size);
				fprintf (fp, "    (allocate count: %I64d)\n", entries[i].backtraces[idx].count);
			}

			total_bytes += entries[i].total_size;
		}
	}

	fprintf (fp, "%s\n", sep);
	fprintf (fp, "Total bytes allocated: %I64d\n", total_bytes);
	fclose (fp);

	delete dc;
	::InterlockedExchange (&lastDumpHandle, 0L);

	return 0;
}

bool ATOM_MemLog::full_dump (const char *filename) const
{
	DumpContext *dc = new DumpContext;
	std::vector<DumpEntry> &entries = dc->entries;
	dc->filename = filename;

    m_lock.lock ();

    int i;

    for (i = 0; i < num_buckets; ++i)
    {
	    for (int j = 0; j < m_buckets[i].size(); ++j)
		{
			DumpEntry de;
			de.allocid = m_buckets[i][j]->id;
			de.counter = m_buckets[i][j]->counter;
			de.total_size = m_buckets[i][j]->total_size;
			de.filename = m_buckets[i][j]->filename;
			de.lineno = m_buckets[i][j]->lineno;
			entries.push_back (de);
			std::vector<BackTraceEntry> &backtraces = entries.back().backtraces;

			try
			{
				ATOM_BlockEntry *b = m_buckets[i][j]->blocks;
				while (b)
				{
		            ATOM_BlockEntry *tmp = b->next;

					try
					{
						BackTraceEntry bte;
						bte.num_frames = b->num_frames;
						bte.size = b->size;
						for (unsigned i = 0; i < b->num_frames; ++i)
						{
							bte.offset[i] = (char*)b->backtrace[i];
						}
						backtraces.push_back (bte);
					}
					catch (...)
					{
					}

					b = tmp;
				}
			}
			catch(...)
			{
				//fprintf (fp, "** error: exception occured while extracting backtraces for allocation %d\n", m_buckets[i][j]->id);
			}
		}
	}

    m_lock.unlock ();

	unsigned threadid;
	return _beginthreadex (NULL, 0, &fullDumpThreadFunc, dc, 0, &threadid) != NULL;
}

extern char lastMemErrorFileName[512];

void ATOM_MemLog::record_error (const char *filename, int lineno, const char *errmsg, bool fatal, int num_frames, void **frames)
{
  if (!m_fperr)
  {
    SYSTEMTIME time;
    ::GetSystemTime (&time);
    
    ModuleName modulename;

    sprintf (lastMemErrorFileName, "%s-memory-errors-%d-%d-%d-%d-%d-%d.txt", modulename._modulename, time.wYear, time.wMonth, 
      time.wDay, time.wHour, time.wMinute, time.wSecond);

    m_fperr = fopen (lastMemErrorFileName, "wt");
    if (m_fperr)
    {
      setvbuf (m_fperr, 0, _IONBF, 0);
    }
  }

  if (m_fperr)
  {
    ATOM_FastMutex::ScopeMutex l(m_errlock);

	ATOM_VERIFY(0, NULL)(errmsg);

    std::pair<std::multimap<const char*, int>::iterator, std::multimap<const char*, int>::iterator> range =
      m_errors.equal_range (filename);

    for (std::multimap<const char*, int>::iterator it = range.first; it != range.second; ++it)
    {
      if (it->second == lineno)
      {
        // already recorded.
        return;
      }
    }

    m_errors.insert(std::pair<const char*, int>(filename, lineno));

    fprintf (m_fperr, "**%s: %s at %s(%d)\n", fatal ? "Fatal" : "Warning", errmsg, filename, lineno);

	if (num_frames > 0)
	{
		unsigned options = ATOM_CallStack::SYM_BUILDPATH;
		if (UseSymServer)
		{
			options |= ATOM_CallStack::SYM_USESYMSRV;
		}
		ATOM_CallStack callstack(0, options);

		fprintf (m_fperr, "Callstack:\r\n");
		for (int i = 0; i < num_frames; ++i)
		{
			ATOM_CallStack::StackEntry e;
			e.offset = frames[i];
			callstack.resolveSymbols (e);
			fprintf (m_fperr, "\t%s(%s@%d)\r\n", e.und_name, e.line_filename, e.line_number);
		}
	}
  }
}

extern int testGuard (ATOM_BlockGuard *guard);

bool ATOM_MemLog::check_bounds (void (ATOM_CALL *on_check_failed)(void*, ATOM_BlockEntry*, const char*, int), void *userdata) const
{
  bool ret = true;

  m_lock.lock ();

  for (unsigned n = 0; ret && n < num_buckets; ++n)
  {
    const EntryArray &array = m_buckets[n];

    for (unsigned i = 0; ret && i < array.size(); ++i)
    {
      ATOM_BlockEntry *b = array[i]->blocks;

      while (b)
      {
        ATOM_BlockEntry *tmp = b->next;
        ATOM_BlockGuard *guard = (ATOM_BlockGuard*)((char*)b->block + b->size);
        bool result;

        try
        {
          result = testGuard (guard);
        }
        catch(...)
        {
          result = false;
        }

        if (!result && on_check_failed)
        {
          try
          {
            on_check_failed(userdata, b, array[i]->filename, array[i]->lineno);
          }
          catch(...)
          {
          }

          ret = false;
        }

        if (!result)
        {
          ret = false;
          break;
        }

        b = tmp;
      }
    }
  }

  m_lock.unlock ();

  return ret;
}

