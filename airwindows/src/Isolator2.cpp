#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Isolator2"
#define AIRWINDOWS_DESCRIPTION "Isolator, but with control smoothing and a new resonance booster."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','s','2' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "Freq", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "High", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		float highA;
		float highB;
		float lowA;
		float lowB;
		uint32_t fpd;
	
	struct _dram {
			float biquadA[biq_total];
		float biquadB[biq_total];
		float biquadC[biq_total];
		float biquadD[biq_total];
		float biquadE[biq_total];
		float biquadF[biq_total];
		float biquadG[biq_total];
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
	
	dram->biquadA[biq_freq] = powf(GetParameter( kParam_One ),(2.0f*sqrt(overallscale)))*0.4999f;
	if (dram->biquadA[biq_freq] < 0.0005f) dram->biquadA[biq_freq] = 0.0005f;
	dram->biquadG[biq_freq] = dram->biquadF[biq_freq] = dram->biquadE[biq_freq] = dram->biquadD[biq_freq] = dram->biquadC[biq_freq] = dram->biquadB[biq_freq] = dram->biquadA[biq_freq];
	
	float reso = powf(GetParameter( kParam_Two ),2);
	float resoBoost = reso+1.0f;
	reso = 1.0f-reso;
	
	dram->biquadA[biq_reso] = 4.46570214f * resoBoost;
	dram->biquadB[biq_reso] = 1.51387132f * resoBoost;
	dram->biquadC[biq_reso] = 0.93979296f * resoBoost;
	dram->biquadD[biq_reso] = 0.70710678f * resoBoost;
	dram->biquadE[biq_reso] = 0.59051105f * resoBoost;
	dram->biquadF[biq_reso] = 0.52972649f * resoBoost;
	dram->biquadG[biq_reso] = 0.50316379f * resoBoost;
	
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
	
	dram->biquadE[biq_aA0] = dram->biquadE[biq_aB0];
	dram->biquadE[biq_aA1] = dram->biquadE[biq_aB1];
	dram->biquadE[biq_aA2] = dram->biquadE[biq_aB2];
	dram->biquadE[biq_bA1] = dram->biquadE[biq_bB1];
	dram->biquadE[biq_bA2] = dram->biquadE[biq_bB2];
	
	dram->biquadF[biq_aA0] = dram->biquadF[biq_aB0];
	dram->biquadF[biq_aA1] = dram->biquadF[biq_aB1];
	dram->biquadF[biq_aA2] = dram->biquadF[biq_aB2];
	dram->biquadF[biq_bA1] = dram->biquadF[biq_bB1];
	dram->biquadF[biq_bA2] = dram->biquadF[biq_bB2];
	
	dram->biquadG[biq_aA0] = dram->biquadG[biq_aB0];
	dram->biquadG[biq_aA1] = dram->biquadG[biq_aB1];
	dram->biquadG[biq_aA2] = dram->biquadG[biq_aB2];
	dram->biquadG[biq_bA1] = dram->biquadG[biq_bB1];
	dram->biquadG[biq_bA2] = dram->biquadG[biq_bB2];
	
	//previous run through the buffer is still in the filter, so we move it
	//to the A section and now it's the new starting point.	
	float K = tan(M_PI * dram->biquadA[biq_freq]);
	float norm = 1.0f / (1.0f + K / dram->biquadA[biq_reso] + K * K);
	dram->biquadA[biq_aB0] = K * K * norm;
	dram->biquadA[biq_aB1] = 2.0f * dram->biquadA[biq_aB0];
	dram->biquadA[biq_aB2] = dram->biquadA[biq_aB0];
	dram->biquadA[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquadA[biq_bB2] = (1.0f - K / dram->biquadA[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->biquadB[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->biquadB[biq_reso] + K * K);
	dram->biquadB[biq_aB0] = K * K * norm;
	dram->biquadB[biq_aB1] = 2.0f * dram->biquadB[biq_aB0];
	dram->biquadB[biq_aB2] = dram->biquadB[biq_aB0];
	dram->biquadB[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquadB[biq_bB2] = (1.0f - K / dram->biquadB[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->biquadC[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->biquadC[biq_reso] + K * K);
	dram->biquadC[biq_aB0] = K * K * norm;
	dram->biquadC[biq_aB1] = 2.0f * dram->biquadC[biq_aB0];
	dram->biquadC[biq_aB2] = dram->biquadC[biq_aB0];
	dram->biquadC[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquadC[biq_bB2] = (1.0f - K / dram->biquadC[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->biquadD[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->biquadD[biq_reso] + K * K);
	dram->biquadD[biq_aB0] = K * K * norm;
	dram->biquadD[biq_aB1] = 2.0f * dram->biquadD[biq_aB0];
	dram->biquadD[biq_aB2] = dram->biquadD[biq_aB0];
	dram->biquadD[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquadD[biq_bB2] = (1.0f - K / dram->biquadD[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->biquadE[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->biquadE[biq_reso] + K * K);
	dram->biquadE[biq_aB0] = K * K * norm;
	dram->biquadE[biq_aB1] = 2.0f * dram->biquadE[biq_aB0];
	dram->biquadE[biq_aB2] = dram->biquadE[biq_aB0];
	dram->biquadE[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquadE[biq_bB2] = (1.0f - K / dram->biquadE[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->biquadF[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->biquadF[biq_reso] + K * K);
	dram->biquadF[biq_aB0] = K * K * norm;
	dram->biquadF[biq_aB1] = 2.0f * dram->biquadF[biq_aB0];
	dram->biquadF[biq_aB2] = dram->biquadF[biq_aB0];
	dram->biquadF[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquadF[biq_bB2] = (1.0f - K / dram->biquadF[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->biquadG[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->biquadG[biq_reso] + K * K);
	dram->biquadG[biq_aB0] = K * K * norm;
	dram->biquadG[biq_aB1] = 2.0f * dram->biquadG[biq_aB0];
	dram->biquadG[biq_aB2] = dram->biquadG[biq_aB0];
	dram->biquadG[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquadG[biq_bB2] = (1.0f - K / dram->biquadG[biq_reso] + K * K) * norm;
	
	bool bypass = (GetParameter( kParam_One ) == 1.0f);
	highA = highB;
	highB = GetParameter( kParam_Three )*reso;
	if (highB > 0.0f) bypass = false;
	lowA = lowB;
	lowB = GetParameter( kParam_Four )*reso;
	if (lowB < 1.0f) bypass = false;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		
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
		
		dram->biquadE[biq_a0] = (dram->biquadE[biq_aA0]*temp)+(dram->biquadE[biq_aB0]*(1.0f-temp));
		dram->biquadE[biq_a1] = (dram->biquadE[biq_aA1]*temp)+(dram->biquadE[biq_aB1]*(1.0f-temp));
		dram->biquadE[biq_a2] = (dram->biquadE[biq_aA2]*temp)+(dram->biquadE[biq_aB2]*(1.0f-temp));
		dram->biquadE[biq_b1] = (dram->biquadE[biq_bA1]*temp)+(dram->biquadE[biq_bB1]*(1.0f-temp));
		dram->biquadE[biq_b2] = (dram->biquadE[biq_bA2]*temp)+(dram->biquadE[biq_bB2]*(1.0f-temp));
		
		dram->biquadF[biq_a0] = (dram->biquadF[biq_aA0]*temp)+(dram->biquadF[biq_aB0]*(1.0f-temp));
		dram->biquadF[biq_a1] = (dram->biquadF[biq_aA1]*temp)+(dram->biquadF[biq_aB1]*(1.0f-temp));
		dram->biquadF[biq_a2] = (dram->biquadF[biq_aA2]*temp)+(dram->biquadF[biq_aB2]*(1.0f-temp));
		dram->biquadF[biq_b1] = (dram->biquadF[biq_bA1]*temp)+(dram->biquadF[biq_bB1]*(1.0f-temp));
		dram->biquadF[biq_b2] = (dram->biquadF[biq_bA2]*temp)+(dram->biquadF[biq_bB2]*(1.0f-temp));
		
		dram->biquadG[biq_a0] = (dram->biquadG[biq_aA0]*temp)+(dram->biquadG[biq_aB0]*(1.0f-temp));
		dram->biquadG[biq_a1] = (dram->biquadG[biq_aA1]*temp)+(dram->biquadG[biq_aB1]*(1.0f-temp));
		dram->biquadG[biq_a2] = (dram->biquadG[biq_aA2]*temp)+(dram->biquadG[biq_aB2]*(1.0f-temp));
		dram->biquadG[biq_b1] = (dram->biquadG[biq_bA1]*temp)+(dram->biquadG[biq_bB1]*(1.0f-temp));
		dram->biquadG[biq_b2] = (dram->biquadG[biq_bA2]*temp)+(dram->biquadG[biq_bB2]*(1.0f-temp));
		
		//this is the interpolation code for the biquad
		float high = (highA*temp)+(highB*(1.0f-temp));
		float low = (lowA*temp)+(lowB*(1.0f-temp));
		
		float outSample = (inputSample * dram->biquadA[biq_a0]) + dram->biquadA[biq_sL1];
		dram->biquadA[biq_sL1] = (inputSample * dram->biquadA[biq_a1]) - (outSample * dram->biquadA[biq_b1]) + dram->biquadA[biq_sL2];
		dram->biquadA[biq_sL2] = (inputSample * dram->biquadA[biq_a2]) - (outSample * dram->biquadA[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->biquadB[biq_a0]) + dram->biquadB[biq_sL1];
		dram->biquadB[biq_sL1] = (inputSample * dram->biquadB[biq_a1]) - (outSample * dram->biquadB[biq_b1]) + dram->biquadB[biq_sL2];
		dram->biquadB[biq_sL2] = (inputSample * dram->biquadB[biq_a2]) - (outSample * dram->biquadB[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->biquadC[biq_a0]) + dram->biquadC[biq_sL1];
		dram->biquadC[biq_sL1] = (inputSample * dram->biquadC[biq_a1]) - (outSample * dram->biquadC[biq_b1]) + dram->biquadC[biq_sL2];
		dram->biquadC[biq_sL2] = (inputSample * dram->biquadC[biq_a2]) - (outSample * dram->biquadC[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->biquadD[biq_a0]) + dram->biquadD[biq_sL1];
		dram->biquadD[biq_sL1] = (inputSample * dram->biquadD[biq_a1]) - (outSample * dram->biquadD[biq_b1]) + dram->biquadD[biq_sL2];
		dram->biquadD[biq_sL2] = (inputSample * dram->biquadD[biq_a2]) - (outSample * dram->biquadD[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->biquadE[biq_a0]) + dram->biquadE[biq_sL1];
		dram->biquadE[biq_sL1] = (inputSample * dram->biquadE[biq_a1]) - (outSample * dram->biquadE[biq_b1]) + dram->biquadE[biq_sL2];
		dram->biquadE[biq_sL2] = (inputSample * dram->biquadE[biq_a2]) - (outSample * dram->biquadE[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->biquadF[biq_a0]) + dram->biquadF[biq_sL1];
		dram->biquadF[biq_sL1] = (inputSample * dram->biquadF[biq_a1]) - (outSample * dram->biquadF[biq_b1]) + dram->biquadF[biq_sL2];
		dram->biquadF[biq_sL2] = (inputSample * dram->biquadF[biq_a2]) - (outSample * dram->biquadF[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->biquadG[biq_a0]) + dram->biquadG[biq_sL1];
		dram->biquadG[biq_sL1] = (inputSample * dram->biquadG[biq_a1]) - (outSample * dram->biquadG[biq_b1]) + dram->biquadG[biq_sL2];
		dram->biquadG[biq_sL2] = (inputSample * dram->biquadG[biq_a2]) - (outSample * dram->biquadG[biq_b2]);
		inputSample = outSample;
		
		if (bypass) inputSample = drySample;
		else inputSample = (inputSample * low) + ((drySample - inputSample)*high);
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < biq_total; x++) {
		dram->biquadA[x] = 0.0;
		dram->biquadB[x] = 0.0;
		dram->biquadC[x] = 0.0;
		dram->biquadD[x] = 0.0;
		dram->biquadE[x] = 0.0;
		dram->biquadF[x] = 0.0;
		dram->biquadG[x] = 0.0;
	}
	highA = 0.0; highB = 0.0;
	lowA = 1.0; lowB = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
