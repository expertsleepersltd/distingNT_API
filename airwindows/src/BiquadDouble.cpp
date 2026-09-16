#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BiquadDouble"
#define AIRWINDOWS_DESCRIPTION "A handy Airwindows cascaded biquad filter: steeper roll-off before resonance."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','e' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Type", .min = 1000, .max = 4000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Q", .min = 10, .max = 30000, .def = 707, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Dry/Wet", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float biquadA[9];
		float biquadB[9];
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
	
	int type = GetParameter( kParam_One);
	
	biquadA[0] = GetParameter( kParam_Two )*0.499f;
	if (biquadA[0] < 0.0001f) biquadA[0] = 0.0001f;
	
    biquadA[1] = GetParameter( kParam_Three );
	if (biquadA[1] < 0.0001f) biquadA[1] = 0.0001f;
	
	Float32 wet = GetParameter( kParam_Four );
	
	//biquad contains these values:
	//[0] is frequency: 0.000001f to 0.499999f is near-zero to near-Nyquist
	//[1] is resonance, 0.7071f is Butterworth. Also can't be zero
	//[2] is a0 but you need distinct ones for additional biquad instances so it's here
	//[3] is a1 but you need distinct ones for additional biquad instances so it's here
	//[4] is a2 but you need distinct ones for additional biquad instances so it's here
	//[5] is b1 but you need distinct ones for additional biquad instances so it's here
	//[6] is b2 but you need distinct ones for additional biquad instances so it's here
	//[7] is a stored delayed sample (freq and res are stored so you can move them sample by sample)
	//[8] is a stored delayed sample (you have to include the coefficient making code if you do that)
	
	//to build a dedicated filter, rename 'biquad' to whatever the new filter is, then
	//put this code either within the sample buffer (for smoothly modulating freq or res)
	//or in this 'read the controls' area (for letting you change freq and res with controls)
	//or in 'reset' if the freq and res are absolutely fixed (use GetSampleRate to define freq)
	
	if (type == 1) { //lowpass
		float K = tan(M_PI * biquadA[0]);
		float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
		biquadA[2] = K * K * norm;
		biquadA[3] = 2.0f * biquadA[2];
		biquadA[4] = biquadA[2];
		biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
		biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	}
	
	if (type == 2) { //highpass
		float K = tan(M_PI * biquadA[0]);
		float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
		biquadA[2] = norm;
		biquadA[3] = -2.0f * biquadA[2];
		biquadA[4] = biquadA[2];
		biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
		biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	}
	
	if (type == 3) { //bandpass
		float K = tan(M_PI * biquadA[0]);
		float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
		biquadA[2] = K / biquadA[1] * norm;
		biquadA[3] = 0.0f; //bandpass can simplify the biquad kernel: leave out this multiply
		biquadA[4] = -biquadA[2];
		biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
		biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	}
	
	if (type == 4) { //notch
		float K = tan(M_PI * biquadA[0]);
		float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
		biquadA[2] = (1.0f + K * K) * norm;
		biquadA[3] = 2.0f * (K * K - 1) * norm;
		biquadA[4] = biquadA[2];
		biquadA[5] = biquadA[3];
		biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	}
	
	for (int x = 0; x < 7; x++) {biquadB[x] = biquadA[x];}

	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = *sourceP;
		
		
		inputSample = sin(inputSample);
		//encode Console5: good cleanness
		
		float tempSample = (inputSample * biquadA[2]) + biquadA[7];
		biquadA[7] = (inputSample * biquadA[3]) - (tempSample * biquadA[5]) + biquadA[8];
		biquadA[8] = (inputSample * biquadA[4]) - (tempSample * biquadA[6]);
		inputSample = tempSample;
		
		tempSample = (inputSample * biquadB[2]) + biquadB[7];
		biquadB[7] = (inputSample * biquadB[3]) - (tempSample * biquadB[5]) + biquadB[8];
		biquadB[8] = (inputSample * biquadB[4]) - (tempSample * biquadB[6]);
		inputSample = tempSample;
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSample = asin(inputSample);
		//amplitude aspect
		
		if (wet < 1.0f) {
			inputSample = (inputSample*wet) + (drySample*(1.0f-fabs(wet)));
			//inv/dry/wet lets us turn LP into HP and band into notch
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 9; x++) {biquadA[x] = 0.0;biquadB[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
