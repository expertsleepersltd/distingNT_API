
#include <math.h>
#include <string.h>
#include <algorithm>
#include <new>
#include <distingnt/microtuning.h>
#include <distingnt/wav.h>

enum
{
	kParamInput,
	kParamOutput,
	kParamOutputMode,
	kParamScl,

	kNumParams,
};

static const _NT_parameter	parameters[] = {
	NT_PARAMETER_CV_INPUT( "CV", 1, 1 )
	NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output", 1, 13 )
	{ .name = "Scl file", .min = 0, .max = 32767, .def = 0, .unit = kNT_unitConfirm, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamScl };
static const uint8_t page2[] = { kParamInput, kParamOutput, kParamOutputMode };

static const _NT_parameterPage pages[] = {
	{ .name = "Microtuning", .numParams = ARRAY_SIZE(page1), .params = page1 },
	{ .name = "Routing", .numParams = ARRAY_SIZE(page2), .params = page2 },
};

static const _NT_parameterPages parameterPages = {
	.numPages = ARRAY_SIZE(pages),
	.pages = pages,
};

struct _microtuningDemo : public _NT_algorithm
{
	_microtuningDemo() {}
	~_microtuningDemo() {}

	_NT_parameter	params[ kNumParams ];

	_NT_sclRequest	request;

	_NT_sclNote		notes[100];
	char 			name[22];
	char			description[44];

	bool			cardMounted;
	bool			awaitingCallback;
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_microtuningDemo);
	req.dram = 0;
	req.dtc = 0;
	req.itc = 0;
}

static void requestCallback( void* callbackData )
{
	_microtuningDemo* pThis = (_microtuningDemo*)callbackData;
	pThis->awaitingCallback = false;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	static_assert( kNumParams == ARRAY_SIZE(parameters) );

	_microtuningDemo* alg = new (ptrs.sram) _microtuningDemo();

	alg->request.notes = alg->notes;
	alg->request.maxNotes = ARRAY_SIZE(alg->notes);
	alg->request.nameBuffer = alg->name;
	alg->request.nameBufferSize = sizeof alg->name;
	alg->request.descriptionBuffer = alg->description;
	alg->request.descriptionBufferSize = sizeof alg->description;

	memcpy( alg->params, parameters, sizeof parameters );

	alg->parameters = alg->params;
	alg->parameterPages = &parameterPages;

	alg->request.callback = requestCallback;
	alg->request.callbackData = alg;

	return alg;
}

int 	parameterString( _NT_algorithm* self, int p, int v, char* buff )
{
	int len = 0;

	switch ( p )
	{
	case kParamScl:
	{
		_NT_sclInfo info;
		NT_getSclInfo( v, info );
		if ( info.name )
		{
			strncpy( buff, info.name, kNT_parameterStringSize-1 );
			buff[ kNT_parameterStringSize-1 ] = 0;
			len = strlen( buff );
		}
	}
		break;
	}

	return len;
}

void	parameterChanged( _NT_algorithm* self, int p )
{
	_microtuningDemo* pThis = (_microtuningDemo*)self;

	switch ( p )
	{
	case kParamScl:
		if ( !pThis->awaitingCallback )
		{
			pThis->request.index = pThis->v[ kParamScl ];
			if ( NT_readScl( pThis->request ) )
				pThis->awaitingCallback = true;
		}
		break;
	}
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_microtuningDemo* pThis = (_microtuningDemo*)self;

	bool cardMounted = NT_isSdCardMounted();
	if ( pThis->cardMounted != cardMounted )
	{
		pThis->cardMounted = cardMounted;
		if ( cardMounted )
		{
			// set the maximum value of the wavetable parameter
			pThis->params[ kParamScl ].max = NT_getNumScl() - 1;
			NT_updateParameterDefinition( NT_algorithmIndex( self ), kParamScl );
			// cause the scl to be loaded
			parameterChanged( self, kParamScl );
		}
	}

	if ( pThis->request.error )
		return;

	int numFrames = numFramesBy4 * 4;
//	const float* cvIn = busFrames + ( pThis->v[kParamInput] - 1 ) * numFrames;
	float* out = busFrames + ( pThis->v[kParamOutput] - 1 ) * numFrames;
	bool replace = pThis->v[kParamOutputMode];

	if ( replace )
		memset( out, 0, numFrames * sizeof(float) );
}

bool	draw( _NT_algorithm* self )
{
	_microtuningDemo* pThis = (_microtuningDemo*)self;

	char buff[16];

	NT_intToString( buff, pThis->request.numNotes );
	NT_drawText( 0, 20, "Notes:" );
	NT_drawText( 50, 20, buff );

	NT_drawText( 0, 30, pThis->name );
	NT_drawText( 0, 40, pThis->description );

	return false;
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 'm', 't' ),
	.name = "Microtuning demo",
	.description = "Loads scl files",
	.numSpecifications = 0,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.parameterChanged = parameterChanged,
	.step = step,
	.draw = draw,
	.tags = kNT_tagInstrument,
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
