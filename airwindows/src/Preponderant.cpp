#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Preponderant"
#define AIRWINDOWS_DESCRIPTION "Airwindows Anti-Soothe, a strange tone intensifier and balancer."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','r','t' )
#define AIRWINDOWS_TAGS kNT_tagEffect
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Narrow", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Medium", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wide", .min = 0, .max = 1000, .def = 750, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Resonance", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float biquadA[9];
		float biquadB[9];
		float biquadC[9];
		float bassBalance;
		float midBalance;
		float highBalance;
		float bassTrack;
		float midTrack;
		float highTrack;
		float quickness;
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
	
	Float32 chase = 0.00005f / overallscale;
			
	biquadA[0] = powf(GetParameter( kParam_One ),(3.0f*cbrt(overallscale)))*0.42f;
	if (biquadA[0] < 0.0001f) biquadA[0] = 0.0001f;
	
	biquadB[0] = powf(GetParameter( kParam_Two ),(3.0f*cbrt(overallscale)))*0.42f;
	if (biquadB[0] < 0.0001f) biquadB[0] = 0.0001f;
	
	biquadC[0] = powf(GetParameter( kParam_Three ),(3.0f*cbrt(overallscale)))*0.42f;
	if (biquadC[0] < 0.0001f) biquadC[0] = 0.0001f;
	
    biquadA[1] = biquadB[1] = biquadC[1] = (powf(GetParameter( kParam_Four ),3)*8.0f)+0.33f;
	biquadB[1] /= 2.0f; biquadC[1] /= 4.0f;
	
	Float32 volumeCompensation = sqrt(biquadA[1]);

	Float32 wet = GetParameter( kParam_Five );
		
	float K = tan(M_PI * biquadA[0]);
	float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
	biquadA[2] = K / biquadA[1] * norm;
	biquadA[3] = 0.0f; //bandpass can simplify the biquad kernel: leave out this multiply
	biquadA[4] = -biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	
	K = tan(M_PI * biquadB[0]);
	norm = 1.0f / (1.0f + K / biquadB[1] + K * K);
	biquadB[2] = K / biquadB[1] * norm;
	biquadB[3] = 0.0f; //bandpass can simplify the biquad kernel: leave out this multiply
	biquadB[4] = -biquadB[2];
	biquadB[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadB[6] = (1.0f - K / biquadB[1] + K * K) * norm;
	
	K = tan(M_PI * biquadC[0]);
	norm = 1.0f / (1.0f + K / biquadC[1] + K * K);
	biquadC[2] = K / biquadC[1] * norm;
	biquadC[3] = 0.0f; //bandpass can simplify the biquad kernel: leave out this multiply
	biquadC[4] = -biquadC[2];
	biquadC[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadC[6] = (1.0f - K / biquadC[1] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = *sourceP;
				
		float tempSample = (inputSample * biquadA[2]) + biquadA[7];
		biquadA[7] = -(tempSample * biquadA[5]) + biquadA[8];
		biquadA[8] = (inputSample * biquadA[4]) - (tempSample * biquadA[6]);
		float bassSample = tempSample; //BANDPASS form
		
		tempSample = (inputSample * biquadB[2]) + biquadB[7];
		biquadB[7] = -(tempSample * biquadB[5]) + biquadB[8];
		biquadB[8] = (inputSample * biquadB[4]) - (tempSample * biquadB[6]);
		float midSample = tempSample; //BANDPASS form
		
		tempSample = (inputSample * biquadC[2]) + biquadC[7];
		biquadC[7] = -(tempSample * biquadC[5]) + biquadC[8];
		biquadC[8] = (inputSample * biquadC[4]) - (tempSample * biquadC[6]);
		float highSample = tempSample; //BANDPASS form
		
		if (bassBalance < 0.00125f) bassBalance = 0.00125f;
		if (bassBalance > 1.0f) bassBalance = 1.0f;
		
		if (midBalance < 0.00125f) midBalance = 0.00125f;
		if (midBalance > 1.0f) midBalance = 1.0f;
		
		if (highBalance < 0.00125f) highBalance = 0.00125f;
		if (highBalance > 1.0f) highBalance = 1.0f;
				
		bassSample *= bassBalance;
		midSample *= midBalance;
		highSample *= highBalance;
		
		if (bassTrack < fabs(bassSample)) bassTrack = fabs(bassSample);
		if (midTrack < fabs(midSample)) midTrack = fabs(midSample);
		if (highTrack < fabs(highSample)) highTrack = fabs(highSample);
		if (bassTrack > quickness) bassTrack -= quickness;
		if (midTrack > quickness) midTrack -= quickness;
		if (highTrack > quickness) highTrack -= quickness;
		
		
		Float32 disparity = 0.0f;
		if (disparity < fabs(bassTrack - midTrack)) disparity = fabs(bassTrack - midTrack);
		if (disparity < fabs(bassTrack - highTrack)) disparity = fabs(bassTrack - highTrack);
		if (disparity < fabs(midTrack - highTrack)) disparity = fabs(midTrack - highTrack);
		
		quickness *= (1.0f-chase);
		quickness += ((1.0f+disparity) * 0.0001f * chase);
		
		
		if (bassTrack > midTrack) {bassBalance -= quickness; midBalance += quickness;}
		else {bassBalance += quickness; midBalance -= quickness;}
		if (midTrack > highTrack) {midBalance -= quickness; highBalance += quickness;}
		else {midBalance += quickness; highBalance -= quickness;}
		if (highTrack > bassTrack) {highBalance -= quickness; bassBalance += quickness;}
		else {highBalance += quickness; bassBalance -= quickness;}
		
		if (highBalance > 0.618033988749894848204586f) highBalance -= (quickness*0.001f); else highBalance += (quickness*0.001f);
		if (midBalance > 0.618033988749894848204586f) midBalance -= (quickness*0.001f); else midBalance += (quickness*0.001f);
		if (bassBalance > 0.618033988749894848204586f) bassBalance -= (quickness*0.001f); else bassBalance += (quickness*0.001f);
		
		inputSample = (bassSample + midSample + highSample) * volumeCompensation;
		
		if (wet < 1.0f) {
			inputSample = (inputSample*wet) + (drySample*(1.0f-wet));
			//inv/dry/wet lets us turn LP into HP and band into notch
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 9; x++) {biquadA[x] = 0.0;biquadB[x] = 0.0;biquadC[x] = 0.0;}
	bassBalance = midBalance = highBalance = 1.0;
	bassTrack = midTrack = highTrack = 0.0;
	quickness = 0.0001;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
