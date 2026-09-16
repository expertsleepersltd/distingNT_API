#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PeaksOnly"
#define AIRWINDOWS_DESCRIPTION "A transformative mix check tool."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','e','y' )
#define AIRWINDOWS_KERNELS
enum {

	kNumberOfParameters=0
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
};
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		int ax, bx, cx, dx;
		uint32_t fpd;
	
	struct _dram {
			Float32 a[1503];
		Float32 b[1503];
		Float32 c[1503];
		Float32 d[1503];
	};
	_dram* dram;
};
_kernel kernels[1];

#include "../include/template2.h"
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();

	int am = (int)149.0f * overallscale;
	int bm = (int)179.0f * overallscale;
	int cm = (int)191.0f * overallscale;
	int dm = (int)223.0f * overallscale; //these are 'good' primes, spacing out the allpasses
	int allpasstemp = 0;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
				
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSample = asin(inputSample);
		//amplitude aspect

		allpasstemp = ax - 1; if (allpasstemp < 0 || allpasstemp > am) allpasstemp = am;
		inputSample -= dram->a[allpasstemp]*0.5f;
		dram->a[ax] = inputSample;
		inputSample *= 0.5f;
		ax--; if (ax < 0 || ax > am) {ax = am;}
		inputSample += (dram->a[ax]);
		//a single Midiverb-style allpass
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSample = asin(inputSample);
		//amplitude aspect
		
		allpasstemp = bx - 1; if (allpasstemp < 0 || allpasstemp > bm) allpasstemp = bm;
		inputSample -= dram->b[allpasstemp]*0.5f;
		dram->b[bx] = inputSample;
		inputSample *= 0.5f;
		bx--; if (bx < 0 || bx > bm) {bx = bm;}
		inputSample += (dram->b[bx]);
		//a single Midiverb-style allpass
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSample = asin(inputSample);
		//amplitude aspect
		
		allpasstemp = cx - 1; if (allpasstemp < 0 || allpasstemp > cm) allpasstemp = cm;
		inputSample -= dram->c[allpasstemp]*0.5f;
		dram->c[cx] = inputSample;
		inputSample *= 0.5f;
		cx--; if (cx < 0 || cx > cm) {cx = cm;}
		inputSample += (dram->c[cx]);
		//a single Midiverb-style allpass
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSample = asin(inputSample);
		//amplitude aspect
		
		allpasstemp = dx - 1; if (allpasstemp < 0 || allpasstemp > dm) allpasstemp = dm;
		inputSample -= dram->d[allpasstemp]*0.5f;
		dram->d[dx] = inputSample;
		inputSample *= 0.5f;
		dx--; if (dx < 0 || dx > dm) {dx = dm;}
		inputSample += (dram->d[dx]);
		//a single Midiverb-style allpass
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSample = asin(inputSample);
		//amplitude aspect
		
		inputSample *= 0.63679f; //scale it to 0dB output at full blast
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 1502; count++) {dram->a[count] = 0.0; dram->b[count] = 0.0; dram->c[count] = 0.0; dram->d[count] = 0.0;}
	ax = 1; bx = 1; cx = 1; dx = 1;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
