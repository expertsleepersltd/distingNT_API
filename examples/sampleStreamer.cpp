
#include <math.h>
#include <string.h>
#include <new>
#include <distingnt/api.h>
#include <distingnt/wav.h>

enum
{
	kParamOutputL,
	kParamOutputR,
	kParamOutputMode,
	kParamFolder,
	kParamSample,

	kNumParams,
};

static const _NT_parameter	parameters[] = {
	NT_PARAMETER_AUDIO_OUTPUT( "Output L", 1, 13 )
	NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
	{ .name = "Folder", .min = 0, .max = 32767, .def = 0, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "Sample", .min = 0, .max = 32767, .def = 0, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamFolder, kParamSample };
static const uint8_t page2[] = { kParamOutputL, kParamOutputR, kParamOutputMode };

static const _NT_parameterPage pages[] = {
	{ .name = "Sample", .numParams = ARRAY_SIZE(page1), .params = page1 },
	{ .name = "Routing", .numParams = ARRAY_SIZE(page2), .params = page2 },
};

static const _NT_parameterPages parameterPages = {
	.numPages = ARRAY_SIZE(pages),
	.pages = pages,
};

struct  _samplePlayerDTC
{
	float			inc;
	bool			cardMounted;

	// variable size memory block
	uint32_t		streams[1];

	_NT_stream		stream( uint32_t index ) { return (uint8_t*)streams + index * NT_globals.streamSizeBytes; }
};

struct _samplePlayerDRAM
{
	// variable size memory block
	uint32_t	 	streamBuffers[1];

	void*			streamBuffer( uint32_t index ) { return (uint8_t*)streamBuffers + index * NT_globals.streamBufferSizeBytes; }
};

struct _samplePlayer : public _NT_algorithm
{
	_samplePlayer() {}
	~_samplePlayer() {}

	_NT_parameter	params[ kNumParams ];

	_samplePlayerDTC*	dtc;
	_samplePlayerDRAM*	dram;

	static constexpr int kBufferSizeFrames = 48000;
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	constexpr int numStreams = 1;

	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_samplePlayer);
	req.dram = sizeof(_samplePlayerDRAM) + numStreams * NT_globals.streamBufferSizeBytes - sizeof(uint32_t);
	req.dtc = sizeof(_samplePlayerDTC) + numStreams * NT_globals.streamSizeBytes - sizeof(uint32_t);
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	static_assert( kNumParams == ARRAY_SIZE(parameters) );

	_samplePlayer* alg = new (ptrs.sram) _samplePlayer();

	alg->dram = (_samplePlayerDRAM*)ptrs.dram;
	alg->dtc = (_samplePlayerDTC*)ptrs.dtc;

	memcpy( alg->params, parameters, sizeof parameters );

	alg->parameters = alg->params;
	alg->parameterPages = &parameterPages;

	return alg;
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
	{
		_NT_wavInfo info;
		NT_getSampleFileInfo( pThis->v[ kParamFolder ], pThis->v[ kParamSample ], info );
		pThis->dtc->inc = info.sampleRate/(float)NT_globals.sampleRate;

		_NT_streamOpenData data = {
				.streamBuffer = pThis->dram->streamBuffer( 0 ),
				.folder = (uint32_t)pThis->v[ kParamFolder ],
				.sample = (uint32_t)pThis->v[ kParamSample ],
				.velocity = 1.0f,
				.startOffset = 0,
				.reverse = false,
				.rrMode = kNT_RRModeSequential,
		};
		NT_streamOpen( pThis->dtc->stream( 0 ), data );
	}
		break;
	}
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_samplePlayer* pThis = (_samplePlayer*)self;

	bool cardMounted = NT_isSdCardMounted();
	if ( pThis->dtc->cardMounted != cardMounted )
	{
		pThis->dtc->cardMounted = cardMounted;
		if ( cardMounted )
		{
			// set the maximum value of the folder parameter
			pThis->params[ kParamFolder ].max = NT_getNumSampleFolders() - 1;
			NT_updateParameterDefinition( NT_algorithmIndex( self ), kParamFolder );
			// trigger the sample to start streaming
			parameterChanged( self, kParamSample );
		}
	}

	if ( !pThis->dtc->cardMounted )
		return;

	int numFrames = numFramesBy4 * 4;
	if ( NT_globals.workBufferSizeBytes < numFrames * sizeof(_NT_frame) )
		return;
	_NT_frame* renderBuffer = (_NT_frame*)NT_globals.workBuffer;

	float* outL = busFrames + ( pThis->v[kParamOutputL] - 1 ) * numFrames;
	float* outR = busFrames + ( pThis->v[kParamOutputR] - 1 ) * numFrames;
	bool replace = pThis->v[kParamOutputMode];

	int framesRendered = NT_streamRender( pThis->dtc->stream( 0 ), renderBuffer, numFrames, pThis->dtc->inc );

	float gain = 8.0f;

	for ( int i=0; i<framesRendered; ++i )
	{
		float v0 = renderBuffer[i][0];
		float v1 = renderBuffer[i][1];
		v0 *= gain;
		v1 *= gain;
		if ( !replace )
		{
			v0 += outL[i];
			v1 += outR[i];
		}
		outL[i] = v0;
		outR[i] = v1;
	}
	if ( replace )
	{
		for ( int i=framesRendered; i<numFrames; ++i )
		{
			outL[i] = 0.0f;
			outR[i] = 0.0f;
		}
	}
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
	.guid = NT_MULTICHAR( 'E', 'x', 's', 't' ),
	.name = "Demo sample streamer",
	.description = "Streams samples",
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
