#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "StereoFX"
#define AIRWINDOWS_DESCRIPTION "An aggressive stereo widener. Historical note: this post included in full as it's right after my Dad died, and includes my thanks to Airwindows folks for supporting me through that:"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','t','X' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Stereo Wide", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mono Bass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Center Squish", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	Float32 iirSampleA;
	Float32 iirSampleB;
	uint32_t fpdL;
	uint32_t fpdR;
	bool flip;
	

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	float inputSampleL;
	float inputSampleR;
	float mid;
	float side;
	//High Impact section
	Float32 stereowide = GetParameter( kParam_One );
	Float32 centersquish = GetParameter( kParam_Three );
	Float32 density = stereowide * 2.4f;
	Float32 sustain = 1.0f - (1.0f/(1.0f + (density/7.0f)));
	//this way, enhance increases up to 50% and then mid falls off beyond that
	Float32 bridgerectifier;
	Float32 count;
	//Highpass section
	Float32 iirAmount = powf(GetParameter( kParam_Two ),3)/overallscale;
	Float32 tight = -0.33333333333333f;
	Float32 offset;
	//we are setting it up so that to either extreme we can get an audible sound,
	//but sort of scaled so small adjustments don't shift the cutoff frequency yet.
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		//assign working variables		
		mid = inputSampleL + inputSampleR;
		side = inputSampleL - inputSampleR;
		//assign mid and side. Now, High Impact code
		count = density;
		while (count > 1.0f)
			{
				bridgerectifier = fabs(side)*1.57079633f;
				if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
				//max value for sine function
				bridgerectifier = sin(bridgerectifier);
				if (side > 0.0f) side = bridgerectifier;
				else side = -bridgerectifier;
				count = count - 1.0f;
			}
		//we have now accounted for any really high density settings.
		bridgerectifier = fabs(side)*1.57079633f;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (side > 0) side = (side*(1-count))+(bridgerectifier*count);
		else side = (side*(1-count))-(bridgerectifier*count);
		//blend according to density control
		//done first density. Next, sustain-reducer
		bridgerectifier = fabs(side)*1.57079633f;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (1-cos(bridgerectifier))*3.141592653589793f;
		if (side > 0) side = (side*(1-sustain))+(bridgerectifier*sustain);
		else side = (side*(1-sustain))-(bridgerectifier*sustain);
		//done with High Impact code
		
		//now, Highpass code
		offset = 0.666666666666666f + ((1-fabs(side))*tight);
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		if (flip)
			{
			iirSampleA = (iirSampleA * (1 - (offset * iirAmount))) + (side * (offset * iirAmount));
			side = side - iirSampleA;
			}
		else
			{
			iirSampleB = (iirSampleB * (1 - (offset * iirAmount))) + (side * (offset * iirAmount));
			side = side - iirSampleB;
			}
		//done with Highpass code
		
		bridgerectifier = fabs(mid)/1.273239544735162f;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier)*1.273239544735162f;
		if (mid > 0) mid = (mid*(1-centersquish))+(bridgerectifier*centersquish);
		else mid = (mid*(1-centersquish))-(bridgerectifier*centersquish);
		//done with the mid saturating section.
		
		inputSampleL = (mid+side)/2.0f;
		inputSampleR = (mid-side)/2.0f;
		flip = !flip;
		
		
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//don't know why we're getting a volume boost, cursed thing
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
};
int _airwindowsAlgorithm::reset(void) {

{
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	flip = false;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}


};
