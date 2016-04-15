#ifndef __ATOM_UTILS_RADIXSORT_H
#define __ATOM_UTILS_RADIXSORT_H

#if _MSC_VER > 1000
# pragma once
#endif

#define ATOM_RADIXSORT_CHECK_RESIZE(n)																			\
  if(n!=mPreviousSize)																		\
  {																							\
		    if(n>mCurrentSize)	resize(n);													\
    else						resetIndices();												\
    mPreviousSize = n;																		\
  }

#define ATOM_RADIXSORT_GET_ELEMENT(type, buffer, index, stride) \
  (*(type*)(((const unsigned char*)(buffer)) + (index)*(stride)))

#define ATOM_RADIXSORT_CREATE_HISTOGRAMS(type, buffer, stride)															\
  /* Clear counters */																		\
  memset(mHistogram, 0, 256*4*sizeof(unsigned));												\
																						    \
  /* Prepare for temporal coherence */														\
  type PrevVal = ATOM_RADIXSORT_GET_ELEMENT(type, buffer, mIndices[0], stride);													\
  bool AlreadySorted = true;	/* Optimism... */												\
  unsigned* Indices = mIndices;																	\
																						    \
  /* Prepare to count */																		\
  unsigned char* p = (unsigned char*)input;																	\
  unsigned char* pe = &p[nb*stride];																		\
  unsigned* h0= &mHistogram[0];		/* Histogram for first pass (LSB)	*/						\
  unsigned* h1= &mHistogram[256];	/* Histogram for second pass		*/						\
  unsigned* h2= &mHistogram[512];	/* Histogram for third pass			*/						\
  unsigned* h3= &mHistogram[768];	/* Histogram for last pass (MSB)	*/						\
																						    \
  while(p!=pe)																				\
  {																							\
    /* Read input buffer in previous sorted order */										\
    type Val = ATOM_RADIXSORT_GET_ELEMENT(type, buffer, (*Indices++), stride);													\
    /* Check whether already sorted or not */												\
    if(Val<PrevVal)	{ AlreadySorted = false; break; } /* Early out */						\
    /* Update for next iteration */															\
    PrevVal = Val;																			\
																						    \
    /* Create histograms */																	\
    h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;											\
    p += (stride - 4);            \
  }																							\
																						    \
  /* If all input values are already sorted, we just have to return and leave the */			\
  /* previous list unchanged. That way the routine may take advantage of temporal */			\
  /* coherence, for example when used to sort transparent faces.					*/			\
  if(AlreadySorted)	{ return *this;	}											\
																						    \
  /* Else there has been an early out and we must finish computing the histograms */			\
  while(p!=pe)																				\
  {																							\
    /* Create histograms without the previous overhead */									\
    h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;											\
    p += (stride - 4);        \
  }

#define ATOM_RADIXSORT_CHECK_PASS_VALIDITY(pass)																\
  /* Shortcut to current counters */															\
  unsigned* CurCount = &mHistogram[pass<<8];													\
																						    \
  /* Reset flag. The sorting pass is supposed to be performed. (default) */					\
  bool PerformPass = true;																	\
																						    \
  /* Check pass validity */																	\
																						    \
  /* If all values have the same byte, sorting is useless. */									\
  /* It may happen when sorting bytes or words instead of dwords. */							\
  /* This routine actually sorts words faster than dwords, and bytes */						\
  /* faster than words. Standard running time (O(4*n))is reduced to O(2*n) */					\
  /* for words and O(n) for bytes. Running time for floats depends on actual values... */		\
																						    \
  /* Get first byte */																		\
  unsigned char UniqueVal = *(((unsigned char*)input)+pass);													\
																						    \
  /* Check that byte's counter */																\
  if(CurCount[UniqueVal]==nb)	PerformPass=false;

class ATOM_RadixSort
{
public:
  ATOM_RadixSort (void)
  {
    mIndices = 0;
    mIndices2 = 0;
    mCurrentSize = 0;
    mPreviousSize = 0;

      resetIndices();
  }

  ~ATOM_RadixSort (void)
  {
    delete [] mIndices2;
    delete [] mIndices;
  }

private:
  ATOM_RadixSort (const ATOM_RadixSort&);
  ATOM_RadixSort& operator = (const ATOM_RadixSort&);

public:
  ATOM_RadixSort & sort (const unsigned *input, unsigned nb, bool signedvalues = true, unsigned stride = 0)
  {
    if (stride == 0)
    {
      stride = 4;
    }

      // Checkings
      if(!input || !nb)	return *this;

      // Resize lists if needed
      ATOM_RADIXSORT_CHECK_RESIZE(nb);

      unsigned mHistogram[256*4];
      unsigned mOffset[256];
    unsigned i,j;

      // Create histograms (counters). Counters for all passes are created in one run.
      // Pros:	read input buffer once instead of four times
      // Cons:	mHistogram is 4Kb instead of 1Kb
      // We must take care of signed/unsigned values for temporal coherence.... I just
      // have 2 code paths even if just a single opcode changes. Self-modifying code, someone?
      if(!signedvalues)	{ ATOM_RADIXSORT_CREATE_HISTOGRAMS(unsigned, input, stride);	}
      else				{ ATOM_RADIXSORT_CREATE_HISTOGRAMS(int, input, stride);	}

      // Compute #negative values involved if needed
      unsigned NbNegativeValues = 0;
      if(signedvalues)
      {
	      // An efficient way to compute the number of negatives values we'll have to deal with is simply to sum the 128
	      // last values of the last histogram. Last histogram because that's the one for the Most Significant Byte,
	      // responsible for the sign. 128 last values because the 128 first ones are related to positive numbers.
	      unsigned* h3= &mHistogram[768];
	      for(i=128;i<256;i++)	NbNegativeValues += h3[i];	// 768 for last histogram, 128 for negative part
      }

      // Radix sort, j is the pass number (0=LSB, 3=MSB)
      for(j=0;j<4;j++)
      {
	      ATOM_RADIXSORT_CHECK_PASS_VALIDITY(j);

	      // Sometimes the fourth (negative) pass is skipped because all numbers are negative and the MSB is 0xFF (for example). This is
	      // not a problem, numbers are correctly sorted anyway.
	      if(PerformPass)
	      {
		      // Should we care about negative values?
		      if(j!=3 || !signedvalues)
		      {
			      // Here we deal with positive values only

			      // Create offsets
			      mOffset[0] = 0;
			      for(i=1;i<256;i++)		mOffset[i] = mOffset[i-1] + CurCount[i-1];
		      }
		      else
		      {
			      // This is a special case to correctly handle negative integers. They're sorted in the right order but at the wrong place.

			      // Create biased offsets, in order for negative numbers to be sorted as well
			      mOffset[0] = NbNegativeValues;												// First positive number takes place after the negative ones
			      for(i=1;i<128;i++)		mOffset[i] = mOffset[i-1] + CurCount[i-1];	// 1 to 128 for positive numbers

			      // Fixing the wrong place for negative values
			      mOffset[128] = 0;
			      for(i=129;i<256;i++)			mOffset[i] = mOffset[i-1] + CurCount[i-1];
		      }

		      // Perform Radix Sort
		      unsigned char* InputBytes	= (unsigned char*)input;
		      unsigned* Indices		= mIndices;
		      unsigned* IndicesEnd	= &mIndices[nb];
		      InputBytes += j;
		      while(Indices!=IndicesEnd)
		      {
			      unsigned id = *Indices++;
			      mIndices2[mOffset[InputBytes[id * stride]]++] = id;
		      }

		      // Swap pointers for next pass. Valid indices - the most recent ones - are in mIndices after the swap.
		      unsigned* Tmp	= mIndices;	mIndices = mIndices2; mIndices2 = Tmp;
	      }
      }
      return *this;
  }

  ATOM_RadixSort & sort (const float *input2, unsigned nb, unsigned stride = 0)
  {
    if (stride == 0)
    {
      stride = 4;
    }

      // Checkings
      if(!input2 || !nb)	return *this;

      unsigned* input = (unsigned*)input2;

      // Resize lists if needed
      ATOM_RADIXSORT_CHECK_RESIZE(nb);

      unsigned mHistogram[256*4];
      unsigned mOffset[256];
    unsigned i,j;

      // Create histograms (counters). Counters for all passes are created in one run.
      // Pros:	read input buffer once instead of four times
      // Cons:	mHistogram is 4Kb instead of 1Kb
      // Floating-point values are always supposed to be signed values, so there's only one code path there.
      // Please note the floating point comparison needed for temporal coherence! Although the resulting asm code
      // is dreadful, this is surprisingly not such a performance hit - well, I suppose that's a big one on first
      // generation Pentiums....We can't make comparison on integer representations because, as Chris said, it just
      // wouldn't work with mixed positive/negative values....
      { ATOM_RADIXSORT_CREATE_HISTOGRAMS(float, input2, stride); }

      // Compute #negative values involved if needed
      unsigned NbNegativeValues = 0;
      // An efficient way to compute the number of negatives values we'll have to deal with is simply to sum the 128
      // last values of the last histogram. Last histogram because that's the one for the Most Significant Byte,
      // responsible for the sign. 128 last values because the 128 first ones are related to positive numbers.
      unsigned* h3= &mHistogram[768];
      for(i=128;i<256;i++)	NbNegativeValues += h3[i];	// 768 for last histogram, 128 for negative part

      // Radix sort, j is the pass number (0=LSB, 3=MSB)
      for(j=0;j<4;j++)
      {
	      // Should we care about negative values?
	      if(j!=3)
	      {
		      // Here we deal with positive values only
		      ATOM_RADIXSORT_CHECK_PASS_VALIDITY(j);

		      if(PerformPass)
		      {
			      // Create offsets
			      mOffset[0] = 0;
			      for(i=1;i<256;i++)		mOffset[i] = mOffset[i-1] + CurCount[i-1];

			      // Perform Radix Sort
			      unsigned char* InputBytes	= (unsigned char*)input;
			      unsigned* Indices		= mIndices;
			      unsigned* IndicesEnd	= &mIndices[nb];
			      InputBytes += j;
			      while(Indices!=IndicesEnd)
			      {
				      unsigned id = *Indices++;
				      mIndices2[mOffset[InputBytes[id*stride]]++] = id;
			      }

			      // Swap pointers for next pass. Valid indices - the most recent ones - are in mIndices after the swap.
			      unsigned* Tmp	= mIndices;	mIndices = mIndices2; mIndices2 = Tmp;
		      }
	      }
	      else
	      {
		      // This is a special case to correctly handle negative values
		      ATOM_RADIXSORT_CHECK_PASS_VALIDITY(j);

		      if(PerformPass)
		      {
			      // Create biased offsets, in order for negative numbers to be sorted as well
			      mOffset[0] = NbNegativeValues;												// First positive number takes place after the negative ones
			      for(i=1;i<128;i++)		mOffset[i] = mOffset[i-1] + CurCount[i-1];	// 1 to 128 for positive numbers

			      // We must reverse the sorting order for negative numbers!
			      mOffset[255] = 0;
			      for(i=0;i<127;i++)		mOffset[254-i] = mOffset[255-i] + CurCount[255-i];	// Fixing the wrong order for negative values
			      for(i=128;i<256;i++)	mOffset[i] += CurCount[i];							// Fixing the wrong place for negative values

			      // Perform Radix Sort
			      for(i=0;i<nb;i++)
			      {
            unsigned Radix = (ATOM_RADIXSORT_GET_ELEMENT(unsigned, input, mIndices[i], stride)) >> 24;
				      //unsigned Radix = input[mIndices[i]]>>24;								// Radix byte, same as above. AND is useless here (unsigned).
				      // ### cmp to be killed. Not good. Later.
				      if(Radix<128)		mIndices2[mOffset[Radix]++] = mIndices[i];		// Number is positive, same as above
				      else				mIndices2[--mOffset[Radix]] = mIndices[i];		// Number is negative, flip the sorting order
			      }
			      // Swap pointers for next pass. Valid indices - the most recent ones - are in mIndices after the swap.
			      unsigned* Tmp	= mIndices;	mIndices = mIndices2; mIndices2 = Tmp;
		      }
		      else
		      {
			      // The pass is useless, yet we still have to reverse the order of current list if all values are negative.
			      if(UniqueVal>=128)
			      {
				      for(i=0;i<nb;i++)	mIndices2[i] = mIndices[nb-i-1];

				      // Swap pointers for next pass. Valid indices - the most recent ones - are in mIndices after the swap.
				      unsigned* Tmp	= mIndices;	mIndices = mIndices2; mIndices2 = Tmp;
			      }
		      }
	      }
      }
      return *this;
  }

  unsigned * getIndices (void) const
  {
    return mIndices;
  }

private:
  void resize (unsigned nb)
  {
      delete [] mIndices2;
      delete [] mIndices;

      mIndices = new unsigned[nb];
      mIndices2 = new unsigned[nb];
      mCurrentSize = nb;

      resetIndices();
  }

  void resetIndices (void)
  {
      for(unsigned i=0;i<mCurrentSize;i++)	mIndices[i] = i;
  }

private:
  unsigned mCurrentSize;
  unsigned mPreviousSize;
  unsigned *mIndices;
  unsigned *mIndices2;
};

#endif // __ATOM_ENGINE_RADIXSORT_H
