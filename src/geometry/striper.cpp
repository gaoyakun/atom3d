#include <assert.h>
#include <string.h>
#include "striper.h"
#include "radixsort.h"
#include "adjacency.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Striper::Striper()
  : mAdj(0), mTags(0), mSingleStrip(0) {
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Striper::~Striper()
{
    FreeUsedRam();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A method to free possibly used ram
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input	:	-
// Output	:	-
// Return	:	Self-reference
// Exception:	-
// Remark	:	-
Striper& Striper::FreeUsedRam()
{
  mStripLengths.clear ();
  mStripRuns.clear();
  mSingleStrip.clear();
  delete []mTags;
  mTags = 0;
  delete mAdj;
  mAdj = 0;
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A method to initialize the striper
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input	:	create,		the creation structure
// Output	:	-
// Return	:	true if success
// Exception:	-
// Remark	:	-
bool Striper::Init(StripCreateInfo& create)
{
    // Release possibly already used ram
    FreeUsedRam();

    // Create adjacencies
    {
	    mAdj = new Adjacencies;
    assert (mAdj);

	    ADJACENCIESCREATE ac;
	    ac.NbFaces	= create.NbFaces;
	    ac.WFaces	= create.WFaces;
	    bool Status = mAdj->Init(ac);
	    if(!Status)	{ delete mAdj; mAdj = 0; return false; }

	    Status = mAdj->CreateDatabase();
	    if(!Status)	{ delete mAdj; mAdj = 0; return false; }

	    mAskForWords		= create.AskForWords;
	    mOneSided			= create.OneSided;
	    mSGIAlgorithm		= create.SGIAlgorithm;
	    mConnectAllStrips	= create.ConnectAllStrips;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A method to create the triangle strips
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input	:	result,		the result structure
// Output	:	-
// Return	:	true if success
// Exception:	-
// Remark	:	-
bool Striper::Compute(StripCreateResult& result)
{
    // You must call Init() first
    if(!mAdj)	return false;

    // Get some bytes
    mTags	= new bool[mAdj->mNbFaces];
  assert (mTags);
    unsigned* Connectivity	= new unsigned[mAdj->mNbFaces];
  assert (Connectivity);

    // mTags contains one bool/face. True=>the face has already been included in a strip
    memset(mTags, 0, mAdj->mNbFaces*sizeof(bool));

    // Compute the number of connections for each face. This buffer is further recycled into
    // the insertion order, ie contains face indices in the order we should treat them
    memset(Connectivity, 0, mAdj->mNbFaces*sizeof(unsigned));
    if(mSGIAlgorithm)
    {
	    // Compute number of adjacent triangles for each face
	    for(unsigned i=0;i<mAdj->mNbFaces;i++)
	    {
		    AdjTriangle* Tri = &mAdj->mFaces[i];
		    if(!IS_BOUNDARY(Tri->ATri[0]))	Connectivity[i]++;
		    if(!IS_BOUNDARY(Tri->ATri[1]))	Connectivity[i]++;
		    if(!IS_BOUNDARY(Tri->ATri[2]))	Connectivity[i]++;
	    }

	    // Sort by number of neighbors
	    RadixSorter RS;
	    unsigned* Sorted = RS.Sort(Connectivity, mAdj->mNbFaces).GetIndices();

	    // The sorted indices become the order of insertion in the strips
	    memcpy(Connectivity, Sorted, mAdj->mNbFaces*sizeof(unsigned));
    }
    else
    {
	    // Default order
	    for(unsigned i=0;i<mAdj->mNbFaces;i++)	Connectivity[i] = i;
    }

    mNbStrips			= 0;	// #strips created
    unsigned TotalNbFaces	= 0;	// #faces already transformed into strips
    unsigned Index		= 0;	// Index of first face

    while(TotalNbFaces!=mAdj->mNbFaces)
    {
	    // Look for the first face [could be optimized]
	    while(mTags[Connectivity[Index]])	Index++;
	    unsigned FirstFace = Connectivity[Index];

	    // Compute the three possible strips from this face and take the best
	    TotalNbFaces += ComputeBestStrip(FirstFace);

	    // Let's wrap
	    mNbStrips++;
    }

    // Free now useless ram
    delete []Connectivity;
  Connectivity = 0;
    delete []mTags;
  mTags = 0;

    // Fill result structure and exit
    result.NbStrips		= mNbStrips;
    result.StripLengths	= (unsigned*)	&mStripLengths[0];
    result.StripRuns =	(unsigned short*) &mStripRuns[0];

    if(mConnectAllStrips)	ConnectAllStrips(result);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A method to compute the three possible strips starting from a given face
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input	:	face,		the first face
// Output	:	-
// Return	:	unsigned,		the #faces included in the strip
// Exception:	-
// Remark	:	mStripLengths and mStripRuns are filled with strip data
unsigned Striper::ComputeBestStrip(unsigned face)
{
    unsigned* Strip[3];		// Strips computed in the 3 possible directions
    unsigned* Faces[3];		// Faces involved in the 3 previous strips
    unsigned Length[3];		// Lengths of the 3 previous strips

    unsigned FirstLength[3];	// Lengths of the first parts of the strips are saved for culling

    // Starting references
    unsigned Refs0[3];
    unsigned Refs1[3];
    Refs0[0] = mAdj->mFaces[face].VRef[0];
    Refs1[0] = mAdj->mFaces[face].VRef[1];

    // Bugfix by Eric Malafeew!
    Refs0[1] = mAdj->mFaces[face].VRef[2];
    Refs1[1] = mAdj->mFaces[face].VRef[0];

    Refs0[2] = mAdj->mFaces[face].VRef[1];
    Refs1[2] = mAdj->mFaces[face].VRef[2];

    // Compute 3 strips
  unsigned i,j;
    for(j=0;j<3;j++)
    {
	    // Get some bytes for the strip and its faces
	    Strip[j] = new unsigned[mAdj->mNbFaces+2+1+2];	// max possible length is NbFaces+2, 1 more if the first index gets replicated
	    Faces[j] = new unsigned[mAdj->mNbFaces+2];
	    memset(Strip[j], 0xFF, (mAdj->mNbFaces+2+1+2)*sizeof(unsigned));
	    memset(Faces[j], 0xFF, (mAdj->mNbFaces+2)*sizeof(unsigned));

	    // Create a local copy of the tags
	    bool* Tags	= new bool[mAdj->mNbFaces];
	    memcpy(Tags, mTags, mAdj->mNbFaces*sizeof(bool));

	    // Track first part of the strip
	    Length[j] = TrackStrip(face, Refs0[j], Refs1[j], &Strip[j][0], &Faces[j][0], Tags);

	    // Save first length for culling
	    FirstLength[j] = Length[j];
//		if(j==1)	FirstLength[j]++;	// ...because the first face is written in reverse order for j==1

	    // Reverse first part of the strip
	    for(i=0;i<Length[j]/2;i++)
	    {
		    Strip[j][i]				^= Strip[j][Length[j]-i-1];
		    Strip[j][Length[j]-i-1]	^= Strip[j][i];
		    Strip[j][i]				^= Strip[j][Length[j]-i-1];
	    }
	    for(i=0;i<(Length[j]-2)/2;i++)
	    {
		    Faces[j][i]				^= Faces[j][Length[j]-i-3];
		    Faces[j][Length[j]-i-3]	^= Faces[j][i];
		    Faces[j][i]				^= Faces[j][Length[j]-i-3];
	    }

	    // Track second part of the strip
	    unsigned NewRef0 = Strip[j][Length[j]-3];
	    unsigned NewRef1 = Strip[j][Length[j]-2];
	    unsigned ExtraLength = TrackStrip(face, NewRef0, NewRef1, &Strip[j][Length[j]-3], &Faces[j][Length[j]-3], Tags);
	    Length[j]+=ExtraLength-3;

	    // Free temp ram
	    delete []Tags;
    Tags = 0;
    }

    // Look for the best strip among the three
    unsigned Longest	= Length[0];
    unsigned Best		= 0;
    if(Length[1] > Longest)	{	Longest = Length[1];	Best = 1;	}
    if(Length[2] > Longest)	{	Longest = Length[2];	Best = 2;	}

    unsigned NbFaces = Longest-2;

    // Update global tags
    for(j=0;j<Longest-2;j++)	mTags[Faces[Best][j]] = true;

    // Flip strip if needed ("if the length of the first part of the strip is odd, the strip must be reversed")
    if(mOneSided && FirstLength[Best]&1)
    {
	    // Here the strip must be flipped. I hardcoded a special case for triangles and quads.
	    if(Longest==3 || Longest==4)
	    {
		    // Flip isolated triangle or quad
		    Strip[Best][1] ^= Strip[Best][2];
		    Strip[Best][2] ^= Strip[Best][1];
		    Strip[Best][1] ^= Strip[Best][2];
	    }
	    else
	    {
		    // "to reverse the strip, write it in reverse order"
		    for(j=0;j<Longest/2;j++)
		    {
			    Strip[Best][j]				^= Strip[Best][Longest-j-1];
			    Strip[Best][Longest-j-1]	^= Strip[Best][j];
			    Strip[Best][j]				^= Strip[Best][Longest-j-1];
		    }

		    // "If the position of the original face in this new reversed strip is odd, you're done"
		    unsigned NewPos = Longest-FirstLength[Best];
		    if(NewPos&1)
		    {
			    // "Else replicate the first index"
			    for(j=0;j<Longest;j++)	Strip[Best][Longest-j] = Strip[Best][Longest-j-1];
			    Longest++;
		    }
	    }
    }

    // Copy best strip in the strip buffers
    for(j=0;j<Longest;j++)
    {
	    unsigned Ref = Strip[Best][j];
	    mStripRuns.push_back(Ref);	// Saves word reference
    }
    mStripLengths.push_back(Longest);

    // Free local ram
    for(j=0;j<3;j++)
    {
	    delete []Faces[j];
    Faces[j] = 0;
	    delete []Strip[j];
    Strip[j] = 0;
    }

    // Returns #faces involved in the strip
    return NbFaces;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A method to extend a strip in a given direction, starting from a given face
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input	:	face,				the starting face
//				oldest, middle,		the two first indices of the strip == a starting edge == a direction
// Output	:	strip,				a buffer to store the strip
//				faces,				a buffer to store the faces of the strip
//				tags,				a buffer to mark the visited faces
// Return	:	unsigned,				the strip length
// Exception:	-
// Remark	:	-
unsigned Striper::TrackStrip(unsigned face, unsigned oldest, unsigned middle, unsigned* strip, unsigned* faces, bool* tags)
{
    unsigned Length = 2;														// Initial length is 2 since we have 2 indices in input
    strip[0] = oldest;														// First index of the strip
    strip[1] = middle;														// Second index of the strip

    bool DoTheStrip = true;
    while(DoTheStrip)
    {
	    unsigned Newest = mAdj->mFaces[face].OppositeVertex(oldest, middle);	// Get the third index of a face given two of them
	    strip[Length++] = Newest;											// Extend the strip,...
	    *faces++ = face;													// ...keep track of the face,...
	    tags[face] = true;													// ...and mark it as "done".

	    unsigned char CurEdge = mAdj->mFaces[face].FindEdge(middle, Newest);		// Get the edge ID...

	    unsigned Link = mAdj->mFaces[face].ATri[CurEdge];						// ...and use it to catch the link to adjacent face.
	    if(IS_BOUNDARY(Link))	DoTheStrip = false;							// If the face is no more connected, we're done...
	    else
	    {
		    face = MAKE_ADJ_TRI(Link);										// ...else the link gives us the new face index.
		    if(tags[face])	DoTheStrip=false;								// Is the new face already done?
	    }
	    oldest = middle;													// Shift the indices and wrap
	    middle = Newest;
    }
    return Length;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A method to link all strips in a single one.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input	:	result,		the result structure
// Output	:	the result structure is updated
// Return	:	true if success
// Exception:	-
// Remark	:	-
bool Striper::ConnectAllStrips(StripCreateResult& result)
{
    mTotalLength	= 0;
    unsigned short* wrefs	= (unsigned short*)result.StripRuns;
//	    unsigned* drefs	= mAskForWords ? 0 : (unsigned*)result.StripRuns;

    // Loop over strips and link them together
    for(unsigned k=0;k<result.NbStrips;k++)
    {
	    // Nothing to do for the first strip, we just copy it
	    if(k)
	    {
		    // This is not the first strip, so we must copy two void vertices between the linked strips
		    unsigned LastRef	= (unsigned)wrefs[-1];
		    unsigned FirstRef	= (unsigned)wrefs[0];
		    mSingleStrip.push_back(LastRef);
      mSingleStrip.push_back(FirstRef);
		    mTotalLength += 2;

		    // Linking two strips may flip their culling. If the user asked for single-sided strips we must fix that
		    if(mOneSided)
		    {
			    // Culling has been inverted only if mTotalLength is odd
			    if(mTotalLength&1)
			    {
				    // We can fix culling by replicating the first vertex once again...
				    unsigned SecondRef = (unsigned)wrefs[1];
				    if(FirstRef!=SecondRef)
				    {
					    mSingleStrip.push_back(FirstRef);
					    mTotalLength++;
				    }
				    else
				    {
					    // ...but if flipped strip already begin with a replicated vertex, we just can skip it.
					    result.StripLengths[k]--;
					    if(wrefs)	wrefs++;
				    }
			    }
		    }
	    }

	    // Copy strip
	    for(unsigned j=0;j<result.StripLengths[k];j++)
	    {
		    unsigned Ref = (unsigned)wrefs[j];
		    mSingleStrip.push_back(Ref);
	    }
	    if(wrefs)	wrefs += result.StripLengths[k];
	    mTotalLength += result.StripLengths[k];
    }

    // Update result structure
    result.NbStrips		= 1;
    result.StripRuns	= (unsigned short*)&mSingleStrip[0];
    result.StripLengths	= &mTotalLength;

    return true;
}
