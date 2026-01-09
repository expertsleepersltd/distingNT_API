
#include <math.h>
#include <new>
#include <distingnt/api.h>

struct _gainAlgorithm : public _NT_algorithm
{
	_gainAlgorithm() {}
	~_gainAlgorithm() {}
	
	float gain;
};

constexpr int maxCustomGainParam = 1000;

static float customGain( int v )
{
	constexpr float minv = 0.1f;
	constexpr float maxv = 1.0f;
	constexpr float mult = log( maxv/minv )/maxCustomGainParam;
	return minv * exp( mult * v );
}

enum
{
	kParamInput,
	kParamOutput,
	kParamOutputMode,
	kParamGainLinear,
	kParamGainDecibels,
	kParamGainCustom,
};

static const _NT_parameter	parameters[] = {
	NT_PARAMETER_AUDIO_INPUT( "Input", 1, 1 )
	NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output", 1, 13 )
	{ .name = "Linear", .min = 0, .max = 100, .def = 50, .unit = kNT_unitPercent, .scaling = 0, .enumStrings = NULL },
	{ .name = "Decibels", .min = -12, .max = 0, .def = 0, .unit = kNT_unitDb, .scaling = 0, .enumStrings = NULL },
	{ .name = "Custom", .min = 0, .max = maxCustomGainParam, .def = maxCustomGainParam, .unit = kNT_unitHasStrings, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamGainLinear, kParamGainDecibels, kParamGainCustom };
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

int 	parameterString( _NT_algorithm* self, int p, int v, char* buff )
{
	int len = 0;
	
	switch ( p )
	{
	case kParamGainCustom:
		len = NT_floatToString( buff, customGain( v ) );
		break;
	}
	
	return len;
}

void	parameterChanged( _NT_algorithm* self, int p )
{
	_gainAlgorithm* pThis = (_gainAlgorithm*)self;
	if ( p >= kParamGainLinear && p <= kParamGainCustom )
	{
		pThis->gain = pThis->v[kParamGainLinear] / 100.0f;
		pThis->gain *= pow( 10.0f, pThis->v[kParamGainDecibels] / 20.0f );
		pThis->gain *= customGain( pThis->v[kParamGainCustom] );
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
	
	int gain = pThis->gain * 100;
	for ( int i=0; i<gain; ++i )
		NT_screen[ 128 * 20 + i ] = 0xa5;
		
	NT_drawText( 10, 40, "Gain" );
	NT_drawText( 256, 64, "OK", 8, kNT_textRight, kNT_textLarge );
	
	NT_drawShapeI( kNT_line, 20, 45, 50, 55, 8 );
	NT_drawShapeF( kNT_line, 20, 50, 50, 60 );
		
	return false;
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 'g', 'a' ),
	.name = "Gain",
	.description = "Applies gain",
	.numSpecifications = 0,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.parameterChanged = parameterChanged,
	.step = step,
	.draw = draw,
	.tags = kNT_tagUtility,
	.parameterString = parameterString,
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
