#include "StdAfx.h"
#include "plugin.h"
#include "editor.h"
#include "app.h"
#include "plugin_model.h"
#include "model_prop_editor.h"

#define BUTTONID_CHANGE_MATERIAL 4
#define BUTTONID_CHANGE_TEXTURE  5

class MyValueChangeCallback: public ATOMX_TweakBar::ValueChangedCallback
{
	ModelPropEditor *_editor;

public:
	MyValueChangeCallback (ModelPropEditor *editor)
	{
		_editor = editor;
	}

public:
	virtual void callback (ATOMX_TWValueChangedEvent *event)
	{
		int currentMesh = _editor->getCurrentMesh ();
		ATOM_ASSERT(currentMesh >= 0);
		ATOM_SharedMesh *mesh = _editor->getModel()->getStaticMesh(currentMesh)->getSharedMesh ();
		if (event->id == 1)
		{
			mesh->setName (event->newValue.getS());
		}
		else
		{
			ATOM_ASSERT(mesh && mesh->getMaterial());
			ATOM_ParameterTable *paramTable = mesh->getMaterial()->getParameterTable();
			ATOM_ASSERT(paramTable);

			ATOM_ParameterTable::ValueHandle handle = paramTable->getValueHandle (event->name.c_str());
			ATOM_ASSERT(handle);

			switch (handle->handle->getParamType())
			{
			case ATOM_MaterialParam::ParamType_Float:
				paramTable->setFloat (handle, event->newValue.getF());
				_editor->setModified (true);
				break;
			case ATOM_MaterialParam::ParamType_Int:
				paramTable->setInt (handle, event->newValue.getI());
				_editor->setModified (true);
				break;
			case ATOM_MaterialParam::ParamType_Vector:
				paramTable->setVector (handle, *((ATOM_Vector4f*)(event->newValue.get4F())));
				_editor->setModified (true);
				break;
			}
		}
	}
};

void ModelPropCommandCallback::callback (ATOMX_TWCommandEvent *event)
{
	switch (event->id)
	{
	case BUTTONID_CHANGE_MATERIAL:
		{
			if (_editor->getEditor()->getOpenFileNames ("mat", "atom3d material (*.mat)|*.mat|", false, false) == 1)
			{
				
				ATOM_AUTOPTR(ATOM_Material) material = ATOM_MaterialManager::createMaterialFromCore(ATOM_GetRenderDevice(), _editor->getEditor()->getOpenedFileName(0));
				if (!material)
				{
					::MessageBox ((HWND)_editor->getEditor()->getRenderWindow()->getWindowId(), _T("Load material failed"), _T("Error"), MB_OK|MB_ICONHAND);
				}
				else
				{
					int currentMesh = _editor->getCurrentMesh();
					ATOM_ASSERT(currentMesh >= 0);
					ATOM_StaticMesh *mesh = _editor->getModel()->getStaticMesh (currentMesh);

					//--- wangjian modified ---//
					if( strstr( _editor->getEditor()->getOpenedFileName(0),"model_bumpmap" ) || 
						strstr( _editor->getEditor()->getOpenedFileName(0),"model_flux" )	)
						mesh->getSharedMesh()->genTangentSpace();

					ATOM_AUTOPTR(ATOM_Material) mat_current = mesh->getMaterial();
					if( mat_current )
					{
						ATOM_ParameterTable * param_current = mat_current->getParameterTable();
						if( param_current )
						{
							ATOM_ParameterTable * param_new = material->getParameterTable();
							if( param_new )
							{
								param_new->getParameterFrom(param_current);
							}
						}
					}
					//-------------------------//
					
					mesh->getSharedMesh()->setMaterial (material.get());
					mesh->setMaterial (material.get());
					_editor->refresh ();
					_editor->setModified (true);
				}
			}
			break;
		}
	case BUTTONID_CHANGE_TEXTURE:
		{
			if (_editor->getEditor()->getOpenImageFileNames (false, false) == 1)
			{
				int currentMesh = _editor->getCurrentMesh();
				ATOM_ASSERT(currentMesh >= 0);
				ATOM_SharedMesh *mesh = _editor->getModel()->getStaticMesh (currentMesh)->getSharedMesh();
				mesh->getMaterial()->getParameterTable()->setTexture (event->name.c_str(), _editor->getEditor()->getOpenedImageFileName(0));
				_editor->setModified (true);
			}
			break;
		}
	default:
		break;
	}
}

ModelPropEditor::ModelPropEditor (AS_Editor *editor)
{
	_editor = editor;
	_currentMesh = -1;
	_bar = 0;
	_isShown = false;
	_callback = ATOM_NEW(ModelPropCommandCallback, this);
	_valueCallback = ATOM_NEW(MyValueChangeCallback, this);
	_modified = false;
}

ModelPropEditor::~ModelPropEditor (void)
{
	ATOM_DELETE(_bar);
	ATOM_DELETE(_callback);
	ATOM_DELETE(_valueCallback);
}

void ModelPropEditor::setPosition (int x, int y, int w, int h)
{
	if (!_bar)
	{
		createBar ();
	}

	_bar->setBarPosition (x, y);
	_bar->setBarSize (w, h);
}

void ModelPropEditor::createBar (void)
{
	if (!_bar)
	{
		_bar = ATOM_NEW(ATOMX_TweakBar, "Mesh Properties");
		_bar->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
		_bar->setBarMovable (false);
		_bar->setBarResizable (false);
		_bar->setBarIconifiable (false);
		_bar->enableNotifying (true);
		_bar->setCommandCallback (_callback);
		_bar->setValueChangedCallback (_valueCallback);
	}

	setupBar ();
}

void ModelPropEditor::setModel (ATOM_Geode *model)
{
	_model = model;
	_currentMesh = -1;
	refresh ();
}

void ModelPropEditor::setCurrentMesh (int currentMesh)
{
	if (_currentMesh != currentMesh)
	{
		_currentMesh = currentMesh;
		refresh ();
	}
}

int ModelPropEditor::getCurrentMesh (void) const
{
	return _currentMesh;
}

ATOM_Geode *ModelPropEditor::getModel (void) const
{
	return _model.get();
}

void ModelPropEditor::refresh (void)
{
	if (_model)
	{
		if (!_bar)
		{
			createBar ();
		}
		setupBar ();
	}
	else if (_bar)
	{
		ATOM_DELETE(_bar);
		_bar = 0;
	}
}

void ModelPropEditor::setModified (bool b)
{
	_modified = b;
}

bool ModelPropEditor::isModified (void) const
{
	return _modified;
}

void ModelPropEditor::show (bool b)
{
	if (b && !_bar)
	{
		createBar ();
	}

	_isShown = b;
	_bar->setBarVisible (b);
}

bool ModelPropEditor::isShown (void) const
{
	return _isShown;
}

void ModelPropEditor::newParameter (const char *name, ATOM_ParameterTable::ValueHandle value)
{
	ATOM_VECTOR<ATOM_STRING> enumNames;
	ATOM_VECTOR<int> enumValues;

#define ENUM(name, type) \
	enumNames.push_back (#name); \
	enumValues.push_back (ATOM_RenderAttributes::type##_##name);

	if (value->handle->getEditorType() != ATOM_MaterialParam::ParamEditorType_Disable)
	{
		switch (value->handle->getParamType ())
		{
		case ATOM_MaterialParam::ParamType_Float:
			newFloatParameter (name, *(value->f));
			break;
		case ATOM_MaterialParam::ParamType_Int:
			switch (value->handle->getEditorType())
			{
			case ATOM_MaterialParam::ParamEditorType_Bool:
				newBoolParameter (name, (*(value->i)) != 0);
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_BlendFunc:
				ENUM(Zero, BlendFunc);
				ENUM(One, BlendFunc);
				ENUM(SrcColor, BlendFunc);
				ENUM(InvSrcColor, BlendFunc);
				ENUM(SrcAlpha, BlendFunc);
				ENUM(InvSrcAlpha, BlendFunc);
				ENUM(DestAlpha, BlendFunc);
				ENUM(InvDestAlpha, BlendFunc);
				ENUM(DestColor, BlendFunc);
				ENUM(InvDestColor, BlendFunc);
				ENUM(Constant, BlendFunc);
				ENUM(InvConstant, BlendFunc);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_BlendOp:
				ENUM(Add, BlendOp);
				ENUM(Sub, BlendOp);
				ENUM(InvSub, BlendOp);
				ENUM(Min, BlendOp);
				ENUM(Max, BlendOp);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_ColorMask:
				ENUM(All, ColorMask);
				ENUM(None, ColorMask);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_CompareFunc:
				ENUM(Never, CompareFunc);
				ENUM(Always, CompareFunc);
				ENUM(Less, CompareFunc);
				ENUM(LessEqual, CompareFunc);
				ENUM(Greater, CompareFunc);
				ENUM(GreaterEqual, CompareFunc);
				ENUM(Equal, CompareFunc);
				ENUM(NotEqual, CompareFunc);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_CullMode:
				ENUM(None, CullMode);
				ENUM(Front, CullMode);
				ENUM(Back, CullMode);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_FillMode:
				ENUM(Solid, FillMode);
				ENUM(Line, FillMode);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_FogMode:
				ENUM(Linear, FogMode);
				ENUM(Exp, FogMode);
				ENUM(Exp2, FogMode);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_FrontFace:
				ENUM(CW, FrontFace);
				ENUM(CCW, FrontFace);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_PrimitiveType:
				ENUM(TriList, PrimitiveType);
				ENUM(TriStrip, PrimitiveType);
				ENUM(TriFan, PrimitiveType);
				ENUM(LineList, PrimitiveType);
				ENUM(LineStrip, PrimitiveType);
				ENUM(PointList, PrimitiveType);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_SamplerAddress:
				ENUM(Wrap, SamplerAddress);
				ENUM(Clamp, SamplerAddress);
				ENUM(Mirror, SamplerAddress);
				ENUM(Border, SamplerAddress);
				ENUM(MirrorOnce, SamplerAddress);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_SamplerFilter:
				ENUM(PPP, SamplerFilter);
				ENUM(PPL, SamplerFilter);
				ENUM(PPN, SamplerFilter);
				ENUM(PLP, SamplerFilter);
				ENUM(PLL, SamplerFilter);
				ENUM(PLN, SamplerFilter);
				ENUM(LPP, SamplerFilter);
				ENUM(LPL, SamplerFilter);
				ENUM(LPN, SamplerFilter);
				ENUM(LLP, SamplerFilter);
				ENUM(LLL, SamplerFilter);
				ENUM(LLN, SamplerFilter);
				ENUM(AnisotropicL, SamplerFilter);
				ENUM(AnisotropicP, SamplerFilter);
				ENUM(AnisotropicN, SamplerFilter);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_ShadeMode:
				ENUM(Flat, ShadeMode);
				ENUM(Gouraud, ShadeMode);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_StencilOp:
				ENUM(Keep, StencilOp);
				ENUM(Zero, StencilOp);
				ENUM(Replace, StencilOp);
				ENUM(Inc, StencilOp);
				ENUM(Dec, StencilOp);
				ENUM(Invert, StencilOp);
				ENUM(IncWrap, StencilOp);
				ENUM(DecWrap, StencilOp);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_TexCoordGen:
				ENUM(Auto, TexCoordGen);
				ENUM(SphereMap, TexCoordGen);
				ENUM(TexCoord0, TexCoordGen);
				ENUM(TexCoord1, TexCoordGen);
				ENUM(TexCoord2, TexCoordGen);
				ENUM(TexCoord3, TexCoordGen);
				ENUM(TexCoord4, TexCoordGen);
				ENUM(TexCoord5, TexCoordGen);
				ENUM(TexCoord6, TexCoordGen);
				ENUM(TexCoord7, TexCoordGen);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_TextureArg:
				ENUM(Constant, TextureArg);
				ENUM(Current, TextureArg);
				ENUM(Diffuse, TextureArg);
				ENUM(Texture, TextureArg);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			case ATOM_MaterialParam::ParamEditorType_Constant_TextureOp:
				ENUM(Disable, TextureOp);
				ENUM(Replace, TextureOp);
				ENUM(Modulate, TextureOp);
				ENUM(Modulate2x, TextureOp);
				ENUM(Modulate4x, TextureOp);
				ENUM(Add, TextureOp);
				ENUM(AddSigned, TextureOp);
				ENUM(Sub, TextureOp);
				ENUM(Dot3, TextureOp);
				ENUM(Lerp, TextureOp);
				newEnumParameter (name, enumNames, enumValues, *(value->i));
				break;
			default:
				newIntParameter (name, *(value->i));
				break;
			}
			break;
		case ATOM_MaterialParam::ParamType_Vector:
			switch (value->handle->getEditorType())
			{
			case ATOM_MaterialParam::ParamEditorType_Color:
				newColorParameter (name, *(value->v));
				break;
			case ATOM_MaterialParam::ParamEditorType_Direction:
				newDirectionParameter (name, *(value->v));
				break;
			default:
				newVectorParameter (name, *(value->v));
			}
			break;
		case ATOM_MaterialParam::ParamType_Texture:
			newTextureParameter (name, value->textureFileName.c_str());
			break;
		}
	}
}

void ModelPropEditor::newTextureParameter (const char *name, const char *filename)
{
	_bar->addButton (name, BUTTONID_CHANGE_TEXTURE, name, 0, 0);
}

void ModelPropEditor::newFloatParameter (const char *name, float value)
{
	_bar->addFloatVar (name, 0, value, false, 0, 0);
}

void ModelPropEditor::newIntParameter (const char *name, int value)
{
	_bar->addIntVar (name, 0, value, false, 0, 0);
}

void ModelPropEditor::newVectorParameter (const char *name, const ATOM_Vector4f &value)
{
	_bar->addVector4fVar (name, 0, value.x, value.y, value.z, value.w, false, 0, 0);
}

void ModelPropEditor::newColorParameter (const char *name, const ATOM_Vector4f &value)
{
	_bar->addRGBAVar (name, 0, value.x, value.y, value.z, value.w, false, 0, 0);
}

void ModelPropEditor::newDirectionParameter (const char *name, const ATOM_Vector4f &value)
{
	_bar->addDirVar (name, 0, value.x, value.y, value.z, false, 0, 0);
}

void ModelPropEditor::newBoolParameter (const char *name, bool value)
{
	_bar->addBoolVar (name, 0, value, false, 0, 0);
}

void ModelPropEditor::newEnumParameter (const char *name, const ATOM_VECTOR<ATOM_STRING> &enumNames, const ATOM_VECTOR<int> &enumValues, int value)
{
	ATOMX_TBEnum enumValue;
	for (unsigned i = 0; i < enumNames.size(); ++i)
	{
		enumValue.addEnum (enumNames[i].c_str(), enumValues[i]);
	}
	_bar->addEnum (name, 0, value, enumValue, false, 0, 0);
}

void ModelPropEditor::setupBar (void)
{
	_bar->clear ();

	if (_currentMesh >= 0)
	{
		ATOM_SharedMesh *mesh = _model->getComponents(0).getMesh(_currentMesh);
		_bar->addButton ("ChangeMaterial", BUTTONID_CHANGE_MATERIAL, "Change material...", NULL, mesh);
		_bar->addStringVar ("Name", 1, mesh->getName(), false, 0, 0);

		ATOM_ParameterTable *paramTable = mesh->getMaterial()->getParameterTable();
		for (int i = 0; i < paramTable->getNumParameters(); ++i)
		{
			ATOM_ParameterTable::ValueHandle value = paramTable->getParameter(i);
			newParameter (value->getParameterName(), value);
		}
	}
}

ATOMX_TweakBar *ModelPropEditor::getBar (void) const
{
	return _bar;
}

AS_Editor *ModelPropEditor::getEditor (void) const
{
	return _editor;
}

