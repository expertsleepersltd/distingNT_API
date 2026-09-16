#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "StoneFireComp"
#define AIRWINDOWS_DESCRIPTION "Some of the dynamics for ConsoleX."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','t','p' )
enum {

	kParam_B =0,
	kParam_C =1,
	kParam_D =2,
	kParam_E =3,
	kParam_F =4,
	kParam_G =5,
	kParam_H =6,
	kParam_I =7,
	kParam_J =8,
	kParam_K =9,
	//Add your parameters here...
	kNumberOfParameters=10
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Fire Th", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fire", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "StoneTh", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Stone", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Range", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Ratio", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	
	enum {
		prevSampL1, prevSlewL1, accSlewL1,
		prevSampL2, prevSlewL2, accSlewL2,
		prevSampL3, prevSlewL3, accSlewL3,
		kalGainL, kalOutL,
		prevSampR1, prevSlewR1, accSlewR1,
		prevSampR2, prevSlewR2, accSlewR2,
		prevSampR3, prevSlewR3, accSlewR3,
		kalGainR, kalOutR,
		kal_total
	};
	float fireCompL;
	float fireCompR;
	float stoneCompL;
	float stoneCompR;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float kal[kal_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	float compFThresh = powf(GetParameter( kParam_B ),4);
	float compFAttack = 1.0f/(((powf(GetParameter( kParam_C ),3)*5000.0f)+500.0f)*overallscale);
	float compFRelease = 1.0f/(((powf(GetParameter( kParam_D ),5)*50000.0f)+500.0f)*overallscale);
	float fireGain = GetParameter( kParam_E )*2.0f;
	if (fireGain > 1.0f) fireGain *= fireGain; else fireGain = 1.0f-powf(1.0f-fireGain,2);
	float firePad = fireGain; if (firePad > 1.0f) firePad = 1.0f;
	
	float compSThresh = powf(GetParameter( kParam_F ),4);
	float compSAttack = 1.0f/(((powf(GetParameter( kParam_G ),3)*5000.0f)+500.0f)*overallscale);
	float compSRelease = 1.0f/(((powf(GetParameter( kParam_H ),5)*50000.0f)+500.0f)*overallscale);
	float stoneGain = GetParameter( kParam_I )*2.0f;
	if (stoneGain > 1.0f) stoneGain *= stoneGain; else stoneGain = 1.0f-powf(1.0f-stoneGain,2);
	float stonePad = stoneGain; if (stonePad > 1.0f) stonePad = 1.0f;
	
	float kalman = 1.0f-(powf(GetParameter( kParam_J ),2)/overallscale);
	//crossover frequency between mid/bass
	float compRatio = 1.0f-powf(1.0f-GetParameter( kParam_K ),2);
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		//begin KalmanL
		float fireL = inputSampleL;
		float temp = inputSampleL = inputSampleL*(1.0f-kalman)*0.777f;
		inputSampleL *= (1.0f-kalman);
		//set up gain levels to control the beast
		dram->kal[prevSlewL3] += dram->kal[prevSampL3] - dram->kal[prevSampL2]; dram->kal[prevSlewL3] *= 0.5f;
		dram->kal[prevSlewL2] += dram->kal[prevSampL2] - dram->kal[prevSampL1]; dram->kal[prevSlewL2] *= 0.5f;
		dram->kal[prevSlewL1] += dram->kal[prevSampL1] - inputSampleL; dram->kal[prevSlewL1] *= 0.5f;
		//make slews from each set of samples used
		dram->kal[accSlewL2] += dram->kal[prevSlewL3] - dram->kal[prevSlewL2]; dram->kal[accSlewL2] *= 0.5f;
		dram->kal[accSlewL1] += dram->kal[prevSlewL2] - dram->kal[prevSlewL1]; dram->kal[accSlewL1] *= 0.5f;
		//differences between slews: rate of change of rate of change
		dram->kal[accSlewL3] += (dram->kal[accSlewL2] - dram->kal[accSlewL1]); dram->kal[accSlewL3] *= 0.5f;
		//entering the abyss, what even is this
		dram->kal[kalOutL] += dram->kal[prevSampL1] + dram->kal[prevSlewL2] + dram->kal[accSlewL3]; dram->kal[kalOutL] *= 0.5f;
		//resynthesizing predicted result (all iir smoothed)
		dram->kal[kalGainL] += fabs(temp-dram->kal[kalOutL])*kalman*8.0f; dram->kal[kalGainL] *= 0.5f;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kal[kalGainL] > kalman*0.5f) dram->kal[kalGainL] = kalman*0.5f;
		//attempts to avoid explosions
		dram->kal[kalOutL] += (temp*(1.0f-(0.68f+(kalman*0.157f))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kal[prevSampL3] = dram->kal[prevSampL2]; dram->kal[prevSampL2] = dram->kal[prevSampL1];
		dram->kal[prevSampL1] = (dram->kal[kalGainL] * dram->kal[kalOutL]) + ((1.0f-dram->kal[kalGainL])*temp);
		//feed the chain of previous samples
		if (dram->kal[prevSampL1] > 1.0f) dram->kal[prevSampL1] = 1.0f; if (dram->kal[prevSampL1] < -1.0f) dram->kal[prevSampL1] = -1.0f;
		float stoneL = dram->kal[kalOutL]*0.777f;
		fireL -= stoneL;
		//end KalmanL
		
		//begin KalmanR
		float fireR = inputSampleR;
		temp = inputSampleR = inputSampleR*(1.0f-kalman)*0.777f;
		inputSampleR *= (1.0f-kalman);
		//set up gain levels to control the beast
		dram->kal[prevSlewR3] += dram->kal[prevSampR3] - dram->kal[prevSampR2]; dram->kal[prevSlewR3] *= 0.5f;
		dram->kal[prevSlewR2] += dram->kal[prevSampR2] - dram->kal[prevSampR1]; dram->kal[prevSlewR2] *= 0.5f;
		dram->kal[prevSlewR1] += dram->kal[prevSampR1] - inputSampleR; dram->kal[prevSlewR1] *= 0.5f;
		//make slews from each set of samples used
		dram->kal[accSlewR2] += dram->kal[prevSlewR3] - dram->kal[prevSlewR2]; dram->kal[accSlewR2] *= 0.5f;
		dram->kal[accSlewR1] += dram->kal[prevSlewR2] - dram->kal[prevSlewR1]; dram->kal[accSlewR1] *= 0.5f;
		//differences between slews: rate of change of rate of change
		dram->kal[accSlewR3] += (dram->kal[accSlewR2] - dram->kal[accSlewR1]); dram->kal[accSlewR3] *= 0.5f;
		//entering the abyss, what even is this
		dram->kal[kalOutR] += dram->kal[prevSampR1] + dram->kal[prevSlewR2] + dram->kal[accSlewR3]; dram->kal[kalOutR] *= 0.5f;
		//resynthesizing predicted result (all iir smoothed)
		dram->kal[kalGainR] += fabs(temp-dram->kal[kalOutR])*kalman*8.0f; dram->kal[kalGainR] *= 0.5f;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kal[kalGainR] > kalman*0.5f) dram->kal[kalGainR] = kalman*0.5f;
		//attempts to avoid explosions
		dram->kal[kalOutR] += (temp*(1.0f-(0.68f+(kalman*0.157f))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kal[prevSampR3] = dram->kal[prevSampR2]; dram->kal[prevSampR2] = dram->kal[prevSampR1];
		dram->kal[prevSampR1] = (dram->kal[kalGainR] * dram->kal[kalOutR]) + ((1.0f-dram->kal[kalGainR])*temp);
		//feed the chain of previous samples
		if (dram->kal[prevSampR1] > 1.0f) dram->kal[prevSampR1] = 1.0f; if (dram->kal[prevSampR1] < -1.0f) dram->kal[prevSampR1] = -1.0f;
		float stoneR = dram->kal[kalOutR]*0.777f;
		fireR -= stoneR;
		//end KalmanR
		
		//fire dynamics
		if (fabs(fireL) > compFThresh) { //compression L
			fireCompL -= (fireCompL * compFAttack);
			fireCompL += ((compFThresh / fabs(fireL))*compFAttack);
		} else fireCompL = (fireCompL*(1.0f-compFRelease))+compFRelease;
		if (fabs(fireR) > compFThresh) { //compression R
			fireCompR -= (fireCompR * compFAttack);
			fireCompR += ((compFThresh / fabs(fireR))*compFAttack);
		} else fireCompR = (fireCompR*(1.0f-compFRelease))+compFRelease;
		if (fireCompL > fireCompR) fireCompL -= (fireCompL * compFAttack);
		if (fireCompR > fireCompL) fireCompR -= (fireCompR * compFAttack);
		fireCompL = fmax(fmin(fireCompL,1.0f),0.0f);
		fireCompR = fmax(fmin(fireCompR,1.0f),0.0f);
		fireL *= (((1.0f-compRatio)*firePad)+(fireCompL*compRatio*fireGain));
		fireR *= (((1.0f-compRatio)*firePad)+(fireCompR*compRatio*fireGain));
		
		//stone dynamics
		if (fabs(stoneL) > compSThresh) { //compression L
			stoneCompL -= (stoneCompL * compSAttack);
			stoneCompL += ((compSThresh / fabs(stoneL))*compSAttack);
		} else stoneCompL = (stoneCompL*(1.0f-compSRelease))+compSRelease;
		if (fabs(stoneR) > compSThresh) { //compression R
			stoneCompR -= (stoneCompR * compSAttack);
			stoneCompR += ((compSThresh / fabs(stoneR))*compSAttack);
		} else stoneCompR = (stoneCompR*(1.0f-compSRelease))+compSRelease;
		if (stoneCompL > stoneCompR) stoneCompL -= (stoneCompL * compSAttack);
		if (stoneCompR > stoneCompL) stoneCompR -= (stoneCompR * compSAttack);
		stoneCompL = fmax(fmin(stoneCompL,1.0f),0.0f);
		stoneCompR = fmax(fmin(stoneCompR,1.0f),0.0f);
		stoneL *= (((1.0f-compRatio)*stonePad)+(stoneCompL*compRatio*stoneGain));
		stoneR *= (((1.0f-compRatio)*stonePad)+(stoneCompR*compRatio*stoneGain));
		
		inputSampleL = stoneL+fireL;
		inputSampleR = stoneR+fireR;
		
		
		
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
	for (int x = 0; x < kal_total; x++) dram->kal[x] = 0.0;
	fireCompL = 1.0;
	fireCompR = 1.0;
	stoneCompL = 1.0;
	stoneCompR = 1.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
