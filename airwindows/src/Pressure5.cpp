#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pressure5"
#define AIRWINDOWS_DESCRIPTION "Expands Pressure4 with after-boost and built-in ClipOnly2."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','r','5' )
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
{ .name = "Pressure", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mewiness", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "PawsClaws", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	Float32 muVary;
	Float32 muAttack;
	Float32 muNewSpeed;
	Float32 muSpeedA;
	Float32 muSpeedB;
	Float32 muCoefficientA;
	Float32 muCoefficientB;
	bool flip; //Pressure
	
	enum {
		fix_freq,
		fix_reso,
		fix_a0,
		fix_a1,
		fix_a2,
		fix_b1,
		fix_b2,
		fix_sL1,
		fix_sL2,
		fix_sR1,
		fix_sR2,
		fix_lastSampleL,
		fix_lastSampleR,
		fix_total
	};
	
	float lastSampleL;
	float intermediateL[16];
	bool wasPosClipL;
	bool wasNegClipL;
	float lastSampleR;
	float intermediateR[16];
	bool wasPosClipR;
	bool wasNegClipR; //Stereo ClipOnly2
	
	float slewMax; //to adust mewiness
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float fixA[fix_total];
	float fixB[fix_total]; //fixed frequency biquad filter for ultrasonics, stereo
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
		
	float threshold = 1.0f - (GetParameter( kParam_One ) * 0.95f);
	float muMakeupGain = 1.0f / threshold;
	//gain settings around threshold
	float release = powf((1.28f-GetParameter( kParam_Two )),5)*32768.0f;
	float fastest = sqrt(release);
	release /= overallscale;
	fastest /= overallscale;
	//speed settings around release
	float mewinessRef = GetParameter( kParam_Three );
	float pawsClaws = -(GetParameter( kParam_Four )-0.5f)*1.618033988749894848204586f;
	// µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
	float outputGain = powf(GetParameter( kParam_Five )*2.0f,2); //max 4.0f gain
	float wet = GetParameter( kParam_Six );
	
	dram->fixA[fix_freq] = 24000.0f / GetSampleRate();
    dram->fixA[fix_reso] = 0.7071f; //butterworth Q
	float K = tan(M_PI * dram->fixA[fix_freq]);
	float norm = 1.0f / (1.0f + K / dram->fixA[fix_reso] + K * K);
	dram->fixA[fix_a0] = K * K * norm;
	dram->fixA[fix_a1] = 2.0f * dram->fixA[fix_a0];
	dram->fixA[fix_a2] = dram->fixA[fix_a0];
	dram->fixA[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fixA[fix_b2] = (1.0f - K / dram->fixA[fix_reso] + K * K) * norm;
	//for the fixed-position biquad filter
	for (int x = 0; x < fix_sL1; x++) dram->fixB[x] = dram->fixA[x];
	//make the second filter same as the first, don't use sample slots

	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		inputSampleL = inputSampleL * muMakeupGain;
		inputSampleR = inputSampleR * muMakeupGain;
		
		if (dram->fixA[fix_freq] < 0.4999f) {
			float temp = (inputSampleL * dram->fixA[fix_a0]) + dram->fixA[fix_sL1];
			dram->fixA[fix_sL1] = (inputSampleL * dram->fixA[fix_a1]) - (temp * dram->fixA[fix_b1]) + dram->fixA[fix_sL2];
			dram->fixA[fix_sL2] = (inputSampleL * dram->fixA[fix_a2]) - (temp * dram->fixA[fix_b2]);
			inputSampleL = temp;
			temp = (inputSampleR * dram->fixA[fix_a0]) + dram->fixA[fix_sR1];
			dram->fixA[fix_sR1] = (inputSampleR * dram->fixA[fix_a1]) - (temp * dram->fixA[fix_b1]) + dram->fixA[fix_sR2];
			dram->fixA[fix_sR2] = (inputSampleR * dram->fixA[fix_a2]) - (temp * dram->fixA[fix_b2]);
			inputSampleR = temp; //fixed biquad filtering ultrasonics before Pressure
		}
		
		float inputSense = fabs(inputSampleL);
		if (fabs(inputSampleR) > inputSense)
			inputSense = fabs(inputSampleR);
		//we will take the greater of either channel and just use that, then apply the result
		//to both stereo channels.
		
		float mewiness = sin(mewinessRef + (slewMax * pawsClaws));
		bool positivemu = true; if (mewiness < 0) {positivemu = false; mewiness = -mewiness;}
		
		if (flip)
		{
			if (inputSense > threshold)
			{
				muVary = threshold / inputSense;
				muAttack = sqrt(fabs(muSpeedA));
				muCoefficientA = muCoefficientA * (muAttack-1.0f);
				if (muVary < threshold)
				{
					muCoefficientA = muCoefficientA + threshold;
				}
				else
				{
					muCoefficientA = muCoefficientA + muVary;
				}
				muCoefficientA = muCoefficientA / muAttack;
			}
			else
			{
				muCoefficientA = muCoefficientA * ((muSpeedA * muSpeedA)-1.0f);
				muCoefficientA = muCoefficientA + 1.0f;
				muCoefficientA = muCoefficientA / (muSpeedA * muSpeedA);
			}
			muNewSpeed = muSpeedA * (muSpeedA-1);
			muNewSpeed = muNewSpeed + fabs(inputSense*release)+fastest;
			muSpeedA = muNewSpeed / muSpeedA;
		}
		else
		{
			if (inputSense > threshold)
			{
				muVary = threshold / inputSense;
				muAttack = sqrt(fabs(muSpeedB));
				muCoefficientB = muCoefficientB * (muAttack-1);
				if (muVary < threshold)
				{
					muCoefficientB = muCoefficientB + threshold;
				}
				else
				{
					muCoefficientB = muCoefficientB + muVary;
				}
				muCoefficientB = muCoefficientB / muAttack;
			}
			else
			{
				muCoefficientB = muCoefficientB * ((muSpeedB * muSpeedB)-1.0f);
				muCoefficientB = muCoefficientB + 1.0f;
				muCoefficientB = muCoefficientB / (muSpeedB * muSpeedB);
			}
			muNewSpeed = muSpeedB * (muSpeedB-1);
			muNewSpeed = muNewSpeed + fabs(inputSense*release)+fastest;
			muSpeedB = muNewSpeed / muSpeedB;
		}
		//got coefficients, adjusted speeds
		
		float coefficient;
		if (flip) {
			if (positivemu) coefficient = powf(muCoefficientA,2);
			else coefficient = sqrt(muCoefficientA);
			coefficient = (coefficient*mewiness)+(muCoefficientA*(1.0f-mewiness));
			inputSampleL *= coefficient;
			inputSampleR *= coefficient;
		} else {
			if (positivemu) coefficient = powf(muCoefficientB,2);
			else coefficient = sqrt(muCoefficientB);
			coefficient = (coefficient*mewiness)+(muCoefficientB*(1.0f-mewiness));
			inputSampleL *= coefficient;
			inputSampleR *= coefficient;
		}
		//applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		//applied gain correction to control output level- tends to constrain sound rather than inflate it
		
		if (outputGain != 1.0f) {
			inputSampleL *= outputGain;
			inputSampleR *= outputGain;
		}		
		
		flip = !flip;
				
		if (dram->fixB[fix_freq] < 0.49999f) {
			float temp = (inputSampleL * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
			dram->fixB[fix_sL1] = (inputSampleL * dram->fixB[fix_a1]) - (temp * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
			dram->fixB[fix_sL2] = (inputSampleL * dram->fixB[fix_a2]) - (temp * dram->fixB[fix_b2]);
			inputSampleL = temp;
			temp = (inputSampleR * dram->fixB[fix_a0]) + dram->fixB[fix_sR1];
			dram->fixB[fix_sR1] = (inputSampleR * dram->fixB[fix_a1]) - (temp * dram->fixB[fix_b1]) + dram->fixB[fix_sR2];
			dram->fixB[fix_sR2] = (inputSampleR * dram->fixB[fix_a2]) - (temp * dram->fixB[fix_b2]);
			inputSampleR = temp; //fixed biquad filtering ultrasonics between Pressure and ClipOnly
		}
		
		if (wet != 1.0f) {
		 inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
		 inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f-wet));
		}
		//Dry/Wet control, BEFORE ClipOnly
		
		slewMax = fabs(inputSampleL - lastSampleL);
		if (slewMax < fabs(inputSampleR - lastSampleR)) slewMax = fabs(inputSampleR - lastSampleR);
		//set up for fiddling with mewiness. Largest amount of slew in any direction
		
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
		//final clip runs AFTER the Dry/Wet. It serves as a safety clip even if you're not full wet

		
		
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
	muSpeedA = 10000;
	muSpeedB = 10000;
	muCoefficientA = 1;
	muCoefficientB = 1;
	muVary = 1;
	flip = false;
	for (int x = 0; x < fix_total; x++) {dram->fixA[x] = 0.0; dram->fixB[x] = 0.0;}
	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	lastSampleR = 0.0;
	wasPosClipR = false;
	wasNegClipR = false;
	for (int x = 0; x < 16; x++) {intermediateL[x] = 0.0; intermediateR[x] = 0.0;}
	slewMax = 0.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
