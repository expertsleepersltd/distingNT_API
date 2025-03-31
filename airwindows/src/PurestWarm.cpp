#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestWarm"
#define AIRWINDOWS_DESCRIPTION "A subtle tone shaper and warmth adder."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','%' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
static const int kNormal = 1;
static const int kInverse = 2;
static const int kDefaultValue_ParamOne = kNormal;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, };
static char const * const enumStrings0[] = { "", "Normal", "Inverse", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Polarity", .min = 1, .max = 2, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	int polarity = (int) GetParameter( kParam_One );
	double inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		if (polarity == 1)
		{
			if (inputSample < 0) 
			{
				inputSample = -(sin(-inputSample*1.57079634)/1.57079634);
				
				//begin 32 bit floating point dither
				int expon; frexpf((float)inputSample, &expon);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
				inputSample += static_cast<int32_t>(fpd) * 5.960464655174751e-36L * pow(2,expon+62);
				//end 32 bit floating point dither
			}
		} else {
			if (inputSample > 0)
			{
				inputSample = sin(inputSample*1.57079634)/1.57079634;
				
				//begin 32 bit floating point dither
				int expon; frexpf((float)inputSample, &expon);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
				inputSample += static_cast<int32_t>(fpd) * 5.960464655174751e-36L * pow(2,expon+62);
				//end 32 bit floating point dither
			}
		}
		//that's it. Only applies on one half of the waveform, other half is passthrough untouched.
		//even the dither to the 32 bit buss is only applied as needed.
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
