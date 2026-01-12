#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BitShiftPan"
#define AIRWINDOWS_DESCRIPTION "Gives you a gain and a pan control that are ONLY done using bit shifts."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','%' )
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Vol", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;

	float gainControl = (GetParameter( kParam_One )*0.5f)+0.05f; //0.0f to 1.0f
	int gainBits = 20; //start beyond maximum attenuation
	if (gainControl > 0.0f) gainBits = floor(1.0f / gainControl);
	int bitshiftL = gainBits - 3;
	int bitshiftR = gainBits - 3;
	float panControl = (GetParameter( kParam_Two )*2.0f)-1.0f; //-1.0f to 1.0f
	float panAttenuation = (1.0f-fabs(panControl));
	int panBits = 20; //start centered
	if (panAttenuation > 0.0f) panBits = floor(1.0f / panAttenuation);
	if (panControl > 0.25f) bitshiftL += panBits;
	if (panControl < -0.25f) bitshiftR += panBits;
	if (bitshiftL < -2) bitshiftL = -2; if (bitshiftL > 17) bitshiftL = 17;
	if (bitshiftR < -2) bitshiftR = -2; if (bitshiftR > 17) bitshiftR = 17;
	float gainL = 1.0f;
	float gainR = 1.0f;
	switch (bitshiftL)
	{
		case 17: gainL = 0.0f; break;
		case 16: gainL = 0.0000152587890625f; break;
		case 15: gainL = 0.000030517578125f; break;
		case 14: gainL = 0.00006103515625f; break;
		case 13: gainL = 0.0001220703125f; break;
		case 12: gainL = 0.000244140625f; break;
		case 11: gainL = 0.00048828125f; break;
		case 10: gainL = 0.0009765625f; break;
		case 9: gainL = 0.001953125f; break;
		case 8: gainL = 0.00390625f; break;
		case 7: gainL = 0.0078125f; break;
		case 6: gainL = 0.015625f; break;
		case 5: gainL = 0.03125f; break;
		case 4: gainL = 0.0625f; break;
		case 3: gainL = 0.125f; break;
		case 2: gainL = 0.25f; break;
		case 1: gainL = 0.5f; break;
		case 0: gainL = 1.0f; break;
		case -1: gainL = 2.0f; break;
		case -2: gainL = 4.0f; break;
	}
	switch (bitshiftR)
	{
		case 17: gainR = 0.0f; break;
		case 16: gainR = 0.0000152587890625f; break;
		case 15: gainR = 0.000030517578125f; break;
		case 14: gainR = 0.00006103515625f; break;
		case 13: gainR = 0.0001220703125f; break;
		case 12: gainR = 0.000244140625f; break;
		case 11: gainR = 0.00048828125f; break;
		case 10: gainR = 0.0009765625f; break;
		case 9: gainR = 0.001953125f; break;
		case 8: gainR = 0.00390625f; break;
		case 7: gainR = 0.0078125f; break;
		case 6: gainR = 0.015625f; break;
		case 5: gainR = 0.03125f; break;
		case 4: gainR = 0.0625f; break;
		case 3: gainR = 0.125f; break;
		case 2: gainR = 0.25f; break;
		case 1: gainR = 0.5f; break;
		case 0: gainR = 1.0f; break;
		case -1: gainR = 2.0f; break;
		case -2: gainR = 4.0f; break;
	}
	
	while (nSampleFrames-- > 0) {
		*outputL = *inputL * gainL; //in this plugin, all the work is done 
		*outputR = *inputR * gainR; //before the buffer is processed
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
