#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ADClip9"
#define AIRWINDOWS_DESCRIPTION "The ClipOnly3 version of a loudenator/biggenator."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','D','9' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kNumberOfParameters=5
};
static const int kBoost = 1;
static const int kMatch = 2;
static const int kClipOnly = 3;
static const int kDefaultValue_ParamE = kBoost;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static char const * const enumStrings4[] = { "", "Boost", "Match", "ClipOnly", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Boost", .min = 0, .max = 18000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Match", .min = 0, .max = 18000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Noise", .min = 0, .max = 1000, .def = 700, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Ceiling", .min = 0, .max = 1000, .def = 750, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mode", .min = 1, .max = 3, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings4 },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float lastSampleL;
		float lastDryL;
		bool wasPosClipL;
		bool wasNegClipL;
		uint32_t fpdL;
	
	struct _dram {
			float intermediateL[18];
		float slewL[34];
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
	
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;

	float inputGain = powf(10.0f,(GetParameter( kParam_A ))/20.0f);
	float match = powf(10.0f,(GetParameter( kParam_B ))/20.0f);
	float noiseLevel = 1.0f-powf(1.0f-GetParameter( kParam_C ),2.0f);
	float ceiling = 1.0f-powf(1.0f-GetParameter( kParam_D ),2.0f);
	int mode = (int) GetParameter( kParam_E );
	
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		
		switch (mode)
		{
			case 1: inputSampleL *= inputGain; break; //Boost
			case 2: inputSampleL *= match; break; //Match
			case 3: inputSampleL *= inputGain; break; //ClipOnly
		}
		//this is our output mode switch, showing the effects
				
		float overshootL = lastDryL;
		lastDryL = inputSampleL;
		
		//begin ClipOnly3 as a little, compressed chunk that can be dropped into code
		float noise = 1.0f-((float(fpdL)/UINT32_MAX)*(1.0f-noiseLevel));//0.076f
		if (wasPosClipL == true) { //current will be over
			if (inputSampleL<lastSampleL) lastSampleL=(noiseLevel*noise)+(inputSampleL*(1.0f-noise));
			else lastSampleL = ceiling;
		} wasPosClipL = false;
		if (inputSampleL>noiseLevel) {wasPosClipL=true;inputSampleL=(noiseLevel*noise)+(lastSampleL*(1.0f-noise));}
		if (wasNegClipL == true) { //current will be -over
			if (inputSampleL > lastSampleL) lastSampleL=(-noiseLevel*noise)+(inputSampleL*(1.0f-noise));
			else lastSampleL = -ceiling;
		} wasNegClipL = false;
		if (inputSampleL<-noiseLevel) {wasNegClipL=true;inputSampleL=(-noiseLevel*noise)+(lastSampleL*(1.0f-noise));}
		dram->slewL[spacing*2] = fabs(lastSampleL-inputSampleL);
		for (int x = spacing*2; x > 0; x--) dram->slewL[x-1] = dram->slewL[x];
		dram->intermediateL[spacing] = inputSampleL; inputSampleL = lastSampleL;
		//Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) {dram->intermediateL[x-1] = dram->intermediateL[x];} lastSampleL = dram->intermediateL[0];
		if (wasPosClipL || wasNegClipL) {
			for (int x = spacing; x > 0; x--) lastSampleL += dram->intermediateL[x];
			lastSampleL /= spacing;
		} float finalSlew = 0.0f;
		for (int x = spacing*2; x >= 0; x--) if (finalSlew < dram->slewL[x]) finalSlew = dram->slewL[x];
		float postclip = ceiling / (1.0f+(finalSlew*1.3986013f));
		if (inputSampleL > postclip) inputSampleL = postclip; if (inputSampleL < -postclip) inputSampleL = -postclip;
		//end ClipOnly3 as a little, compressed chunk that can be dropped into code
		
		switch (mode)
		{
			case 1: break; //Boost
			case 2: inputSampleL /= match; break; //Match
			case 3: inputSampleL = (inputSampleL-overshootL)/inputGain; break; //Clip Only
		}
		//this is our output mode switch, showing the effects
	
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	for (int x = 0; x < 17; x++) dram->intermediateL[x] = 0.0;
	for (int x = 0; x < 33; x++) dram->slewL[x] = 0.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
}
};
