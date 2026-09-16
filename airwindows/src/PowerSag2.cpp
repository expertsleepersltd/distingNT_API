#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PowerSag2"
#define AIRWINDOWS_DESCRIPTION "My improved circuit-starve plugin, now with inverse effect!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','5' )
#define AIRWINDOWS_TAGS kNT_tagEffect
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Range", .min = 0, .max = 1000, .def = 300, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Dry/Wet", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		Float32 control;
		int gcount;		
		uint32_t fpd;
	
	struct _dram {
			Float32 d[16386];
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

	Float32 depth = powf(GetParameter( kParam_One ),4);
	int offset = (int)(depth * 16383) + 1;	
	Float32 wet = GetParameter( kParam_Two );

	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		Float32 drySample = inputSample;

		if (gcount < 0 || gcount > 16384) {gcount = 16384;}		
		dram->d[gcount] = fabs(inputSample);
		control += dram->d[gcount];
		control -= dram->d[gcount+offset-((gcount+offset > 16384)?16384:0)];
		gcount--;
		
		if (control > offset) control = offset; if (control < 0.0f) control = 0.0f;				
		
		Float32 burst = inputSample * (control / sqrt(offset));
		Float32 clamp = inputSample / ((burst == 0.0f)?1.0f:burst);
		
		if (clamp > 1.0f) clamp = 1.0f; if (clamp < 0.0f) clamp = 0.0f;
		inputSample *= clamp;
		Float32 difference = drySample - inputSample;
		if (wet < 0.0f) drySample *= (wet+1.0f);
		inputSample = drySample - (difference * wet);
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 16385; count++) {dram->d[count] = 0;}
	control = 0;
	gcount = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
