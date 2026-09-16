#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Exciter"
#define AIRWINDOWS_DESCRIPTION "An Aural Exciter plugin that can be both subtle and extreme."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','E','x','r' )
#define AIRWINDOWS_TAGS kNT_tagEffect
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
{ .name = "Freq", .min = 0, .max = 1000, .def = 600, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Excite", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float biquad[11];
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

	biquad[0] = ((GetParameter( kParam_One )*7000.0f)+8000.0f)/GetSampleRate();
	biquad[1] = GetParameter( kParam_One )+GetParameter( kParam_Two )+0.7071f;
	//tighter resonance as frequency and boost increases
	float boost = powf(GetParameter( kParam_Two ),2.0f)*16.0f;
	float K = tan(M_PI * biquad[0]);
	float norm = 1.0f / (1.0f + K / biquad[1] + K * K);
	biquad[2] = K / 0.7071f * norm;
	biquad[4] = -biquad[2];
	biquad[5] = 2.0f * (K * K - 1.0f) * norm;
	biquad[6] = (1.0f - K / biquad[1] + K * K) * norm;
	//bandpass to focus the intensity of the effect
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		float outSample = biquad[2]*inputSample+biquad[4]*biquad[8]-biquad[5]*biquad[9]-biquad[6]*biquad[10];
		biquad[8] = biquad[7]; biquad[7] = inputSample; biquad[10] = biquad[9];
		biquad[9] = outSample; //DF1
		//so the audio we're working with is going to be a bandpassed signal: only high mids
		
		outSample *= boost;
		if (outSample > M_PI) outSample = M_PI;
		if (outSample < -M_PI) outSample = -M_PI;
		outSample -= sin(outSample);
		//so we're clipping to generate artifacts, but subtracting them, meaning that
		//our deviations from the unclipped waveform are now negative in form.
		
		outSample *= boost;
		if (outSample > M_PI) outSample = M_PI;
		if (outSample < -M_PI) outSample = -M_PI;
		outSample = sin(outSample);
		//now we're clipping the result, to make the peaks less intense
		
		inputSample -= outSample;
		//and we apply only those deviations from distorted high mids, by subtracting them from
		//the original full bandwidth audio. And that's how analog aural exciters basically work.
		//The real ones used a 4049 chip sometimes to produce the soft saturation we're getting with sin()	
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		//and we'll do the harshest of hardclips to cope with how intense the new peaks can get,
		//without in any way dialing back the ruthless brightness the exciter can create.
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 11; x++) {biquad[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
