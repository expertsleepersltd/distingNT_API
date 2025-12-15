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

#include <math.h>
#include <new>
#include <cstring>
#include <distingnt/api.h>
#include <distingnt/serialisation.h>

struct _gainAlgorithm : public _NT_algorithm
{
	_gainAlgorithm() {}
	~_gainAlgorithm() {}
	
	float gain;
};

enum
{
	kParamInput,
	kParamOutput,
	kParamOutputMode,
	kParamGain,
	kParamGrayDemo,
};

static const _NT_parameter	parameters[] = {
	NT_PARAMETER_AUDIO_INPUT( "Input", 1, 1 )
	NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output", 1, 13 )
	{ .name = "Gain", .min = 0, .max = 100, .def = 50, .unit = kNT_unitPercent, .scaling = 0, .enumStrings = NULL },
	{ .name = "Gray demo", .min = 0, .max = 1, .def = 0, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamGain, kParamGrayDemo };
static const uint8_t page2[] = { kParamInput, kParamOutput, kParamOutputMode };

static const _NT_parameterPage pages[] = {
	{ .name = "Gain", .numParams = ARRAY_SIZE(page1), .params = page1 },
	{ .name = "Routing", .numParams = ARRAY_SIZE(page2), .params = page2 },
};

static const _NT_parameterPages parameterPages = {
	.numPages = ARRAY_SIZE(pages),
	.pages = pages,
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_gainAlgorithm);
	req.dram = 0;
	req.dtc = 0;
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	_gainAlgorithm* alg = new (ptrs.sram) _gainAlgorithm();
	alg->parameters = parameters;
	alg->parameterPages = &parameterPages;
	return alg;
}

void	parameterChanged( _NT_algorithm* self, int p )
{
	_gainAlgorithm* pThis = (_gainAlgorithm*)self;
	switch ( p )
	{
	case kParamGain:
		pThis->gain = pThis->v[kParamGain] / 100.0f;
		break;
	case kParamGrayDemo:
		NT_setParameterGrayedOut( NT_algorithmIndex( self ), kParamGain + NT_parameterOffset(), pThis->v[ kParamGrayDemo ] );
		break;
	}
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_gainAlgorithm* pThis = (_gainAlgorithm*)self;
	float gain = pThis->gain;
	int numFrames = numFramesBy4 * 4;
	const float* in = busFrames + ( pThis->v[kParamInput] - 1 ) * numFrames;
	float* out = busFrames + ( pThis->v[kParamOutput] - 1 ) * numFrames;
	bool replace = pThis->v[kParamOutputMode];
	if ( !replace )
	{
		for ( int i=0; i<numFrames; ++i )
			out[i] += in[i] * gain;
	}
	else
	{
		for ( int i=0; i<numFrames; ++i )
			out[i] = in[i] * gain;
	}
}

bool	draw( _NT_algorithm* self )
{
	_gainAlgorithm* pThis = (_gainAlgorithm*)self;
	
	for ( int i=0; i<pThis->v[kParamGain]; ++i )
		NT_screen[ 128 * 20 + i ] = 0xa5;
		
	NT_drawText( 10, 40, "Turn centre pot for gain" );
		
	return false;
}

uint32_t	hasCustomUi( _NT_algorithm* self )
{
	return kNT_potC;
}

void	customUi( _NT_algorithm* self, const _NT_uiData& data )
{
	if ( data.controls & kNT_potC )
	{
		int value = round( 100.0f * data.pots[1] );
		NT_setParameterFromUi( NT_algorithmIndex( self ), kParamGain + NT_parameterOffset(), value );
	}
}

void	setupUi( _NT_algorithm* self, _NT_float3& pots )
{
	pots[1] = self->v[kParamGain] * 0.01f;
}

void	serialise( _NT_algorithm* self, _NT_jsonStream& stream )
{
	stream.addMemberName( "test_array" );
	stream.openArray();
		stream.addNumber( 1 );
		stream.addString( "banana" );
		stream.addFourCC( NT_MULTICHAR( 'b', 'e', 'e', 'f' ) );
		stream.addBoolean( true );
		stream.addNull();
	stream.closeArray();

	stream.addMemberName( "test_object" );
	stream.openObject();
		stream.addMemberName( "pi" );
		stream.addNumber( 3.1415f );
		stream.addMemberName( "answer" );
		stream.addNumber( 42.0f );
	stream.closeObject();
}

bool	deserialise( _NT_algorithm* self, _NT_jsonParse& parse )
{
	int num;
	if ( !parse.numberOfObjectMembers( num ) )
		return false;
	
	for ( int i=0; i<num; ++i )
	{
		if ( parse.matchName( "test_array" ) )
		{
			int num_a;
			if ( !parse.numberOfArrayElements( num_a ) )
				return false;
			if ( num_a != 5 )
				return false;
			int iv = 0;
			if ( !parse.number( iv ) || ( iv != 1 ) )
				return false;
			const char* str = NULL;
			if ( !parse.string( str ) || ( str == NULL ) )
				return false;
			if ( strcmp( str, "banana" ) != 0 )
				return false;
			if ( !parse.string( str ) || ( str == NULL ) )
				return false;
			if ( strcmp( str, "beef" ) != 0 )
				return false;
			bool b = false;
			if ( !parse.boolean( b ) || ( b != true ) )
				return false;
			if ( !parse.null() )
				return false;
		}
		else if ( parse.matchName( "test_object" ) )
		{
			int num_o;
			if ( !parse.numberOfObjectMembers( num_o ) )
				return false;
			float pi = 0.0f, fv = 0.0f;
			for ( int j=0; j<num_o; ++j )
			{
				if ( parse.matchName( "pi" ) )
				{
					if ( !parse.number( pi ) )
						return false;
				}
				else if ( parse.matchName( "answer" ) )
				{
					if ( !parse.number( fv ) )
						return false;
				}
				else
				{
					if ( !parse.skipMember() )
						return false;
				}
			}
			if ( pi != 3.1415f || fv != 42.0f )
    			return false;
		}
		else
		{
			if ( !parse.skipMember() )
				return false;
		}
	}
	
	return true;
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 'g', 'u' ),
	.name = "Gain (UI)",
	.description = "Applies gain with a custom UI",
	.numSpecifications = 0,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.parameterChanged = parameterChanged,
	.step = step,
	.draw = draw,
	.tags = kNT_tagUtility,
	.hasCustomUi = hasCustomUi,
	.customUi = customUi,
	.setupUi = setupUi,
	.serialise = serialise,
	.deserialise = deserialise,
};

uintptr_t pluginEntry( _NT_selector selector, uint32_t data )
{
	switch ( selector )
	{
	case kNT_selector_version:
		return kNT_apiVersionCurrent;
	case kNT_selector_numFactories:
		return 1;
	case kNT_selector_factoryInfo:
		return (uintptr_t)( ( data == 0 ) ? &factory : NULL );
	}
	return 0;
}
