/*
MIT License

Copyright (c) 2025 Expert Sleepers Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _DISTINGNT_SERIALISATION_H
#define _DISTINGNT_SERIALISATION_H

#include <stdint.h>

class _NT_jsonStream
{
private:
	void*		refCon;
	// disallow copy and assign
	_NT_jsonStream(const _NT_jsonStream&);
	void operator=(const _NT_jsonStream&);

#ifdef _DISTINGNT_SERIALISATION_INTERNAL
public:
#endif
	_NT_jsonStream( void* );
	~_NT_jsonStream();

public:
	// Open and close an array.
	// Within an array, you can nest arrays, objects, or primitives.
	void	openArray(void);
	void	closeArray(void);

	// Open and close an object.
	// An object is a collection of members. First add the member name, then the member itself.
	// A member can be an array, an object, or a primitive.
	void	openObject(void);
	void	closeObject(void);
	void 	addMemberName( const char* str );

	// Add primitives.
	void	addNumber( int v );
	void	addNumber( float v );
	void 	addString( const char* str );
	void	addFourCC( uint32_t v );			// A four character code; a string in the JSON.
	void	addBoolean( bool v );
	void	addNull(void);

};

class _NT_jsonParse
{
private:
	void*		refCon;
	int			i;
	// disallow copy and assign
	_NT_jsonParse(const _NT_jsonParse&);
	void operator=(const _NT_jsonParse&);

#ifdef _DISTINGNT_SERIALISATION_INTERNAL
public:
#endif
	_NT_jsonParse( void*, int );
	~_NT_jsonParse();

public:
	// Get the number of elements in an array. Returns false if current token is not an array.
	bool	numberOfArrayElements( int& num );

	// Get the number of members in an object. Returns false if current token is not an object.
	bool	numberOfObjectMembers( int& num );
	// Return true if the current token matches the given name.
	bool	matchName( const char* name );
	// Skip a member in an object. Returns false on failure;
	bool	skipMember(void);

	// Primitive parsing methods return false on failure.
	bool	number( int& v );
	bool	number( float& v );
	bool	string( const char*& str );
	bool	boolean( bool& v );
	bool	null(void);

	// The following allow arrays and objects to be parsed without knowing the number of elements in advance, e.g.
	//
	//	if ( !parse.beginObject() )
	//		return false;
	//	while ( parse.more() )
	//	{
	//		if ( parse.matchName( "x" ) )
	//		{
	//			if ( !parse.number( x ) )
	//				return false;
	//		}
	//		else if ( !parse.skipMember() )
	//			return false;
	//	}
	//
	// Start parsing an array. Returns false if current token is not an array.
	bool	beginArray(void);
	// Start parsing an object. Returns false if current token is not an object.
	bool	beginObject(void);
	// Returns true if there is another element (or member) in the current array (or object).
	// Returns false at the end, and moves past it.
	bool	more(void);
	// Skip a value, e.g. an array element. Returns false on failure.
	bool	skipValue(void);
};

#endif // _DISTINGNT_SERIALISATION_H
