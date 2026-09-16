#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ToTape7"
#define AIRWINDOWS_DESCRIPTION "Airwindows tape emulation with Bias and Dubly."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','o','7' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	kParam_H =7,
	kParam_I =8,
	kParam_J =9,
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
{ .name = "EncAmt", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "EncFreq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "TapeDrv", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Flutter", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FlutSpd", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bias", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HeadBmp", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HeadFrq", .min = 2500, .max = 20000, .def = 5000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "DecAmt", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DecFreq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	
	float iirEncL;
	float iirEncR;

	float sweepL;
	float sweepR;
	float nextmaxL;
	float nextmaxR;
	int gcount;		
	
	enum {
		prevSampL1,
		prevSampR1,
		threshold1,
		prevSampL2,
		prevSampR2,
		threshold2,
		prevSampL3,
		prevSampR3,
		threshold3,
		prevSampL4,
		prevSampR4,
		threshold4,
		prevSampL5,
		prevSampR5,
		threshold5,
		prevSampL6,
		prevSampR6,
		threshold6,
		prevSampL7,
		prevSampR7,
		threshold7,
		prevSampL8,
		prevSampR8,
		threshold8,
		prevSampL9,
		prevSampR9,
		threshold9,
		gslew_total
	}; //fixed frequency pear filter for ultrasonics, stereo
	
	float iirMidRollerL;
	float iirLowCutoffL;
	float iirMidRollerR;
	float iirLowCutoffR;
	
	float headBumpL;
	float headBumpR;
	enum {
		hdb_freq,
		hdb_reso,
		hdb_a0,
		hdb_a1,
		hdb_a2,
		hdb_b1,
		hdb_b2,
		hdb_sL1,
		hdb_sL2,
		hdb_sR1,
		hdb_sR2,
		hdb_total
	}; //fixed frequency biquad filter for ultrasonics, stereo
	
	float iirDecL;
	float iirDecR;
	
	float lastSampleL;
	float intermediateL[16];
	bool wasPosClipL;
	bool wasNegClipL;
	float lastSampleR;
	float intermediateR[16];
	bool wasPosClipR;
	bool wasNegClipR; //Stereo ClipOnly2	
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float dL[1002];
	float dR[1002];
	float gslew[gslew_total]; //end bias	
	float hdbA[hdb_total];
	float hdbB[hdb_total];
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

	float dublyAmount = powf(GetParameter( kParam_A ),3)*0.105468f;
	float iirEncFreq = GetParameter( kParam_B )/overallscale;
	float iirMidFreq = ((GetParameter( kParam_B ) * 0.618f) + 0.382f)/overallscale;
	float inputGain = powf(GetParameter( kParam_C )*2.0f,2.0f);
	float flutDepth = powf(GetParameter( kParam_D ),6)*overallscale*50;
	if (flutDepth > 498.0f) flutDepth = 498.0f;
	float flutFrequency = (0.02f*powf(GetParameter( kParam_E ),3))/overallscale;
	float bias = (GetParameter( kParam_F )*2.0f)-1.0f;
	float underBias = (powf(bias,4)*0.25f)/overallscale;
	float overBias = powf(1.0f-bias,3)/overallscale;
	if (bias > 0.0f) underBias = 0.0f;
	if (bias < 0.0f) overBias = 1.0f/overallscale;
	
	dram->gslew[threshold9] = overBias;
	overBias *= 1.618033988749894848204586f;
	dram->gslew[threshold8] = overBias;
	overBias *= 1.618033988749894848204586f;
	dram->gslew[threshold7] = overBias;
	overBias *= 1.618033988749894848204586f;
	dram->gslew[threshold6] = overBias;
	overBias *= 1.618033988749894848204586f;
	dram->gslew[threshold5] = overBias;
	overBias *= 1.618033988749894848204586f;
	dram->gslew[threshold4] = overBias;
	overBias *= 1.618033988749894848204586f;
	dram->gslew[threshold3] = overBias;
	overBias *= 1.618033988749894848204586f;
	dram->gslew[threshold2] = overBias;
	overBias *= 1.618033988749894848204586f;
	dram->gslew[threshold1] = overBias;
	overBias *= 1.618033988749894848204586f;
	
	float headBumpDrive = (GetParameter( kParam_G )*0.1f)/overallscale;
	float headBumpMix = GetParameter( kParam_G )*0.5f;
	
	dram->hdbA[hdb_freq] = GetParameter( kParam_H )/GetSampleRate();
	dram->hdbB[hdb_freq] = dram->hdbA[hdb_freq]*0.9375f;
	dram->hdbB[hdb_reso] = dram->hdbA[hdb_reso] = 0.618033988749894848204586f;
	dram->hdbB[hdb_a1] = dram->hdbA[hdb_a1] = 0.0f;
	
	float K = tan(M_PI * dram->hdbA[hdb_freq]);
	float norm = 1.0f / (1.0f + K / dram->hdbA[hdb_reso] + K * K);
	dram->hdbA[hdb_a0] = K / dram->hdbA[hdb_reso] * norm;
	dram->hdbA[hdb_a2] = -dram->hdbA[hdb_a0];
	dram->hdbA[hdb_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hdbA[hdb_b2] = (1.0f - K / dram->hdbA[hdb_reso] + K * K) * norm;
	K = tan(M_PI * dram->hdbB[hdb_freq]);
	norm = 1.0f / (1.0f + K / dram->hdbB[hdb_reso] + K * K);
	dram->hdbB[hdb_a0] = K / dram->hdbB[hdb_reso] * norm;
	dram->hdbB[hdb_a2] = -dram->hdbB[hdb_a0];
	dram->hdbB[hdb_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hdbB[hdb_b2] = (1.0f - K / dram->hdbB[hdb_reso] + K * K) * norm;

	float outlyAmount = powf(GetParameter( kParam_I ),3)*0.109744f;
	float iirDecFreq = GetParameter( kParam_J )/overallscale;
	float subCurve = sin(GetParameter( kParam_G )*M_PI);
	float iirSubFreq = (subCurve*0.008f)/overallscale;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		//begin Dubly encode
		float doubly = 0.0f;
		if (dublyAmount > 0.0f) {
			iirEncL = (iirEncL * (1.0f - iirEncFreq)) + (inputSampleL * iirEncFreq);
			float doubly = inputSampleL - iirEncL;
			if (doubly > 1.0f) doubly = 1.0f; if (doubly < -1.0f) doubly = -1.0f;
			if (doubly > 0) doubly = log(1.0f+(255*fabs(doubly)))/2.40823996531f;
			if (doubly < 0) doubly = -log(1.0f+(255*fabs(doubly)))/2.40823996531f;
			inputSampleL += doubly*dublyAmount;
			iirEncR = (iirEncR * (1.0f - iirEncFreq)) + (inputSampleR * iirEncFreq);
			doubly = inputSampleR - iirEncR;
			if (doubly > 1.0f) doubly = 1.0f; if (doubly < -1.0f) doubly = -1.0f;
			if (doubly > 0) doubly = log(1.0f+(255*fabs(doubly)))/2.40823996531f;
			if (doubly < 0) doubly = -log(1.0f+(255*fabs(doubly)))/2.40823996531f;
			inputSampleR += doubly*dublyAmount;
		}
		//end Dubly encode
		
		if (inputGain != 1.0f) {
			inputSampleL *= inputGain;
			inputSampleR *= inputGain;
		}
		
		//begin Flutter
		if (flutDepth > 0.0f) {
			if (gcount < 0 || gcount > 999) gcount = 999;
			dram->dL[gcount] = inputSampleL;
			int count = gcount;
			float offset = flutDepth + (flutDepth * sin(sweepL));
			sweepL += nextmaxL * flutFrequency;
			if (sweepL > (M_PI*2.0f)) {
				sweepL -= M_PI*2.0f;
				float flutA = 0.24f + (fpdL / (float)UINT32_MAX * 0.74f);
				fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
				float flutB = 0.24f + (fpdL / (float)UINT32_MAX * 0.74f);
				if (fabs(flutA-sin(sweepR+nextmaxR))<fabs(flutB-sin(sweepR+nextmaxR))) nextmaxL = flutA; else nextmaxL = flutB;
			}
			count += (int)floor(offset);
			inputSampleL = (dram->dL[count-((count > 999)?1000:0)] * (1-(offset-floor(offset))));
			inputSampleL += (dram->dL[count+1-((count+1 > 999)?1000:0)] * (offset-floor(offset)));
			dram->dR[gcount] = inputSampleR;
			count = gcount;
			offset = flutDepth + (flutDepth * sin(sweepR));
			sweepR += nextmaxR * flutFrequency;
			if (sweepR > (M_PI*2.0f)) {
				sweepR -= M_PI*2.0f;
				float flutA = 0.24f + (fpdR / (float)UINT32_MAX * 0.74f);
				fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
				float flutB = 0.24f + (fpdR / (float)UINT32_MAX * 0.74f);
				if (fabs(flutA-sin(sweepL+nextmaxL))<fabs(flutB-sin(sweepL+nextmaxL))) nextmaxR = flutA; else nextmaxR = flutB;
			}
			count += (int)floor(offset);
			inputSampleR = (dram->dR[count-((count > 999)?1000:0)] * (1-(offset-floor(offset))));
			inputSampleR += (dram->dR[count+1-((count+1 > 999)?1000:0)] * (offset-floor(offset)));
			gcount--;
		}
		//end Flutter
		
		//start bias routine
		if (fabs(bias) > 0.001f) {
			for (int x = 0; x < gslew_total; x += 3) {
				if (underBias > 0.0f) {
					float stuck = fabs(inputSampleL - (dram->gslew[x]/0.975f)) / underBias;
					if (stuck < 1.0f) inputSampleL = (inputSampleL * stuck) + ((dram->gslew[x]/0.975f)*(1.0f-stuck));
					stuck =  fabs(inputSampleR - (dram->gslew[x+1]/0.975f)) / underBias;
					if (stuck < 1.0f) inputSampleR = (inputSampleR * stuck) + ((dram->gslew[x+1]/0.975f)*(1.0f-stuck));
				}
				if ((inputSampleL - dram->gslew[x]) > dram->gslew[x+2]) inputSampleL = dram->gslew[x] + dram->gslew[x+2];
				if (-(inputSampleL - dram->gslew[x]) > dram->gslew[x+2]) inputSampleL = dram->gslew[x] - dram->gslew[x+2];
				dram->gslew[x] = inputSampleL * 0.975f;
				if ((inputSampleR - dram->gslew[x+1]) > dram->gslew[x+2]) inputSampleR = dram->gslew[x+1] + dram->gslew[x+2];
				if (-(inputSampleR - dram->gslew[x+1]) > dram->gslew[x+2]) inputSampleR = dram->gslew[x+1] - dram->gslew[x+2];
				dram->gslew[x+1] = inputSampleR * 0.975f;
			}
		}
		//end bias routine
		
		//toTape basic algorithm L
		iirMidRollerL = (iirMidRollerL * (1.0f-iirMidFreq)) + (inputSampleL*iirMidFreq);
		float HighsSampleL = inputSampleL - iirMidRollerL;
		float LowsSampleL = iirMidRollerL;
		if (iirSubFreq > 0.0f) {
			iirLowCutoffL = (iirLowCutoffL * (1.0f-iirSubFreq)) + (LowsSampleL*iirSubFreq);
			LowsSampleL -= iirLowCutoffL;
		}
		if (LowsSampleL > 1.57079633f) LowsSampleL = 1.57079633f;
		if (LowsSampleL < -1.57079633f) LowsSampleL = -1.57079633f;
		LowsSampleL = sin(LowsSampleL);
		float thinnedHighSample = fabs(HighsSampleL)*1.57079633f;
		if (thinnedHighSample > 1.57079633f) thinnedHighSample = 1.57079633f;
		thinnedHighSample = 1.0f-cos(thinnedHighSample);
		if (HighsSampleL < 0) thinnedHighSample = -thinnedHighSample;
		HighsSampleL -= thinnedHighSample;
		
		//toTape basic algorithm R
		iirMidRollerR = (iirMidRollerR * (1.0f-iirMidFreq)) + (inputSampleR*iirMidFreq);
		float HighsSampleR = inputSampleR - iirMidRollerR;
		float LowsSampleR = iirMidRollerR;
		if (iirSubFreq > 0.0f) {
			iirLowCutoffR = (iirLowCutoffR * (1.0f-iirSubFreq)) + (LowsSampleR*iirSubFreq);
			LowsSampleR -= iirLowCutoffR;
		}
		if (LowsSampleR > 1.57079633f) LowsSampleR = 1.57079633f;
		if (LowsSampleR < -1.57079633f) LowsSampleR = -1.57079633f;
		LowsSampleR = sin(LowsSampleR);
		thinnedHighSample = fabs(HighsSampleR)*1.57079633f;
		if (thinnedHighSample > 1.57079633f) thinnedHighSample = 1.57079633f;
		thinnedHighSample = 1.0f-cos(thinnedHighSample);
		if (HighsSampleR < 0) thinnedHighSample = -thinnedHighSample;
		HighsSampleR -= thinnedHighSample;
		
		//begin HeadBump
		float headBumpSampleL = 0.0f;
		float headBumpSampleR = 0.0f;
		if (headBumpMix > 0.0f) {
			headBumpL += (LowsSampleL * headBumpDrive);
			headBumpL -= (headBumpL * headBumpL * headBumpL * (0.0618f/sqrt(overallscale)));
			headBumpR += (LowsSampleR * headBumpDrive);
			headBumpR -= (headBumpR * headBumpR * headBumpR * (0.0618f/sqrt(overallscale)));
			float headBiqSampleL = (headBumpL * dram->hdbA[hdb_a0]) + dram->hdbA[hdb_sL1];
			dram->hdbA[hdb_sL1] = (headBumpL * dram->hdbA[hdb_a1]) - (headBiqSampleL * dram->hdbA[hdb_b1]) + dram->hdbA[hdb_sL2];
			dram->hdbA[hdb_sL2] = (headBumpL * dram->hdbA[hdb_a2]) - (headBiqSampleL * dram->hdbA[hdb_b2]);
			headBumpSampleL = (headBiqSampleL * dram->hdbB[hdb_a0]) + dram->hdbB[hdb_sL1];
			dram->hdbB[hdb_sL1] = (headBiqSampleL * dram->hdbB[hdb_a1]) - (headBumpSampleL * dram->hdbB[hdb_b1]) + dram->hdbB[hdb_sL2];
			dram->hdbB[hdb_sL2] = (headBiqSampleL * dram->hdbB[hdb_a2]) - (headBumpSampleL * dram->hdbB[hdb_b2]);
			float headBiqSampleR = (headBumpR * dram->hdbA[hdb_a0]) + dram->hdbA[hdb_sR1];
			dram->hdbA[hdb_sR1] = (headBumpR * dram->hdbA[hdb_a1]) - (headBiqSampleR * dram->hdbA[hdb_b1]) + dram->hdbA[hdb_sR2];
			dram->hdbA[hdb_sR2] = (headBumpR * dram->hdbA[hdb_a2]) - (headBiqSampleR * dram->hdbA[hdb_b2]);
			headBumpSampleR = (headBiqSampleR * dram->hdbB[hdb_a0]) + dram->hdbB[hdb_sR1];
			dram->hdbB[hdb_sR1] = (headBiqSampleR * dram->hdbB[hdb_a1]) - (headBumpSampleR * dram->hdbB[hdb_b1]) + dram->hdbB[hdb_sR2];
			dram->hdbB[hdb_sR2] = (headBiqSampleR * dram->hdbB[hdb_a2]) - (headBumpSampleR * dram->hdbB[hdb_b2]);
		}
		//end HeadBump
		
		inputSampleL = LowsSampleL + HighsSampleL + (headBumpSampleL * headBumpMix);
		inputSampleR = LowsSampleR + HighsSampleR + (headBumpSampleR * headBumpMix);
		
		//begin Dubly decode
		if (outlyAmount > 0.0f) {
			iirDecL = (iirDecL * (1.0f - iirDecFreq)) + (inputSampleL * iirDecFreq);
			doubly = inputSampleL - iirDecL;
			if (doubly > 1.0f) doubly = 1.0f; if (doubly < -1.0f) doubly = -1.0f;
			if (doubly > 0) doubly = log(1.0f+(255*fabs(doubly)))/2.40823996531f;
			if (doubly < 0) doubly = -log(1.0f+(255*fabs(doubly)))/2.40823996531f;
			inputSampleL -= doubly*outlyAmount;
			iirDecR = (iirDecR * (1.0f - iirDecFreq)) + (inputSampleR * iirDecFreq);
			doubly = inputSampleR - iirDecR;
			if (doubly > 1.0f) doubly = 1.0f; if (doubly < -1.0f) doubly = -1.0f;
			if (doubly > 0) doubly = log(1.0f+(255*fabs(doubly)))/2.40823996531f;
			if (doubly < 0) doubly = -log(1.0f+(255*fabs(doubly)))/2.40823996531f;
			inputSampleR -= doubly*outlyAmount;
		}
		//end Dubly decode

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
	iirEncL = 0.0;
	iirEncR = 0.0;
	
	for (int temp = 0; temp < 1001; temp++) {dram->dL[temp] = 0.0;dram->dR[temp] = 0.0;}
	sweepL = M_PI;
	sweepR = M_PI;
	nextmaxL = 0.5;	
	nextmaxR = 0.5;
	gcount = 0;	
	
	for (int x = 0; x < gslew_total; x++) dram->gslew[x] = 0.0;

	iirMidRollerL = 0.0;
	iirLowCutoffL = 0.0;
	iirMidRollerR = 0.0;
	iirLowCutoffR = 0.0;

	headBumpL = 0.0;
	headBumpR = 0.0;
	for (int x = 0; x < hdb_total; x++) {dram->hdbA[x] = 0.0;dram->hdbB[x] = 0.0;}
	//from ZBandpass, so I can use enums with it	
	
	iirDecL = 0.0;
	iirDecR = 0.0;
	
	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	lastSampleR = 0.0;
	wasPosClipR = false;
	wasNegClipR = false;
	for (int x = 0; x < 16; x++) {intermediateL[x] = 0.0; intermediateR[x] = 0.0;}
	//this is reset: values being initialized only once. Startup values, whatever they are.
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
