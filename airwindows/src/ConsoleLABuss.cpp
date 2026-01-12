#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleLABuss"
#define AIRWINDOWS_DESCRIPTION "The Airwindows take on the Quad Eight console."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','5' )
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Master", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float lastSinewL;
	float lastSinewR;
	
	float subAL;
	float subAR;
	float subBL;
	float subBR;
	float subCL;
	float subCR;
	
	float gainA;
	float gainB; //smoothed master fader for channel, from Z2 series filter code
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	gainA = gainB;
	gainB = sqrt(GetParameter( kParam_One )); //smoothed master fader from Z2 filters
	//this will be applied three times: this is to make the various tone alterations
	//hit differently at different master fader drive levels.
	//in particular, backing off the master fader tightens the super lows
	//but opens up the modified Sinew, because more of the attentuation happens before
	//you even get to slew clipping :) and if the fader is not active, it bypasses completely.
	
	float threshSinew = 0.718f/overallscale;
	float subTrim = 0.0011f / overallscale;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float gain = (gainA*temp)+(gainB*(1.0f-temp));
		//setting up smoothed master fader
		
		//begin SubTight section
		float subSampleL = inputSampleL * subTrim;
		float subSampleR = inputSampleR * subTrim;
		
		float scale = 0.5f+fabs(subSampleL*0.5f);
		subSampleL = (subAL+(sin(subAL-subSampleL)*scale));
		subAL = subSampleL*scale;
		scale = 0.5f+fabs(subSampleR*0.5f);
		subSampleR = (subAR+(sin(subAR-subSampleR)*scale));
		subAR = subSampleR*scale;
		scale = 0.5f+fabs(subSampleL*0.5f);
		subSampleL = (subBL+(sin(subBL-subSampleL)*scale));
		subBL = subSampleL*scale;
		scale = 0.5f+fabs(subSampleR*0.5f);
		subSampleR = (subBR+(sin(subBR-subSampleR)*scale));
		subBR = subSampleR*scale;
		scale = 0.5f+fabs(subSampleL*0.5f);
		subSampleL = (subCL+(sin(subCL-subSampleL)*scale));
		subCL = subSampleL*scale;
		scale = 0.5f+fabs(subSampleR*0.5f);
		subSampleR = (subCR+(sin(subCR-subSampleR)*scale));
		subCR = subSampleR*scale;
		if (subSampleL > 0.25f) subSampleL = 0.25f;
		if (subSampleL < -0.25f) subSampleL = -0.25f;
		if (subSampleR > 0.25f) subSampleR = 0.25f;
		if (subSampleR < -0.25f) subSampleR = -0.25f;
		inputSampleL -= (subSampleL*16.0f);
		inputSampleR -= (subSampleR*16.0f);
		//end SubTight section
		
		if (gain < 1.0f) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		} //if using the master fader, we are going to attenuate three places.
		//subtight is always fully engaged: tighten response when restraining full console
		
		//begin ConsoleZeroBuss which is the one we choose for ConsoleLA
		if (inputSampleL > 2.8f) inputSampleL = 2.8f;
		if (inputSampleL < -2.8f) inputSampleL = -2.8f;
		if (inputSampleL > 0.0f) inputSampleL = (inputSampleL*2.0f)/(3.0f-inputSampleL);
		else inputSampleL = -(inputSampleL*-2.0f)/(3.0f+inputSampleL);
		
		if (inputSampleR > 2.8f) inputSampleR = 2.8f;
		if (inputSampleR < -2.8f) inputSampleR = -2.8f;
		if (inputSampleR > 0.0f) inputSampleR = (inputSampleR*2.0f)/(3.0f-inputSampleR);
		else inputSampleR = -(inputSampleR*-2.0f)/(3.0f+inputSampleR);
		//ConsoleZero Buss
		
		if (gain < 1.0f) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		} //if using the master fader, we are going to attenuate three places.
		//after C0Buss but before EverySlew: allow highs to come out a bit more
		//when pulling back master fader. Less drive equals more open
		
		temp = inputSampleL;
		float clamp = inputSampleL - lastSinewL;
		float sinew = threshSinew * cos(lastSinewL);
		if (clamp > sinew) temp = lastSinewL + sinew;
		if (-clamp > sinew) temp = lastSinewL - sinew;
		inputSampleL = lastSinewL = temp;
		if (lastSinewL > 1.0f) lastSinewL = 1.0f;
		if (lastSinewL < -1.0f) lastSinewL = -1.0f;
		temp = inputSampleR;
		clamp = inputSampleR - lastSinewR;
		sinew = threshSinew * cos(lastSinewR);
		if (clamp > sinew) temp = lastSinewR + sinew;
		if (-clamp > sinew) temp = lastSinewR - sinew;
		inputSampleR = lastSinewR = temp;
		if (lastSinewR > 1.0f) lastSinewR = 1.0f;
		if (lastSinewR < -1.0f) lastSinewR = -1.0f;
		
		if (gain < 1.0f) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		} //if using the master fader, we are going to attenuate three places.
		//after EverySlew fades the total output sound: least change in tone here.
		
		
		
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
	lastSinewL = lastSinewR = 0.0;
	subAL = subAR = subBL = subBR = subCL = subCR = 0.0;
	gainA = gainB = 1.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
