#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Console4Buss"
#define AIRWINDOWS_DESCRIPTION "The original Airwindows ITB mixing system."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','p' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Input Pad", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		
		Float64 lastSample;
		Float64 gainchase;
		Float64 settingchase;
		Float64 chasespeed;
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	
	Float64 overallscale = GetSampleRate();
	if (overallscale < 16000.0) overallscale = 16000.0;
	//insanity check to not divide by zero if GSR() has failed
	overallscale /= 44100.0;
	// 1 for CD rate, more if it's a high sample rate
	
	double inputSample;
	double half;
	double falf;
	Float64 slewcompensation;
	Float64 inputgain = GetParameter( kParam_One );
	if (settingchase != inputgain) {
		chasespeed *= 2.0;
		settingchase = inputgain;
	}
	if (chasespeed > 2500.0) chasespeed = 2500.0;
	if (gainchase < 0.0) gainchase = inputgain;
	
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		//if we're going to do a gain, we must apply it to everything here
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;

		
		chasespeed *= 0.9999;
		chasespeed -= 0.01;
		if (chasespeed < 350.0) chasespeed = 350.0;
		//we have our chase speed compensated for recent fader activity
		
		gainchase = (((gainchase*chasespeed)+inputgain)/(chasespeed+1.0));
		//gainchase is chasing the target, as a simple multiply gain factor
		
		if (1.0 != gainchase) inputSample *= gainchase;
		//done with trim control

		half = inputSample;
		falf = fabs(half);
		half *= falf;
		half *= falf;
		
		slewcompensation = fabs(inputSample - lastSample) * overallscale;
		//magnify effect at high sample rate so it will still register when inter-sample changes
		//are very small at high rates.
		
		if (slewcompensation > 1.0) slewcompensation = 1.0;
		//let's not invert the effect: maximum application is to cancel out half entirely
		
		half *= (1.0 - slewcompensation);
		//apply it
		
		lastSample = inputSample;
		
		inputSample += half;
		//this is the inverse processing for Console: boosts but not so much if there's slew.
		//is this too subtle an effect?
				
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
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
