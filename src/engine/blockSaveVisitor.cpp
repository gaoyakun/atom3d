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

	///////////////////////////////// 计算场景包围盒 //////////////////////////////////////////////

	scene->getRootNode()->setLoadPriority(ATOM_LoadPriority_IMMEDIATE);

	//ATOM_BBox & worldBB = scene->getPrecalculatedBoundingBox();
	ATOM_ASSERT(scene);
	ATOM_BBox & worldBB = scene->calculateBoundingBox();

	const ATOM_Vector3f & world_min = worldBB.getMin();
	const ATOM_Vector3f & world_max = worldBB.getMax();

	ATOM_ASSERT ( !world_min.almostEqual(world_max) );
	if(world_min.almostEqual(world_max))
	{
		::MessageBoxA( ATOM_APP->getMainWindow(),"关卡保存失败，请确定场景包围盒合法!", "ATOM3D编辑器", MB_OK|MB_ICONHAND );
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
		// 保存
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

	// 检查是否是"World"
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

	// 不是第一个节点也不是world节点
	if( !bFirst && !bWorldNode )
	{
		// 如果有父节点(除了根节点都有父节点)
		if( node.getParent() )
		{
			ATOM_ScriptVar Describe;
			ATOM_GetAttribValue(node.getParent(),"Describe",Describe);
			ATOM_STRING desc = Describe.asString();

			// 如果父节点是world
			if( !stricmp( desc.c_str(), "World") )
			{
				// 光源节点 /*非*/异步
				if( ATOM_LightNode * ln = dynamic_cast<ATOM_LightNode *>(&node) )
				{
					node.setLoadPriority(ATOM_LoadPriority_IMMEDIATE);
					_xmlElement.back().bBLocked = false;
				}
				// 天空节点 可以使用异步 但是不放入BLOCK中
				else if( ATOM_Sky * sn = dynamic_cast<ATOM_Sky *>(&node) )
				{
					_xmlElement.back().bBLocked = false;
				}
				// 地形节点 可以使用异步 但是不放入BLOCK中
				else if( ATOM_Terrain * tn = dynamic_cast<ATOM_Terrain *>(&node) )
				{
					_xmlElement.back().bBLocked = false;
				}
				// 其他节点类型
				else
				{
					// 如异步 放进BLOCK
					if( node.getLoadPriority() != ATOM_LoadPriority_IMMEDIATE )
					{
						_xmlElement.back().bBLocked = true;
					}
					// 如非异步 不放进BLOCK
					else
					{
						_xmlElement.back().bBLocked = false;
					}
				}
			}
			// 如果父节点是地形
			else if( stricmp( desc.c_str(), "Terrain") == 0 )
			{
				// /*非*/异步
				node.setLoadPriority( ATOM_LoadPriority_IMMEDIATE );
				// 不放入BLOCK中
				_xmlElement.back().bBLocked = false;
			}
			// 如果父节点是其他节点
			if( stricmp( desc.c_str(), "World") != 0 && stricmp( desc.c_str(), "Terrain") != 0 )
			{
				// 异步与否取决于其父节点是否异步
				node.setLoadPriority( node.getParent()->getLoadPriority() );

				// 不放入BLOCK中（因为其父节点已经在BLOCK中)
				_xmlElement.back().bBLocked = false;
			}
		}
	}

	// 如果祖辈节点不在BLOCK中
	//if(!bAncesteralreadyInBlock)
	//{
	//	
	//	/*bool bAsyncLoad = node.isAsyncLoad();
	//	if( !bAsyncLoad )*/

	//	// 如果父节点是异步加载节点 其自身也是异步加载节点
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

	//	// 如果该节点设定为异步加载节点
	//	if( node.isAsyncLoad() )
	//	{
	//		// 设置放入BLOCK中
	//		_xmlElement.back().bBLocked = true;

	//		// 如果是光源节点/天空节点/地形节点 不作为异步加载
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
	//// 如果祖辈节点在BLOCK中,祖辈节点是异步加载节点，其子节点应该也是异步加载节点
	//else
	//{
	//	if( !bFirst && !bWorldOrTerrainNode )
	//		node.setLoadPriority(true);
	//}


	// 否则 不会放到BLOCK中
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
