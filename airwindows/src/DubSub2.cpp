#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DubSub2"
#define AIRWINDOWS_DESCRIPTION "The essence of the Airwindows head bump."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','3' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "HeadBmp", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HeadFrq", .min = 2500, .max = 20000, .def = 5000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	
	float headBumpL;
	float headBumpR;
	enum {
		hdb_freq,
		hdb_reso,
		hdb_a0,
		hdb_a1,
		hdb_a2,
		hdb_b1,
		hdb_b2,
		hdb_sL1,
		hdb_sL2,
		hdb_sR1,
		hdb_sR2,
		hdb_total
	}; //fixed frequency biquad filter for ultrasonics, stereo
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float hdbA[hdb_total];
	float hdbB[hdb_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();

	float headBumpDrive = (GetParameter( kParam_A )*0.1f)/overallscale;

	dram->hdbA[hdb_freq] = GetParameter( kParam_B )/GetSampleRate();
	dram->hdbB[hdb_freq] = dram->hdbA[hdb_freq]*0.9375f;
	//displayNumber = ((B*B)*175.0f)+25.0f
	dram->hdbB[hdb_reso] = dram->hdbA[hdb_reso] = 0.618033988749894848204586f;
	dram->hdbB[hdb_a1] = dram->hdbA[hdb_a1] = 0.0f;
	
	float K = tan(M_PI * dram->hdbA[hdb_freq]);
	float norm = 1.0f / (1.0f + K / dram->hdbA[hdb_reso] + K * K);
	dram->hdbA[hdb_a0] = K / dram->hdbA[hdb_reso] * norm;
	dram->hdbA[hdb_a2] = -dram->hdbA[hdb_a0];
	dram->hdbA[hdb_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hdbA[hdb_b2] = (1.0f - K / dram->hdbA[hdb_reso] + K * K) * norm;
	K = tan(M_PI * dram->hdbB[hdb_freq]);
	norm = 1.0f / (1.0f + K / dram->hdbB[hdb_reso] + K * K);
	dram->hdbB[hdb_a0] = K / dram->hdbB[hdb_reso] * norm;
	dram->hdbB[hdb_a2] = -dram->hdbB[hdb_a0];
	dram->hdbB[hdb_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hdbB[hdb_b2] = (1.0f - K / dram->hdbB[hdb_reso] + K * K) * norm;
	
	float headWet = GetParameter( kParam_C );	
		
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
				
		//begin HeadBump
		headBumpL += (inputSampleL * headBumpDrive);
		headBumpL -= (headBumpL * headBumpL * headBumpL * (0.0618f/sqrt(overallscale)));
		headBumpR += (inputSampleR * headBumpDrive);
		headBumpR -= (headBumpR * headBumpR * headBumpR * (0.0618f/sqrt(overallscale)));
		float headBiqSampleL = (headBumpL * dram->hdbA[hdb_a0]) + dram->hdbA[hdb_sL1];
		dram->hdbA[hdb_sL1] = (headBumpL * dram->hdbA[hdb_a1]) - (headBiqSampleL * dram->hdbA[hdb_b1]) + dram->hdbA[hdb_sL2];
		dram->hdbA[hdb_sL2] = (headBumpL * dram->hdbA[hdb_a2]) - (headBiqSampleL * dram->hdbA[hdb_b2]);
		float headBumpSampleL = (headBiqSampleL * dram->hdbB[hdb_a0]) + dram->hdbB[hdb_sL1];
		dram->hdbB[hdb_sL1] = (headBiqSampleL * dram->hdbB[hdb_a1]) - (headBumpSampleL * dram->hdbB[hdb_b1]) + dram->hdbB[hdb_sL2];
		dram->hdbB[hdb_sL2] = (headBiqSampleL * dram->hdbB[hdb_a2]) - (headBumpSampleL * dram->hdbB[hdb_b2]);
		float headBiqSampleR = (headBumpR * dram->hdbA[hdb_a0]) + dram->hdbA[hdb_sR1];
		dram->hdbA[hdb_sR1] = (headBumpR * dram->hdbA[hdb_a1]) - (headBiqSampleR * dram->hdbA[hdb_b1]) + dram->hdbA[hdb_sR2];
		dram->hdbA[hdb_sR2] = (headBumpR * dram->hdbA[hdb_a2]) - (headBiqSampleR * dram->hdbA[hdb_b2]);
		float headBumpSampleR = (headBiqSampleR * dram->hdbB[hdb_a0]) + dram->hdbB[hdb_sR1];
		dram->hdbB[hdb_sR1] = (headBiqSampleR * dram->hdbB[hdb_a1]) - (headBumpSampleR * dram->hdbB[hdb_b1]) + dram->hdbB[hdb_sR2];
		dram->hdbB[hdb_sR2] = (headBiqSampleR * dram->hdbB[hdb_a2]) - (headBumpSampleR * dram->hdbB[hdb_b2]);
		//end HeadBump
		
		inputSampleL = (headBumpSampleL * headWet) + (drySampleL * (1.0f-headWet));
		inputSampleR = (headBumpSampleR * headWet) + (drySampleR * (1.0f-headWet));

		
		
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
	headBumpL = 0.0;
	headBumpR = 0.0;
	for (int x = 0; x < hdb_total; x++) {dram->hdbA[x] = 0.0;dram->hdbB[x] = 0.0;}
	//from ZBandpass, so I can use enums with it
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
