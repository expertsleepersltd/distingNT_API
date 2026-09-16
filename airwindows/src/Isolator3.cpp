#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Isolator3"
#define AIRWINDOWS_DESCRIPTION "Isolator2, but on one slider, with a band-narrower control."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','s','3' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Iso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Q", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
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
		
		float lastSampleL;
		float intermediateL[16];
		bool wasPosClipL;
		bool wasNegClipL;
		
		uint32_t fpd;
	
	struct _dram {
			float biquadA[biq_total];
		float biquadB[biq_total];
		float biquadC[biq_total];
		float biquadD[biq_total];
		float biquadE[biq_total];
		float biquadF[biq_total];
		float biquadG[biq_total];
		float hiquadA[biq_total];
		float hiquadB[biq_total];
		float hiquadC[biq_total];
		float hiquadD[biq_total];
		float hiquadE[biq_total];
		float hiquadF[biq_total];
		float hiquadG[biq_total];
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

	float f = GetParameter( kParam_A );
	float q = GetParameter( kParam_B );	
	float isoFreq = ((f*2.0f)*(1.0f-q))+(f*q);
	float hisoFreq = (((f*2.0f)-1.0f)*(1.0f-q))+(f*q);
	
	if (isoFreq > 1.0f) isoFreq = 1.0f;
	if (hisoFreq < 0.0f) hisoFreq = 0.0f;
		
	dram->biquadA[biq_freq] = powf(isoFreq,(2.0f*sqrt(overallscale)))*0.4999f;
	if (dram->biquadA[biq_freq] < 0.00025f) dram->biquadA[biq_freq] = 0.00025f;
	dram->biquadG[biq_freq] = dram->biquadF[biq_freq] = dram->biquadE[biq_freq] = dram->biquadD[biq_freq] = dram->biquadC[biq_freq] = dram->biquadB[biq_freq] = dram->biquadA[biq_freq];

	dram->hiquadA[biq_freq] = powf(hisoFreq,(2.0f*sqrt(overallscale)))*0.4999f;
	if (dram->hiquadA[biq_freq] < 0.00025f) dram->hiquadA[biq_freq] = 0.00025f;
	dram->hiquadG[biq_freq] = dram->hiquadF[biq_freq] = dram->hiquadE[biq_freq] = dram->hiquadD[biq_freq] = dram->hiquadC[biq_freq] = dram->hiquadB[biq_freq] = dram->hiquadA[biq_freq];
		
	dram->hiquadA[biq_reso] = dram->biquadA[biq_reso] = 4.46570214f;
	dram->hiquadB[biq_reso] = dram->biquadB[biq_reso] = 1.51387132f;
	dram->hiquadC[biq_reso] = dram->biquadC[biq_reso] = 0.93979296f;
	dram->hiquadD[biq_reso] = dram->biquadD[biq_reso] = 0.70710678f;
	dram->hiquadE[biq_reso] = dram->biquadE[biq_reso] = 0.59051105f;
	dram->hiquadF[biq_reso] = dram->biquadF[biq_reso] = 0.52972649f;
	dram->hiquadG[biq_reso] = dram->biquadG[biq_reso] = 0.50316379f;
	
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
	
	dram->hiquadA[biq_aA0] = dram->hiquadA[biq_aB0];
	dram->hiquadA[biq_aA1] = dram->hiquadA[biq_aB1];
	dram->hiquadA[biq_aA2] = dram->hiquadA[biq_aB2];
	dram->hiquadA[biq_bA1] = dram->hiquadA[biq_bB1];
	dram->hiquadA[biq_bA2] = dram->hiquadA[biq_bB2];
	
	dram->hiquadB[biq_aA0] = dram->hiquadB[biq_aB0];
	dram->hiquadB[biq_aA1] = dram->hiquadB[biq_aB1];
	dram->hiquadB[biq_aA2] = dram->hiquadB[biq_aB2];
	dram->hiquadB[biq_bA1] = dram->hiquadB[biq_bB1];
	dram->hiquadB[biq_bA2] = dram->hiquadB[biq_bB2];
	
	dram->hiquadC[biq_aA0] = dram->hiquadC[biq_aB0];
	dram->hiquadC[biq_aA1] = dram->hiquadC[biq_aB1];
	dram->hiquadC[biq_aA2] = dram->hiquadC[biq_aB2];
	dram->hiquadC[biq_bA1] = dram->hiquadC[biq_bB1];
	dram->hiquadC[biq_bA2] = dram->hiquadC[biq_bB2];
	
	dram->hiquadD[biq_aA0] = dram->hiquadD[biq_aB0];
	dram->hiquadD[biq_aA1] = dram->hiquadD[biq_aB1];
	dram->hiquadD[biq_aA2] = dram->hiquadD[biq_aB2];
	dram->hiquadD[biq_bA1] = dram->hiquadD[biq_bB1];
	dram->hiquadD[biq_bA2] = dram->hiquadD[biq_bB2];
	
	dram->hiquadE[biq_aA0] = dram->hiquadE[biq_aB0];
	dram->hiquadE[biq_aA1] = dram->hiquadE[biq_aB1];
	dram->hiquadE[biq_aA2] = dram->hiquadE[biq_aB2];
	dram->hiquadE[biq_bA1] = dram->hiquadE[biq_bB1];
	dram->hiquadE[biq_bA2] = dram->hiquadE[biq_bB2];
	
	dram->hiquadF[biq_aA0] = dram->hiquadF[biq_aB0];
	dram->hiquadF[biq_aA1] = dram->hiquadF[biq_aB1];
	dram->hiquadF[biq_aA2] = dram->hiquadF[biq_aB2];
	dram->hiquadF[biq_bA1] = dram->hiquadF[biq_bB1];
	dram->hiquadF[biq_bA2] = dram->hiquadF[biq_bB2];
	
	dram->hiquadG[biq_aA0] = dram->hiquadG[biq_aB0];
	dram->hiquadG[biq_aA1] = dram->hiquadG[biq_aB1];
	dram->hiquadG[biq_aA2] = dram->hiquadG[biq_aB2];
	dram->hiquadG[biq_bA1] = dram->hiquadG[biq_bB1];
	dram->hiquadG[biq_bA2] = dram->hiquadG[biq_bB2];
	
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
	
	K = tan(M_PI * dram->hiquadA[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->hiquadA[biq_reso] + K * K);
	dram->hiquadA[biq_aB0] = K * K * norm;
	dram->hiquadA[biq_aB1] = 2.0f * dram->hiquadA[biq_aB0];
	dram->hiquadA[biq_aB2] = dram->hiquadA[biq_aB0];
	dram->hiquadA[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hiquadA[biq_bB2] = (1.0f - K / dram->hiquadA[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->hiquadB[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->hiquadB[biq_reso] + K * K);
	dram->hiquadB[biq_aB0] = K * K * norm;
	dram->hiquadB[biq_aB1] = 2.0f * dram->hiquadB[biq_aB0];
	dram->hiquadB[biq_aB2] = dram->hiquadB[biq_aB0];
	dram->hiquadB[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hiquadB[biq_bB2] = (1.0f - K / dram->hiquadB[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->hiquadC[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->hiquadC[biq_reso] + K * K);
	dram->hiquadC[biq_aB0] = K * K * norm;
	dram->hiquadC[biq_aB1] = 2.0f * dram->hiquadC[biq_aB0];
	dram->hiquadC[biq_aB2] = dram->hiquadC[biq_aB0];
	dram->hiquadC[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hiquadC[biq_bB2] = (1.0f - K / dram->hiquadC[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->hiquadD[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->hiquadD[biq_reso] + K * K);
	dram->hiquadD[biq_aB0] = K * K * norm;
	dram->hiquadD[biq_aB1] = 2.0f * dram->hiquadD[biq_aB0];
	dram->hiquadD[biq_aB2] = dram->hiquadD[biq_aB0];
	dram->hiquadD[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hiquadD[biq_bB2] = (1.0f - K / dram->hiquadD[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->hiquadE[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->hiquadE[biq_reso] + K * K);
	dram->hiquadE[biq_aB0] = K * K * norm;
	dram->hiquadE[biq_aB1] = 2.0f * dram->hiquadE[biq_aB0];
	dram->hiquadE[biq_aB2] = dram->hiquadE[biq_aB0];
	dram->hiquadE[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hiquadE[biq_bB2] = (1.0f - K / dram->hiquadE[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->hiquadF[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->hiquadF[biq_reso] + K * K);
	dram->hiquadF[biq_aB0] = K * K * norm;
	dram->hiquadF[biq_aB1] = 2.0f * dram->hiquadF[biq_aB0];
	dram->hiquadF[biq_aB2] = dram->hiquadF[biq_aB0];
	dram->hiquadF[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hiquadF[biq_bB2] = (1.0f - K / dram->hiquadF[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->hiquadG[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->hiquadG[biq_reso] + K * K);
	dram->hiquadG[biq_aB0] = K * K * norm;
	dram->hiquadG[biq_aB1] = 2.0f * dram->hiquadG[biq_aB0];
	dram->hiquadG[biq_aB2] = dram->hiquadG[biq_aB0];
	dram->hiquadG[biq_bB1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hiquadG[biq_bB2] = (1.0f - K / dram->hiquadG[biq_reso] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		
		dram->hiquadA[biq_a0] = (dram->hiquadA[biq_aA0]*temp)+(dram->hiquadA[biq_aB0]*(1.0f-temp));
		dram->hiquadA[biq_a1] = (dram->hiquadA[biq_aA1]*temp)+(dram->hiquadA[biq_aB1]*(1.0f-temp));
		dram->hiquadA[biq_a2] = (dram->hiquadA[biq_aA2]*temp)+(dram->hiquadA[biq_aB2]*(1.0f-temp));
		dram->hiquadA[biq_b1] = (dram->hiquadA[biq_bA1]*temp)+(dram->hiquadA[biq_bB1]*(1.0f-temp));
		dram->hiquadA[biq_b2] = (dram->hiquadA[biq_bA2]*temp)+(dram->hiquadA[biq_bB2]*(1.0f-temp));
		
		dram->hiquadB[biq_a0] = (dram->hiquadB[biq_aA0]*temp)+(dram->hiquadB[biq_aB0]*(1.0f-temp));
		dram->hiquadB[biq_a1] = (dram->hiquadB[biq_aA1]*temp)+(dram->hiquadB[biq_aB1]*(1.0f-temp));
		dram->hiquadB[biq_a2] = (dram->hiquadB[biq_aA2]*temp)+(dram->hiquadB[biq_aB2]*(1.0f-temp));
		dram->hiquadB[biq_b1] = (dram->hiquadB[biq_bA1]*temp)+(dram->hiquadB[biq_bB1]*(1.0f-temp));
		dram->hiquadB[biq_b2] = (dram->hiquadB[biq_bA2]*temp)+(dram->hiquadB[biq_bB2]*(1.0f-temp));
		
		dram->hiquadC[biq_a0] = (dram->hiquadC[biq_aA0]*temp)+(dram->hiquadC[biq_aB0]*(1.0f-temp));
		dram->hiquadC[biq_a1] = (dram->hiquadC[biq_aA1]*temp)+(dram->hiquadC[biq_aB1]*(1.0f-temp));
		dram->hiquadC[biq_a2] = (dram->hiquadC[biq_aA2]*temp)+(dram->hiquadC[biq_aB2]*(1.0f-temp));
		dram->hiquadC[biq_b1] = (dram->hiquadC[biq_bA1]*temp)+(dram->hiquadC[biq_bB1]*(1.0f-temp));
		dram->hiquadC[biq_b2] = (dram->hiquadC[biq_bA2]*temp)+(dram->hiquadC[biq_bB2]*(1.0f-temp));
		
		dram->hiquadD[biq_a0] = (dram->hiquadD[biq_aA0]*temp)+(dram->hiquadD[biq_aB0]*(1.0f-temp));
		dram->hiquadD[biq_a1] = (dram->hiquadD[biq_aA1]*temp)+(dram->hiquadD[biq_aB1]*(1.0f-temp));
		dram->hiquadD[biq_a2] = (dram->hiquadD[biq_aA2]*temp)+(dram->hiquadD[biq_aB2]*(1.0f-temp));
		dram->hiquadD[biq_b1] = (dram->hiquadD[biq_bA1]*temp)+(dram->hiquadD[biq_bB1]*(1.0f-temp));
		dram->hiquadD[biq_b2] = (dram->hiquadD[biq_bA2]*temp)+(dram->hiquadD[biq_bB2]*(1.0f-temp));
		
		dram->hiquadE[biq_a0] = (dram->hiquadE[biq_aA0]*temp)+(dram->hiquadE[biq_aB0]*(1.0f-temp));
		dram->hiquadE[biq_a1] = (dram->hiquadE[biq_aA1]*temp)+(dram->hiquadE[biq_aB1]*(1.0f-temp));
		dram->hiquadE[biq_a2] = (dram->hiquadE[biq_aA2]*temp)+(dram->hiquadE[biq_aB2]*(1.0f-temp));
		dram->hiquadE[biq_b1] = (dram->hiquadE[biq_bA1]*temp)+(dram->hiquadE[biq_bB1]*(1.0f-temp));
		dram->hiquadE[biq_b2] = (dram->hiquadE[biq_bA2]*temp)+(dram->hiquadE[biq_bB2]*(1.0f-temp));
		
		dram->hiquadF[biq_a0] = (dram->hiquadF[biq_aA0]*temp)+(dram->hiquadF[biq_aB0]*(1.0f-temp));
		dram->hiquadF[biq_a1] = (dram->hiquadF[biq_aA1]*temp)+(dram->hiquadF[biq_aB1]*(1.0f-temp));
		dram->hiquadF[biq_a2] = (dram->hiquadF[biq_aA2]*temp)+(dram->hiquadF[biq_aB2]*(1.0f-temp));
		dram->hiquadF[biq_b1] = (dram->hiquadF[biq_bA1]*temp)+(dram->hiquadF[biq_bB1]*(1.0f-temp));
		dram->hiquadF[biq_b2] = (dram->hiquadF[biq_bA2]*temp)+(dram->hiquadF[biq_bB2]*(1.0f-temp));
		
		dram->hiquadG[biq_a0] = (dram->hiquadG[biq_aA0]*temp)+(dram->hiquadG[biq_aB0]*(1.0f-temp));
		dram->hiquadG[biq_a1] = (dram->hiquadG[biq_aA1]*temp)+(dram->hiquadG[biq_aB1]*(1.0f-temp));
		dram->hiquadG[biq_a2] = (dram->hiquadG[biq_aA2]*temp)+(dram->hiquadG[biq_aB2]*(1.0f-temp));
		dram->hiquadG[biq_b1] = (dram->hiquadG[biq_bA1]*temp)+(dram->hiquadG[biq_bB1]*(1.0f-temp));
		dram->hiquadG[biq_b2] = (dram->hiquadG[biq_bA2]*temp)+(dram->hiquadG[biq_bB2]*(1.0f-temp));
		//this is the interpolation code for the hiquad
		
		float outSample = (inputSample * dram->hiquadA[biq_a0]) + dram->hiquadA[biq_sL1];
		dram->hiquadA[biq_sL1] = (inputSample * dram->hiquadA[biq_a1]) - (outSample * dram->hiquadA[biq_b1]) + dram->hiquadA[biq_sL2];
		dram->hiquadA[biq_sL2] = (inputSample * dram->hiquadA[biq_a2]) - (outSample * dram->hiquadA[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->hiquadB[biq_a0]) + dram->hiquadB[biq_sL1];
		dram->hiquadB[biq_sL1] = (inputSample * dram->hiquadB[biq_a1]) - (outSample * dram->hiquadB[biq_b1]) + dram->hiquadB[biq_sL2];
		dram->hiquadB[biq_sL2] = (inputSample * dram->hiquadB[biq_a2]) - (outSample * dram->hiquadB[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->hiquadC[biq_a0]) + dram->hiquadC[biq_sL1];
		dram->hiquadC[biq_sL1] = (inputSample * dram->hiquadC[biq_a1]) - (outSample * dram->hiquadC[biq_b1]) + dram->hiquadC[biq_sL2];
		dram->hiquadC[biq_sL2] = (inputSample * dram->hiquadC[biq_a2]) - (outSample * dram->hiquadC[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->hiquadD[biq_a0]) + dram->hiquadD[biq_sL1];
		dram->hiquadD[biq_sL1] = (inputSample * dram->hiquadD[biq_a1]) - (outSample * dram->hiquadD[biq_b1]) + dram->hiquadD[biq_sL2];
		dram->hiquadD[biq_sL2] = (inputSample * dram->hiquadD[biq_a2]) - (outSample * dram->hiquadD[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->hiquadE[biq_a0]) + dram->hiquadE[biq_sL1];
		dram->hiquadE[biq_sL1] = (inputSample * dram->hiquadE[biq_a1]) - (outSample * dram->hiquadE[biq_b1]) + dram->hiquadE[biq_sL2];
		dram->hiquadE[biq_sL2] = (inputSample * dram->hiquadE[biq_a2]) - (outSample * dram->hiquadE[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->hiquadF[biq_a0]) + dram->hiquadF[biq_sL1];
		dram->hiquadF[biq_sL1] = (inputSample * dram->hiquadF[biq_a1]) - (outSample * dram->hiquadF[biq_b1]) + dram->hiquadF[biq_sL2];
		dram->hiquadF[biq_sL2] = (inputSample * dram->hiquadF[biq_a2]) - (outSample * dram->hiquadF[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * dram->hiquadG[biq_a0]) + dram->hiquadG[biq_sL1];
		dram->hiquadG[biq_sL1] = (inputSample * dram->hiquadG[biq_a1]) - (outSample * dram->hiquadG[biq_b1]) + dram->hiquadG[biq_sL2];
		dram->hiquadG[biq_sL2] = (inputSample * dram->hiquadG[biq_a2]) - (outSample * dram->hiquadG[biq_b2]);
		inputSample = outSample;
		
		inputSample = (drySample - inputSample);
		
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
		
		outSample = (inputSample * dram->biquadA[biq_a0]) + dram->biquadA[biq_sL1];
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
		
		
		//begin ClipOnly2 as a little, compressed chunk that can be dropped into code
		if (inputSample > 4.0f) inputSample = 4.0f; if (inputSample < -4.0f) inputSample = -4.0f;
		if (wasPosClipL == true) { //current will be over
			if (inputSample<lastSampleL) lastSampleL=0.7058208f+(inputSample*0.2609148f);
			else lastSampleL = 0.2491717f+(lastSampleL*0.7390851f);
		} wasPosClipL = false;
		if (inputSample>0.9549925859f) {wasPosClipL=true;inputSample=0.7058208f+(lastSampleL*0.2609148f);}
		if (wasNegClipL == true) { //current will be -over
			if (inputSample > lastSampleL) lastSampleL=-0.7058208f+(inputSample*0.2609148f);
			else lastSampleL=-0.2491717f+(lastSampleL*0.7390851f);
		} wasNegClipL = false;
		if (inputSample<-0.9549925859f) {wasNegClipL=true;inputSample=-0.7058208f+(lastSampleL*0.2609148f);}
		intermediateL[spacing] = inputSample;
        inputSample = lastSampleL; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediateL[x-1] = intermediateL[x];
		lastSampleL = intermediateL[0]; //run a little buffer to handle this
		//end ClipOnly2 as a little, compressed chunk that can be dropped into code		
		
		
		
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
		dram->hiquadA[x] = 0.0;
		dram->hiquadB[x] = 0.0;
		dram->hiquadC[x] = 0.0;
		dram->hiquadD[x] = 0.0;
		dram->hiquadE[x] = 0.0;
		dram->hiquadF[x] = 0.0;
		dram->hiquadG[x] = 0.0;
	}
	
	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	for (int x = 0; x < 16; x++) {intermediateL[x] = 0.0;}
	//this is reset: values being initialized only once. Startup values, whatever they are.
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
