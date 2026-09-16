#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ZRegion"
#define AIRWINDOWS_DESCRIPTION "An Emu e6400 style Airwindows Region filter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','Z','R','n' )
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
 
		float biquad[11];
		float biquadA[11];
		float biquadB[11];
		float biquadC[11];
		float biquadD[11];
		float biquadE[11];
		float biquadF[11];
		float iirSampleA;
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
		
	//begin from XRegion
	Float32 high = GetParameter( kParam_Two );
	Float32 low = GetParameter( kParam_Three );
	Float32 mid = (high+low)*0.5f;
	Float32 spread = 1.001f-fabs(high-low);
	
	biquad[0] = (powf(high,3)*20000.0f)/GetSampleRate();
	if (biquad[0] < 0.00009f) biquad[0] = 0.00009f;
	Float32 compensation = sqrt(biquad[0])*6.4f*spread;
	Float32 clipFactor = 0.75f+(biquad[0]*GetParameter( kParam_Four )*37.0f);
	
	biquadA[0] = (powf((high+mid)*0.5f,3)*20000.0f)/GetSampleRate();
	if (biquadA[0] < 0.00009f) biquadA[0] = 0.00009f;
	Float32 compensationA = sqrt(biquadA[0])*6.4f*spread;
	Float32 clipFactorA = 0.75f+(biquadA[0]*GetParameter( kParam_Four )*37.0f);
	
	biquadB[0] = (powf(mid,3)*20000.0f)/GetSampleRate();
	if (biquadB[0] < 0.00009f) biquadB[0] = 0.00009f;
	Float32 compensationB = sqrt(biquadB[0])*6.4f*spread;
	Float32 clipFactorB = 0.75f+(biquadB[0]*GetParameter( kParam_Four )*37.0f);
	
	biquadC[0] = (powf((mid+low)*0.5f,3)*20000.0f)/GetSampleRate();
	if (biquadC[0] < 0.00009f) biquadC[0] = 0.00009f;
	Float32 compensationC = sqrt(biquadC[0])*6.4f*spread;
	Float32 clipFactorC = 0.75f+(biquadC[0]*GetParameter( kParam_Four )*37.0f);
	
	biquadD[0] = (powf(low,3)*20000.0f)/GetSampleRate();
	if (biquadD[0] < 0.00009f) biquadD[0] = 0.00009f;
	Float32 compensationD = sqrt(biquadD[0])*6.4f*spread;
	Float32 clipFactorD = 0.75f+(biquadD[0]*GetParameter( kParam_Four )*37.0f);
	
	float K = tan(M_PI * biquad[0]);
	float norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	biquad[2] = K / 0.7071f * norm;
	biquad[4] = -biquad[2];
	biquad[5] = 2.0f * (K * K - 1.0f) * norm;
	biquad[6] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tan(M_PI * biquadA[0]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	biquadA[2] = K / 0.7071f * norm;
	biquadA[4] = -biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tan(M_PI * biquadB[0]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	biquadB[2] = K / 0.7071f * norm;
	biquadB[4] = -biquadB[2];
	biquadB[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadB[6] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tan(M_PI * biquadC[0]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	biquadC[2] = K / 0.7071f * norm;
	biquadC[4] = -biquadC[2];
	biquadC[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadC[6] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tan(M_PI * biquadD[0]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	biquadD[2] = K / 0.7071f * norm;
	biquadD[4] = -biquadD[2];
	biquadD[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadD[6] = (1.0f - K / 0.7071f + K * K) * norm;	
	
	//opamp stuff
	float inTrim = GetParameter( kParam_One )*10.0f;
	inTrim *= inTrim; inTrim *= inTrim;
	float iirAmountA = 0.00069f/overallscale;
	biquadF[0] = biquadE[0] = 15500.0f / GetSampleRate();
    biquadF[1] = biquadE[1] = 0.935f;
	K = tan(M_PI * biquadE[0]); //lowpass
	norm = 1.0f / (1.0f + K / biquadE[1] + K * K);
	biquadE[2] = K * K * norm;
	biquadE[3] = 2.0f * biquadE[2];
	biquadE[4] = biquadE[2];
	biquadE[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadE[6] = (1.0f - K / biquadE[1] + K * K) * norm;
	for (int x = 0; x < 7; x++) biquadF[x] = biquadE[x];
	//end opamp stuff	
	
	float aWet = 1.0f;
	float bWet = 1.0f;
	float cWet = 1.0f;
	float dWet = GetParameter( kParam_Four )*4.0f;
	//four-stage wet/dry control using progressive stages that bypass when not engaged
	if (dWet < 1.0f) {aWet = dWet; bWet = 0.0f; cWet = 0.0f; dWet = 0.0f;}
	else if (dWet < 2.0f) {bWet = dWet - 1.0f; cWet = 0.0f; dWet = 0.0f;}
	else if (dWet < 3.0f) {cWet = dWet - 2.0f; dWet = 0.0f;}
	else {dWet -= 3.0f;}
	//this is one way to make a little set of dry/wet stages that are successively added to the
	//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	//beyond that point: this is a way to progressively add a 'black box' sound processing
	//which lets you fall through to simpler processing at lower settings.
	float wet = GetParameter( kParam_Five );
	float outSample = 0.0f;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		float overallDrySample = drySample;
		
		if (inTrim != 1.0f) inputSample *= inTrim;
		
		float nukeLevel = inputSample;

		//begin XRegion
		inputSample *= clipFactor;
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		outSample = biquad[2]*inputSample+biquad[4]*biquad[8]-biquad[5]*biquad[9]-biquad[6]*biquad[10];
		biquad[8] = biquad[7]; biquad[7] = inputSample; biquad[10] = biquad[9];
		biquad[9] = outSample; //DF1
		inputSample = outSample / compensation; nukeLevel = inputSample;
		
		if (aWet > 0.0f) {
			inputSample *= clipFactorA;
			if (inputSample > 1.57079633f) inputSample = 1.57079633f;
			if (inputSample < -1.57079633f) inputSample = -1.57079633f;
			inputSample = sin(inputSample);
			outSample = biquadA[2]*inputSample+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
			biquadA[8] = biquadA[7]; biquadA[7] = inputSample; biquadA[10] = biquadA[9];
			biquadA[9] = outSample; //DF1
			inputSample = outSample / compensationA; inputSample = (inputSample * aWet) + (nukeLevel * (1.0f-aWet));
			nukeLevel = inputSample;
		}
		if (bWet > 0.0f) {
			inputSample *= clipFactorB;
			if (inputSample > 1.57079633f) inputSample = 1.57079633f;
			if (inputSample < -1.57079633f) inputSample = -1.57079633f;
			inputSample = sin(inputSample);
			outSample = biquadB[2]*inputSample+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
			biquadB[8] = biquadB[7]; biquadB[7] = inputSample; biquadB[10] = biquadB[9]; 
			biquadB[9] = outSample; //DF1
			inputSample = outSample / compensationB; inputSample = (inputSample * bWet) + (nukeLevel * (1.0f-bWet));
			nukeLevel = inputSample;
		}
		if (cWet > 0.0f) {
			inputSample *= clipFactorC;
			if (inputSample > 1.57079633f) inputSample = 1.57079633f;
			if (inputSample < -1.57079633f) inputSample = -1.57079633f;
			inputSample = sin(inputSample);
			outSample = biquadC[2]*inputSample+biquadC[4]*biquadC[8]-biquadC[5]*biquadC[9]-biquadC[6]*biquadC[10];
			biquadC[8] = biquadC[7]; biquadC[7] = inputSample; biquadC[10] = biquadC[9]; 
			biquadC[9] = outSample; //DF1
			inputSample = outSample / compensationC; inputSample = (inputSample * cWet) + (nukeLevel * (1.0f-cWet));
			nukeLevel = inputSample;
		}
		if (dWet > 0.0f) {
			inputSample *= clipFactorD;
			if (inputSample > 1.57079633f) inputSample = 1.57079633f;
			if (inputSample < -1.57079633f) inputSample = -1.57079633f;
			inputSample = sin(inputSample);
			outSample = biquadD[2]*inputSample+biquadD[4]*biquadD[8]-biquadD[5]*biquadD[9]-biquadD[6]*biquadD[10];
			biquadD[8] = biquadD[7]; biquadD[7] = inputSample; biquadD[10] = biquadD[9]; 
			biquadD[9] = outSample; //DF1
			inputSample = outSample / compensationD; inputSample = (inputSample * dWet) + (nukeLevel * (1.0f-dWet));
		}
		//end XRegion
		
		//opamp stage
		if (fabs(iirSampleA)<1.18e-37f) iirSampleA = 0.0f;
		iirSampleA = (iirSampleA * (1.0f - iirAmountA)) + (inputSample * iirAmountA);
		inputSample -= iirSampleA;
		
		outSample = biquadE[2]*inputSample+biquadE[3]*biquadE[7]+biquadE[4]*biquadE[8]-biquadE[5]*biquadE[9]-biquadE[6]*biquadE[10];
		biquadE[8] = biquadE[7]; biquadE[7] = inputSample; inputSample = outSample; biquadE[10] = biquadE[9]; biquadE[9] = inputSample; //DF1		
		
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		inputSample -= (inputSample*inputSample*inputSample*inputSample*inputSample*0.1768f);
		
		outSample = biquadF[2]*inputSample+biquadF[3]*biquadF[7]+biquadF[4]*biquadF[8]-biquadF[5]*biquadF[9]-biquadF[6]*biquadF[10];
		biquadF[8] = biquadF[7]; biquadF[7] = inputSample; inputSample = outSample; biquadF[10] = biquadF[9]; biquadF[9] = inputSample; //DF1		
		//end opamp stage
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (overallDrySample * (1.0f-wet));
		}	
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 11; x++) {biquad[x] = 0.0; biquadA[x] = 0.0; biquadB[x] = 0.0; biquadC[x] = 0.0; biquadD[x] = 0.0; biquadE[x] = 0.0; biquadF[x] = 0.0;}
	iirSampleA = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
