#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Texturize"
#define AIRWINDOWS_DESCRIPTION "A hidden-noise plugin for adding sonic texture to things."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','e','e' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Bright", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Punchy", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		bool polarity;
		float lastSample;
		float iirSample;
		float noiseA;
		float noiseB;
		float noiseC;
		bool flip;
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
	
	Float32 slewAmount = ((powf(GetParameter( kParam_One ),2.0f)*4.0f)+0.71f)/overallscale;
	Float32 dynAmount = powf(GetParameter( kParam_Two ),2.0f);
	Float32 wet = powf(GetParameter( kParam_Three ),5);
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		if (inputSample < 0) {
			if (polarity == true) {
				if (noiseA < 0) flip = true;
				else flip = false;
			}
			polarity = false;
		} else polarity = true;		
		
		if (flip) noiseA += (float(fpd)/UINT32_MAX);
		else noiseA -= (float(fpd)/UINT32_MAX);
		//here's the guts of the random walk		
		flip = !flip;
		
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		if (dynAmount < 0.4999f) inputSample = (inputSample*dynAmount*2.0f) + (sin(inputSample)*(1.0f-(dynAmount*2.0f)));		
		if (dynAmount > 0.5001f) inputSample = (asin(inputSample)*((dynAmount*2.0f)-1.0f)) + (inputSample*(1.0f-((dynAmount*2.0f)-1.0f)));
		//doing this in two steps means I get to not run an extra sin/asin function per sample
		
		noiseB = sin(noiseA*(0.2f-(dynAmount*0.125f))*fabs(inputSample));
		
		float slew = fabs(inputSample-lastSample)*slewAmount;
		lastSample = inputSample*(0.86f-(dynAmount*0.125f));
		
		if (slew > 1.0f) slew = 1.0f;
		float iirIntensity = slew;
		iirIntensity *= 2.472f;
		iirIntensity *= iirIntensity;
		if (iirIntensity > 1.0f) iirIntensity = 1.0f;
		
		iirSample = (iirSample * (1.0f - iirIntensity)) + (noiseB * iirIntensity);
		noiseB = iirSample;
		noiseB = (noiseB * slew) + (noiseC * (1.0f-slew));
		noiseC = noiseB;
		
		inputSample = (noiseC * wet) + (drySample * (1.0f-wet));
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	polarity = false;
	lastSample = 0.0;
	iirSample = 0.0;
	noiseA = 0.0;
	noiseB = 0.0;
	noiseC = 0.0;
	flip = true;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
