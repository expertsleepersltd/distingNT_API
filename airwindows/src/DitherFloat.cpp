#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DitherFloat"
#define AIRWINDOWS_DESCRIPTION "A demo of floating point truncation: can also dither to 32 bit float output."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i',' ' )
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
{ .name = "Float Offset", .min = 0, .max = 32000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dither Engage", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

	int floatOffset = GetParameter( kParam_One );
	float blend = GetParameter( kParam_Two );

	float gain = 0;
	
	switch (floatOffset)
	{
		case 0: gain = 1.0f; break;
		case 1: gain = 2.0f; break;
		case 2: gain = 4.0f; break;
		case 3: gain = 8.0f; break;
		case 4: gain = 16.0f; break;
		case 5: gain = 32.0f; break;
		case 6: gain = 64.0f; break;
		case 7: gain = 128.0f; break;
		case 8: gain = 256.0f; break;
		case 9: gain = 512.0f; break;
		case 10: gain = 1024.0f; break;
		case 11: gain = 2048.0f; break;
		case 12: gain = 4096.0f; break;
		case 13: gain = 8192.0f; break;
		case 14: gain = 16384.0f; break;
		case 15: gain = 32768.0f; break;
		case 16: gain = 65536.0f; break;
		case 17: gain = 131072.0f; break;
		case 18: gain = 262144.0f; break;
		case 19: gain = 524288.0f; break;
		case 20: gain = 1048576.0f; break;
		case 21: gain = 2097152.0f; break;
		case 22: gain = 4194304.0f; break;
		case 23: gain = 8388608.0f; break;
		case 24: gain = 16777216.0f; break;
		case 25: gain = 33554432.0f; break;
		case 26: gain = 67108864.0f; break;
		case 27: gain = 134217728.0f; break;
		case 28: gain = 268435456.0f; break;
		case 29: gain = 536870912.0f; break;
		case 30: gain = 1073741824.0f; break;
		case 31: gain = 2147483648.0f; break;
		case 32: gain = 4294967296.0f; break;
	}
	//we are directly punching in the gain values rather than calculating them
	//gain *= gain; //for testing with float precision, 64 bit
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP + (gain-1); //this offsets the float into total truncation-land.
		

		
		

		/* copy and paste into code, fpd is a uint32_t initialized as 17 (NOT 0)
		 
		
		
		//begin 64 bit floating point dither
		int expon; frexp((float)inputSample, &expon);
		fpd ^= fpd<<13; fpd ^= fpd>>17; fpd ^= fpd<<5;
		inputSample += (fpd*6.4e-45l*powf(2,expon+62));	
		//end 64 bit floating point dither
		 
		*/
		
		
		inputSample = (float)inputSample; //equivalent of 'floor' for 32 bit floating point
		//We do that separately, we're truncating to floating point WHILE heavily offset.
		
		*destP = inputSample - (gain-1); //this de-offsets the float.
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
