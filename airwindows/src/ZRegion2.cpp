#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ZRegion2"
#define AIRWINDOWS_DESCRIPTION "An Emu e6400 style Airwindows Region filter, with coefficient smoothing."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','Z','R','2' )
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
{ .name = "Input", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "First", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Last", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Poles", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		float iirSampleA;
		enum {
			biq_freq,
			biq_reso,
			biq_a0,
			biq_a1,
			biq_a2,
			biq_b1,
			biq_b2,
			biq_aA0,
			biq_aA1,
			biq_aA2,
			biq_bA1,
			biq_bA2,
			biq_aB0,
			biq_aB1,
			biq_aB2,
			biq_bB1,
			biq_bB2,
			biq_sL1,
			biq_sL2,
			biq_sR1,
			biq_sR2,
			biq_total
		}; //coefficient interpolating biquad filter, stereo
		float inTrimA;
		float inTrimB;
		float wetA;
		float wetB;
		float overallWetA;
		float overallWetB;
		
		enum {
			fix_freq,
			fix_reso,
			fix_a0,
			fix_a1,
			fix_a2,
			fix_b1,
			fix_b2,
			fix_sL1,
			fix_sL2,
			fix_sR1,
			fix_sR2,
			fix_total
		}; //fixed frequency biquad filter for ultrasonics, stereo
		
		uint32_t fpd;
	
	struct _dram {
			float biquad[biq_total];
		float biquadA[biq_total];
		float biquadB[biq_total];
		float biquadC[biq_total];
		float biquadD[biq_total];
		float fixA[fix_total];
		float fixB[fix_total];
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
	
	//begin from XRegion
	Float32 high = GetParameter( kParam_Two );
	Float32 low = GetParameter( kParam_Three );
	Float32 mid = (high+low)*0.5f;
	Float32 spread = 1.001f-fabs(high-low);
	
	dram->biquad[biq_freq] = (powf(high,3)*20000.0f)/GetSampleRate();
	if (dram->biquad[biq_freq] < 0.00009f) dram->biquad[biq_freq] = 0.00009f;
	Float32 compensation = sqrt(dram->biquad[biq_freq])*6.4f*spread;
	Float32 clipFactor = 0.75f+(dram->biquad[biq_freq]*GetParameter( kParam_Four )*37.0f);
	
	dram->biquadA[biq_freq] = (powf((high+mid)*0.5f,3)*20000.0f)/GetSampleRate();
	if (dram->biquadA[biq_freq] < 0.00009f) dram->biquadA[biq_freq] = 0.00009f;
	Float32 compensationA = sqrt(dram->biquadA[biq_freq])*6.4f*spread;
	Float32 clipFactorA = 0.75f+(dram->biquadA[biq_freq]*GetParameter( kParam_Four )*37.0f);
	
	dram->biquadB[biq_freq] = (powf(mid,3)*20000.0f)/GetSampleRate();
	if (dram->biquadB[biq_freq] < 0.00009f) dram->biquadB[biq_freq] = 0.00009f;
	Float32 compensationB = sqrt(dram->biquadB[biq_freq])*6.4f*spread;
	Float32 clipFactorB = 0.75f+(dram->biquadB[biq_freq]*GetParameter( kParam_Four )*37.0f);
	
	dram->biquadC[biq_freq] = (powf((mid+low)*0.5f,3)*20000.0f)/GetSampleRate();
	if (dram->biquadC[biq_freq] < 0.00009f) dram->biquadC[biq_freq] = 0.00009f;
	Float32 compensationC = sqrt(dram->biquadC[biq_freq])*6.4f*spread;
	Float32 clipFactorC = 0.75f+(dram->biquadC[biq_freq]*GetParameter( kParam_Four )*37.0f);
	
	dram->biquadD[biq_freq] = (powf(low,3)*20000.0f)/GetSampleRate();
	if (dram->biquadD[biq_freq] < 0.00009f) dram->biquadD[biq_freq] = 0.00009f;
	Float32 compensationD = sqrt(dram->biquadD[biq_freq])*6.4f*spread;
	Float32 clipFactorD = 0.75f+(dram->biquadD[biq_freq]*GetParameter( kParam_Four )*37.0f);
	
	//set up all the interpolations
	dram->biquad[biq_aA0] = dram->biquad[biq_aB0];
	dram->biquad[biq_aA1] = dram->biquad[biq_aB1];
	dram->biquad[biq_aA2] = dram->biquad[biq_aB2];
	dram->biquad[biq_bA1] = dram->biquad[biq_bB1];
	dram->biquad[biq_bA2] = dram->biquad[biq_bB2];

	dram->biquadA[biq_aA0] = dram->biquadA[biq_aB0];
	dram->biquadA[biq_aA1] = dram->biquadA[biq_aB1];
	dram->biquadA[biq_aA2] = dram->biquadA[biq_aB2];
	dram->biquadA[biq_bA1] = dram->biquadA[biq_bB1];
	dram->biquadA[biq_bA2] = dram->biquadA[biq_bB2];

	dram->biquadB[biq_aA0] = dram->biquadB[biq_aB0];
	dram->biquadB[biq_aA1] = dram->biquadB[biq_aB1];
	dram->biquadB[biq_aA2] = dram->biquadB[biq_aB2];
	dram->biquadB[biq_bA1] = dram->biquadB[biq_bB1];
	dram->biquadB[biq_bA2] = dram->biquadB[biq_bB2];

	dram->biquadC[biq_aA0] = dram->biquadC[biq_aB0];
	dram->biquadC[biq_aA1] = dram->biquadC[biq_aB1];
	dram->biquadC[biq_aA2] = dram->biquadC[biq_aB2];
	dram->biquadC[biq_bA1] = dram->biquadC[biq_bB1];
	dram->biquadC[biq_bA2] = dram->biquadC[biq_bB2];

	dram->biquadD[biq_aA0] = dram->biquadD[biq_aB0];
	dram->biquadD[biq_aA1] = dram->biquadD[biq_aB1];
	dram->biquadD[biq_aA2] = dram->biquadD[biq_aB2];
	dram->biquadD[biq_bA1] = dram->biquadD[biq_bB1];
	dram->biquadD[biq_bA2] = dram->biquadD[biq_bB2];
	//since this is Region, they are all different
	
	float K = tan(M_PI * dram->biquad[biq_freq]);
	float norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	dram->biquad[biq_aB0] = K / 0.7071f * norm;
	dram->biquad[biq_aB2] = -dram->biquad[biq_aB0];
	dram->biquad[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquad[biq_bB2] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tan(M_PI * dram->biquadA[biq_freq]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	dram->biquadA[biq_aB0] = K / 0.7071f * norm;
	dram->biquadA[biq_aB2] = -dram->biquadA[biq_aB0];
	dram->biquadA[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquadA[biq_bB2] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tan(M_PI * dram->biquadB[biq_freq]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	dram->biquadB[biq_aB0] = K / 0.7071f * norm;
	dram->biquadB[biq_aB2] = -dram->biquadB[biq_aB0];
	dram->biquadB[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquadB[biq_bB2] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tan(M_PI * dram->biquadC[biq_freq]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	dram->biquadC[biq_aB0] = K / 0.7071f * norm;
	dram->biquadC[biq_aB2] = -dram->biquadC[biq_aB0];
	dram->biquadC[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquadC[biq_bB2] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tan(M_PI * dram->biquadD[biq_freq]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	dram->biquadD[biq_aB0] = K / 0.7071f * norm;
	dram->biquadD[biq_aB2] = -dram->biquadD[biq_aB0];
	dram->biquadD[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquadD[biq_bB2] = (1.0f - K / 0.7071f + K * K) * norm;	
	//end from XRegion	
		
	//opamp stuff
	inTrimA = inTrimB;
	inTrimB = GetParameter( kParam_One )*10.0f;
	inTrimB *= inTrimB; inTrimB *= inTrimB;
	wetA = wetB;
	wetB = GetParameter( kParam_Four );
	overallWetA = overallWetB;
	overallWetB = GetParameter( kParam_Five );

	float iirAmountA = 0.00069f/overallscale;
	dram->fixA[fix_freq] = dram->fixB[fix_freq] = 15500.0f / GetSampleRate();
    dram->fixA[fix_reso] = dram->fixB[fix_reso] = 0.935f;
	K = tan(M_PI * dram->fixB[fix_freq]); //lowpass
	norm = 1.0f / (1.0f + K / dram->fixB[fix_reso] + K * K);
	dram->fixA[fix_a0] = dram->fixB[fix_a0] = K * K * norm;
	dram->fixA[fix_a1] = dram->fixB[fix_a1] = 2.0f * dram->fixB[fix_a0];
	dram->fixA[fix_a2] = dram->fixB[fix_a2] = dram->fixB[fix_a0];
	dram->fixA[fix_b1] = dram->fixB[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fixA[fix_b2] = dram->fixB[fix_b2] = (1.0f - K / dram->fixB[fix_reso] + K * K) * norm;
	//end opamp stuff	
	
	float outSample = 0.0f;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float overallDrySample = *sourceP;
		float nukeLevel = inputSample;

		float temp = (float)nSampleFrames/inFramesToProcess;
		dram->biquad[biq_a0] = (dram->biquad[biq_aA0]*temp)+(dram->biquad[biq_aB0]*(1.0f-temp));
		dram->biquad[biq_a1] = (dram->biquad[biq_aA1]*temp)+(dram->biquad[biq_aB1]*(1.0f-temp));
		dram->biquad[biq_a2] = (dram->biquad[biq_aA2]*temp)+(dram->biquad[biq_aB2]*(1.0f-temp));
		dram->biquad[biq_b1] = (dram->biquad[biq_bA1]*temp)+(dram->biquad[biq_bB1]*(1.0f-temp));
		dram->biquad[biq_b2] = (dram->biquad[biq_bA2]*temp)+(dram->biquad[biq_bB2]*(1.0f-temp));
		
		dram->biquadA[biq_a0] = (dram->biquadA[biq_aA0]*temp)+(dram->biquadA[biq_aB0]*(1.0f-temp));
		dram->biquadA[biq_a1] = (dram->biquadA[biq_aA1]*temp)+(dram->biquadA[biq_aB1]*(1.0f-temp));
		dram->biquadA[biq_a2] = (dram->biquadA[biq_aA2]*temp)+(dram->biquadA[biq_aB2]*(1.0f-temp));
		dram->biquadA[biq_b1] = (dram->biquadA[biq_bA1]*temp)+(dram->biquadA[biq_bB1]*(1.0f-temp));
		dram->biquadA[biq_b2] = (dram->biquadA[biq_bA2]*temp)+(dram->biquadA[biq_bB2]*(1.0f-temp));
		
		dram->biquadB[biq_a0] = (dram->biquadB[biq_aA0]*temp)+(dram->biquadB[biq_aB0]*(1.0f-temp));
		dram->biquadB[biq_a1] = (dram->biquadB[biq_aA1]*temp)+(dram->biquadB[biq_aB1]*(1.0f-temp));
		dram->biquadB[biq_a2] = (dram->biquadB[biq_aA2]*temp)+(dram->biquadB[biq_aB2]*(1.0f-temp));
		dram->biquadB[biq_b1] = (dram->biquadB[biq_bA1]*temp)+(dram->biquadB[biq_bB1]*(1.0f-temp));
		dram->biquadB[biq_b2] = (dram->biquadB[biq_bA2]*temp)+(dram->biquadB[biq_bB2]*(1.0f-temp));
		
		dram->biquadC[biq_a0] = (dram->biquadC[biq_aA0]*temp)+(dram->biquadC[biq_aB0]*(1.0f-temp));
		dram->biquadC[biq_a1] = (dram->biquadC[biq_aA1]*temp)+(dram->biquadC[biq_aB1]*(1.0f-temp));
		dram->biquadC[biq_a2] = (dram->biquadC[biq_aA2]*temp)+(dram->biquadC[biq_aB2]*(1.0f-temp));
		dram->biquadC[biq_b1] = (dram->biquadC[biq_bA1]*temp)+(dram->biquadC[biq_bB1]*(1.0f-temp));
		dram->biquadC[biq_b2] = (dram->biquadC[biq_bA2]*temp)+(dram->biquadC[biq_bB2]*(1.0f-temp));
		
		dram->biquadD[biq_a0] = (dram->biquadD[biq_aA0]*temp)+(dram->biquadD[biq_aB0]*(1.0f-temp));
		dram->biquadD[biq_a1] = (dram->biquadD[biq_aA1]*temp)+(dram->biquadD[biq_aB1]*(1.0f-temp));
		dram->biquadD[biq_a2] = (dram->biquadD[biq_aA2]*temp)+(dram->biquadD[biq_aB2]*(1.0f-temp));
		dram->biquadD[biq_b1] = (dram->biquadD[biq_bA1]*temp)+(dram->biquadD[biq_bB1]*(1.0f-temp));
		dram->biquadD[biq_b2] = (dram->biquadD[biq_bA2]*temp)+(dram->biquadD[biq_bB2]*(1.0f-temp));
		//this is the interpolation code for all the biquads
		
		float inTrim = (inTrimA*temp)+(inTrimB*(1.0f-temp));
		float wet = (wetA*temp)+(wetB*(1.0f-temp));
		float aWet = 1.0f;
		float bWet = 1.0f;
		float cWet = 1.0f;
		float dWet = wet*4.0f;
		//four-stage wet/dry control using progressive stages that bypass when not engaged
		if (dWet < 1.0f) {aWet = dWet; bWet = 0.0f; cWet = 0.0f; dWet = 0.0f;}
		else if (dWet < 2.0f) {bWet = dWet - 1.0f; cWet = 0.0f; dWet = 0.0f;}
		else if (dWet < 3.0f) {cWet = dWet - 2.0f; dWet = 0.0f;}
		else {dWet -= 3.0f;}
		//this is one way to make a little set of dry/wet stages that are successively added to the
		//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
		//beyond that point: this is a way to progressively add a 'black box' sound processing
		//which lets you fall through to simpler processing at lower settings.
		float overallWet = (overallWetA*temp)+(overallWetB*(1.0f-temp));
		
		if (inTrim != 1.0f) inputSample *= inTrim;
		
		inputSample *= clipFactor;
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);		
		outSample = (inputSample * dram->biquad[biq_a0]) + dram->biquad[biq_sL1];
		dram->biquad[biq_sL1] = (inputSample * dram->biquad[biq_a1]) - (outSample * dram->biquad[biq_b1]) + dram->biquad[biq_sL2];
		dram->biquad[biq_sL2] = (inputSample * dram->biquad[biq_a2]) - (outSample * dram->biquad[biq_b2]);
		inputSample = outSample / compensation; nukeLevel = inputSample;
		
		if (aWet > 0.0f) {
			inputSample *= clipFactorA;
			if (inputSample > 1.57079633f) inputSample = 1.57079633f;
			if (inputSample < -1.57079633f) inputSample = -1.57079633f;
			inputSample = sin(inputSample);
			outSample = (inputSample * dram->biquadA[biq_a0]) + dram->biquadA[biq_sL1];
			dram->biquadA[biq_sL1] = (inputSample * dram->biquadA[biq_a1]) - (outSample * dram->biquadA[biq_b1]) + dram->biquadA[biq_sL2];
			dram->biquadA[biq_sL2] = (inputSample * dram->biquadA[biq_a2]) - (outSample * dram->biquadA[biq_b2]);
			inputSample = outSample / compensationA; inputSample = (inputSample * aWet) + (nukeLevel * (1.0f-aWet));
			nukeLevel = inputSample;
		}
		if (bWet > 0.0f) {
			inputSample *= clipFactorB;
			if (inputSample > 1.57079633f) inputSample = 1.57079633f;
			if (inputSample < -1.57079633f) inputSample = -1.57079633f;
			inputSample = sin(inputSample);
			outSample = (inputSample * dram->biquadB[biq_a0]) + dram->biquadB[biq_sL1];
			dram->biquadB[biq_sL1] = (inputSample * dram->biquadB[biq_a1]) - (outSample * dram->biquadB[biq_b1]) + dram->biquadB[biq_sL2];
			dram->biquadB[biq_sL2] = (inputSample * dram->biquadB[biq_a2]) - (outSample * dram->biquadB[biq_b2]);			
			inputSample = outSample / compensationB; inputSample = (inputSample * bWet) + (nukeLevel * (1.0f-bWet));
			nukeLevel = inputSample;
		}
		if (cWet > 0.0f) {
			inputSample *= clipFactorC;
			if (inputSample > 1.57079633f) inputSample = 1.57079633f;
			if (inputSample < -1.57079633f) inputSample = -1.57079633f;
			inputSample = sin(inputSample);
			outSample = (inputSample * dram->biquadC[biq_a0]) + dram->biquadC[biq_sL1];
			dram->biquadC[biq_sL1] = (inputSample * dram->biquadC[biq_a1]) - (outSample * dram->biquadC[biq_b1]) + dram->biquadC[biq_sL2];
			dram->biquadC[biq_sL2] = (inputSample * dram->biquadC[biq_a2]) - (outSample * dram->biquadC[biq_b2]);
			inputSample = outSample / compensationC; inputSample = (inputSample * cWet) + (nukeLevel * (1.0f-cWet));
			nukeLevel = inputSample;
		}
		if (dWet > 0.0f) {
			inputSample *= clipFactorD;
			if (inputSample > 1.57079633f) inputSample = 1.57079633f;
			if (inputSample < -1.57079633f) inputSample = -1.57079633f;
			inputSample = sin(inputSample);
			outSample = (inputSample * dram->biquadD[biq_a0]) + dram->biquadD[biq_sL1];
			dram->biquadD[biq_sL1] = (inputSample * dram->biquadD[biq_a1]) - (outSample * dram->biquadD[biq_b1]) + dram->biquadD[biq_sL2];
			dram->biquadD[biq_sL2] = (inputSample * dram->biquadD[biq_a2]) - (outSample * dram->biquadD[biq_b2]);
			inputSample = outSample / compensationD; inputSample = (inputSample * dWet) + (nukeLevel * (1.0f-dWet));
		}
				
		//opamp stage
		if (fabs(iirSampleA)<1.18e-37f) iirSampleA = 0.0f;
		iirSampleA = (iirSampleA * (1.0f - iirAmountA)) + (inputSample * iirAmountA);
		inputSample -= iirSampleA;
		
		outSample = (inputSample * dram->fixA[fix_a0]) + dram->fixA[fix_sL1];
		dram->fixA[fix_sL1] = (inputSample * dram->fixA[fix_a1]) - (outSample * dram->fixA[fix_b1]) + dram->fixA[fix_sL2];
		dram->fixA[fix_sL2] = (inputSample * dram->fixA[fix_a2]) - (outSample * dram->fixA[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		inputSample -= (inputSample*inputSample*inputSample*inputSample*inputSample*0.1768f);
		
		outSample = (inputSample * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
		dram->fixB[fix_sL1] = (inputSample * dram->fixB[fix_a1]) - (outSample * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
		dram->fixB[fix_sL2] = (inputSample * dram->fixB[fix_a2]) - (outSample * dram->fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		//end opamp stage
		
		if (overallWet !=1.0f) {
			inputSample = (inputSample * overallWet) + (overallDrySample * (1.0f-overallWet));
		}	
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirSampleA = 0.0;
	for (int x = 0; x < biq_total; x++) {dram->biquad[x] = 0.0; dram->biquadA[x] = 0.0; dram->biquadB[x] = 0.0; dram->biquadC[x] = 0.0; dram->biquadD[x] = 0.0;}
	inTrimA = 0.1; inTrimB = 0.1;
	//outTrimA = 1.0; outTrimB = 1.0;
	wetA = 0.5; wetB = 0.5;
	overallWetA = 1.0; overallWetB = 1.0;
	for (int x = 0; x < fix_total; x++) {dram->fixA[x] = 0.0; dram->fixB[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
