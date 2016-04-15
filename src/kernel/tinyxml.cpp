/*
www.sourceforge.net/projects/tinyxml
Original code (2.0 and earlier )copyright (c) 2000-2006 Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "StdAfx.h"
#include <ctype.h>

#ifdef TIXML_USE_STL
#include <sstream>
#include <iostream>
#endif

#include "tinyxml.h"


bool ATOM_TiXmlBase::condenseWhiteSpace = true;

// Microsoft compiler security
FILE* TiXmlFOpen( const char* filename, const char* mode )
{
	#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
		FILE* fp = 0;
		errno_t err = fopen_s( &fp, filename, mode );
		if ( !err && fp )
			return fp;
		return 0;
	#else
		return fopen( filename, mode );
	#endif
}

void ATOM_TiXmlBase::EncodeString( const ATOM_TIXML_STRING& str, ATOM_TIXML_STRING* outString )
{
	int i=0;

	while( i<(int)str.length() )
	{
		unsigned char c = (unsigned char) str[i];

		if (    c == '&' 
		     && i < ( (int)str.length() - 2 )
			 && str[i+1] == '#'
			 && str[i+2] == 'x' )
		{
			// Hexadecimal character reference.
			// Pass through unchanged.
			// &#xA9;	-- copyright symbol, for example.
			//
			// The -1 is a bug fix from Rob Laveaux. It keeps
			// an overflow from happening if there is no ';'.
			// There are actually 2 ways to exit this loop -
			// while fails (error case) and break (semicolon found).
			// However, there is no mechanism (currently) for
			// this function to return an error.
			while ( i<(int)str.length()-1 )
			{
				outString->append( str.c_str() + i, 1 );
				++i;
				if ( str[i] == ';' )
					break;
			}
		}
		else if ( c == '&' )
		{
			outString->append( entity[0].str, entity[0].strLength );
			++i;
		}
		else if ( c == '<' )
		{
			outString->append( entity[1].str, entity[1].strLength );
			++i;
		}
		else if ( c == '>' )
		{
			outString->append( entity[2].str, entity[2].strLength );
			++i;
		}
		else if ( c == '\"' )
		{
			outString->append( entity[3].str, entity[3].strLength );
			++i;
		}
		else if ( c == '\'' )
		{
			outString->append( entity[4].str, entity[4].strLength );
			++i;
		}
		else if ( c < 32 )
		{
			// Easy pass at non-alpha/numeric/symbol
			// Below 32 is symbolic.
			char buf[ 32 ];
			
			#if defined(ATOM_TIXML_SNPRINTF)		
				ATOM_TIXML_SNPRINTF( buf, sizeof(buf), "&#x%02X;", (unsigned) ( c & 0xff ) );
			#else
				sprintf( buf, "&#x%02X;", (unsigned) ( c & 0xff ) );
			#endif		

			//*ME:	warning C4267: convert 'size_t' to 'int'
			//*ME:	Int-Cast to make compiler happy ...
			outString->append( buf, (int)strlen( buf ) );
			++i;
		}
		else
		{
			//char realc = (char) c;
			//outString->append( &realc, 1 );
			*outString += (char) c;	// somewhat more efficient function call.
			++i;
		}
	}
}


ATOM_TiXmlNode::ATOM_TiXmlNode( NodeType _type ) : ATOM_TiXmlBase()
{
	parent = 0;
	type = _type;
	firstChild = 0;
	lastChild = 0;
	prev = 0;
	next = 0;
}


ATOM_TiXmlNode::~ATOM_TiXmlNode()
{
	ATOM_TiXmlNode* node = firstChild;
	ATOM_TiXmlNode* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->next;
		ATOM_POOL_DELETE(temp);
	}	
}


void ATOM_TiXmlNode::CopyTo( ATOM_TiXmlNode* target ) const
{
	target->SetValue (value.c_str() );
	target->userData = userData; 
}


void ATOM_TiXmlNode::Clear()
{
	ATOM_TiXmlNode* node = firstChild;
	ATOM_TiXmlNode* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->next;
		ATOM_POOL_DELETE(temp);
	}	

	firstChild = 0;
	lastChild = 0;
}


ATOM_TiXmlNode* ATOM_TiXmlNode::LinkEndChild( ATOM_TiXmlNode* node )
{
	assert( node->parent == 0 || node->parent == this );
	assert( node->GetDocument() == 0 || node->GetDocument() == this->GetDocument() );

	if ( node->Type() == ATOM_TiXmlNode::DOCUMENT )
	{
		ATOM_POOL_DELETE(node);
		if ( GetDocument() ) GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, ATOM_TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	node->parent = this;

	node->prev = lastChild;
	node->next = 0;

	if ( lastChild )
		lastChild->next = node;
	else
		firstChild = node;			// it was an empty list.

	lastChild = node;
	return node;
}


ATOM_TiXmlNode* ATOM_TiXmlNode::InsertEndChild( const ATOM_TiXmlNode& addThis )
{
	if ( addThis.Type() == ATOM_TiXmlNode::DOCUMENT )
	{
		if ( GetDocument() ) GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, ATOM_TIXML_ENCODING_UNKNOWN );
		return 0;
	}
	ATOM_TiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;

	return LinkEndChild( node );
}


ATOM_TiXmlNode* ATOM_TiXmlNode::InsertBeforeChild( ATOM_TiXmlNode* beforeThis, const ATOM_TiXmlNode& addThis )
{	
	if ( !beforeThis || beforeThis->parent != this ) {
		return 0;
	}
	if ( addThis.Type() == ATOM_TiXmlNode::DOCUMENT )
	{
		if ( GetDocument() ) GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, ATOM_TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	ATOM_TiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;

	node->next = beforeThis;
	node->prev = beforeThis->prev;
	if ( beforeThis->prev )
	{
		beforeThis->prev->next = node;
	}
	else
	{
		assert( firstChild == beforeThis );
		firstChild = node;
	}
	beforeThis->prev = node;
	return node;
}


ATOM_TiXmlNode* ATOM_TiXmlNode::InsertAfterChild( ATOM_TiXmlNode* afterThis, const ATOM_TiXmlNode& addThis )
{
	if ( !afterThis || afterThis->parent != this ) {
		return 0;
	}
	if ( addThis.Type() == ATOM_TiXmlNode::DOCUMENT )
	{
		if ( GetDocument() ) GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, ATOM_TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	ATOM_TiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;

	node->prev = afterThis;
	node->next = afterThis->next;
	if ( afterThis->next )
	{
		afterThis->next->prev = node;
	}
	else
	{
		assert( lastChild == afterThis );
		lastChild = node;
	}
	afterThis->next = node;
	return node;
}


ATOM_TiXmlNode* ATOM_TiXmlNode::ReplaceChild( ATOM_TiXmlNode* replaceThis, const ATOM_TiXmlNode& withThis )
{
	if ( replaceThis->parent != this )
		return 0;

	ATOM_TiXmlNode* node = withThis.Clone();
	if ( !node )
		return 0;

	node->next = replaceThis->next;
	node->prev = replaceThis->prev;

	if ( replaceThis->next )
		replaceThis->next->prev = node;
	else
		lastChild = node;

	if ( replaceThis->prev )
		replaceThis->prev->next = node;
	else
		firstChild = node;

	ATOM_POOL_DELETE(replaceThis);
	node->parent = this;
	return node;
}


bool ATOM_TiXmlNode::RemoveChild( ATOM_TiXmlNode* removeThis )
{
	if ( removeThis->parent != this )
	{	
		assert( 0 );
		return false;
	}

	if ( removeThis->next )
		removeThis->next->prev = removeThis->prev;
	else
		lastChild = removeThis->prev;

	if ( removeThis->prev )
		removeThis->prev->next = removeThis->next;
	else
		firstChild = removeThis->next;

	ATOM_POOL_DELETE(removeThis);
	return true;
}

const ATOM_TiXmlNode* ATOM_TiXmlNode::FirstChild( const char * _value ) const
{
	const ATOM_TiXmlNode* node;
	for ( node = firstChild; node; node = node->next )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}


const ATOM_TiXmlNode* ATOM_TiXmlNode::LastChild( const char * _value ) const
{
	const ATOM_TiXmlNode* node;
	for ( node = lastChild; node; node = node->prev )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}


const ATOM_TiXmlNode* ATOM_TiXmlNode::IterateChildren( const ATOM_TiXmlNode* previous ) const
{
	if ( !previous )
	{
		return FirstChild();
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling();
	}
}


const ATOM_TiXmlNode* ATOM_TiXmlNode::IterateChildren( const char * val, const ATOM_TiXmlNode* previous ) const
{
	if ( !previous )
	{
		return FirstChild( val );
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling( val );
	}
}


const ATOM_TiXmlNode* ATOM_TiXmlNode::NextSibling( const char * _value ) const 
{
	const ATOM_TiXmlNode* node;
	for ( node = next; node; node = node->next )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}


const ATOM_TiXmlNode* ATOM_TiXmlNode::PreviousSibling( const char * _value ) const
{
	const ATOM_TiXmlNode* node;
	for ( node = prev; node; node = node->prev )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}


void ATOM_TiXmlElement::RemoveAttribute( const char * name )
{
    #ifdef TIXML_USE_STL
	ATOM_TIXML_STRING str( name );
	ATOM_TiXmlAttribute* node = attributeSet.Find( str );
	#else
	ATOM_TiXmlAttribute* node = attributeSet.Find( name );
	#endif
	if ( node )
	{
		attributeSet.Remove( node );
		ATOM_POOL_DELETE(node);
	}
}

const ATOM_TiXmlElement* ATOM_TiXmlNode::FirstChildElement() const
{
	const ATOM_TiXmlNode* node;

	for (	node = FirstChild();
			node;
			node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


const ATOM_TiXmlElement* ATOM_TiXmlNode::FirstChildElement( const char * _value ) const
{
	const ATOM_TiXmlNode* node;

	for (	node = FirstChild( _value );
			node;
			node = node->NextSibling( _value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


const ATOM_TiXmlElement* ATOM_TiXmlNode::NextSiblingElement() const
{
	const ATOM_TiXmlNode* node;

	for (	node = NextSibling();
			node;
			node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


const ATOM_TiXmlElement* ATOM_TiXmlNode::NextSiblingElement( const char * _value ) const
{
	const ATOM_TiXmlNode* node;

	for (	node = NextSibling( _value );
			node;
			node = node->NextSibling( _value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


const ATOM_TiXmlDocument* ATOM_TiXmlNode::GetDocument() const
{
	const ATOM_TiXmlNode* node;

	for( node = this; node; node = node->parent )
	{
		if ( node->ToDocument() )
			return node->ToDocument();
	}
	return 0;
}


ATOM_TiXmlElement::ATOM_TiXmlElement (const char * _value)
	: ATOM_TiXmlNode( ATOM_TiXmlNode::ELEMENT )
{
	firstChild = lastChild = 0;
	value = _value;
}


#ifdef TIXML_USE_STL
ATOM_TiXmlElement::ATOM_TiXmlElement( const std::string& _value ) 
	: ATOM_TiXmlNode( ATOM_TiXmlNode::ELEMENT )
{
	firstChild = lastChild = 0;
	value = _value;
}
#endif


ATOM_TiXmlElement::ATOM_TiXmlElement( const ATOM_TiXmlElement& copy)
	: ATOM_TiXmlNode( ATOM_TiXmlNode::ELEMENT )
{
	firstChild = lastChild = 0;
	copy.CopyTo( this );	
}


void ATOM_TiXmlElement::operator=( const ATOM_TiXmlElement& base )
{
	ClearThis();
	base.CopyTo( this );
}


ATOM_TiXmlElement::~ATOM_TiXmlElement()
{
	ClearThis();
}


void ATOM_TiXmlElement::ClearThis()
{
	Clear();
	while( attributeSet.First() )
	{
		ATOM_TiXmlAttribute* node = attributeSet.First();
		attributeSet.Remove( node );
		ATOM_POOL_DELETE(node);
	}
}


const char* ATOM_TiXmlElement::Attribute( const char* name ) const
{
	const ATOM_TiXmlAttribute* node = attributeSet.Find( name );
	if ( node )
		return node->Value();
	return 0;
}


#ifdef TIXML_USE_STL
const std::string* ATOM_TiXmlElement::Attribute( const std::string& name ) const
{
	const ATOM_TiXmlAttribute* node = attributeSet.Find( name );
	if ( node )
		return &node->ValueStr();
	return 0;
}
#endif


const char* ATOM_TiXmlElement::Attribute( const char* name, int* i ) const
{
	const char* s = Attribute( name );
	if ( i )
	{
		if ( s ) {
			*i = atoi( s );
		}
		else {
			*i = 0;
		}
	}
	return s;
}


#ifdef TIXML_USE_STL
const std::string* ATOM_TiXmlElement::Attribute( const std::string& name, int* i ) const
{
	const std::string* s = Attribute( name );
	if ( i )
	{
		if ( s ) {
			*i = atoi( s->c_str() );
		}
		else {
			*i = 0;
		}
	}
	return s;
}
#endif


const char* ATOM_TiXmlElement::Attribute( const char* name, double* d ) const
{
	const char* s = Attribute( name );
	if ( d )
	{
		if ( s ) {
			*d = atof( s );
		}
		else {
			*d = 0;
		}
	}
	return s;
}


#ifdef TIXML_USE_STL
const std::string* ATOM_TiXmlElement::Attribute( const std::string& name, double* d ) const
{
	const std::string* s = Attribute( name );
	if ( d )
	{
		if ( s ) {
			*d = atof( s->c_str() );
		}
		else {
			*d = 0;
		}
	}
	return s;
}
#endif


int ATOM_TiXmlElement::QueryIntAttribute( const char* name, int* ival ) const
{
	const ATOM_TiXmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return ATOM_TIXML_NO_ATTRIBUTE;
	return node->QueryIntValue( ival );
}


#ifdef TIXML_USE_STL
int ATOM_TiXmlElement::QueryIntAttribute( const std::string& name, int* ival ) const
{
	const ATOM_TiXmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return ATOM_TIXML_NO_ATTRIBUTE;
	return node->QueryIntValue( ival );
}
#endif


int ATOM_TiXmlElement::QueryDoubleAttribute( const char* name, double* dval ) const
{
	const ATOM_TiXmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return ATOM_TIXML_NO_ATTRIBUTE;
	return node->QueryDoubleValue( dval );
}


#ifdef TIXML_USE_STL
int ATOM_TiXmlElement::QueryDoubleAttribute( const std::string& name, double* dval ) const
{
	const ATOM_TiXmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return ATOM_TIXML_NO_ATTRIBUTE;
	return node->QueryDoubleValue( dval );
}
#endif


void ATOM_TiXmlElement::SetAttribute( const char * name, int val )
{	
	char buf[64];
	#if defined(ATOM_TIXML_SNPRINTF)		
		ATOM_TIXML_SNPRINTF( buf, sizeof(buf), "%d", val );
	#else
		sprintf( buf, "%d", val );
	#endif
	SetAttribute( name, buf );
}


#ifdef TIXML_USE_STL
void ATOM_TiXmlElement::SetAttribute( const std::string& name, int val )
{	
   std::ostringstream oss;
   oss << val;
   SetAttribute( name, oss.str() );
}
#endif


void ATOM_TiXmlElement::SetDoubleAttribute( const char * name, double val )
{	
	char buf[256];
	#if defined(ATOM_TIXML_SNPRINTF)		
		ATOM_TIXML_SNPRINTF( buf, sizeof(buf), "%f", val );
	#else
		sprintf( buf, "%f", val );
	#endif
	SetAttribute( name, buf );
}


void ATOM_TiXmlElement::SetAttribute( const char * cname, const char * cvalue )
{
    #ifdef TIXML_USE_STL
	ATOM_TIXML_STRING _name( cname );
	ATOM_TIXML_STRING _value( cvalue );
	#else
	const char* _name = cname;
	const char* _value = cvalue;
	#endif

	ATOM_TiXmlAttribute* node = attributeSet.Find( _name );
	if ( node )
	{
		node->SetValue( _value );
		return;
	}

	ATOM_TiXmlAttribute* attrib = ATOM_POOL_NEW (ATOM_TiXmlAttribute, cname, cvalue );
	if ( attrib )
	{
		attributeSet.Add( attrib );
	}
	else
	{
		ATOM_TiXmlDocument* document = GetDocument();
		if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, ATOM_TIXML_ENCODING_UNKNOWN );
	}
}


#ifdef TIXML_USE_STL
void ATOM_TiXmlElement::SetAttribute( const std::string& name, const std::string& _value )
{
	ATOM_TiXmlAttribute* node = attributeSet.Find( name );
	if ( node )
	{
		node->SetValue( _value );
		return;
	}

	ATOM_TiXmlAttribute* attrib = ATOM_POOL_NEW(ATOM_TiXmlAttribute, name, _value );
	if ( attrib )
	{
		attributeSet.Add( attrib );
	}
	else
	{
		ATOM_TiXmlDocument* document = GetDocument();
		if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, ATOM_TIXML_ENCODING_UNKNOWN );
	}
}
#endif


void ATOM_TiXmlElement::Print( FILE* cfile, int depth ) const
{
	int i;
	assert( cfile );
	for ( i=0; i<depth; i++ ) {
		fprintf( cfile, "    " );
	}

	fprintf( cfile, "<%s", value.c_str() );

	const ATOM_TiXmlAttribute* attrib;
	for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
	{
		fprintf( cfile, " " );
		attrib->Print( cfile, depth );
	}

	// There are 3 different formatting approaches:
	// 1) An element without children is printed as a <foo /> node
	// 2) An element with only a text child is printed as <foo> text </foo>
	// 3) An element with children is printed on multiple lines.
	ATOM_TiXmlNode* node;
	if ( !firstChild )
	{
		fprintf( cfile, " />" );
	}
	else if ( firstChild == lastChild && firstChild->ToText() )
	{
		fprintf( cfile, ">" );
		firstChild->Print( cfile, depth + 1 );
		fprintf( cfile, "</%s>", value.c_str() );
	}
	else
	{
		fprintf( cfile, ">" );

		for ( node = firstChild; node; node=node->NextSibling() )
		{
			if ( !node->ToText() )
			{
				fprintf( cfile, "\n" );
			}
			node->Print( cfile, depth+1 );
		}
		fprintf( cfile, "\n" );
		for( i=0; i<depth; ++i ) {
			fprintf( cfile, "    " );
		}
		fprintf( cfile, "</%s>", value.c_str() );
	}
}


void ATOM_TiXmlElement::CopyTo( ATOM_TiXmlElement* target ) const
{
	// superclass:
	ATOM_TiXmlNode::CopyTo( target );

	// Element class: 
	// Clone the attributes, then clone the children.
	const ATOM_TiXmlAttribute* attribute = 0;
	for(	attribute = attributeSet.First();
	attribute;
	attribute = attribute->Next() )
	{
		target->SetAttribute( attribute->Name(), attribute->Value() );
	}

	ATOM_TiXmlNode* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		target->LinkEndChild( node->Clone() );
	}
}

bool ATOM_TiXmlElement::Accept( ATOM_TiXmlVisitor* visitor ) const
{
	if ( visitor->VisitEnter( *this, attributeSet.First() ) ) 
	{
		for ( const ATOM_TiXmlNode* node=FirstChild(); node; node=node->NextSibling() )
		{
			if ( !node->Accept( visitor ) )
				break;
		}
	}
	return visitor->VisitExit( *this );
}


ATOM_TiXmlNode* ATOM_TiXmlElement::Clone() const
{
	ATOM_TiXmlElement* clone = ATOM_POOL_NEW(ATOM_TiXmlElement, Value() );
	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


const char* ATOM_TiXmlElement::GetText() const
{
	const ATOM_TiXmlNode* child = this->FirstChild();
	if ( child ) {
		const ATOM_TiXmlText* childText = child->ToText();
		if ( childText ) {
			return childText->Value();
		}
	}
	return 0;
}


ATOM_TiXmlDocument::ATOM_TiXmlDocument() : ATOM_TiXmlNode( ATOM_TiXmlNode::DOCUMENT )
{
	tabsize = 4;
	useMicrosoftBOM = false;
	ClearError();
}

ATOM_TiXmlDocument::ATOM_TiXmlDocument( const char * documentName ) : ATOM_TiXmlNode( ATOM_TiXmlNode::DOCUMENT )
{
	tabsize = 4;
	useMicrosoftBOM = false;
	value = documentName;
	ClearError();
}


#ifdef TIXML_USE_STL
ATOM_TiXmlDocument::ATOM_TiXmlDocument( const std::string& documentName ) : ATOM_TiXmlNode( ATOM_TiXmlNode::DOCUMENT )
{
	tabsize = 4;
	useMicrosoftBOM = false;
    value = documentName;
	ClearError();
}
#endif


ATOM_TiXmlDocument::ATOM_TiXmlDocument( const ATOM_TiXmlDocument& copy ) : ATOM_TiXmlNode( ATOM_TiXmlNode::DOCUMENT )
{
	copy.CopyTo( this );
}


void ATOM_TiXmlDocument::operator=( const ATOM_TiXmlDocument& copy )
{
	Clear();
	copy.CopyTo( this );
}


bool ATOM_TiXmlDocument::LoadFile( ATOM_TiXmlEncoding encoding )
{
	// See STL_STRING_BUG below.
	//StringToBuffer buf( value );

	return LoadFile( Value(), encoding );
}


bool ATOM_TiXmlDocument::SaveFile() const
{
	// See STL_STRING_BUG below.
//	StringToBuffer buf( value );
//
//	if ( buf.buffer && SaveFile( buf.buffer ) )
//		return true;
//
//	return false;
	return SaveFile( Value() );
}

bool ATOM_TiXmlDocument::LoadFile( const char* _filename, ATOM_TiXmlEncoding encoding )
{
	// There was a really terrifying little bug here. The code:
	//		value = filename
	// in the STL case, cause the assignment method of the std::string to
	// be called. What is strange, is that the std::string had the same
	// address as it's c_str() method, and so bad things happen. Looks
	// like a bug in the Microsoft STL implementation.
	// Add an extra string to avoid the crash.
	ATOM_TIXML_STRING filename( _filename );
	value = filename;

	// reading in binary mode so that tinyxml can normalize the EOL
	FILE* file = TiXmlFOpen( value.c_str (), "rb" );	

	if ( file )
	{
		bool result = LoadFile( file, encoding );
		fclose( file );
		return result;
	}
	else
	{
		SetError( TIXML_ERROR_OPENING_FILE, 0, 0, ATOM_TIXML_ENCODING_UNKNOWN );
		return false;
	}
}

bool ATOM_TiXmlDocument::LoadFile( FILE* file, ATOM_TiXmlEncoding encoding )
{
	if ( !file ) 
	{
		SetError( TIXML_ERROR_OPENING_FILE, 0, 0, ATOM_TIXML_ENCODING_UNKNOWN );
		return false;
	}

	// Delete the existing data:
	Clear();
	location.Clear();

	// Get the file size, so we can pre-allocate the string. HUGE speed impact.
	long length = 0;
	fseek( file, 0, SEEK_END );
	length = ftell( file );
	fseek( file, 0, SEEK_SET );

	// Strange case, but good to handle up front.
	if ( length <= 0 )
	{
		SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, ATOM_TIXML_ENCODING_UNKNOWN );
		return false;
	}

	// If we have a file, assume it is all one big XML file, and read it in.
	// The document parser may decide the document ends sooner than the entire file, however.
	ATOM_TIXML_STRING data;
	data.reserve( length );

	// Subtle bug here. TinyXml did use fgets. But from the XML spec:
	// 2.11 End-of-Line Handling
	// <snip>
	// <quote>
	// ...the XML processor MUST behave as if it normalized all line breaks in external 
	// parsed entities (including the document entity) on input, before parsing, by translating 
	// both the two-character sequence #xD #xA and any #xD that is not followed by #xA to 
	// a single #xA character.
	// </quote>
	//
	// It is not clear fgets does that, and certainly isn't clear it works cross platform. 
	// Generally, you expect fgets to translate from the convention of the OS to the c/unix
	// convention, and not work generally.

	/*
	while( fgets( buf, sizeof(buf), file ) )
	{
		data += buf;
	}
	*/

	char* buf = new char[ length+1 ];
	buf[0] = 0;

	if ( fread( buf, length, 1, file ) != 1 ) {
		delete [] buf;
		SetError( TIXML_ERROR_OPENING_FILE, 0, 0, ATOM_TIXML_ENCODING_UNKNOWN );
		return false;
	}

	const char* lastPos = buf;
	const char* p = buf;

	buf[length] = 0;
	while( *p ) {
		assert( p < (buf+length) );
		if ( *p == 0xa ) {
			// Newline character. No special rules for this. Append all the characters
			// since the last string, and include the newline.
			data.append( lastPos, (p-lastPos+1) );	// append, include the newline
			++p;									// move past the newline
			lastPos = p;							// and point to the new buffer (may be 0)
			assert( p <= (buf+length) );
		}
		else if ( *p == 0xd ) {
			// Carriage return. Append what we have so far, then
			// handle moving forward in the buffer.
			if ( (p-lastPos) > 0 ) {
				data.append( lastPos, p-lastPos );	// do not add the CR
			}
			data += (char)0xa;						// a proper newline

			if ( *(p+1) == 0xa ) {
				// Carriage return - new line sequence
				p += 2;
				lastPos = p;
				assert( p <= (buf+length) );
			}
			else {
				// it was followed by something else...that is presumably characters again.
				++p;
				lastPos = p;
				assert( p <= (buf+length) );
			}
		}
		else {
			++p;
		}
	}
	// Handle any left over characters.
	if ( p-lastPos ) {
		data.append( lastPos, p-lastPos );
	}		
	delete [] buf;
	buf = 0;

	Parse( data.c_str(), 0, encoding );

	if (  Error() )
        return false;
    else
		return true;
}


bool ATOM_TiXmlDocument::SaveFile( const char * filename ) const
{
	// The old c stuff lives on...
	FILE* fp = TiXmlFOpen( filename, "w" );
	if ( fp )
	{
		bool result = SaveFile( fp );
		fclose( fp );
		return result;
	}
	return false;
}


bool ATOM_TiXmlDocument::SaveFile( FILE* fp ) const
{
	if ( useMicrosoftBOM ) 
	{
		const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
		const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
		const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

		fputc( TIXML_UTF_LEAD_0, fp );
		fputc( TIXML_UTF_LEAD_1, fp );
		fputc( TIXML_UTF_LEAD_2, fp );
	}
	Print( fp, 0 );
	return (ferror(fp) == 0);
}


void ATOM_TiXmlDocument::CopyTo( ATOM_TiXmlDocument* target ) const
{
	ATOM_TiXmlNode::CopyTo( target );

	target->error = error;
	target->errorId = errorId;
	target->errorDesc = errorDesc;
	target->tabsize = tabsize;
	target->errorLocation = errorLocation;
	target->useMicrosoftBOM = useMicrosoftBOM;

	ATOM_TiXmlNode* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		target->LinkEndChild( node->Clone() );
	}	
}


ATOM_TiXmlNode* ATOM_TiXmlDocument::Clone() const
{
	ATOM_TiXmlDocument* clone = ATOM_POOL_NEW(ATOM_TiXmlDocument);
	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


void ATOM_TiXmlDocument::Print( FILE* cfile, int depth ) const
{
	assert( cfile );
	for ( const ATOM_TiXmlNode* node=FirstChild(); node; node=node->NextSibling() )
	{
		node->Print( cfile, depth );
		fprintf( cfile, "\n" );
	}
}


bool ATOM_TiXmlDocument::Accept( ATOM_TiXmlVisitor* visitor ) const
{
	if ( visitor->VisitEnter( *this ) )
	{
		for ( const ATOM_TiXmlNode* node=FirstChild(); node; node=node->NextSibling() )
		{
			if ( !node->Accept( visitor ) )
				break;
		}
	}
	return visitor->VisitExit( *this );
}


const ATOM_TiXmlAttribute* ATOM_TiXmlAttribute::Next() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( next->value.empty() && next->name.empty() )
		return 0;
	return next;
}

/*
ATOM_TiXmlAttribute* ATOM_TiXmlAttribute::Next()
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( next->value.empty() && next->name.empty() )
		return 0;
	return next;
}
*/

const ATOM_TiXmlAttribute* ATOM_TiXmlAttribute::Previous() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( prev->value.empty() && prev->name.empty() )
		return 0;
	return prev;
}

/*
ATOM_TiXmlAttribute* ATOM_TiXmlAttribute::Previous()
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( prev->value.empty() && prev->name.empty() )
		return 0;
	return prev;
}
*/

void ATOM_TiXmlAttribute::Print( FILE* cfile, int /*depth*/, ATOM_TIXML_STRING* str ) const
{
	ATOM_TIXML_STRING n, v;

	EncodeString( name, &n );
	EncodeString( value, &v );

	if (value.find ('\"') == ATOM_TIXML_STRING::npos) {
		if ( cfile ) {
		fprintf (cfile, "%s=\"%s\"", n.c_str(), v.c_str() );
		}
		if ( str ) {
			(*str) += n; (*str) += "=\""; (*str) += v; (*str) += "\"";
		}
	}
	else {
		if ( cfile ) {
		fprintf (cfile, "%s='%s'", n.c_str(), v.c_str() );
		}
		if ( str ) {
			(*str) += n; (*str) += "='"; (*str) += v; (*str) += "'";
		}
	}
}


int ATOM_TiXmlAttribute::QueryIntValue( int* ival ) const
{
	if ( ATOM_TIXML_SSCANF( value.c_str(), "%d", ival ) == 1 )
		return ATOM_TIXML_SUCCESS;
	return ATOM_TIXML_WRONG_TYPE;
}

int ATOM_TiXmlAttribute::QueryDoubleValue( double* dval ) const
{
	if ( ATOM_TIXML_SSCANF( value.c_str(), "%lf", dval ) == 1 )
		return ATOM_TIXML_SUCCESS;
	return ATOM_TIXML_WRONG_TYPE;
}

void ATOM_TiXmlAttribute::SetIntValue( int _value )
{
	char buf [64];
	#if defined(ATOM_TIXML_SNPRINTF)		
		ATOM_TIXML_SNPRINTF(buf, sizeof(buf), "%d", _value);
	#else
		sprintf (buf, "%d", _value);
	#endif
	SetValue (buf);
}

void ATOM_TiXmlAttribute::SetDoubleValue( double _value )
{
	char buf [256];
	#if defined(ATOM_TIXML_SNPRINTF)		
		ATOM_TIXML_SNPRINTF( buf, sizeof(buf), "%lf", _value);
	#else
		sprintf (buf, "%lf", _value);
	#endif
	SetValue (buf);
}

int ATOM_TiXmlAttribute::IntValue() const
{
	return atoi (value.c_str ());
}

double  ATOM_TiXmlAttribute::DoubleValue() const
{
	return atof (value.c_str ());
}


ATOM_TiXmlComment::ATOM_TiXmlComment( const ATOM_TiXmlComment& copy ) : ATOM_TiXmlNode( ATOM_TiXmlNode::COMMENT )
{
	copy.CopyTo( this );
}


void ATOM_TiXmlComment::operator=( const ATOM_TiXmlComment& base )
{
	Clear();
	base.CopyTo( this );
}


void ATOM_TiXmlComment::Print( FILE* cfile, int depth ) const
{
	assert( cfile );
	for ( int i=0; i<depth; i++ )
	{
		fprintf( cfile,  "    " );
	}
	fprintf( cfile, "<!--%s-->", value.c_str() );
}


void ATOM_TiXmlComment::CopyTo( ATOM_TiXmlComment* target ) const
{
	ATOM_TiXmlNode::CopyTo( target );
}


bool ATOM_TiXmlComment::Accept( ATOM_TiXmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


ATOM_TiXmlNode* ATOM_TiXmlComment::Clone() const
{
	ATOM_TiXmlComment* clone = ATOM_POOL_NEW(ATOM_TiXmlComment);

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


void ATOM_TiXmlText::Print( FILE* cfile, int depth ) const
{
	assert( cfile );
	if ( cdata )
	{
		int i;
		fprintf( cfile, "\n" );
		for ( i=0; i<depth; i++ ) {
			fprintf( cfile, "    " );
		}
		fprintf( cfile, "<![CDATA[%s]]>\n", value.c_str() );	// unformatted output
	}
	else
	{
		ATOM_TIXML_STRING buffer;
		EncodeString( value, &buffer );
		fprintf( cfile, "%s", buffer.c_str() );
	}
}


void ATOM_TiXmlText::CopyTo( ATOM_TiXmlText* target ) const
{
	ATOM_TiXmlNode::CopyTo( target );
	target->cdata = cdata;
}


bool ATOM_TiXmlText::Accept( ATOM_TiXmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


ATOM_TiXmlNode* ATOM_TiXmlText::Clone() const
{	
	ATOM_TiXmlText* clone = 0;
	clone = ATOM_POOL_NEW(ATOM_TiXmlText, "" );

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


ATOM_TiXmlDeclaration::ATOM_TiXmlDeclaration( const char * _version,
									const char * _encoding,
									const char * _standalone )
	: ATOM_TiXmlNode( ATOM_TiXmlNode::DECLARATION )
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}


#ifdef TIXML_USE_STL
ATOM_TiXmlDeclaration::ATOM_TiXmlDeclaration(	const std::string& _version,
									const std::string& _encoding,
									const std::string& _standalone )
	: ATOM_TiXmlNode( ATOM_TiXmlNode::DECLARATION )
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}
#endif


ATOM_TiXmlDeclaration::ATOM_TiXmlDeclaration( const ATOM_TiXmlDeclaration& copy )
	: ATOM_TiXmlNode( ATOM_TiXmlNode::DECLARATION )
{
	copy.CopyTo( this );	
}


void ATOM_TiXmlDeclaration::operator=( const ATOM_TiXmlDeclaration& copy )
{
	Clear();
	copy.CopyTo( this );
}


void ATOM_TiXmlDeclaration::Print( FILE* cfile, int /*depth*/, ATOM_TIXML_STRING* str ) const
{
	if ( cfile ) fprintf( cfile, "<?xml " );
	if ( str )	 (*str) += "<?xml ";

	if ( !version.empty() ) {
		if ( cfile ) fprintf (cfile, "version=\"%s\" ", version.c_str ());
		if ( str ) { (*str) += "version=\""; (*str) += version; (*str) += "\" "; }
	}
	if ( !encoding.empty() ) {
		if ( cfile ) fprintf (cfile, "encoding=\"%s\" ", encoding.c_str ());
		if ( str ) { (*str) += "encoding=\""; (*str) += encoding; (*str) += "\" "; }
	}
	if ( !standalone.empty() ) {
		if ( cfile ) fprintf (cfile, "standalone=\"%s\" ", standalone.c_str ());
		if ( str ) { (*str) += "standalone=\""; (*str) += standalone; (*str) += "\" "; }
	}
	if ( cfile ) fprintf( cfile, "?>" );
	if ( str )	 (*str) += "?>";
}


void ATOM_TiXmlDeclaration::CopyTo( ATOM_TiXmlDeclaration* target ) const
{
	ATOM_TiXmlNode::CopyTo( target );

	target->version = version;
	target->encoding = encoding;
	target->standalone = standalone;
}


bool ATOM_TiXmlDeclaration::Accept( ATOM_TiXmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


ATOM_TiXmlNode* ATOM_TiXmlDeclaration::Clone() const
{	
	ATOM_TiXmlDeclaration* clone = ATOM_POOL_NEW(ATOM_TiXmlDeclaration);

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


void ATOM_TiXmlUnknown::Print( FILE* cfile, int depth ) const
{
	for ( int i=0; i<depth; i++ )
		fprintf( cfile, "    " );
	fprintf( cfile, "<%s>", value.c_str() );
}


void ATOM_TiXmlUnknown::CopyTo( ATOM_TiXmlUnknown* target ) const
{
	ATOM_TiXmlNode::CopyTo( target );
}


bool ATOM_TiXmlUnknown::Accept( ATOM_TiXmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


ATOM_TiXmlNode* ATOM_TiXmlUnknown::Clone() const
{
	ATOM_TiXmlUnknown* clone = ATOM_POOL_NEW(ATOM_TiXmlUnknown);

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


ATOM_TiXmlAttributeSet::ATOM_TiXmlAttributeSet()
{
	sentinel.next = &sentinel;
	sentinel.prev = &sentinel;
}


ATOM_TiXmlAttributeSet::~ATOM_TiXmlAttributeSet()
{
	assert( sentinel.next == &sentinel );
	assert( sentinel.prev == &sentinel );
}


void ATOM_TiXmlAttributeSet::Add( ATOM_TiXmlAttribute* addMe )
{
    #ifdef TIXML_USE_STL
	assert( !Find( ATOM_TIXML_STRING( addMe->Name() ) ) );	// Shouldn't be multiply adding to the set.
	#else
	assert( !Find( addMe->Name() ) );	// Shouldn't be multiply adding to the set.
	#endif

	addMe->next = &sentinel;
	addMe->prev = sentinel.prev;

	sentinel.prev->next = addMe;
	sentinel.prev      = addMe;
}

void ATOM_TiXmlAttributeSet::Remove( ATOM_TiXmlAttribute* removeMe )
{
	ATOM_TiXmlAttribute* node;

	for( node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node == removeMe )
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->next = 0;
			node->prev = 0;
			return;
		}
	}
	assert( 0 );		// we tried to remove a non-linked attribute.
}


#ifdef TIXML_USE_STL
const ATOM_TiXmlAttribute* ATOM_TiXmlAttributeSet::Find( const std::string& name ) const
{
	for( const ATOM_TiXmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node->name == name )
			return node;
	}
	return 0;
}

/*
ATOM_TiXmlAttribute*	ATOM_TiXmlAttributeSet::Find( const std::string& name )
{
	for( ATOM_TiXmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node->name == name )
			return node;
	}
	return 0;
}
*/
#endif


const ATOM_TiXmlAttribute* ATOM_TiXmlAttributeSet::Find( const char* name ) const
{
	for( const ATOM_TiXmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( strcmp( node->name.c_str(), name ) == 0 )
			return node;
	}
	return 0;
}

/*
ATOM_TiXmlAttribute*	ATOM_TiXmlAttributeSet::Find( const char* name )
{
	for( ATOM_TiXmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( strcmp( node->name.c_str(), name ) == 0 )
			return node;
	}
	return 0;
}
*/

#ifdef TIXML_USE_STL	
std::istream& operator>> (std::istream & in, ATOM_TiXmlNode & base)
{
	ATOM_TIXML_STRING tag;
	tag.reserve( 8 * 1000 );
	base.StreamIn( &in, &tag );

	base.Parse( tag.c_str(), 0, ATOM_TIXML_DEFAULT_ENCODING );
	return in;
}
#endif


#ifdef TIXML_USE_STL	
std::ostream& operator<< (std::ostream & out, const ATOM_TiXmlNode & base)
{
	ATOM_TiXmlPrinter printer;
	printer.SetStreamPrinting();
	base.Accept( &printer );
	out << printer.Str();

	return out;
}


std::string& operator<< (std::string& out, const ATOM_TiXmlNode& base )
{
	ATOM_TiXmlPrinter printer;
	printer.SetStreamPrinting();
	base.Accept( &printer );
	out.append( printer.Str() );

	return out;
}
#endif


ATOM_TiXmlHandle ATOM_TiXmlHandle::FirstChild() const
{
	if ( node )
	{
		ATOM_TiXmlNode* child = node->FirstChild();
		if ( child )
			return ATOM_TiXmlHandle( child );
	}
	return ATOM_TiXmlHandle( 0 );
}


ATOM_TiXmlHandle ATOM_TiXmlHandle::FirstChild( const char * value ) const
{
	if ( node )
	{
		ATOM_TiXmlNode* child = node->FirstChild( value );
		if ( child )
			return ATOM_TiXmlHandle( child );
	}
	return ATOM_TiXmlHandle( 0 );
}


ATOM_TiXmlHandle ATOM_TiXmlHandle::FirstChildElement() const
{
	if ( node )
	{
		ATOM_TiXmlElement* child = node->FirstChildElement();
		if ( child )
			return ATOM_TiXmlHandle( child );
	}
	return ATOM_TiXmlHandle( 0 );
}


ATOM_TiXmlHandle ATOM_TiXmlHandle::FirstChildElement( const char * value ) const
{
	if ( node )
	{
		ATOM_TiXmlElement* child = node->FirstChildElement( value );
		if ( child )
			return ATOM_TiXmlHandle( child );
	}
	return ATOM_TiXmlHandle( 0 );
}


ATOM_TiXmlHandle ATOM_TiXmlHandle::Child( int count ) const
{
	if ( node )
	{
		int i;
		ATOM_TiXmlNode* child = node->FirstChild();
		for (	i=0;
				child && i<count;
				child = child->NextSibling(), ++i )
		{
			// nothing
		}
		if ( child )
			return ATOM_TiXmlHandle( child );
	}
	return ATOM_TiXmlHandle( 0 );
}


ATOM_TiXmlHandle ATOM_TiXmlHandle::Child( const char* value, int count ) const
{
	if ( node )
	{
		int i;
		ATOM_TiXmlNode* child = node->FirstChild( value );
		for (	i=0;
				child && i<count;
				child = child->NextSibling( value ), ++i )
		{
			// nothing
		}
		if ( child )
			return ATOM_TiXmlHandle( child );
	}
	return ATOM_TiXmlHandle( 0 );
}


ATOM_TiXmlHandle ATOM_TiXmlHandle::ChildElement( int count ) const
{
	if ( node )
	{
		int i;
		ATOM_TiXmlElement* child = node->FirstChildElement();
		for (	i=0;
				child && i<count;
				child = child->NextSiblingElement(), ++i )
		{
			// nothing
		}
		if ( child )
			return ATOM_TiXmlHandle( child );
	}
	return ATOM_TiXmlHandle( 0 );
}


ATOM_TiXmlHandle ATOM_TiXmlHandle::ChildElement( const char* value, int count ) const
{
	if ( node )
	{
		int i;
		ATOM_TiXmlElement* child = node->FirstChildElement( value );
		for (	i=0;
				child && i<count;
				child = child->NextSiblingElement( value ), ++i )
		{
			// nothing
		}
		if ( child )
			return ATOM_TiXmlHandle( child );
	}
	return ATOM_TiXmlHandle( 0 );
}


bool ATOM_TiXmlPrinter::VisitEnter( const ATOM_TiXmlDocument& )
{
	return true;
}

bool ATOM_TiXmlPrinter::VisitExit( const ATOM_TiXmlDocument& )
{
	return true;
}

bool ATOM_TiXmlPrinter::VisitEnter( const ATOM_TiXmlElement& element, const ATOM_TiXmlAttribute* firstAttribute )
{
	DoIndent();
	buffer += "<";
	buffer += element.Value();

	for( const ATOM_TiXmlAttribute* attrib = firstAttribute; attrib; attrib = attrib->Next() )
	{
		buffer += " ";
		attrib->Print( 0, 0, &buffer );
	}

	if ( !element.FirstChild() ) 
	{
		buffer += " />";
		DoLineBreak();
	}
	else 
	{
		buffer += ">";
		if (    element.FirstChild()->ToText()
			  && element.LastChild() == element.FirstChild()
			  && element.FirstChild()->ToText()->CDATA() == false )
		{
			simpleTextPrint = true;
			// no DoLineBreak()!
		}
		else
		{
			DoLineBreak();
		}
	}
	++depth;	
	return true;
}


bool ATOM_TiXmlPrinter::VisitExit( const ATOM_TiXmlElement& element )
{
	--depth;
	if ( !element.FirstChild() ) 
	{
		// nothing.
	}
	else 
	{
		if ( simpleTextPrint )
		{
			simpleTextPrint = false;
		}
		else
		{
			DoIndent();
		}
		buffer += "</";
		buffer += element.Value();
		buffer += ">";
		DoLineBreak();
	}
	return true;
}


bool ATOM_TiXmlPrinter::Visit( const ATOM_TiXmlText& text )
{
	if ( text.CDATA() )
	{
		DoIndent();
		buffer += "<![CDATA[";
		buffer += text.Value();
		buffer += "]]>";
		DoLineBreak();
	}
	else if ( simpleTextPrint )
	{
		ATOM_TIXML_STRING str;
		ATOM_TiXmlBase::EncodeString( text.ValueTStr(), &str );
		buffer += str;
	}
	else
	{
		DoIndent();
		ATOM_TIXML_STRING str;
		ATOM_TiXmlBase::EncodeString( text.ValueTStr(), &str );
		buffer += str;
		DoLineBreak();
	}
	return true;
}


bool ATOM_TiXmlPrinter::Visit( const ATOM_TiXmlDeclaration& declaration )
{
	DoIndent();
	declaration.Print( 0, 0, &buffer );
	DoLineBreak();
	return true;
}


bool ATOM_TiXmlPrinter::Visit( const ATOM_TiXmlComment& comment )
{
	DoIndent();
	buffer += "<!--";
	buffer += comment.Value();
	buffer += "-->";
	DoLineBreak();
	return true;
}


bool ATOM_TiXmlPrinter::Visit( const ATOM_TiXmlUnknown& unknown )
{
	DoIndent();
	buffer += "<";
	buffer += unknown.Value();
	buffer += ">";
	DoLineBreak();
	return true;
}

