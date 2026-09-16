#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Holt2"
#define AIRWINDOWS_DESCRIPTION "A bass-cab-like resonant lowpass filter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','o','2' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Frequency", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Resonance", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Poles", .min = 0, .max = 8000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float previousSampleA;
		float previousTrendA;
		float previousSampleB;
		float previousTrendB;
		float previousSampleC;
		float previousTrendC;
		float previousSampleD;
		float previousTrendD;
		float previousSampleE;
		float previousTrendE;
		float previousSampleF;
		float previousTrendF;
		float previousSampleG;
		float previousTrendG;
		float previousSampleH;
		float previousTrendH;
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
	
	Float32 alpha = powf(GetParameter( kParam_One ),4)+0.00001f;
	if (alpha > 1.0f) alpha = 1.0f;
	Float32 resControl = (GetParameter( kParam_Two )*0.15f)+0.12f;
	Float32 beta = (alpha * powf(resControl,2));
	//0.27f max resonance for full stages on white noise keeping below 0dB
	//0.12f min resonance for not losing all the level as we go down
	//as we remove the 'avoid zero' +0.00001f on beta, our subsonic stability improves
	
	alpha += ((1.0f-beta)*powf(GetParameter( kParam_One ),3)); //correct for droop in frequency
	if (alpha > 1.0f) alpha = 1.0f;
	
	float trend;
	float forecast; //defining these here because we're copying the routine eight times
	
	Float32 aWet = 0.0f;
	Float32 bWet = 0.0f;
	Float32 cWet = 0.0f;
	Float32 dWet = 0.0f;
	Float32 eWet = 0.0f;
	Float32 fWet = 0.0f;
	Float32 gWet = 0.0f;
	Float32 hWet = GetParameter( kParam_Three );
	//eight-stage wet/dry control using progressive stages that bypass when not engaged
	if (hWet < 1.0f) {aWet = hWet; hWet = 0.0f;}
	else if (hWet < 2.0f) {bWet = hWet - 1.0f; aWet = 1.0f; hWet = 0.0f;}
	else if (hWet < 3.0f) {cWet = hWet - 2.0f; bWet = aWet = 1.0f; hWet = 0.0f;}
	else if (hWet < 4.0f) {dWet = hWet - 3.0f; cWet = bWet = aWet = 1.0f; hWet = 0.0f;}
	else if (hWet < 5.0f) {eWet = hWet - 4.0f; dWet = cWet = bWet = aWet = 1.0f; hWet = 0.0f;}
	else if (hWet < 6.0f) {fWet = hWet - 5.0f; eWet = dWet = cWet = bWet = aWet = 1.0f; hWet = 0.0f;}
	else if (hWet < 7.0f) {gWet = hWet - 6.0f; fWet = eWet = dWet = cWet = bWet = aWet = 1.0f; hWet = 0.0f;}
	else {hWet -= 7.0f; gWet = fWet = eWet = dWet = cWet = bWet = aWet = 1.0f;}
	//this is one way to make a little set of dry/wet stages that are successively added to the
	//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	//beyond that point: this is a way to progressively add a 'black box' sound processing
	//which lets you fall through to simpler processing at lower settings.
	
	Float32 gain = GetParameter( kParam_Four );	
	Float32 wet = GetParameter( kParam_Five );	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		if (aWet > 0.0f) {
			trend = (beta * (inputSample - previousSampleA) + ((0.999f-beta) * previousTrendA));
			forecast = previousSampleA + previousTrendA;
			inputSample = (alpha * inputSample) + ((0.999f-alpha) * forecast);
			previousSampleA = inputSample; previousTrendA = trend;
			inputSample = (inputSample * aWet) + (drySample * (1.0f-aWet));
		}
		
		if (bWet > 0.0f) {
			trend = (beta * (inputSample - previousSampleB) + ((0.999f-beta) * previousTrendB));
			forecast = previousSampleB + previousTrendB;
			inputSample = (alpha * inputSample) + ((0.999f-alpha) * forecast);
			previousSampleB = inputSample; previousTrendB = trend;
			inputSample = (inputSample * bWet) + (previousSampleA * (1.0f-bWet));
		}
		
		if (cWet > 0.0f) {
			trend = (beta * (inputSample - previousSampleC) + ((0.999f-beta) * previousTrendC));
			forecast = previousSampleC + previousTrendC;
			inputSample = (alpha * inputSample) + ((0.999f-alpha) * forecast);
			previousSampleC = inputSample; previousTrendC = trend;
			inputSample = (inputSample * cWet) + (previousSampleB * (1.0f-cWet));
		}
		
		if (dWet > 0.0f) {
			trend = (beta * (inputSample - previousSampleD) + ((0.999f-beta) * previousTrendD));
			forecast = previousSampleD + previousTrendD;
			inputSample = (alpha * inputSample) + ((0.999f-alpha) * forecast);
			previousSampleD = inputSample; previousTrendD = trend;
			inputSample = (inputSample * dWet) + (previousSampleC * (1.0f-dWet));
		}
		
		if (eWet > 0.0f) {
			trend = (beta * (inputSample - previousSampleE) + ((0.999f-beta) * previousTrendE));
			forecast = previousSampleE + previousTrendE;
			inputSample = (alpha * inputSample) + ((0.999f-alpha) * forecast);
			previousSampleE = inputSample; previousTrendE = trend;
			inputSample = (inputSample * eWet) + (previousSampleD * (1.0f-eWet));
		}
		
		if (fWet > 0.0f) {
			trend = (beta * (inputSample - previousSampleF) + ((0.999f-beta) * previousTrendF));
			forecast = previousSampleF + previousTrendF;
			inputSample = (alpha * inputSample) + ((0.999f-alpha) * forecast);
			previousSampleF = inputSample; previousTrendF = trend;
			inputSample = (inputSample * fWet) + (previousSampleE * (1.0f-fWet));
		}
		
		if (gWet > 0.0f) {
			trend = (beta * (inputSample - previousSampleG) + ((0.999f-beta) * previousTrendG));
			forecast = previousSampleG + previousTrendG;
			inputSample = (alpha * inputSample) + ((0.999f-alpha) * forecast);
			previousSampleG = inputSample; previousTrendG = trend;
			inputSample = (inputSample * gWet) + (previousSampleF * (1.0f-gWet));
		}
		
		if (hWet > 0.0f) {
			trend = (beta * (inputSample - previousSampleH) + ((0.999f-beta) * previousTrendH));
			forecast = previousSampleH + previousTrendH;
			inputSample = (alpha * inputSample) + ((0.999f-alpha) * forecast);
			previousSampleH = inputSample; previousTrendH = trend;
			inputSample = (inputSample * hWet) + (previousSampleG * (1.0f-hWet));
		}
		
		if (gain < 1.0f) {
			inputSample *= gain;
		}
		
		if (wet < 1.0f) {
			inputSample = (inputSample*wet)+(drySample*(1.0f-wet));
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	previousSampleA = 0.0;
	previousTrendA = 0.0;
	previousSampleB = 0.0;
	previousTrendB = 0.0;
	previousSampleC = 0.0;
	previousTrendC = 0.0;
	previousSampleD = 0.0;
	previousTrendD = 0.0;
	previousSampleE = 0.0;
	previousTrendE = 0.0;
	previousSampleF = 0.0;
	previousTrendF = 0.0;
	previousSampleG = 0.0;
	previousTrendG = 0.0;
	previousSampleH = 0.0;
	previousTrendH = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
