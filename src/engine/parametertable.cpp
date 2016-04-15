#include "StdAfx.h"
#include "parametertable.h"

extern bool setFloatParameter (ATOM_CoreMaterial::ParamHandle param, float value);
extern bool setFloatArrayParameter (ATOM_CoreMaterial::ParamHandle param, const float *values, unsigned count);
extern bool setIntParameter (ATOM_CoreMaterial::ParamHandle param, int value);
extern bool setIntArrayParameter (ATOM_CoreMaterial::ParamHandle param, const int *values, unsigned count);
extern bool setVectorParameter (ATOM_CoreMaterial::ParamHandle param, const ATOM_Vector4f &value);
extern bool setVectorArrayParameter (ATOM_CoreMaterial::ParamHandle param, const ATOM_Vector4f *values, unsigned count);
extern bool setMatrixParameter (ATOM_CoreMaterial::ParamHandle param, const ATOM_Matrix4x4f &value);
extern bool setMatrixArrayParameter (ATOM_CoreMaterial::ParamHandle param, const ATOM_Matrix4x4f *values, unsigned count);
extern bool setTextureParameter (ATOM_CoreMaterial::ParamHandle param, ATOM_Texture *value);

ATOM_ParameterTable::ATOM_ParameterTable (void)
{
	_coreMaterial = 0;
	_currentPass = 0;
	_constantDirty = true;

	//-- wangjian added ---//
	_dirtyFlag = 0xffffffff;
	//---------------------//
}

ATOM_ParameterTable::ATOM_ParameterTable (ATOM_CoreMaterial *material)
{
	_coreMaterial = 0;

	if (material)
	{
		_coreMaterial = material;
		_coreMaterial->initParameterTable (this);

		_effectValidationInfos.resize (_coreMaterial->getNumEffects ());
		for (unsigned i = 0; i < _effectValidationInfos.size(); ++i)
		{
			createValidationInfo (&_effectValidationInfos[i], _coreMaterial->getEffect (i));
		}
	}

	_constantDirty = true;
	_currentPass = 0;

	//-- wangjian added ---//
	_dirtyFlag = 0xffffffff;
	//---------------------//
}

ATOM_ParameterTable::~ATOM_ParameterTable (void)
{
	for (int i = 0; i < _paramsArray.size(); ++i)
	{
		ATOM_DELETE(_paramsArray[i]);
	}
	_paramsArray.clear ();

	_paramTable.clear ();
}

ATOM_ParameterTable::ValueHandle ATOM_ParameterTable::findValueHandle (ATOM_MaterialParam *param) const
{
	for (int i = 0; i < _paramsArray.size(); ++i)
	{
		if (_paramsArray[i]->handle == param)
		{
			return _paramsArray[i];
		}
	}
	return 0;
}

void ATOM_ParameterTable::createValidationInfo (EffectValidationInfo *info, ATOM_MaterialEffect *effect) const
{
	bool (*func[6][2]) (void *context, void *context2) =
	{
		{ &requireFunc_FloatEQ, &requireFunc_IntEQ },
		{ &requireFunc_FloatGE, &requireFunc_IntGE },
		{ &requireFunc_FloatGT, &requireFunc_IntGT },
		{ &requireFunc_FloatLE, &requireFunc_IntLE },
		{ &requireFunc_FloatLT, &requireFunc_IntLT },
		{ &requireFunc_FloatNE, &requireFunc_IntNE }
	};

	info->callbacks.resize (effect->getRequireEntries().size());
	for (unsigned i = 0; i < info->callbacks.size(); ++i)
	{
		const ATOM_MaterialEffect::RequireEntry &entry = effect->getRequireEntries()[i];
		int n;
		switch (entry.param->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Float:
			n = 0;
			break;
		case ATOM_MaterialParam::ParamType_Int:
			n = 1;
			break;
		default:
			ATOM_ASSERT(0);
			info->callbacks[i].callback = 0;
			return;
		}
		info->callbacks[i].callback = func[entry.compare][n];

		ValueHandle handle = findValueHandle (entry.param);
		info->callbacks[i].context = handle;
		info->callbacks[i].context2 = entry.context;

		//--- wangjian added ---//
		//handle->isValidationFlag |= ATOM_RenderScheme::getSchemeMaskID(effect->getRenderScheme());
		handle->isValidationFlag |= ATOM_RenderScheme::getSchemeLayerMaskID(effect->_schemeFactory,effect->getRenderSchemeLayerNumber());
		/*if( handle->validationBitIndex == -1 )
		{
			handle->validationBitIndex = effect->getMaterial()->validationBitIndex++;
			effect->getMaterial()->appendLayerValidationBit(effect->_renderSchemeLayerNumber,handle->validationBitIndex);
		}*/
		//----------------------//
	}

	//--- wangjian added ---//
	//unsigned effectid = 
	//effect->getMaterial()->appendSchemeLayerEffect(effect->_schemeFactory,effect->_renderSchemeLayerNumber,/**/,effect);
}

unsigned ATOM_ParameterTable::getNumParameters (void) const
{
	return _paramsArray.size();
}

ATOM_EffectParameterValue *ATOM_ParameterTable::getParameter (int index) const
{
	return _paramsArray[index];
}

bool ATOM_ParameterTable::isEffectValid (unsigned index) const
{
	//--- wangjian added ---//
	ATOM_AUTOPTR(ATOM_MaterialEffect) effect = _coreMaterial->getEffect(index);
	if( !effect || !effect->isEnable() )
		return false;
	//----------------------//

	if (index < _effectValidationInfos.size())
	{
		const ATOM_VECTOR<ValidationCallbackInfo> &callbacks = _effectValidationInfos[index].callbacks;
		for (unsigned i = 0; i < callbacks.size(); ++i)
		{
			if (!callbacks[i].callback(callbacks[i].context, callbacks[i].context2))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

typedef ATOM_HASHMAP<ATOM_STRING, ATOM_ParameterTable::ValueHandle>::const_iterator _ParamConstIter;

//--- wangjian modified ---//
// 异步加载 ：在该方法中，会有加载纹理的操作，所以加了一个纹理加载优先级（默认为异步）
void ATOM_ParameterTable::loadFromXML (const ATOM_TiXmlElement *xml, int texLoadPriority/* = ATOM_LoadPriority_ASYNCBASE*/)
{
	_constantDirty = true;

	ATOM_TiXmlElement *xml2 = const_cast<ATOM_TiXmlElement*>(xml);
	for (ATOM_TiXmlElement *paramElement = xml2->FirstChildElement ("Param"); paramElement; paramElement = paramElement->NextSiblingElement ("Param"))
	{
		const char *name = paramElement->Attribute ("name");
		if (name)
		{
			ATOM_HASHMAP<ATOM_STRING, int>::iterator it = _paramTable.find (name);
			if (it != _paramTable.end ())
			{
				ATOM_EffectParameterValue *val = _paramsArray[it->second];
				ATOM_MaterialParam *param = val->handle;
				switch (param->getParamType ())
				{
				case ATOM_MaterialParam::ParamType_Float:
					{
						const char *s = paramElement->Attribute ("type");
						ATOM_ASSERT(s && !stricmp(s, "float"));
						paramElement->QueryFloatAttribute ("value", val->f);
						break;
					}
				case ATOM_MaterialParam::ParamType_FloatArray:
					{
						const char *s = paramElement->Attribute ("type");
						ATOM_ASSERT(s && !stricmp(s, "float_array"));
						unsigned num = 0;
						for (ATOM_TiXmlElement *element = paramElement->FirstChildElement("element"); element && num < val->dimension; element = element->NextSiblingElement ("element"), num++)
						{
							element->QueryFloatAttribute ("value", &val->f[num]);
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Int:
					{
						const char *s = paramElement->Attribute ("type");
						ATOM_ASSERT(s && !stricmp(s, "int"));
						paramElement->QueryIntAttribute ("value", val->i);
						break;
					}
				case ATOM_MaterialParam::ParamType_IntArray:
					{
						const char *s = paramElement->Attribute ("type");
						ATOM_ASSERT(s && !stricmp(s, "int_array"));
						unsigned num = 0;
						for (ATOM_TiXmlElement *element = paramElement->FirstChildElement("element"); element && num < val->dimension; element = element->NextSiblingElement ("element"), num++)
						{
							element->QueryIntAttribute ("value", &val->i[num]);
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Matrix43:
					{
						const char *s = paramElement->Attribute ("type");
						ATOM_ASSERT(s && !stricmp(s, "matrix43"));
						const char *m = paramElement->Attribute ("value");
						if (m)
						{
							sscanf (m, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
								&val->m43->m00, &val->m43->m01, &val->m43->m02, &val->m43->m03,
								&val->m43->m10, &val->m43->m11, &val->m43->m12, &val->m43->m13,
								&val->m43->m20, &val->m43->m21, &val->m43->m22, &val->m43->m23);
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Matrix43Array:
					{
						const char *s = paramElement->Attribute ("type");
						ATOM_ASSERT(s && !stricmp(s, "matrix43_array"));
						unsigned num = 0;
						for (ATOM_TiXmlElement *element = paramElement->FirstChildElement("element"); element && num < val->dimension; element = element->NextSiblingElement ("element"), num++)
						{
							const char *m = element->Attribute ("value");
							if (m)
							{
								sscanf (m, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
									&val->m43[num].m00, &val->m43[num].m01, &val->m43[num].m02, &val->m43[num].m03,
									&val->m43[num].m10, &val->m43[num].m11, &val->m43[num].m12, &val->m43[num].m13,
									&val->m43[num].m20, &val->m43[num].m21, &val->m43[num].m22, &val->m43[num].m23);
							}
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Matrix44:
					{
						const char *s = paramElement->Attribute ("type");
						ATOM_ASSERT(s && !stricmp(s, "matrix44"));
						const char *m = paramElement->Attribute ("value");
						if (m)
						{
							sscanf (m, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
								&val->m44->m00, &val->m44->m01, &val->m44->m02, &val->m44->m03,
								&val->m44->m10, &val->m44->m11, &val->m44->m12, &val->m44->m13,
								&val->m44->m20, &val->m44->m21, &val->m44->m22, &val->m44->m23,
								&val->m44->m30, &val->m44->m31, &val->m44->m32, &val->m44->m33);
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Matrix44Array:
					{
						const char *s = paramElement->Attribute ("type");
						ATOM_ASSERT(s && !stricmp(s, "matrix44_array"));
						unsigned num = 0;
						for (ATOM_TiXmlElement *element = paramElement->FirstChildElement("element"); element && num < val->dimension; element = element->NextSiblingElement ("element"), num++)
						{
							const char *m = element->Attribute ("value");
							if (m)
							{
								sscanf (m, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
									&val->m44[num].m00, &val->m44[num].m01, &val->m44[num].m02, &val->m44[num].m03,
									&val->m44[num].m10, &val->m44[num].m11, &val->m44[num].m12, &val->m44[num].m13,
									&val->m44[num].m20, &val->m44[num].m21, &val->m44[num].m22, &val->m44[num].m23,
									&val->m44[num].m30, &val->m44[num].m31, &val->m44[num].m32, &val->m44[num].m33);
							}
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Texture:
					{
						const char *s = paramElement->Attribute ("type");
						ATOM_ASSERT(s && !stricmp(s, "texture"));
						const char *str = paramElement->Attribute ("value");
						if (str)
						{
							//--- wangjian modified ---//
							// 异步加载 ：纹理异步加载标志
							val->setTexture ( str, texLoadPriority );
							//-------------------------//
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Vector:
					{
						const char *s = paramElement->Attribute ("type");
						ATOM_ASSERT(s && !stricmp(s, "vector4"));
						const char *v = paramElement->Attribute ("value");
						if (v)
						{
							sscanf (v, "%f,%f,%f,%f", &val->v->x, &val->v->y, &val->v->z, &val->v->w);
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_VectorArray:
					{
						const char *s = paramElement->Attribute ("type");
						ATOM_ASSERT(s && !stricmp(s, "vector4_array"));
						unsigned num = 0;
						for (ATOM_TiXmlElement *element = paramElement->FirstChildElement("element"); element && num < val->dimension; element = element->NextSiblingElement ("element"), num++)
						{
							const char *m = element->Attribute ("value");
							if (m)
							{
								sscanf (m, "%f,%f,%f,%f", &val->v[num].x, &val->v[num].y, &val->v[num].z, &val->v[num].w);
							}
						}
						break;
					}
				}
			}
		}
	}
}

void ATOM_ParameterTable::saveToXML (ATOM_TiXmlElement *xml) const
{
	char buffer[1024];

	for (ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.begin(); it != _paramTable.end(); ++it)
	{
		ATOM_EffectParameterValue *val = _paramsArray[it->second];
		ATOM_MaterialParam *param = val->handle;
		ATOM_TiXmlElement paramElement("Param");
		paramElement.SetAttribute ("name", it->first.c_str());

		if (param->getEditorType() != ATOM_MaterialParam::ParamEditorType_Disable)
		{
			switch (param->getParamType ())
			{
			case ATOM_MaterialParam::ParamType_Float:
				{
					paramElement.SetAttribute ("type", "float");
					paramElement.SetDoubleAttribute("value", *val->f);
					break;
				}
			case ATOM_MaterialParam::ParamType_FloatArray:
				{
					paramElement.SetAttribute ("type", "float_array");
					for (unsigned i = 0;i < val->dimension; ++i)
					{
						ATOM_TiXmlElement e("element");
						e.SetDoubleAttribute ("value", val->f[i]);
						paramElement.InsertEndChild (e);
					}
					break;
				}
			case ATOM_MaterialParam::ParamType_Int:
				{
					paramElement.SetAttribute ("type", "int");
					paramElement.SetAttribute("value", *val->i);
					break;
				}
			case ATOM_MaterialParam::ParamType_IntArray:
				{
					paramElement.SetAttribute ("type", "int_array");
					for (unsigned i = 0;i < val->dimension; ++i)
					{
						ATOM_TiXmlElement e("element");
						e.SetAttribute ("value", val->i[i]);
						paramElement.InsertEndChild (e);
					}
					break;
				}
			case ATOM_MaterialParam::ParamType_Matrix43:
				{
					paramElement.SetAttribute ("type", "matrix43");
					sprintf(buffer, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
						val->m43->m00, val->m43->m01, val->m43->m02, val->m43->m03,
						val->m43->m10, val->m43->m11, val->m43->m12, val->m43->m13,
						val->m43->m20, val->m43->m21, val->m43->m22, val->m43->m23);
					paramElement.SetAttribute ("value", buffer);
					break;
				}
			case ATOM_MaterialParam::ParamType_Matrix43Array:
				{
					paramElement.SetAttribute ("type", "matrix43_array");
					for (unsigned i = 0; i < val->dimension; ++i)
					{
						ATOM_TiXmlElement e("element");
						sprintf(buffer, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
							val->m43[i].m00, val->m43[i].m01, val->m43[i].m02, val->m43[i].m03,
							val->m43[i].m10, val->m43[i].m11, val->m43[i].m12, val->m43[i].m13,
							val->m43[i].m20, val->m43[i].m21, val->m43[i].m22, val->m43[i].m23);
						e.SetAttribute ("value", buffer);
						paramElement.InsertEndChild (e);
					}
					break;
				}
			case ATOM_MaterialParam::ParamType_Matrix44:
				{
					paramElement.SetAttribute ("type", "matrix44");
					sprintf(buffer, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
						val->m44->m00, val->m44->m01, val->m44->m02, val->m44->m03,
						val->m44->m10, val->m44->m11, val->m44->m12, val->m44->m13,
						val->m44->m20, val->m44->m21, val->m44->m22, val->m44->m23,
						val->m44->m30, val->m44->m31, val->m44->m32, val->m44->m33);
					paramElement.SetAttribute ("value", buffer);
					break;
				}
			case ATOM_MaterialParam::ParamType_Matrix44Array:
				{
					paramElement.SetAttribute ("type", "matrix44_array");
					for (unsigned i = 0; i < val->dimension; ++i)
					{
						ATOM_TiXmlElement e("element");
						sprintf(buffer, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
							val->m44[i].m00, val->m44[i].m01, val->m44[i].m02, val->m44[i].m03,
							val->m44[i].m10, val->m44[i].m11, val->m44[i].m12, val->m44[i].m13,
							val->m44[i].m20, val->m44[i].m21, val->m44[i].m22, val->m44[i].m23,
							val->m44[i].m30, val->m44[i].m31, val->m44[i].m32, val->m44[i].m33);
						e.SetAttribute ("value", buffer);
						paramElement.InsertEndChild (e);
					}
					break;
				}
			case ATOM_MaterialParam::ParamType_Texture:
				{
					paramElement.SetAttribute ("type", "texture");
					paramElement.SetAttribute ("value", val->textureFileName.c_str());
					break;
				}
			case ATOM_MaterialParam::ParamType_Vector:
				{
					paramElement.SetAttribute ("type", "vector4");
					sprintf(buffer, "%f,%f,%f,%f", val->v->x, val->v->y, val->v->z, val->v->w);
					paramElement.SetAttribute ("value", buffer);
					break;
				}
			case ATOM_MaterialParam::ParamType_VectorArray:
				{
					paramElement.SetAttribute ("type", "vector4_array");
					for (unsigned i = 0; i < val->dimension; ++i)
					{
						ATOM_TiXmlElement e("element");
						sprintf(buffer, "%f,%f,%f,%f", val->v[i].x, val->v[i].y, val->v[i].z, val->v[i].w);
						e.SetAttribute ("value", buffer);
						paramElement.InsertEndChild (e);
					}
					break;
				}
			default:
				{
					continue;
				}
			}
			xml->InsertEndChild (paramElement);
		}
	}
}

ATOM_ParameterTable::ValueHandle ATOM_ParameterTable::getValueHandle (const char *param) const
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		return (it == _paramTable.end()) ? 0 : _paramsArray[it->second];
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATOM_ParameterTable::setFloat (const char *param, float value)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setFloat (_paramsArray[it->second], value);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setFloat (ValueHandle param, float value)
{
	if (param && *param->f != value)
	{
		//--- wangjian added ---//
		if( param->isValidationFlag && ( *(param->f) != value ) )
		{
			_dirtyFlag |= param->isValidationFlag;
		}
		//----------------------//

		*param->f = value;
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

void ATOM_ParameterTable::writeShaderConstant (ValueHandle param)
{
	if (_currentPass)
	{
		int paramIndex = param->handle->getParamIndex();
		const ATOM_Shader::ParamDesc *descVS = _currentPass->getShaderParamDescVS();
		if (descVS)
		{
			descVS += paramIndex;
			int regCount = descVS->registerCount;
			if (regCount > 0)
			{
				ATOM_Vector4f *dest = &_shaderConstantMirrorVS[descVS->registerIndex];
				param->handle->writeRegisterValue (dest, param->p, descVS);
				if (_currentPass->isInBegin ())
				{
					ATOM_GetRenderDevice()->setConstantsVS(descVS->registerIndex, dest, descVS->registerCount);
				}
			}
		}
		const ATOM_Shader::ParamDesc *descPS = _currentPass->getShaderParamDescPS();
		if (descPS)
		{
			descPS += paramIndex;
			int regCount = descPS->registerCount;
			if (regCount > 0)
			{
				ATOM_Vector4f *dest = &_shaderConstantMirrorPS[descPS->registerIndex];
				param->handle->writeRegisterValue (dest, param->p, descPS);
				if (_currentPass->isInBegin ())
				{
					ATOM_GetRenderDevice()->setConstantsPS(descPS->registerIndex, dest, descPS->registerCount);
				}
			}
		}
	}
}

bool ATOM_ParameterTable::setFloatArray (const char *param, const float *values, unsigned count)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setFloatArray (_paramsArray[it->second], values, count);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setFloatArray (ValueHandle param, const float *values, unsigned count)
{
	if (param)
	{
		param->setFloatArray (values, count);
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setInt (const char *param, int value)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setInt (_paramsArray[it->second], value);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setInt (ValueHandle param, int value)
{
	if (param && *param->i != value)
	{
		//--- wangjian added ---//
		if( param->isValidationFlag && ( *(param->i) != value ) )
		{
			_dirtyFlag |= param->isValidationFlag;
		}
		//----------------------//

		param->setInt (value);
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setIntArray (const char *param, const int *values, unsigned count)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setIntArray (_paramsArray[it->second], values, count);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setIntArray (ValueHandle param, const int *values, unsigned count)
{
	if (param)
	{
		param->setIntArray (values, count);
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setVector (const char *param, const ATOM_Vector4f &value)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setVector (_paramsArray[it->second], value);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setVector (ValueHandle param, const ATOM_Vector4f &value)
{
	if (param)
	{
		param->setVector (value);
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setVector (const char *param, const ATOM_Vector3f &value)
{
	return setVector (param, ATOM_Vector4f(value.x, value.y, value.z, 0.f));
}

bool ATOM_ParameterTable::setVector (ValueHandle param, const ATOM_Vector3f &value)
{
	return setVector (param, ATOM_Vector4f(value.x, value.y, value.z, 0.f));
}

bool ATOM_ParameterTable::setVector (const char *param, const ATOM_Vector2f &value)
{
	return setVector (param, ATOM_Vector4f(value.x, value.y, 0.f, 0.f));
}

bool ATOM_ParameterTable::setVector (ValueHandle param, const ATOM_Vector2f &value)
{
	return setVector (param, ATOM_Vector4f(value.x, value.y, 0.f, 0.f));
}

bool ATOM_ParameterTable::setVectorArray (const char *param, const ATOM_Vector4f *values, unsigned count)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setVectorArray (_paramsArray[it->second], values, count);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setVectorArray (ValueHandle param, const ATOM_Vector4f *values, unsigned count)
{
	if (param)
	{
		param->setVectorArray (values, count);
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setVectorArray (const char *param, const ATOM_Vector3f *values, unsigned count)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setVectorArray (_paramsArray[it->second], values, count);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setVectorArray (ValueHandle param, const ATOM_Vector3f *values, unsigned count)
{
	if (param)
	{
		param->setVectorArray (values, count);
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setVectorArray (const char *param, const ATOM_Vector2f *values, unsigned count)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setVectorArray (_paramsArray[it->second], values, count);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setVectorArray (ValueHandle param, const ATOM_Vector2f *values, unsigned count)
{
	if (param)
	{
		param->setVectorArray (values, count);
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setMatrix44 (const char *param, const ATOM_Matrix4x4f &value)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setMatrix44 (_paramsArray[it->second], value);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setMatrix44 (ValueHandle param, const ATOM_Matrix4x4f &value)
{
	if (param)
	{
		param->setMatrix44 (value);
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setMatrix44Array (const char *param, const ATOM_Matrix4x4f *values, unsigned count)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setMatrix44Array (_paramsArray[it->second], values, count);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setMatrix44Array (ValueHandle param, const ATOM_Matrix4x4f *values, unsigned count)
{
	if (param)
	{
		param->setMatrix44Array (values, count);
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setMatrix43 (const char *param, const ATOM_Matrix3x4f &value)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setMatrix43 (_paramsArray[it->second], value);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setMatrix43 (ValueHandle param, const ATOM_Matrix3x4f &value)
{
	if (param)
	{
		param->setMatrix43 (value);
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setMatrix43Array (const char *param, const ATOM_Matrix3x4f *values, unsigned count)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setMatrix43Array (_paramsArray[it->second], values, count);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setMatrix43Array (ValueHandle param, const ATOM_Matrix3x4f *values, unsigned count)
{
	if (param)
	{
		param->setMatrix43Array (values, count);
		writeShaderConstant (param);
		_constantDirty = true;
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setTexture (const char *param, ATOM_Texture *value)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setTexture (_paramsArray[it->second], value);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setTexture (ValueHandle param, ATOM_Texture *value)
{
	if (param)
	{
		param->setTexture (value);
		return true;
	}
	return false;
}

bool ATOM_ParameterTable::setTexture (const char *param, const char *filename)
{
	if (param)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.find (param);
		if (it != _paramTable.end ())
		{
			return setTexture (_paramsArray[it->second], filename);
		}
	}
	return false;
}

bool ATOM_ParameterTable::setTexture (ValueHandle param, const char *filename)
{
	if (param)
	{
		param->setTexture (filename);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////


bool ATOM_ParameterTable::addParameter (const char *name, ATOM_CoreMaterial::ParamHandle param)
{
	if (name && param)
	{
		ValueHandle handle = getValueHandle (name);
		if (!handle)
		{
			int index = param->getParamIndex ();
			if (index >= _paramsArray.size())
			{
				_paramsArray.resize(index+1);
			}
			handle = ATOM_NEW(ATOM_EffectParameterValue, name, param);
			handle->handle = param;
			_paramTable[name] = index;
			_paramsArray[index] = handle;
			_constantDirty = true;
			return true;
		}
	}
	return false;
}

void ATOM_ParameterTable::clear (void)
{
	_constantDirty = true;
	_paramTable.clear ();
	for (int i = 0; i < _paramsArray.size(); ++i)
	{
		ATOM_DELETE(_paramsArray[i]);
	}
	_paramsArray.clear ();

	_shaderConstantMirrorVS.clear ();
	_shaderConstantMirrorPS.clear ();
}

bool ATOM_ParameterTable::hasSameParametersAs (ATOM_ParameterTable *other) const
{
	if (_paramTable.size() != other->_paramTable.size())
	{
		return false;
	}

	for (ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.begin(); it != _paramTable.end(); ++it)
	{
		ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it2 = other->_paramTable.find(it->first);
		if (it2 == other->_paramTable.end ())
		{
			return false;
		}

		ATOM_EffectParameterValue *val1 = getParameter (it->second);
		ATOM_EffectParameterValue *val2 = other->getParameter (it2->second);
		int auto1 = val1->isAutoParameter()?1:0;
		int auto2 = val2->isAutoParameter()?1:0;
		if (auto1 != auto2)
		{
			return false;
		}
		if (!auto1 && !val1->isSameValueAs (*val2))
		{
			return false;
		}
	}

	return true;
}

bool ATOM_ParameterTable::requireFunc_FloatEQ (void *context, void *context2)
{
	return *((ValueHandle)context)->f == *((float*)(&context2));
}

bool ATOM_ParameterTable::requireFunc_IntEQ (void *context, void *context2)
{
	return *((ValueHandle)context)->i == *((int*)(&context2));
}

bool ATOM_ParameterTable::requireFunc_FloatNE (void *context, void *context2)
{
	return *((ValueHandle)context)->f != *((float*)(&context2));
}

bool ATOM_ParameterTable::requireFunc_IntNE (void *context, void *context2)
{
	return *((ValueHandle)context)->i != *((int*)(&context2));
}

bool ATOM_ParameterTable::requireFunc_FloatGE (void *context, void *context2)
{
	return *((ValueHandle)context)->f >= *((float*)(&context2));
}

bool ATOM_ParameterTable::requireFunc_IntGE (void *context, void *context2)
{
	return *((ValueHandle)context)->i >= *((int*)(&context2));
}

bool ATOM_ParameterTable::requireFunc_FloatGT (void *context, void *context2)
{
	return *((ValueHandle)context)->f > *((float*)(&context2));
}

bool ATOM_ParameterTable::requireFunc_IntGT (void *context, void *context2)
{
	return *((ValueHandle)context)->i > *((int*)(&context2));
}

bool ATOM_ParameterTable::requireFunc_FloatLE (void *context, void *context2)
{
	return *((ValueHandle)context)->f <= *((float*)(&context2));
}

bool ATOM_ParameterTable::requireFunc_IntLE (void *context, void *context2)
{
	return *((ValueHandle)context)->i <= *((int*)(&context2));
}

bool ATOM_ParameterTable::requireFunc_FloatLT (void *context, void *context2)
{
	return *((ValueHandle)context)->f < *((float*)(&context2));
}

bool ATOM_ParameterTable::requireFunc_IntLT (void *context, void *context2)
{
	return *((ValueHandle)context)->i < *((int*)(&context2));
}

ATOM_AUTOPTR(ATOM_ParameterTable) ATOM_ParameterTable::clone (void) const
{
	ATOM_AUTOPTR(ATOM_ParameterTable) paramTable = ATOM_NEW(ATOM_ParameterTable);

	paramTable->_paramTable = _paramTable;
	paramTable->_paramsArray.resize (_paramsArray.size());
	for (int i = 0; i < _paramsArray.size(); ++i)
	{
		paramTable->_paramsArray[i] = ATOM_NEW(ATOM_EffectParameterValue, *_paramsArray[i]);
	}

	paramTable->_coreMaterial = _coreMaterial;

	if (_coreMaterial)
	{
		paramTable->_effectValidationInfos.resize (_coreMaterial->getNumEffects ());
		for (unsigned i = 0; i < paramTable->_effectValidationInfos.size(); ++i)
		{
			paramTable->createValidationInfo (&paramTable->_effectValidationInfos[i], _coreMaterial->getEffect (i));
		}
	}

	paramTable->_shaderConstantMirrorVS = _shaderConstantMirrorVS;
	paramTable->_shaderConstantMirrorPS = _shaderConstantMirrorPS;
	paramTable->_currentPass = 0;

	return paramTable;
}

//--- wangjian added ---//
bool ATOM_ParameterTable::isDirty( ATOM_RenderScheme* scheme/* = 0*/, int layer/* = -1*/ )
{
	if( scheme == 0 && layer < 0 )
	{
		scheme = ATOM_RenderScheme::getCurrentRenderScheme();
		layer = ATOM_RenderScheme::getCurrentRenderSchemeLayer();
	}
	if( scheme == 0 || layer < 0 )
		return true;

	//===================================================================================================================//
	ATOM_MAP<ATOM_RenderSchemeFactory*,ATOM_RenderScheme*>::iterator iter = _default_schemes.find( scheme->getFactory() );
	if( iter == _default_schemes.end() )
	{
		_default_schemes.insert( std::make_pair( scheme->getFactory(), scheme ) );
		setDirty(1,scheme,layer);
	}
	else if( iter->second != scheme )
	{
		setDirty(1,scheme,layer);
		//return true;
	}
	//===================================================================================================================//

	//unsigned id = ATOM_RenderScheme::getSchemeMaskID(ATOM_RenderScheme::getCurrentRenderScheme()->getName());
	unsigned id = ATOM_RenderScheme::getSchemeLayerMaskID(	scheme->getFactory(), layer	);

	return ( _dirtyFlag & id ); 
}
void ATOM_ParameterTable::setDirty( int dirty, ATOM_RenderScheme* scheme/* = 0*/, int layer/* = -1*/ )
{
	//unsigned id = ATOM_RenderScheme::getSchemeMaskID(ATOM_RenderScheme::getCurrentRenderScheme()->getName());

	if( scheme == 0 && layer < 0 )
	{
		scheme = ATOM_RenderScheme::getCurrentRenderScheme();
		layer = ATOM_RenderScheme::getCurrentRenderSchemeLayer();
	}
	if( scheme == 0 || layer < 0 )
		return;

	unsigned id = ATOM_RenderScheme::getSchemeLayerMaskID(	scheme->getFactory(),
															layer	);

	if( dirty  )
	{
		_dirtyFlag |= id;
	}
	else
	{
		if( _dirtyFlag & id )
			_dirtyFlag &= ~id;
	}
}
/*
void ATOM_ParameterTable::writeShaderConstant (ValueHandle param, const void *data, unsigned regCount)
{
	if (_currentPass)
	{
		int paramIndex = param->handle->getParamIndex();

		const int *locationsVS = _currentPass->getShaderParamLocationsVS();
		if (locationsVS)
		{
			int regIndex = locationsVS[paramIndex];
			if (regIndex >= 0)
			{
				memcpy (&_shaderConstantMirrorVS[regIndex], data, regCount*sizeof(float)*4);
			}
		}

		const int *locationsPS = _currentPass->getShaderParamLocationsPS();
		if (locationsPS)
		{
			int regIndex = locationsPS[paramIndex];
			if (regIndex >= 0)
			{
				memcpy (&_shaderConstantMirrorPS[regIndex], data, regCount*sizeof(float)*4);
			}
		}
	}
}
*/
void ATOM_ParameterTable::commitShaderConstants (ATOM_RenderDevice *device)
{
	if (_currentPass)
	{
		unsigned vsIndex = _currentPass->getMinRegisterIndexVS();
		unsigned vsCount = _currentPass->getRegisterCountVS();
		if (vsCount)
		{
			device->setConstantsVS (vsIndex, &_shaderConstantMirrorVS[vsIndex], vsCount);
		}

		unsigned psIndex = _currentPass->getMinRegisterIndexPS();
		unsigned psCount = _currentPass->getRegisterCountPS();
		if (psCount)
		{
			device->setConstantsPS (psIndex, &_shaderConstantMirrorPS[psIndex], psCount);
		}
	}
}

void ATOM_ParameterTable::evalValue (ValueHandle param)
{
	ATOM_MaterialParam::ValueCallback callback = param->handle->getValueCallback ();
	if (callback)
	{
		unsigned flags = param->handle->getFlags();
		ATOM_Vector4f *shaderValuePtrVS = 0;
		ATOM_Vector4f *shaderValuePtrPS = 0;
		const ATOM_Shader::ParamDesc *descVS = 0;
		const ATOM_Shader::ParamDesc *descPS = 0;

		if (_currentPass)
		{
			int paramIndex = param->handle->getParamIndex();

			descVS = _currentPass->getShaderParamDescVS();
			if (descVS)
			{
				descVS += paramIndex;
				if (descVS->registerCount)
				{
					shaderValuePtrVS = &_shaderConstantMirrorVS[descVS->registerIndex];
				}
			}
			descPS = _currentPass->getShaderParamDescPS();
			if (descPS)
			{
				descPS += paramIndex;
				if (descPS->registerCount)
				{
					shaderValuePtrPS = &_shaderConstantMirrorPS[descPS->registerIndex];
				}
			}
		}

		callback (param, shaderValuePtrVS, shaderValuePtrPS, descVS, descPS);
	}
}

void ATOM_ParameterTable::updateShaderConstantMirrors (void)
{
	if (_currentPass /*&& _constantDirty*/)
	{
		_constantDirty = false;

		for (int i = 0; i < _paramsArray.size(); ++i)
		{
			ValueHandle param = _paramsArray[i];
			if (param->handle->getValueCallback())
			{
				continue;
			}

			int paramIndex = param->handle->getParamIndex();

			const ATOM_Shader::ParamDesc *descVS = _currentPass->getShaderParamDescVS();
			if (descVS)
			{
				descVS += paramIndex;
				if (descVS->registerCount)
				{
					param->handle->writeRegisterValue (&_shaderConstantMirrorVS[descVS->registerIndex], param->p, descVS);
				}
			}

			const ATOM_Shader::ParamDesc *descPS = _currentPass->getShaderParamDescPS();
			if (descPS)
			{
				descPS += paramIndex;
				if (descPS->registerCount)
				{
					param->handle->writeRegisterValue (&_shaderConstantMirrorPS[descPS->registerIndex], param->p, descPS);
				}
			}
		}
	}
}

void ATOM_ParameterTable::resetDirtyFlag()
{
	_dirtyFlag = 0xffffffff;
}

bool ATOM_ParameterTable::getParameterFrom( ATOM_ParameterTable * other ) const
{
	if( !other )
		return false;

	for (ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _paramTable.begin(); it != _paramTable.end(); ++it)
	{
		ATOM_EffectParameterValue *val = _paramsArray[it->second];
		if( val->handle->getEditorType() == ATOM_MaterialParam::ParamEditorType_Disable )
			continue;

		ValueHandle paramvalue_other = other->getValueHandle(it->first.c_str());
		if( paramvalue_other )
		{
			ATOM_MaterialParam * handle = val->handle;
			switch (handle->getParamType())
			{
			case ATOM_MaterialParam::ParamType_Texture:
				{
					val->t = paramvalue_other->t;
					if (val->t)
					{
						val->t->addRef ();
					}
					val->textureFileName = paramvalue_other->textureFileName;
					break;
				}
			case ATOM_MaterialParam::ParamType_Sampler:
				val->p = 0;
				break;
			default:
				val->dimension = handle->getDimension();
				memcpy (val->p, paramvalue_other->p, val->dimension * handle->getValueSize() );
				break;
			}
		}
	}

	return true;
}
//----------------------//

