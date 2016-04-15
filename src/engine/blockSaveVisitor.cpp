#include "StdAfx.h"
#include "blocksavevisitor.h"

ATOM_XmlBlockSaveVisitor::block ATOM_XmlBlockSaveVisitor::_worldBlock;
float ATOM_XmlBlockSaveVisitor::_blockMinSize = 300.0f;
//ATOM_XmlBlockSaveVisitor::block * 		 ATOM_XmlBlockSaveVisitor::_blocks = 0;
unsigned int ATOM_XmlBlockSaveVisitor::_block_num_x = 0;
unsigned int ATOM_XmlBlockSaveVisitor::_block_num_z = 0;
unsigned int ATOM_XmlBlockSaveVisitor::_block_total_count = 0;
//ATOM_SET<ATOM_Node*>	ATOM_XmlBlockSaveVisitor::_blocked_nodes;
//bool ATOM_XmlBlockSaveVisitor::CheckAncesterInBlock(ATOM_Node* pNode)
//{
//	ATOM_Node * parent = pNode->getParent();
//	while( parent )
//	{
//		if( _blocked_nodes.find(parent) != _blocked_nodes.end() )
//			return true;
//		parent = parent->getParent();
//	}
//	return false;
//}

static int total = 0;

ATOM_XmlBlockSaveVisitor::ATOM_XmlBlockSaveVisitor (void)
{
	_rootXmlElement = 0;
	_savedOK = true;

	
}

ATOM_XmlBlockSaveVisitor::ATOM_XmlBlockSaveVisitor (ATOM_TiXmlElement *xmlElement, ATOM_SDLScene* scene)
{
	_rootXmlElement = xmlElement;
	_savedOK = true;

	///////////////////////////////// ���㳡����Χ�� //////////////////////////////////////////////

	scene->getRootNode()->setLoadPriority(ATOM_LoadPriority_IMMEDIATE);

	//ATOM_BBox & worldBB = scene->getPrecalculatedBoundingBox();
	ATOM_ASSERT(scene);
	ATOM_BBox & worldBB = scene->calculateBoundingBox();

	const ATOM_Vector3f & world_min = worldBB.getMin();
	const ATOM_Vector3f & world_max = worldBB.getMax();

	ATOM_ASSERT ( !world_min.almostEqual(world_max) );
	if(world_min.almostEqual(world_max))
	{
		::MessageBoxA( ATOM_APP->getMainWindow(),"�ؿ�����ʧ�ܣ���ȷ��������Χ�кϷ�!", "ATOM3D�༭��", MB_OK|MB_ICONHAND );
	}

	float size_x = world_max.x - world_min.x;
	float size_z = world_max.z - world_min.z;
	_block_num_x = ceil( size_x / _blockMinSize ); 
	_block_num_z = ceil( size_z / _blockMinSize );

	_worldBlock.min_corner = ATOM_Vector2f( world_min.x, world_min.z );
	_worldBlock.max_corner = _worldBlock.min_corner + ATOM_Vector2f( _block_num_x * _blockMinSize, _block_num_z * _blockMinSize );
	
	_block_total_count = 0;
	if( !_rootXmlElement->FirstChildElement("Blocks") )
	{
		ATOM_TiXmlElement eBlocks ( "Blocks" );
		// ����
		eBlocks.SetDoubleAttribute("block_orgin_x",_worldBlock.min_corner.x);
		eBlocks.SetDoubleAttribute("block_orgin_z",_worldBlock.min_corner.y);
		eBlocks.SetAttribute("block_num_x",_block_num_x);
		eBlocks.SetAttribute("block_num_z",_block_num_z);
		eBlocks.SetDoubleAttribute("block_size",_blockMinSize);
		eBlocks.SetAttribute("block_total_count",0);
		_rootXmlElement->InsertEndChild (eBlocks);
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
}

ATOM_XmlBlockSaveVisitor::~ATOM_XmlBlockSaveVisitor (void)
{
	ATOM_LOGGER::log("+++++++++++++++++++++++++ total node count saved is %d +++++++++++++++++++++++ \n", total );
}

void ATOM_XmlBlockSaveVisitor::setFilter (FilterFunction function, void *userData)
{
	_filterFunction = function;
	_filterUserData = userData;
}

void ATOM_XmlBlockSaveVisitor::onBeginVisitNodeTree (ATOM_Node &node)
{
	_xmlElement.resize (_xmlElement.size() + 1);
	_xmlElement.back().xml = ATOM_NEW(ATOM_TiXmlElement, "node");
	_xmlElement.back().skipped = false;
	_xmlElement.back().bBLocked = false;
}

void ATOM_XmlBlockSaveVisitor::onEndVisitNodeTree (ATOM_Node &node)
{
	if (!_xmlElement.back().skipped)
	{
		if( !_xmlElement.back().bBLocked )
		{
			ATOM_TiXmlElement *parent = (_xmlElement.size() == 1) ? _rootXmlElement : _xmlElement[_xmlElement.size() - 2].xml;
			parent->InsertEndChild (*_xmlElement.back().xml);
		}
		else
		{
			InsertNodeToBlock(node);
			//_blocked_nodes.insert(&node);
		}

		total++;
	}


	ATOM_DELETE(_xmlElement.back().xml);
	_xmlElement.pop_back ();
}

void ATOM_XmlBlockSaveVisitor::onResetVisitor (void)
{
	for (unsigned i = 0; i < _xmlElement.size(); ++i)
	{
		ATOM_DELETE(_xmlElement[i].xml);
	}
	_xmlElement.resize (0);
}

void ATOM_XmlBlockSaveVisitor::traverse (ATOM_Node &node)
{
	_savedOK = true;

	ATOM_Visitor::traverse (node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_Node &node)
{
	if (node.getPrivate ())
	{
		skipChildren ();
		_xmlElement.back().skipped = true;
		return;
	}

	// ����Ƿ���"World"
	bool bWorldNode = false;
	ATOM_ScriptVar Describe;
	ATOM_GetAttribValue(&node,"Describe",Describe);
	ATOM_STRING desc = Describe.asString();
	if( stricmp( desc.c_str(), "World") == 0 )
	{
		bWorldNode = true;
		node.setLoadPriority(ATOM_LoadPriority_IMMEDIATE);
	}

	bool bFirst = (_xmlElement.size() == 1);
	//bool bAncesteralreadyInBlock = ( bFirst || bWorldNode ) ? true : ( _xmlElement[_xmlElement.size() - 2].bBLocked );

	// ���ǵ�һ���ڵ�Ҳ����world�ڵ�
	if( !bFirst && !bWorldNode )
	{
		// ����и��ڵ�(���˸��ڵ㶼�и��ڵ�)
		if( node.getParent() )
		{
			ATOM_ScriptVar Describe;
			ATOM_GetAttribValue(node.getParent(),"Describe",Describe);
			ATOM_STRING desc = Describe.asString();

			// ������ڵ���world
			if( !stricmp( desc.c_str(), "World") )
			{
				// ��Դ�ڵ� /*��*/�첽
				if( ATOM_LightNode * ln = dynamic_cast<ATOM_LightNode *>(&node) )
				{
					node.setLoadPriority(ATOM_LoadPriority_IMMEDIATE);
					_xmlElement.back().bBLocked = false;
				}
				// ��սڵ� ����ʹ���첽 ���ǲ�����BLOCK��
				else if( ATOM_Sky * sn = dynamic_cast<ATOM_Sky *>(&node) )
				{
					_xmlElement.back().bBLocked = false;
				}
				// ���νڵ� ����ʹ���첽 ���ǲ�����BLOCK��
				else if( ATOM_Terrain * tn = dynamic_cast<ATOM_Terrain *>(&node) )
				{
					_xmlElement.back().bBLocked = false;
				}
				// �����ڵ�����
				else
				{
					// ���첽 �Ž�BLOCK
					if( node.getLoadPriority() != ATOM_LoadPriority_IMMEDIATE )
					{
						_xmlElement.back().bBLocked = true;
					}
					// ����첽 ���Ž�BLOCK
					else
					{
						_xmlElement.back().bBLocked = false;
					}
				}
			}
			// ������ڵ��ǵ���
			else if( stricmp( desc.c_str(), "Terrain") == 0 )
			{
				// /*��*/�첽
				node.setLoadPriority( ATOM_LoadPriority_IMMEDIATE );
				// ������BLOCK��
				_xmlElement.back().bBLocked = false;
			}
			// ������ڵ��������ڵ�
			if( stricmp( desc.c_str(), "World") != 0 && stricmp( desc.c_str(), "Terrain") != 0 )
			{
				// �첽���ȡ�����丸�ڵ��Ƿ��첽
				node.setLoadPriority( node.getParent()->getLoadPriority() );

				// ������BLOCK�У���Ϊ�丸�ڵ��Ѿ���BLOCK��)
				_xmlElement.back().bBLocked = false;
			}
		}
	}

	// ����汲�ڵ㲻��BLOCK��
	//if(!bAncesteralreadyInBlock)
	//{
	//	
	//	/*bool bAsyncLoad = node.isAsyncLoad();
	//	if( !bAsyncLoad )*/

	//	// ������ڵ����첽���ؽڵ� ������Ҳ���첽���ؽڵ�
	//	if( ATOM_Terrain * tn = dynamic_cast<ATOM_Terrain *>(&node) )
	//	{

	//	}
	//	else if( ATOM_Sky * sn = dynamic_cast<ATOM_Sky *>(&node) )
	//	{

	//	}
	//	else
	//	{
	//		if( node.getParent() )
	//		{
	//			ATOM_ScriptVar Describe;
	//			ATOM_GetAttribValue(node.getParent(),"Describe",Describe);
	//			ATOM_STRING desc = Describe.asString();
	//			if( stricmp( desc.c_str(), "World") != 0 )
	//				node.setLoadPriority( node.getParent()->isAsyncLoad() );
	//		}
	//	}

	//	// ����ýڵ��趨Ϊ�첽���ؽڵ�
	//	if( node.isAsyncLoad() )
	//	{
	//		// ���÷���BLOCK��
	//		_xmlElement.back().bBLocked = true;

	//		// ����ǹ�Դ�ڵ�/��սڵ�/���νڵ� ����Ϊ�첽����
	//		if( ATOM_LightNode * ln = dynamic_cast<ATOM_LightNode *>(&node) )
	//		{
	//			node.setLoadPriority(false);
	//			_xmlElement.back().bBLocked = false;
	//		}
	//		if( ATOM_Sky * sn = dynamic_cast<ATOM_Sky *>(&node) )
	//			_xmlElement.back().bBLocked = false;
	//		if( ATOM_Terrain * tn = dynamic_cast<ATOM_Terrain *>(&node) )
	//			_xmlElement.back().bBLocked = false;

	//		/*if( ATOM_VisualNode * vn = dynamic_cast<ATOM_VisualNode *>(&node) )
	//		{
	//			_xmlElement.back().bBLocked = true;
	//		}
	//		else if(  ATOM_ParticleSystem * psn = dynamic_cast<ATOM_ParticleSystem *>(&node) )
	//		{
	//			_xmlElement.back().bBLocked = true;
	//		}*/
	//	}
	//}
	//// ����汲�ڵ���BLOCK��,�汲�ڵ����첽���ؽڵ㣬���ӽڵ�Ӧ��Ҳ���첽���ؽڵ�
	//else
	//{
	//	if( !bFirst && !bWorldOrTerrainNode )
	//		node.setLoadPriority(true);
	//}


	// ���� ����ŵ�BLOCK��
	//-------------//

	if (!_filterFunction || _filterFunction (&node, _filterUserData))
	{
		const char *className = node.getClassName();

		ATOM_TiXmlElement *e = _xmlElement.back().xml;

		if (!node.writeAttribute (e))
		{
			ATOM_LOGGER::error ("Save node %s(%s) failed.\n", node.getObjectName(), node.getClassName());
			_savedOK = false;
			cancelTraverse ();
		}
	}
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_NodeOctree &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_Terrain &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_VisualNode &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_Geode &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_ParticleSystem &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_Hud &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_GuiHud &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_Water &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_LightNode &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_Atmosphere &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_Atmosphere2 &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_Sky &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ATOM_Actor &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_XmlBlockSaveVisitor::visit (ClientSimpleCharacter &node)
{
	visit ((ATOM_Geode&)node);
}

bool ATOM_XmlBlockSaveVisitor::savedOK (void) const
{
	return _savedOK;
}

void ATOM_XmlBlockSaveVisitor::InsertNodeToBlock( ATOM_Node &node )
{
	const ATOM_BBox & bb = node.getWorldBoundingbox();
	ATOM_Vector3f min = bb.getMin();
	ATOM_Vector3f max = bb.getMax();
	unsigned int minX = ( min.x - _worldBlock.min_corner.x ) / _blockMinSize;
	unsigned int maxX = ( max.x - _worldBlock.min_corner.x ) / _blockMinSize;
	unsigned int minZ = ( min.z - _worldBlock.min_corner.y ) / _blockMinSize;
	unsigned int maxZ = ( max.z - _worldBlock.min_corner.y ) / _blockMinSize;

	unsigned int bestfit = 0;

	if( minX == maxX )
	{
		if( minZ == maxZ )
			bestfit = minX + minZ * _block_num_x;
		else
		{
			if( maxZ - minZ == 1 )
			{
				if( ( (maxZ+1) * _blockMinSize + _worldBlock.min_corner.y - max.z ) > ( (minZ+1) * _blockMinSize + _worldBlock.min_corner.y - min.z ) )
					bestfit = minX + maxZ * _block_num_x;
				else 
					bestfit = minX + minZ * _block_num_x;
			}
			else 
			{
				bestfit = minX + int( float( ( maxZ+minZ ) ) / 2 ) * _block_num_x;
			}
		}
	}
	else if( minZ == maxZ )
	{
		if( maxX - minX == 1 )
		{
			if( ( (maxX+1) * _blockMinSize + _worldBlock.min_corner.x - max.x ) > ( (minX+1) * _blockMinSize + _worldBlock.min_corner.x - min.x ) )
				bestfit = maxX + minZ * _block_num_x;
			else 
				bestfit = minX + minZ * _block_num_x;
		}
		else
			bestfit = int( float( (maxZ+minZ) ) / 2 ) + minZ * _block_num_x;
	}
	else
	{
		//for( unsigned int col = minX; col <= maxX; col++ )
		//	for( unsigned int row = minX; row <= maxX; row++ )
		//	{
		//		unsigned int index = col + row * _block_num_x;
		//		bestfit = index;
		//		
		//	}
		unsigned int bestfit_x = 0;
		unsigned int bestfit_z = 0;

		if( maxZ - minZ == 1 )
		{
			if( ( (maxZ+1) * _blockMinSize + _worldBlock.min_corner.y - max.z ) > ( (minZ+1) * _blockMinSize + _worldBlock.min_corner.y - min.z ) )
				bestfit_z = maxZ;
			else 
				bestfit_z = minX;
		}
		else 
		{
			bestfit_z = int( float( (maxZ+minZ) ) / 2 );
		}

		if( maxX - minX == 1 )
		{
			if( ( (maxX+1) * _blockMinSize + _worldBlock.min_corner.x - max.x ) > ( (minX+1) * _blockMinSize + _worldBlock.min_corner.x - min.x ) )
				bestfit_x = maxX;
			else 
				bestfit_x = minX;
		}
		else
		{
			bestfit_x = int( float( (maxX+minX) ) / 2 );
		}

		bestfit = bestfit_x + bestfit_z * _block_num_x;
	}

	char block_id[256]={0};
	sprintf(block_id,"Block_%u",bestfit);

	if( _rootXmlElement )
	{
		ATOM_TiXmlElement * blocks = _rootXmlElement->FirstChildElement("Blocks");
		if( blocks )
		{
			ATOM_TiXmlElement * block = blocks->FirstChildElement(block_id);
			if( !block )
			{
				ATOM_TiXmlElement eBlock ( block_id );
				eBlock.InsertEndChild(*_xmlElement.back().xml);
				blocks->InsertEndChild (eBlock);
				_block_total_count++;
				blocks->SetAttribute("block_total_count",_block_total_count);
			}
			else
				block->InsertEndChild(*_xmlElement.back().xml);
		}
	}
}
