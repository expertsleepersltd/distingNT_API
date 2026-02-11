#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "TapeHack2"
#define AIRWINDOWS_DESCRIPTION "Brings Airwindows tape to a new level."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','a','x' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Input", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float avg8L[9];
		float avg4L[5];
		float avg2L[3];
		float post8L[9];
		float post4L[5];
		float post2L[3];
		int avgPos;
		float lastDarkL;
		//preTapeHack	
		
		uint32_t fpd;
	
	struct _dram {
			float avg32L[33];
		float avg16L[17];
		float post32L[33];
		float post16L[17];
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
	int spacing = floor(overallscale*2.0f);
	if (spacing < 2) spacing = 2; if (spacing > 32) spacing = 32;

	float inputGain = GetParameter( kParam_A )*10.0f;
	float outputGain = GetParameter( kParam_B )*0.9239f;
	float wet = GetParameter( kParam_C );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		float drySampleL = inputSampleL;

		//begin Left
		inputSampleL *= inputGain;
		float darkSampleL = inputSampleL;
		if (avgPos > 31) avgPos = 0;
		if (spacing > 31) {
			dram->avg32L[avgPos] = darkSampleL;
			darkSampleL = 0.0f;
			for (int x = 0; x < 32; x++) {darkSampleL += dram->avg32L[x];}
			darkSampleL /= 32.0f;
		} if (spacing > 15) {
			dram->avg16L[avgPos%16] = darkSampleL;
			darkSampleL = 0.0f;
			for (int x = 0; x < 16; x++) {darkSampleL += dram->avg16L[x];}
			darkSampleL /= 16.0f;
		} if (spacing > 7) {
			avg8L[avgPos%8] = darkSampleL;
			darkSampleL = 0.0f;
			for (int x = 0; x < 8; x++) {darkSampleL += avg8L[x];}
			darkSampleL /= 8.0f;
		} if (spacing > 3) {
			avg4L[avgPos%4] = darkSampleL;
			darkSampleL = 0.0f;
			for (int x = 0; x < 4; x++) {darkSampleL += avg4L[x];}
			darkSampleL /= 4.0f;
		} if (spacing > 1) {
			avg2L[avgPos%2] = darkSampleL;
			darkSampleL = 0.0f;
			for (int x = 0; x < 2; x++) {darkSampleL += avg2L[x];}
			darkSampleL /= 2.0f; 
		} //only update after the post-distortion filter stage
		
		float avgSlewL = fmin(fabs(lastDarkL-inputSampleL)*0.12f*overallscale,1.0f);
		avgSlewL = 1.0f-(1.0f-avgSlewL*1.0f-avgSlewL);
		inputSampleL = (inputSampleL*(1.0f-avgSlewL)) + (darkSampleL*avgSlewL);
		lastDarkL = darkSampleL;

		inputSampleL = fmax(fmin(inputSampleL,2.305929007734908f),-2.305929007734908f);
		float addtwo = inputSampleL * inputSampleL;
		float empower = inputSampleL * addtwo; // inputSampleL to the third power
		inputSampleL -= (empower / 6.0f);
		empower *= addtwo; // to the fifth power
		inputSampleL += (empower / 69.0f);
		empower *= addtwo; //seventh
		inputSampleL -= (empower / 2530.08f);
		empower *= addtwo; //ninth
		inputSampleL += (empower / 224985.6f);
		empower *= addtwo; //eleventh
		inputSampleL -= (empower / 9979200.0f);
		//this is a degenerate form of a Taylor Series to approximate sin()
		
		darkSampleL = inputSampleL;
		if (avgPos > 31) avgPos = 0;
		if (spacing > 31) {
			dram->post32L[avgPos] = darkSampleL;
			darkSampleL = 0.0f;
			for (int x = 0; x < 32; x++) {darkSampleL += dram->post32L[x];}
			darkSampleL /= 32.0f;
		} if (spacing > 15) {
			dram->post16L[avgPos%16] = darkSampleL;
			darkSampleL = 0.0f;
			for (int x = 0; x < 16; x++) {darkSampleL += dram->post16L[x];}
			darkSampleL /= 16.0f;
		} if (spacing > 7) {
			post8L[avgPos%8] = darkSampleL;
			darkSampleL = 0.0f;
			for (int x = 0; x < 8; x++) {darkSampleL += post8L[x];}
			darkSampleL /= 8.0f;
		} if (spacing > 3) {
			post4L[avgPos%4] = darkSampleL;
			darkSampleL = 0.0f;
			for (int x = 0; x < 4; x++) {darkSampleL += post4L[x];}
			darkSampleL /= 4.0f;
		} if (spacing > 1) {
			post2L[avgPos%2] = darkSampleL;
			darkSampleL = 0.0f;
			for (int x = 0; x < 2; x++) {darkSampleL += post2L[x];}
			darkSampleL /= 2.0f; 
		} avgPos++;
		inputSampleL = (inputSampleL*(1.0f-avgSlewL)) + (darkSampleL*avgSlewL);
		//use the previously calculated depth of the filter
		
		inputSampleL = (inputSampleL * outputGain * wet) + (drySampleL * (1.0f-wet));
		//end Left
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 33; x++) {dram->avg32L[x] = 0.0; dram->post32L[x] = 0.0;}
	for (int x = 0; x < 17; x++) {dram->avg16L[x] = 0.0; dram->post16L[x] = 0.0;}
	for (int x = 0; x < 9; x++) {avg8L[x] = 0.0; post8L[x] = 0.0;}
	for (int x = 0; x < 5; x++) {avg4L[x] = 0.0; post4L[x] = 0.0;}
	for (int x = 0; x < 3; x++) {avg2L[x] = 0.0; post2L[x] = 0.0;}
	avgPos = 0;
	lastDarkL = 0.0;
	//preTapeHack
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
