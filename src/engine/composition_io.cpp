#include "StdAfx.h"
#include "composition_io.h"
#include "composition_actor.h"

void ATOM_SaveActorToXML (ATOM_CompositionActor *actor, ATOM_TiXmlElement *xml)
{
	xml->SetAttribute ("ClassType", actor->getNode()->getClassName());
	xml->SetAttribute ("Ref", actor->isReference() ? 1 : 0);
	xml->SetAttribute ("NoReset", actor->isResetAllowed() ? 0 : 1);

	//--------- wangjian added -------------//
	// 子actor节点的优先级是和父级相同 所以还原成默认优先级
	actor->getNode()->setLoadPriority(ATOM_LoadPriority_ASYNCBASE);
	//--------------------------------------//

	actor->getNode()->writeAttribute (xml);
	actor->saveToXML (xml);

	for (unsigned i = 0; i < actor->getNumChildActors(); ++i)
	{
		ATOM_TiXmlElement e("Actor");
		ATOM_SaveActorToXML (actor->getChildActor(i), &e);
		xml->InsertEndChild (e);
	}
}

//--- wangjian modified ---//
// 异步加载：增加了一个异步加载的优先级（默认非异步）
ATOM_CompositionActor *ATOM_LoadActorFromXML (ATOM_RenderDevice *device, const ATOM_TiXmlElement *xml, bool loadRef, int loadPriority)
{
	int ref = 0;
	xml->QueryIntAttribute ("Ref", &ref);
	if (ref && !loadRef)
	{
		return 0;
	}

	int noReset = 0;
	xml->QueryIntAttribute ("NoReset", &noReset);
	
	const char *type = xml->Attribute ("ClassType");
	if (type)
	{
		ATOM_CompositionActor *actor = 0;

		if (!stricmp (type, ATOM_ParticleSystem::_classname()))
		{
			actor = ATOM_NEW(ATOM_ParticleSystemActor);
		}
		else if (!stricmp (type, ATOM_ShapeNode::_classname()))
		{
			actor = ATOM_NEW(ATOM_ShapeActor);
		}
		else if (!stricmp (type, ATOM_Geode::_classname()))
		{
			actor = ATOM_NEW(ATOM_GeodeActor);
		}
		else if (!stricmp (type, ATOM_LightNode::_classname()))
		{
			actor = ATOM_NEW(ATOM_LightActor);
		}
		else if (!stricmp (type, ATOM_WeaponTrail::_classname()))
		{
			actor = ATOM_NEW(ATOM_WeaponTrailActor);
		}
		else if (!stricmp (type, ATOM_Decal::_classname()))
		{
			actor = ATOM_NEW(ATOM_DecalActor);
		}
		else
		{
			return 0;
		}

		if (!actor->getNode ())
		{
			ATOM_DELETE(actor);
			return 0;
		}

		//--- WANGJIAN : 这里在loadAttribute前后都设置优先级 ---------------//
		// 异步加载
		// 因为loadAttribute可能会改变优先级，但是因为loadAttribute中可能会加载纹理等，
		// 而纹理是否异步加载需要优先级做判断
		//bool bMT = ( loadPriority != ATOM_LoadPriority_IMMEDIATE );

		//if( !bMT )
		actor->getNode()->setLoadPriority(loadPriority);
		//---------------------------------------//

		//--- wangjian : 这里加载属性时，如果是shapenode,会同时加载材质属性，可能会加载设置纹理 ---//
		actor->getNode()->loadAttribute (xml);

		//--- wangjian added --------------------//
		// 重设
		//if( !bMT )
		actor->getNode()->setLoadPriority(loadPriority);
		//---------------------------------------//

		//------------------------------------------------------------------//
		// 加载如果失败 删除actor 返回NULL
		if( false == actor->getNode()->load (device) )
		{
			ATOM_DELETE(actor);
			return 0;
		}
		//------------------------------------------------------------------//

		actor->getNode()->setPickable (ATOM_Node::INHERIT);

		actor->makeReference (ref != 0);
		actor->allowReset (noReset == 0);
		actor->loadFromXML (xml);

		for (const ATOM_TiXmlElement *actorElement = xml->FirstChildElement("Actor"); actorElement; actorElement = actorElement->NextSiblingElement ("Actor"))
		{
			//--- wangjian modified ---//
			// 异步加载 ：设置加载优先级
			ATOM_AUTOPTR(ATOM_CompositionActor) childActor = ATOM_LoadActorFromXML ( device, actorElement, loadRef, loadPriority );
			//-------------------------//
			if (childActor)
			{
				actor->addChildActor (childActor.get());
			}
		}

		return actor;
	}

	return 0;
}
