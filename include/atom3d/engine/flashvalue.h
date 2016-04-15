#ifndef ATOM3D_FLASH_FLASHVALUE_H
#define ATOM3D_FLASH_FLASHVALUE_H

#include "../ATOM_dbghlp.h"
#include "../ATOM_utils.h"
#include "basedefs.h"

/**
* FlashValue represents a Flash ActionScript variable. The currently supported types are: null, boolean, number, and string.
*/
class ATOM_ENGINE_API ATOM_FlashValue
{
	ATOM_STRING strValue;
	float numValue;
	bool boolValue;
	short valueType;

public:
	enum FlashType
	{
		FT_NULL,
		FT_BOOLEAN,
		FT_NUMBER,
		FT_STRING
	};

public:
	/**
	* Creates a null FlashValue.
	*/
	ATOM_FlashValue();

	/**
	* Creates a FlashValue as a boolean type.
	*
	* @param	booleanValue	The value to initialize this FlashValue with.
	*/
	ATOM_FlashValue(bool booleanValue);

	/**
	* Creates a FlashValue as a number type.
	*
	* @param	numericValue	The value to initialize this FlashValue with.
	*/
	ATOM_FlashValue(int numericValue);
	
	/**
	* Creates a FlashValue as a number type.
	*
	* @param	numericValue	The value to initialize this FlashValue with.
	*/
	ATOM_FlashValue(float numericValue);

	/**
	* Creates a FlashValue as a string type.
	*
	* @param	stringValue	The value to initialize this FlashValue with.
	*/
	ATOM_FlashValue(const char* stringValue);

	/**
	* Retrieves the FlashType of this FlashValue.
	*/
	short getType() const;

	/**
	* Returns whether or not this FlashValue is of a null type.
	*/
	bool isNull() const;

	/**
	* Sets this FlashValue as a null type.
	*/
	void setNull();

	/**
	* Retrieves the value as a boolean.
	*
	* @note	If the actual value type is FT_BOOLEAN, this directly retrieves the actual value. Otherwise
	*		this function will make an interpretation of the value as that of a boolean. Number values
	*		will be cast to boolean and string values will be parsed lexically ("true" and "false" are valid).
	*		A value type of null will always return false.
	*/
	bool getBool() const;

	/**
	* Retrieves the value as a number.
	*
	* @note	If the actual value type is FT_NUMBER, this directly retrieves the actual value. Otherwise
	*		this function will make an interpretation of the value as that of a number type. Boolean
	*		values will be cast to a number and string values will be parsed lexically. A value type of
	*		null will always return '0'.
	*/
	float getNumber() const;

	/**
	* If this FlashValue is a number type, retrieves the number value interpreted as a color.
	*
	* @note	Color values in ActionScript are generally encoded as a number, hence this function's utility.
	*/
	ATOM_ColorARGB getNumberAsColor() const;

	/**
	* Retrieves the value as a string.
	*
	* @note	If the actual value type is FT_STRING, this directly retrieves the actual value. Otherwise
	*		this function will make an interpretation of the value as that of a string type. Boolean
	*		values will either be "true" or "false", number values will be output in standard form,
	*		and null value types will always return an empty string.
	*/
	ATOM_STRING getString() const;
};

/**
* Arguments is defined as a vector of FlashValues. Used to communicate with ActionScript functions.
*/
typedef ATOM_VECTOR<ATOM_FlashValue> ATOM_FlashArguments;

/**
* Use the FLASH_VOID macro to represent a null FlashValue type.
*/
#define FLASH_VOID ATOM_FlashValue()

/**
* This helper class can be used to quickly declare variable-length FlashValue arguments inline.
*
* Syntax is: \code Args(x)(x)(x)(x)... \endcode
*/
class ATOM_ENGINE_API ATOM_FlashArgs : public ATOM_FlashArguments
{
public:
	ATOM_FlashArgs();

	explicit ATOM_FlashArgs(const ATOM_FlashValue& firstArg);

	ATOM_FlashArgs& operator()(const ATOM_FlashValue& newArg);
};

#endif // ATOM3D_FLASH_FLASHVALUE_H
