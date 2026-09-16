#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BrassRider"
#define AIRWINDOWS_DESCRIPTION "One Weird Trick for drum overheads in metal!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','r','r' )
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
{ .name = "Threshold", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 

		Float32 highIIRL;
		Float32 slewIIRL;
		Float32 highIIR2L;
		Float32 slewIIR2L;
		Float32 highIIRR;
		Float32 slewIIRR;
		Float32 highIIR2R;
		Float32 slewIIR2R;
		Float32 control;
		Float32 clamp;
		Float32 lastSampleL;
		Float32 lastSlewL;
		Float32 lastSampleR;
		Float32 lastSlewR;
		int gcount;
		uint32_t fpdL;
		uint32_t fpdR;


	struct _dram {
			Float32 d[80002];
		Float32 e[80002];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	Float32 limitOut = GetParameter( kParam_One )*16;
	int offsetA = 13500;
	int offsetB = 16700;
	Float32 wet = GetParameter( kParam_Two );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		//assign working variables
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;

		inputSampleL *= limitOut;
		highIIRL = (highIIRL*0.5f);
		highIIRL += (inputSampleL*0.5f);
		inputSampleL -= highIIRL;
		highIIR2L = (highIIR2L*0.5f);
		highIIR2L += (inputSampleL*0.5f);
		inputSampleL -= highIIR2L;
		float slewSampleL = fabs(inputSampleL - lastSampleL);
		lastSampleL = inputSampleL;
		slewSampleL /= fabs(inputSampleL * lastSampleL)+0.2f;
		slewIIRL = (slewIIRL*0.5f);
		slewIIRL += (slewSampleL*0.5f);
		slewSampleL = fabs(slewSampleL - slewIIRL);
		slewIIR2L = (slewIIR2L*0.5f);
		slewIIR2L += (slewSampleL*0.5f);
		slewSampleL = fabs(slewSampleL - slewIIR2L);
		float bridgerectifier = slewSampleL;
		//there's the left channel, now to feed it to overall clamp
		
		if (bridgerectifier > 3.1415f) bridgerectifier = 0.0f;
		bridgerectifier = sin(bridgerectifier);
		if (gcount < 0 || gcount > 40000) {gcount = 40000;}
		dram->d[gcount+40000] = dram->d[gcount] = bridgerectifier;
		control += (dram->d[gcount] / (offsetA+1));
		control -= (dram->d[gcount+offsetA] / offsetA);
		Float32 ramp = (control*control) * 16.0f;
		dram->e[gcount+40000] = dram->e[gcount] = ramp;
		clamp += (dram->e[gcount] / (offsetB+1));
		clamp -= (dram->e[gcount+offsetB] / offsetB);
		if (clamp > wet*8) clamp = wet*8;
		gcount--;

		inputSampleR *= limitOut;
		highIIRR = (highIIRR*0.5f);
		highIIRR += (inputSampleR*0.5f);
		inputSampleR -= highIIRR;
		highIIR2R = (highIIR2R*0.5f);
		highIIR2R += (inputSampleR*0.5f);
		inputSampleR -= highIIR2R;
		float slewSampleR = fabs(inputSampleR - lastSampleR);
		lastSampleR = inputSampleR;
		slewSampleR /= fabs(inputSampleR * lastSampleR)+0.2f;
		slewIIRR = (slewIIRR*0.5f);
		slewIIRR += (slewSampleR*0.5f);
		slewSampleR = fabs(slewSampleR - slewIIRR);
		slewIIR2R = (slewIIR2R*0.5f);
		slewIIR2R += (slewSampleR*0.5f);
		slewSampleR = fabs(slewSampleR - slewIIR2R);
		bridgerectifier = slewSampleR;
		//there's the right channel, now to feed it to overall clamp
		
		if (bridgerectifier > 3.1415f) bridgerectifier = 0.0f;
		bridgerectifier = sin(bridgerectifier);
		if (gcount < 0 || gcount > 40000) {gcount = 40000;}
		dram->d[gcount+40000] = dram->d[gcount] = bridgerectifier;
		control += (dram->d[gcount] / (offsetA+1));
		control -= (dram->d[gcount+offsetA] / offsetA);
		ramp = (control*control) * 16.0f;
		dram->e[gcount+40000] = dram->e[gcount] = ramp;
		clamp += (dram->e[gcount] / (offsetB+1));
		clamp -= (dram->e[gcount+offsetB] / offsetB);
		if (clamp > wet*8) clamp = wet*8;
		gcount--;

		inputSampleL = (drySampleL * (1.0f-wet)) + (drySampleL * clamp * wet * 16.0f);
		inputSampleR = (drySampleR * (1.0f-wet)) + (drySampleR * clamp * wet * 16.0f);

		
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
};
int _airwindowsAlgorithm::reset(void) {

{
	for(int count = 0; count < 80001; count++) {dram->d[count] = 0.0; dram->e[count] = 0.0;}
	control = 0.0;
	clamp = 0.0;
	highIIRL = 0.0;
	slewIIRL = 0.0;
	highIIR2L = 0.0;
	slewIIR2L = 0.0;
	lastSampleL = 0.0;
	lastSlewL = 0.0;
	highIIRR = 0.0;
	slewIIRR = 0.0;
	highIIR2R = 0.0;
	slewIIR2R = 0.0;
	lastSampleR = 0.0;
	lastSlewR = 0.0;
	gcount = 0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;

	return noErr;
}


};
