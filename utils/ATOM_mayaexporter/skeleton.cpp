#include <windows.h>

#include <maya/MDagPathArray.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MFnIkJoint.h>
#include <maya/MVector.h>
#include <maya/MQuaternion.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFnMatrixData.h>
#include <maya/MItDag.h>

#include "skeleton.h"

struct CRC32
{
	unsigned crc_table[256];

	CRC32 (void)
	{
		unsigned poly = 0xEDB88320;
		for (unsigned i = 0; i < 256; ++i)
		{
			unsigned crc = i;
			for (unsigned j = 8; j > 0; --j)
			{
				if (crc & 1)
				{
					crc = (crc >> 1) ^ poly;
				}
				else
				{
					crc >>= 1;
				}
			}
			crc_table[i] = crc;
		}
	}

	unsigned generate (const char *block, unsigned len) const
	{
		register unsigned crc = 0xFFFFFFFF;

		for (unsigned i = 0; i < len; ++i)
		{
			crc = ((crc >> 8) & 0x00FFFFFF) ^ crc_table[(crc ^ *block++) & 0xFF];
		}

		return crc ^ 0xFFFFFFFF;
	}
};

#define PRECISION 0.0001

ATOM_Skeleton::ATOM_Skeleton()
{
	m_joints.clear();
	m_skinCluster = 0;
	m_hash = 0;
}


ATOM_Skeleton::~ATOM_Skeleton()
{
	clear();
}


void ATOM_Skeleton::clear()
{
	m_joints.clear();
	m_skinCluster = 0;
}

MStatus ATOM_Skeleton::loadNames (MFnSkinCluster *pSkinCluster)
{
	m_skinCluster = pSkinCluster;

	MStatus stat;
	MDagPath jointDag,rootDag;
	MDagPathArray influenceDags;
	int numInfluenceObjs = pSkinCluster->influenceObjects(influenceDags,&stat);
	std::cout << "num influence objects: " << numInfluenceObjs << "\n";
	std::cout.flush();
	for (int i=0; i<numInfluenceObjs; i++)
	{
		jointDag = influenceDags[i];
		if (influenceDags[i].hasFn(MFn::kJoint))
		{
			//retrieve root joint
			rootDag = jointDag;
			while (jointDag.length()>0)
			{
				jointDag.pop();
				if (jointDag.hasFn(MFn::kJoint) && jointDag.length()>0)
					rootDag = jointDag;
			}
			//check if skeleton has already been loaded
			bool skip = false;
			for (int j=0; j<m_joints.size() && !skip; j++)
			{
				//skip skeleton if already loaded
				if (rootDag.partialPathName() == m_joints[j].name)
				{
					skip = true;
				}
			}
			//load joints data from root
			if (!skip)
			{
				//load joints data
				stat = loadJoint(rootDag,NULL,pSkinCluster);
				if (MS::kSuccess == stat)
				{
					std::cout << "OK\n";
					std::cout.flush();
				}
				else
				{
					std::cout << "Failed\n";
					std::cout.flush();
				}
			}
		}
	}

	std::string jointNameChain;

	for (unsigned i = 0; i < m_joints.size(); ++i)
	{
		jointNameChain += m_joints[i].name.asChar();
	}

	CRC32 crc32;
	m_hash = crc32.generate (jointNameChain.c_str(), jointNameChain.length());

	return MS::kSuccess;
}

MStatus ATOM_Skeleton::load(MFnSkinCluster* pSkinCluster)
{
	m_skinCluster = pSkinCluster;

	MStatus stat;
	//check for valid skin cluster pointer
	if (!pSkinCluster)
	{
		return MS::kFailure;
	}
	//retrieve and load joints from the skin cluster
	MDagPath jointDag,rootDag;
	MDagPathArray influenceDags;
	int numInfluenceObjs = pSkinCluster->influenceObjects(influenceDags,&stat);
	std::cout << "num influence objects: " << numInfluenceObjs << "\n";
	std::cout.flush();
	for (int i=0; i<numInfluenceObjs; i++)
	{
		jointDag = influenceDags[i];
		if (influenceDags[i].hasFn(MFn::kJoint))
		{
			//retrieve root joint
			rootDag = jointDag;
			while (jointDag.length()>0)
			{
				jointDag.pop();
				if (jointDag.hasFn(MFn::kJoint) && jointDag.length()>0)
					rootDag = jointDag;
			}
			//check if skeleton has already been loaded
			bool skip = false;
			for (int j=0; j<m_joints.size() && !skip; j++)
			{
				//skip skeleton if already loaded
				if (rootDag.partialPathName() == m_joints[j].name)
				{
					skip = true;
				}
			}
			//load joints data from root
			if (!skip)
			{
				// load the skeleton
				std::cout <<  "Loading skeleton with root: " << rootDag.fullPathName().asChar() << "...\n";
				std::cout.flush();
				// save current selection list
				MSelectionList selectionList;
				MGlobal::getActiveSelectionList(selectionList);
				// select the root joint dag
				MGlobal::selectByName(rootDag.fullPathName(),MGlobal::kReplaceList);

				//load joints data
				stat = loadJoint(rootDag,NULL,pSkinCluster);
				if (MS::kSuccess == stat)
				{
					std::cout << "OK\n";
					std::cout.flush();
				}
				else
				{
					std::cout << "Failed\n";
					std::cout.flush();
				}
				//restore selection list
				MGlobal::setActiveSelectionList(selectionList,MGlobal::kReplaceList);
			}
		}
	}

	std::string jointNameChain;

	for (unsigned i = 0; i < m_joints.size(); ++i)
	{
		jointNameChain += m_joints[i].name.asChar();
	}

	CRC32 crc32;
	m_hash = crc32.generate (jointNameChain.c_str(), jointNameChain.length());

	return MS::kSuccess;
}

static bool getJointBindPoseMatrix (MDagPath &jointDag, MMatrix &matrix)
{
	MStatus stat;

	MObject jointNode = jointDag.node();
	MFnDependencyNode fnJoint(jointNode);
	MObject attrWorldMatrix = fnJoint.attribute("worldMatrix", &stat);

	MPlug plugWorldMatrixArray(jointNode,attrWorldMatrix);

	for (unsigned i = 0; i < plugWorldMatrixArray.numElements (); i++)
	{
		unsigned connLength = 0;
		MPlugArray connPlugs;

		MPlug elementPlug = plugWorldMatrixArray [i];
		unsigned logicalIndex = elementPlug.logicalIndex();

		MItDependencyGraph dgIt(elementPlug,
								MFn::kInvalid,
								MItDependencyGraph::kDownstream,
								MItDependencyGraph::kDepthFirst,
								MItDependencyGraph::kPlugLevel,
								&stat);
		if (MS::kSuccess == stat)
		{
			dgIt.disablePruningOnFilter();
			int count = 0;

			for ( ; ! dgIt.isDone(); dgIt.next() )
			{
				MObject thisNode = dgIt.thisNode();

				if (thisNode.apiType() == MFn::kSkinClusterFilter)
				{
					MFnSkinCluster skinFn(thisNode);

					MPlug bindPreMatrixArrayPlug = 
						 skinFn.findPlug("bindPreMatrix", &stat);
					MPlug bindPreMatrixPlug =
						 bindPreMatrixArrayPlug.elementByLogicalIndex(logicalIndex, &stat);
					MObject dataObject;
					bindPreMatrixPlug.getValue( dataObject );

					MFnMatrixData matDataFn ( dataObject );

					MMatrix invMat = matDataFn.matrix();
					matrix = invMat.inverse();

					return true;

					// glbMat is now the world matrix for this
					// particular joint at bind/pose time
				}
			}
		}
	}

	return false;
}

// Load a joint
MStatus ATOM_Skeleton::loadJoint(MDagPath& jointDag,joint* parent,MFnSkinCluster* pSkinCluster)
{
	MStatus stat;
	int i;
	joint newJoint;
	joint* parentJoint = parent;
	// if it is a joint node translate it and then proceed to child nodes, otherwise skip it
	// and proceed directly to child nodes
	if (jointDag.hasFn(MFn::kJoint))
	{
		MFnIkJoint jointFn(jointDag);
		// Display info
		std::cout << "Loading joint: " << jointFn.fullPathName().asChar();
		std::cout.flush();
		if (parent)
		{
			std::cout << " (parent: " << parent->name.asChar() << ")\n";
			std::cout.flush();
		}
		else
		{
			std::cout << "\n";
			std::cout.flush();
		}
		// Get parent index
		int idx=-1;
		if (parent)
		{
			for (i=0; i<m_joints.size() && idx<0; i++)
			{
				if (m_joints[i].name == parent->name)
					idx=i;
			}
		}
		// Get world bind matrix
		ATOM_Matrix4x4f bindMatrix = MMatrixToMatrix (jointDag.inclusiveMatrix ());
		// Calculate local bind matrix
		ATOM_Matrix4x4f localMatrix;
		if (parent)
		{
			ATOM_Matrix4x4f parentInverseMatrix;
			parentInverseMatrix.invertAffineFrom (parent->bindMatrix);
			localMatrix = parentInverseMatrix >> bindMatrix;
		}
		else
		{	// root node of skeleton
			localMatrix = bindMatrix;
		}
		// Set joint info
		newJoint.name = jointFn.partialPathName();
		CRC32 crc32;
		newJoint.hashCode = crc32.generate (newJoint.name.asChar(), newJoint.name.length());
		if (strlen(newJoint.name.asChar()) > 63)
		{
			char buffer[1024];
			sprintf (buffer, "骨头名字太长:%s\n", newJoint.name.asChar());
			::MessageBox (::GetActiveWindow(), buffer, "atom exporter", MB_OK|MB_ICONHAND);
			return MS::kFailure;
		}

		newJoint.id = m_joints.size();
		newJoint.parentIndex = idx;
		newJoint.bindMatrix = bindMatrix;
		newJoint.localMatrix = localMatrix;
		newJoint.jointDag = jointDag;
		m_joints.push_back(newJoint);
		// If root is a root joint, save it's index in the roots list
		if (idx < 0)
		{
			m_roots.push_back(m_joints.size() - 1);
		}
		// Get pointer to newly created joint
		parentJoint = &newJoint;
	}
	// Load child joints
	for (i=0; i<jointDag.childCount();i++)
	{
		MObject child;
		child = jointDag.child(i);
		MDagPath childDag = jointDag;
		childDag.push(child);
		loadJoint(childDag,parentJoint,pSkinCluster);
	}

	return MS::kSuccess;
}

// Get joint list
std::vector<joint>& ATOM_Skeleton::getJoints()
{
	return m_joints;
}

unsigned ATOM_Skeleton::getHash (void) const
{
	if (!m_hash)
	{
		std::string jointNameChain;
		for (unsigned i = 0; i < m_joints.size(); ++i)
		{
			jointNameChain += m_joints[i].name.asChar();
		}

		CRC32 crc32;
		m_hash = crc32.generate (jointNameChain.c_str(), jointNameChain.length());
	}

	return m_hash;
}

bool ATOM_Skeleton::isIdentical (const ATOM_Skeleton &other) const
{
	if (m_joints.size() != other.m_joints.size())
	{
		return false;
	}

	for (unsigned i = 0 ;i < m_joints.size(); ++i)
	{
		if (m_joints[i].name != other.m_joints[i].name)
		{
			return false;
		}

		if (m_joints[i].parentIndex != other.m_joints[i].parentIndex)
		{
			return false;
		}
	}

	return true;
}

MStatus ATOM_Skeleton::load (void)
{
	MItDag itDag(MItDag::kDepthFirst, MFn::kJoint);
	MDagPath originPath;
	itDag.getPath (originPath);

	for (; !itDag.isDone (); itDag.next ())
	{
		MDagPath jointDagPath;
		itDag.getPath (jointDagPath);
		MFnIkJoint jointFn (jointDagPath);
		m_joints.resize (m_joints.size() + 1);
		m_joints.back().name = jointFn.partialPathName ();
		m_joints.back().jointDag = jointDagPath;
		m_joints.back().id = (int)m_joints.size() - 1;
		m_joints.back().parentIndex = -1;
	}

	buildHeirarchy ();

	return MStatus::kSuccess;
}

bool ATOM_Skeleton::buildHeirarchy (void)
{
	for (unsigned i = 0; i < m_joints.size(); ++i)
	{
		MFnIkJoint jointFn(m_joints[i].jointDag);

		unsigned numParents = jointFn.parentCount();
		if (numParents > 0)
		{
			bool found = false;

			for (unsigned j = 0; j < numParents; ++j)
			{
				if (found)
				{
					break;
				}

				if (jointFn.parent (j).hasFn (MFn::kJoint))
				{
					MFnDagNode parentNode (jointFn.parent (j));
					MDagPath parentDagPath;
					parentNode.getPath (parentDagPath);
					MFnIkJoint parentFn (parentDagPath);

					for (int k = 0; k < m_joints.size(); ++k)
					{
						if (m_joints[k].name == parentFn.partialPathName())
						{
							m_joints[i].parentIndex = k;
							found = true;
							break;
						}
					}
				}
			}
		}

		joint *parent = m_joints[i].parentIndex >= 0 ? &m_joints[m_joints[i].parentIndex] : 0;

		// Display info
		std::cout << "Loading joint: " << jointFn.fullPathName().asChar();
		std::cout.flush();
		if (parent)
		{
			std::cout << " (parent: " << parent->name.asChar() << ")\n";
			std::cout.flush();
		}
		else
		{
			std::cout << "\n";
			std::cout.flush();
		}

		// Get world bind matrix
		ATOM_Matrix4x4f bindMatrix = MMatrixToMatrix (m_joints[i].jointDag.inclusiveMatrix());
		// Calculate local bind matrix
		ATOM_Matrix4x4f localMatrix;
		if (parent)
		{
			ATOM_Matrix4x4f parentInverseMatrix;
			parentInverseMatrix.invertAffineFrom (parent->bindMatrix);
			localMatrix = parentInverseMatrix >> bindMatrix;
		}
		else
		{	// root node of skeleton
			localMatrix = bindMatrix;
		}

		CRC32 crc32;
		m_joints[i].hashCode = crc32.generate (m_joints[i].name.asChar(), m_joints[i].name.length());
		if (strlen(m_joints[i].name.asChar()) > 63)
		{
			char buffer[1024];
			sprintf (buffer, "骨头名字太长:%s\n", m_joints[i].name.asChar());
			::MessageBox (::GetActiveWindow(), buffer, "atom exporter", MB_OK|MB_ICONHAND);
			return false;
		}

		m_joints[i].bindMatrix = bindMatrix;
		m_joints[i].localMatrix = localMatrix;
	}

	return true;
}

void ATOM_Skeleton::remapJoint (void)
{
	m_remapOldToNew.resize (m_joints.size());
	m_remapNewToOld.resize (attachPoints.size());

	for (unsigned i = 0; i < attachPoints.size(); ++i)
	{
		m_remapOldToNew[attachPoints[i]] = i;
		m_remapNewToOld[i] = attachPoints[i];
	}
}

