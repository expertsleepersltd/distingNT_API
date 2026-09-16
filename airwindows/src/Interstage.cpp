#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Interstage"
#define AIRWINDOWS_DESCRIPTION "A subtle and sophisticated analogifier."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','n','e' )
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
 
		
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 iirSampleC;
		Float32 iirSampleD;
		Float32 iirSampleE;
		Float32 iirSampleF;
		float lastSample;
		uint32_t fpd;
		bool flip;
	
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
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	Float32 firstStage = 0.381966011250105f / overallscale;
	Float32 iirAmount = 0.00295f / overallscale;
	Float32 threshold = 0.381966011250105f;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = *sourceP;
		
		inputSample = (inputSample+lastSample)*0.5f; //start the lowpassing with an average
		
		if (flip) {
			iirSampleA = (iirSampleA * (1 - firstStage)) + (inputSample * firstStage); inputSample = iirSampleA;
			iirSampleC = (iirSampleC * (1 - iirAmount)) + (inputSample * iirAmount); inputSample = iirSampleC;
			iirSampleE = (iirSampleE * (1 - iirAmount)) + (inputSample * iirAmount); inputSample = iirSampleE;
			inputSample = drySample - inputSample;
			//make highpass
			if (inputSample - iirSampleA > threshold) inputSample = iirSampleA + threshold;
			if (inputSample - iirSampleA < -threshold) inputSample = iirSampleA - threshold;
			//slew limit against lowpassed reference point
		} else {
			iirSampleB = (iirSampleB * (1 - firstStage)) + (inputSample * firstStage); inputSample = iirSampleB;
			iirSampleD = (iirSampleD * (1 - iirAmount)) + (inputSample * iirAmount); inputSample = iirSampleD;
			iirSampleF = (iirSampleF * (1 - iirAmount)) + (inputSample * iirAmount); inputSample = iirSampleF;
			inputSample = drySample - inputSample;
			//make highpass
			if (inputSample - iirSampleB > threshold) inputSample = iirSampleB + threshold;
			if (inputSample - iirSampleB < -threshold) inputSample = iirSampleB - threshold;
			//slew limit against lowpassed reference point
		}
		flip = !flip;
		lastSample = inputSample;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	flip = true;
}
};
