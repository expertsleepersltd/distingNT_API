#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Inflamer"
#define AIRWINDOWS_DESCRIPTION "An Airwindowsized take on the waveshapers in Oxford Inflator."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','n','r' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Curve", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Effect", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
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
	
	float gainControl = (GetParameter( kParam_One )*0.5f)+0.05f; //0.0f to 1.0f
	int gainBits = 20; //start beyond maximum attenuation
	if (gainControl > 0.0f) gainBits = floor(1.0f / gainControl)-2;
	if (gainBits < -2) gainBits = -2; if (gainBits > 17) gainBits = 17;
	float gain = 1.0f;
	switch (gainBits)
	{
		case 17: gain = 0.0f; break;
		case 16: gain = 0.0000152587890625f; break;
		case 15: gain = 0.000030517578125f; break;
		case 14: gain = 0.00006103515625f; break;
		case 13: gain = 0.0001220703125f; break;
		case 12: gain = 0.000244140625f; break;
		case 11: gain = 0.00048828125f; break;
		case 10: gain = 0.0009765625f; break;
		case 9: gain = 0.001953125f; break;
		case 8: gain = 0.00390625f; break;
		case 7: gain = 0.0078125f; break;
		case 6: gain = 0.015625f; break;
		case 5: gain = 0.03125f; break;
		case 4: gain = 0.0625f; break;
		case 3: gain = 0.125f; break;
		case 2: gain = 0.25f; break;
		case 1: gain = 0.5f; break;
		case 0: gain = 1.0f; break;
		case -1: gain = 2.0f; break;
		case -2: gain = 4.0f; break;
	} //now we have our input trim
	
	int bitshiftL = 1;
	int bitshiftR = 1;
	float panControl = (GetParameter( kParam_Two )*2.0f)-1.0f; //-1.0f to 1.0f
	float panAttenuation = (1.0f-fabs(panControl));
	int panBits = 20; //start centered
	if (panAttenuation > 0.0f) panBits = floor(1.0f / panAttenuation);
	if (panControl > 0.25f) bitshiftL += panBits;
	if (panControl < -0.25f) bitshiftR += panBits;
	if (bitshiftL < -2) bitshiftL = -2; if (bitshiftL > 17) bitshiftL = 17;
	if (bitshiftR < -2) bitshiftR = -2; if (bitshiftR > 17) bitshiftR = 17;
	float negacurve = 0.5f;
	float curve = 0.5f;
	switch (bitshiftL)
	{
		case 17: negacurve = 0.0f; break;
		case 16: negacurve = 0.0000152587890625f; break;
		case 15: negacurve = 0.000030517578125f; break;
		case 14: negacurve = 0.00006103515625f; break;
		case 13: negacurve = 0.0001220703125f; break;
		case 12: negacurve = 0.000244140625f; break;
		case 11: negacurve = 0.00048828125f; break;
		case 10: negacurve = 0.0009765625f; break;
		case 9: negacurve = 0.001953125f; break;
		case 8: negacurve = 0.00390625f; break;
		case 7: negacurve = 0.0078125f; break;
		case 6: negacurve = 0.015625f; break;
		case 5: negacurve = 0.03125f; break;
		case 4: negacurve = 0.0625f; break;
		case 3: negacurve = 0.125f; break;
		case 2: negacurve = 0.25f; break;
		case 1: negacurve = 0.5f; break;
		case 0: negacurve = 1.0f; break;
		case -1: negacurve = 2.0f; break;
		case -2: negacurve = 4.0f; break;
	}
	switch (bitshiftR)
	{
		case 17: curve = 0.0f; break;
		case 16: curve = 0.0000152587890625f; break;
		case 15: curve = 0.000030517578125f; break;
		case 14: curve = 0.00006103515625f; break;
		case 13: curve = 0.0001220703125f; break;
		case 12: curve = 0.000244140625f; break;
		case 11: curve = 0.00048828125f; break;
		case 10: curve = 0.0009765625f; break;
		case 9: curve = 0.001953125f; break;
		case 8: curve = 0.00390625f; break;
		case 7: curve = 0.0078125f; break;
		case 6: curve = 0.015625f; break;
		case 5: curve = 0.03125f; break;
		case 4: curve = 0.0625f; break;
		case 3: curve = 0.125f; break;
		case 2: curve = 0.25f; break;
		case 1: curve = 0.5f; break;
		case 0: curve = 1.0f; break;
		case -1: curve = 2.0f; break;
		case -2: curve = 4.0f; break;
	}
	float effectOut = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		
		inputSample *= gain; //input trim
		float inflamerPlus = (inputSample*2.0f)-powf(inputSample,2); //+50, very much just second harmonic
		float inflamerMinus = inputSample+(powf(inputSample,3)*0.25f)-((powf(inputSample,2)+powf(inputSample,4))*0.0625f); //-50
		inputSample = (inflamerPlus * curve) + (inflamerMinus * negacurve);
		inputSample = (inputSample * effectOut) + (drySample * (1.0f-effectOut));
		
		
		
		
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
