#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dynamics3"
#define AIRWINDOWS_DESCRIPTION "Morphs between vari-mu and expander!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','y','3' )
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
{ .name = "Thresh", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 382, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 618, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 

	enum {
		bez_A,
		bez_B,
		bez_C,
		bez_Ctrl,
		bez_cycle,
		bez_min,
		bez_comp,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	//Dynamics3
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float bezComp[bez_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	//begin Dynamics3
	float bezThresh = powf(GetParameter( kParam_A )+0.6180339887498949f,2.0f)*1.6180339887498949f;
	float sqrThresh = sqrt(bezThresh);
	float bezRez = fmax(powf((1.0f-GetParameter( kParam_B ))*0.4f,4.0f)/overallscale,0.0001f); 
	bezRez /= (2.0f/powf(overallscale,0.5f-((overallscale-1.0f)*0.0375f)));
	int stepped = 999999; if (bezRez > 0.000001f) stepped = (int)(1.0f/bezRez);
	bezRez = 0.99999999f / stepped;
	float bezTrim = 1.0f-(bezRez*((float)stepped/(stepped+1.0f)));
	float sloRez = fmax(powf((1.0f-GetParameter( kParam_C ))*0.4f,4.0f)/overallscale,0.00001f);
	sloRez /= (2.0f/powf(overallscale,0.5f-((overallscale-1.0f)*0.0375f)));
	float invDry = powf(1.0f-(fabs(GetParameter( kParam_D )-0.5f)*2.0f),1.6180339887498949f);
	bool compress = (GetParameter( kParam_D ) > 0.499999f);
	bool compBypass = (bezThresh > 4.236f);
	//end Dynamics3

	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;

		if (!compBypass) {
			//begin Dynamics3
			float dryCompL = inputSampleL;
			float dryCompR = inputSampleR;
			if (compress) {
				inputSampleL *= (dram->bezComp[bez_comp]/bezThresh);
				inputSampleR *= (dram->bezComp[bez_comp]/bezThresh);
			} else {
				inputSampleL /= bezThresh;
				inputSampleR /= bezThresh;
			}
			float ctrl = fmin(fmax(fabs(inputSampleL),fabs(inputSampleR)),sqrThresh*dram->bezComp[bez_comp]*0.6180339887498949f);
			dram->bezComp[bez_min] = fmax(dram->bezComp[bez_min]-sloRez,ctrl);
			dram->bezComp[bez_Ctrl] += (dram->bezComp[bez_min] * bezRez);
			dram->bezComp[bez_cycle] += bezRez;
			if (dram->bezComp[bez_cycle] > bezTrim) {dram->bezComp[bez_cycle] = 0.0f;
				dram->bezComp[bez_C] = dram->bezComp[bez_B]; dram->bezComp[bez_B] = dram->bezComp[bez_A];
				dram->bezComp[bez_A] = dram->bezComp[bez_Ctrl]; dram->bezComp[bez_Ctrl] = 0.0f;}
			float X = dram->bezComp[bez_cycle];
			dram->bezComp[bez_comp] = dram->bezComp[bez_B]+(dram->bezComp[bez_C]*(1.0f-X)*(1.0f-X))+(dram->bezComp[bez_B]*2.0f*(1.0f-X)*X)+(dram->bezComp[bez_A]*X*X);
			dram->bezComp[bez_comp] = ((1.0f-(fmin(dram->bezComp[bez_comp],0.9999f))));
			if (compress) {
				inputSampleL = inputSampleL*(1.0f-invDry)*dram->bezComp[bez_comp]*bezThresh;
				inputSampleL = fmax(fmin(inputSampleL,2.305929007734908f),-2.305929007734908f);
				float addtwo = inputSampleL * inputSampleL;
				float empower = inputSampleL * addtwo; // inputSampleL to the third power
				inputSampleL -= (empower / 6.0f); empower *= addtwo; // to the fifth power
				inputSampleL += (empower / 69.0f); empower *= addtwo; //seventh
				inputSampleL -= (empower / 2530.08f); empower *= addtwo; //ninth
				inputSampleL += (empower / 224985.6f); empower *= addtwo; //eleventh
				inputSampleL -= (empower / 9979200.0f);				
				//this is a degenerate form of a Taylor Series to approximate sin()				
				inputSampleL += dryCompL*invDry*(1.0f-(dram->bezComp[bez_comp]*(1.0f-invDry)));
				inputSampleR = inputSampleR*(1.0f-invDry)*dram->bezComp[bez_comp]*bezThresh;
				inputSampleR = fmax(fmin(inputSampleR,2.305929007734908f),-2.305929007734908f);
				addtwo = inputSampleR * inputSampleR;
				empower = inputSampleR * addtwo; // inputSampleR to the third power
				inputSampleR -= (empower / 6.0f); empower *= addtwo; // to the fifth power
				inputSampleR += (empower / 69.0f); empower *= addtwo; //seventh
				inputSampleR -= (empower / 2530.08f); empower *= addtwo; //ninth
				inputSampleR += (empower / 224985.6f); empower *= addtwo; //eleventh
				inputSampleR -= (empower / 9979200.0f);
				//this is a degenerate form of a Taylor Series to approximate sin()
				inputSampleR += dryCompR*invDry*(1.0f-(dram->bezComp[bez_comp]*(1.0f-invDry)));
			} else {
				inputSampleL = ((inputSampleL/(0.1f+bezThresh))*(1.0f-invDry))/dram->bezComp[bez_comp];
				inputSampleL = fmax(fmin(inputSampleL,2.305929007734908f),-2.305929007734908f);
				float addtwo = inputSampleL * inputSampleL;
				float empower = inputSampleL * addtwo; // inputSampleL to the third power
				inputSampleL -= (empower / 6.0f); empower *= addtwo; // to the fifth power
				inputSampleL += (empower / 69.0f); empower *= addtwo; //seventh
				inputSampleL -= (empower / 2530.08f); empower *= addtwo; //ninth
				inputSampleL += (empower / 224985.6f); empower *= addtwo; //eleventh
				inputSampleL -= (empower / 9979200.0f);				
				//this is a degenerate form of a Taylor Series to approximate sin()				
				inputSampleL += dryCompL*invDry*(1.0f-(dram->bezComp[bez_comp]*(1.0f-invDry)));
				inputSampleR = ((inputSampleR/(0.1f+bezThresh))*(1.0f-invDry))/dram->bezComp[bez_comp];
				inputSampleR = fmax(fmin(inputSampleR,2.305929007734908f),-2.305929007734908f);
				addtwo = inputSampleR * inputSampleR;
				empower = inputSampleR * addtwo; // inputSampleR to the third power
				inputSampleR -= (empower / 6.0f); empower *= addtwo; // to the fifth power
				inputSampleR += (empower / 69.0f); empower *= addtwo; //seventh
				inputSampleR -= (empower / 2530.08f); empower *= addtwo; //ninth
				inputSampleR += (empower / 224985.6f); empower *= addtwo; //eleventh
				inputSampleR -= (empower / 9979200.0f);
				//this is a degenerate form of a Taylor Series to approximate sin()				
				inputSampleR += dryCompR*invDry*(1.0f-(dram->bezComp[bez_comp]*(1.0f-invDry)));
			}
		} //end Dynamics3
				
		
		
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
	for (int x = 0; x < bez_total; x++) dram->bezComp[x] = 0.0;
	//Dynamics3
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
