#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Gatelinked"
#define AIRWINDOWS_DESCRIPTION "Gatelinked"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','a','d' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
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
{ .name = "Threshold", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Treble Sustain", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Sustain", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack Speed", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
		Float32 iirLowpassAR;
		Float32 iirLowpassBR;
		Float32 iirHighpassAR;
		Float32 iirHighpassBR;
		Float32 iirLowpassAL;
		Float32 iirLowpassBL;
		Float32 iirHighpassAL;
		Float32 iirHighpassBL;
		Float32 treblefreq;
		Float32 bassfreq;
		bool flip;
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
	//speed settings around release
	Float32 threshold = powf(GetParameter( kParam_One ),2);
	//gain settings around threshold
	Float32 trebledecay = powf(1.0f-GetParameter( kParam_Two ),2)/4196.0f;
	Float32 bassdecay =  powf(1.0f-GetParameter( kParam_Three ),2)/8192.0f;
	Float32 slowAttack = (powf(GetParameter( kParam_Four ),3)*3)+0.003f;
	Float32 wet = GetParameter( kParam_Five );
	slowAttack /= overallscale;
	trebledecay /= overallscale;
	bassdecay /= overallscale;
	trebledecay += 1.0f;
	bassdecay += 1.0f;
	Float32 attackSpeed;
	Float32 highestSample;

	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		if (fabs(inputSampleL) > fabs(inputSampleR)) {
			attackSpeed = slowAttack - (fabs(inputSampleL)*slowAttack*0.5f);
			highestSample = fabs(inputSampleL);
		} else {
			attackSpeed = slowAttack - (fabs(inputSampleR)*slowAttack*0.5f); //we're triggering off the highest amplitude
			highestSample = fabs(inputSampleR); //and making highestSample the abs() of that amplitude
		}
		
		if (attackSpeed < 0.0f) attackSpeed = 0.0f;
		//softening onset click depending on how hard we're getting it
		
		if (flip)
		{
			if (highestSample > threshold)
			{
				treblefreq += attackSpeed;
				if (treblefreq > 2.0f) treblefreq = 2.0f;
				bassfreq -= attackSpeed;
				bassfreq -= attackSpeed;
				if (bassfreq < 0.0f) bassfreq = 0.0f;
				iirLowpassAL = iirLowpassBL = inputSampleL;
				iirHighpassAL = iirHighpassBL = 0.0f;
				iirLowpassAR = iirLowpassBR = inputSampleR;
				iirHighpassAR = iirHighpassBR = 0.0f;
			}
			else
			{
				treblefreq -= bassfreq;
				treblefreq /= trebledecay;
				treblefreq += bassfreq;
				bassfreq -= treblefreq;
				bassfreq /= bassdecay;
				bassfreq += treblefreq;
			}
			
			if (treblefreq >= 1.0f) {
				iirLowpassAL = inputSampleL;
				iirLowpassAR = inputSampleR;
			} else {
				iirLowpassAL = (iirLowpassAL * (1.0f - treblefreq)) + (inputSampleL * treblefreq);
				iirLowpassAR = (iirLowpassAR * (1.0f - treblefreq)) + (inputSampleR * treblefreq);
			}
			
			if (bassfreq > 1.0f) bassfreq = 1.0f;
			
			if (bassfreq > 0.0f) {
				iirHighpassAL = (iirHighpassAL * (1.0f - bassfreq)) + (inputSampleL * bassfreq);
				iirHighpassAR = (iirHighpassAR * (1.0f - bassfreq)) + (inputSampleR * bassfreq);
			} else {
				iirHighpassAL = 0.0f;
				iirHighpassAR = 0.0f;
			}
			
			if (treblefreq > bassfreq) {
				inputSampleL = (iirLowpassAL - iirHighpassAL);
				inputSampleR = (iirLowpassAR - iirHighpassAR);
			} else {
				inputSampleL = 0.0f;
				inputSampleR = 0.0f;
			}
		}
		else
		{
			if (highestSample > threshold)
			{
				treblefreq += attackSpeed;
				if (treblefreq > 2.0f) treblefreq = 2.0f;
				bassfreq -= attackSpeed;
				bassfreq -= attackSpeed;
				if (bassfreq < 0.0f) bassfreq = 0.0f;
				iirLowpassAL = iirLowpassBL = inputSampleL;
				iirHighpassAL = iirHighpassBL = 0.0f;
				iirLowpassAR = iirLowpassBR = inputSampleR;
				iirHighpassAR = iirHighpassBR = 0.0f;
			}
			else
			{
				treblefreq -= bassfreq;
				treblefreq /= trebledecay;
				treblefreq += bassfreq;
				bassfreq -= treblefreq;
				bassfreq /= bassdecay;
				bassfreq += treblefreq;
			}
			
			if (treblefreq >= 1.0f) {
				iirLowpassBL = inputSampleL;
				iirLowpassBR = inputSampleR;
			} else {
				iirLowpassBL = (iirLowpassBL * (1.0f - treblefreq)) + (inputSampleL * treblefreq);
				iirLowpassBR = (iirLowpassBR * (1.0f - treblefreq)) + (inputSampleR * treblefreq);
			}
			
			if (bassfreq > 1.0f) bassfreq = 1.0f;

			if (bassfreq > 0.0f) {
				iirHighpassBL = (iirHighpassBL * (1.0f - bassfreq)) + (inputSampleL * bassfreq);
				iirHighpassBR = (iirHighpassBR * (1.0f - bassfreq)) + (inputSampleR * bassfreq);
			} else {
				iirHighpassBL = 0.0f;
				iirHighpassBR = 0.0f;
			}
			
			if (treblefreq > bassfreq) {
				inputSampleL = (iirLowpassBL - iirHighpassBL);
				inputSampleR = (iirLowpassBR - iirHighpassBR);
			} else {
				inputSampleL = 0.0f;
				inputSampleR = 0.0f;
			}			
		}
		//done full gated envelope filtered effect
		inputSampleL  = ((1-wet)*drySampleL)+(wet*inputSampleL);
		inputSampleR  = ((1-wet)*drySampleR)+(wet*inputSampleR);
		//we're going to set up a dry/wet control instead of a min. threshold
		
		flip = !flip;
				
		
		
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
	iirLowpassAL = 0.0;
	iirLowpassBL = 0.0;
	iirHighpassAL = 0.0;
	iirHighpassBL = 0.0;
	iirLowpassAR = 0.0;
	iirLowpassBR = 0.0;
	iirHighpassAR = 0.0;
	iirHighpassBR = 0.0;
	treblefreq = 1.0;
	bassfreq = 0.0;
	flip = false;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
