#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DoublePaul"
#define AIRWINDOWS_DESCRIPTION "Like PaulDither but more so."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','o','l' )
#define AIRWINDOWS_KERNELS
enum {

	//Add your parameters here...
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

		Float32 b[11];	
		uint32_t fpd;
	
	struct _dram {
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
	Float32 inputSample;
	Float32 currentDither;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		
		inputSample *= 8388608.0f;
		//0-1 is now one bit, now we dither
		
		b[9] = b[8]; b[8] = b[7]; b[7] = b[6]; b[6] = b[5];
		b[5] = b[4]; b[4] = b[3]; b[3] = b[2]; b[2] = b[1];
		b[1] = b[0]; b[0] = (float(fpd)/UINT32_MAX);
		
		currentDither  = (b[0] * 0.061f);
		currentDither -= (b[1] * 0.11f);
		currentDither += (b[8] * 0.126f);
		currentDither -= (b[7] * 0.23f);
		currentDither += (b[2] * 0.25f);
		currentDither -= (b[3] * 0.43f);
		currentDither += (b[6] * 0.5f);
		currentDither -= b[5];
		currentDither += b[4];
		//this sounds different from doing it in order of sample position
		//cumulative tiny errors seem to build up even at this buss depth
		//considerably more pronounced at 32 bit float.
		//Therefore we add the most significant components LAST.
		//trying to keep values on like exponents of the floating point value.
		inputSample += currentDither;
		
		inputSample = floor(inputSample);
		
		inputSample /= 8388608.0f;

		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		//pseudorandom number updater
		
		*destP = inputSample;
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 11; count++) {b[count] = 0.0;}
}
};
