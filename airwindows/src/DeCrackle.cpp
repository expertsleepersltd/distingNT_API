#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DeCrackle"
#define AIRWINDOWS_DESCRIPTION "Isolates clicks and vinyl crackles to remove them."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','D' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
const int kshort = 1600;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Filter", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Window", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Thresld", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Surface", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"

	
	int count;
	float iirTargetL;
	float iirTargetR;
	float iirClickL;
	float iirClickR;
	float prevSampleL;
	float prevSampleR;
	float prevSurfaceL;
	float prevSurfaceR;
	float prevOutL;
	float prevOutR;
		
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float aAL[kshort+5];
	float aBL[kshort+5];
	float aAR[kshort+5];
	float aBR[kshort+5];
	float aCL[kshort+5];
	float iirLSample[18];
	float iirRSample[18];
	float iirLAngle[18];
	float iirRAngle[18];
	float iirCSample[18];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	int offset = (int)(overallscale*1.1f);
	float maxHeight = 1.0f*overallscale;
	float filterOut = powf((GetParameter( kParam_A )*0.618f)+0.1f,2.0f)/overallscale;
	float filterRef = powf(((1.0f-GetParameter( kParam_B ))*0.618f)+0.1f,2.0f)/overallscale;
	float iirCut = (powf(1.0f-GetParameter( kParam_B ),2.0f)*0.2f)/overallscale;
	int adjDelay = (int)(GetParameter( kParam_B )*((float)(kshort/8)*overallscale))-2.0f;
	if (adjDelay > kshort - 1) adjDelay = kshort - 1; //don't break on silly samplerates
	int halfTrig = fmin(0.5f+powf(GetParameter( kParam_B ),3.0f),0.999f)*adjDelay;
	int halfRaw = 0.5f*adjDelay;
	int halfBez = 0.5f*adjDelay;
	float threshold = powf(GetParameter( kParam_C )*0.618f,2.0f)-0.1f;
	float surface = (1.0f-powf(1.0f-GetParameter( kParam_D ),3.0f))*0.9f;
	float wet = GetParameter( kParam_E );
	int options = 0; if (wet < 0.001f) options = 1;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		inputSampleL -= prevSampleL; prevSampleL = drySampleL;
		inputSampleR -= prevSampleR; prevSampleR = drySampleR;
		inputSampleL *= 16.0f; inputSampleR *= 16.0f;
		
		float bezL = drySampleL;
		for(int x = 0; x < 6; x++) {
			dram->iirLAngle[x] = (dram->iirLAngle[x]*(1.0f-filterOut))+((bezL-dram->iirLSample[x])*filterOut);
			bezL = ((dram->iirLSample[x]+(dram->iirLAngle[x]*filterOut))*(1.0f-filterOut))+(bezL*filterOut);
			dram->iirLSample[x] = ((dram->iirLSample[x]+(dram->iirLAngle[x]*filterOut))*(1.0f-filterOut))+(bezL*filterOut);
		} //lowpass audio band
		float bezR = drySampleR;
		for(int x = 0; x < 6; x++) {
			dram->iirRAngle[x] = (dram->iirRAngle[x]*(1.0f-filterOut))+((bezR-dram->iirRSample[x])*filterOut);
			bezR = ((dram->iirRSample[x]+(dram->iirRAngle[x]*filterOut))*(1.0f-filterOut))+(bezR*filterOut);
			dram->iirRSample[x] = ((dram->iirRSample[x]+(dram->iirRAngle[x]*filterOut))*(1.0f-filterOut))+(bezR*filterOut);
		} //lowpass audio band
		
		float rect = fabs(drySampleL*drySampleR*64.0f);
		for(int x = 0; x < 6; x++) {
			rect = fabs((dram->iirCSample[x]*(1.0f-filterRef))+(rect*filterRef));
			dram->iirCSample[x] = (dram->iirCSample[x]*(1.0f-filterRef))+(rect*filterRef);
		} //lowpass rectified CV band
		
		dram->aAL[count] = drySampleL;
		dram->aAR[count] = drySampleR;
		dram->aBL[count] = bezL;
		dram->aBR[count] = bezR;
		dram->aCL[count] = rect;
		
		count++; if (count < 0 || count > adjDelay) count = 0;
		float near = rect;
		float far = dram->aCL[count-((count > adjDelay)?adjDelay+1:0)];
		//near and far are the smoothed rectified version
		
		float prevL = dram->aAL[(count+halfRaw+offset)-(((count+halfRaw+offset) > adjDelay)?adjDelay+1:0)];
		float prevR = dram->aAR[(count+halfRaw+offset)-(((count+halfRaw+offset) > adjDelay)?adjDelay+1:0)];
		float outL = dram->aAL[(count+halfRaw)-(((count+halfRaw) > adjDelay)?adjDelay+1:0)];
		float outR = dram->aAR[(count+halfRaw)-(((count+halfRaw) > adjDelay)?adjDelay+1:0)];
		float outBezL = dram->aBL[(count+halfBez)-(((count+halfBez) > adjDelay)?adjDelay+1:0)];
		float outBezR = dram->aBR[(count+halfBez)-(((count+halfBez) > adjDelay)?adjDelay+1:0)];
		//these are the various outputs to fade between
		float trigL = dram->aAL[(count+halfTrig)-(((count+halfTrig) > adjDelay)?adjDelay+1:0)];
		float trigR = dram->aAR[(count+halfTrig)-(((count+halfTrig) > adjDelay)?adjDelay+1:0)];
		
		float deClickL = powf(fmax((fabs(trigL)-threshold)-fmax(near,far),0.0f)*16.0f,3.0f);
		float deClickR = powf(fmax((fabs(trigR)-threshold)-fmax(near,far),0.0f)*16.0f,3.0f);
		iirTargetL = fmax(iirTargetL-iirCut,0.0f);
		iirTargetR = fmax(iirTargetR-iirCut,0.0f); //taper down at iirCut speed
		if (deClickL > iirTargetL) iirTargetL = fmin(deClickL,maxHeight);
		if (deClickR > iirTargetR) iirTargetR = fmin(deClickR,maxHeight); //beyond 1.0f to stretch window
		if (deClickR*0.618f > iirTargetL) iirTargetL = fmin(deClickR*0.618f,maxHeight);
		if (deClickL*0.618f > iirTargetR) iirTargetR = fmin(deClickL*0.618f,maxHeight); //opposite channel
		iirClickL = fmin(iirClickL+iirCut,iirTargetL);
		iirClickR = fmin(iirClickR+iirCut,iirTargetR); //taper up on attack
		inputSampleL = (outBezL*fmin(iirClickL, 1.0f)) + (outL*(1.0f-fmin(iirClickL, 1.0f)));
		inputSampleR = (outBezR*fmin(iirClickR, 1.0f)) + (outR*(1.0f-fmin(iirClickR, 1.0f)));
		//declicking is applied: crossfade between darkened and normal
		
		if (wet < 1.0f && wet > 0.0f) {
			inputSampleL = (inputSampleL*wet)+(outL*(1.0f-wet));
			inputSampleR = (inputSampleR*wet)+(outR*(1.0f-wet));
		} //neither full wet, nor the 'clicks only' special case below
		
		if (wet == 0.0f) {
			inputSampleL = outL - inputSampleL;
			inputSampleR = outR - inputSampleR;
		} //clicks only at full dry
		
		float recordVolume = fmax(fmax(near,far),fmax(prevL,prevR))+0.001f; //engage only at quietest
		float surfaceL = sin(fmin((fabs(outL-prevL)/recordVolume)*surface, 3.14159265358979f))*0.5f;
		float surfaceR = sin(fmin((fabs(outR-prevR)/recordVolume)*surface, 3.14159265358979f))*0.5f;
		float gateOnAudio = fmax(surface-(recordVolume*surface*4.0f),0.0f);
		if (surface > 0.0f && wet > 0.0f) {
			inputSampleL = (prevOutL*surfaceL)+(inputSampleL*(1.0f-surfaceL));
			inputSampleR = (prevOutR*surfaceR)+(inputSampleR*(1.0f-surfaceR));
			inputSampleL = (prevOutL*gateOnAudio)+(inputSampleL*(1.0f-gateOnAudio));
			inputSampleR = (prevOutR*gateOnAudio)+(inputSampleR*(1.0f-gateOnAudio));
			prevOutL = (prevOutL*gateOnAudio)+(inputSampleL*(1.0f-gateOnAudio));
			prevOutR = (prevOutR*gateOnAudio)+(inputSampleR*(1.0f-gateOnAudio));
		} //if we're also adding surface noise reduction that goes here
		
		
		
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
	for(int x = 0; x < kshort+2; x++) {dram->aAL[x] = 0.0; dram->aAR[x] = 0.0;}
	for(int x = 0; x < kshort+2; x++) {dram->aBL[x] = 0.0; dram->aBR[x] = 0.0;}
	for(int x = 0; x < kshort+2; x++) {dram->aCL[x] = 0.0;}
	count = 1;
	for(int x = 0; x < 17; x++) {
		dram->iirLSample[x] = 0.0;
		dram->iirRSample[x] = 0.0;
		dram->iirCSample[x] = 0.0;
		dram->iirLAngle[x] = 0.0;
		dram->iirRAngle[x] = 0.0;
	}
	iirTargetL = 0.0;
	iirTargetR = 0.0;
	iirClickL = 0.0;
	iirClickR = 0.0;
	prevSampleL = 0.0;
	prevSampleR = 0.0;
	prevSurfaceL = 0.0;
	prevSurfaceR = 0.0;
	prevOutL = 0.0;
	prevOutR = 0.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
