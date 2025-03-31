#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BiquadNonLin"
#define AIRWINDOWS_DESCRIPTION "Capacitor2, but for biquad filtering."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','u' )
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
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Type", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Q", .min = 10, .max = 30000, .def = 707, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "NonLin", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		
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
		double biquad[biq_total];
		
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	int type = 0;
	if (GetParameter( kParam_One ) > 0.5) type = 1;
	
	biquad[biq_freq] = ((pow(GetParameter( kParam_Two ),3)*20000.0)/GetSampleRate());
	if (biquad[biq_freq] < 0.0001) biquad[biq_freq] = 0.0001;
	
    biquad[biq_reso] = GetParameter( kParam_Three );
	if (biquad[biq_reso] < 0.0001) biquad[biq_reso] = 0.0001;
	
	double nonLin = pow(GetParameter( kParam_Four ),2);
	//if you're using a 0.5 for a lowpass fixed frequency, value is 0.25
	
	double wet = GetParameter( kParam_Five );
	
	//biquad contains these values:
	//[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
	//[1] is resonance, 0.7071 is Butterworth. Also can't be zero
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
	
	biquad[biq_aA0] = biquad[biq_aB0];
	biquad[biq_aA1] = biquad[biq_aB1];
	biquad[biq_aA2] = biquad[biq_aB2];
	biquad[biq_bA1] = biquad[biq_bB1];
	biquad[biq_bA2] = biquad[biq_bB2];
	//previous run through the buffer is still in the filter, so we move it
	//to the A section and now it's the new starting point.
	
	if (type == 0) { //lowpass
		double K = tan(M_PI * biquad[biq_freq]);
		double norm = 1.0 / (1.0 + K / biquad[biq_reso] + K * K);
		biquad[biq_aB0] = K * K * norm;
		biquad[biq_aB1] = 2.0 * biquad[biq_aB0];
		biquad[biq_aB2] = biquad[biq_aB0];
		biquad[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
		biquad[biq_bB2] = (1.0 - K / biquad[biq_reso] + K * K) * norm;
	}
		
	if (type == 1) { //bandpass
		double K = tan(M_PI * biquad[biq_freq]);
		double norm = 1.0 / (1.0 + K / biquad[biq_reso] + K * K);
		biquad[biq_aB0] = K / biquad[biq_reso] * norm;
		biquad[biq_aB1] = 0.0; //bandpass can simplify the biquad kernel: leave out this multiply
		biquad[biq_aB2] = -biquad[biq_aB0];
		biquad[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
		biquad[biq_bB2] = (1.0 - K / biquad[biq_reso] + K * K) * norm;
	}
		
	if (biquad[biq_aA0] == 0.0) { // if we have just started, start directly with raw info
		biquad[biq_aA0] = biquad[biq_aB0];
		biquad[biq_aA1] = biquad[biq_aB1];
		biquad[biq_aA2] = biquad[biq_aB2];
		biquad[biq_bA1] = biquad[biq_bB1];
		biquad[biq_bA2] = biquad[biq_bB2];
	}
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = *sourceP;
		
		double buf = (double)nSampleFrames/inFramesToProcess;
		biquad[biq_a0] = (biquad[biq_aA0]*buf)+(biquad[biq_aB0]*(1.0-buf));
		biquad[biq_a1] = (biquad[biq_aA1]*buf)+(biquad[biq_aB1]*(1.0-buf));
		biquad[biq_a2] = (biquad[biq_aA2]*buf)+(biquad[biq_aB2]*(1.0-buf));
		biquad[biq_b1] = (biquad[biq_bA1]*buf)+(biquad[biq_bB1]*(1.0-buf));
		biquad[biq_b2] = (biquad[biq_bA2]*buf)+(biquad[biq_bB2]*(1.0-buf));
		
		
		double dia0 = fabs(biquad[biq_a0]*(1.0+(inputSample*nonLin))); if (dia0 > 1.0) dia0 = 1.0;
		double dia2 = dia0; //if lowpass, use this in both places
		if (type == 1) dia2 = -dia2;
		//if bandpass, you must reverse polarity
		
		double tempSample = (inputSample * dia0) + biquad[biq_sL1];
		biquad[biq_sL1] = (inputSample * biquad[biq_a1]) - (tempSample * biquad[biq_b1]) + biquad[biq_sL2];
		biquad[biq_sL2] = (inputSample * dia2) - (tempSample * biquad[biq_b2]);
		inputSample = tempSample;
		
		
		inputSample *= wet;
		if (wet > 0.0) {
			inputSample += (drySample*(1.0-wet));
		} else {
			inputSample += drySample;
		} //inv/dry/wet lets us turn LP into HP and band into notch
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < biq_total; x++) {biquad[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
