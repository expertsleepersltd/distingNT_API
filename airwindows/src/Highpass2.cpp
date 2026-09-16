#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Highpass2"
#define AIRWINDOWS_DESCRIPTION "An unusual-sounding variable-slope highpass filter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','i','2' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Highpass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Loose/Tight", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Poles", .min = 0, .max = 4000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 iirSampleC;
		Float32 iirSampleD;
		Float32 iirSampleE;
		Float32 iirSampleF;
		Float32 iirSampleG;
		Float32 iirSampleH;
		uint32_t fpd;
		bool fpFlip;
	
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
	Float32 iirAmount = powf(GetParameter( kParam_One ),4);
	Float32 tight = GetParameter( kParam_Two );	
	if (iirAmount < 0.0000001f) iirAmount = 0.0000001f;
	if (iirAmount > 1) iirAmount = 1;
	if (tight < 0.0f) tight *= 0.5f;
	Float32 offset;
	Float32 aWet = 1.0f;
	Float32 bWet = 1.0f;
	Float32 cWet = 1.0f;
	Float32 dWet = GetParameter( kParam_Three );
	//four-stage wet/dry control using progressive stages that bypass when not engaged
	if (dWet < 1.0f) {aWet = dWet; bWet = 0.0f; cWet = 0.0f; dWet = 0.0f;}
	else if (dWet < 2.0f) {bWet = dWet - 1.0f; cWet = 0.0f; dWet = 0.0f;}
	else if (dWet < 3.0f) {cWet = dWet - 2.0f; dWet = 0.0f;}
	else {dWet -= 3.0f;}
	//this is one way to make a little set of dry/wet stages that are successively added to the
	//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	//beyond that point: this is a way to progressively add a 'black box' sound processing
	//which lets you fall through to simpler processing at lower settings.
	Float32 wet = GetParameter( kParam_Four );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
				
		if (tight > 0) offset = (1 - tight) + (fabs(inputSample)*tight);
		else offset = (1 + tight) + ((1-fabs(inputSample))*tight);
		if (offset < 0.0000001f) offset = 0.0000001f; if (offset > 1) offset = 1;

		if (fpFlip)
		{
			if (aWet > 0.0f) {
				iirSampleA = (iirSampleA * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = ((inputSample-iirSampleA) * aWet) + (inputSample * (1.0f-aWet));
			}
			if (bWet > 0.0f) {
				iirSampleC = (iirSampleC * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = ((inputSample-iirSampleC) * bWet) + (inputSample * (1.0f-bWet));
			}
			if (cWet > 0.0f) {
				iirSampleE = (iirSampleE * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = ((inputSample-iirSampleE) * cWet) + (inputSample * (1.0f-cWet));
			}
			if (dWet > 0.0f) {
				iirSampleG = (iirSampleG * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = ((inputSample-iirSampleG) * dWet) + (inputSample * (1.0f-dWet));
			}
		}
		else
		{
			if (aWet > 0.0f) {
				iirSampleB = (iirSampleB * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = ((inputSample-iirSampleB) * aWet) + (inputSample * (1.0f-aWet));
			}
			if (bWet > 0.0f) {
				iirSampleD = (iirSampleD * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = ((inputSample-iirSampleD) * bWet) + (inputSample * (1.0f-bWet));
			}
			if (cWet > 0.0f) {
				iirSampleF = (iirSampleF * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = ((inputSample-iirSampleF) * cWet) + (inputSample * (1.0f-cWet));
			}
			if (dWet > 0.0f) {
				iirSampleH = (iirSampleH * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = ((inputSample-iirSampleH) * dWet) + (inputSample * (1.0f-dWet));
			}
		}
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider
		fpFlip = !fpFlip;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	iirSampleG = 0.0;
	iirSampleH = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = true;
}
};
