#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Console4Buss"
#define AIRWINDOWS_DESCRIPTION "The original Airwindows ITB mixing system."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','v' )
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
 
		
		Float32 lastSample;
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
	
	Float32 overallscale = GetSampleRate();
	if (overallscale < 16000.0f) overallscale = 16000.0f;
	//insanity check to not divide by zero if GSR() has failed
	overallscale /= 44100.0f;
	// 1 for CD rate, more if it's a high sample rate
	
	float inputSample;
	float half;
	float falf;
	Float32 slewcompensation;
	Float32 inputgain = GetParameter( kParam_One );
	if (settingchase != inputgain) {
		chasespeed *= 2.0f;
		settingchase = inputgain;
	}
	if (chasespeed > 2500.0f) chasespeed = 2500.0f;
	if (gainchase < 0.0f) gainchase = inputgain;
	
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		//if we're going to do a gain, we must apply it to everything here
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		
		chasespeed *= 0.9999f;
		chasespeed -= 0.01f;
		if (chasespeed < 350.0f) chasespeed = 350.0f;
		//we have our chase speed compensated for recent fader activity
		
		gainchase = (((gainchase*chasespeed)+inputgain)/(chasespeed+1.0f));
		//gainchase is chasing the target, as a simple multiply gain factor
		
		if (1.0f != gainchase) inputSample *= gainchase;
		//done with trim control

		half = inputSample;
		falf = fabs(half);
		half *= falf;
		half *= falf;
		
		slewcompensation = fabs(inputSample - lastSample) * overallscale;
		//magnify effect at high sample rate so it will still register when inter-sample changes
		//are very small at high rates.
		
		if (slewcompensation > 1.0f) slewcompensation = 1.0f;
		//let's not invert the effect: maximum application is to cancel out half entirely
		
		half *= (1.0f - slewcompensation);
		//apply it
		
		lastSample = inputSample;
		
		inputSample += half;
		//this is the inverse processing for Console: boosts but not so much if there's slew.
		//is this too subtle an effect?
				
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	gainchase = -90.0;
	settingchase = -90.0;
	chasespeed = 350.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
