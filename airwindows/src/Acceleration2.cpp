#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Acceleration2"
#define AIRWINDOWS_DESCRIPTION "Acceleration, better! Updated for high sample rate and more extreme effect."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','c','2' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
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
{ .name = "Limit", .min = 0, .max = 1000, .def = 320, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 lastSample;
		Float32 m1;
		Float32 m2;
		float biquadA[11];
		float biquadB[11];
		uint32_t fpd;
	
	struct _dram {
			Float32 s[34];
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
	
	Float32 intensity = powf(GetParameter( kParam_One ),3)*32;
	Float32 wet = GetParameter( kParam_Two );	
	int spacing = (int)(1.73f*overallscale)+1;
	if (spacing > 16) spacing = 16;
	
	biquadA[0] = (20000.0f * (1.0f-(GetParameter( kParam_One )*0.618033988749894848204586f))) / GetSampleRate();
	biquadB[0] = 20000.0f / GetSampleRate();
    biquadA[1] = 0.7071f;
	biquadB[1] = 0.7071f;
	
	float K = tan(M_PI * biquadA[0]); //lowpass
	float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
	biquadA[2] = K * K * norm;
	biquadA[3] = 2.0f * biquadA[2];
	biquadA[4] = biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	
	K = tan(M_PI * biquadB[0]);
	norm = 1.0f / (1.0f + K / biquadB[1] + K * K);
	biquadB[2] = K * K * norm;
	biquadB[3] = 2.0f * biquadB[2];
	biquadB[4] = biquadB[2];
	biquadB[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadB[6] = (1.0f - K / biquadB[1] + K * K) * norm;	

	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		float tempSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
		biquadA[8] = biquadA[7]; biquadA[7] = inputSample;
		Float32 smooth = tempSample; 
		biquadA[10] = biquadA[9]; biquadA[9] = smooth; //DF1
		
		for(int count = spacing*2; count >= 0; count--) {dram->s[count+1] = dram->s[count];} dram->s[0] = inputSample;
		m1 = (dram->s[0]-dram->s[spacing])*(fabs(dram->s[0]-dram->s[spacing]));
		m2 = (dram->s[spacing]-dram->s[spacing*2])*(fabs(dram->s[spacing]-dram->s[spacing*2]));
		
		Float32 sense = (intensity*intensity*fabs(m1-m2));
		if (sense > 1.0f) sense = 1.0f;
		inputSample = (inputSample * (1.0f-sense)) + (smooth*sense);
		
		tempSample = biquadB[2]*inputSample+biquadB[3]*biquadB[7]+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
		biquadB[8] = biquadB[7]; biquadB[7] = inputSample; inputSample = tempSample; 
		biquadB[10] = biquadB[9]; biquadB[9] = inputSample; //DF1
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	for(int count = 0; count < 33; count++) {dram->s[count] = 0.0;}
	m1 = m2 = 0.0;
	for (int x = 0; x < 11; x++) {biquadA[x] = 0.0; biquadB[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
