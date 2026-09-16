#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "NotJustAnotherDither"
#define AIRWINDOWS_DESCRIPTION "A next-gen wordlength reducer at 24 bit, with DeRez."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','N','o','s' )
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
 
		Float32 byn[13];
		float noiseShaping;
	
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
		float inputSample = *sourceP;
		
		inputSample *= scaleFactor;
		//0-1 is now one bit, now we dither
		
		bool cutbins; cutbins = false;
		float drySample = inputSample;
		inputSample -= noiseShaping;
		
		float benfordize; benfordize = floor(inputSample);
		while (benfordize >= 1.0f) benfordize /= 10;
		while (benfordize < 1.0f && benfordize > 0.0000001f) benfordize *= 10;
		int hotbinA; hotbinA = floor(benfordize);
		//hotbin becomes the Benford bin value for this number floored
		float totalA; totalA = 0;
		if ((hotbinA > 0) && (hotbinA < 10))
		{
			byn[hotbinA] += 1; if (byn[hotbinA] > 982) cutbins = true;
			totalA += (301-byn[1]); totalA += (176-byn[2]); totalA += (125-byn[3]);
			totalA += (97-byn[4]); totalA += (79-byn[5]); totalA += (67-byn[6]);
			totalA += (58-byn[7]); totalA += (51-byn[8]); totalA += (46-byn[9]);
			byn[hotbinA] -= 1;
		} else hotbinA = 10;
		//produce total number- smaller is closer to Benford real
		
		benfordize = ceil(inputSample);
		while (benfordize >= 1.0f) benfordize /= 10;
		while (benfordize < 1.0f && benfordize > 0.0000001f) benfordize *= 10;
		int hotbinB; hotbinB = floor(benfordize);
		//hotbin becomes the Benford bin value for this number ceiled
		float totalB; totalB = 0;
		if ((hotbinB > 0) && (hotbinB < 10))
		{
			byn[hotbinB] += 1; if (byn[hotbinB] > 982) cutbins = true;
			totalB += (301-byn[1]); totalB += (176-byn[2]); totalB += (125-byn[3]);
			totalB += (97-byn[4]); totalB += (79-byn[5]); totalB += (67-byn[6]);
			totalB += (58-byn[7]); totalB += (51-byn[8]); totalB += (46-byn[9]);
			byn[hotbinB] -= 1;
		} else hotbinB = 10;
		//produce total number- smaller is closer to Benford real
		
		float outputSample;
		if (totalA < totalB) {byn[hotbinA] += 1; outputSample = floor(inputSample);}
		else {byn[hotbinB] += 1; outputSample = floor(inputSample+1);}
		//assign the relevant one to the delay line
		//and floor/ceil signal accordingly
		if (cutbins) {
			byn[1] *= 0.99f; byn[2] *= 0.99f; byn[3] *= 0.99f; byn[4] *= 0.99f; byn[5] *= 0.99f; 
			byn[6] *= 0.99f; byn[7] *= 0.99f; byn[8] *= 0.99f; byn[9] *= 0.99f; byn[10] *= 0.99f; 
		}
		noiseShaping += outputSample - drySample;
		if (noiseShaping > fabs(inputSample)) noiseShaping = fabs(inputSample);
		if (noiseShaping < -fabs(inputSample)) noiseShaping = -fabs(inputSample);		
		
		inputSample = outputSample / outScale;
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
				
		*destP = inputSample;
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	byn[0] = 1000.0;
	byn[1] = 301.0;
	byn[2] = 176.0;
	byn[3] = 125.0;
	byn[4] = 97.0;
	byn[5] = 79.0;
	byn[6] = 67.0;
	byn[7] = 58.0;
	byn[8] = 51.0;
	byn[9] = 46.0;
	byn[10] = 1000.0;
	noiseShaping = 0.0;
}
};
