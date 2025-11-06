#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Distance"
#define AIRWINDOWS_DESCRIPTION "A sound design or reverb far-away-izer."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i','v' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "Distance", .min = 0, .max = 2000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 lastclamp;
		Float32 clamp;
		Float32 change;
		Float32 thirdresult;
		Float32 prevresult;
		Float32 last;
		float fpNShape;
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

	//This code will pass-thru the audio data.
	//This is where you want to process data to produce an effect.
	//persistent- lastclamp, clamp, change, last
	
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	Float32 softslew = (powf(GetParameter( kParam_One ),3)*12)+.6;
	softslew *= overallscale;
	Float32 filtercorrect = softslew / 2.0f;
	Float32 thirdfilter = softslew / 3.0f;
	Float32 levelcorrect = 1.0f + (softslew / 6.0f);
	Float32 postfilter;
	Float32 wet = GetParameter( kParam_Two );
	Float32 dry = 1.0f-wet;
	Float32 bridgerectifier;
	Float32 inputSample;
	Float32 drySample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		
		inputSample *= softslew;
		lastclamp = clamp;
		clamp = inputSample - last;
		postfilter = change = fabs(clamp - lastclamp);
		postfilter += filtercorrect;
		if (change > 1.5707963267949f) change = 1.5707963267949f;
		bridgerectifier = (1.0f-sin(change));
		if (bridgerectifier < 0.0f) bridgerectifier = 0.0f;
		inputSample = last + (clamp * bridgerectifier);
		last = inputSample;
		inputSample /= softslew;
		inputSample += (thirdresult * thirdfilter);
		inputSample /= (thirdfilter + 1.0f);
		inputSample += (prevresult * postfilter);
		inputSample /= (postfilter + 1.0f);
		//do an IIR like thing to further squish superdistant stuff
		thirdresult = prevresult;
		prevresult = inputSample;
		inputSample *= levelcorrect;

		if (wet < 1.0f) inputSample = (drySample * dry)+(inputSample*wet);
		
		
		
		*destP = inputSample;
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	thirdresult = prevresult = lastclamp = clamp = change = last = 0.0;
	//just an example
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
