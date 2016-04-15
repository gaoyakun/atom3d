#ifdef SAVE_RCSID
static char rcsid = "@(#) $Id: radix.h 1736 2008-12-15 07:59:49Z gyk $";
#endif

#ifndef __ATOM_UTILS_RADIX_H
#define __ATOM_UTILS_RADIX_H

#include <cassert>
#include "staticassertion.h"
#include "typetraits.h"

template<class T, class K = T> 
class ATOM_RadixSort
{
public:
  struct identity
  {
    ATOM_STATIC_ASSERT((ATOM_IsSame<T, K>::result));
    inline unsigned & operator () (T & value) const { return (unsigned&)value; }
  };

public:
    ATOM_RadixSort();
  template <class Pred> void Sort(T * pArray, unsigned uCount, Pred) const;
  template <class Pred> bool Sort(T * pArray1, T *pArray2, unsigned uCount, Pred) const;
    void EnableCounterCheck(bool bEnable = true);

private:
  template <class Pred>
    bool InternalExecute( T * pArray1, T * pArray2, bool bTargetArray1, unsigned uCount, 
		    						      unsigned uStartBit, unsigned uRadixWidth, unsigned * pCounters, 
                        unsigned * pOffset, Pred pred) const;
  template <class Pred>
    bool InternalExecute2Complement(T * pArray1, T * pArray2, bool bTargetArray1, unsigned uCount, 
    													    unsigned uStartBit, unsigned uRadixWidth, unsigned * pCounters, 
                                  unsigned * pOffset, Pred pred) const;

  template <class Pred>
    bool InternalExecuteFloatSign ( T * pArray1, T * pArray2, bool bTargetArray1, unsigned uCount, 
				        							      unsigned uStartBit, unsigned * pCounters, unsigned * pOffset, 
                                  Pred pred) const;
  template <class Pred>
    bool InternalExecuteFirst3Bytes(T * pArray1, T * pArray2, unsigned uCount, 
		        										    unsigned * pCounters, unsigned * pOffset, 
                                  Pred pred) const;
  template <class Pred>
    bool PrepareCounters (T * pSourceArray, unsigned uCount, unsigned uStartBit,unsigned uRadixWidth, 
                        unsigned * pCounters, Pred pred) const;
  template <class Pred>
    void RadixSortCore (T * pSourceArray, T * pTargetArray, unsigned uCount, unsigned uStartBit, 
                      unsigned uRadixWidth, unsigned * pCounters, unsigned * pOffset, Pred pred) const;
  template <class Pred>
    void RadixSortCoreFloatSign(T * pSourceArray, T * pTargetArray, unsigned uCount, unsigned uStartBit, 
                              unsigned * pCounters, unsigned * pOffset, Pred pred) const;
    void CopyArray(T * pSourceArray, T * pTargetArray, unsigned uCount) const;
    unsigned CalcNegativeValueCount(unsigned * pCounters, unsigned uRadixWidth) const;

private:
    bool	_M_counter_check;
};


template<class T, class K> 
ATOM_RadixSort<T, K>::ATOM_RadixSort()
  : _M_counter_check(true) {
}


template<class T, class K>
void ATOM_RadixSort<T, K>::EnableCounterCheck(bool bEnable) {
    _M_counter_check = bEnable;
}


template<class T, class K> 
template<class Pred>
bool ATOM_RadixSort<T, K>::PrepareCounters(T * pSourceArray, unsigned uCount, unsigned uStartBit, unsigned uRadixWidth,
						                         unsigned * pCounters, Pred pred) const {
    assert(pSourceArray);
    unsigned uRadixWidthMask = uRadixWidth - 1U;
    assert(pCounters != NULL);
    memset(pCounters, 0, sizeof(unsigned) * uRadixWidth);

    unsigned	u = 0U;
    for(; u<uCount; ++u) {
    ++pCounters[(pred(pSourceArray[u]) >> uStartBit) & uRadixWidthMask];
    }

    if (_M_counter_check)
	    for(u=0U; u<uRadixWidth; ++u)	
    {
		    if (pCounters[u] >= uCount)
			    return false;
	    }

    return true;
}


template<class T, class K> 
unsigned ATOM_RadixSort<T, K>::CalcNegativeValueCount(unsigned * pCounters, unsigned uRadixWidth) const {
    unsigned uNegativeCount = 0U;
    for(unsigned u = uRadixWidth >> 1U; u<uRadixWidth; ++u)
	    uNegativeCount += pCounters[u];
    return uNegativeCount;
}

template<class T, class K> 
template<class Pred>
void ATOM_RadixSort<T, K>::RadixSortCore(T * pSourceArray, T * pTargetArray, unsigned uCount, unsigned uStartBit, 
                                 unsigned uRadixWidth, unsigned * pCounters, unsigned * pOffset, 
                                 Pred pred) const {
    unsigned uRadixWidthMask = uRadixWidth - 1U;
    assert(pSourceArray);
    assert(pTargetArray);
    for(unsigned u=0U; u<uCount; ++u)	
  {
	    T & rSource = pSourceArray[u];
    pTargetArray[pOffset[(pred(rSource) >> uStartBit) & uRadixWidthMask]++] = rSource;
    }	
}

template<class T, class K> 
template<class Pred>
void ATOM_RadixSort<T, K>::RadixSortCoreFloatSign(T * pSourceArray, T * pTargetArray, unsigned uCount, 
								                            unsigned uStartBit, unsigned * pCounters, unsigned * pOffset, 
                                          Pred pred) const {
    unsigned uRadixShift			= 31U - uStartBit;
    unsigned uRadix				= 0U;
    assert(pSourceArray);
    assert(pTargetArray);
    for(unsigned u=0U; u<uCount; ++u)	
  {
	    T & rSource = pSourceArray[u];
    uRadix =  pred(rSource) >> uStartBit;	// No masking required...
	    unsigned & rOffset = pOffset[uRadix];
	    pTargetArray[rOffset] = rSource;
	    rOffset += 1U - ((uRadix >> uRadixShift) << 1U);
    }
}

template<class T, class K> 
template<class Pred>
bool ATOM_RadixSort<T, K>::InternalExecute(T * pArray1, T * pArray2, bool bTargetArray1, unsigned uCount, 
						                         unsigned uStartBit, unsigned uRadixWidth,unsigned * pCounters, 
                                   unsigned * pOffset, Pred pred) const {
    if (bTargetArray1)	
  {
	    if (!PrepareCounters(pArray2, uCount, uStartBit, uRadixWidth, pCounters, pred))
		    return false;
    }
    else 
  {
	    if (!PrepareCounters(pArray1, uCount, uStartBit, uRadixWidth, pCounters, pred))
		    return false;
    }

    assert(pOffset);
    pOffset[0U] = 0U;
    for(unsigned u=1U; u<uRadixWidth; ++u)
	    pOffset[u] = pOffset[u-1U] + pCounters[u-1U];

    if (bTargetArray1)
	    RadixSortCore(pArray2, pArray1, uCount, uStartBit, uRadixWidth, pCounters, pOffset, pred);
    else
	    RadixSortCore(pArray1, pArray2, uCount, uStartBit, uRadixWidth, pCounters, pOffset, pred);

    return true;
}

template<class T, class K> 
template<class Pred>
bool ATOM_RadixSort<T, K>::InternalExecute2Complement( T * pArray1, T * pArray2, bool bTargetArray1, unsigned uCount, 
										                             unsigned uStartBit, unsigned uRadixWidth, unsigned * pCounters, 
                                               unsigned * pOffset, Pred pred) const {
    if (bTargetArray1)	
  {
	    if (!PrepareCounters(pArray2, uCount, uStartBit, uRadixWidth, pCounters, pred))
		    return false;
    }
    else 
  {
	    if (!PrepareCounters(pArray1, uCount, uStartBit, uRadixWidth, pCounters, pred))
		    return false;
    }

    unsigned uNegativeCount = CalcNegativeValueCount(pCounters, uRadixWidth);
    assert(pOffset);
    unsigned uRadixWidthHalf = uRadixWidth >> 1U;
    pOffset[0U] = uNegativeCount;
    unsigned u = 1U;

    for(; u<uRadixWidthHalf; ++u)
	    pOffset[u] = pOffset[u-1U] + pCounters[u-1U];

    pOffset[u] = 0U;

    for(++u; u<uRadixWidth; ++u)
	    pOffset[u] = pOffset[u-1U] + pCounters[u-1U];

    if (bTargetArray1)
	    RadixSortCore(pArray2, pArray1, uCount, uStartBit, uRadixWidth, pCounters, pOffset, pred);
    else
	    RadixSortCore(pArray1, pArray2, uCount, uStartBit, uRadixWidth, pCounters, pOffset, pred);

    return true;
}

template<class T, class K> 
template<class Pred>
bool ATOM_RadixSort<T, K>::InternalExecuteFloatSign( T * pArray1, T * pArray2, bool bTargetArray1, unsigned uCount, 
									                             unsigned uStartBit, unsigned * pCounters, unsigned * pOffset, 
                                             Pred pred) const {
    unsigned uRadixWidth = 1U << (32U - uStartBit);

    if (bTargetArray1)	
  {
	    if (!PrepareCounters(pArray2, uCount, uStartBit, uRadixWidth, pCounters, pred))
		    return false;
    }
    else 
  {
	    if (!PrepareCounters(pArray1, uCount, uStartBit, uRadixWidth, pCounters, pred))
		    return false;
    }

    unsigned uNegativeCount = CalcNegativeValueCount(pCounters, uRadixWidth);
    assert(pOffset);
    unsigned uRadixWidthHalf = uRadixWidth >> 1U;
    pOffset[0U] = uNegativeCount;
    unsigned u = 1U;

    for(; u<uRadixWidthHalf; ++u)
	    pOffset[u] = pOffset[u-1U] + pCounters[u-1U];

    unsigned uRadixWidthMask = uRadixWidth - 1U;
    pOffset[uRadixWidthMask] = 0U;

    for(u=0U; u<uRadixWidthHalf-1U; ++u)
	    pOffset[uRadixWidthMask-1U-u] = pOffset[uRadixWidthMask-u] + pCounters[uRadixWidthMask-u];

    for(u=uRadixWidthHalf; u<uRadixWidth; ++u)
	    pOffset[u] += pCounters[u] - 1U;	

    if (bTargetArray1)
	    RadixSortCoreFloatSign(pArray2, pArray1, uCount, uStartBit, pCounters, pOffset, pred);
    else
	    RadixSortCoreFloatSign(pArray1, pArray2, uCount, uStartBit, pCounters, pOffset, pred);

    return true;
}


template<class T, class K> 
template<class Pred>
bool ATOM_RadixSort<T, K>::InternalExecuteFirst3Bytes( T * pArray1, T * pArray2, unsigned uCount, 
										                             unsigned * pCounters, unsigned * pOffset, 
                                               Pred pred) const {
    bool bTargetArray1 = false;
  if (!ATOM_IsSame<K, char>::result)
    {
	    bTargetArray1 = InternalExecute(pArray1, pArray2, false, uCount, 0U, 256U, pCounters, pOffset, pred);
    if (ATOM_IsSame<K, unsigned char>::result)
		    return bTargetArray1;
    }
    else
	    return InternalExecute2Complement(pArray1, pArray2, false, uCount, 0U, 256U, pCounters, pOffset, pred);

  if (!ATOM_IsSame<K, short>::result)
    {
	    bTargetArray1 ^= InternalExecute(pArray1, pArray2, bTargetArray1, uCount, 8U, 256U, pCounters, pOffset, pred);
    if (ATOM_IsSame<K, unsigned short>::result)
		    return bTargetArray1;
    }
    else
	    return bTargetArray1 ^ InternalExecute2Complement(pArray1, pArray2, bTargetArray1, uCount, 8U, 256U, pCounters, 
                                                      pOffset, pred);

    return bTargetArray1 ^ InternalExecute(pArray1, pArray2, bTargetArray1, uCount, 16U, 256U, pCounters, pOffset, pred);
}

template<class T, class K>
void ATOM_RadixSort<T, K>::CopyArray(T * pSourceArray, T * pTargetArray, unsigned uCount) const
{
    assert(pSourceArray);
    assert(pTargetArray);

  if (IsPOD<T>::result)
    memcpy (pTargetArray, pSourceArray, sizeof(T) * uCount);
  else
      for(unsigned u=0U; u<uCount; ++u)
	      pTargetArray[u] = pSourceArray[u];
}

template<class T, class K>
template<class Pred>
bool ATOM_RadixSort<T, K>::Sort(T * pArray1, T * pArray2, unsigned uCount, Pred pred) const {
  ATOM_STATIC_ASSERT(sizeof(K) == 4);
  ATOM_STATIC_ASSERT(( ATOM_IsSame<K, int>::result || 
                  ATOM_IsSame<K, unsigned>::result || 
                  ATOM_IsSame<K, long>::result ||
                  ATOM_IsSame<K, unsigned long>::result ||
                  ATOM_IsSame<K, float>::result));
    if (uCount <= 1U)
	    return false;

    unsigned arrCounters[256U];
    unsigned arrOffset[256U];

    bool bTargetArray1 = InternalExecuteFirst3Bytes(pArray1, pArray2, uCount, arrCounters, arrOffset, pred);
  if (ATOM_IsSame<K, int>::result || ATOM_IsSame<K, long>::result)
	    return bTargetArray1 ^ InternalExecute2Complement(pArray1, pArray2, bTargetArray1, uCount,
												    24U, 256U, arrCounters, arrOffset, pred);			 
  else if (ATOM_IsSame<K, unsigned>::result || ATOM_IsSame<K, unsigned long>::result) 
	    return bTargetArray1 ^ InternalExecute(pArray1, pArray2, bTargetArray1, uCount, 24U, 256U, arrCounters, 
                                          arrOffset, pred);			 
    else
	    return bTargetArray1 ^ InternalExecuteFloatSign(pArray1, pArray2, bTargetArray1, uCount, 24U, arrCounters, 
                                                    arrOffset, pred);				 
}

template<class T, class K>
template<class Pred>
void ATOM_RadixSort<T, K>::Sort(T * pArray, unsigned uCount, Pred pred) const {
    if (uCount <= 1U)
	    return;

    T * pDummyArray = new T [uCount];
    if (Sort(pArray, pDummyArray, uCount, pred))
	    CopyArray(pDummyArray, pArray, uCount);

    delete [] pDummyArray;
}
#endif // __ATOM_UTILS_RADIX_H
