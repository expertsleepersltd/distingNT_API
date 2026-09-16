#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "VinylDither"
#define AIRWINDOWS_DESCRIPTION "A high-performance dither that converts digital noise to ‘groove noise’."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','V','i','r' )
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
 
		Float32 NSOdd;
		Float32 prev;
		Float32 ns[16];
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
	Float32 absSample;
	
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
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		
		
		inputSample *= scaleFactor;
		//0-1 is now one bit, now we dither
		
		absSample = ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[0] += absSample; ns[0] /= 2; absSample -= ns[0];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[1] += absSample; ns[1] /= 2; absSample -= ns[1];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[2] += absSample; ns[2] /= 2; absSample -= ns[2];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[3] += absSample; ns[3] /= 2; absSample -= ns[3];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[4] += absSample; ns[4] /= 2; absSample -= ns[4];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[5] += absSample; ns[5] /= 2; absSample -= ns[5];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[6] += absSample; ns[6] /= 2; absSample -= ns[6];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[7] += absSample; ns[7] /= 2; absSample -= ns[7];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[8] += absSample; ns[8] /= 2; absSample -= ns[8];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[9] += absSample; ns[9] /= 2; absSample -= ns[9];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[10] += absSample; ns[10] /= 2; absSample -= ns[10];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[11] += absSample; ns[11] /= 2; absSample -= ns[11];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[12] += absSample; ns[12] /= 2; absSample -= ns[12];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[13] += absSample; ns[13] /= 2; absSample -= ns[13];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[14] += absSample; ns[14] /= 2; absSample -= ns[14];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[15] += absSample; ns[15] /= 2; absSample -= ns[15];
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		absSample += inputSample;
		//install noise and then shape it
		
		//NSOdd /= 1.0001f; //NSDensity
		
		if (NSOdd > 0) NSOdd -= 0.97f;
		if (NSOdd < 0) NSOdd += 0.97f;
		
		NSOdd -= (NSOdd * NSOdd * NSOdd * 0.475f);
		
		NSOdd += prev;
		absSample += (NSOdd*0.475f);
		prev = floor(absSample) - inputSample;
		inputSample = floor(absSample);
		//TenNines dither
		
		inputSample /= outScale;
		
		*destP = inputSample;
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	NSOdd = 0.0;
	prev = 0.0;
	ns[0] = 0;
	ns[1] = 0;
	ns[2] = 0;
	ns[3] = 0;
	ns[4] = 0;
	ns[5] = 0;
	ns[6] = 0;
	ns[7] = 0;
	ns[8] = 0;
	ns[9] = 0;
	ns[10] = 0;
	ns[11] = 0;
	ns[12] = 0;
	ns[13] = 0;
	ns[14] = 0;
	ns[15] = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
