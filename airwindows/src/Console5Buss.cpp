#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Console5Buss"
#define AIRWINDOWS_DESCRIPTION "A richer, warmer Console system."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','w' )
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
{ .name = "Input Pad", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 lastFXBuss;
		Float32 lastSampleBuss;
		Float32 iirCorrect;
		Float32 gainchase;
		Float32 settingchase;
		Float32 chasespeed;		

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

	Float32 inputgain = GetParameter( kParam_One );
	Float32 difference;
	Float32 nearZero;
	Float32 servoTrim = 0.0000001f / overallscale;
	Float32 bassTrim = 0.005f / overallscale;
	float inputSample;
	
	if (settingchase != inputgain) {
		chasespeed *= 2.0f;
		settingchase = inputgain;
	}
	if (chasespeed > 2500.0f) chasespeed = 2500.0f;
	if (gainchase < 0.0f) gainchase = inputgain;
	
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		chasespeed *= 0.9999f;
		chasespeed -= 0.01f;
		if (chasespeed < 350.0f) chasespeed = 350.0f;
		//we have our chase speed compensated for recent fader activity
		
		gainchase = (((gainchase*chasespeed)+inputgain)/(chasespeed+1.0f));
		//gainchase is chasing the target, as a simple multiply gain factor
		
		if (1.0f != gainchase) inputSample *= gainchase;
		//done with trim control
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		inputSample = asin(inputSample);
		//amplitude aspect
		
		difference = lastSampleBuss - inputSample;
		lastSampleBuss = inputSample;
		//derive slew part off direct sample measurement + from last time
		if (difference > 1.57079633f) difference = 1.57079633f;
		if (difference < -1.57079633f) difference = -1.57079633f;
		
		difference = lastFXBuss + sin(difference);
		//we're about to use this twice and then not use difference again, so we'll reuse it
		//enhance slew is arcsin(): cutting it back is sin()
		
		iirCorrect += inputSample - difference;
		inputSample = difference;
		//apply the slew to stored value: can develop DC offsets.
		//store the change we made so we can dial it back
		
		
		lastFXBuss = inputSample;		
		if (lastFXBuss > 1.0f) lastFXBuss = 1.0f;
		if (lastFXBuss < -1.0f) lastFXBuss = -1.0f;
		//build new signal off what was present in output last time
		
		nearZero = powf(fabs(fabs(lastFXBuss)-1.0f), 2);
		//if the sample is very near zero this number is higher.
		if (iirCorrect > 0) iirCorrect -= servoTrim;
		if (iirCorrect < 0) iirCorrect += servoTrim;
		//cut back the servo by which we're pulling back the DC
		lastFXBuss += (iirCorrect * 0.0000005f);
		//apply the servo to the stored value, pulling back the DC
		lastFXBuss *= (1.0f - (nearZero * bassTrim));
		//this cuts back the DC offset directly, relative to how near zero we are
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastFXBuss = 0.0;
	lastSampleBuss = 0.0;
	iirCorrect = 0.0;
	gainchase = -90.0;
	settingchase = -90.0;
	chasespeed = 350.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
