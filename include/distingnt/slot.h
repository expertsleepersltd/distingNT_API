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

#ifndef _DISTINGNT_SLOT_H_
#define _DISTINGNT_SLOT_H_

#include <stdint.h>

class _NT_slot
{
private:
	// disallow copy and assign
	_NT_slot(const _NT_slot&);
	void operator=(const _NT_slot&);

#ifdef _DISTINGNT_SLOT_INTERNAL
public:
#endif
	void*		refCon;

public:
	_NT_slot() : refCon( NULL ) {}
	~_NT_slot() {}

	// return the algorithm name
	const char*		name(void) const;
	// return the algorithm guid
    uint32_t		guid(void) const;
    // return the plug-in object, if the algorithm is a plug-in, else NULL
    _NT_algorithm*	plugin(void) const;

    // return the algorithm's number of parameters
    uint32_t		numParameters(void) const;
    // fetch info about the indexed parameter
    // returns false if index out of range
    bool			parameterInfo( _NT_parameter& info, uint32_t index ) const;
    // return the value of an algorithm's parameter, before any mapping is applied
    int16_t			parameterPresetValue( uint32_t index ) const;
    // return the value of an algorithm's parameter, including the effect of any mapping
    int16_t			parameterValue( uint32_t index ) const;
    // fill in buff[] with a null-terminated string representing the parameter value, and return the string length
    // returns 0 if there is no string e.g. if the value is a simple numeric value
    // buff[] should be at least of size kNT_parameterStringSize
    uint32_t		parameterString( uint32_t index, char* buff ) const;

};

#endif /* _DISTINGNT_SLOT_H_ */
