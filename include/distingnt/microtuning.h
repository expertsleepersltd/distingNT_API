/*
MIT License

Copyright (c) 2026 Expert Sleepers Ltd

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

#ifndef _DISTINGNT_MICROTUNING_H
#define _DISTINGNT_MICROTUNING_H

#include <stdint.h>
#include <distingnt/api.h>

/*
 * Represents one note in an scl file.
 */
union _NT_sclNote
{
	double			octaves;		// cents/1200, if the note was specified using a floating point number
	struct							// numerator and denominator, if the note was specified using a ratio
	{
		int32_t		numeratorValue;
		int32_t		denominatorValue;
	};

	inline __attribute__((always_inline))	bool		isRatio(void) const { return denominatorValue < 0; }
	inline __attribute__((always_inline))	uint32_t	numerator(void) const { return numeratorValue; }
	inline __attribute__((always_inline))	uint32_t	denominator(void) const { return -denominatorValue; }
};

/*
 * Used to return info from NT_getSclInfo().
 */
struct _NT_sclInfo
{
	const char* 	name;
};

/*
 * Used with NT_readScl().
 */
struct _NT_sclRequest
{
	// Request inputs

	uint32_t 		index;			// Index of the wavetable to load.

	_NT_sclNote*	notes;			// Pointer to memory to hold the notes.
    uint32_t		maxNotes;      	// Maximum number of notes that 'notes' can hold.

    char*			nameBuffer;				// Pointer to memory to hold the scale name (can be NULL).
    size_t			nameBufferSize;			// Size of nameBuffer.
    char*			descriptionBuffer;		// Pointer to memory to hold the scale description (can be NULL).
    size_t			descriptionBufferSize;	// Size of descriptionBuffer.

    void 			(*callback)( void* );	// The function to call when the read is complete.
	void* 			callbackData;			// Data to be passed to the callback function.

	// Request outputs

    bool    		error;			// An error occurred during load.

    uint32_t		numNotes;		// The number of notes in the scl.
};

extern "C" {

/*
 * Get the number of scl files.
 */
uint32_t	NT_getNumScl(void);

/*
 * Get info about the given scl file.
 */
void		NT_getSclInfo( uint32_t index, _NT_sclInfo& info );

/*
 * Read an scl file into memory.
 *
 * Returns true if the read was successfully initiated.
 * The callback will not be called if the function returns false.
 */
bool		NT_readScl( _NT_sclRequest& request );

}

#endif // _DISTINGNT_MICROTUNING_H
