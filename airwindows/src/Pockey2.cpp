#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pockey2"
#define AIRWINDOWS_DESCRIPTION "More efficient, more intense lo-fi hiphop in a plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','2' )
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
{ .name = "DeFreq", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DeRez", .min = 4000, .max = 16000, .def = 12000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float lastSample;
		float heldSample;
		float previousHeld;
		int position;
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

	int freq = floor(powf(GetParameter( kParam_One ),3)*32.0f*overallscale);
	//dividing of derez must always be integer values now: no freq grinding
	
	float rez = GetParameter( kParam_Two);
	//4 to 16, with 12 being the default.
	int rezFactor = (int)powf(2,rez); //256, 4096, 65536 or anything in between
	
	float wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
				
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		if (inputSample > 0) inputSample = log(1.0f+(255*fabs(inputSample)))/log(255);
		if (inputSample < 0) inputSample = -log(1.0f+(255*fabs(inputSample)))/log(255);
		//end uLaw encode		
		
		inputSample *= rezFactor;		
		if (inputSample > 0) inputSample = floor(inputSample);
		if (inputSample < 0) inputSample = -floor(-inputSample);
		inputSample /= rezFactor;
		
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		if (inputSample > 0) inputSample = (powf(256,fabs(inputSample))-1.0f) / 255;
		if (inputSample < 0) inputSample = -(powf(256,fabs(inputSample))-1.0f) / 255;
		//end uLaw decode
		
		float blur = 0.618033988749894848204586f-(fabs(inputSample - lastSample)*overallscale);
		if (blur < 0.0f) blur = 0.0f; //reverse it. Mellow stuff gets blur, bright gets edge
		
		if (position < 1)
		{
			position = freq; //one to ? scaled by overallscale
			heldSample = inputSample;			
		}
		inputSample = heldSample;
		lastSample = drySample;
		position--;

		inputSample = (inputSample * blur) + (previousHeld * (1.0f-blur));
		//conditional average: only if we actually have brightness
		previousHeld = heldSample;
		//end Frequency Derez
		
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
	lastSample = 0.0;
	heldSample = 0.0;
	previousHeld = 0.0;
	position = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
