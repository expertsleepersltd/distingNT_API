#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BiquadNonLin"
#define AIRWINDOWS_DESCRIPTION "Capacitor2, but for biquad filtering."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','n' )
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
{ .name = "Type", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Q", .min = 10, .max = 30000, .def = 707, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "NonLin", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		};
		
		uint32_t fpd;
	
	struct _dram {
			float biquad[biq_total];
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
	
	int type = 0;
	if (GetParameter( kParam_One ) > 0.5f) type = 1;
	
	dram->biquad[biq_freq] = ((powf(GetParameter( kParam_Two ),3)*20000.0f)/GetSampleRate());
	if (dram->biquad[biq_freq] < 0.0001f) dram->biquad[biq_freq] = 0.0001f;
	
    dram->biquad[biq_reso] = GetParameter( kParam_Three );
	if (dram->biquad[biq_reso] < 0.0001f) dram->biquad[biq_reso] = 0.0001f;
	
	float nonLin = powf(GetParameter( kParam_Four ),2);
	//if you're using a 0.5f for a lowpass fixed frequency, value is 0.25f
	
	float wet = GetParameter( kParam_Five );
	
	//biquad contains these values:
	//[0] is frequency: 0.000001f to 0.499999f is near-zero to near-Nyquist
	//[1] is resonance, 0.7071f is Butterworth. Also can't be zero
	//[2] is a0 but you need distinct ones for additional biquad instances so it's here
	//[3] is a1 but you need distinct ones for additional biquad instances so it's here
	//[4] is a2 but you need distinct ones for additional biquad instances so it's here
	//[5] is b1 but you need distinct ones for additional biquad instances so it's here
	//[6] is b2 but you need distinct ones for additional biquad instances so it's here
	//[7] is LEFT stored delayed sample (freq and res are stored so you can move them sample by sample)
	//[8] is LEFT stored delayed sample (you have to include the coefficient making code if you do that)
	//[9] is RIGHT stored delayed sample (freq and res are stored so you can move them sample by sample)
	//[10] is RIGHT stored delayed sample (you have to include the coefficient making code if you do that)
	
	//to build a dedicated filter, rename 'biquad' to whatever the new filter is, then
	//put this code either within the sample buffer (for smoothly modulating freq or res)
	//or in this 'read the controls' area (for letting you change freq and res with controls)
	//or in 'reset' if the freq and res are absolutely fixed (use GetSampleRate to define freq)
	
	dram->biquad[biq_aA0] = dram->biquad[biq_aB0];
	dram->biquad[biq_aA1] = dram->biquad[biq_aB1];
	dram->biquad[biq_aA2] = dram->biquad[biq_aB2];
	dram->biquad[biq_bA1] = dram->biquad[biq_bB1];
	dram->biquad[biq_bA2] = dram->biquad[biq_bB2];
	//previous run through the buffer is still in the filter, so we move it
	//to the A section and now it's the new starting point.
	
	if (type == 0) { //lowpass
		float K = tan(M_PI * dram->biquad[biq_freq]);
		float norm = 1.0f / (1.0f + K / dram->biquad[biq_reso] + K * K);
		dram->biquad[biq_aB0] = K * K * norm;
		dram->biquad[biq_aB1] = 2.0f * dram->biquad[biq_aB0];
		dram->biquad[biq_aB2] = dram->biquad[biq_aB0];
		dram->biquad[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
		dram->biquad[biq_bB2] = (1.0f - K / dram->biquad[biq_reso] + K * K) * norm;
	}
		
	if (type == 1) { //bandpass
		float K = tan(M_PI * dram->biquad[biq_freq]);
		float norm = 1.0f / (1.0f + K / dram->biquad[biq_reso] + K * K);
		dram->biquad[biq_aB0] = K / dram->biquad[biq_reso] * norm;
		dram->biquad[biq_aB1] = 0.0f; //bandpass can simplify the biquad kernel: leave out this multiply
		dram->biquad[biq_aB2] = -dram->biquad[biq_aB0];
		dram->biquad[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
		dram->biquad[biq_bB2] = (1.0f - K / dram->biquad[biq_reso] + K * K) * norm;
	}
		
	if (dram->biquad[biq_aA0] == 0.0f) { // if we have just started, start directly with raw info
		dram->biquad[biq_aA0] = dram->biquad[biq_aB0];
		dram->biquad[biq_aA1] = dram->biquad[biq_aB1];
		dram->biquad[biq_aA2] = dram->biquad[biq_aB2];
		dram->biquad[biq_bA1] = dram->biquad[biq_bB1];
		dram->biquad[biq_bA2] = dram->biquad[biq_bB2];
	}
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = *sourceP;
		
		float buf = (float)nSampleFrames/inFramesToProcess;
		dram->biquad[biq_a0] = (dram->biquad[biq_aA0]*buf)+(dram->biquad[biq_aB0]*(1.0f-buf));
		dram->biquad[biq_a1] = (dram->biquad[biq_aA1]*buf)+(dram->biquad[biq_aB1]*(1.0f-buf));
		dram->biquad[biq_a2] = (dram->biquad[biq_aA2]*buf)+(dram->biquad[biq_aB2]*(1.0f-buf));
		dram->biquad[biq_b1] = (dram->biquad[biq_bA1]*buf)+(dram->biquad[biq_bB1]*(1.0f-buf));
		dram->biquad[biq_b2] = (dram->biquad[biq_bA2]*buf)+(dram->biquad[biq_bB2]*(1.0f-buf));
		
		
		float dia0 = fabs(dram->biquad[biq_a0]*(1.0f+(inputSample*nonLin))); if (dia0 > 1.0f) dia0 = 1.0f;
		float dia2 = dia0; //if lowpass, use this in both places
		if (type == 1) dia2 = -dia2;
		//if bandpass, you must reverse polarity
		
		float tempSample = (inputSample * dia0) + dram->biquad[biq_sL1];
		dram->biquad[biq_sL1] = (inputSample * dram->biquad[biq_a1]) - (tempSample * dram->biquad[biq_b1]) + dram->biquad[biq_sL2];
		dram->biquad[biq_sL2] = (inputSample * dia2) - (tempSample * dram->biquad[biq_b2]);
		inputSample = tempSample;
		
		
		inputSample *= wet;
		if (wet > 0.0f) {
			inputSample += (drySample*(1.0f-wet));
		} else {
			inputSample += drySample;
		} //inv/dry/wet lets us turn LP into HP and band into notch
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < biq_total; x++) {dram->biquad[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
