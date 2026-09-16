#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Channel9"
#define AIRWINDOWS_DESCRIPTION "Channel8 with ultrasonic filtering, and new Teac and Mackie 'low end' settings."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','9' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
static const int kNeve = 1;
static const int kAPI = 2;
static const int kSSL = 3;
static const int kTeac = 4;
static const int kMackie = 5;
static const int kDefaultValue_ParamOne = kNeve;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static char const * const enumStrings0[] = { "", "Neve", "API", "SSL", "Teac", "Mackie", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Console Type", .min = 1, .max = 5, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "Drive", .min = 0, .max = 20000, .def = 0, .unit = kNT_unitPercent, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		uint32_t fpd;
		float iirSampleA;
		float iirSampleB;
		bool flip;
		float lastSampleA;
		float lastSampleB;
		float lastSampleC;
		float biquadA[11];
		float biquadB[11];

	
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
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	int console = (int) GetParameter( kParam_One );
	Float32 density = GetParameter( kParam_Two )/100.0f; //0-2
	Float32 phattity = density - 1.0f;
	if (density > 1.0f) density = 1.0f; //max out at full wet for Spiral aspect
	if (phattity < 0.0f) phattity = 0.0f; //
	Float32 nonLin = 5.0f-density; //number is smaller for more intense, larger for more subtle
	Float32 output = GetParameter( kParam_Three );
	Float32 iirAmount = 0.005832f;
	Float32 threshold = 0.33362176f;
	Float32 cutoff = 28811.0f;

	switch (console)
	{
		case 1: iirAmount = 0.005832f; threshold = 0.33362176f; cutoff = 28811.0f; break; //Neve
		case 2: iirAmount = 0.004096f; threshold = 0.59969536f; cutoff = 27216.0f; break; //API
		case 3: iirAmount = 0.004913f; threshold = 0.84934656f; cutoff = 23011.0f; break; //SSL
		case 4: iirAmount = 0.009216f; threshold = 0.149f; cutoff = 18544.0f; break; //Teac
		case 5: iirAmount = 0.011449f; threshold = 0.092f; cutoff = 19748.0f; break; //Mackie
	}
	iirAmount /= overallscale; //we've learned not to try and adjust threshold for sample rate
	
	biquadB[0] = biquadA[0] = cutoff / GetSampleRate();
    biquadA[1] = 1.618033988749894848204586f;
	biquadB[1] = 0.618033988749894848204586f;
	
	float K = tan(M_PI * biquadA[0]); //lowpass
	float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
	biquadA[2] = K * K * norm;
	biquadA[3] = 2.0f * biquadA[2];
	biquadA[4] = biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	
	K = tan(M_PI * biquadA[0]);
	norm = 1.0f / (1.0f + K / biquadB[1] + K * K);
	biquadB[2] = K * K * norm;
	biquadB[3] = 2.0f * biquadB[2];
	biquadB[4] = biquadB[2];
	biquadB[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadB[6] = (1.0f - K / biquadB[1] + K * K) * norm;
	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (biquadA[0] < 0.49999f) {
			float tempSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
			biquadA[8] = biquadA[7]; biquadA[7] = inputSample; if (fabs(tempSample)<1.18e-37f) tempSample = 0.0f; inputSample = tempSample; 
			biquadA[10] = biquadA[9]; biquadA[9] = inputSample; //DF1
		}
				
		Float32 dielectricScale = fabs(2.0f-((inputSample+nonLin)/nonLin));
		
		if (flip)
		{
			if (fabs(iirSampleA)<1.18e-37f) iirSampleA = 0.0f;
			iirSampleA = (iirSampleA * (1.0f - (iirAmount * dielectricScale))) + (inputSample * iirAmount * dielectricScale);
			inputSample = inputSample - iirSampleA;
		}
		else
		{
			if (fabs(iirSampleB)<1.18e-37f) iirSampleB = 0.0f;
			iirSampleB = (iirSampleB * (1.0f - (iirAmount * dielectricScale))) + (inputSample * iirAmount * dielectricScale);
			inputSample = inputSample - iirSampleB;
		}
		//highpass section including capacitor modeling nonlinearity
		float drySample = inputSample;
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		
		float phatSample = sin(inputSample * 1.57079633f);
		inputSample *= 1.2533141373155f;
		//clip to 1.2533141373155f to reach maximum output, or 1.57079633f for pure sine 'phat' version
		float distSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0f) ?1:fabs(inputSample));
		
		inputSample = distSample; //purest form is full Spiral
		if (density < 1.0f) inputSample = (drySample*(1-density))+(distSample*density); //fade Spiral aspect
		if (phattity > 0.0f) inputSample = (inputSample*(1-phattity))+(phatSample*phattity); //apply original Density on top
		
		Float32 clamp = (lastSampleB - lastSampleC) * 0.381966011250105f;
		clamp -= (lastSampleA - lastSampleB) * 0.6180339887498948482045f;
		clamp += inputSample - lastSampleA; //regular slew clamping added
		
		lastSampleC = lastSampleB;
		lastSampleB = lastSampleA;
		lastSampleA = inputSample; //now our output relates off lastSampleB
		
		if (clamp > threshold)
			inputSample = lastSampleB + threshold;
		if (-clamp > threshold)
			inputSample = lastSampleB - threshold;
		//slew section		
		lastSampleA = (lastSampleA*0.381966011250105f)+(inputSample*0.6180339887498948482045f); //split the difference between raw and smoothed for buffer
		
		flip = !flip;
		
		if (output < 1.0f)
		{
			inputSample *= output;
		}
		
		if (biquadB[0] < 0.49999f) {
			float tempSample = biquadB[2]*inputSample+biquadB[3]*biquadB[7]+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
			biquadB[8] = biquadB[7]; biquadB[7] = inputSample; if (fabs(tempSample)<1.18e-37f) tempSample = 0.0f; inputSample = tempSample; 
			biquadB[10] = biquadB[9]; biquadB[9] = inputSample; //DF1
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	iirSampleA = iirSampleB = 0.0;
	flip = false;
	lastSampleA = lastSampleB = lastSampleC = 0.0;
	for (int x = 0; x < 11; x++) {biquadA[x] = 0.0;biquadB[x] = 0.0;}
}
};
