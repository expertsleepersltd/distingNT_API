#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Vibrato"
#define AIRWINDOWS_DESCRIPTION "Lets you vibrato, chorus, flange, and make odd FM noises."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','V','i','o' )
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
{ .name = "Main Speed", .min = 0, .max = 1000, .def = 300, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Main Depth", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FM Speed", .min = 0, .max = 1000, .def = 400, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FM Depth", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Dry/Wet", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 sweep;
		Float32 sweepB;
		int gcount;
		Float32 airPrev;
		Float32 airEven;
		Float32 airOdd;
		Float32 airFactor;
		bool flip;
		uint32_t fpd;
	
	struct _dram {
			Float32 p[16386]; //this is processed, not raw incoming samples
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
	Float32 speed = powf(0.1f+GetParameter( kParam_One ),6);
	Float32 depth = (powf(GetParameter( kParam_Two ),3) / sqrt(speed))*4.0f;
	Float32 speedB = powf(0.1f+GetParameter( kParam_Three ),6);
	Float32 depthB = powf(GetParameter( kParam_Four ),3) / sqrt(speedB);
	Float32 tupi = 3.141592653589793238f * 2.0f;
	Float32 wet = GetParameter( kParam_Five ); //note: inv/dry/wet
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		airFactor = airPrev - inputSample;
		if (flip) {airEven += airFactor; airOdd -= airFactor; airFactor = airEven;}
		else {airOdd += airFactor; airEven -= airFactor; airFactor = airOdd;}
		airOdd = (airOdd - ((airOdd - airEven)/256.0f)) / 1.0001f;
		airEven = (airEven - ((airEven - airOdd)/256.0f)) / 1.0001f;
		airPrev = inputSample;
		inputSample += airFactor;
		flip = !flip;
		//air, compensates for loss of highs in the interpolation
		
		if (gcount < 1 || gcount > 8192) {gcount = 8192;}
		int count = gcount;
		dram->p[count+8192] = dram->p[count] = inputSample;
		
		Float32 offset = depth + (depth * sin(sweep));
		count += (int)floor(offset);
		
		inputSample = dram->p[count] * (1-(offset-floor(offset))); //less as value moves away from .0
		inputSample += dram->p[count+1]; //we can assume always using this in one way or another?
		inputSample += dram->p[count+2] * (offset-floor(offset)); //greater as value moves away from .0
		inputSample -= ((dram->p[count]-dram->p[count+1])-(dram->p[count+1]-dram->p[count+2]))/50; //interpolation hacks 'r us
		inputSample *= 0.5f; // gain trim
		
		sweep += (speed + (speedB * sin(sweepB) * depthB));
		sweepB += speedB;
		if (sweep > tupi){sweep -= tupi;}
		if (sweep < 0.0f){sweep += tupi;} //through zero FM
		if (sweepB > tupi){sweepB -= tupi;}
		gcount--;
		//still scrolling through the samples, remember
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-fabs(wet)));
		}
		//Inv/Dry/Wet control

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 16385; count++) {dram->p[count] = 0;}
	sweep = 3.141592653589793238 / 2.0;
	sweepB = 3.141592653589793238 / 2.0;
	gcount = 0;
	airPrev = 0.0;
	airEven = 0.0;
	airOdd = 0.0;
	airFactor = 0.0;
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
