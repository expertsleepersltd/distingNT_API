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

#ifndef _DISTINGNT_API_H
#define _DISTINGNT_API_H

#include <stdint.h>

/*
 * API version. Plug-ins would typically be compiled against kNT_apiVersionCurrent.
 */
enum _NT_version
{
	kNT_apiVersion1 			= 1,
	kNT_apiVersion2,
	kNT_apiVersion3,				// Adds MIDI handling. Compatible with v2.
	kNT_apiVersion4,				// Add specifications. Breaks compatibility.
	kNT_apiVersion5,				// Add tags. Compatible with v4.
	kNT_apiVersion6,				// Add custom UI support. Compatible with v4-5.
	kNT_apiVersion7,				// Add serialisation. Compatible with v4-6.
	kNT_apiVersion8,				// Change hasCustomUi() to return uint32_t; remove _NT_pots; rename _NT_uiData.buttons; remove  _NT_uiData.potChange. Compatible with v4-7.
	kNT_apiVersion9,				// Add midiSysEx. Compatible with v4-8.
	kNT_apiVersion10,				// Add parameterUiPrefixCallback. Compatible with v4-9.

	kNT_apiVersionCurrent 		= kNT_apiVersion10
};

/*
 * selector argument to pluginEntry()
 */
enum _NT_selector
{
	kNT_selector_version,			// Return the API version this plug-in was compiled against. Value from _NT_version.
	kNT_selector_numFactories,		// Return the number of factories defined by this plug-in.
	kNT_selector_factoryInfo,		// Return a pointer to an _NT_factory.
};

/*
 * Text size enum used in drawing.
 */
enum _NT_textSize
{
	kNT_textTiny,					// Tiny 3x5 pixel font.
	kNT_textNormal,					// Normal 8 point font, bitmapped.
	kNT_textLarge,					// Large 21 point font, antialiased.
};

/*
 * Text alignment enum used in drawing.
 */
enum _NT_textAlignment
{
	kNT_textLeft,					// Left justified.
	kNT_textCentre,					// Centre justified.
	kNT_textRight,					// Right justified.
};

/*
 * Shape choice enum used in drawing.
 */
enum _NT_shape
{
	kNT_point,
	kNT_line,
	kNT_box,			// unfilled
	kNT_rectangle,		// filled
	kNT_circle,			// unfilled
};

/*
 * Bitmask for MIDI destinations.
 */
enum _NT_midiDestination
{
	kNT_destinationBreakout 	= (1<<0),
	kNT_destinationSelectBus 	= (1<<1),
	kNT_destinationUSB 			= (1<<2),
	kNT_destinationInternal 	= (1<<3),
};

/*
 * Macro to construct the four character guids used to identify plug-ins.
 */
#define NT_MULTICHAR( a, b, c, d )	( ( (uint32_t)a << 0 ) | ( (uint32_t)b << 8 ) | ( (uint32_t)c << 16 ) | ( (uint32_t)d << 24 ) )

#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/*
 * Globals structure. Plug-ins may always rely on data in the global NT_globals.
 */
struct _NT_globals
{
	uint32_t	sampleRate;				// sample rate in Hz
	uint32_t	maxFramesPerStep;		// maximum number of frames per step() call
	float*		workBuffer;				// buffer available for internal use during step() - does not persist
	uint32_t	workBufferSizeBytes;	// size of workBuffer in bytes
};

/*
 * Structure used by calculateStaticRequirements() and initialise().
 */
struct _NT_staticRequirements
{
	uint32_t	dram;					// Amount of DRAM required.
};

/*
 * Structure used by initialise().
 */
struct _NT_staticMemoryPtrs
{
	uint8_t*	dram;					// Pointer to DRAM allocated to the plug-in.
};

/*
 * Structure used by calculateRequirements() and construct().
 */
struct _NT_algorithmRequirements
{
	uint32_t	numParameters;			// The number of parameters this plug-in has.
	uint32_t	sram;					// Amount of SRAM required.
	uint32_t	dram;					// Amount of DRAM required. Use for large allocations.
	uint32_t	dtc;					// Amount of DTC required. Use for performance-critical data allocations.
	uint32_t	itc;					// Amount of ITC required. Use for code only.
};

/*
 * Structure used by construct().
 */
struct _NT_algorithmMemoryPtrs
{
	uint8_t*	sram;					// Pointer to SRAM allocated to the plug-in.
	uint8_t*	dram;					// Pointer to DRAM allocated to the plug-in.
	uint8_t*	dtc;					// Pointer to DTC allocated to the plug-in.
	uint8_t*	itc;					// Pointer to ITC allocated to the plug-in.
};

/*
 * Values for the unit field of _NT_parameter.
 */
enum
{
	kNT_unitNone,
	kNT_unitEnum,						// enumStrings must also be provided
	kNT_unitDb,
	kNT_unitDb_minInf,					// minimum value will be shown as "-inf" dB
	kNT_unitPercent,
	kNT_unitHz,
	kNT_unitSemitones,
	kNT_unitCents,
	kNT_unitMs,
	kNT_unitSeconds,
	kNT_unitFrames,
	kNT_unitMIDINote,
	kNT_unitMillivolts,
	kNT_unitVolts,
	kNT_unitBPM,

	// These values should only be used via the NT_PARAMETER_AUDIO_INPUT() etc. macros.
	kNT_unitAudioInput = 100,
	kNT_unitCvInput,
	kNT_unitAudioOutput,
	kNT_unitCvOutput,
	kNT_unitOutputMode,
};

/*
 * Values for the scaling field of _NT_parameter.
 */
enum
{
	kNT_scalingNone,
	kNT_scaling10,
	kNT_scaling100,
	kNT_scaling1000,
};

/*
 * Structure to define an algorithm parameter.
 */
struct _NT_parameter
{
	const char* 			name;			// Name
	int16_t	 				min;			// Minimum value
	int16_t					max;			// Maximum value
	int16_t					def;			// Default value
	uint8_t     			unit;			// Unit
	uint8_t					scaling;		// Scaling
	char const * const *	enumStrings;	// Array of strings, if unit is kNT_unitEnum
};

/*
 * Structure to define a page of parameters.
 */
struct _NT_parameterPage
{
	const char* 	name;					// Name of the page
	uint8_t			numParams;				// Number of parameters on this page
	const uint8_t*	params;					// Pointer to array of parameter indices
};

/*
 * Structure to define all the parameter pages of an algorithm.
 */
struct _NT_parameterPages
{
	uint32_t					numPages;	// Number of pages
	const _NT_parameterPage*	pages;		// Pointer to array of pages
};

// These two macros are used to build the following macros
#define NT_PARAMETER_IO( n, m, d, u )	\
		{ .name = n, .min = m, .max = 28, .def = d, .unit = u, .scaling = 0, .enumStrings = NULL },
#define NT_PARAMETER_OUTPUT_MODE( n )	\
		{ .name = n, .min = 0, .max = 1, .def = 0, .unit = kNT_unitOutputMode, .scaling = 0, .enumStrings = NULL },

/*
 * Macros to define algorithm inputs and outputs.
 *
 * n	Parameter name
 * m	Parameter minimum (typically 0 if no input is allowed, otherwise 1)
 * d	Default value.
 */
#define NT_PARAMETER_AUDIO_INPUT( n, m, d )		NT_PARAMETER_IO( n, m, d, kNT_unitAudioInput )
#define NT_PARAMETER_CV_INPUT( n, m, d )		NT_PARAMETER_IO( n, m, d, kNT_unitCvInput )
#define NT_PARAMETER_AUDIO_OUTPUT( n, m, d )	NT_PARAMETER_IO( n, m, d, kNT_unitAudioOutput )
#define NT_PARAMETER_CV_OUTPUT( n, m, d )		NT_PARAMETER_IO( n, m, d, kNT_unitCvOutput )
#define NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( n, m, d )	\
	NT_PARAMETER_IO( n, m, d, kNT_unitAudioOutput )		\
	NT_PARAMETER_OUTPUT_MODE( n " mode" )
#define NT_PARAMETER_CV_OUTPUT_WITH_MODE( n, m, d )		\
	NT_PARAMETER_IO( n, m, d, kNT_unitCvOutput )		\
	NT_PARAMETER_OUTPUT_MODE( n " mode" )

/*
 * Structure that defines an algorithm instance.
 * Plug-ins will typically sub-class this.
 */
struct _NT_algorithm
{
	const _NT_parameter*		parameters;			// Pointer to the algorithm's parameters. Populate in construct().
	const _NT_parameterPages*	parameterPages;		// Pointer to the algorithm's parameter pages. Populate in construct().
	const int16_t*				vIncludingCommon;	// Pointer to the algorithm's parameter values. Managed by the system. Includes common parameters e.g. bypass.
	const int16_t*				v;					// Pointer to the algorithm's parameter values. Managed by the system. Starts at the algorithm's own first parameter.
};

/*
 * Values for the type field of _NT_specification.
 */
enum
{
	kNT_typeGeneric,
	kNT_typeSeconds,
	kNT_typeBoolean,
};

/*
 * Structure that defines an algorithm specification.
 */
struct _NT_specification
{
	const char* 	name;							// The specification name
	int32_t			min;							// Minimum value
	int32_t			max;							// Maximum value
	int32_t			def;							// Default value
	int32_t			type;							// Type (kNT_typeGeneric etc.)
};

/*
 * Values for the tags field of _NT_factory.
 */
enum _NT_tag
{
	kNT_tagInstrument		= (1<<0),
	kNT_tagEffect			= (1<<1),
	kNT_tagReverb			= (1<<2),
	kNT_tagDelay			= (1<<3),
	kNT_tagUtility			= (1<<4),
	kNT_tagFilterEQ			= (1<<5),
};

/*
 * Values for the buttons field of _NT_uiData, and for the return value from hasCustomUi().
 */
enum _NT_controls
{
	kNT_button1			= (1<<0),
	kNT_button2			= (1<<1),
	kNT_button3			= (1<<2),
	kNT_button4			= (1<<3),
	kNT_potButtonL		= (1<<4),
	kNT_potButtonC		= (1<<5),
	kNT_potButtonR		= (1<<6),
	kNT_encoderButtonL	= (1<<7),
	kNT_encoderButtonR	= (1<<8),
	kNT_encoderL		= (1<<9),
	kNT_encoderR		= (1<<10),
	kNT_potL			= (1<<11),
	kNT_potC			= (1<<12),
	kNT_potR			= (1<<13),
};

/*
 * Structure supplied to customUi().
 */
struct _NT_uiData
{
	float		pots[3];		// current pot positions [0.0-1.0]
	uint16_t	controls;		// current button states, and which pots changed (_NT_controls)
	uint16_t 	lastButtons;	// previous button states
	int8_t		encoders[2];	// encoder change ±1 or 0
	uint8_t		unused[2];
};

typedef float _NT_float3[3];

/*
 * The minimum buffer size passed to parameterUiPrefixCallback().
 */
enum { kNT_parameterUiPrefixSize = 16 };

/*
 * Structure that defines an algorithm factory.
 *
 * Returned from pluginEntry().
 *
 * All plug-in guids should include at least one capital letter. All-lower-case guids are reserved for built-in algorithms.
 */
struct _NT_factory
{
	uint32_t					guid;				// Algorithm guid. Use NT_MULTICHAR(). Four ASCII characters - stored in the JSON as a string.
	const char*					name;				// Algorithm name
	const char*					description;		// Algorithm description
	uint32_t					numSpecifications;	// The number of elements in specifications[], if not NULL
	const _NT_specification*	specifications;		// Algorithm specifications (can be NULL)

	/*
	 * Called during plug-in scan, and loading.
	 * This call gives the plug-in an opportunity to reserve memory that will be shared by all plug-in instances,
	 * for example lookup tables.
	 */
	void			(*calculateStaticRequirements)( _NT_staticRequirements& req );

	/*
	 * Called during plug-in scan, and loading.
	 * req is the structure that the plug-in populated in calculateStaticRequirements().
	 * ptrs contains pointers to memory allocations that satisfy the requirements.
	 * Typically this call would prepare any data shared by all instances of the plug-in.
	 */
	void			(*initialise)( _NT_staticMemoryPtrs& ptrs, const _NT_staticRequirements& req );

	/*
	 * Called by the system to determine how much memory a new plug-in instance will require.
	 * This will be during preset loading, or when browsing the 'add algorithm' or 'respecify' menus.
	 * It should be a lightweight function as it may be called frequently.
	 */
	void			(*calculateRequirements)( _NT_algorithmRequirements& req, const int32_t* specifications );

	/*
	 * Called to construct a new plug-in instance. Not optional - this method must be implemented.
	 * req is the structure that the plug-in populated in calculateRequirements().
	 * ptrs contains pointers to memory allocations that satisfy the requirements.
	 * The function should use one of the allocations to prepare an _NT_algorithm (or subclass thereof),
	 * and return a pointer to this object.
	 */
	_NT_algorithm*	(*construct)( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications );

	/*
	 * Called by the host when a parameter value changes.
	 * p is the parameter index.
	 * Typically the plug-in might use the value in self->v[] to compute and cache some derived value.
	 */
	void			(*parameterChanged)( _NT_algorithm* self, int p );

	/*
	 * Primary audio rendering call.
	 * busFrames gives access to all 28 busses.
	 * numFramesBy4 is the number of frames to render, divided by 4.
	 * The busses are arranged one after the other.
	 * i.e. numFrames of bus 0, then numFrames of bus 1, etc.
	 */
	void 			(*step)( _NT_algorithm* self, float* busFrames, int numFramesBy4 );

	/*
	 * Called by the host to allow the plug-in to render a custom display.
	 * Return true to suppress the standard parameter line that is drawn at the top of the screen.
	 */
    bool			(*draw)( _NT_algorithm* self );

    /*
     * Called by the host to allow the plug-in to respond to realtime MIDI messages e.g. clock.
     */
    void			(*midiRealtime)( _NT_algorithm* self, uint8_t byte );

    /*
     * Called by the host to allow the plug-in to respond to MIDI messages.
     * This will only be called for messages 0x80 to 0xe0, the typical
     * two or three byte messages that include a MIDI channel.
     */
    void			(*midiMessage)( _NT_algorithm* self, uint8_t byte0, uint8_t byte1, uint8_t byte2 );

    uint32_t		tags;			// Logical OR of _NT_tag

    /*
     * Called by the host to allow the plug-in to define a custom user interface.
     * Return from hasCustomUi() a logical OR of _NT_controls which the plug-in overrides.
     * If the return is non-zero, customUi() will then be called.
     */
    uint32_t		(*hasCustomUi)( _NT_algorithm* self );
    void			(*customUi)( _NT_algorithm* self, const _NT_uiData& data );

    /*
     * If the plug-in uses customUi() to redefine the behaviour of the pots,
     * it can also define this function, which  is called whenever the algorithm’s UI
     * appears for the first time (for example, when you switch from the overview display
     * to the algorithm display) in order to sync up the soft takeover values.
     * Write the current pot values (in the range 0.0-1.0) into pots[].
     */
    void			(*setupUi)( _NT_algorithm* self, _NT_float3& pots );

    /*
     * Called by the host to allow the plug-in to write custom data to the preset JSON.
     * When called, the stream is within a JSON object, so the first call within serialise()
     * should be addMemberName().
     */
    void			(*serialise)( _NT_algorithm* self, class _NT_jsonStream& stream );

    /*
     * Called by the host to allow the plug-in to parse the data it wrote in serialise().
     * Return boolean success.
     */
    bool			(*deserialise)( _NT_algorithm* self, class _NT_jsonParse& parse );

    /*
     * Called by the host to allow the plug-in to respond to MIDI
     * System Exclusive messages.
     */
    void			(*midiSysEx)( const uint8_t* message, uint32_t count );

    /*
     * Called by the host to allow the algorithm to specify a parameter prefix string,
     * for example in the mapping menu.
     * Typically used to prefix a channel number in multi-channel algorithms.
     * The plug-in should fill buff[] with a NULL-terminated string and
     * return its length, or simply return 0 if there is no prefix.
     * The size of buff is guaranteed to be at least kNT_parameterUiPrefixSize bytes.
     */
	int 			(*parameterUiPrefix)( _NT_algorithm* self, int p, char* buff );
};

extern "C" {

/*
 * Primary interface between system and plug-in, used to obtain the factories.
 */
typedef uintptr_t (_pluginEntry)( _NT_selector selector, uint32_t data );
uintptr_t 	pluginEntry( _NT_selector selector, uint32_t data );

/*
 * Provided mainly as a convenience for Faust programs.
 *
 * Given float parameter requirements, populate the _NT_parameter fields scaling, min, max, and def.
 */
void		NT_setParameterRange( _NT_parameter* ptr, float init, float min, float max, float step );

// global structure - always available
extern const _NT_globals NT_globals;

// Utility function, mainly for profiling.
uint32_t	NT_getCpuCycleCount(void);

// return the index of the given algorithm, or -1 if not found.
int32_t		NT_algorithmIndex( const _NT_algorithm* algorithm );

// return the number of algorithms in the preset.
uint32_t	NT_algorithmCount(void);

/*
 * Populates an opaque wrapper class allowing access to an algorithm in the preset.
 * Returns boolean success (iff index < NT_algorithmCount()).
 */
bool		NT_getSlot( class _NT_slot& slot, uint32_t index );

// Set an algorithm parameter.
// May be called from a plug-in's step(), parameterChanged(), midiRealtime(), or midiMessage().
void		NT_setParameterFromAudio( uint32_t algorithmIndex, uint32_t parameter, int16_t value );

// Set an algorithm parameter. Safe to call from anywhere.
void		NT_setParameterFromUi( uint32_t algorithmIndex, uint32_t parameter, int16_t value );

// Set an algorithm parameter's "grayed out" status.
void		NT_setParameterGrayedOut( uint32_t algorithmIndex, uint32_t parameter, bool gray );

// Returns the offset between plug-in parameter indices and global parameter indices.
// Typically used as e.g. NT_setParameterFromUi( NT_algorithmIndex(self), param + NT_parameterOffset(), value ).
uint32_t	NT_parameterOffset(void);

/*
 * Call to let the host know that the indexed parameter definition has changed.
 * i.e. self->parameters[ parameterIndex ] has been modified.
 */
void		NT_updateParameterDefinition( uint32_t algorithmIndex, uint32_t parameterIndex );

// drawing - use from within draw() only
//

// direct access to screen memory
// screen is 256x64 - each byte contains two pixels
extern uint8_t NT_screen[128*64];

// draw text using internal font
void		NT_drawText( int x, int y, const char* str, int colour=15, _NT_textAlignment align=kNT_textLeft, _NT_textSize size=kNT_textNormal );

// draw a shape (integer coordinates, not antialiased)
void		NT_drawShapeI( _NT_shape shape, int x0, int y0, int x1, int y1, int colour=15 );

// draw a shape (float coordinates, antialiased)
void		NT_drawShapeF( _NT_shape shape, float x0, float y0, float x1, float y1, float colour=15 );

// string formatting helpers
// NT_xxxToString() return the length of the resulting string
//

int			NT_intToString( char* buffer, int32_t value );
int			NT_floatToString( char* buffer, float value, int decimalPlaces=2 );

// MIDI
// May be called from a plug-in's step(), parameterChanged(), midiRealtime(), or midiMessage().
// destination is a logical OR of _NT_midiDestination values.
//

void		NT_sendMidiByte( uint32_t destination, uint8_t b0 );
void		NT_sendMidi2ByteMessage( uint32_t destination, uint8_t b0, uint8_t b1 );				// e.g. aftertouch, b0 = 0xD0
void		NT_sendMidi3ByteMessage( uint32_t destination, uint8_t b0, uint8_t b1, uint8_t b2 );	// e.g. CC, b0 = 0xB0

// Sends a System Exclusive message.
// NT_sendMidiSysEx() may be called repeatedly with partial messages, with 'end' set to false.
// Set 'end' to true for the final call. You do not need to include the 0xF7 byte to end the SysEx message.
void		NT_sendMidiSysEx( uint32_t destination, const uint8_t* data, uint32_t count, bool end );

}

#endif // _DISTINGNT_API_H
