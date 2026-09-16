#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PDChannel"
#define AIRWINDOWS_DESCRIPTION "Console5 and PurestDrive, sittin’ in a tree… (i.e. both at once, on channels and buss)"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','D','l' )
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
{ .name = "Gain Trim", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		Float32 chasespeed;		
		Float32 previousSample;
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
	Float32 intensity = GetParameter( kParam_Two );
	Float32 apply;

	float drySample;
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
		
		inputSample = sin(inputSample);
		//amplitude aspect
		//this is the PurestConsole aspect
		
		drySample = inputSample;
		//we also use it for the dry because the PurestDrive needs to be running off a true Console 'encoding'
		//and will just add a little extra saturation on each stage
		//Technically you can use just a PurestDrive in place of a Console5 channel plugin,
		//if you're OK with things being a little extra 'spiky' and dynamic, or if you're
		//working with low level sounds. If so, crank its 'drive' control or it'll be like you
		//didn't even include a plugin at all.
		
		inputSample = sin(inputSample);
		//basic distortion factor
		
		apply = (fabs(previousSample + inputSample) / 2.0f) * intensity;
		//saturate less if previous sample was undistorted and low level, or if it was
		//inverse polarity. Lets through highs and brightness more.
		
		inputSample = (drySample * (1.0f - apply)) + (inputSample * apply);		
		//dry-wet control for intensity also has FM modulation to clean up highs
		
		previousSample = sin(drySample);
		//apply the sine while storing previous sample
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	gainchase = -90.0;
	settingchase = -90.0;
	chasespeed = 350.0;
	previousSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
