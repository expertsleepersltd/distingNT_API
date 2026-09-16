#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ToTape8Mono"
#define AIRWINDOWS_DESCRIPTION "ToTape8Mono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','o','p' )
#define AIRWINDOWS_KERNELS
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
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
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
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		float iirEncL;
		float iirDecL;
		float compEncL;
		float compDecL;
		float avgEncL;
		float avgDecL;
		
		float sweepL;
		float nextmaxL;
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
		
		float headBumpL;
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
			hdb_total
		}; //fixed frequency biquad filter for ultrasonics, stereo
		
		float lastSampleL;
		float intermediateL[16];
		bool wasPosClipL;
		bool wasNegClipL;				
		uint32_t fpd;
	
	struct _dram {
			float dL[1002];
		float gslew[gslew_total]; //end bias	
		float hdbA[hdb_total];
		float hdbB[hdb_total];
	};
	_dram* dram;
};
_kernel kernels[1];

#include "../include/template2.h"
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;
	
	float inputGain = powf(GetParameter( kParam_A )*2.0f,2.0f);
	
	float dublyAmount = GetParameter( kParam_B )*2.0f;
	float outlyAmount = (1.0f-GetParameter( kParam_B ))*-2.0f;
	if (outlyAmount < -1.0f) outlyAmount = -1.0f;
	float iirEncFreq = (1.0f-GetParameter( kParam_C ))/overallscale;
	float iirDecFreq = GetParameter( kParam_C )/overallscale;
	float iirMidFreq = ((GetParameter( kParam_C ) * 0.618f) + 0.382f)/overallscale;
	
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
	float subCurve = sin(GetParameter( kParam_G )*M_PI);
	float iirSubFreq = (subCurve*0.008f)/overallscale;
	
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
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		if (inputGain != 1.0f) {
			inputSampleL *= inputGain;
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
		
		//begin Flutter
		if (flutDepth > 0.0f) {
			if (gcount < 0 || gcount > 999) gcount = 999;
			dram->dL[gcount] = inputSampleL;
			int count = gcount;
			float offset = flutDepth + (flutDepth * sin(sweepL));
			sweepL += nextmaxL * flutFrequency;
			if (sweepL > (M_PI*2.0f)) {
				sweepL -= M_PI*2.0f;
				float flutA = 0.24f + (fpd / (float)UINT32_MAX * 0.74f);
				nextmaxL = flutA;
			}
			count += (int)floor(offset);
			inputSampleL = (dram->dL[count-((count > 999)?1000:0)] * (1-(offset-floor(offset))));
			inputSampleL += (dram->dL[count+1-((count+1 > 999)?1000:0)] * (offset-floor(offset)));
			gcount--;
		}
		//end Flutter
		
		//start bias routine
		if (fabs(bias) > 0.001f) {
			for (int x = 0; x < gslew_total; x += 3) {
				if (underBias > 0.0f) {
					float stuck = fabs(inputSampleL - (dram->gslew[x]/0.975f)) / underBias;
					if (stuck < 1.0f) inputSampleL = (inputSampleL * stuck) + ((dram->gslew[x]/0.975f)*(1.0f-stuck));
				}
				if ((inputSampleL - dram->gslew[x]) > dram->gslew[x+2]) inputSampleL = dram->gslew[x] + dram->gslew[x+2];
				if (-(inputSampleL - dram->gslew[x]) > dram->gslew[x+2]) inputSampleL = dram->gslew[x] - dram->gslew[x+2];
				dram->gslew[x] = inputSampleL * 0.975f;
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
				
		//begin HeadBump
		float headBumpSampleL = 0.0f;
		if (headBumpMix > 0.0f) {
			headBumpL += (LowsSampleL * headBumpDrive);
			headBumpL -= (headBumpL * headBumpL * headBumpL * (0.0618f/sqrt(overallscale)));
			float headBiqSampleL = (headBumpL * dram->hdbA[hdb_a0]) + dram->hdbA[hdb_sL1];
			dram->hdbA[hdb_sL1] = (headBumpL * dram->hdbA[hdb_a1]) - (headBiqSampleL * dram->hdbA[hdb_b1]) + dram->hdbA[hdb_sL2];
			dram->hdbA[hdb_sL2] = (headBumpL * dram->hdbA[hdb_a2]) - (headBiqSampleL * dram->hdbA[hdb_b2]);
			headBumpSampleL = (headBiqSampleL * dram->hdbB[hdb_a0]) + dram->hdbB[hdb_sL1];
			dram->hdbB[hdb_sL1] = (headBiqSampleL * dram->hdbB[hdb_a1]) - (headBumpSampleL * dram->hdbB[hdb_b1]) + dram->hdbB[hdb_sL2];
			dram->hdbB[hdb_sL2] = (headBiqSampleL * dram->hdbB[hdb_a2]) - (headBumpSampleL * dram->hdbB[hdb_b2]);
		}
		//end HeadBump
		
		inputSampleL = LowsSampleL + HighsSampleL + (headBumpSampleL * headBumpMix);
		
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
		
		if (outputGain != 1.0f) {
			inputSampleL *= outputGain;
		}
		
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
				
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirEncL = 0.0; iirDecL = 0.0;
	compEncL = 1.0; compDecL = 1.0;
	avgEncL = 0.0; avgDecL = 0.0;
	
	for (int temp = 0; temp < 1001; temp++) {dram->dL[temp] = 0.0;}
	sweepL = M_PI;
	nextmaxL = 0.5;
	gcount = 0;	
	
	for (int x = 0; x < gslew_total; x++) dram->gslew[x] = 0.0;
	
	iirMidRollerL = 0.0;
	iirLowCutoffL = 0.0;
	
	headBumpL = 0.0;
	for (int x = 0; x < hdb_total; x++) {dram->hdbA[x] = 0.0;dram->hdbB[x] = 0.0;}
	//from ZBandpass, so I can use enums with it	
	
	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	for (int x = 0; x < 16; x++) {intermediateL[x] = 0.0;}
	//this is reset: values being initialized only once. Startup values, whatever they are.
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
