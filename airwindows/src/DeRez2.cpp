#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DeRez2"
#define AIRWINDOWS_DESCRIPTION "Brings more authentic retro-digital hardware tones."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','2' )
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
{ .name = "Rate", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Rez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Hard", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 lastSample;
		Float32 heldSample;
		Float32 lastDrySample;
		Float32 lastOutputSample;
		Float32 position;
		Float32 incrementA;
		Float32 incrementB;
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
	Float32 targetA = powf(GetParameter( kParam_One ),3)+0.0005f;
	if (targetA > 1.0f) targetA = 1.0f;
	Float32 soften = (1.0f + targetA)/2;
	Float32 targetB = powf(1.0f-GetParameter( kParam_Two ),3) / 3;
	Float32 hard = GetParameter( kParam_Three );
	Float32 wet = GetParameter( kParam_Four );
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	targetA /= overallscale;	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		
		incrementA = ((incrementA*999.0f)+targetA)/1000.0f;
		incrementB = ((incrementB*999.0f)+targetB)/1000.0f;
		//incrementA is the frequency derez
		//incrementB is the bit depth derez
		position += incrementA;
		float outputSample = heldSample;
		if (position > 1.0f)
		{
			position -= 1.0f;
			heldSample = (lastSample * position) + (inputSample * (1-position));
			outputSample = (outputSample * (1-soften)) + (heldSample * soften);
			//softens the edge of the derez
		}
		inputSample = outputSample;
		
		float temp = inputSample;

		if (inputSample != lastOutputSample) {
			temp = inputSample;
			inputSample = (inputSample * hard) + (lastDrySample * (1.0f-hard));
			//transitions get an intermediate dry sample
			lastOutputSample = temp; //only one intermediate sample
		} else {
			lastOutputSample = inputSample;
		}
		lastDrySample = drySample;
		//freq section of soft/hard interpolates dry samples

		temp = inputSample;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		if (inputSample > 0) inputSample = log(1.0f+(255*fabs(inputSample))) / log(256);
		if (inputSample < 0) inputSample = -log(1.0f+(255*fabs(inputSample))) / log(256);
		inputSample = (temp * hard) + (inputSample * (1.0f-hard)); //uLaw encode as part of soft/hard
		temp = inputSample;
		
		if (incrementB > 0.0005f)
		{
			if (inputSample > 0)
			{
				while (temp > 0) {temp -= incrementB;}
				inputSample -= temp;
				//it's below 0 so subtracting adds the remainder
			}
			if (inputSample < 0)
			{
				while (temp < 0) {temp += incrementB;}
				inputSample -= temp;
				//it's above 0 so subtracting subtracts the remainder
			}
			inputSample *= (1.0f - incrementB);
		} //resolution section of derez
		
		temp = inputSample;		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f; //this may be superfluous?
		if (inputSample > 0) inputSample = (powf(256,fabs(inputSample))-1.0f) / 255;
		if (inputSample < 0) inputSample = -(powf(256,fabs(inputSample))-1.0f) / 255;
		inputSample = (temp * hard) + (inputSample * (1.0f-hard)); //uLaw decode as part of soft/hard
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		lastSample = drySample;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	heldSample = 0.0;
	lastDrySample = 0.0;
	lastOutputSample = 0.0;
	position = 0.0;
	incrementA = 0.0;
	incrementB = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
