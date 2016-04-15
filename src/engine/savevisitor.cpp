#include "StdAfx.h"
#include "savevisitor.h"

ATOM_XmlSaveVisitor::ATOM_XmlSaveVisitor (void)
{
	_rootXmlElement = 0;
	_savedOK = true;
}

ATOM_XmlSaveVisitor::ATOM_XmlSaveVisitor (ATOM_TiXmlElement *xmlElement)
{
	_rootXmlElement = xmlElement;
	_savedOK = true;
}

ATOM_XmlSaveVisitor::~ATOM_XmlSaveVisitor (void)
{
}

void ATOM_XmlSaveVisitor::setFilter (FilterFunction function, void *userData)
{
	_filterFunction = function;
	_filterUserData = userData;
}

void ATOM_XmlSaveVisitor::onBeginVisitNodeTree (ATOM_Node &node)
{
	_xmlElement.resize (_xmlElement.size() + 1);
	_xmlElement.back().xml = ATOM_NEW(ATOM_TiXmlElement, "node");
	_xmlElement.back().skipped = false;
}

void ATOM_XmlSaveVisitor::onEndVisitNodeTree (ATOM_Node &node)
{
	if (!_xmlElement.back().skipped)
	{
		ATOM_TiXmlElement *parent = (_xmlElement.size() == 1) ? _rootXmlElement : _xmlElement[_xmlElement.size() - 2].xml;
		parent->InsertEndChild (*_xmlElement.back().xml);
	}
	ATOM_DELETE(_xmlElement.back().xml);
	_xmlElement.pop_back ();
}

void ATOM_XmlSaveVisitor::onResetVisitor (void)
{
	for (unsigned i = 0; i < _xmlElement.size(); ++i)
	{
		ATOM_DELETE(_xmlElement[i].xml);
	}
	_xmlElement.resize (0);
}

void ATOM_XmlSaveVisitor::traverse (ATOM_Node &node)
{
	_savedOK = true;

	ATOM_Visitor::traverse (node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_Node &node)
{
	if (node.getPrivate ())
	{
		skipChildren ();
		_xmlElement.back().skipped = true;
		return;
	}

	if (!_filterFunction || _filterFunction (&node, _filterUserData))
	{
		if (node.getPredefinedBBoxMin() == 0.f)
		{
			node.setPredefinedBBoxMin (node.getBoundingbox().getMin());
		}

		if (node.getPredefinedBBoxMax() == 0.f)
		{
			node.setPredefinedBBoxMax (node.getBoundingbox().getMax());
		}

		ATOM_TiXmlElement *e = _xmlElement.back().xml;
		if (!node.writeAttribute (e))
		{
			ATOM_LOGGER::error ("Save node %s(%s) failed.\n", node.getObjectName(), node.getClassName());
			_savedOK = false;
			cancelTraverse ();
		}
	}
}

void ATOM_XmlSaveVisitor::visit (ATOM_NodeOctree &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_Terrain &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_VisualNode &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_Geode &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_ParticleSystem &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_Hud &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_GuiHud &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_Water &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_LightNode &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_Atmosphere &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_Atmosphere2 &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_Sky &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_XmlSaveVisitor::visit (ATOM_Actor &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_XmlSaveVisitor::visit (ClientSimpleCharacter &node)
{
	visit ((ATOM_Geode&)node);
}

bool ATOM_XmlSaveVisitor::savedOK (void) const
{
	return _savedOK;
}
