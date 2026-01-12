#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestGain"
#define AIRWINDOWS_DESCRIPTION "A high-res noise shaped gain, with smoothed fader."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','$' )
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
{ .name = "Gain", .min = -4000, .max = 4000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Slow Fade", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		Float32 gainchase;
		Float32 settingchase;
		Float32 gainBchase;
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
	Float32 inputgain = GetParameter( kParam_One );
	
	if (settingchase != inputgain) {
		chasespeed *= 2.0f;
		settingchase = inputgain;
		//increment the slowness for each fader movement
		//continuous alteration makes it react smoother
		//sudden jump to setting, not so much
	}
	if (chasespeed > 2500.0f) chasespeed = 2500.0f;
	//bail out if it's too extreme
	if (gainchase < -60.0f) {
		gainchase = powf(10.0f,inputgain/20.0f);
		//shouldn't even be a negative number
		//this is about starting at whatever's set, when
		//plugin is instantiated.
		//Otherwise it's the target, in dB.
	}
	Float32 targetgain;	
	//done with top controller
			
	Float32 targetBgain = GetParameter( kParam_Two );
	if (gainBchase < 0.0f) gainBchase = targetBgain;
	//this one is not a dB value, but straight multiplication
	//done with slow fade controller

	Float32 outputgain;
	
	float inputSample;
	
	while (nSampleFrames-- > 0) {

		targetgain = powf(10.0f,settingchase/20.0f);
		//now we have the target in our temp variable
		
		chasespeed *= 0.9999f;
		chasespeed -= 0.01f;
		if (chasespeed < 350.0f) chasespeed = 350.0f;
		//we have our chase speed compensated for recent fader activity

		gainchase = (((gainchase*chasespeed)+targetgain)/(chasespeed+1.0f));
		//gainchase is chasing the target, as a simple multiply gain factor

		gainBchase = (((gainBchase*4000)+targetBgain)/4001);
		//gainchase is chasing the target, as a simple multiply gain factor
		
		outputgain = gainchase * gainBchase;
		//directly multiply the dB gain by the straight multiply gain
		
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		
		if (1.0f == outputgain) *destP = *sourceP;
		else
			{
				inputSample *= outputgain;
				
				*destP = inputSample;
			}		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	gainchase = -90.0;
	settingchase = -90.0;
	gainBchase = -90.0;
	chasespeed = 350.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
