#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Console7Channel"
#define AIRWINDOWS_DESCRIPTION "Adds anti-alias filtering and special saturation curves tied to the slider position."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','p' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 772, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 gainchase;
		Float32 chasespeed;
		float biquadA[11];
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
	float inputgain = GetParameter( kParam_One )*1.272019649514069f;
	//which is, in fact, the square root of 1.618033988749894848204586f...
	//this happens to give us a boost factor where the track continues to get louder even
	//as it saturates and loses a bit of peak energy. Console7Channel channels go to 12! (.272,etc)
	//Neutral gain through the whole system with a full scale sine ia 0.772f on the gain knob
	if (gainchase != inputgain) chasespeed *= 2.0f;
	if (chasespeed > inFramesToProcess) chasespeed = inFramesToProcess;
	if (gainchase < 0.0f) gainchase = inputgain;
	
	biquadA[0] = 20000.0f / GetSampleRate();
    biquadA[1] = 1.618033988749894848204586f;
	
	float K = tan(M_PI * biquadA[0]); //lowpass
	float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
	biquadA[2] = K * K * norm;
	biquadA[3] = 2.0f * biquadA[2];
	biquadA[4] = biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		float tempSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
		biquadA[8] = biquadA[7]; biquadA[7] = inputSample; inputSample = tempSample; 
		biquadA[10] = biquadA[9]; biquadA[9] = inputSample; //DF1
		
		chasespeed *= 0.9999f; chasespeed -= 0.01f; if (chasespeed < 64.0f) chasespeed = 64.0f;
		//we have our chase speed compensated for recent fader activity
		gainchase = (((gainchase*chasespeed)+inputgain)/(chasespeed+1.0f));
		//gainchase is chasing the target, as a simple multiply gain factor
		if (1.0f != gainchase) inputSample *= powf(gainchase,3);
		//this trim control cuts back extra hard because we will amplify after the distortion
		//that will shift the distortion/antidistortion curve, in order to make faded settings
		//slightly 'expanded' and fall back in the soundstage, subtly
		
		if (inputSample > 1.097f) inputSample = 1.097f;
		if (inputSample < -1.097f) inputSample = -1.097f;
		inputSample = ((sin(inputSample*fabs(inputSample))/((fabs(inputSample) == 0.0f) ?1:fabs(inputSample)))*0.8f)+(sin(inputSample)*0.2f);
		//this is a version of Spiral blended 80/20 with regular Density ConsoleChannel.
		//It's blending between two different harmonics in the overtones of the algorithm
				
		if (1.0f != gainchase && 0.0f != gainchase) inputSample /= gainchase;
		//we re-amplify after the distortion relative to how much we cut back previously.
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	gainchase = -1.0;
	chasespeed = 64.0;
	for (int x = 0; x < 11; x++) {biquadA[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
