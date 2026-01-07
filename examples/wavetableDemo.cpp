
#include <math.h>
#include <string.h>
#include <algorithm>
#include <new>
#include <distingnt/api.h>
#include <distingnt/wav.h>

enum
{
	kParamInput,
	kParamOutput,
	kParamOutputMode,
	kParamWavetable,
	kParamWaveOffset,

	kNumParams,
};

static const _NT_parameter	parameters[] = {
	NT_PARAMETER_CV_INPUT( "CV", 1, 1 )
	NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output", 1, 13 )
	{ .name = "Wavetable", .min = 0, .max = 32767, .def = 0, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "Wave offset", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitPercent, .scaling = kNT_scaling10, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamWavetable, kParamWaveOffset };
static const uint8_t page2[] = { kParamInput, kParamOutput, kParamOutputMode };

static const _NT_parameterPage pages[] = {
	{ .name = "Wavetable", .numParams = ARRAY_SIZE(page1), .params = page1 },
	{ .name = "Routing", .numParams = ARRAY_SIZE(page2), .params = page2 },
};

static const _NT_parameterPages parameterPages = {
	.numPages = ARRAY_SIZE(pages),
	.pages = pages,
};

struct _wavetableDemo : public _NT_algorithm
{
	_wavetableDemo() {}
	~_wavetableDemo() {}

	_NT_parameter	params[ kNumParams ];

	_NT_wavetableRequest	request;

	float			phase;

	bool			cardMounted;
	bool			awaitingCallback;

	static constexpr int kBufferSizeFrames = 256 * 2048;
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_wavetableDemo);
	req.dram = _wavetableDemo::kBufferSizeFrames * sizeof(int16_t);
	req.dtc = 0;
	req.itc = 0;
}

static void requestCallback( void* callbackData )
{
	_wavetableDemo* pThis = (_wavetableDemo*)callbackData;
	pThis->awaitingCallback = false;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	static_assert( kNumParams == ARRAY_SIZE(parameters) );

	_wavetableDemo* alg = new (ptrs.sram) _wavetableDemo();

	alg->request.table = (int16_t*)ptrs.dram;
	alg->request.tableSize = req.dram/sizeof(int16_t);
	memset( alg->request.table, 0, req.dram );

	memcpy( alg->params, parameters, sizeof parameters );

	alg->parameters = alg->params;
	alg->parameterPages = &parameterPages;

	alg->request.callback = requestCallback;
	alg->request.callbackData = alg;

	return alg;
}

void	parameterChanged( _NT_algorithm* self, int p )
{
	_wavetableDemo* pThis = (_wavetableDemo*)self;

	switch ( p )
	{
	case kParamWavetable:
		if ( !pThis->awaitingCallback )
		{
			pThis->request.index = pThis->v[ kParamWavetable ];
			if ( NT_readWavetable( pThis->request ) )
				pThis->awaitingCallback = true;
		}
		break;
	}
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_wavetableDemo* pThis = (_wavetableDemo*)self;

	bool cardMounted = NT_isSdCardMounted();
	if ( pThis->cardMounted != cardMounted )
	{
		pThis->cardMounted = cardMounted;
		if ( cardMounted )
		{
			// set the maximum value of the wavetable parameter
			pThis->params[ kParamWavetable ].max = NT_getNumWavetables() - 1;
			NT_updateParameterDefinition( NT_algorithmIndex( self ), kParamWavetable );
			// cause the wavetable to be loaded
			parameterChanged( self, kParamWavetable );
		}
	}

	if ( pThis->request.error )
		return;

	int numFrames = numFramesBy4 * 4;
	const float* cvIn = busFrames + ( pThis->v[kParamInput] - 1 ) * numFrames;
	float* out = busFrames + ( pThis->v[kParamOutput] - 1 ) * numFrames;
	bool replace = pThis->v[kParamOutputMode];

	// compute the wave index and the fractional interpolation
	// ideally this would be done per-frame or smoothed somehow

	float offset = pThis->v[ kParamWaveOffset ] * pThis->request.numWaves * 0.001f;
	offset = std::min( offset, pThis->request.numWaves - 1 - 1.0e-6f );

	// compute the VCO pitch
	// ideally this would be done per-frame or smoothed somehow

	constexpr float c4 = 440.0f * exp2( ( 48 - 69 )/12.0f );
	float frequency = c4 * exp2( cvIn[0] );
	float inc = frequency / NT_globals.sampleRate;

	_NT_wavetableEvaluation eval = { .offset = offset, .frequency = frequency };

	float gain = 8.0f;

	float p = pThis->phase;

	for ( int i=0; i<numFrames; ++i )
	{
		eval.phase = p;
		p += inc;
		if ( p >= 1.0f )
			p -= 1.0f;

		float v = NT_evaluateWavetable( pThis->request, eval );
		float s = v * gain;
		if ( !replace )
			s += out[i];
		out[i] = s;
	}

	pThis->phase = p;
}

bool	draw( _NT_algorithm* self )
{
	_wavetableDemo* pThis = (_wavetableDemo*)self;
	
	int index = pThis->v[ kParamWavetable ];

	_NT_wavetableInfo info;
	NT_getWavetableInfo( index, info );
	if ( info.name )
		NT_drawText( 10, 30, info.name );
		
    if ( pThis->request.usingMipMaps )
    {
    	// use the 64 x 64 mipmap to draw the waveform
    	float offset = pThis->v[ kParamWaveOffset ] * pThis->request.numWaves * 0.001f;
    	offset = std::min( offset, pThis->request.numWaves - 1 - 1.0e-6f );
    	int wave = (int)offset;
    	float frac = offset - wave;
    	// see the comment in distingnt/wav.h which describes the layout
    	// of the downsampled versions within the table memory
        const int16_t* mip = pThis->request.table + 64 * ( pThis->request.numWaves + wave );
        float xx;
        float yy;
        for ( int i=0; i<64; ++i )
        {
            int v0 = mip[i];
            int v1 = mip[i+64];
            float v = v0 + frac * ( v1 - v0 );
            float x = 192 + i;
            float y = 36.0f - v * (28.0f/32768);
            if ( i > 0 )
            	NT_drawShapeF( kNT_line, xx, yy, x, y );
            xx = x;
            yy = y;
        }
    }

    return false;
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 'w', 't' ),
	.name = "Wavetable demo",
	.description = "Plays a wavetable",
	.numSpecifications = 0,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.parameterChanged = parameterChanged,
	.step = step,
	.draw = draw,
	.tags = kNT_tagInstrument,
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
