#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dynamics2"
#define AIRWINDOWS_DESCRIPTION "The compressor gate for new Console plugins!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','y','o' )
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
{ .name = "Attack", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gate", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	enum {
		bez_AL,
		bez_BL,
		bez_CL,
		bez_InL,
		bez_UnInL,
		bez_SampL,
		bez_AR,
		bez_BR,
		bez_CR,
		bez_InR,
		bez_UnInR,
		bez_SampR,
		bez_cycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	float bezMaxF;
	float bezGate;
		
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float bezCompF[bez_total];
	float bezCompS[bez_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	float bezCThresh = powf(1.0f-GetParameter( kParam_A ), 6.0f) * 8.0f;
	float bezRez = powf(1.0f-GetParameter( kParam_B ), 8.0f) / overallscale; 
	float sloRez = powf(1.0f-GetParameter( kParam_C ),12.0f) / overallscale;
	sloRez = fmin(fmax(sloRez-(bezRez*0.5f),0.00001f),1.0f);
	bezRez = fmin(fmax(bezRez,0.0001f),1.0f);
	float gate = powf(powf(GetParameter( kParam_D ),4.0f),sqrt(bezCThresh+1.0f));
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		if (fmax(fabs(inputSampleL),fabs(inputSampleR)) > gate+(sloRez*bezGate)) bezGate = ((bezGate*overallscale*3.0f)+3.0f)*(0.25f/overallscale);
		else bezGate = fmax(0.0f, bezGate-(sloRez*sloRez));
		
		if (bezCThresh > 0.0f) {
			inputSampleL *= ((bezCThresh*0.5f)+1.0f);
			inputSampleR *= ((bezCThresh*0.5f)+1.0f);
		}
		
		dram->bezCompF[bez_cycle] += bezRez;
		dram->bezCompF[bez_SampL] += (fabs(inputSampleL) * bezRez);
		dram->bezCompF[bez_SampR] += (fabs(inputSampleR) * bezRez);
		bezMaxF = fmax(bezMaxF,fmax(fabs(inputSampleL),fabs(inputSampleR)));
		
		if (dram->bezCompF[bez_cycle] > 1.0f) {
			dram->bezCompF[bez_cycle] -= 1.0f;
			
			if (bezMaxF < gate) dram->bezCompF[bez_SampL] = bezMaxF/gate; //note: SampL is a control voltage,
			if (dram->bezCompF[bez_SampL]<gate) dram->bezCompF[bez_SampL] = 0.0f; //not a bipolar audio signal
			dram->bezCompF[bez_CL] = dram->bezCompF[bez_BL];
			dram->bezCompF[bez_BL] = dram->bezCompF[bez_AL];
			dram->bezCompF[bez_AL] = dram->bezCompF[bez_SampL];
			dram->bezCompF[bez_SampL] = 0.0f;
			
			if (bezMaxF < gate) dram->bezCompF[bez_SampR] = bezMaxF/gate; //note: SampR is a control voltage,
			if (dram->bezCompF[bez_SampR]<gate) dram->bezCompF[bez_SampR] = 0.0f; //not a bipolar audio signal
			dram->bezCompF[bez_CR] = dram->bezCompF[bez_BR];
			dram->bezCompF[bez_BR] = dram->bezCompF[bez_AR];
			dram->bezCompF[bez_AR] = dram->bezCompF[bez_SampR];
			dram->bezCompF[bez_SampR] = 0.0f;
			
			bezMaxF = 0.0f;
		}
		dram->bezCompS[bez_cycle] += sloRez;
		dram->bezCompS[bez_SampL] += (fabs(inputSampleL) * sloRez); //note: SampL is a control voltage
		dram->bezCompS[bez_SampR] += (fabs(inputSampleR) * sloRez); //note: SampR is a control voltage
		if (dram->bezCompS[bez_cycle] > 1.0f) {
			dram->bezCompS[bez_cycle] -= 1.0f;
			
			if (dram->bezCompS[bez_SampL]<gate) dram->bezCompS[bez_SampL] = 0.0f;
			dram->bezCompS[bez_CL] = dram->bezCompS[bez_BL];
			dram->bezCompS[bez_BL] = dram->bezCompS[bez_AL];
			dram->bezCompS[bez_AL] = dram->bezCompS[bez_SampL];
			dram->bezCompS[bez_SampL] = 0.0f;
			
			if (dram->bezCompS[bez_SampR]<gate) dram->bezCompS[bez_SampR] = 0.0f;
			dram->bezCompS[bez_CR] = dram->bezCompS[bez_BR];
			dram->bezCompS[bez_BR] = dram->bezCompS[bez_AR];
			dram->bezCompS[bez_AR] = dram->bezCompS[bez_SampR];
			dram->bezCompS[bez_SampR] = 0.0f;
		}
		float CBFL = (dram->bezCompF[bez_CL]*(1.0f-dram->bezCompF[bez_cycle]))+(dram->bezCompF[bez_BL]*dram->bezCompF[bez_cycle]);
		float BAFL = (dram->bezCompF[bez_BL]*(1.0f-dram->bezCompF[bez_cycle]))+(dram->bezCompF[bez_AL]*dram->bezCompF[bez_cycle]);
		float CBAFL = (dram->bezCompF[bez_BL]+(CBFL*(1.0f-dram->bezCompF[bez_cycle]))+(BAFL*dram->bezCompF[bez_cycle]))*0.5f;
		float CBSL = (dram->bezCompS[bez_CL]*(1.0f-dram->bezCompS[bez_cycle]))+(dram->bezCompS[bez_BL]*dram->bezCompS[bez_cycle]);
		float BASL = (dram->bezCompS[bez_BL]*(1.0f-dram->bezCompS[bez_cycle]))+(dram->bezCompS[bez_AL]*dram->bezCompS[bez_cycle]);
		float CBASL = (dram->bezCompS[bez_BL]+(CBSL*(1.0f-dram->bezCompS[bez_cycle]))+(BASL*dram->bezCompS[bez_cycle]))*0.5f;
		float CBAMax = fmax(CBASL,CBAFL); if (CBAMax > 0.0f) CBAMax = 1.0f/CBAMax;
		float CBAFade = ((CBASL*-CBAMax)+(CBAFL*CBAMax)+1.0f)*0.5f;
		if (bezCThresh > 0.0f) inputSampleL *= 1.0f-(fmin(((CBASL*(1.0f-CBAFade))+(CBAFL*CBAFade))*bezCThresh,1.0f));
		
		float CBFR = (dram->bezCompF[bez_CR]*(1.0f-dram->bezCompF[bez_cycle]))+(dram->bezCompF[bez_BR]*dram->bezCompF[bez_cycle]);
		float BAFR = (dram->bezCompF[bez_BR]*(1.0f-dram->bezCompF[bez_cycle]))+(dram->bezCompF[bez_AR]*dram->bezCompF[bez_cycle]);
		float CBAFR = (dram->bezCompF[bez_BR]+(CBFR*(1.0f-dram->bezCompF[bez_cycle]))+(BAFR*dram->bezCompF[bez_cycle]))*0.5f;
		float CBSR = (dram->bezCompS[bez_CR]*(1.0f-dram->bezCompS[bez_cycle]))+(dram->bezCompS[bez_BR]*dram->bezCompS[bez_cycle]);
		float BASR = (dram->bezCompS[bez_BR]*(1.0f-dram->bezCompS[bez_cycle]))+(dram->bezCompS[bez_AR]*dram->bezCompS[bez_cycle]);
		float CBASR = (dram->bezCompS[bez_BR]+(CBSR*(1.0f-dram->bezCompS[bez_cycle]))+(BASR*dram->bezCompS[bez_cycle]))*0.5f;
		CBAMax = fmax(CBASR,CBAFR); if (CBAMax > 0.0f) CBAMax = 1.0f/CBAMax;
		CBAFade = ((CBASR*-CBAMax)+(CBAFR*CBAMax)+1.0f)*0.5f;
		if (bezCThresh > 0.0f) inputSampleR *= 1.0f-(fmin(((CBASR*(1.0f-CBAFade))+(CBAFR*CBAFade))*bezCThresh,1.0f));
		
		if (bezGate < 1.0f && gate > 0.0f) {inputSampleL *= bezGate; inputSampleR *= bezGate;}
				
		
		
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
	for (int x = 0; x < bez_total; x++) {dram->bezCompF[x] = 0.0;dram->bezCompS[x] = 0.0;}
	dram->bezCompF[bez_cycle] = 1.0; bezMaxF = 0.0;
	dram->bezCompS[bez_cycle] = 1.0; bezGate = 2.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
