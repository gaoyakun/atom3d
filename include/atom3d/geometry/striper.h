#ifndef __ATOM_GEOMETRY_STRIPER_H
#define __ATOM_GEOMETRY_STRIPER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>

struct StripCreateInfo
{
	StripCreateInfo() {
		WFaces				= 0;
		NbFaces				= 0;
		AskForWords			= true;
		OneSided			= true;
		SGIAlgorithm		= true;
		ConnectAllStrips	= false;
	}

	unsigned  NbFaces;			// #faces in source topo
	unsigned short* WFaces;				// list of faces (words) or null
	bool  AskForWords;		// true => results are in words (else dwords)
	bool	OneSided;			// true => create one-sided strips
	bool	SGIAlgorithm;		// true => use the SGI algorithm, pick least connected faces first
	bool	ConnectAllStrips;	// true => create a single strip with void faces
};

struct StripCreateResult 
{
	unsigned	NbStrips;			// #strips created
	unsigned*	StripLengths;		// Lengths of the strips (NbStrips values)
	unsigned short*	StripRuns;			// The strips in words or dwords, depends on AskForWords
	bool	AskForWords;		// true => results are in words (else dwords)
};

class Adjacencies;
class Striper
{
public:
	Striper();
	~Striper();
	bool					Init(StripCreateInfo& create);
	bool					Compute(StripCreateResult& result);

private:
	Striper&	FreeUsedRam();
	unsigned	ComputeBestStrip(unsigned face);
	unsigned	TrackStrip(unsigned face, unsigned oldest, unsigned middle, unsigned* strip, unsigned* faces, bool* tags);
	bool			ConnectAllStrips(StripCreateResult& result);

private:
	Adjacencies*	mAdj;				// Adjacency structures
	bool*					mTags;				// Face markers
	unsigned      mNbStrips;			// The number of strips created for the mesh
	unsigned			mTotalLength;		// The length of the single strip
	bool					mAskForWords;
	bool					mOneSided;
	bool					mSGIAlgorithm;
	bool					mConnectAllStrips;
	std::vector<unsigned>       mStripLengths;		// Array to store strip lengths
	std::vector<unsigned short>	mStripRuns;			// Array to store strip indices
	std::vector<unsigned short> mSingleStrip;		// Array to store the single strip
};

#endif // __ATOM_GEOMETRY_STRIPER_H
