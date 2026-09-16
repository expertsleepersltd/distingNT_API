#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Flipity"
#define AIRWINDOWS_DESCRIPTION "A channel phase flipper/swapper utility."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','l','y' )
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
static const int kDry = 1;
static const int kFlipL = 2;
static const int kFlipR = 3;
static const int kFlipLR = 4;
static const int kSwap = 5;
static const int kSwipL = 6;
static const int kSwipR = 7;
static const int kSwipLR = 8;
static const int kDefaultValue_ParamOne = kDry;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, };
static char const * const enumStrings0[] = { "", "Dry", "Flip L", "Flip R", "Flip LR", "Swap LR", "Swip L", "Swip R", "Swip LR", };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Flipity", .min = 1, .max = 8, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	
	int flipity = (int) GetParameter( kParam_One );

	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		float temp;

		switch (flipity)
		{
			case kDry:
				break;
			case kFlipL:
				inputSampleL = -inputSampleL;
				break;
			case kFlipR:
				inputSampleR = -inputSampleR;
				break;
			case kFlipLR:
				inputSampleL = -inputSampleL;
				inputSampleR = -inputSampleR;
				break;
			case kSwap:
				temp = inputSampleL;
				inputSampleL = inputSampleR;
				inputSampleR = temp;
				break;
			case kSwipL:
				temp = inputSampleL;
				inputSampleL = -inputSampleR;
				inputSampleR = temp;
				break;
			case kSwipR:
				temp = inputSampleL;
				inputSampleL = inputSampleR;
				inputSampleR = -temp;
				break;
			case kSwipLR:
				temp = inputSampleL;
				inputSampleL = -inputSampleR;
				inputSampleR = -temp;
				break;
		}
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//direct stereo out
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	return noErr;
}

};
