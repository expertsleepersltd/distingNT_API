#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ToTape9"
#define AIRWINDOWS_DESCRIPTION "Airwindows tape emulation and brightness compression."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','o','9' )
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
	//Add your parameters here...
	kNumberOfParameters=9
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Input", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tilt", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Shape", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Flutter", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FlutSpd", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bias", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HeadBmp", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HeadFrq", .min = 2500, .max = 20000, .def = 5000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	
	float iirEncL;
	float iirDecL;
	float compEncL;
	float compDecL;
	float avgEncL;
	float avgDecL;
	float iirEncR;
	float iirDecR;
	float compEncR;
	float compDecR;
	float avgEncR;
	float avgDecR;
	
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
	
	float hysteresisL;
	float hysteresisR;
	
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
	
	float avg8L[9];
	float avg8R[9];
	float avg4L[5];
	float avg4R[5];
	float avg2L[3];
	float avg2R[3];
	float post8L[9];
	float post8R[9];
	float post4L[5];
	float post4R[5];
	float post2L[3];
	float post2R[3];
	float lastDarkL;
	float lastDarkR;
	int avgPos;
	//preTapeHack	
	
	float lastSampleL;
	bool wasPosClipL;
	bool wasNegClipL;
	float lastSampleR;
	bool wasPosClipR;
	bool wasNegClipR; //Stereo ClipOnly3
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float dL[1002];
	float dR[1002];
	float gslew[gslew_total]; //end bias
	float hdbA[hdb_total];
	float hdbB[hdb_total];
	float avg32L[33];
	float avg32R[33];
	float avg16L[17];
	float avg16R[17];
	float post32L[33];
	float post32R[33];
	float post16L[17];
	float post16R[17];
	float intermediateL[18];
	float slewL[34];
	float intermediateR[18];
	float slewR[34];
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
	int slewsing = floor(overallscale*2.0f);
	if (slewsing < 2) slewsing = 2; if (slewsing > 32) slewsing = 32;

	float inputGain = powf(GetParameter( kParam_A )*2.0f,2.0f);
	
	float dublyAmount = GetParameter( kParam_B )*2.0f;
	float outlyAmount = (1.0f-GetParameter( kParam_B ))*-2.0f;
	if (outlyAmount < -1.0f) outlyAmount = -1.0f;
	float iirEncFreq = (1.0f-GetParameter( kParam_C ))/overallscale;
	float iirDecFreq = GetParameter( kParam_C )/overallscale;
	
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
	
	float outputGain = GetParameter( kParam_I )*2.0f;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		if (inputGain != 1.0f) {
			inputSampleL *= inputGain;
			inputSampleR *= inputGain;
		}
		
		//Dubly encode
		iirEncL = (iirEncL * (1.0f - iirEncFreq)) + (inputSampleL * iirEncFreq);
		float highPart = ((inputSampleL-iirEncL)*2.848f);
		highPart += avgEncL; avgEncL = (inputSampleL-iirEncL)*1.152f;
		if (highPart > 1.0f) highPart = 1.0f; if (highPart < -1.0f) highPart = -1.0f;
		float dubly = fabs(highPart);
		if (dubly > 0.0f) {
			float adjust = log(1.0f+(255.0f*dubly))/2.40823996531f;
			if (adjust > 0.0f) dubly /= adjust;
			compEncL = (compEncL*(1.0f-iirEncFreq))+(dubly*iirEncFreq);
			inputSampleL += ((highPart*compEncL)*dublyAmount);
		} //end Dubly encode L
		iirEncR = (iirEncR * (1.0f - iirEncFreq)) + (inputSampleR * iirEncFreq);
		highPart = ((inputSampleR-iirEncR)*2.848f);
		highPart += avgEncR; avgEncR = (inputSampleR-iirEncR)*1.152f;
		if (highPart > 1.0f) highPart = 1.0f; if (highPart < -1.0f) highPart = -1.0f;
		dubly = fabs(highPart);
		if (dubly > 0.0f) {
			float adjust = log(1.0f+(255.0f*dubly))/2.40823996531f;
			if (adjust > 0.0f) dubly /= adjust;
			compEncR = (compEncR*(1.0f-iirEncFreq))+(dubly*iirEncFreq);
			inputSampleR += ((highPart*compEncR)*dublyAmount);
		} //end Dubly encode R
		
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
		
		//begin tiny hysteresis
		float applyHysteresis = (1.0f-fabs(inputSampleL))*(1.0f-fabs(inputSampleL))*0.012f;
		hysteresisL = fmax(fmin(hysteresisL+((inputSampleL*fabs(inputSampleL))),0.011449f),-0.011449f)*0.999f;
		inputSampleL += (hysteresisL*applyHysteresis);
		applyHysteresis = (1.0f-fabs(inputSampleR))*(1.0f-fabs(inputSampleR))*0.012f;
		hysteresisR = fmax(fmin(hysteresisR+((inputSampleR*fabs(inputSampleR))),0.011449f),-0.011449f)*0.999f;
		inputSampleR += (hysteresisR*applyHysteresis);
		
		//begin TapeHack2
		float darkSampleL = inputSampleL;
		float darkSampleR = inputSampleR;
		if (avgPos > 31) avgPos = 0;
		if (slewsing > 31) {
			dram->avg32L[avgPos] = darkSampleL; dram->avg32R[avgPos] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 32; x++) {darkSampleL += dram->avg32L[x]; darkSampleR += dram->avg32R[x];}
			darkSampleL /= 32.0f; darkSampleR /= 32.0f;
		} if (slewsing > 15) {
			dram->avg16L[avgPos%16] = darkSampleL; dram->avg16R[avgPos%16] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 16; x++) {darkSampleL += dram->avg16L[x]; darkSampleR += dram->avg16R[x];}
			darkSampleL /= 16.0f; darkSampleR /= 16.0f;
		} if (slewsing > 7) {
			avg8L[avgPos%8] = darkSampleL; avg8R[avgPos%8] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 8; x++) {darkSampleL += avg8L[x]; darkSampleR += avg8R[x];}
			darkSampleL /= 8.0f; darkSampleR /= 8.0f;
		} if (slewsing > 3) {
			avg4L[avgPos%4] = darkSampleL; avg4R[avgPos%4] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 4; x++) {darkSampleL += avg4L[x]; darkSampleR += avg4R[x];}
			darkSampleL /= 4.0f; darkSampleR /= 4.0f;
		} if (slewsing > 1) {
			avg2L[avgPos%2] = darkSampleL; avg2R[avgPos%2] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 2; x++) {darkSampleL += avg2L[x]; darkSampleR += avg2R[x];}
			darkSampleL /= 2.0f; darkSampleR /= 2.0f; 
		} //only update avgPos after the post-distortion filter stage
		float avgSlewL = fmin(fabs(lastDarkL-inputSampleL)*0.12f*overallscale,1.0f);
		avgSlewL = 1.0f-(1.0f-avgSlewL*1.0f-avgSlewL);
		inputSampleL = (inputSampleL*(1.0f-avgSlewL)) + (darkSampleL*avgSlewL);
		lastDarkL = darkSampleL;
		float avgSlewR = fmin(fabs(lastDarkR-inputSampleR)*0.12f*overallscale,1.0f);
		avgSlewR = 1.0f-(1.0f-avgSlewR*1.0f-avgSlewR);
		inputSampleR = (inputSampleR*(1.0f-avgSlewR)) + (darkSampleR*avgSlewR);
		lastDarkR = darkSampleR;
				
		//begin TapeHack
		inputSampleL = fmax(fmin(inputSampleL,2.305929007734908f),-2.305929007734908f);
		float addtwo = inputSampleL * inputSampleL;
		float empower = inputSampleL * addtwo; // inputSample to the third power
		inputSampleL -= (empower / 6.0f);
		empower *= addtwo; // to the fifth power
		inputSampleL += (empower / 69.0f);
		empower *= addtwo; //seventh
		inputSampleL -= (empower / 2530.08f);
		empower *= addtwo; //ninth
		inputSampleL += (empower / 224985.6f);
		empower *= addtwo; //eleventh
		inputSampleL -= (empower / 9979200.0f);
		//this is a degenerate form of a Taylor Series to approximate sin()
		
		inputSampleR = fmax(fmin(inputSampleR,2.305929007734908f),-2.305929007734908f);
		addtwo = inputSampleR * inputSampleR;
		empower = inputSampleR * addtwo; // inputSample to the third power
		inputSampleR -= (empower / 6.0f);
		empower *= addtwo; // to the fifth power
		inputSampleR += (empower / 69.0f);
		empower *= addtwo; //seventh
		inputSampleR -= (empower / 2530.08f);
		empower *= addtwo; //ninth
		inputSampleR += (empower / 224985.6f);
		empower *= addtwo; //eleventh
		inputSampleR -= (empower / 9979200.0f);
		//this is a degenerate form of a Taylor Series to approximate sin()
		
		darkSampleL = inputSampleL;
		darkSampleR = inputSampleR;
		if (avgPos > 31) avgPos = 0;
		if (slewsing > 31) {
			dram->post32L[avgPos] = darkSampleL; dram->post32R[avgPos] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 32; x++) {darkSampleL += dram->post32L[x]; darkSampleR += dram->post32R[x];}
			darkSampleL /= 32.0f; darkSampleR /= 32.0f;
		} if (slewsing > 15) {
			dram->post16L[avgPos%16] = darkSampleL; dram->post16R[avgPos%16] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 16; x++) {darkSampleL += dram->post16L[x]; darkSampleR += dram->post16R[x];}
			darkSampleL /= 16.0f; darkSampleR /= 16.0f;
		} if (slewsing > 7) {
			post8L[avgPos%8] = darkSampleL; post8R[avgPos%8] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 8; x++) {darkSampleL += post8L[x]; darkSampleR += post8R[x];}
			darkSampleL /= 8.0f; darkSampleR /= 8.0f;
		} if (slewsing > 3) {
			post4L[avgPos%4] = darkSampleL; post4R[avgPos%4] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 4; x++) {darkSampleL += post4L[x]; darkSampleR += post4R[x];}
			darkSampleL /= 4.0f; darkSampleR /= 4.0f;
		} if (slewsing > 1) {
			post2L[avgPos%2] = darkSampleL; post2R[avgPos%2] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 2; x++) {darkSampleL += post2L[x]; darkSampleR += post2R[x];}
			darkSampleL /= 2.0f; darkSampleR /= 2.0f; 
		} avgPos++;
		inputSampleL = (inputSampleL*(1.0f-avgSlewL)) + (darkSampleL*avgSlewL);
		inputSampleR = (inputSampleR*(1.0f-avgSlewR)) + (darkSampleR*avgSlewR);
		//use the previously calculated depth of the filter
		
		//begin HeadBump
		float headBumpSampleL = 0.0f;
		float headBumpSampleR = 0.0f;
		if (headBumpMix > 0.0f) {
			headBumpL += (inputSampleL * headBumpDrive);
			headBumpL -= (headBumpL * headBumpL * headBumpL * (0.0618f/sqrt(overallscale)));
			headBumpR += (inputSampleR * headBumpDrive);
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
		
		inputSampleL += (headBumpSampleL * headBumpMix);
		inputSampleR += (headBumpSampleR * headBumpMix);
		
		//Dubly decode
		iirDecL = (iirDecL * (1.0f - iirDecFreq)) + (inputSampleL * iirDecFreq);
		highPart = ((inputSampleL-iirDecL)*2.628f);
		highPart += avgDecL; avgDecL = (inputSampleL-iirDecL)*1.372f;
		if (highPart > 1.0f) highPart = 1.0f; if (highPart < -1.0f) highPart = -1.0f;
		dubly = fabs(highPart);
		if (dubly > 0.0f) {
			float adjust = log(1.0f+(255.0f*dubly))/2.40823996531f;
			if (adjust > 0.0f) dubly /= adjust;
			compDecL = (compDecL*(1.0f-iirDecFreq))+(dubly*iirDecFreq);
			inputSampleL += ((highPart*compDecL)*outlyAmount);
		} //end Dubly decode L
		iirDecR = (iirDecR * (1.0f - iirDecFreq)) + (inputSampleR * iirDecFreq);
		highPart = ((inputSampleR-iirDecR)*2.628f);
		highPart += avgDecR; avgDecR = (inputSampleR-iirDecR)*1.372f;
		if (highPart > 1.0f) highPart = 1.0f; if (highPart < -1.0f) highPart = -1.0f;
		dubly = fabs(highPart);
		if (dubly > 0.0f) {
			float adjust = log(1.0f+(255.0f*dubly))/2.40823996531f;
			if (adjust > 0.0f) dubly /= adjust;
			compDecR = (compDecR*(1.0f-iirDecFreq))+(dubly*iirDecFreq);
			inputSampleR += ((highPart*compDecR)*outlyAmount);
		} //end Dubly decode R
		
		
		if (outputGain != 1.0f) {
			inputSampleL *= outputGain;
			inputSampleR *= outputGain;
		}
		
		//begin ClipOnly3 as a little, compressed chunk that can be dropped into code
		float noise = 1.0f-((float(fpdL)/UINT32_MAX)*0.076f);
		if (wasPosClipL == true) { //current will be over
			if (inputSampleL<lastSampleL) lastSampleL=(0.9085097f*noise)+(inputSampleL*(1.0f-noise));
			else lastSampleL = 0.94f; //~-0.2dB to nearly match ClipOnly and ClipOnly2
		} wasPosClipL = false;
		if (inputSampleL>0.9085097f) {wasPosClipL=true;inputSampleL=(0.9085097f*noise)+(lastSampleL*(1.0f-noise));}
		if (wasNegClipL == true) { //current will be -over
			if (inputSampleL > lastSampleL) lastSampleL=(-0.9085097f*noise)+(inputSampleL*(1.0f-noise));
			else lastSampleL = -0.94f;
		} wasNegClipL = false;
		if (inputSampleL<-0.9085097f) {wasNegClipL=true;inputSampleL=(-0.9085097f*noise)+(lastSampleL*(1.0f-noise));}
		dram->slewL[spacing*2] = fabs(lastSampleL-inputSampleL);
		for (int x = spacing*2; x > 0; x--) dram->slewL[x-1] = dram->slewL[x];
		dram->intermediateL[spacing] = inputSampleL; inputSampleL = lastSampleL;
		//latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) {dram->intermediateL[x-1] = dram->intermediateL[x];} lastSampleL = dram->intermediateL[0];
		if (wasPosClipL || wasNegClipL) {
			for (int x = spacing; x > 0; x--) lastSampleL += dram->intermediateL[x];
			lastSampleL /= spacing;
		} float finalSlew = 0.0f;
		for (int x = spacing*2; x >= 0; x--) if (finalSlew < dram->slewL[x]) finalSlew = dram->slewL[x];
		float postclip = 0.94f / (1.0f+(finalSlew*1.3986013f));
		if (inputSampleL > postclip) inputSampleL = postclip; if (inputSampleL < -postclip) inputSampleL = -postclip;
		
		noise = 1.0f-((float(fpdR)/UINT32_MAX)*0.076f);
		if (wasPosClipR == true) { //current will be over
			if (inputSampleR<lastSampleR) lastSampleR=(0.9085097f*noise)+(inputSampleR*(1.0f-noise));
			else lastSampleR = 0.94f; //~-0.2dB to nearly match ClipOnly and ClipOnly2
		} wasPosClipR = false;
		if (inputSampleR>0.9085097f) {wasPosClipR=true;inputSampleR=(0.9085097f*noise)+(lastSampleR*(1.0f-noise));}
		if (wasNegClipR == true) { //current will be -over
			if (inputSampleR > lastSampleR) lastSampleR=(-0.9085097f*noise)+(inputSampleR*(1.0f-noise));
			else lastSampleR = -0.94f;
		} wasNegClipR = false;
		if (inputSampleR<-0.9085097f) {wasNegClipR=true;inputSampleR=(-0.9085097f*noise)+(lastSampleR*(1.0f-noise));}
		dram->slewR[spacing*2] = fabs(lastSampleR-inputSampleR);
		for (int x = spacing*2; x > 0; x--) dram->slewR[x-1] = dram->slewR[x];
		dram->intermediateR[spacing] = inputSampleR; inputSampleR = lastSampleR;
		//latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) {dram->intermediateR[x-1] = dram->intermediateR[x];} lastSampleR = dram->intermediateR[0];
		if (wasPosClipR || wasNegClipR) {
			for (int x = spacing; x > 0; x--) lastSampleR += dram->intermediateR[x];
			lastSampleR /= spacing;
		} finalSlew = 0.0f;
		for (int x = spacing*2; x >= 0; x--) if (finalSlew < dram->slewR[x]) finalSlew = dram->slewR[x];
		postclip = 0.94f / (1.0f+(finalSlew*1.3986013f));
		if (inputSampleR > postclip) inputSampleR = postclip; if (inputSampleR < -postclip) inputSampleR = -postclip;
		//end ClipOnly3 as a little, compressed chunk that can be dropped into code
		
		
		
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
	iirEncL = 0.0; iirDecL = 0.0;
	compEncL = 1.0; compDecL = 1.0;
	avgEncL = 0.0; avgDecL = 0.0;
	
	iirEncR = 0.0; iirDecR = 0.0;
	compEncR = 1.0; compDecR = 1.0;
	avgEncR = 0.0; avgDecR = 0.0;
	
	for (int temp = 0; temp < 1001; temp++) {dram->dL[temp] = 0.0;dram->dR[temp] = 0.0;}
	sweepL = M_PI;
	sweepR = M_PI;
	nextmaxL = 0.5;	
	nextmaxR = 0.5;
	gcount = 0;	
	
	for (int x = 0; x < gslew_total; x++) dram->gslew[x] = 0.0;
	
	hysteresisL = 0.0;
	hysteresisR = 0.0;
		
	headBumpL = 0.0;
	headBumpR = 0.0;
	for (int x = 0; x < hdb_total; x++) {dram->hdbA[x] = 0.0;dram->hdbB[x] = 0.0;}
	//from ZBandpass, so I can use enums with it
	
	for (int x = 0; x < 33; x++) {dram->avg32L[x] = 0.0; dram->post32L[x] = 0.0; dram->avg32R[x] = 0.0; dram->post32R[x] = 0.0;}
	for (int x = 0; x < 17; x++) {dram->avg16L[x] = 0.0; dram->post16L[x] = 0.0; dram->avg16R[x] = 0.0; dram->post16R[x] = 0.0;}
	for (int x = 0; x < 9; x++) {avg8L[x] = 0.0; post8L[x] = 0.0; avg8R[x] = 0.0; post8R[x] = 0.0;}
	for (int x = 0; x < 5; x++) {avg4L[x] = 0.0; post4L[x] = 0.0; avg4R[x] = 0.0; post4R[x] = 0.0;}
	for (int x = 0; x < 3; x++) {avg2L[x] = 0.0; post2L[x] = 0.0; avg2R[x] = 0.0; post2R[x] = 0.0;}
	avgPos = 0;
	lastDarkL = 0.0; lastDarkR = 0.0;
	//preTapeHack	
	
	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	lastSampleR = 0.0;
	wasPosClipR = false;
	wasNegClipR = false;
	for (int x = 0; x < 17; x++) {dram->intermediateL[x] = 0.0; dram->intermediateR[x] = 0.0;}
	for (int x = 0; x < 33; x++) {dram->slewL[x] = 0.0; dram->slewR[x] = 0.0;}
	//this is reset: values being initialized only once. Startup values, whatever they are.
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
