
#include <math.h>
#include <new>
#include <distingnt/api.h>
#include <distingnt/dsp.h>

enum { kFFTSize = 10 };
enum { kFFTSamples = 1 << kFFTSize };

struct _chunk
{
    float	input[ kFFTSamples ];
    float	output [kFFTSamples ];
};

struct _audioWorkerWithFFT_DTC
{
	// storage for the _NT_FFT object
	uint8_t		fftObject[ NT_FFT_objectSize( kFFTSize ) ];
	
	// double buffered chunks
	_chunk		chunks[2];
	
	// which chunk are we processing?
	uint32_t	chunk;
	
	// sample counter within the chunk
	uint32_t	counter;

	// FFT work buffer
    float		fft[ kFFTSamples ];
};

struct _audioWorkerWithFFT : public _NT_algorithm
{
	_audioWorkerWithFFT() {}
	~_audioWorkerWithFFT() {}
	
	_audioWorkerWithFFT_DTC*	dtc;
	
	static void	workerCB( _NT_algorithm* alg, uint32_t data );
	void		worker( uint32_t which );
};

enum
{
	kParamInput,
	kParamOutput,
	kParamOutputMode,
	kParamChum,
};

enum { kMaxChum = (kFFTSamples/2)-1 };

static const _NT_parameter	parameters[] = {
	NT_PARAMETER_AUDIO_INPUT( "Input", 1, 1 )
	NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output", 1, 13 )
	{ .name = "Chum", .min = 1, .max = kMaxChum, .def = kMaxChum, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamChum };
static const uint8_t page2[] = { kParamInput, kParamOutput, kParamOutputMode };

static const _NT_parameterPage pages[] = {
	{ .name = "Effect", .numParams = ARRAY_SIZE(page1), .params = page1 },
	{ .name = "Routing", .numParams = ARRAY_SIZE(page2), .params = page2 },
};

static const _NT_parameterPages parameterPages = {
	.numPages = ARRAY_SIZE(pages),
	.pages = pages,
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_audioWorkerWithFFT);
	req.dram = 0;
	req.dtc = sizeof(_audioWorkerWithFFT_DTC);
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	_audioWorkerWithFFT* alg = new (ptrs.sram) _audioWorkerWithFFT();
	alg->parameters = parameters;
	alg->parameterPages = &parameterPages;
	alg->dtc = (_audioWorkerWithFFT_DTC*)ptrs.dtc;
	
	// initialise the FFT object
	NT_constructFFT( alg->dtc->fftObject, kFFTSize );
	
	return alg;
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_audioWorkerWithFFT* pThis = (_audioWorkerWithFFT*)self;
	_audioWorkerWithFFT_DTC* dtc = pThis->dtc;
	int numFrames = numFramesBy4 * 4;
	const float* in = busFrames + ( pThis->v[kParamInput] - 1 ) * numFrames;
	float* out = busFrames + ( pThis->v[kParamOutput] - 1 ) * numFrames;
	bool replace = pThis->v[kParamOutputMode];

	_chunk* chunk = &dtc->chunks[ dtc->chunk ];
	uint32_t c = dtc->counter;

	for ( int frame=0; frame<numFrames; ++frame )
	{
		// fill input sample
		chunk->input[ c ] = in[ frame ];
		
		// take output sample
		float output = chunk->output[ c ];
		if ( !replace )
			output += out[frame];
		out[frame] = output;

		c += 1;
		if ( c == kFFTSamples )
		{
			// chunk full - submit to worker
			c = 0;

			if ( !NT_audioWorkerSubmit( _audioWorkerWithFFT::workerCB, NT_algorithmIndex( self ), dtc->chunk ) )
			{
				// error - queue full
			}

			// now using other chunk
			dtc->chunk ^= 1;
			chunk = &dtc->chunks[ dtc->chunk ];
		}
	}

	dtc->counter = c;
}

void	_audioWorkerWithFFT::workerCB( _NT_algorithm* alg, uint32_t data )
{
	((_audioWorkerWithFFT*)alg)->worker( data );
}

void	_audioWorkerWithFFT::worker( uint32_t which )
{
	_chunk& chunk = dtc->chunks[ which ];

	// perform FFT
	NT_FFT( dtc->fftObject, dtc->fft, chunk.input );

	// FFT processing goes here

	int chum = v[kParamChum];
	for ( int i=chum; i<kFFTSamples/2; ++i )
	{
		dtc->fft[ i ] = 0.0f;
		dtc->fft[ (kFFTSamples/2) + i ] = 0.0f;
	}
	
	// perform inverse FFT
	NT_IFFT( dtc->fftObject, dtc->fft, chunk.output );
	
	// rescale
	float s = 1.0f / kFFTSamples;
	for ( int i=0; i<kFFTSamples; ++i )
		chunk.output[i] *= s;
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 'a', 'w' ),
	.name = "Audio worker/FFT demo",
	.description = "Uses background FFT processing",
	.numSpecifications = 0,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.step = step,
	.tags = kNT_tagEffect,
	.audioWorkerStackSize = 1024,
};

_NT_DRAM_SECTION
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
