#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BitShiftGain"
#define AIRWINDOWS_DESCRIPTION "The ‘One Weird Trick’ perfect boost/pad, but in 6db increments ONLY."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','$' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Bit Shift", .min = -16000, .max = 16000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
	
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
	int bitshiftGain = GetParameter( kParam_One );
	Float32 gain = 1.0f; //default unity gain
	
	switch (bitshiftGain)
	{
		case -16: gain = 0.0000152587890625f; break;
		case -15: gain = 0.000030517578125f; break;
		case -14: gain = 0.00006103515625f; break;
		case -13: gain = 0.0001220703125f; break;
		case -12: gain = 0.000244140625f; break;
		case -11: gain = 0.00048828125f; break;
		case -10: gain = 0.0009765625f; break;
		case -9: gain = 0.001953125f; break;
		case -8: gain = 0.00390625f; break;
		case -7: gain = 0.0078125f; break;
		case -6: gain = 0.015625f; break;
		case -5: gain = 0.03125f; break;
		case -4: gain = 0.0625f; break;
		case -3: gain = 0.125f; break;
		case -2: gain = 0.25f; break;
		case -1: gain = 0.5f; break;
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
	}
	//we are directly punching in the gain values rather than calculating them
	
	while (nSampleFrames-- > 0) {
		*destP = *sourceP * gain;		
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
}
};
