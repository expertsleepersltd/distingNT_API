
#include <math.h>
#include <string.h>
#include <new>
#include <distingnt/api.h>

enum { kMaxChannels = 8 };

enum
{
	kParamInput,
	kParamOutput,
	kParamOutputMode,
	
	kNumCommonParameters,
};

enum
{
	kParamGain,
	
	kNumPerChannelParameters,
};

static const _NT_parameter	commonParameters[] = {
	NT_PARAMETER_AUDIO_INPUT( "Input", 1, 1 )
	NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output", 1, 13 )
};

static const _NT_parameter	perChannelParameters[] = {
	{ .name = "Gain", .min = 0, .max = 100, .def = 50, .unit = kNT_unitPercent, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t routingParams[] = { kParamInput, kParamOutput, kParamOutputMode };

static char const * const pageNames[] = {
	"Channel 1",
	"Channel 2",
	"Channel 3",
	"Channel 4",
	"Channel 5",
	"Channel 6",
	"Channel 7",
	"Channel 8",
};

struct _gainAlgorithm : public _NT_algorithm
{
	_gainAlgorithm( int32_t numChannels_ )
	: numChannels( numChannels_ )
	{
		pagesDefs.numPages = 1 + numChannels;
		pagesDefs.pages = pageDefs;
		
		memcpy( parameterDefs, commonParameters, kNumCommonParameters * sizeof(_NT_parameter) );
		for ( uint32_t i=0; i<numChannels; ++i )
		{
			memcpy( parameterDefs + kNumCommonParameters + i * kNumPerChannelParameters, perChannelParameters, kNumPerChannelParameters * sizeof(_NT_parameter) );
			
			pageDefs[i].name = pageNames[i];
			pageDefs[i].numParams = kNumPerChannelParameters;
			uint8_t* p = pageParams + i * kNumPerChannelParameters;
			pageDefs[i].params = p;
			for ( int j=0; j<kNumPerChannelParameters; ++j )
				p[ j ] = kNumCommonParameters + i * kNumPerChannelParameters + j;
		}
		
		pageDefs[numChannels].name = "Routing";
		pageDefs[numChannels].numParams = ARRAY_SIZE(routingParams);
		pageDefs[numChannels].params = routingParams;

		// populate the _NT_algorithm members			
		parameters = parameterDefs;
		parameterPages = &pagesDefs;
	}
	~_gainAlgorithm() {}
	
	uint32_t			numChannels;
	
	_NT_parameter		parameterDefs[ kNumCommonParameters + kMaxChannels * kNumPerChannelParameters ];
	
	_NT_parameterPages	pagesDefs;
	_NT_parameterPage	pageDefs[ 1 + kMaxChannels ];
	uint8_t				pageParams[ kMaxChannels * kNumPerChannelParameters ];
	
	float 				gains[kMaxChannels];
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	int32_t numChannels = specifications[0];

	req.numParameters = kNumCommonParameters + numChannels * kNumPerChannelParameters;
	req.sram = sizeof(_gainAlgorithm);
	req.dram = 0;
	req.dtc = 0;
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	int32_t numChannels = specifications[0];
	
	_gainAlgorithm* alg = new (ptrs.sram) _gainAlgorithm( numChannels );
	return alg;
}

void	parameterChanged( _NT_algorithm* self, int p )
{
	_gainAlgorithm* pThis = (_gainAlgorithm*)self;
	if ( p >= kNumCommonParameters )
	{
		int ch = p - kNumCommonParameters;
		pThis->gains[ ch ] = pThis->v[ p ] / 100.0f;
	}
}

int 	parameterUiPrefix( _NT_algorithm* self, int p, char* buff )
{
	int len = 0;
	if ( p >= kNumCommonParameters )
	{
		int ch = p - kNumCommonParameters;
		len = NT_intToString( buff, 1+ch );
		buff[len++] = ':';
		buff[len] = 0;
	}
	return len;
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_gainAlgorithm* pThis = (_gainAlgorithm*)self;
	int numFrames = numFramesBy4 * 4;
	const float* in = busFrames + ( pThis->v[kParamInput] - 1 ) * numFrames;
	float* out = busFrames + ( pThis->v[kParamOutput] - 1 ) * numFrames;
	bool replace = pThis->v[kParamOutputMode];
	for ( uint32_t ch=0; ch<pThis->numChannels; ++ch )
	{
		float gain = pThis->gains[ ch ];
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
		in += numFrames;
		out += numFrames;
	}
}

static const _NT_specification specifications[] = {
	{ .name = "Channels", .min = 1, .max = kMaxChannels, .def = 1, .type = kNT_typeGeneric },
};

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 'g', 'm' ),
	.name = "Gain (multi-channel)",
	.description = "Applies gain",
	.numSpecifications = ARRAY_SIZE(specifications),
	.specifications = specifications,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.parameterChanged = parameterChanged,
	.step = step,
	.draw = NULL,
	.parameterUiPrefix = parameterUiPrefix,
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
