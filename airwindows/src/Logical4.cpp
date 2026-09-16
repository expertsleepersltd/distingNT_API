#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Logical4"
#define AIRWINDOWS_DESCRIPTION "A classic 2-buss compressor."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','L','o','4' )
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
{ .name = "Threshold", .min = -20000, .max = 20000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Ratio", .min = 1000, .max = 16000, .def = 4000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reaction Speed", .min = 100, .max = 10000, .def = 2000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Makeup Gain", .min = -20000, .max = 20000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"

	
	//begin ButterComp
	Float32 controlAposL;
	Float32 controlAnegL;
	Float32 controlBposL;
	Float32 controlBnegL;
	Float32 targetposL;
	Float32 targetnegL;
	Float32 controlAposBL;
	Float32 controlAnegBL;
	Float32 controlBposBL;
	Float32 controlBnegBL;
	Float32 targetposBL;
	Float32 targetnegBL;
	Float32 controlAposCL;
	Float32 controlAnegCL;
	Float32 controlBposCL;
	Float32 controlBnegCL;
	Float32 targetposCL;
	Float32 targetnegCL;
	Float32 avgAL;
	Float32 avgBL;
	Float32 avgCL;
	Float32 avgDL;
	Float32 avgEL;
	Float32 avgFL;
	Float32 nvgAL;
	Float32 nvgBL;
	Float32 nvgCL;
	Float32 nvgDL;
	Float32 nvgEL;
	Float32 nvgFL;
	//end ButterComp
	
	//begin Power Sag
	Float32 controlL;
	Float32 controlBL;
	Float32 controlCL;
	//end Power Sag
	
	//begin ButterComp
	Float32 controlAposR;
	Float32 controlAnegR;
	Float32 controlBposR;
	Float32 controlBnegR;
	Float32 targetposR;
	Float32 targetnegR;
	Float32 controlAposBR;
	Float32 controlAnegBR;
	Float32 controlBposBR;
	Float32 controlBnegBR;
	Float32 targetposBR;
	Float32 targetnegBR;
	Float32 controlAposCR;
	Float32 controlAnegCR;
	Float32 controlBposCR;
	Float32 controlBnegCR;
	Float32 targetposCR;
	Float32 targetnegCR;
	Float32 avgAR;
	Float32 avgBR;
	Float32 avgCR;
	Float32 avgDR;
	Float32 avgER;
	Float32 avgFR;
	Float32 nvgAR;
	Float32 nvgBR;
	Float32 nvgCR;
	Float32 nvgDR;
	Float32 nvgER;
	Float32 nvgFR;
	//end ButterComp
	
	//begin Power Sag
	Float32 controlR;
	Float32 controlBR;
	Float32 controlCR;
	//end Power Sag
	
	int gcount;
	
	bool fpFlip;
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		Float32 dL[1000];
	Float32 bL[1000];
	Float32 cL[1000];
	Float32 dR[1000];
	Float32 bR[1000];
	Float32 cR[1000];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();	
	UInt32 nSampleFrames = inFramesToProcess;
	
	Float32 drySampleL;
	Float32 drySampleR;
	float inputSampleL;
	float inputSampleR;

	float fpOld = 0.618033988749894848204586f; //golden ratio!
	float fpNew = 1.0f - fpOld;

	//begin ButterComp
	Float32 inputpos;
	Float32 inputneg;
	Float32 calcpos;
	Float32 calcneg;
	Float32 outputpos;
	Float32 outputneg;
	Float32 totalmultiplier;
	Float32 inputgain = powf(10.0f,(-GetParameter( kParam_One ))/20.0f);
	//fussing with the controls to make it hit the right threshold values
	Float32 compoutgain = inputgain; //let's try compensating for this
	
	Float32 attackspeed = GetParameter( kParam_Three );
	//is in ms
	attackspeed = 10.0f / sqrt(attackspeed);
	//convert to a remainder for use in comp
	Float32 divisor = 0.000782f*attackspeed;
	//First Speed control
	divisor /= overallscale;
	Float32 remainder = divisor;
	divisor = 1.0f - divisor;
	
	Float32 divisorB = 0.000819f*attackspeed;
	//Second Speed control
	divisorB /= overallscale;
	Float32 remainderB = divisorB;
	divisorB = 1.0f - divisorB;
	
	Float32 divisorC = 0.000857f;
	//Third Speed control
	divisorC /= overallscale;
	Float32 remainderC = divisorC*attackspeed;
	divisorC = 1.0f - divisorC;
	//end ButterComp
	
	Float32 dynamicDivisor;
	Float32 dynamicRemainder;
	//used for variable release
	
	//begin Desk Power Sag
	Float32 intensity = 0.0445556f;
	Float32 depthA = 2.42f;
	int offsetA = (int)(depthA * overallscale);
	if (offsetA < 1) offsetA = 1;
	if (offsetA > 498) offsetA = 498;
	
	Float32 depthB = 2.42f; //was 3.38f
	int offsetB = (int)(depthB * overallscale);
	if (offsetB < 1) offsetB = 1;
	if (offsetB > 498) offsetB = 498;
	
	Float32 depthC = 2.42f; //was 4.35f
	int offsetC = (int)(depthC * overallscale);
	if (offsetC < 1) offsetC = 1;
	if (offsetC > 498) offsetC = 498;
	
	Float32 clamp;
	Float32 thickness;
	Float32 out;
	Float32 bridgerectifier;
	Float32 powerSag = 0.003300223685324102874217f; //was .00365 
	//the Power Sag constant is how much the sag is cut back in high compressions. Increasing it
	//increases the guts and gnarl of the music, decreasing it or making it negative causes the texture to go 
	//'ethereal' and unsolid under compression. Very subtle!
	//end Desk Power Sag	
	
	Float32 ratio = sqrt(GetParameter( kParam_Two ))-1.0f;
	if (ratio > 2.99999f) ratio = 2.99999f;
	if (ratio < 0.0f) ratio = 0.0f;
	//anything we do must adapt to our dry/a/b/c output stages
	int ratioselector = floor( ratio );
	//zero to three, it'll become, always with 3 as the max
	ratio -= ratioselector;
	Float32 invRatio = 1.0f - ratio;
	//for all processing we've settled on the 'stage' and are just interpolating between top and bottom
	//ratio is the more extreme case, invratio becomes our 'floor' case including drySample
	
	Float32 outSampleAL = 0.0f;
	Float32 outSampleBL = 0.0f;
	Float32 outSampleCL = 0.0f;
	Float32 outSampleAR = 0.0f;
	Float32 outSampleBR = 0.0f;
	Float32 outSampleCR = 0.0f;
	//what we interpolate between using ratio
	
	Float32 outputgain = powf(10.0f,GetParameter( kParam_Four )/20.0f);
	Float32 wet = GetParameter( kParam_Five );
	//removed unnecessary dry variable
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		//assign working variables like the dry
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;
		
		gcount--;
		if (gcount < 0 || gcount > 499) {gcount = 499;}

		//begin first Power SagL
		dram->dL[gcount+499] = dram->dL[gcount] = fabs(inputSampleL)*(intensity-((controlAposL+controlBposL+controlAnegL+controlBnegL)*powerSag));
		controlL += (dram->dL[gcount] / offsetA);
		controlL -= (dram->dL[gcount+offsetA] / offsetA);
		controlL -= 0.000001f;
		clamp = 1;
		if (controlL < 0) {controlL = 0;}
		if (controlL > 1) {clamp -= (controlL - 1); controlL = 1;}
		if (clamp < 0.5f) {clamp = 0.5f;}
		//control = 0 to 1
		thickness = ((1.0f - controlL) * 2.0f) - 1.0f;
		out = fabs(thickness);		
		bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSampleL > 0) inputSampleL = (inputSampleL*(1-out))+(bridgerectifier*out);
		else inputSampleL = (inputSampleL*(1-out))-(bridgerectifier*out);
		//blend according to density control
		if (clamp != 1.0f) inputSampleL *= clamp;
		//end first Power SagL
		
		//begin first Power SagR
		dram->dR[gcount+499] = dram->dR[gcount] = fabs(inputSampleR)*(intensity-((controlAposR+controlBposR+controlAnegR+controlBnegR)*powerSag));
		controlR += (dram->dR[gcount] / offsetA);
		controlR -= (dram->dR[gcount+offsetA] / offsetA);
		controlR -= 0.000001f;
		clamp = 1;
		if (controlR < 0) {controlR = 0;}
		if (controlR > 1) {clamp -= (controlR - 1); controlR = 1;}
		if (clamp < 0.5f) {clamp = 0.5f;}
		//control = 0 to 1
		thickness = ((1.0f - controlR) * 2.0f) - 1.0f;
		out = fabs(thickness);		
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSampleR > 0) inputSampleR = (inputSampleR*(1-out))+(bridgerectifier*out);
		else inputSampleR = (inputSampleR*(1-out))-(bridgerectifier*out);
		//blend according to density control
		if (clamp != 1.0f) inputSampleR *= clamp;
		//end first Power SagR
		
		//begin first compressorL
		if (inputgain != 1.0f) inputSampleL *= inputgain;
		inputpos = (inputSampleL * fpOld) + (avgAL * fpNew) + 1.0f;
		avgAL = inputSampleL;
		//hovers around 1 when there's no signal
		
		if (inputpos < 0.001f) inputpos = 0.001f;
		outputpos = inputpos / 2.0f;
		if (outputpos > 1.0f) outputpos = 1.0f;		
		inputpos *= inputpos;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainder * (inputpos + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetposL *= dynamicDivisor;
		targetposL += (inputpos * dynamicRemainder);
		calcpos = powf((1.0f/targetposL),2);
		//extra tiny, quick, if the inputpos of this polarity is high
		
		inputneg = (-inputSampleL * fpOld) + (nvgAL * fpNew) + 1.0f;
		nvgAL = -inputSampleL;
		
		if (inputneg < 0.001f) inputneg = 0.001f;
		outputneg = inputneg / 2.0f;
		if (outputneg > 1.0f) outputneg = 1.0f;		
		inputneg *= inputneg;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainder * (inputneg + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetnegL *= dynamicDivisor;
		targetnegL += (inputneg * dynamicRemainder);
		calcneg = powf((1.0f/targetnegL),2);
		//now we have mirrored targets for comp
		//each calc is a blowed up chased target from tiny (at high levels of that polarity) to 1 at no input
		//calc is the one we want to react differently: go tiny quick, 
		//outputpos and outputneg go from 0 to 1
		
		if (inputSampleL > 0)
		{ //working on pos
			if (true == fpFlip)
			{
				controlAposL *= divisor;
				controlAposL += (calcpos*remainder);
				if (controlAposR > controlAposL) controlAposR = (controlAposR + controlAposL) * 0.5f;
				//this part makes the compressor linked: both channels will converge towards whichever
				//is the most compressed at the time.
			}
			else
			{
				controlBposL *= divisor;
				controlBposL += (calcpos*remainder);
				if (controlBposR > controlBposL) controlBposR = (controlBposR + controlBposL) * 0.5f;
			}	
		}
		else
		{ //working on neg
			if (true == fpFlip)
			{
				controlAnegL *= divisor;
				controlAnegL += (calcneg*remainder);
				if (controlAnegR > controlAnegL) controlAnegR = (controlAnegR + controlAnegL) * 0.5f;
			}
			else
			{
				controlBnegL *= divisor;
				controlBnegL += (calcneg*remainder);
				if (controlBnegR > controlBnegL) controlBnegR = (controlBnegR + controlBnegL) * 0.5f;
			}
		}
		//this causes each of the four to update only when active and in the correct 'fpFlip'
		
		if (true == fpFlip)
		{totalmultiplier = (controlAposL * outputpos) + (controlAnegL * outputneg);}
		else
		{totalmultiplier = (controlBposL * outputpos) + (controlBnegL * outputneg);}
		//this combines the sides according to fpFlip, blending relative to the input value
		
		if (totalmultiplier != 1.0f) inputSampleL *= totalmultiplier;
		//if (compoutgain != 1.0f) inputSample /= compoutgain;
		if (inputSampleL > 36.0f) inputSampleL = 36.0f;
		if (inputSampleL < -36.0f) inputSampleL = -36.0f;
		//build in +18db hard clip on insano inputs
		outSampleAL = inputSampleL / compoutgain;
		//end first compressorL
		
		//begin first compressorR
		if (inputgain != 1.0f) inputSampleR *= inputgain;
		inputpos = (inputSampleR * fpOld) + (avgAR * fpNew) + 1.0f;
		avgAR = inputSampleR;
		//hovers around 1 when there's no signal
		
		if (inputpos < 0.001f) inputpos = 0.001f;
		outputpos = inputpos / 2.0f;
		if (outputpos > 1.0f) outputpos = 1.0f;		
		inputpos *= inputpos;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainder * (inputpos + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetposR *= dynamicDivisor;
		targetposR += (inputpos * dynamicRemainder);
		calcpos = powf((1.0f/targetposR),2);
		//extra tiny, quick, if the inputpos of this polarity is high
		
		inputneg = (-inputSampleR * fpOld) + (nvgAR * fpNew) + 1.0f;
		nvgAR = -inputSampleR;
		
		if (inputneg < 0.001f) inputneg = 0.001f;
		outputneg = inputneg / 2.0f;
		if (outputneg > 1.0f) outputneg = 1.0f;		
		inputneg *= inputneg;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainder * (inputneg + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetnegR *= dynamicDivisor;
		targetnegR += (inputneg * dynamicRemainder);
		calcneg = powf((1.0f/targetnegR),2);
		//now we have mirrored targets for comp
		//each calc is a blowed up chased target from tiny (at high levels of that polarity) to 1 at no input
		//calc is the one we want to react differently: go tiny quick, 
		//outputpos and outputneg go from 0 to 1
		
		if (inputSampleR > 0)
		{ //working on pos
			if (true == fpFlip)
			{
				controlAposR *= divisor;
				controlAposR += (calcpos*remainder);
				if (controlAposL > controlAposR) controlAposL = (controlAposR + controlAposL) * 0.5f;
				//this part makes the compressor linked: both channels will converge towards whichever
				//is the most compressed at the time.				
			}
			else
			{
				controlBposR *= divisor;
				controlBposR += (calcpos*remainder);
				if (controlBposL > controlBposR) controlBposL = (controlBposR + controlBposL) * 0.5f;
			}	
		}
		else
		{ //working on neg
			if (true == fpFlip)
			{
				controlAnegR *= divisor;
				controlAnegR += (calcneg*remainder);
				if (controlAnegL > controlAnegR) controlAnegL = (controlAnegR + controlAnegL) * 0.5f;
			}
			else
			{
				controlBnegR *= divisor;
				controlBnegR += (calcneg*remainder);
				if (controlBnegL > controlBnegR) controlBnegL = (controlBnegR + controlBnegL) * 0.5f;
			}
		}
		//this causes each of the four to update only when active and in the correct 'fpFlip'
		
		if (true == fpFlip)
		{totalmultiplier = (controlAposR * outputpos) + (controlAnegR * outputneg);}
		else
		{totalmultiplier = (controlBposR * outputpos) + (controlBnegR * outputneg);}
		//this combines the sides according to fpFlip, blending relative to the input value
		
		if (totalmultiplier != 1.0f) inputSampleR *= totalmultiplier;
		//if (compoutgain != 1.0f) inputSample /= compoutgain;
		if (inputSampleR > 36.0f) inputSampleR = 36.0f;
		if (inputSampleR < -36.0f) inputSampleR = -36.0f;
		//build in +18db hard clip on insano inputs
		outSampleAR = inputSampleR / compoutgain;
		//end first compressorR
		
		if (ratioselector > 0) {
			
		//begin second Power SagL
		dram->bL[gcount+499] = dram->bL[gcount] = fabs(inputSampleL)*(intensity-((controlAposBL+controlBposBL+controlAnegBL+controlBnegBL)*powerSag));
		controlBL += (dram->bL[gcount] / offsetB);
		controlBL -= (dram->bL[gcount+offsetB] / offsetB);
		controlBL -= 0.000001f;
		clamp = 1;
		if (controlBL < 0) {controlBL = 0;}
		if (controlBL > 1) {clamp -= (controlBL - 1); controlBL = 1;}
		if (clamp < 0.5f) {clamp = 0.5f;}
		//control = 0 to 1
		thickness = ((1.0f - controlBL) * 2.0f) - 1.0f;
		out = fabs(thickness);		
		bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSampleL > 0) inputSampleL = (inputSampleL*(1-out))+(bridgerectifier*out);
		else inputSampleL = (inputSampleL*(1-out))-(bridgerectifier*out);
		//blend according to density control
		if (clamp != 1.0f) inputSampleL *= clamp;
		//end second Power SagL
		
		//begin second Power SagR
		dram->bR[gcount+499] = dram->bR[gcount] = fabs(inputSampleR)*(intensity-((controlAposBR+controlBposBR+controlAnegBR+controlBnegBR)*powerSag));
		controlBR += (dram->bR[gcount] / offsetB);
		controlBR -= (dram->bR[gcount+offsetB] / offsetB);
		controlBR -= 0.000001f;
		clamp = 1;
		if (controlBR < 0) {controlBR = 0;}
		if (controlBR > 1) {clamp -= (controlBR - 1); controlBR = 1;}
		if (clamp < 0.5f) {clamp = 0.5f;}
		//control = 0 to 1
		thickness = ((1.0f - controlBR) * 2.0f) - 1.0f;
		out = fabs(thickness);		
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSampleR > 0) inputSampleR = (inputSampleR*(1-out))+(bridgerectifier*out);
		else inputSampleR = (inputSampleR*(1-out))-(bridgerectifier*out);
		//blend according to density control
		if (clamp != 1.0f) inputSampleR *= clamp;
		//end second Power SagR
		
		
		//begin second compressorL
		inputpos = (inputSampleL * fpOld) + (avgBL * fpNew) + 1.0f;
		avgBL = inputSampleL;
		
		if (inputpos < 0.001f) inputpos = 0.001f;
		outputpos = inputpos / 2.0f;
		if (outputpos > 1.0f) outputpos = 1.0f;		
		inputpos *= inputpos;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainderB * (inputpos + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetposBL *= dynamicDivisor;
		targetposBL += (inputpos * dynamicRemainder);
		calcpos = powf((1.0f/targetposBL),2);
		
		inputneg = (-inputSampleL * fpOld) + (nvgBL * fpNew) + 1.0f;
		nvgBL = -inputSampleL;
		
		if (inputneg < 0.001f) inputneg = 0.001f;
		outputneg = inputneg / 2.0f;
		if (outputneg > 1.0f) outputneg = 1.0f;		
		inputneg *= inputneg;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainderB * (inputneg + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetnegBL *= dynamicDivisor;
		targetnegBL += (inputneg * dynamicRemainder);
		calcneg = powf((1.0f/targetnegBL),2);
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1
		if (inputSampleL > 0)
		{ //working on pos
			if (true == fpFlip)
			{
				controlAposBL *= divisorB;
				controlAposBL += (calcpos*remainderB);
				if (controlAposBR > controlAposBL) controlAposBR = (controlAposBR + controlAposBL) * 0.5f;
				//this part makes the compressor linked: both channels will converge towards whichever
				//is the most compressed at the time.				
			}
			else
			{
				controlBposBL *= divisorB;
				controlBposBL += (calcpos*remainderB);
				if (controlBposBR > controlBposBL) controlBposBR = (controlBposBR + controlBposBL) * 0.5f;
			}	
		}
		else
		{ //working on neg
			if (true == fpFlip)
			{
				controlAnegBL *= divisorB;
				controlAnegBL += (calcneg*remainderB);
				if (controlAnegBR > controlAnegBL) controlAnegBR = (controlAnegBR + controlAnegBL) * 0.5f;
			}
			else
			{
				controlBnegBL *= divisorB;
				controlBnegBL += (calcneg*remainderB);
				if (controlBnegBR > controlBnegBL) controlBnegBR = (controlBnegBR + controlBnegBL) * 0.5f;
			}
		}
		//this causes each of the four to update only when active and in the correct 'fpFlip'
		
		if (true == fpFlip)
		{totalmultiplier = (controlAposBL * outputpos) + (controlAnegBL * outputneg);}
		else
		{totalmultiplier = (controlBposBL * outputpos) + (controlBnegBL * outputneg);}
		//this combines the sides according to fpFlip, blending relative to the input value
		
		if (totalmultiplier != 1.0f) inputSampleL *= totalmultiplier;
		//if (compoutgain != 1.0f) inputSample /= compoutgain;
		if (inputSampleL > 36.0f) inputSampleL = 36.0f;
		if (inputSampleL < -36.0f) inputSampleL = -36.0f;
		//build in +18db hard clip on insano inputs
		outSampleBL = inputSampleL / compoutgain;
		//end second compressorL
		
		//begin second compressorR
		inputpos = (inputSampleR * fpOld) + (avgBR * fpNew) + 1.0f;
		avgBR = inputSampleR;
		
		if (inputpos < 0.001f) inputpos = 0.001f;
		outputpos = inputpos / 2.0f;
		if (outputpos > 1.0f) outputpos = 1.0f;		
		inputpos *= inputpos;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainderB * (inputpos + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetposBR *= dynamicDivisor;
		targetposBR += (inputpos * dynamicRemainder);
		calcpos = powf((1.0f/targetposBR),2);
		
		inputneg = (-inputSampleR * fpOld) + (nvgBR * fpNew) + 1.0f;
		nvgBR = -inputSampleR;
		
		if (inputneg < 0.001f) inputneg = 0.001f;
		outputneg = inputneg / 2.0f;
		if (outputneg > 1.0f) outputneg = 1.0f;		
		inputneg *= inputneg;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainderB * (inputneg + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetnegBR *= dynamicDivisor;
		targetnegBR += (inputneg * dynamicRemainder);
		calcneg = powf((1.0f/targetnegBR),2);
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1
		if (inputSampleR > 0)
		{ //working on pos
			if (true == fpFlip)
			{
				controlAposBR *= divisorB;
				controlAposBR += (calcpos*remainderB);
				if (controlAposBL > controlAposBR) controlAposBL = (controlAposBR + controlAposBL) * 0.5f;
				//this part makes the compressor linked: both channels will converge towards whichever
				//is the most compressed at the time.				
			}
			else
			{
				controlBposBR *= divisorB;
				controlBposBR += (calcpos*remainderB);
				if (controlBposBL > controlBposBR) controlBposBL = (controlBposBR + controlBposBL) * 0.5f;
			}	
		}
		else
		{ //working on neg
			if (true == fpFlip)
			{
				controlAnegBR *= divisorB;
				controlAnegBR += (calcneg*remainderB);
				if (controlAnegBL > controlAnegBR) controlAnegBL = (controlAnegBR + controlAnegBL) * 0.5f;
			}
			else
			{
				controlBnegBR *= divisorB;
				controlBnegBR += (calcneg*remainderB);
				if (controlBnegBL > controlBnegBR) controlBnegBL = (controlBnegBR + controlBnegBL) * 0.5f;
			}
		}
		//this causes each of the four to update only when active and in the correct 'fpFlip'
		
		if (true == fpFlip)
		{totalmultiplier = (controlAposBR * outputpos) + (controlAnegBR * outputneg);}
		else
		{totalmultiplier = (controlBposBR * outputpos) + (controlBnegBR * outputneg);}
		//this combines the sides according to fpFlip, blending relative to the input value
		
		if (totalmultiplier != 1.0f) inputSampleR *= totalmultiplier;
		//if (compoutgain != 1.0f) inputSample /= compoutgain;
		if (inputSampleR > 36.0f) inputSampleR = 36.0f;
		if (inputSampleR < -36.0f) inputSampleR = -36.0f;
		//build in +18db hard clip on insano inputs
		outSampleBR = inputSampleR / compoutgain;
		//end second compressorR
			
			if (ratioselector > 1) {
		
		//begin third Power SagL
		dram->cL[gcount+499] = dram->cL[gcount] = fabs(inputSampleL)*(intensity-((controlAposCL+controlBposCL+controlAnegCL+controlBnegCL)*powerSag));
		controlCL += (dram->cL[gcount] / offsetC);
		controlCL -= (dram->cL[gcount+offsetB] / offsetC);
		controlCL -= 0.000001f;
		clamp = 1;
		if (controlCL < 0) {controlCL = 0;}
		if (controlCL > 1) {clamp -= (controlCL - 1); controlCL = 1;}
		if (clamp < 0.5f) {clamp = 0.5f;}
		//control = 0 to 1
		thickness = ((1.0f - controlCL) * 2.0f) - 1.0f;
		out = fabs(thickness);		
		bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSampleL > 0) inputSampleL = (inputSampleL*(1-out))+(bridgerectifier*out);
		else inputSampleL = (inputSampleL*(1-out))-(bridgerectifier*out);
		//blend according to density control
		if (clamp != 1.0f) inputSampleL *= clamp;
		//end third Power SagL
		
		//begin third Power SagR
		dram->cR[gcount+499] = dram->cR[gcount] = fabs(inputSampleR)*(intensity-((controlAposCR+controlBposCR+controlAnegCR+controlBnegCR)*powerSag));
		controlCR += (dram->cR[gcount] / offsetC);
		controlCR -= (dram->cR[gcount+offsetB] / offsetC);
		controlCR -= 0.000001f;
		clamp = 1;
		if (controlCR < 0) {controlCR = 0;}
		if (controlCR > 1) {clamp -= (controlCR - 1); controlCR = 1;}
		if (clamp < 0.5f) {clamp = 0.5f;}
		//control = 0 to 1
		thickness = ((1.0f - controlCR) * 2.0f) - 1.0f;
		out = fabs(thickness);		
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSampleR > 0) inputSampleR = (inputSampleR*(1-out))+(bridgerectifier*out);
		else inputSampleR = (inputSampleR*(1-out))-(bridgerectifier*out);
		//blend according to density control
		if (clamp != 1.0f) inputSampleR *= clamp;
		//end third Power SagR
		
		//begin third compressorL
		inputpos = (inputSampleL * fpOld) + (avgCL * fpNew) + 1.0f;
		avgCL = inputSampleL;
		
		if (inputpos < 0.001f) inputpos = 0.001f;
		outputpos = inputpos / 2.0f;
		if (outputpos > 1.0f) outputpos = 1.0f;		
		inputpos *= inputpos;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainderC * (inputpos + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetposCL *= dynamicDivisor;
		targetposCL += (inputpos * dynamicRemainder);
		calcpos = powf((1.0f/targetposCL),2);
		
		inputneg = (-inputSampleL * fpOld) + (nvgCL * fpNew) + 1.0f;
		nvgCL = -inputSampleL;
		
		if (inputneg < 0.001f) inputneg = 0.001f;
		outputneg = inputneg / 2.0f;
		if (outputneg > 1.0f) outputneg = 1.0f;		
		inputneg *= inputneg;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainderC * (inputneg + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetnegCL *= dynamicDivisor;
		targetnegCL += (inputneg * dynamicRemainder);
		calcneg = powf((1.0f/targetnegCL),2);
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1
		if (inputSampleL > 0)
		{ //working on pos
			if (true == fpFlip)
			{
				controlAposCL *= divisorC;
				controlAposCL += (calcpos*remainderC);
				if (controlAposCR > controlAposCL) controlAposCR = (controlAposCR + controlAposCL) * 0.5f;
				//this part makes the compressor linked: both channels will converge towards whichever
				//is the most compressed at the time.				
			}
			else
			{
				controlBposCL *= divisorC;
				controlBposCL += (calcpos*remainderC);
				if (controlBposCR > controlBposCL) controlBposCR = (controlBposCR + controlBposCL) * 0.5f;
			}	
		}
		else
		{ //working on neg
			if (true == fpFlip)
			{
				controlAnegCL *= divisorC;
				controlAnegCL += (calcneg*remainderC);
				if (controlAnegCR > controlAnegCL) controlAnegCR = (controlAnegCR + controlAnegCL) * 0.5f;
			}
			else
			{
				controlBnegCL *= divisorC;
				controlBnegCL += (calcneg*remainderC);
				if (controlBnegCR > controlBnegCL) controlBnegCR = (controlBnegCR + controlBnegCL) * 0.5f;
			}
		}
		//this causes each of the four to update only when active and in the correct 'fpFlip'
		
		if (true == fpFlip)
		{totalmultiplier = (controlAposCL * outputpos) + (controlAnegCL * outputneg);}
		else
		{totalmultiplier = (controlBposCL * outputpos) + (controlBnegCL * outputneg);}
		//this combines the sides according to fpFlip, blending relative to the input value
		
		if (totalmultiplier != 1.0f) inputSampleL *= totalmultiplier;
		if (inputSampleL > 36.0f) inputSampleL = 36.0f;
		if (inputSampleL < -36.0f) inputSampleL = -36.0f;
		//build in +18db hard clip on insano inputs
		outSampleCL = inputSampleL / compoutgain;
		//if (compoutgain != 1.0f) inputSample /= compoutgain;
		//end third compressorL
		
		//begin third compressorR
		inputpos = (inputSampleR * fpOld) + (avgCR * fpNew) + 1.0f;
		avgCR = inputSampleR;
		
		if (inputpos < 0.001f) inputpos = 0.001f;
		outputpos = inputpos / 2.0f;
		if (outputpos > 1.0f) outputpos = 1.0f;		
		inputpos *= inputpos;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainderC * (inputpos + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetposCL *= dynamicDivisor;
		targetposCL += (inputpos * dynamicRemainder);
		calcpos = powf((1.0f/targetposCR),2);
		
		inputneg = (-inputSampleR * fpOld) + (nvgCR * fpNew) + 1.0f;
		nvgCR = -inputSampleR;
		
		if (inputneg < 0.001f) inputneg = 0.001f;
		outputneg = inputneg / 2.0f;
		if (outputneg > 1.0f) outputneg = 1.0f;		
		inputneg *= inputneg;
		//will be very high for hot input, can be 0.00001f-1 for other-polarity
		
		dynamicRemainder = remainderC * (inputneg + 1.0f);
		if (dynamicRemainder > 1.0f) dynamicRemainder = 1.0f;
		dynamicDivisor = 1.0f - dynamicRemainder;
		//calc chases much faster if input swing is high
		
		targetnegCR *= dynamicDivisor;
		targetnegCR += (inputneg * dynamicRemainder);
		calcneg = powf((1.0f/targetnegCR),2);
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1
		if (inputSampleR > 0)
		{ //working on pos
			if (true == fpFlip)
			{
				controlAposCR *= divisorC;
				controlAposCR += (calcpos*remainderC);
				if (controlAposCL > controlAposCR) controlAposCL = (controlAposCR + controlAposCL) * 0.5f;
				//this part makes the compressor linked: both channels will converge towards whichever
				//is the most compressed at the time.				
			}
			else
			{
				controlBposCR *= divisorC;
				controlBposCR += (calcpos*remainderC);
				if (controlBposCL > controlBposCR) controlBposCL = (controlBposCR + controlBposCL) * 0.5f;
			}	
		}
		else
		{ //working on neg
			if (true == fpFlip)
			{
				controlAnegCR *= divisorC;
				controlAnegCR += (calcneg*remainderC);
				if (controlAnegCL > controlAnegCR) controlAnegCL = (controlAnegCR + controlAnegCL) * 0.5f;
			}
			else
			{
				controlBnegCR *= divisorC;
				controlBnegCR += (calcneg*remainderC);
				if (controlBnegCL > controlBnegCR) controlBnegCL = (controlBnegCR + controlBnegCL) * 0.5f;
			}
		}
		//this causes each of the four to update only when active and in the correct 'fpFlip'
		
		if (true == fpFlip)
		{totalmultiplier = (controlAposCR * outputpos) + (controlAnegCR * outputneg);}
		else
		{totalmultiplier = (controlBposCR * outputpos) + (controlBnegCR * outputneg);}
		//this combines the sides according to fpFlip, blending relative to the input value
		
		if (totalmultiplier != 1.0f) inputSampleR *= totalmultiplier;
		if (inputSampleR > 36.0f) inputSampleR = 36.0f;
		if (inputSampleR < -36.0f) inputSampleR = -36.0f;
		//build in +18db hard clip on insano inputs
		outSampleCR = inputSampleR / compoutgain;
		//if (compoutgain != 1.0f) inputSample /= compoutgain;
		//end third compressorR
		}
		} //these nested if blocks are not indented because the old xCode doesn't support it
		
		//here we will interpolate between dry, and the three post-stages of processing
		switch (ratioselector) {
			case 0:
				inputSampleL = (drySampleL * invRatio) + (outSampleAL * ratio);
				inputSampleR = (drySampleR * invRatio) + (outSampleAR * ratio);
				break;
			case 1:
				inputSampleL = (outSampleAL * invRatio) + (outSampleBL * ratio);
				inputSampleR = (outSampleAR * invRatio) + (outSampleBR * ratio);
				break;
			default:
				inputSampleL = (outSampleBL * invRatio) + (outSampleCL * ratio);
				inputSampleR = (outSampleBR * invRatio) + (outSampleCR * ratio);
				break;
		}
		//only then do we reconstruct the output, but our ratio is built here
		
		if (outputgain != 1.0f) {
			inputSampleL *= outputgain;
			inputSampleR *= outputgain;
		}

		if (wet != 1.0f) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f-wet));
		}		
		fpFlip = !fpFlip;
		
		

		*outputL = inputSampleL;
		*outputR = inputSampleR;
		
		inputL += 1; inputR += 1; outputL += 1; outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	//begin ButterComps
	controlAposL = 1.0;
	controlAnegL = 1.0;
	controlBposL = 1.0;
	controlBnegL = 1.0;
	targetposL = 1.0;
	targetnegL = 1.0;
	
	controlAposBL = 1.0;
	controlAnegBL = 1.0;
	controlBposBL = 1.0;
	controlBnegBL = 1.0;
	targetposBL = 1.0;
	targetnegBL = 1.0;
	
	controlAposCL = 1.0;
	controlAnegCL = 1.0;
	controlBposCL = 1.0;
	controlBnegCL = 1.0;
	targetposCL = 1.0;
	targetnegCL = 1.0;
	
	avgAL = avgBL = avgCL = avgDL = avgEL = avgFL = 0.0;
	nvgAL = nvgBL = nvgCL = nvgDL = nvgEL = nvgFL = 0.0;
	//end ButterComps
	
	//begin ButterComps
	controlAposR = 1.0;
	controlAnegR = 1.0;
	controlBposR = 1.0;
	controlBnegR = 1.0;
	targetposR = 1.0;
	targetnegR = 1.0;
	
	controlAposBR = 1.0;
	controlAnegBR = 1.0;
	controlBposBR = 1.0;
	controlBnegBR = 1.0;
	targetposBR = 1.0;
	targetnegBR = 1.0;
	
	controlAposCR = 1.0;
	controlAnegCR = 1.0;
	controlBposCR = 1.0;
	controlBnegCR = 1.0;
	targetposCR = 1.0;
	targetnegCR = 1.0;
	
	avgAR = avgBR = avgCR = avgDR = avgER = avgFR = 0.0;
	nvgAR = nvgBR = nvgCR = nvgDR = nvgER = nvgFR = 0.0;
	//end ButterComps
	
	//begin Power Sags
	for(int count = 0; count < 999; count++) {dram->dL[count] = 0; dram->bL[count] = 0; dram->cL[count] = 0; dram->dR[count] = 0; dram->bR[count] = 0; dram->cR[count] = 0;}
	controlL = 0; controlBL = 0; controlCL = 0;
	controlR = 0; controlBR = 0; controlCR = 0;
	
	gcount = 0;
	//end Power Sags
	
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	fpFlip = true;
	return noErr;
}


};
