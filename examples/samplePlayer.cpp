
#include <math.h>
#include <string.h>
#include <new>
#include <distingnt/api.h>
#include <distingnt/wav.h>

enum
{
	kParamOutput,
	kParamOutputMode,
	kParamFolder,
	kParamSample,

	kNumParams,
};

static const _NT_parameter	parameters[] = {
	NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output", 1, 13 )
	{ .name = "Folder", .min = 0, .max = 32767, .def = 0, .unit = kNT_unitHasStrings, .scaling = 0, .enumStrings = NULL },
	{ .name = "Sample", .min = 0, .max = 32767, .def = 0, .unit = kNT_unitConfirm, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamFolder, kParamSample };
static const uint8_t page2[] = { kParamOutput, kParamOutputMode };

static const _NT_parameterPage pages[] = {
	{ .name = "Sample", .numParams = ARRAY_SIZE(page1), .params = page1 },
	{ .name = "Routing", .numParams = ARRAY_SIZE(page2), .params = page2 },
};

static const _NT_parameterPages parameterPages = {
	.numPages = ARRAY_SIZE(pages),
	.pages = pages,
};

struct _samplePlayer : public _NT_algorithm
{
	_samplePlayer() {}
	~_samplePlayer() {}

	_NT_parameter	params[ kNumParams ];

	_NT_wavRequest	request;

	bool			cardMounted;
	bool			awaitingCallback;

	float*			dram;

	float			pos;
	float			inc;

	static constexpr int kBufferSizeFrames = 48000;
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_samplePlayer);
	req.dram = _samplePlayer::kBufferSizeFrames * sizeof(float);
	req.dtc = 0;
	req.itc = 0;
}

static void wavCallback( void* callbackData, bool success )
{
	_samplePlayer* pThis = (_samplePlayer*)callbackData;
	pThis->awaitingCallback = false;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	static_assert( kNumParams == ARRAY_SIZE(parameters) );

	_samplePlayer* alg = new (ptrs.sram) _samplePlayer();

	alg->dram = (float*)ptrs.dram;
	memset( alg->dram, 0, req.dram );

	memcpy( alg->params, parameters, sizeof parameters );

	alg->parameters = alg->params;
	alg->parameterPages = &parameterPages;

	alg->request.callback = wavCallback;
	alg->request.callbackData = alg;
	alg->request.bits = kNT_WavBits32;
	alg->request.channels = kNT_WavMono;
	alg->request.progress = kNT_WavProgress;
	alg->request.numFrames = _samplePlayer::kBufferSizeFrames;
	alg->request.startOffset = 0;
	alg->request.dst = alg->dram;

	return alg;
}

int 	parameterString( _NT_algorithm* self, int p, int v, char* buff )
{
	_samplePlayer* pThis = (_samplePlayer*)self;
	int len = 0;
	
	switch ( p )
	{
	case kParamFolder:
	{
		_NT_wavFolderInfo folderInfo;
		NT_getSampleFolderInfo( v, folderInfo );
		if ( folderInfo.name )
		{
			strncpy( buff, folderInfo.name, kNT_parameterStringSize-1 );
			buff[ kNT_parameterStringSize-1 ] = 0;
			len = strlen( buff );
		}
	}
		break;
	case kParamSample:
	{
		_NT_wavInfo info;
		NT_getSampleFileInfo( pThis->v[ kParamFolder ], v, info );
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
	_samplePlayer* pThis = (_samplePlayer*)self;

	switch ( p )
	{
	case kParamFolder:
	{
		// set the maximum value of the sample parameter
		_NT_wavFolderInfo folderInfo;
		NT_getSampleFolderInfo( pThis->v[ kParamFolder ], folderInfo );
		pThis->params[ kParamSample ].max = folderInfo.numSampleFiles - 1;
		NT_updateParameterDefinition( NT_algorithmIndex( self ), kParamSample );
	}
		break;
	case kParamSample:
		if ( !pThis->awaitingCallback )
		{
			_NT_wavInfo info;
			NT_getSampleFileInfo( pThis->v[ kParamFolder ], pThis->v[ kParamSample ], info );
			pThis->inc = info.sampleRate/(float)NT_globals.sampleRate;

			pThis->request.folder = pThis->v[ kParamFolder ];
			pThis->request.sample = pThis->v[ kParamSample ];
			if ( NT_readSampleFrames( pThis->request ) )
				pThis->awaitingCallback = true;
		}
		break;
	}
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_samplePlayer* pThis = (_samplePlayer*)self;

	bool cardMounted = NT_isSdCardMounted();
	if ( pThis->cardMounted != cardMounted )
	{
		pThis->cardMounted = cardMounted;
		if ( cardMounted )
		{
			// set the maximum value of the folder parameter
			pThis->params[ kParamFolder ].max = NT_getNumSampleFolders() - 1;
			NT_updateParameterDefinition( NT_algorithmIndex( self ), kParamFolder );
		}
	}

	int numFrames = numFramesBy4 * 4;
	float* out = busFrames + ( pThis->v[kParamOutput] - 1 ) * numFrames;
	bool replace = pThis->v[kParamOutputMode];

	float pos = pThis->pos;
	float inc = pThis->inc;
	float max = _samplePlayer::kBufferSizeFrames - 1;
	float gain = 8.0f;
	const float* dram = pThis->dram;

	for ( int i=0; i<numFrames; ++i )
	{
		pos += inc;
		if ( pos >= max )
			pos -= max;
		uint32_t index = pos;
		float f = pos - index;
		float s0 = dram[ index ];
		float s1 = dram[ index+1 ];
		float s = s0 + f * ( s1 - s0 );
		s *= gain;
		if ( !replace )
			s += out[i];
		out[i] = s;
	}

	pThis->pos = pos;
}

bool	draw( _NT_algorithm* self )
{
	_samplePlayer* pThis = (_samplePlayer*)self;
	
	int folder = pThis->v[ kParamFolder ];
	int sample = pThis->v[ kParamSample ];

	_NT_wavFolderInfo folderInfo;
	NT_getSampleFolderInfo( folder, folderInfo );
	if ( folderInfo.name )
		NT_drawText( 10, 30, folderInfo.name );

	_NT_wavInfo info;
	NT_getSampleFileInfo( folder, sample, info );
	if ( info.name )
		NT_drawText( 10, 40, info.name );
		
	return false;
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 's', 'p' ),
	.name = "Demo sample player",
	.description = "Plays samples",
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
