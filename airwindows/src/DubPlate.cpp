#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DubPlate"
#define AIRWINDOWS_DESCRIPTION "Ruthless dubplate mastering for electronic music."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','e' )
enum {

	kNumberOfParameters=0
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
};
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"

	float iirA;
	float iirB; //first stage is the flipping one, for lowest slope. It is always engaged, and is the highest one
	float iirC; //we introduce the second pole at the same frequency, to become a pseudo-Capacitor behavior
	float iirD; //then there's a Mid highpass.
	bool fpFlip;
	float lastSinewAL;
	float lastSinewAR;
	float lastSinewBL;
	float lastSinewBR;
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
	
	float rangescale = 0.1f / overallscale;
	float iirSide = 0.287496f * rangescale;
	float iirMid = 0.20123f * rangescale;
	float threshSinewA = 0.15f / overallscale;
	float threshSinewB = 0.127f / overallscale;
		
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		float mid = inputSampleL + inputSampleR;
		float side = inputSampleL - inputSampleR;
		//assign mid and side.Between these sections, you can do mid/side processing
		float temp = side;
		float correction;
		
		if (fpFlip) {
			iirA = (iirA * (1.0f - iirSide)) + (temp * iirSide); temp -= iirA; correction = iirA;
		} else {
			iirB = (iirB * (1.0f - iirSide)) + (temp * iirSide); temp -= iirB; correction = iirB;
		}
		iirC = (iirC * (1.0f - iirSide)) + (temp * iirSide); temp -= iirC; correction += (iirC * 0.162f);

		side -= sin(correction);
		fpFlip = !fpFlip;
		
		iirD = (iirD * (1.0f - iirMid)) + (mid * iirMid);
		mid -= sin(iirD);
		//gonna cut those lows a hair
				
		inputSampleL = (mid+side)/2.0f;
		inputSampleR = (mid-side)/2.0f;
		//unassign mid and side
		
		temp = inputSampleL;
		float sinew = threshSinewA * cos(lastSinewAL*lastSinewAL);
		if (inputSampleL - lastSinewAL > sinew) temp = lastSinewAL + sinew;
		if (-(inputSampleL - lastSinewAL) > sinew) temp = lastSinewAL - sinew;
		lastSinewAL = temp;
		if (lastSinewAL > 1.0f) lastSinewAL = 1.0f;
		if (lastSinewAL < -1.0f) lastSinewAL = -1.0f;
		inputSampleL = (inputSampleL * 0.5f)+(lastSinewAL * 0.5f);
		
		sinew = threshSinewB * cos(lastSinewBL*lastSinewBL);
		if (inputSampleL - lastSinewBL > sinew) temp = lastSinewBL + sinew;
		if (-(inputSampleL - lastSinewBL) > sinew) temp = lastSinewBL - sinew;
		lastSinewBL = temp;
		if (lastSinewBL > 1.0f) lastSinewBL = 1.0f;
		if (lastSinewBL < -1.0f) lastSinewBL = -1.0f;
		inputSampleL = (inputSampleL * 0.414f)+(lastSinewBL * 0.586f);
				
		temp = inputSampleR;
		sinew = threshSinewA * cos(lastSinewAR*lastSinewAR);
		if (inputSampleR - lastSinewAR > sinew) temp = lastSinewAR + sinew;
		if (-(inputSampleR - lastSinewAR) > sinew) temp = lastSinewAR - sinew;
		lastSinewAR = temp;
		if (lastSinewAR > 1.0f) lastSinewAR = 1.0f;
		if (lastSinewAR < -1.0f) lastSinewAR = -1.0f;
		inputSampleR = (inputSampleR * 0.5f)+(lastSinewAR * 0.5f);
				
		sinew = threshSinewB * cos(lastSinewBR*lastSinewBR);
		if (inputSampleR - lastSinewBR > sinew) temp = lastSinewBR + sinew;
		if (-(inputSampleR - lastSinewBR) > sinew) temp = lastSinewBR - sinew;
		lastSinewBR = temp;
		if (lastSinewBR > 1.0f) lastSinewBR = 1.0f;
		if (lastSinewBR < -1.0f) lastSinewBR = -1.0f;
		inputSampleR = (inputSampleR * 0.414f)+(lastSinewBR * 0.586f);
		//run Sinew to stop excess slews, two layers to make it more audible
		
		
		
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
	iirA = 0.0;
	iirB = 0.0;
	iirC = 0.0;
	iirD = 0.0;
	fpFlip = true;
	lastSinewAL = 0.0;
	lastSinewAR = 0.0;
	lastSinewBL = 0.0;
	lastSinewBR = 0.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
