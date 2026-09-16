#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BeziComp"
#define AIRWINDOWS_DESCRIPTION "A radical compressor that eliminates aliasing!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','e','p' )
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
{ .name = "Comp", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	enum {
		bez_AL,
		bez_BL,
		bez_CL,
		bez_InL,
		bez_UnInL,
		bez_SampL,
		bez_cycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	
	float lastSampleL;
	float intermediateL[16];
	bool wasPosClipL;
	bool wasNegClipL;
	float lastSampleR;
	float intermediateR[16];
	bool wasPosClipR;
	bool wasNegClipR; //Stereo ClipOnly2
	
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
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;
	
	float bezCThresh = powf(GetParameter( kParam_A ),2.0f) * 64.0f;
	float bezMakeUp = sqrt(bezCThresh+1.0f);
	float bezRez = (powf(GetParameter( kParam_B ),6.0f)+0.0001f)/overallscale; if (bezRez > 1.0f) bezRez = 1.0f;
	float wet = GetParameter( kParam_C );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		dram->bezComp[bez_cycle] += bezRez;
		dram->bezComp[bez_SampL] += (fmax(fabs(inputSampleL),fabs(inputSampleR)) * bezRez);
		
		if (dram->bezComp[bez_cycle] > 1.0f) {
			dram->bezComp[bez_cycle] -= 1.0f;
			dram->bezComp[bez_CL] = dram->bezComp[bez_BL];
			dram->bezComp[bez_BL] = dram->bezComp[bez_AL];
			dram->bezComp[bez_AL] = dram->bezComp[bez_SampL];
			dram->bezComp[bez_SampL] = 0.0f;
		}
		float CBL = (dram->bezComp[bez_CL]*(1.0f-dram->bezComp[bez_cycle]))+(dram->bezComp[bez_BL]*dram->bezComp[bez_cycle]);
		float BAL = (dram->bezComp[bez_BL]*(1.0f-dram->bezComp[bez_cycle]))+(dram->bezComp[bez_AL]*dram->bezComp[bez_cycle]);
		float CBAL = (dram->bezComp[bez_BL]+(CBL*(1.0f-dram->bezComp[bez_cycle]))+(BAL*dram->bezComp[bez_cycle]))*0.5f;
		
		inputSampleL *= 1.0f-(fmin(CBAL*bezCThresh,1.0f));
		inputSampleL *= bezMakeUp;
		inputSampleR *= 1.0f-(fmin(CBAL*bezCThresh,1.0f));
		inputSampleR *= bezMakeUp;
		
		if (wet < 1.0f) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f-wet));
		}
		
		//begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0f) inputSampleL = 4.0f; if (inputSampleL < -4.0f) inputSampleL = -4.0f;
		if (wasPosClipL == true) { //current will be over
			if (inputSampleL<lastSampleL) lastSampleL=0.7058208f+(inputSampleL*0.2609148f);
			else lastSampleL = 0.2491717f+(lastSampleL*0.7390851f);
		} wasPosClipL = false;
		if (inputSampleL>0.9549925859f) {wasPosClipL=true;inputSampleL=0.7058208f+(lastSampleL*0.2609148f);}
		if (wasNegClipL == true) { //current will be -over
			if (inputSampleL > lastSampleL) lastSampleL=-0.7058208f+(inputSampleL*0.2609148f);
			else lastSampleL=-0.2491717f+(lastSampleL*0.7390851f);
		} wasNegClipL = false;
		if (inputSampleL<-0.9549925859f) {wasNegClipL=true;inputSampleL=-0.7058208f+(lastSampleL*0.2609148f);}
		intermediateL[spacing] = inputSampleL;
        inputSampleL = lastSampleL; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediateL[x-1] = intermediateL[x];
		lastSampleL = intermediateL[0]; //run a little buffer to handle this
		
		if (inputSampleR > 4.0f) inputSampleR = 4.0f; if (inputSampleR < -4.0f) inputSampleR = -4.0f;
		if (wasPosClipR == true) { //current will be over
		if (inputSampleR<lastSampleR) lastSampleR=0.7058208f+(inputSampleR*0.2609148f);
		else lastSampleR = 0.2491717f+(lastSampleR*0.7390851f);
		} wasPosClipR = false;
		if (inputSampleR>0.9549925859f) {wasPosClipR=true;inputSampleR=0.7058208f+(lastSampleR*0.2609148f);}
		if (wasNegClipR == true) { //current will be -over
		if (inputSampleR > lastSampleR) lastSampleR=-0.7058208f+(inputSampleR*0.2609148f);
		else lastSampleR=-0.2491717f+(lastSampleR*0.7390851f);
		} wasNegClipR = false;
		if (inputSampleR<-0.9549925859f) {wasNegClipR=true;inputSampleR=-0.7058208f+(lastSampleR*0.2609148f);}
		intermediateR[spacing] = inputSampleR;
		inputSampleR = lastSampleR; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediateR[x-1] = intermediateR[x];
		lastSampleR = intermediateR[0]; //run a little buffer to handle this
		//end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code

		
		
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
	for (int x = 0; x < bez_total; x++) {dram->bezComp[x] = 0.0;}
	dram->bezComp[bez_cycle] = 1.0;
	
	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	lastSampleR = 0.0;
	wasPosClipR = false;
	wasNegClipR = false;
	for (int x = 0; x < 16; x++) {intermediateL[x] = 0.0; intermediateR[x] = 0.0;}
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
