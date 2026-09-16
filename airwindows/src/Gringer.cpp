#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Gringer"
#define AIRWINDOWS_DESCRIPTION "A full-wave rectifier plugin, like a Green Ringer guitar effect."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','r','r' )
#define AIRWINDOWS_TAGS kNT_tagEffect
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
 
		float inband[9];
		float outband[9];
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
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	inband[0] = 0.025f/overallscale;
	outband[0] = 0.025f/overallscale;
	inband[1] = 0.001f;
	outband[1] = 0.001f;	
	//hardwired for wide bandpass around the rectification
	
	float K = tan(M_PI * inband[0]);
	float norm = 1.0f / (1.0f + K / inband[1] + K * K);
	inband[2] = K / inband[1] * norm;
	inband[4] = -inband[2];
	inband[5] = 2.0f * (K * K - 1.0f) * norm;
	inband[6] = (1.0f - K / inband[1] + K * K) * norm;
	
	K = tan(M_PI * outband[0]);
	norm = 1.0f / (1.0f + K / outband[1] + K * K);
	outband[2] = K / outband[1] * norm;
	outband[4] = -outband[2];
	outband[5] = 2.0f * (K * K - 1.0f) * norm;
	outband[6] = (1.0f - K / outband[1] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		inputSample = sin(inputSample);
		//encode Console5: good cleanness
		
		float tempSample = (inputSample * inband[2]) + inband[7];
		inband[7] = -(tempSample * inband[5]) + inband[8];
		inband[8] = (inputSample * inband[4]) - (tempSample * inband[6]);
		
		inputSample = fabs(tempSample);
		//this is all you gotta do to make the Green Ringer fullwave rectification effect
		//the rest is about making it work within a DAW context w. filtering and such
		
		tempSample = (inputSample * outband[2]) + outband[7];
		outband[7] = -(tempSample * outband[5]) + outband[8];
		outband[8] = (inputSample * outband[4]) - (tempSample * outband[6]);
		inputSample = tempSample;
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSample = asin(inputSample);
		//amplitude aspect
				
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 9; x++) {inband[x] = 0.0; outband[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
