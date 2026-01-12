#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BitDualPan"
#define AIRWINDOWS_DESCRIPTION "BitDualPan"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','!' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "L Vol", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "L Pan", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "R Vol", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "R Pan", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	
	float gainControl = (GetParameter( kParam_A )*0.5f)+0.05f; //0.0f to 1.0f
	int gainBits = 20; //start beyond maximum attenuation
	if (gainControl > 0.0f) gainBits = floor(1.0f / gainControl);
	int bitshiftL = gainBits - 3;
	int bitshiftR = gainBits - 3;
	float panControl = (GetParameter( kParam_B )*2.0f)-1.0f; //-1.0f to 1.0f
	float panAttenuation = (1.0f-fabs(panControl));
	int panBits = 20; //start centered
	if (panAttenuation > 0.0f) panBits = floor(1.0f / panAttenuation);
	if (panControl > 0.25f) bitshiftL += panBits;
	if (panControl < -0.25f) bitshiftR += panBits;
	if (bitshiftL < -2) bitshiftL = -2; if (bitshiftL > 17) bitshiftL = 17;
	if (bitshiftR < -2) bitshiftR = -2; if (bitshiftR > 17) bitshiftR = 17;
	float LgainL = 1.0f;
	float LgainR = 1.0f;
	switch (bitshiftL)
	{
		case 17: LgainL = 0.0f; break;
		case 16: LgainL = 0.0000152587890625f; break;
		case 15: LgainL = 0.000030517578125f; break;
		case 14: LgainL = 0.00006103515625f; break;
		case 13: LgainL = 0.0001220703125f; break;
		case 12: LgainL = 0.000244140625f; break;
		case 11: LgainL = 0.00048828125f; break;
		case 10: LgainL = 0.0009765625f; break;
		case 9: LgainL = 0.001953125f; break;
		case 8: LgainL = 0.00390625f; break;
		case 7: LgainL = 0.0078125f; break;
		case 6: LgainL = 0.015625f; break;
		case 5: LgainL = 0.03125f; break;
		case 4: LgainL = 0.0625f; break;
		case 3: LgainL = 0.125f; break;
		case 2: LgainL = 0.25f; break;
		case 1: LgainL = 0.5f; break;
		case 0: LgainL = 1.0f; break;
		case -1: LgainL = 2.0f; break;
		case -2: LgainL = 4.0f; break;
	}
	switch (bitshiftR)
	{
		case 17: LgainR = 0.0f; break;
		case 16: LgainR = 0.0000152587890625f; break;
		case 15: LgainR = 0.000030517578125f; break;
		case 14: LgainR = 0.00006103515625f; break;
		case 13: LgainR = 0.0001220703125f; break;
		case 12: LgainR = 0.000244140625f; break;
		case 11: LgainR = 0.00048828125f; break;
		case 10: LgainR = 0.0009765625f; break;
		case 9: LgainR = 0.001953125f; break;
		case 8: LgainR = 0.00390625f; break;
		case 7: LgainR = 0.0078125f; break;
		case 6: LgainR = 0.015625f; break;
		case 5: LgainR = 0.03125f; break;
		case 4: LgainR = 0.0625f; break;
		case 3: LgainR = 0.125f; break;
		case 2: LgainR = 0.25f; break;
		case 1: LgainR = 0.5f; break;
		case 0: LgainR = 1.0f; break;
		case -1: LgainR = 2.0f; break;
		case -2: LgainR = 4.0f; break;
	}

	gainControl = (GetParameter( kParam_C )*0.5f)+0.05f; //0.0f to 1.0f
	gainBits = 20; //start beyond maximum attenuation
	if (gainControl > 0.0f) gainBits = floor(1.0f / gainControl);
	bitshiftL = gainBits - 3;
	bitshiftR = gainBits - 3;
	panControl = (GetParameter( kParam_D )*2.0f)-1.0f; //-1.0f to 1.0f
	panAttenuation = (1.0f-fabs(panControl));
	panBits = 20; //start centered
	if (panAttenuation > 0.0f) panBits = floor(1.0f / panAttenuation);
	if (panControl > 0.25f) bitshiftL += panBits;
	if (panControl < -0.25f) bitshiftR += panBits;
	if (bitshiftL < -2) bitshiftL = -2; if (bitshiftL > 17) bitshiftL = 17;
	if (bitshiftR < -2) bitshiftR = -2; if (bitshiftR > 17) bitshiftR = 17;
	float RgainL = 1.0f;
	float RgainR = 1.0f;
	switch (bitshiftL)
	{
		case 17: RgainL = 0.0f; break;
		case 16: RgainL = 0.0000152587890625f; break;
		case 15: RgainL = 0.000030517578125f; break;
		case 14: RgainL = 0.00006103515625f; break;
		case 13: RgainL = 0.0001220703125f; break;
		case 12: RgainL = 0.000244140625f; break;
		case 11: RgainL = 0.00048828125f; break;
		case 10: RgainL = 0.0009765625f; break;
		case 9: RgainL = 0.001953125f; break;
		case 8: RgainL = 0.00390625f; break;
		case 7: RgainL = 0.0078125f; break;
		case 6: RgainL = 0.015625f; break;
		case 5: RgainL = 0.03125f; break;
		case 4: RgainL = 0.0625f; break;
		case 3: RgainL = 0.125f; break;
		case 2: RgainL = 0.25f; break;
		case 1: RgainL = 0.5f; break;
		case 0: RgainL = 1.0f; break;
		case -1: RgainL = 2.0f; break;
		case -2: RgainL = 4.0f; break;
	}
	switch (bitshiftR)
	{
		case 17: RgainR = 0.0f; break;
		case 16: RgainR = 0.0000152587890625f; break;
		case 15: RgainR = 0.000030517578125f; break;
		case 14: RgainR = 0.00006103515625f; break;
		case 13: RgainR = 0.0001220703125f; break;
		case 12: RgainR = 0.000244140625f; break;
		case 11: RgainR = 0.00048828125f; break;
		case 10: RgainR = 0.0009765625f; break;
		case 9: RgainR = 0.001953125f; break;
		case 8: RgainR = 0.00390625f; break;
		case 7: RgainR = 0.0078125f; break;
		case 6: RgainR = 0.015625f; break;
		case 5: RgainR = 0.03125f; break;
		case 4: RgainR = 0.0625f; break;
		case 3: RgainR = 0.125f; break;
		case 2: RgainR = 0.25f; break;
		case 1: RgainR = 0.5f; break;
		case 0: RgainR = 1.0f; break;
		case -1: RgainR = 2.0f; break;
		case -2: RgainR = 4.0f; break;
	}
	
	while (nSampleFrames-- > 0) {
		float monoL = *inputL;
		float monoR = *inputR;
				
		*outputL = (monoL*LgainL)+(monoR*RgainL);
		*outputR = (monoL*LgainR)+(monoR*RgainR);
		
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
