#include "stdafx.h"

static void encodeMatrix(const ATOM_Matrix4x4f &Matrix, char* TheString)
{
  sprintf (TheString, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f",
    Matrix.m00, Matrix.m01, Matrix.m02, Matrix.m03,
    Matrix.m10, Matrix.m11, Matrix.m12, Matrix.m13,
    Matrix.m20, Matrix.m21, Matrix.m22, Matrix.m23,
    Matrix.m30, Matrix.m31, Matrix.m32, Matrix.m33);
}
static bool decodeMatrix(const char* TheString, ATOM_Matrix4x4f& Matrix)
{
  return 16 == sscanf (TheString, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
    &Matrix.m00, &Matrix.m01, &Matrix.m02, &Matrix.m03,
    &Matrix.m10, &Matrix.m11, &Matrix.m12, &Matrix.m13,
    &Matrix.m20, &Matrix.m21, &Matrix.m22, &Matrix.m23,
    &Matrix.m30, &Matrix.m31, &Matrix.m32, &Matrix.m33);
}

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Object)
  ATOM_ATTRIBUTES_BEGIN(ATOM_Object)
    ATOM_ATTRIBUTE_READONLY(ATOM_Object, "ClassName", att_GetClassName, "group=ATOM_Object;desc='类名'")
    ATOM_ATTRIBUTE_READONLY(ATOM_Object, "ObjectName", att_GetObjectName, "group=ATOM_Object;desc='对象名'")
  ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_ROOT_END(ATOM_Object)

ATOM_Object::ATOM_Object (void) 
{
	ATOM_STACK_TRACE(ATOM_Object::ATOM_Object);
}

ATOM_Object::~ATOM_Object (void) 
{
	ATOM_STACK_TRACE(ATOM_Object::~ATOM_Object);
	setClassName(0);
	setObjectName(0);
}

const char* ATOM_Object::getObjectName (void) const 
{
	ATOM_STACK_TRACE(ATOM_Object::getObjectName);
	return _M_object_name.empty () ? 0 : _M_object_name.c_str();
}

const char* ATOM_Object::getClassName (void) const 
{
	ATOM_STACK_TRACE(ATOM_Object::getClassName);
	return _M_class_name;
}

void ATOM_Object::setObjectName(const char* name) 
{
	if (name && name[0])
	{
		_M_object_name = name;
	}
	else
	{
		_M_object_name.clear();
	}
}

void ATOM_Object::setClassName(const char* name) 
{
    _M_class_name = name;
}

ATOM_STRING ATOM_Object::att_GetClassName () const 
{
	return _M_class_name;
}

const ATOM_STRING &ATOM_Object::att_GetObjectName () const 
{
  return _M_object_name;
}

bool ATOM_Object::loadAttribute(const ATOM_TiXmlElement *pXmlElement)
{
	ATOM_STACK_TRACE(ATOM_Object::loadAttribute);
	int nAttrCount = ATOM_GetAttribCount(_M_class_name);
	for(int i = 0; i < nAttrCount; i++)
	{
		const char* szPropName = ATOM_GetAttribName(_M_class_name, i);
		if(!ATOM_IsAttribPersistent(_M_class_name, szPropName))		//不需要序列化，则不用XML读入.
		{
			continue;
		}

		ATOM_ScriptVar value;
		ATOM_GetAttribValue(this, szPropName, value);
		switch(value.getType())
		{
		case TYPE_INT:
			{
				int nValue;
				if(pXmlElement && pXmlElement->Attribute(szPropName, &nValue))
				{
					value.asInteger() = nValue;
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_FLOAT:
			{
				double dbValue;
				if(pXmlElement && pXmlElement->Attribute(szPropName, &dbValue))
				{
					value.asFloat() = (float)dbValue;
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_STRING:
			{
				const char* szValue = pXmlElement ? pXmlElement->Attribute(szPropName) : NULL;
				if(szValue)
				{
					value.asString() = szValue;
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_VEC2:
			{
				const char* szValue = pXmlElement ? pXmlElement->Attribute(szPropName) : NULL;
				if(szValue)
				{
					ATOM_Vector2f v2;
					sscanf(szValue, "%f,%f", &v2.x, &v2.y);
					value.asVector2() = v2;
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_VEC3:
			{
				const char* szValue = pXmlElement ? pXmlElement->Attribute(szPropName) : NULL;
				if(szValue)
				{
					ATOM_Vector3f v3;
					sscanf(szValue, "%f,%f,%f", &v3.x, &v3.y, &v3.z);
					value.asVector3() = v3;
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_VEC4:
			{
				const char* szValue = pXmlElement ? pXmlElement->Attribute(szPropName) : NULL;
				if(szValue)
				{
					ATOM_Vector4f v4;
					sscanf(szValue, "%f,%f,%f,%f", &v4.x, &v4.y, &v4.z, &v4.w);
					value.asVector4() = v4;
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_MAT33:
			{
				const char* szValue = pXmlElement ? pXmlElement->Attribute(szPropName) : NULL;
				if(szValue)
				{
					ATOM_Matrix3x3f m33;
					sscanf(szValue, "%f,%f,%f,%f,%f,%f,%f,%f,%f"
					, &m33(0, 0), &m33(0, 1), &m33(0, 2)
					, &m33(1, 0), &m33(1, 1), &m33(1, 2)
					, &m33(2, 0), &m33(2, 1), &m33(2, 2));
					value.asMatrix33() = m33;
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_MAT44:
			{
				const char* szValue = pXmlElement ? pXmlElement->Attribute(szPropName) : NULL;
				if(szValue)
				{
					ATOM_Matrix4x4f m44;
					decodeMatrix(szValue, m44);
					value.asMatrix44() = m44;
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_OBJECT:
			{
				const ATOM_TiXmlElement* pXmlObj = pXmlElement ? pXmlElement->FirstChildElement(szPropName) : NULL;
				const char*  szClassName = 0;
				if(pXmlObj && (szClassName = pXmlObj->Attribute("class")))
				{
					ATOM_HARDREF(ATOM_Object) obj(szClassName, 0);
					if (obj)
					{
						obj->loadAttribute(pXmlObj);
					}
					value.asObject() = obj;
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_INT_ARRAY:
			{
				const ATOM_TiXmlElement* pXmlInt = pXmlElement ? pXmlElement->FirstChildElement(szPropName) : NULL;
				if (pXmlInt)
				{
					while(pXmlInt)
					{
						int nValue;
						pXmlInt->Attribute("value", &nValue);
						value.asIntegerArray().push_back(nValue);
						pXmlInt = pXmlInt->NextSiblingElement(szPropName);
					}
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_FLOAT_ARRAY:
			{
				const ATOM_TiXmlElement* pXmlFloat = pXmlElement ? pXmlElement->FirstChildElement(szPropName) : NULL;
				if (pXmlFloat)
				{
					while(pXmlFloat)
					{
						double dbValue;
						pXmlFloat->Attribute("value", &dbValue);
						value.asFloatArray().push_back((float)dbValue);
						pXmlFloat = pXmlFloat->NextSiblingElement(szPropName);
					}
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_STRING_ARRAY:
			{
				const ATOM_TiXmlElement* pXmlString = pXmlElement ? pXmlElement->FirstChildElement(szPropName) : NULL;
				if (pXmlString)
				{
					while(pXmlString)
					{
						const char* szValue = pXmlString->Attribute("value");
						if(szValue)
						{
							value.asStringArray().push_back(ATOM_STRING(szValue));
						}
						pXmlString = pXmlString->NextSiblingElement(szPropName);
					}
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_VEC2_ARRAY:
			{
				const ATOM_TiXmlElement* pXmlV2 = pXmlElement ? pXmlElement->FirstChildElement(szPropName) : NULL;
				if (pXmlV2)
				{
					while(pXmlV2)
					{
						const char* szValue = pXmlV2->Attribute("value");
						if(szValue)
						{
							ATOM_Vector2f v2Value;
							sscanf(szValue, "%f,%f", &v2Value.x, &v2Value.y);
							value.asVector2Array().push_back(v2Value);
						}
						pXmlV2 = pXmlV2->NextSiblingElement(szPropName);
					}
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_VEC3_ARRAY:
			{
				const ATOM_TiXmlElement* pXmlV3 = pXmlElement ? pXmlElement->FirstChildElement(szPropName) : NULL;
				if (pXmlV3)
				{
					while(pXmlV3)
					{
						const char* szValue = pXmlV3->Attribute("value");
						if(szValue)
						{
							ATOM_Vector3f v3Value;
							sscanf(szValue, "%f,%f,%f", &v3Value.x, &v3Value.y, &v3Value.z);
							value.asVector3Array().push_back(v3Value);
						}
						pXmlV3 = pXmlV3->NextSiblingElement(szPropName);
					}
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_VEC4_ARRAY:
			{
				const ATOM_TiXmlElement* pXmlV4 = pXmlElement ? pXmlElement->FirstChildElement(szPropName) : NULL;
				if (pXmlV4)
				{
					while(pXmlV4)
					{
						const char* szValue = pXmlV4->Attribute("value");
						if(szValue)
						{
							ATOM_Vector4f v4Value;
							sscanf(szValue, "%f,%f,%f,%f", &v4Value.x, &v4Value.y, &v4Value.z, &v4Value.w);
							value.asVector4Array().push_back(v4Value);
						}
						pXmlV4 = pXmlV4->NextSiblingElement(szPropName);
					}
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_MAT33_ARRAY:
			{
				const ATOM_TiXmlElement* pXmlM33 = pXmlElement ? pXmlElement->FirstChildElement(szPropName) : NULL;
				if (pXmlM33)
				{
					while(pXmlM33)
					{
						const char* szValue = pXmlM33->Attribute("value");
						if(szValue)
						{
							ATOM_Matrix3x3f m33Value;
							sscanf(szValue, "%f,%f,%f,%f,%f,%f,%f,%f,%f"
							, &m33Value(0, 0), &m33Value(0, 1), &m33Value(0, 2)
							, &m33Value(1, 0), &m33Value(1, 1), &m33Value(1, 2)
							, &m33Value(2, 0), &m33Value(2, 1), &m33Value(2, 2));
							value.asMatrix33Array().push_back(m33Value);
						}
						pXmlM33 = pXmlM33->NextSiblingElement(szPropName);
					}
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_MAT44_ARRAY:
			{
				const ATOM_TiXmlElement* pXmlM44 = pXmlElement ? pXmlElement->FirstChildElement(szPropName) : NULL;
				if (pXmlM44)
				{
					while(pXmlM44)
					{
						const char* szValue = pXmlM44->Attribute("value");
						if(szValue)
						{
							ATOM_Matrix4x4f m44Value;
							decodeMatrix(szValue, m44Value);
							value.asMatrix44Array().push_back(m44Value);
						}
						pXmlM44 = pXmlM44->NextSiblingElement(szPropName);
					}
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		case TYPE_OBJECT_ARRAY:
			{
				const ATOM_TiXmlElement* pXmlObj = pXmlElement ? pXmlElement->FirstChildElement(szPropName) : NULL;
				if (pXmlObj)
				{
					while(pXmlObj)
					{
						const char* szClassName = pXmlObj->Attribute("class");
						if(szClassName)
						{
							ATOM_HARDREF(ATOM_Object) obj(szClassName, 0);
							if(obj)
							{
								obj->loadAttribute(pXmlObj);
								value.asObjectArray().push_back(obj);
							}
						}						
						pXmlObj = pXmlObj->NextSiblingElement(szPropName);
					}
					ATOM_SetAttribValue(this, szPropName, value);
				}
				else if (ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName))
				{
					ATOM_GetDefaultAttribValue (_M_class_name, szPropName, value);
					ATOM_SetAttribValue(this, szPropName, value);
				}
			}
			break;
		}
	}
	return true;
}

void ATOM_Object::dumpAttributes (const char *indent, const char *attrib)
{
	ATOM_STACK_TRACE(ATOM_Object::dumpAttributes);
	char szBuffer[256];
	int nAttrCount = ATOM_GetAttribCount(_M_class_name);
	for(int i = 0; i < nAttrCount; i++)
	{
		const char* szPropName = ATOM_GetAttribName(_M_class_name, i);
		if (attrib && stricmp(szPropName, attrib))
		{
			continue;
		}

		ATOM_ScriptVar value, defaultValue;
		bool hasDefaultValue = ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName);

		ATOM_GetAttribValue(this, szPropName, value);

		switch(value.getType())
		{
		case TYPE_INT:
			ATOM_LOGGER::log ("%s%s = %d\n", indent, szPropName, value.asInteger());
			break;
		case TYPE_FLOAT:
			ATOM_LOGGER::log ("%s%s = %f\n", indent, szPropName, value.asFloat());
			break;
		case TYPE_STRING:
			ATOM_LOGGER::log ("%s%s = \"%s\"\n", indent, szPropName, value.asString().c_str());
			break;
		case TYPE_VEC2:
			ATOM_LOGGER::log ("%s%s = (%f,%f)\n", indent, szPropName, value.asVector2().x, value.asVector2().y);
			break;
		case TYPE_VEC3:
			ATOM_LOGGER::log ("%s%s = (%f,%f,%f)\n", indent, szPropName, value.asVector3().x, value.asVector3().y, value.asVector3().z);
			break;
		case TYPE_VEC4:
			ATOM_LOGGER::log ("%s%s = (%f,%f,%f,%f)\n", indent, szPropName, value.asVector4().x, value.asVector4().y, value.asVector4().z, value.asVector4().w);
			break;
		case TYPE_MAT33:
			{
				ATOM_Matrix3x3f& m33 = value.asMatrix33();
				sprintf(szBuffer, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f"
				, m33(0, 0), m33(0, 1), m33(0, 2)
				, m33(1, 0), m33(1, 1), m33(1, 2)
				, m33(2, 0), m33(2, 1), m33(2, 2));
				ATOM_LOGGER::log ("%s%s = (%s)\n", indent, szPropName, szBuffer);
			}
			break;
		case TYPE_MAT44:
			{
				ATOM_Matrix4x4f& m44 = value.asMatrix44();
				sprintf(szBuffer, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f"
				, m44(0, 0), m44(0, 1), m44(0, 2), m44(0,3)
				, m44(1, 0), m44(1, 1), m44(1, 2), m44(1,3)
				, m44(2, 0), m44(2, 1), m44(2, 2), m44(2,3)
				, m44(3, 0), m44(3, 1), m44(3, 2), m44(3,3));
				ATOM_LOGGER::log ("%s%s = (%s)\n", indent, szPropName, szBuffer);
			}
			break;
		case TYPE_OBJECT:
			{
				ATOM_AUTOREF(ATOM_Object)& obj = value.asObject();
				ATOM_LOGGER::log ("%s%s = 0x%08X(%s,%s)\n", indent, szPropName, obj.pointer, obj ? obj->getClassName() : 0, obj ? obj->getObjectName() : 0);
				if (obj)
				{
					ATOM_STRING s = indent;
					s += "\t";
					obj->dumpAttributes (s.c_str());
				}
			}
			break;
		case TYPE_INT_ARRAY:
			{
				ATOM_VECTOR<int>& vtInt = value.asIntegerArray();
				ATOM_LOGGER::log ("%s%s = Integer array (%d elements)\n", indent, szPropName, vtInt.size());
				for(unsigned j = 0; j < vtInt.size(); ++j)
				{
					ATOM_LOGGER::log ("%s\t%s[%d] = %d\n", indent, szPropName, j, vtInt[j]);
				}
			}
			break;
		case TYPE_FLOAT_ARRAY:
			{
				ATOM_VECTOR<float>& vtFloat = value.asFloatArray();
				ATOM_LOGGER::log ("%s%s = Float array (%d elements)\n", indent, szPropName, vtFloat.size());
				for(unsigned j = 0; j < vtFloat.size(); ++j)
				{
					ATOM_LOGGER::log ("%s\t%s[%d] = %f\n", indent, szPropName, j, vtFloat[j]);
				}
			}
			break;
		case TYPE_STRING_ARRAY:
			{
				ATOM_VECTOR<ATOM_STRING>& vtStr = value.asStringArray();
				ATOM_LOGGER::log ("%s%s = String array (%d elements)\n", indent, szPropName, vtStr.size());
				for(unsigned j = 0; j < vtStr.size(); ++j)
				{
					ATOM_LOGGER::log ("%s\t%s[%d] = \"%s\"\n", indent, szPropName, j, vtStr[j].c_str());
				}
			}
			break;
		case TYPE_VEC2_ARRAY:
			{
				ATOM_VECTOR<ATOM_Vector2f>& vtV2 = value.asVector2Array();
				ATOM_LOGGER::log ("%s%s = Vector2 array (%d elements)\n", indent, szPropName, vtV2.size());
				for(unsigned j = 0; j < vtV2.size(); ++j)
				{
					ATOM_LOGGER::log ("%s\t%s[%d] = (%f,%f)\n", indent, szPropName, j, vtV2[j].x, vtV2[j].y);
				}
			}
			break;
		case TYPE_VEC3_ARRAY:
			{
				ATOM_VECTOR<ATOM_Vector3f>& vtV3 = value.asVector3Array();
				ATOM_LOGGER::log ("%s%s = Vector3 array (%d elements)\n", indent, szPropName, vtV3.size());
				for(unsigned j = 0; j < vtV3.size(); ++j)
				{
					ATOM_LOGGER::log ("%s\t%s[%d] = (%f,%f,%f)\n", indent, szPropName, j, vtV3[j].x, vtV3[j].y, vtV3[j].z);
				}
			}
			break;
		case TYPE_VEC4_ARRAY:
			{
				ATOM_VECTOR<ATOM_Vector4f>& vtV4 = value.asVector4Array();
				ATOM_LOGGER::log ("%s%s = Vector4 array (%d elements)\n", indent, szPropName, vtV4.size());
				for(unsigned j = 0; j < vtV4.size(); ++j)
				{
					ATOM_LOGGER::log ("%s\t%s[%d] = (%f,%f,%f,%f)\n", indent, szPropName, j, vtV4[j].x, vtV4[j].y, vtV4[j].z, vtV4[j].w);
				}
			}
			break;
		case TYPE_MAT33_ARRAY:
			{
				ATOM_VECTOR<ATOM_Matrix3x3f>& vtM33 = value.asMatrix33Array();
				ATOM_LOGGER::log ("%s%s = Matrix33 array (%d elements)\n", indent, szPropName, vtM33.size());
				for(unsigned j = 0; j < vtM33.size(); ++j)
				{
					sprintf(szBuffer, "%f,%f,%f,%f,%f,%f,%f,%f,%f"
					, vtM33[j](0, 0), vtM33[j](0, 1), vtM33[j](0, 2)
					, vtM33[j](1, 0), vtM33[j](1, 1), vtM33[j](1, 2)
					, vtM33[j](2, 0), vtM33[j](2, 1), vtM33[j](2, 2));
					ATOM_LOGGER::log ("%s\t%s[%d] = (%s)\n", indent, szPropName, j, szBuffer);
				}
			}
			break;
		case TYPE_MAT44_ARRAY:
			{
				ATOM_VECTOR<ATOM_Matrix4x4f>& vtM44 = value.asMatrix44Array();
				ATOM_LOGGER::log ("%s%s = Matrix44 array (%d elements)\n", indent, szPropName, vtM44.size());
				for(unsigned j = 0; j < vtM44.size(); ++j)
				{
					sprintf(szBuffer, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f"
					, vtM44[j](0, 0), vtM44[j](0, 1), vtM44[j](0, 2), vtM44[j](0,3)
					, vtM44[j](1, 0), vtM44[j](1, 1), vtM44[j](1, 2), vtM44[j](1,3)
					, vtM44[j](2, 0), vtM44[j](2, 1), vtM44[j](2, 2), vtM44[j](2,3)
					, vtM44[j](3, 0), vtM44[j](3, 1), vtM44[j](3, 2), vtM44[j](3,3));
					ATOM_LOGGER::log ("%s\t%s[%d] = (%s)\n", indent, szPropName, j, szBuffer);
				}
			}
			break;
		case TYPE_OBJECT_ARRAY:
			{
				ATOM_VECTOR<ATOM_AUTOREF(ATOM_Object) >& vtObj = value.asObjectArray();
				ATOM_LOGGER::log ("%s%s = Object array (%d elements)\n", indent, szPropName, vtObj.size());
			}
			break;
		}
	}
}

bool ATOM_Object::writeAttribute(ATOM_TiXmlElement *pXmlElement)
{
	ATOM_STACK_TRACE(ATOM_Object::writeAttribute);
	pXmlElement->SetAttribute("class", _M_class_name);
	if(!_M_object_name.empty())
	{
		pXmlElement->SetAttribute("name", _M_object_name.c_str());
	}

	char szBuffer[256];
	int nAttrCount = ATOM_GetAttribCount(_M_class_name);
	for(int i = 0; i < nAttrCount; i++)
	{
		const char* szPropName = ATOM_GetAttribName(_M_class_name, i);
		if(!ATOM_IsAttribPersistent(_M_class_name, szPropName))		//不需要序列化，则不需写入XML.
		{
			continue;
		}

		ATOM_ScriptVar value, defaultValue;
		bool hasDefaultValue = ATOM_IsAttribHasDefaultValue (_M_class_name, szPropName);

		ATOM_GetAttribValue(this, szPropName, value);
		if (hasDefaultValue)
		{
			ATOM_GetDefaultAttribValue(_M_class_name, szPropName, defaultValue);
		}

		switch(value.getType())
		{
		case TYPE_INT:
			if(value != defaultValue || !hasDefaultValue)
			{
				pXmlElement->SetAttribute(szPropName, value.asInteger());
			}
			break;
		case TYPE_FLOAT:
			if(value != defaultValue || !hasDefaultValue)
			{
				sprintf(szBuffer, "%f", value.asFloat());
				pXmlElement->SetAttribute(szPropName, szBuffer);
			}
			break;
		case TYPE_STRING:
			if(value != defaultValue || !hasDefaultValue)
			{
				pXmlElement->SetAttribute(szPropName, value.asString().c_str());
			}
			break;
		case TYPE_VEC2:
			if(value != defaultValue || !hasDefaultValue)
			{
				ATOM_Vector2f& v2 = value.asVector2();
				sprintf(szBuffer, "%f, %f", v2.x, v2.y);
				pXmlElement->SetAttribute(szPropName, szBuffer);
			}
			break;
		case TYPE_VEC3:
			if(value != defaultValue || !hasDefaultValue)
			{
				ATOM_Vector3f& v3 = value.asVector3();
				sprintf(szBuffer, "%f, %f, %f", v3.x, v3.y, v3.z);
				pXmlElement->SetAttribute(szPropName, szBuffer);
			}
			break;
		case TYPE_VEC4:
			if(value != defaultValue || !hasDefaultValue)
			{
				ATOM_Vector4f& v4 = value.asVector4();
				sprintf(szBuffer, "%f, %f, %f, %f", v4.x, v4.y, v4.z, v4.w);
				pXmlElement->SetAttribute(szPropName, szBuffer);
			}
			break;
		case TYPE_MAT33:
			if(value != defaultValue || !hasDefaultValue)
			{
				ATOM_Matrix3x3f& m33 = value.asMatrix33();
				sprintf(szBuffer, "%f, %f, %f, %f, %f, %f, %f, %f, %f"
				, m33(0, 0), m33(0, 1), m33(0, 2)
				, m33(1, 0), m33(1, 1), m33(1, 2)
				, m33(2, 0), m33(2, 1), m33(2, 2));
				pXmlElement->SetAttribute(szPropName, szBuffer);
			}
			break;
		case TYPE_MAT44:
			if(value != defaultValue || !hasDefaultValue)
			{
				ATOM_Matrix4x4f& m44 = value.asMatrix44();
				encodeMatrix(m44, szBuffer);
				pXmlElement->SetAttribute(szPropName, szBuffer);
			}
			break;
		case TYPE_OBJECT:
			if(value != defaultValue || !hasDefaultValue)
			{
				ATOM_AUTOREF(ATOM_Object)& obj = value.asObject();
				if (obj)
				{
					ATOM_TiXmlElement o(szPropName);
					obj->writeAttribute(&o);
					pXmlElement->InsertEndChild(o);
				}
			}
			break;
		case TYPE_INT_ARRAY:
			{
				ATOM_VECTOR<int>& vtInt = value.asIntegerArray();
				for(unsigned j = 0; j < vtInt.size(); ++j)
				{
					ATOM_TiXmlElement p(szPropName);
					p.SetAttribute("value", vtInt[j]);
					pXmlElement->InsertEndChild(p);
				}
			}
			break;
		case TYPE_FLOAT_ARRAY:
			{
				ATOM_VECTOR<float>& vtFloat = value.asFloatArray();
				for(unsigned j = 0; j < vtFloat.size(); ++j)
				{
					ATOM_TiXmlElement p(szPropName);
					sprintf(szBuffer, "%f", vtFloat[j]);
					p.SetAttribute("value", szBuffer);
					pXmlElement->InsertEndChild(p);
				}
			}
			break;
		case TYPE_STRING_ARRAY:
			{
				ATOM_VECTOR<ATOM_STRING>& vtString = value.asStringArray();
				for(unsigned j = 0; j < vtString.size(); ++j)
				{
					ATOM_TiXmlElement p(szPropName);
					p.SetAttribute("value", vtString[j].c_str());
					pXmlElement->InsertEndChild(p);
				}
			}
			break;
		case TYPE_VEC2_ARRAY:
			{
				ATOM_VECTOR<ATOM_Vector2f>& vtV2 = value.asVector2Array();
				for(unsigned j = 0; j < vtV2.size(); ++j)
				{
					ATOM_TiXmlElement p(szPropName);
					sprintf(szBuffer, "%f, %f", vtV2[j].x, vtV2[j].y);
					p.SetAttribute("value", szBuffer);
					pXmlElement->InsertEndChild(p);
				}
			}
			break;
		case TYPE_VEC3_ARRAY:
			{
				ATOM_VECTOR<ATOM_Vector3f>& vtV3 = value.asVector3Array();
				for(unsigned j = 0; j < vtV3.size(); ++j)
				{
					ATOM_TiXmlElement p(szPropName);
					sprintf(szBuffer, "%f, %f, %f", vtV3[j].x, vtV3[j].y, vtV3[j].z);
					p.SetAttribute("value", szBuffer);
					pXmlElement->InsertEndChild(p);
				}
			}
			break;
		case TYPE_VEC4_ARRAY:
			{
				ATOM_VECTOR<ATOM_Vector4f>& vtV4 = value.asVector4Array();
				for(unsigned j = 0; j < vtV4.size(); ++j)
				{
					ATOM_TiXmlElement p(szPropName);
					sprintf(szBuffer, "%f, %f, %f, %f", vtV4[j].x, vtV4[j].y, vtV4[j].z, vtV4[j].w);
					p.SetAttribute("value", szBuffer);
					pXmlElement->InsertEndChild(p);
				}
			}
			break;
		case TYPE_MAT33_ARRAY:
			{
				ATOM_VECTOR<ATOM_Matrix3x3f>& vtM33 = value.asMatrix33Array();
				for(unsigned j = 0; j < vtM33.size(); ++j)
				{
					ATOM_TiXmlElement p(szPropName);
					sprintf(szBuffer, "%f, %f, %f, %f, %f, %f, %f, %f, %f"
					, vtM33[j](0, 0), vtM33[j](0, 1), vtM33[j](0, 2)
					, vtM33[j](1, 0), vtM33[j](1, 1), vtM33[j](1, 2)
					, vtM33[j](2, 0), vtM33[j](2, 1), vtM33[j](2, 2));
					p.SetAttribute("value", szBuffer);
					pXmlElement->InsertEndChild(p);
				}
			}
			break;
		case TYPE_MAT44_ARRAY:
			{
				ATOM_VECTOR<ATOM_Matrix4x4f>& vtM44 = value.asMatrix44Array();
				for(unsigned j = 0; j < vtM44.size(); ++j)
				{
					ATOM_TiXmlElement p(szPropName);
					encodeMatrix(vtM44[j], szBuffer);
					p.SetAttribute("value", szBuffer);
					pXmlElement->InsertEndChild(p);
				}
			}
			break;
		case TYPE_OBJECT_ARRAY:
			{
				ATOM_VECTOR<ATOM_AUTOREF(ATOM_Object) >& vtObj = value.asObjectArray();
				for(unsigned j = 0; j < vtObj.size(); ++j)
				{
					ATOM_TiXmlElement p(szPropName);
					vtObj[j]->writeAttribute(&p);
					pXmlElement->InsertEndChild(p);
				}
			}
			break;
		}
	}
	return true;
}

void ATOM_Object::copyAttributesTo (ATOM_Object *obj) const
{
	ATOM_STACK_TRACE(ATOM_Object::copyAttributesTo);
	if (!obj)
	{
		return;
	}

	int nAttrCount = ATOM_GetAttribCount(_M_class_name);
	for (unsigned i = 0; i < nAttrCount; ++i)
	{
		const char *name = ATOM_GetAttribName (_M_class_name, i);

		if (!ATOM_FindAttrib (obj->getClassName(), name))
		{
			continue;
		}

		if (ATOM_IsAttribReadonly (obj->getClassName(), name))
		{
			continue;
		}

		ATOM_ScriptVar value;
		if (!ATOM_GetAttribValue (const_cast<ATOM_Object*>(this), name, value))
		{
			continue;
		}

		ATOM_SetAttribValue (obj, name, value);
	}
}

ATOM_WeakPtrList &ATOM_Object::getWeakPtrList (void)
{
	return _M_weakptr_list;
}

