#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "TexturizeMS"
#define AIRWINDOWS_DESCRIPTION "A hidden-noise plugin for adding mid-side sonic texture to things."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','e','S' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "BrightM", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "PunchyM", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DryWetM", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "BrightS", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "PunchyS", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DryWetS", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	bool polarityM;
	float lastSampleM;
	float iirSampleM;
	float noiseAM;
	float noiseBM;
	float noiseCM;
	bool flipM;
	
	bool polarityS;
	float lastSampleS;
	float iirSampleS;
	float noiseAS;
	float noiseBS;
	float noiseCS;
	bool flipS;
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
	
	float slewAmountM = ((powf(GetParameter( kParam_One ),2.0f)*4.0f)+0.71f)/overallscale;
	float dynAmountM = powf(GetParameter( kParam_Two ),2.0f);
	float wetM = powf(GetParameter( kParam_Three ),5);
	
	float slewAmountS = ((powf(GetParameter( kParam_Four ),2.0f)*4.0f)+0.71f)/overallscale;
	float dynAmountS = powf(GetParameter( kParam_Five ),2.0f);
	float wetS = powf(GetParameter( kParam_Six ),5);
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		float mid = inputSampleL + inputSampleR;
		float side = inputSampleL - inputSampleR;
		//assign mid and side.Between these sections, you can do mid/side processing
		float drySampleM = mid;
		float drySampleS = side;
		
		//begin mid
		if (mid < 0) {
			if (polarityM == true) {
				if (noiseAM < 0) flipM = true;
				else flipM = false;
			}
			polarityM = false;
		} else polarityM = true;		
		
		if (flipM) noiseAM += (float(fpdL)/UINT32_MAX);
		else noiseAM -= (float(fpdL)/UINT32_MAX);
		//here's the guts of the random walk		
		flipM = !flipM;
		
		if (mid > 1.0f) mid = 1.0f; if (mid < -1.0f) mid = -1.0f;
		if (dynAmountM < 0.4999f) mid = (mid*dynAmountM*2.0f) + (sin(mid)*(1.0f-(dynAmountM*2.0f)));		
		if (dynAmountM > 0.5001f) mid = (asin(mid)*((dynAmountM*2.0f)-1.0f)) + (mid*(1.0f-((dynAmountM*2.0f)-1.0f)));
		//doing this in two steps means I get to not run an extra sin/asin function per sample
		
		noiseBM = sin(noiseAM*(0.2f-(dynAmountM*0.125f))*fabs(mid));
		
		float slew = fabs(mid-lastSampleM)*slewAmountM;
		lastSampleM = mid*(0.86f-(dynAmountM*0.125f));
		
		if (slew > 1.0f) slew = 1.0f;
		float iirIntensity = slew;
		iirIntensity *= 2.472f;
		iirIntensity *= iirIntensity;
		if (iirIntensity > 1.0f) iirIntensity = 1.0f;
		
		iirSampleM = (iirSampleM * (1.0f - iirIntensity)) + (noiseBM * iirIntensity);
		noiseBM = iirSampleM;
		noiseBM = (noiseBM * slew) + (noiseCM * (1.0f-slew));
		noiseCM = noiseBM;
		
		mid = (noiseCM * wetM) + (drySampleM * (1.0f-wetM));
		//end mid

		//begin side
		if (side < 0) {
			if (polarityS == true) {
				if (noiseAS < 0) flipS = true;
				else flipS = false;
			}
			polarityS = false;
		} else polarityS = true;		
		
		if (flipS) noiseAS += (float(fpdR)/UINT32_MAX);
		else noiseAS -= (float(fpdR)/UINT32_MAX);
		//here's the guts of the random walk		
		flipS = !flipS;
		
		if (side > 1.0f) side = 1.0f; if (side < -1.0f) side = -1.0f;
		if (dynAmountS < 0.4999f) side = (side*dynAmountS*2.0f) + (sin(side)*(1.0f-(dynAmountS*2.0f)));		
		if (dynAmountS > 0.5001f) side = (asin(side)*((dynAmountS*2.0f)-1.0f)) + (side*(1.0f-((dynAmountS*2.0f)-1.0f)));
		//doing this in two steps means I get to not run an extra sin/asin function per sample
		
		noiseBS = sin(noiseAS*(0.2f-(dynAmountS*0.125f))*fabs(side));
		
		slew = fabs(side-lastSampleS)*slewAmountS;
		lastSampleS = side*(0.86f-(dynAmountS*0.125f));
		
		if (slew > 1.0f) slew = 1.0f;
		iirIntensity = slew;
		iirIntensity *= 2.472f;
		iirIntensity *= iirIntensity;
		if (iirIntensity > 1.0f) iirIntensity = 1.0f;
		
		iirSampleS = (iirSampleS * (1.0f - iirIntensity)) + (noiseBS * iirIntensity);
		noiseBS = iirSampleS;
		noiseBS = (noiseBS * slew) + (noiseCS * (1.0f-slew));
		noiseCS = noiseBS;
		
		side = (noiseCS * wetS) + (drySampleS * (1.0f-wetS));
		//end side
		
		inputSampleL = (mid+side)/2.0f;
		inputSampleR = (mid-side)/2.0f;
		//unassign mid and side
		
		
		
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
	polarityM = false;
	lastSampleM = 0.0;
	iirSampleM = 0.0;
	noiseAM = 0.0;
	noiseBM = 0.0;
	noiseCM = 0.0;
	flipM = true;
	polarityS = false;
	lastSampleS = 0.0;
	iirSampleS = 0.0;
	noiseAS = 0.0;
	noiseBS = 0.0;
	noiseCS = 0.0;
	flipS = true;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
