#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PitchNasty"
#define AIRWINDOWS_DESCRIPTION "A primitive pitch shifter that also has primitive time-stretch artifacts."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','i',' ' )
#define AIRWINDOWS_TAGS kNT_tagEffect
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Note", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bend", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Grind", .min = -3600, .max = 3600, .def = -1200, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Feedback", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		int inCount;
		float outCount;
		bool switchTransition;
		float switchAmount;
		float feedbackSample;
		uint32_t fpd;
	
	struct _dram {
			float d[10002];
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
	
	float note = GetParameter( kParam_One );// -12 to +12
	float bend = GetParameter( kParam_Two );// -12.0f to +12.0f
	float speed = powf(2,note/12.0f)*powf(2,bend/12.0f);
	
	float grindRef = (1.0f/261.6f)*GetSampleRate(); //samples per Middle C cycle
	float grindNote = powf(2,GetParameter( kParam_Three )/12.0f); // -36 to 36
	float width = grindRef / grindNote;
	if (width > 9990) width = 9990; //safety check
	float feedback = GetParameter( kParam_Four );
	float trim = GetParameter( kParam_Five );
	float wet = GetParameter( kParam_Six );

	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		inputSample += (feedbackSample * feedback);
		if (fabs(feedbackSample) > 1.0f) inputSample /= fabs(feedbackSample);
		
		dram->d[inCount] = inputSample;
		
		inCount++;
		outCount += speed;
		
		if (outCount > inCount && switchTransition) {
			switchAmount = 1.0f;
			switchTransition = false;
		}
				
		if (inCount > width) inCount -= width;
		if (outCount > width) {
			outCount -= width;
			switchTransition = true;
			//reset the transition point
		}
				
		int count = floor(outCount);
		int arrayWidth = floor(width);
		
		inputSample = (dram->d[count-((count > arrayWidth)?arrayWidth+1:0)] * (1-(outCount-floor(outCount))));
		inputSample += (dram->d[count+1-((count+1 > arrayWidth)?arrayWidth+1:0)] * (outCount-floor(outCount)));
		
		if (switchAmount > fabs(inputSample-dram->d[inCount])*2.0f) {
			switchAmount = (switchAmount * 0.5f) + (fabs(inputSample-dram->d[inCount]));
		}
		inputSample = (dram->d[inCount] * switchAmount) + (inputSample * (1.0f-switchAmount));
		
		feedbackSample = inputSample;
		
		if (trim != 1.0f) {
			inputSample *= trim;
		}
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int temp = 0; temp < 10001; temp++) {dram->d[temp] = 0.0;}
	inCount = 1;	
	outCount = 1.0;
	switchTransition = false;
	switchAmount = 0.0;
	feedbackSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
