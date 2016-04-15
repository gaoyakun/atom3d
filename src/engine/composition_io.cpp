#include "StdAfx.h"
#include "composition_io.h"
#include "composition_actor.h"

void ATOM_SaveActorToXML (ATOM_CompositionActor *actor, ATOM_TiXmlElement *xml)
{
	xml->SetAttribute ("ClassType", actor->getNode()->getClassName());
	xml->SetAttribute ("Ref", actor->isReference() ? 1 : 0);
	xml->SetAttribute ("NoReset", actor->isResetAllowed() ? 0 : 1);

	//--------- wangjian added -------------//
	// ��actor�ڵ�����ȼ��Ǻ͸�����ͬ ���Ի�ԭ��Ĭ�����ȼ�
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
// �첽���أ�������һ���첽���ص����ȼ���Ĭ�Ϸ��첽��
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

		//--- WANGJIAN : ������loadAttributeǰ���������ȼ� ---------------//
		// �첽����
		// ��ΪloadAttribute���ܻ�ı����ȼ���������ΪloadAttribute�п��ܻ��������ȣ�
		// �������Ƿ��첽������Ҫ���ȼ����ж�
		//bool bMT = ( loadPriority != ATOM_LoadPriority_IMMEDIATE );

		//if( !bMT )
		actor->getNode()->setLoadPriority(loadPriority);
		//---------------------------------------//

		//--- wangjian : �����������ʱ�������shapenode,��ͬʱ���ز������ԣ����ܻ������������ ---//
		actor->getNode()->loadAttribute (xml);

		//--- wangjian added --------------------//
		// ����
		//if( !bMT )
		actor->getNode()->setLoadPriority(loadPriority);
		//---------------------------------------//

		//------------------------------------------------------------------//
		// �������ʧ�� ɾ��actor ����NULL
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
			// �첽���� �����ü������ȼ�
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
