#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PaulDither"
#define AIRWINDOWS_DESCRIPTION "A highpassed TPDF dither. (quieter, airier)"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','a','r' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One = 0,
	kParam_Two = 1,
	//Add your parameters here...
	kNumberOfParameters=2
};
static const int kCD = 0;
static const int kHD = 1;
static const int kDefaultValue_ParamOne = kHD;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, };
static char const * const enumStrings0[] = { "CD 16 bit", "HD 24 bit", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Quantizer", .min = 0, .max = 1, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "DeRez", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		Float32 previousDither;
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
	
	float inputSample;
	Float32 currentDither;
	
	bool highres = false;
	if (GetParameter( kParam_One ) == 1) highres = true;
	Float32 scaleFactor;
	if (highres) scaleFactor = 8388608.0f;
	else scaleFactor = 32768.0f;
	Float32 derez = GetParameter( kParam_Two );
	if (derez > 0.0f) scaleFactor *= powf(1.0f-derez,6);
	if (scaleFactor < 0.0001f) scaleFactor = 0.0001f;
	Float32 outScale = scaleFactor;
	if (outScale < 8.0f) outScale = 8.0f;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		
		inputSample *= scaleFactor;
		//0-1 is now one bit, now we dither
		
		currentDither = (float(fpd)/UINT32_MAX);
		inputSample += currentDither;
		inputSample -= previousDither;
		inputSample = floor(inputSample);
		previousDither = currentDither;
		
		//Paul Frindle: It's true that the dither itself can sound different 
		//if it's given a different freq response and you get to hear it. 
		//The one we use most is triangular single pole high pass dither. 
		//It not freq bent enough sound odd, but is slightly less audible than 
		//flat dither. It can also be easily made by taking one sample of dither 
		//away from the previous one - this gives you the triangular PDF and the 
		//filtering in one go :-)
		
		
		inputSample /= outScale;

		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		//pseudorandom number updater
		
		*destP = inputSample;
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	previousDither = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
