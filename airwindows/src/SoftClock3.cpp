#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SoftClock3"
#define AIRWINDOWS_DESCRIPTION "SoftClock3"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','o','h' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
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
{ .name = "Tempo", .min = 4000, .max = 24000, .def = 12000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Count", .min = 0, .max = 32000, .def = 4000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tuple", .min = 0, .max = 16000, .def = 4000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Triplet", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Flavor", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float sinePos;
	float barPos;
	float inc;
	int beatPos;
	int beatTable[35];
	int currentBPM;
	float step;
	float swagger;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float beatAccent[35];
	float beatSwing[35];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	int bpm = GetParameter( kParam_A );
	int beatCode = GetParameter( kParam_B );
	float notes = (float)fmax(GetParameter( kParam_C )/4.0f, 0.125f);
	float bpmTarget = (GetSampleRate()*60.0f)/((float)bpm*notes);
	float triplet = 0.0f;
	if (GetParameter( kParam_D ) > 0) triplet = 1.0f;
	float accent = GetParameter( kParam_E );
	
	if (bpm != currentBPM) {
		currentBPM = bpm;
		step = 20.0f;
		float trial = 250.0f;
		while (trial > bpm) {step -= 1.0f; trial = (0.016666666666f*powf(step,M_PI))+(step*M_PI)+20;}
		//we now have the nearest 'flow' energy state UNDER the real bpm, and will step up more slowly
		while (trial < bpm) {step += 0.01f; trial = (0.016666666666f*powf(step,M_PI))+(step*M_PI)+20;}
		//we now have the nearest tenth of the energy state and have overshot
		while (trial > bpm) {step -= 0.001f; trial = (0.016666666666f*powf(step,M_PI))+(step*M_PI)+20;}
		//we now have the nearest hundredth to the correct energy state, and can throw away 'trial'
		step = step-((int)step); //and calculate speed based on the fractional remainder of 'step'
		step = ((0.5f-fabs(step-0.5f))*2.0f) * 0.75f;
		swagger = powf((0.5f-fabs(step-0.5f))*2.0f,1.618033988749894f) * 0.5f * accent;
		//and turn it into the 'speed' control
		//with step 0.0f being 'flow', 0.25f being 'groove', 0.5f being 'edge' and 0.75f being 'tude'
		//turned into a linear speed control where edge is top step and flow being lowest step.
	}	
	float swing = (triplet+swagger)*bpmTarget*0.33333f;
	//swing makes beats hit LATER, so the One is 0.0f
	int beatMax = beatTable[beatCode];
	//only some counts are literal, others are ways to do prime grooves with different subrhythms
	for (int x = 0; x < (beatMax+1); x++) {
		dram->beatAccent[x] = ((float)fabs((float)beatMax-((float)x*2.0f)))/(float)(beatMax*1.618033988749894f);
		if (x % 2 > 0) dram->beatSwing[x] = (swagger*(1.0f-dram->beatAccent[x]));
		else dram->beatSwing[x] = swing;
	} //this makes the non-accented beats drop down to quiet and back up to half volume
	//we're also decoupling swing from triplet feel unless actually playing triplets
	//otherwise, it's part of how you hear accents and tempos
	if (beatCode > 0) dram->beatAccent[1] = 0.9f; dram->beatSwing[1] = 0.0f; //first note is an accent at full crank
	switch (beatCode)
	{
		case 0:	break; //not used
		case 1:	break; //1
		case 2:	break; //2
		case 3:	break; //3
		case 4: dram->beatAccent[3]=0.9f;
			dram->beatSwing[3]=0.0f; break; //4-22
		case 5:	dram->beatAccent[4]=0.9f;
			dram->beatSwing[4]=0.0f; break; //5-32
		case 6:	dram->beatAccent[4]=0.9f;
			dram->beatSwing[4]=0.0f; break; //6-33
		case 7:	dram->beatAccent[5]=0.9f;
			dram->beatSwing[5]=0.0f; break; //7-43
		case 8:	dram->beatAccent[5]=0.9f;
			dram->beatSwing[5]=0.0f; break; //8-44
		case 9:	dram->beatAccent[4]=0.9f; dram->beatAccent[7]=0.8f;
			dram->beatSwing[4]=0.0f; dram->beatSwing[7]=0.0f; break; //9-333
		case 10: dram->beatAccent[6]=0.9f;
			dram->beatSwing[6]=0.0f; break; //10-55
		case 11: dram->beatAccent[4]=0.9f;	dram->beatAccent[7]=0.8f;	dram->beatAccent[10]=0.7f;
			dram->beatSwing[4]=0.0f; dram->beatSwing[7]=0.0f; dram->beatSwing[10]=0.0f; break; //11-3332
		case 12: dram->beatAccent[5]=0.9f;	dram->beatAccent[9]=0.8f;
			dram->beatSwing[5]=0.0f; dram->beatSwing[9]=0.0f; break; //11-443
		case 13: dram->beatAccent[6]=0.9f;	dram->beatAccent[11]=0.8f;
			dram->beatSwing[6]=0.0f; dram->beatSwing[11]=0.0f; break; //11-551
		case 14: dram->beatAccent[7]=0.9f;
			dram->beatSwing[7]=0.0f; break; //11-65
		case 15: dram->beatAccent[4]=0.9f;	dram->beatAccent[7]=0.8f;	dram->beatAccent[10]=0.7f;
			dram->beatSwing[4]=0.0f; dram->beatSwing[7]=0.0f; dram->beatSwing[10]=0.0f; break; //13-3334
		case 16: dram->beatAccent[9]=0.9f;
			dram->beatSwing[9]=0.0f; break; //16-88
		case 17: dram->beatAccent[5]=0.9f;	dram->beatAccent[9]=0.8f;
			dram->beatSwing[5]=0.0f; dram->beatSwing[9]=0.0f; break; //13-445
		case 18: dram->beatAccent[6]=0.9f;	dram->beatAccent[11]=0.8f;
			dram->beatSwing[6]=0.0f; dram->beatSwing[11]=0.0f; break; //13-553
		case 19: dram->beatAccent[5]=0.9f;	dram->beatAccent[9]=0.85f;	dram->beatAccent[13]=0.8f;	dram->beatAccent[17]=0.75f;
			dram->beatSwing[5]=0.0f; dram->beatSwing[9]=0.0f; dram->beatSwing[13]=0.0f; dram->beatSwing[17]=0.0f; break; //17-44441
		case 20: dram->beatAccent[6]=0.9f;	dram->beatAccent[11]=0.8f;	dram->beatAccent[16]=0.7f;
			dram->beatSwing[6]=0.0f; dram->beatSwing[11]=0.0f; dram->beatSwing[16]=0.0f; break; //17-5552
		case 21: dram->beatAccent[8]=0.9f;	dram->beatAccent[15]=0.8f;
			dram->beatSwing[8]=0.0f; dram->beatSwing[15]=0.0f; break; //17-773
		case 22: dram->beatAccent[9]=0.9f;	dram->beatAccent[17]=0.8f;
			dram->beatSwing[9]=0.0f; dram->beatSwing[17]=0.0f; break; //17-881
		case 23: dram->beatAccent[5]=0.9f;	dram->beatAccent[9]=0.85f;	dram->beatAccent[13]=0.8f;	dram->beatAccent[17]=0.75f;
			dram->beatSwing[5]=0.0f; dram->beatSwing[9]=0.0f; dram->beatSwing[13]=0.0f; dram->beatSwing[17]=0.0f; break; //19-44443
		case 24: dram->beatAccent[9]=0.9f;	dram->beatAccent[17]=0.8f;
			dram->beatSwing[9]=0.0f; dram->beatSwing[17]=0.0f; break; //24-888
		case 25: dram->beatAccent[6]=0.9f;	dram->beatAccent[11]=0.8f;	dram->beatAccent[16]=0.7f;
			dram->beatSwing[6]=0.0f; dram->beatSwing[11]=0.0f; dram->beatSwing[16]=0.0f; break; //19-5554
		case 26: dram->beatAccent[8]=0.9f;	dram->beatAccent[15]=0.8f;
			dram->beatSwing[8]=0.0f; dram->beatSwing[15]=0.0f; break; //19-775
		case 27: dram->beatAccent[9]=0.9f;	dram->beatAccent[17]=0.8f;
			dram->beatSwing[9]=0.0f; dram->beatSwing[17]=0.0f; break; //19-883
		case 28: dram->beatAccent[5]=0.9f;	dram->beatAccent[9]=0.85f;	dram->beatAccent[13]=0.8f;	dram->beatAccent[17]=0.75f;	dram->beatAccent[21]=0.7f;
			dram->beatSwing[5]=0.0f; dram->beatSwing[9]=0.0f; dram->beatSwing[13]=0.0f; dram->beatSwing[17]=0.0f; dram->beatSwing[21]=0.0f; break; //23-444443
		case 29: dram->beatAccent[6]=0.9f;	dram->beatAccent[11]=0.8f;	dram->beatAccent[16]=0.7f;
			dram->beatSwing[6]=0.0f; dram->beatSwing[11]=0.0f; dram->beatSwing[16]=0.0f; break; //23-5558
		case 30: dram->beatAccent[8]=0.9f;	dram->beatAccent[15]=0.8f;	dram->beatAccent[22]=0.7f;
			dram->beatSwing[8]=0.0f; dram->beatSwing[15]=0.0f; dram->beatSwing[22]=0.0f; break; //23-7772
		case 31: dram->beatAccent[9]=0.9f;	dram->beatAccent[17]=0.8f;
			dram->beatSwing[9]=0.0f; dram->beatSwing[17]=0.0f; break; //23-887
		case 32: dram->beatAccent[9]=0.9f;	dram->beatAccent[17]=0.8f;	dram->beatAccent[25]=0.7f;
			dram->beatSwing[9]=0.0f; dram->beatSwing[17]=0.0f; dram->beatSwing[25]=0.0f; break; //32-8888
		default: break;			
	}			
	float chaseSpeed = ((step*0.000125f)+0.0000125f)/overallscale;
	float rootSpeed = 0.9999f-(chaseSpeed*((2.0f*step)+2.0f));
	float pulseWidth = (0.085f+(accent*0.04f)-((1.0f-step)*0.06f))/chaseSpeed;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		barPos += 1.0f;
		if (barPos>bpmTarget) {
			barPos=0.0f;
			beatPos++;
			if (beatPos>beatMax) beatPos=1;
		}
		if ((barPos < (pulseWidth+dram->beatSwing[beatPos])) && (barPos > dram->beatSwing[beatPos]))
			inc = (((dram->beatAccent[beatPos]*accent)+(1.0f-accent))*chaseSpeed)+(inc*(1.0f-chaseSpeed));
		else
			inc *= rootSpeed;
		sinePos += inc;
		if (sinePos > 6.283185307179586f) sinePos -= 6.283185307179586f;
		inputSampleL = inputSampleR = sin(sin(sinePos)*inc*5.0f);
		
		
		
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
	sinePos = 0.0;
	barPos = 0.0;
	beatPos = 0;
	for (int x = 0; x < 34; x++) {dram->beatAccent[x] = 0.0; dram->beatSwing[x] = 0.0;}
	inc = 0.0;
	beatTable[0]=0;
	beatTable[1]=1;
	beatTable[2]=2;
	beatTable[3]=3;
	beatTable[4]=4;
	beatTable[5]=5;
	beatTable[6]=6;
	beatTable[7]=7;
	beatTable[8]=8;
	beatTable[9]=9;
	beatTable[10]=10;
	beatTable[11]=11;
	beatTable[12]=11;
	beatTable[13]=11;
	beatTable[14]=11;
	beatTable[15]=13;
	beatTable[16]=16;
	beatTable[17]=13;
	beatTable[18]=13;
	beatTable[19]=17;
	beatTable[20]=17;
	beatTable[21]=17;
	beatTable[22]=17;
	beatTable[23]=19;
	beatTable[24]=24;
	beatTable[25]=19;
	beatTable[26]=19;
	beatTable[27]=19;
	beatTable[28]=23;
	beatTable[29]=23;
	beatTable[30]=23;
	beatTable[31]=23;
	beatTable[32]=32;
	beatTable[33]=32;
	beatTable[34]=32;
	currentBPM = 0;
	step = 0.0;
	swagger = 0.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
