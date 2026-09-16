#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dynamics"
#define AIRWINDOWS_DESCRIPTION "A combination compressor gate."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','y','s' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
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
{ .name = "Comp", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gate", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 

	//begin Gate
	bool WasNegativeL;
	int ZeroCrossL;
	float gaterollerL;
	float gateL;
	
	bool WasNegativeR;
	int ZeroCrossR;
	float gaterollerR;
	float gateR;
	//end Gate
	
	//begin ButterComp
	float controlAposL;
	float controlAnegL;
	float controlBposL;
	float controlBnegL;
	float targetposL;
	float targetnegL;
	float avgLA;
	float avgLB;
	float nvgLA;
	float nvgLB;
	
	float controlAposR;
	float controlAnegR;
	float controlBposR;
	float controlBnegR;
	float targetposR;
	float targetnegR;
	float avgRA;
	float avgRB;
	float nvgRA;
	float nvgRB;
	bool flip;
	//end ButterComp
	
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
		
	//begin ButterComp
	float inputgain = (powf(GetParameter( kParam_One ),5)*35)+1.0f;
	float divisor = (powf(GetParameter( kParam_Two ),4) * 0.01f)+0.0005f;
	divisor /= overallscale;
	float remainder = divisor;
	divisor = 1.0f - divisor;
	//end ButterComp
	
	//begin Gate
	float onthreshold = (powf(GetParameter( kParam_Three ),3)/3)+0.00018f;
	float offthreshold = onthreshold * 1.1f;	
	float release = 0.028331119964586f;
	float absmax = 220.9f;
	//speed to be compensated w.r.t sample rate
	//end Gate
	
	float wet = GetParameter(kParam_Four );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;		
		
		//begin compressor
		//begin L
		inputSampleL *= inputgain;
		float inputpos = inputSampleL + 1.0f;		
		if (inputpos < 0.0f) inputpos = 0.0f;
		float outputpos = inputpos / 2.0f;
		if (outputpos > 1.0f) outputpos = 1.0f;		
		inputpos *= inputpos;
		targetposL *= divisor;
		targetposL += (inputpos * remainder);
		float calcpos = 1.0f/targetposL;
		
		float inputneg = -inputSampleL + 1.0f;		
		if (inputneg < 0.0f) inputneg = 0.0f;
		float outputneg = inputneg / 2.0f;
		if (outputneg > 1.0f) outputneg = 1.0f;		
		inputneg *= inputneg;
		targetnegL *= divisor;
		targetnegL += (inputneg * remainder);
		float calcneg = 1.0f/targetnegL;
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1
		
		if (inputSampleL > 0)
		{ //working on pos
			if (true == flip)
			{
				controlAposL *= divisor;
				controlAposL += (calcpos*remainder);
			} else {
				controlBposL *= divisor;
				controlBposL += (calcpos*remainder);
			}	
		} else { //working on neg
			if (true == flip)
			{
				controlAnegL *= divisor;
				controlAnegL += (calcneg*remainder);
			} else {
				controlBnegL *= divisor;
				controlBnegL += (calcneg*remainder);
			}
		}
		//this causes each of the four to update only when active and in the correct 'flip'
		
		float totalmultiplier;
		if (true == flip) totalmultiplier = (controlAposL * outputpos) + (controlAnegL * outputneg);
		else totalmultiplier = (controlBposL * outputpos) + (controlBnegL * outputneg);
		//this combines the sides according to flip, blending relative to the input value
		inputSampleL *= totalmultiplier;
		inputSampleL /= inputgain;
		//end L
		
		//begin R
		inputSampleR *= inputgain;
		inputpos = inputSampleR + 1.0f;		
		if (inputpos < 0.0f) inputpos = 0.0f;
		outputpos = inputpos / 2.0f;
		if (outputpos > 1.0f) outputpos = 1.0f;		
		inputpos *= inputpos;
		targetposR *= divisor;
		targetposR += (inputpos * remainder);
		calcpos = 1.0f/targetposR;
		
		inputneg = -inputSampleR + 1.0f;		
		if (inputneg < 0.0f) inputneg = 0.0f;
		outputneg = inputneg / 2.0f;
		if (outputneg > 1.0f) outputneg = 1.0f;		
		inputneg *= inputneg;
		targetnegR *= divisor;
		targetnegR += (inputneg * remainder);
		calcneg = 1.0f/targetnegR;
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1
		
		if (inputSampleR > 0)
		{ //working on pos
			if (true == flip)
			{
				controlAposR *= divisor;
				controlAposR += (calcpos*remainder);
			} else {
				controlBposR *= divisor;
				controlBposR += (calcpos*remainder);
			}	
		} else { //working on neg
			if (true == flip)
			{
				controlAnegR *= divisor;
				controlAnegR += (calcneg*remainder);
			} else {
				controlBnegR *= divisor;
				controlBnegR += (calcneg*remainder);
			}
		}
		//this causes each of the four to update only when active and in the correct 'flip'
		
		if (true == flip) totalmultiplier = (controlAposR * outputpos) + (controlAnegR * outputneg);
		else totalmultiplier = (controlBposR * outputpos) + (controlBnegR * outputneg);
		//this combines the sides according to flip, blending relative to the input value
		
		inputSampleR *= totalmultiplier;
		inputSampleR /= inputgain;
		//end R
		flip = !flip;
		//end compressor
		
		
		
		//begin Gate
		if (drySampleL > 0.0f)
		{
			if (WasNegativeL == true) ZeroCrossL = absmax * 0.3f;
			WasNegativeL = false;
		} else {
			ZeroCrossL += 1; WasNegativeL = true;
		}
		
		if (drySampleR > 0.0f)
		{
			if (WasNegativeR == true) ZeroCrossR = absmax * 0.3f;
			WasNegativeR = false;
		} else {
			ZeroCrossR += 1; WasNegativeR = true;
		}
		
		if (ZeroCrossL > absmax) ZeroCrossL = absmax;
		if (ZeroCrossR > absmax) ZeroCrossR = absmax;
		
		if (gateL == 0.0f)
		{
			//if gate is totally silent
			if (fabs(drySampleL) > onthreshold)
			{
				if (gaterollerL == 0.0f) gaterollerL = ZeroCrossL;
				else gaterollerL -= release;
				// trigger from total silence only- if we're active then signal must clear offthreshold
			}
			else gaterollerL -= release;
		} else {
			//gate is not silent but closing
			if (fabs(drySampleL) > offthreshold)
			{
				if (gaterollerL < ZeroCrossL) gaterollerL = ZeroCrossL;
				else gaterollerL -= release;
				//always trigger if gate is over offthreshold, otherwise close anyway
			}
			else gaterollerL -= release;
		}
		
		if (gateR == 0.0f)
		{
			//if gate is totally silent
			if (fabs(drySampleR) > onthreshold)
			{
				if (gaterollerR == 0.0f) gaterollerR = ZeroCrossR;
				else gaterollerR -= release;
				// trigger from total silence only- if we're active then signal must clear offthreshold
			}
			else gaterollerR -= release;
		} else {
			//gate is not silent but closing
			if (fabs(drySampleR) > offthreshold)
			{
				if (gaterollerR < ZeroCrossR) gaterollerR = ZeroCrossR;
				else gaterollerR -= release;
				//always trigger if gate is over offthreshold, otherwise close anyway
			}
			else gaterollerR -= release;
		}
		
		if (gaterollerL < 0.0f) gaterollerL = 0.0f;
		if (gaterollerR < 0.0f) gaterollerR = 0.0f;
		
		if (gaterollerL < 1.0f)
		{
			gateL = gaterollerL;
			float bridgerectifier = 1-cos(fabs(inputSampleL));			
			if (inputSampleL > 0) inputSampleL = (inputSampleL*gateL)+(bridgerectifier*(1.0f-gateL));
			else inputSampleL = (inputSampleL*gateL)-(bridgerectifier*(1.0f-gateL));
			if (gateL == 0.0f) inputSampleL = 0.0f;			
		} else gateL = 1.0f;
		
		if (gaterollerR < 1.0f)
		{
			gateR = gaterollerR;
			float bridgerectifier = 1-cos(fabs(inputSampleR));			
			if (inputSampleR > 0) inputSampleR = (inputSampleR*gateR)+(bridgerectifier*(1.0f-gateR));
			else inputSampleR = (inputSampleR*gateR)-(bridgerectifier*(1.0f-gateR));
			if (gateR == 0.0f) inputSampleR = 0.0f;			
		} else gateR = 1.0f;
		//end Gate
		
		if (wet != 1.0f) {
		 inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
		 inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider

		
		
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
	//begin Gate
	WasNegativeL = false;
	ZeroCrossL = 0;
	gaterollerL = 0.0;
	gateL = 0.0;
	
	WasNegativeR = false;
	ZeroCrossR = 0;
	gaterollerR = 0.0;
	gateR = 0.0;
	//end Gate
	
	//begin ButterComp
	controlAposL = 1.0;
	controlAnegL = 1.0;
	controlBposL = 1.0;
	controlBnegL = 1.0;
	targetposL = 1.0;
	targetnegL = 1.0;	
	avgLA = avgLB = 0.0;
	nvgLA = nvgLB = 0.0;
	
	controlAposR = 1.0;
	controlAnegR = 1.0;
	controlBposR = 1.0;
	controlBnegR = 1.0;
	targetposR = 1.0;
	targetnegR = 1.0;	
	avgRA = avgRB = 0.0;
	nvgRA = nvgRB = 0.0;
	//end ButterComp
	flip = false;

	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
