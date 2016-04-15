#ifndef __ATOM_GEOMETRY_RADIXSORT_H
#define __ATOM_GEOMETRY_RADIXSORT_H

#if _MSC_VER > 1000
# pragma once
#endif

class RadixSorter
{
public:
// Constructor/Destructor
	RadixSorter();
	~RadixSorter();

// Sorting methods
	RadixSorter& Sort(unsigned* input, unsigned nb, bool signedvalues=true);
	RadixSorter& Sort(float* input, unsigned nb);
  // Access to results
  // mIndices is a list of indices in sorted order, i.e. in the order you may further process your data
	unsigned *GetIndices() { return mIndices; }
  // Reset the inner indices
	RadixSorter& ResetIndices();
  // Stats
	unsigned					GetUsedRam();

private:
	unsigned*	mHistogram;			// Counters for each byte
	unsigned*	mOffset;				// Offsets (nearly a cumulative distribution function)
	unsigned	mCurrentSize;		// Current size of the indices list
	unsigned*	mIndices;				// Two lists, swapped each pass
	unsigned*	mIndices2;
};

#endif // __ATOM_GEOMETRY_RADIXSORT_H
