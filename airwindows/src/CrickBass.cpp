#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "CrickBass"
#define AIRWINDOWS_DESCRIPTION "A flexible but aggressive bass tone for dual pickups."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','r','s' )
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tone", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	
	float lastASampleL;
	float lastSlewL;
	float iirSampleAL;
	float iirSampleBL;
	float iirSampleCL;
	float iirSampleDL;
	float iirSampleEL;
	float iirSampleFL;
	float iirSampleGL;
	float iirSampleHL;
	float iirSampleIL;
	float iirSampleJL;
	
	float lastCabSampleL;
	float smoothCabAL;
	float smoothCabBL; //cab
	
	
	float lastASampleR;
	float lastSlewR;
	float iirSampleAR;
	float iirSampleBR;
	float iirSampleCR;
	float iirSampleDR;
	float iirSampleER;
	float OddAR;
	float OddBR;
	float OddCR;
	float OddDR;
	float OddER;
	float EvenAR;
	float EvenBR;
	float EvenCR;
	float EvenDR;
	float EvenER;
	
	float lastCabSampleR;
	float smoothCabAR;
	float smoothCabBR; //cab
	

	float lastRefL[10];
	float lastRefR[10];
	int cycle;	//undersampling
	
	bool flip;
	int count; //amp
	
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
		fix_total
	}; //fixed frequency biquad filter for ultrasonics, stereo
	
	float lastSampleR;
	float intermediateR[16];
	bool wasPosClipR;
	bool wasNegClipR; //ClipOnly2	
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float OddL[257];
	float EvenL[257]; //amp
	float bL[90];
	float bR[90];
	float fixA[fix_total];
	float fixB[fix_total];
	float fixC[fix_total];
	float fixD[fix_total];
	float fixE[fix_total];
	float fixF[fix_total]; //filtering
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check		
	
	float inputlevelL = powf(GetParameter( kParam_One )+0.5f,2);
	float inputlevelH = inputlevelL*4.0f;
	float basstrimH = (GetParameter( kParam_Two )*0.6f)+0.2f;
	float basstrimL = basstrimH*0.618f;
	
	float EQL = (basstrimL/GetSampleRate())*22050.0f;
	float EQH = (basstrimH/GetSampleRate())*22050.0f;
	float BEQ = (0.0625f/GetSampleRate())*22050.0f;
	
	int diagonal = (int)(0.000861678f*GetSampleRate());
	if (diagonal > 127) diagonal = 127;
	int side = (int)(diagonal/1.4142135623730951f);
	int down = (side + diagonal)/2;
	//now we've got down, side and diagonal as offsets and we also use three successive samples upfront
	float skewlevel = powf(basstrimH,2);
	
	float cutoff = (15000.0f+(basstrimH*10000.0f))/GetSampleRate();
	if (cutoff > 0.49f) cutoff = 0.49f; //don't crash if run at 44.1k
	if (cutoff < 0.001f) cutoff = 0.001f; //or if cutoff's too low
	
	dram->fixF[fix_freq] = dram->fixE[fix_freq] = dram->fixD[fix_freq] = dram->fixC[fix_freq] = dram->fixB[fix_freq] = dram->fixA[fix_freq] = cutoff;
	
    dram->fixA[fix_reso] = 4.46570214f;
	dram->fixB[fix_reso] = 1.51387132f;
	dram->fixC[fix_reso] = 0.93979296f;
	dram->fixD[fix_reso] = 0.70710678f;
	dram->fixE[fix_reso] = 0.52972649f;
	dram->fixF[fix_reso] = 0.50316379f;
	
	float K = tan(M_PI * dram->fixA[fix_freq]); //lowpass
	float norm = 1.0f / (1.0f + K / dram->fixA[fix_reso] + K * K);
	dram->fixA[fix_a0] = K * K * norm;
	dram->fixA[fix_a1] = 2.0f * dram->fixA[fix_a0];
	dram->fixA[fix_a2] = dram->fixA[fix_a0];
	dram->fixA[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fixA[fix_b2] = (1.0f - K / dram->fixA[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->fixB[fix_freq]);
	norm = 1.0f / (1.0f + K / dram->fixB[fix_reso] + K * K);
	dram->fixB[fix_a0] = K * K * norm;
	dram->fixB[fix_a1] = 2.0f * dram->fixB[fix_a0];
	dram->fixB[fix_a2] = dram->fixB[fix_a0];
	dram->fixB[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fixB[fix_b2] = (1.0f - K / dram->fixB[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->fixC[fix_freq]);
	norm = 1.0f / (1.0f + K / dram->fixC[fix_reso] + K * K);
	dram->fixC[fix_a0] = K * K * norm;
	dram->fixC[fix_a1] = 2.0f * dram->fixC[fix_a0];
	dram->fixC[fix_a2] = dram->fixC[fix_a0];
	dram->fixC[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fixC[fix_b2] = (1.0f - K / dram->fixC[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->fixD[fix_freq]);
	norm = 1.0f / (1.0f + K / dram->fixD[fix_reso] + K * K);
	dram->fixD[fix_a0] = K * K * norm;
	dram->fixD[fix_a1] = 2.0f * dram->fixD[fix_a0];
	dram->fixD[fix_a2] = dram->fixD[fix_a0];
	dram->fixD[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fixD[fix_b2] = (1.0f - K / dram->fixD[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->fixE[fix_freq]);
	norm = 1.0f / (1.0f + K / dram->fixE[fix_reso] + K * K);
	dram->fixE[fix_a0] = K * K * norm;
	dram->fixE[fix_a1] = 2.0f * dram->fixE[fix_a0];
	dram->fixE[fix_a2] = dram->fixE[fix_a0];
	dram->fixE[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fixE[fix_b2] = (1.0f - K / dram->fixE[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->fixF[fix_freq]);
	norm = 1.0f / (1.0f + K / dram->fixF[fix_reso] + K * K);
	dram->fixF[fix_a0] = K * K * norm;
	dram->fixF[fix_a1] = 2.0f * dram->fixF[fix_a0];
	dram->fixF[fix_a2] = dram->fixF[fix_a0];
	dram->fixF[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fixF[fix_b2] = (1.0f - K / dram->fixF[fix_reso] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		float outSample = (inputSampleL * dram->fixA[fix_a0]) + dram->fixA[fix_sL1];
		dram->fixA[fix_sL1] = (inputSampleL * dram->fixA[fix_a1]) - (outSample * dram->fixA[fix_b1]) + dram->fixA[fix_sL2];
		dram->fixA[fix_sL2] = (inputSampleL * dram->fixA[fix_a2]) - (outSample * dram->fixA[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * dram->fixA[fix_a0]) + dram->fixA[fix_sR1];
		dram->fixA[fix_sR1] = (inputSampleR * dram->fixA[fix_a1]) - (outSample * dram->fixA[fix_b1]) + dram->fixA[fix_sR2];
		dram->fixA[fix_sR2] = (inputSampleR * dram->fixA[fix_a2]) - (outSample * dram->fixA[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 1
		
		float skewL = (inputSampleL - lastASampleL);
		lastASampleL = inputSampleL;
		//skew will be direction/angle
		float bridgerectifier = fabs(skewL);
		if (bridgerectifier > 3.1415926f) bridgerectifier = 3.1415926f;
		//for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skewL > 0) skewL = bridgerectifier;
		else skewL = -bridgerectifier;
		//skew is now sined and clamped and then re-amplified again
		skewL *= inputSampleL;
		skewL = (skewL+(skewL*basstrimL))/2.0f;
		inputSampleL *= basstrimL;
		float basscut = basstrimL;
		
		float skewR = (inputSampleR - lastASampleR);
		lastASampleR = inputSampleR;
		//skew will be direction/angle
		bridgerectifier = fabs(skewR);
		if (bridgerectifier > 3.1415926f) bridgerectifier = 3.1415926f;
		//for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skewR > 0.0f) skewR = bridgerectifier;
		else skewR = -bridgerectifier;
		//skew is now sined and clamped and then re-amplified again
		skewR *= inputSampleR;
		skewR *= skewlevel;
		inputSampleR *= basstrimH;
		inputSampleR *= inputlevelH;
		float offsetR = (1.0f - EQH) + (fabs(inputSampleR)*EQH);
		if (offsetR < 0.0f) offsetR = 0.0f;
		if (offsetR > 1.0f) offsetR = 1.0f;
		iirSampleAR = (iirSampleAR * (1.0f - (offsetR * EQH))) + (inputSampleR * (offsetR * EQH));
		inputSampleR = inputSampleR - iirSampleAR;
		//highpass
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skewR;
		if (inputSampleR > 0) inputSampleR = (inputSampleR*(-0.57079633f+skewR))+(bridgerectifier*(1.57079633f+skewR));
		else inputSampleR = (inputSampleR*(-0.57079633f+skewR))-(bridgerectifier*(1.57079633f+skewR));
		//overdrive
		iirSampleCR = (iirSampleCR * (1.0f - (offsetR * EQH))) + (inputSampleR * (offsetR * EQH));
		inputSampleR = iirSampleCR;
		//lowpass. Use offset from before gain stage
		//finished first gain stage
		
		outSample = (inputSampleL * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
		dram->fixB[fix_sL1] = (inputSampleL * dram->fixB[fix_a1]) - (outSample * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
		dram->fixB[fix_sL2] = (inputSampleL * dram->fixB[fix_a2]) - (outSample * dram->fixB[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * dram->fixB[fix_a0]) + dram->fixB[fix_sR1];
		dram->fixB[fix_sR1] = (inputSampleR * dram->fixB[fix_a1]) - (outSample * dram->fixB[fix_b1]) + dram->fixB[fix_sR2];
		dram->fixB[fix_sR2] = (inputSampleR * dram->fixB[fix_a2]) - (outSample * dram->fixB[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 2
		
		inputSampleL *= inputlevelL;
		float offsetL = (1.0f - EQL) + (fabs(inputSampleL)*EQL);
		if (offsetL < 0.0f) offsetL = 0.0f;
		if (offsetL > 1.0f) offsetL = 1.0f;
		iirSampleAL = (iirSampleAL * (1.0f - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = inputSampleL - (iirSampleAL*basscut);
		//highpass
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skewL;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSampleL > 0.0f) inputSampleL = (inputSampleL*(-0.57079633f+skewL))+(bridgerectifier*(1.57079633f+skewL));
		else inputSampleL = (inputSampleL*(-0.57079633f+skewL))-(bridgerectifier*(1.57079633f+skewL));
		//overdrive
		iirSampleBL = (iirSampleBL * (1.0f - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = inputSampleL - (iirSampleBL*basscut);
		//highpass. Use offset from before gain stage
		//finished first gain stage
		
		inputSampleR *= inputlevelH;
		offsetR = (1.0f + offsetR) / 2.0f;
		iirSampleBR = (iirSampleBR * (1.0f - (offsetR * EQH))) + (inputSampleR * (offsetR * EQH));
		inputSampleR = inputSampleR - iirSampleBR;
		//highpass
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skewR;
		if (inputSampleR > 0.0f) inputSampleR = (inputSampleR*(-0.57079633f+skewR))+(bridgerectifier*(1.57079633f+skewR));
		else inputSampleR = (inputSampleR*(-0.57079633f+skewR))-(bridgerectifier*(1.57079633f+skewR));
		//overdrive
		iirSampleDR = (iirSampleDR * (1.0f - (offsetR * EQH))) + (inputSampleR * (offsetR * EQH));
		inputSampleR = iirSampleDR;
		//lowpass. Use offset from before gain stage
		if (flip)
		{
			OddDR = OddCR; OddCR = OddBR; OddBR = OddAR; OddAR = inputSampleR;
			inputSampleR = (OddAR + OddBR + OddCR + OddDR) / 4.0f;
		}
		else
		{
			EvenDR = EvenCR; EvenCR = EvenBR; EvenBR = EvenAR; EvenAR = inputSampleR;
			inputSampleR = (EvenAR + EvenBR + EvenCR + EvenDR) / 4.0f;
		}
		
		outSample = (inputSampleL * dram->fixC[fix_a0]) + dram->fixC[fix_sL1];
		dram->fixC[fix_sL1] = (inputSampleL * dram->fixC[fix_a1]) - (outSample * dram->fixC[fix_b1]) + dram->fixC[fix_sL2];
		dram->fixC[fix_sL2] = (inputSampleL * dram->fixC[fix_a2]) - (outSample * dram->fixC[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * dram->fixC[fix_a0]) + dram->fixC[fix_sR1];
		dram->fixC[fix_sR1] = (inputSampleR * dram->fixC[fix_a1]) - (outSample * dram->fixC[fix_b1]) + dram->fixC[fix_sR2];
		dram->fixC[fix_sR2] = (inputSampleR * dram->fixC[fix_a2]) - (outSample * dram->fixC[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 3
		
		inputSampleL *= inputlevelL;
		skewL /= 2.0f;
		offsetL = (1.0f + offsetL) / 2.0f;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skewL;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSampleL > 0.0f) inputSampleL = (inputSampleL*(-0.57079633f+skewL))+(bridgerectifier*(1.57079633f+skewL));
		else inputSampleL = (inputSampleL*(-0.57079633f+skewL))-(bridgerectifier*(1.57079633f+skewL));
		//overdrive
		iirSampleCL = (iirSampleCL * (1.0f - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = inputSampleL - (iirSampleCL*basscut);
		//highpass.
		iirSampleDL = (iirSampleDL * (1.0f - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = iirSampleDL;
		//lowpass. Use offset from before gain stage
		
		inputSampleR *= inputlevelH;
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSampleR > 0.0f) inputSampleR = (inputSampleR*-0.57079633f)+(bridgerectifier*1.57079633f);
		else inputSampleR = (inputSampleR*-0.57079633f)-(bridgerectifier*1.57079633f);
		//output stage has less gain, no highpass, straight lowpass
		iirSampleER = (iirSampleER * (1.0f - EQH)) + (inputSampleR * EQH);
		inputSampleR = iirSampleER;
		//lowpass. Use offset from before gain stage

		outSample = (inputSampleL * dram->fixD[fix_a0]) + dram->fixD[fix_sL1];
		dram->fixD[fix_sL1] = (inputSampleL * dram->fixD[fix_a1]) - (outSample * dram->fixD[fix_b1]) + dram->fixD[fix_sL2];
		dram->fixD[fix_sL2] = (inputSampleL * dram->fixD[fix_a2]) - (outSample * dram->fixD[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * dram->fixD[fix_a0]) + dram->fixD[fix_sR1];
		dram->fixD[fix_sR1] = (inputSampleR * dram->fixD[fix_a1]) - (outSample * dram->fixD[fix_b1]) + dram->fixD[fix_sR2];
		dram->fixD[fix_sR2] = (inputSampleR * dram->fixD[fix_a2]) - (outSample * dram->fixD[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 4
		
		inputSampleL *= inputlevelL;
		skewL /= 2.0f;
		offsetL = (1.0f + offsetL) / 2.0f;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skewL;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSampleL > 0.0f) inputSampleL = (inputSampleL*(-0.57079633f+skewL))+(bridgerectifier*(1.57079633f+skewL));
		else inputSampleL = (inputSampleL*(-0.57079633f+skewL))-(bridgerectifier*(1.57079633f+skewL));
		//overdrive
		iirSampleEL = (iirSampleEL * (1.0f - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = inputSampleL - (iirSampleEL*basscut);
		//we don't need to do basscut again, that was the last one
		//highpass.
		iirSampleFL = (iirSampleFL * (1.0f - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = iirSampleFL;
		//lowpass. Use offset from before gain stage
		
		inputSampleR = sin(inputSampleR);
		
		outSample = (inputSampleL * dram->fixE[fix_a0]) + dram->fixE[fix_sL1];
		dram->fixE[fix_sL1] = (inputSampleL * dram->fixE[fix_a1]) - (outSample * dram->fixE[fix_b1]) + dram->fixE[fix_sL2];
		dram->fixE[fix_sL2] = (inputSampleL * dram->fixE[fix_a2]) - (outSample * dram->fixE[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * dram->fixE[fix_a0]) + dram->fixE[fix_sR1];
		dram->fixE[fix_sR1] = (inputSampleR * dram->fixE[fix_a1]) - (outSample * dram->fixE[fix_b1]) + dram->fixE[fix_sR2];
		dram->fixE[fix_sR2] = (inputSampleR * dram->fixE[fix_a2]) - (outSample * dram->fixE[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 5
		
		inputSampleL *= inputlevelL;
		skewL /= 2.0f;
		offsetL= (1.0f + offsetL) / 2.0f;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSampleL > 0.0f) inputSampleL = (inputSampleL*(-0.57079633f+skewL))+(bridgerectifier*(1.57079633f+skewL));
		else inputSampleL = (inputSampleL*(-0.57079633f+skewL))-(bridgerectifier*(1.57079633f+skewL));
		//output stage has less gain, no highpass, straight lowpass
		iirSampleGL = (iirSampleGL * (1.0f - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = iirSampleGL;
		//lowpass. Use offset from before gain stage
		iirSampleHL = (iirSampleHL * (1.0f - (offsetL * BEQ))) + (inputSampleL * (offsetL * BEQ));
		//extra lowpass for 4*12" speakers
		
		if (count < 0 || count > 128) count = 128;
		float resultBL = 0.0f;
		if (flip)
		{
			dram->OddL[count+128] = dram->OddL[count] = iirSampleHL;
			resultBL = (dram->OddL[count+down] + dram->OddL[count+side] + dram->OddL[count+diagonal]);
		} else {
			dram->EvenL[count+128] = dram->EvenL[count] = iirSampleHL;
			resultBL = (dram->EvenL[count+down] + dram->EvenL[count+side] + dram->EvenL[count+diagonal]);
		}
		count--;
		
		iirSampleIL = (iirSampleIL * (1.0f - (offsetL * BEQ))) + (resultBL * (offsetL * BEQ));
		inputSampleL += (iirSampleIL*0.0625f);
		//extra lowpass for 4*12" speakers
		iirSampleJL = (iirSampleJL * (1.0f - (offsetL * BEQ))) + (inputSampleL * (offsetL * BEQ));
		inputSampleL += iirSampleJL;
		inputSampleL = sin(inputSampleL);
				
		outSample = (inputSampleL * dram->fixF[fix_a0]) + dram->fixF[fix_sL1];
		dram->fixF[fix_sL1] = (inputSampleL * dram->fixF[fix_a1]) - (outSample * dram->fixF[fix_b1]) + dram->fixF[fix_sL2];
		dram->fixF[fix_sL2] = (inputSampleL * dram->fixF[fix_a2]) - (outSample * dram->fixF[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * dram->fixF[fix_a0]) + dram->fixF[fix_sR1];
		dram->fixF[fix_sR1] = (inputSampleR * dram->fixF[fix_a1]) - (outSample * dram->fixF[fix_b1]) + dram->fixF[fix_sR2];
		dram->fixF[fix_sR2] = (inputSampleR * dram->fixF[fix_a2]) - (outSample * dram->fixF[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 6
		
		flip = !flip;
		//amp
		
		cycle++;
		if (cycle == cycleEnd) {
			
			float temp = (inputSampleL + smoothCabAL)/3.0f;
			smoothCabAL = inputSampleL;
			inputSampleL = temp;
			
			dram->bL[81] = dram->bL[80]; dram->bL[80] = dram->bL[79]; 
			dram->bL[79] = dram->bL[78]; dram->bL[78] = dram->bL[77]; dram->bL[77] = dram->bL[76]; dram->bL[76] = dram->bL[75]; dram->bL[75] = dram->bL[74]; dram->bL[74] = dram->bL[73]; dram->bL[73] = dram->bL[72]; dram->bL[72] = dram->bL[71]; 
			dram->bL[71] = dram->bL[70]; dram->bL[70] = dram->bL[69]; dram->bL[69] = dram->bL[68]; dram->bL[68] = dram->bL[67]; dram->bL[67] = dram->bL[66]; dram->bL[66] = dram->bL[65]; dram->bL[65] = dram->bL[64]; dram->bL[64] = dram->bL[63]; 
			dram->bL[63] = dram->bL[62]; dram->bL[62] = dram->bL[61]; dram->bL[61] = dram->bL[60]; dram->bL[60] = dram->bL[59]; dram->bL[59] = dram->bL[58]; dram->bL[58] = dram->bL[57]; dram->bL[57] = dram->bL[56]; dram->bL[56] = dram->bL[55]; 
			dram->bL[55] = dram->bL[54]; dram->bL[54] = dram->bL[53]; dram->bL[53] = dram->bL[52]; dram->bL[52] = dram->bL[51]; dram->bL[51] = dram->bL[50]; dram->bL[50] = dram->bL[49]; dram->bL[49] = dram->bL[48]; dram->bL[48] = dram->bL[47]; 
			dram->bL[47] = dram->bL[46]; dram->bL[46] = dram->bL[45]; dram->bL[45] = dram->bL[44]; dram->bL[44] = dram->bL[43]; dram->bL[43] = dram->bL[42]; dram->bL[42] = dram->bL[41]; dram->bL[41] = dram->bL[40]; dram->bL[40] = dram->bL[39]; 
			dram->bL[39] = dram->bL[38]; dram->bL[38] = dram->bL[37]; dram->bL[37] = dram->bL[36]; dram->bL[36] = dram->bL[35]; dram->bL[35] = dram->bL[34]; dram->bL[34] = dram->bL[33]; dram->bL[33] = dram->bL[32]; dram->bL[32] = dram->bL[31]; 
			dram->bL[31] = dram->bL[30]; dram->bL[30] = dram->bL[29]; dram->bL[29] = dram->bL[28]; dram->bL[28] = dram->bL[27]; dram->bL[27] = dram->bL[26]; dram->bL[26] = dram->bL[25]; dram->bL[25] = dram->bL[24]; dram->bL[24] = dram->bL[23]; 
			dram->bL[23] = dram->bL[22]; dram->bL[22] = dram->bL[21]; dram->bL[21] = dram->bL[20]; dram->bL[20] = dram->bL[19]; dram->bL[19] = dram->bL[18]; dram->bL[18] = dram->bL[17]; dram->bL[17] = dram->bL[16]; dram->bL[16] = dram->bL[15]; 
			dram->bL[15] = dram->bL[14]; dram->bL[14] = dram->bL[13]; dram->bL[13] = dram->bL[12]; dram->bL[12] = dram->bL[11]; dram->bL[11] = dram->bL[10]; dram->bL[10] = dram->bL[9]; dram->bL[9] = dram->bL[8]; dram->bL[8] = dram->bL[7]; 
			dram->bL[7] = dram->bL[6]; dram->bL[6] = dram->bL[5]; dram->bL[5] = dram->bL[4]; dram->bL[4] = dram->bL[3]; dram->bL[3] = dram->bL[2]; dram->bL[2] = dram->bL[1]; dram->bL[1] = dram->bL[0]; dram->bL[0] = inputSampleL;
			
			inputSampleL += (dram->bL[1] * (1.35472031405494242f  + (0.00220914099195157f*fabs(dram->bL[1]))));
			inputSampleL += (dram->bL[2] * (1.63534207755253003f  - (0.11406232654509685f*fabs(dram->bL[2]))));
			inputSampleL += (dram->bL[3] * (1.82334575691525869f  - (0.42647194712964054f*fabs(dram->bL[3]))));
			inputSampleL += (dram->bL[4] * (1.86156386235405868f  - (0.76744187887586590f*fabs(dram->bL[4]))));
			inputSampleL += (dram->bL[5] * (1.67332739338852599f  - (0.95161997324293013f*fabs(dram->bL[5]))));
			inputSampleL += (dram->bL[6] * (1.25054130794899021f  - (0.98410433514572859f*fabs(dram->bL[6]))));
			inputSampleL += (dram->bL[7] * (0.70049121047281737f  - (0.87375612110718992f*fabs(dram->bL[7]))));
			inputSampleL += (dram->bL[8] * (0.15291791448081560f  - (0.61195266024519046f*fabs(dram->bL[8]))));
			inputSampleL -= (dram->bL[9] * (0.37301992914152693f  + (0.16755422915252094f*fabs(dram->bL[9]))));
			inputSampleL -= (dram->bL[10] * (0.76568539228498433f  - (0.28554435228965386f*fabs(dram->bL[10]))));
			inputSampleL -= (dram->bL[11] * (0.95726568749937369f  - (0.61659719162806048f*fabs(dram->bL[11]))));
			inputSampleL -= (dram->bL[12] * (1.01273552193911032f  - (0.81827288407943954f*fabs(dram->bL[12]))));
			inputSampleL -= (dram->bL[13] * (0.93920108117234447f  - (0.80077111864205874f*fabs(dram->bL[13]))));
			inputSampleL -= (dram->bL[14] * (0.79831898832953974f  - (0.65814750339694406f*fabs(dram->bL[14]))));
			inputSampleL -= (dram->bL[15] * (0.64200088100452313f  - (0.46135833001232618f*fabs(dram->bL[15]))));
			inputSampleL -= (dram->bL[16] * (0.48807302802822128f  - (0.15506178974799034f*fabs(dram->bL[16]))));
			inputSampleL -= (dram->bL[17] * (0.36545171501947982f  + (0.16126103769376721f*fabs(dram->bL[17]))));
			inputSampleL -= (dram->bL[18] * (0.31469581455759105f  + (0.32250870039053953f*fabs(dram->bL[18]))));
			inputSampleL -= (dram->bL[19] * (0.36893534817945800f  + (0.25409418897237473f*fabs(dram->bL[19]))));
			inputSampleL -= (dram->bL[20] * (0.41092557722975687f  + (0.13114730488878301f*fabs(dram->bL[20]))));
			inputSampleL -= (dram->bL[21] * (0.38584044480710594f  + (0.06825323739722661f*fabs(dram->bL[21]))));
			inputSampleL -= (dram->bL[22] * (0.33378434007178670f  + (0.04144255489164217f*fabs(dram->bL[22]))));
			inputSampleL -= (dram->bL[23] * (0.26144203061699706f  + (0.06031313105098152f*fabs(dram->bL[23]))));
			inputSampleL -= (dram->bL[24] * (0.25818342000920502f  + (0.03642289242586355f*fabs(dram->bL[24]))));
			inputSampleL -= (dram->bL[25] * (0.28096018498822661f  + (0.00976973667327174f*fabs(dram->bL[25]))));
			inputSampleL -= (dram->bL[26] * (0.25845682019095384f  + (0.02749015358080831f*fabs(dram->bL[26]))));
			inputSampleL -= (dram->bL[27] * (0.26655607865953096f  - (0.00329839675455690f*fabs(dram->bL[27]))));
			inputSampleL -= (dram->bL[28] * (0.30590085026938518f  - (0.07375043215328811f*fabs(dram->bL[28]))));
			inputSampleL -= (dram->bL[29] * (0.32875683916470899f  - (0.12454134857516502f*fabs(dram->bL[29]))));
			inputSampleL -= (dram->bL[30] * (0.38166643180506560f  - (0.19973911428609989f*fabs(dram->bL[30]))));
			inputSampleL -= (dram->bL[31] * (0.49068186937289598f  - (0.34785166842136384f*fabs(dram->bL[31]))));
			inputSampleL -= (dram->bL[32] * (0.60274753867622777f  - (0.48685038872711034f*fabs(dram->bL[32]))));
			inputSampleL -= (dram->bL[33] * (0.65944678627090636f  - (0.49844657885975518f*fabs(dram->bL[33]))));
			inputSampleL -= (dram->bL[34] * (0.64488955808717285f  - (0.40514406499806987f*fabs(dram->bL[34]))));
			inputSampleL -= (dram->bL[35] * (0.55818730353434354f  - (0.28029870614987346f*fabs(dram->bL[35]))));
			inputSampleL -= (dram->bL[36] * (0.43110859113387556f  - (0.15373504582939335f*fabs(dram->bL[36]))));
			inputSampleL -= (dram->bL[37] * (0.37726894966096269f  - (0.11570983506028532f*fabs(dram->bL[37]))));
			inputSampleL -= (dram->bL[38] * (0.39953242355200935f  - (0.17879231130484088f*fabs(dram->bL[38]))));
			inputSampleL -= (dram->bL[39] * (0.36726676379100875f  - (0.22013553023983223f*fabs(dram->bL[39]))));
			inputSampleL -= (dram->bL[40] * (0.27187029469227386f  - (0.18461171768478427f*fabs(dram->bL[40]))));
			inputSampleL -= (dram->bL[41] * (0.21109334552321635f  - (0.14497481318083569f*fabs(dram->bL[41]))));
			inputSampleL -= (dram->bL[42] * (0.19808797405293213f  - (0.14916579928186940f*fabs(dram->bL[42]))));
			inputSampleL -= (dram->bL[43] * (0.16287926785495671f  - (0.15146098461120627f*fabs(dram->bL[43]))));
			inputSampleL -= (dram->bL[44] * (0.11086621477163359f  - (0.13182973443924018f*fabs(dram->bL[44]))));
			inputSampleL -= (dram->bL[45] * (0.07531043236890560f  - (0.08062172796472888f*fabs(dram->bL[45]))));
			inputSampleL -= (dram->bL[46] * (0.01747364473230771f  + (0.02201865873632456f*fabs(dram->bL[46]))));
			inputSampleL += (dram->bL[47] * (0.03080279125662693f  - (0.08721756240972101f*fabs(dram->bL[47]))));
			inputSampleL += (dram->bL[48] * (0.02354148659185142f  - (0.06376361763053796f*fabs(dram->bL[48]))));
			inputSampleL -= (dram->bL[49] * (0.02835772372098715f  + (0.00589978513642627f*fabs(dram->bL[49]))));
			inputSampleL -= (dram->bL[50] * (0.08983370744565244f  - (0.02350960427706536f*fabs(dram->bL[50]))));
			inputSampleL -= (dram->bL[51] * (0.14148947620055380f  - (0.03329826628693369f*fabs(dram->bL[51]))));
			inputSampleL -= (dram->bL[52] * (0.17576502674572581f  - (0.06507546651241880f*fabs(dram->bL[52]))));
			inputSampleL -= (dram->bL[53] * (0.17168865666573860f  - (0.07734801128437317f*fabs(dram->bL[53]))));
			inputSampleL -= (dram->bL[54] * (0.14107027738292105f  - (0.03136459344220402f*fabs(dram->bL[54]))));
			inputSampleL -= (dram->bL[55] * (0.12287163395380074f  + (0.01933408169185258f*fabs(dram->bL[55]))));
			inputSampleL -= (dram->bL[56] * (0.12276622398112971f  + (0.01983508766241737f*fabs(dram->bL[56]))));
			inputSampleL -= (dram->bL[57] * (0.12349721440213673f  - (0.01111031415304768f*fabs(dram->bL[57]))));
			inputSampleL -= (dram->bL[58] * (0.08649454142716655f  + (0.02252815645513927f*fabs(dram->bL[58]))));
			inputSampleL -= (dram->bL[59] * (0.00953083685474757f  + (0.13778878548343007f*fabs(dram->bL[59]))));
			inputSampleL += (dram->bL[60] * (0.06045983158868478f  - (0.23966318224935096f*fabs(dram->bL[60]))));
			inputSampleL += (dram->bL[61] * (0.09053229817093242f  - (0.27190119941572544f*fabs(dram->bL[61]))));
			inputSampleL += (dram->bL[62] * (0.08112662178843048f  - (0.22456862606452327f*fabs(dram->bL[62]))));
			inputSampleL += (dram->bL[63] * (0.07503525686243730f  - (0.14330154410548213f*fabs(dram->bL[63]))));
			inputSampleL += (dram->bL[64] * (0.07372595404399729f  - (0.06185193766408734f*fabs(dram->bL[64]))));
			inputSampleL += (dram->bL[65] * (0.06073789200080433f  + (0.01261857435786178f*fabs(dram->bL[65]))));
			inputSampleL += (dram->bL[66] * (0.04616712695742254f  + (0.05851771967084609f*fabs(dram->bL[66]))));
			inputSampleL += (dram->bL[67] * (0.01036235510345900f  + (0.08286534414423796f*fabs(dram->bL[67]))));
			inputSampleL -= (dram->bL[68] * (0.03708389413229191f  - (0.06695282381039531f*fabs(dram->bL[68]))));
			inputSampleL -= (dram->bL[69] * (0.07092204876981217f  - (0.01915829199112784f*fabs(dram->bL[69]))));
			inputSampleL -= (dram->bL[70] * (0.09443579589460312f  + (0.01210082455316246f*fabs(dram->bL[70]))));
			inputSampleL -= (dram->bL[71] * (0.07824038577769601f  + (0.06121988546065113f*fabs(dram->bL[71]))));
			inputSampleL -= (dram->bL[72] * (0.00854730633079399f  + (0.14468518752295506f*fabs(dram->bL[72]))));
			inputSampleL += (dram->bL[73] * (0.06845589924191028f  - (0.18902431382592944f*fabs(dram->bL[73]))));
			inputSampleL += (dram->bL[74] * (0.10351569998375465f  - (0.13204443060279647f*fabs(dram->bL[74]))));
			inputSampleL += (dram->bL[75] * (0.10513368758532179f  - (0.02993199294485649f*fabs(dram->bL[75]))));
			inputSampleL += (dram->bL[76] * (0.08896978950235003f  + (0.04074499273825906f*fabs(dram->bL[76]))));
			inputSampleL += (dram->bL[77] * (0.03697537734050980f  + (0.09217751130846838f*fabs(dram->bL[77]))));
			inputSampleL -= (dram->bL[78] * (0.04014322441280276f  - (0.14062297149365666f*fabs(dram->bL[78]))));
			inputSampleL -= (dram->bL[79] * (0.10505934581398618f  - (0.16988861157275814f*fabs(dram->bL[79]))));
			inputSampleL -= (dram->bL[80] * (0.13937661651676272f  - (0.15083294570551492f*fabs(dram->bL[80]))));
			inputSampleL -= (dram->bL[81] * (0.13183458845108439f  - (0.06657454442471208f*fabs(dram->bL[81]))));
			
			temp = (inputSampleL + smoothCabBL)/3.0f;
			smoothCabBL = inputSampleL;
			inputSampleL = temp/4.0f;
			
			temp = (inputSampleR + smoothCabAR)/3.0f;
			smoothCabAR = inputSampleR;
			inputSampleR = temp;
			
			dram->bR[82] = dram->bR[81]; dram->bR[81] = dram->bR[80]; dram->bR[80] = dram->bR[79]; 
			dram->bR[79] = dram->bR[78]; dram->bR[78] = dram->bR[77]; dram->bR[77] = dram->bR[76]; dram->bR[76] = dram->bR[75]; dram->bR[75] = dram->bR[74]; dram->bR[74] = dram->bR[73]; dram->bR[73] = dram->bR[72]; dram->bR[72] = dram->bR[71]; 
			dram->bR[71] = dram->bR[70]; dram->bR[70] = dram->bR[69]; dram->bR[69] = dram->bR[68]; dram->bR[68] = dram->bR[67]; dram->bR[67] = dram->bR[66]; dram->bR[66] = dram->bR[65]; dram->bR[65] = dram->bR[64]; dram->bR[64] = dram->bR[63]; 
			dram->bR[63] = dram->bR[62]; dram->bR[62] = dram->bR[61]; dram->bR[61] = dram->bR[60]; dram->bR[60] = dram->bR[59]; dram->bR[59] = dram->bR[58]; dram->bR[58] = dram->bR[57]; dram->bR[57] = dram->bR[56]; dram->bR[56] = dram->bR[55]; 
			dram->bR[55] = dram->bR[54]; dram->bR[54] = dram->bR[53]; dram->bR[53] = dram->bR[52]; dram->bR[52] = dram->bR[51]; dram->bR[51] = dram->bR[50]; dram->bR[50] = dram->bR[49]; dram->bR[49] = dram->bR[48]; dram->bR[48] = dram->bR[47]; 
			dram->bR[47] = dram->bR[46]; dram->bR[46] = dram->bR[45]; dram->bR[45] = dram->bR[44]; dram->bR[44] = dram->bR[43]; dram->bR[43] = dram->bR[42]; dram->bR[42] = dram->bR[41]; dram->bR[41] = dram->bR[40]; dram->bR[40] = dram->bR[39]; 
			dram->bR[39] = dram->bR[38]; dram->bR[38] = dram->bR[37]; dram->bR[37] = dram->bR[36]; dram->bR[36] = dram->bR[35]; dram->bR[35] = dram->bR[34]; dram->bR[34] = dram->bR[33]; dram->bR[33] = dram->bR[32]; dram->bR[32] = dram->bR[31]; 
			dram->bR[31] = dram->bR[30]; dram->bR[30] = dram->bR[29]; dram->bR[29] = dram->bR[28]; dram->bR[28] = dram->bR[27]; dram->bR[27] = dram->bR[26]; dram->bR[26] = dram->bR[25]; dram->bR[25] = dram->bR[24]; dram->bR[24] = dram->bR[23]; 
			dram->bR[23] = dram->bR[22]; dram->bR[22] = dram->bR[21]; dram->bR[21] = dram->bR[20]; dram->bR[20] = dram->bR[19]; dram->bR[19] = dram->bR[18]; dram->bR[18] = dram->bR[17]; dram->bR[17] = dram->bR[16]; dram->bR[16] = dram->bR[15]; 
			dram->bR[15] = dram->bR[14]; dram->bR[14] = dram->bR[13]; dram->bR[13] = dram->bR[12]; dram->bR[12] = dram->bR[11]; dram->bR[11] = dram->bR[10]; dram->bR[10] = dram->bR[9]; dram->bR[9] = dram->bR[8]; dram->bR[8] = dram->bR[7]; 
			dram->bR[7] = dram->bR[6]; dram->bR[6] = dram->bR[5]; dram->bR[5] = dram->bR[4]; dram->bR[4] = dram->bR[3]; dram->bR[3] = dram->bR[2]; dram->bR[2] = dram->bR[1]; dram->bR[1] = dram->bR[0]; dram->bR[0] = inputSampleR;
			inputSampleR += (dram->bR[1] * (1.42133070619855229f  - (0.18270903813104500f*fabs(dram->bR[1]))));
			inputSampleR += (dram->bR[2] * (1.47209686171873821f  - (0.27954009590498585f*fabs(dram->bR[2]))));
			inputSampleR += (dram->bR[3] * (1.34648011331265294f  - (0.47178343556301960f*fabs(dram->bR[3]))));
			inputSampleR += (dram->bR[4] * (0.82133804036124580f  - (0.41060189990353935f*fabs(dram->bR[4]))));
			inputSampleR += (dram->bR[5] * (0.21628057120466901f  - (0.26062442734317454f*fabs(dram->bR[5]))));
			inputSampleR -= (dram->bR[6] * (0.30306716082877883f  + (0.10067648425439185f*fabs(dram->bR[6]))));
			inputSampleR -= (dram->bR[7] * (0.69484313178531876f  - (0.09655574841702286f*fabs(dram->bR[7]))));
			inputSampleR -= (dram->bR[8] * (0.88320822356980833f  - (0.26501644327144314f*fabs(dram->bR[8]))));
			inputSampleR -= (dram->bR[9] * (0.81326147029423723f  - (0.31115926837054075f*fabs(dram->bR[9]))));
			inputSampleR -= (dram->bR[10] * (0.56728759049069222f  - (0.23304233545561287f*fabs(dram->bR[10]))));
			inputSampleR -= (dram->bR[11] * (0.33340326645198737f  - (0.12361361388240180f*fabs(dram->bR[11]))));
			inputSampleR -= (dram->bR[12] * (0.20280263733605616f  - (0.03531960962500105f*fabs(dram->bR[12]))));
			inputSampleR -= (dram->bR[13] * (0.15864533788751345f  + (0.00355160825317868f*fabs(dram->bR[13]))));
			inputSampleR -= (dram->bR[14] * (0.12544767480555119f  + (0.01979010423176500f*fabs(dram->bR[14]))));
			inputSampleR -= (dram->bR[15] * (0.06666788902658917f  + (0.00188830739903378f*fabs(dram->bR[15]))));
			inputSampleR += (dram->bR[16] * (0.02977793355081072f  + (0.02304216615605394f*fabs(dram->bR[16]))));
			inputSampleR += (dram->bR[17] * (0.12821526330916558f  + (0.02636238376777800f*fabs(dram->bR[17]))));
			inputSampleR += (dram->bR[18] * (0.19933812710210136f  - (0.02932657234709721f*fabs(dram->bR[18]))));
			inputSampleR += (dram->bR[19] * (0.18346460191225772f  - (0.12859581955080629f*fabs(dram->bR[19]))));
			inputSampleR -= (dram->bR[20] * (0.00088697526755385f  + (0.15855257539277415f*fabs(dram->bR[20]))));
			inputSampleR -= (dram->bR[21] * (0.28904286712096761f  + (0.06226286786982616f*fabs(dram->bR[21]))));
			inputSampleR -= (dram->bR[22] * (0.49133546282552537f  - (0.06512851581813534f*fabs(dram->bR[22]))));
			inputSampleR -= (dram->bR[23] * (0.52908013030763046f  - (0.13606992188523465f*fabs(dram->bR[23]))));
			inputSampleR -= (dram->bR[24] * (0.45897241332311706f  - (0.15527194946346906f*fabs(dram->bR[24]))));
			inputSampleR -= (dram->bR[25] * (0.35535938629924352f  - (0.13634771941703441f*fabs(dram->bR[25]))));
			inputSampleR -= (dram->bR[26] * (0.26185269405237693f  - (0.08736651482771546f*fabs(dram->bR[26]))));
			inputSampleR -= (dram->bR[27] * (0.19997351944186473f  - (0.01714565029656306f*fabs(dram->bR[27]))));
			inputSampleR -= (dram->bR[28] * (0.18894054145105646f  + (0.04557612705740050f*fabs(dram->bR[28]))));
			inputSampleR -= (dram->bR[29] * (0.24043993691153928f  + (0.05267500387081067f*fabs(dram->bR[29]))));
			inputSampleR -= (dram->bR[30] * (0.29191852873822671f  + (0.01922151122971644f*fabs(dram->bR[30]))));
			inputSampleR -= (dram->bR[31] * (0.29399783430587761f  - (0.02238952856106585f*fabs(dram->bR[31]))));
			inputSampleR -= (dram->bR[32] * (0.26662219155294159f  - (0.07760819463416335f*fabs(dram->bR[32]))));
			inputSampleR -= (dram->bR[33] * (0.20881206667122221f  - (0.11930017354479640f*fabs(dram->bR[33]))));
			inputSampleR -= (dram->bR[34] * (0.12916658879944876f  - (0.11798638949823513f*fabs(dram->bR[34]))));
			inputSampleR -= (dram->bR[35] * (0.07678815166012012f  - (0.06826864734598684f*fabs(dram->bR[35]))));
			inputSampleR -= (dram->bR[36] * (0.08568505484529348f  - (0.00510459741104792f*fabs(dram->bR[36]))));
			inputSampleR -= (dram->bR[37] * (0.13613615872486634f  + (0.02288223583971244f*fabs(dram->bR[37]))));
			inputSampleR -= (dram->bR[38] * (0.17426657494209266f  + (0.02723737220296440f*fabs(dram->bR[38]))));
			inputSampleR -= (dram->bR[39] * (0.17343619261009030f  + (0.01412920547179825f*fabs(dram->bR[39]))));
			inputSampleR -= (dram->bR[40] * (0.14548368977428555f  - (0.02640418940455951f*fabs(dram->bR[40]))));
			inputSampleR -= (dram->bR[41] * (0.10485295885802372f  - (0.06334665781931498f*fabs(dram->bR[41]))));
			inputSampleR -= (dram->bR[42] * (0.06632268974717079f  - (0.05960343688612868f*fabs(dram->bR[42]))));
			inputSampleR -= (dram->bR[43] * (0.06915692039882040f  - (0.03541337869596061f*fabs(dram->bR[43]))));
			inputSampleR -= (dram->bR[44] * (0.11889611687783583f  - (0.02250608307287119f*fabs(dram->bR[44]))));
			inputSampleR -= (dram->bR[45] * (0.14598456370320673f  + (0.00280345943128246f*fabs(dram->bR[45]))));
			inputSampleR -= (dram->bR[46] * (0.12312084125613143f  + (0.04947283933434576f*fabs(dram->bR[46]))));
			inputSampleR -= (dram->bR[47] * (0.11379940289994711f  + (0.06590080966570636f*fabs(dram->bR[47]))));
			inputSampleR -= (dram->bR[48] * (0.12963290754003182f  + (0.02597647654256477f*fabs(dram->bR[48]))));
			inputSampleR -= (dram->bR[49] * (0.12723837402978638f  - (0.04942071966927938f*fabs(dram->bR[49]))));
			inputSampleR -= (dram->bR[50] * (0.09185015882996231f  - (0.10420810015956679f*fabs(dram->bR[50]))));
			inputSampleR -= (dram->bR[51] * (0.04011592913036545f  - (0.10234174227772008f*fabs(dram->bR[51]))));
			inputSampleR += (dram->bR[52] * (0.00992597785057113f  + (0.05674042373836896f*fabs(dram->bR[52]))));
			inputSampleR += (dram->bR[53] * (0.04921452178306781f  - (0.00222698867111080f*fabs(dram->bR[53]))));
			inputSampleR += (dram->bR[54] * (0.06096504883783566f  - (0.04040426549982253f*fabs(dram->bR[54]))));
			inputSampleR += (dram->bR[55] * (0.04113530718724200f  - (0.04190143593049960f*fabs(dram->bR[55]))));
			inputSampleR += (dram->bR[56] * (0.01292699017654650f  - (0.01121994018532499f*fabs(dram->bR[56]))));
			inputSampleR -= (dram->bR[57] * (0.00437123132431870f  - (0.02482497612289103f*fabs(dram->bR[57]))));
			inputSampleR -= (dram->bR[58] * (0.02090571264211918f  - (0.03732746039260295f*fabs(dram->bR[58]))));
			inputSampleR -= (dram->bR[59] * (0.04749751678612051f  - (0.02960060937328099f*fabs(dram->bR[59]))));
			inputSampleR -= (dram->bR[60] * (0.07675095194206227f  - (0.02241927084099648f*fabs(dram->bR[60]))));
			inputSampleR -= (dram->bR[61] * (0.08879414028581609f  - (0.01144281133042115f*fabs(dram->bR[61]))));
			inputSampleR -= (dram->bR[62] * (0.07378854974999530f  + (0.02518742701599147f*fabs(dram->bR[62]))));
			inputSampleR -= (dram->bR[63] * (0.04677309194488959f  + (0.08984657372223502f*fabs(dram->bR[63]))));
			inputSampleR -= (dram->bR[64] * (0.02911874044176449f  + (0.14202665940555093f*fabs(dram->bR[64]))));
			inputSampleR -= (dram->bR[65] * (0.02103564720234969f  + (0.14640411976171003f*fabs(dram->bR[65]))));
			inputSampleR -= (dram->bR[66] * (0.01940626429101940f  + (0.10867274382865903f*fabs(dram->bR[66]))));
			inputSampleR -= (dram->bR[67] * (0.03965401793931531f  + (0.04775225375522835f*fabs(dram->bR[67]))));
			inputSampleR -= (dram->bR[68] * (0.08102486457314527f  - (0.03204447425666343f*fabs(dram->bR[68]))));
			inputSampleR -= (dram->bR[69] * (0.11794849372825778f  - (0.12755667382696789f*fabs(dram->bR[69]))));
			inputSampleR -= (dram->bR[70] * (0.11946469076758266f  - (0.20151394599125422f*fabs(dram->bR[70]))));
			inputSampleR -= (dram->bR[71] * (0.07404630324668053f  - (0.21300634351769704f*fabs(dram->bR[71]))));
			inputSampleR -= (dram->bR[72] * (0.00477584437144086f  - (0.16864707684978708f*fabs(dram->bR[72]))));
			inputSampleR += (dram->bR[73] * (0.05924822014377220f  + (0.09394651445109450f*fabs(dram->bR[73]))));
			inputSampleR += (dram->bR[74] * (0.10060989907457370f  + (0.00419196431884887f*fabs(dram->bR[74]))));
			inputSampleR += (dram->bR[75] * (0.10817907203844988f  - (0.07459664480796091f*fabs(dram->bR[75]))));
			inputSampleR += (dram->bR[76] * (0.08701102204768002f  - (0.11129477437630560f*fabs(dram->bR[76]))));
			inputSampleR += (dram->bR[77] * (0.05673785623180162f  - (0.10638640242375266f*fabs(dram->bR[77]))));
			inputSampleR += (dram->bR[78] * (0.02944190197442081f  - (0.08499792583420167f*fabs(dram->bR[78]))));
			inputSampleR += (dram->bR[79] * (0.01570145445652971f  - (0.06190456843465320f*fabs(dram->bR[79]))));
			inputSampleR += (dram->bR[80] * (0.02770233032476748f  - (0.04573713136865480f*fabs(dram->bR[80]))));
			inputSampleR += (dram->bR[81] * (0.05417160459175360f  - (0.03965651064634598f*fabs(dram->bR[81]))));
			inputSampleR += (dram->bR[82] * (0.06080831637644498f  - (0.02909500789113911f*fabs(dram->bR[82]))));
			
			temp = (inputSampleR + smoothCabBR)/3.0f;
			smoothCabBR = inputSampleR;
			inputSampleR = temp/4.0f;
			
			if (cycleEnd == 4) {
				lastRefL[0] = lastRefL[4]; //start from previous last
				lastRefL[2] = (lastRefL[0] + inputSampleL)/2; //half
				lastRefL[1] = (lastRefL[0] + lastRefL[2])/2; //one quarter
				lastRefL[3] = (lastRefL[2] + inputSampleL)/2; //three quarters
				lastRefL[4] = inputSampleL; //full
				lastRefR[0] = lastRefR[4]; //start from previous last
				lastRefR[2] = (lastRefR[0] + inputSampleR)/2; //half
				lastRefR[1] = (lastRefR[0] + lastRefR[2])/2; //one quarter
				lastRefR[3] = (lastRefR[2] + inputSampleR)/2; //three quarters
				lastRefR[4] = inputSampleR; //full
			}
			if (cycleEnd == 3) {
				lastRefL[0] = lastRefL[3]; //start from previous last
				lastRefL[2] = (lastRefL[0]+lastRefL[0]+inputSampleL)/3; //third
				lastRefL[1] = (lastRefL[0]+inputSampleL+inputSampleL)/3; //two thirds
				lastRefL[3] = inputSampleL; //full
				lastRefR[0] = lastRefR[3]; //start from previous last
				lastRefR[2] = (lastRefR[0]+lastRefR[0]+inputSampleR)/3; //third
				lastRefR[1] = (lastRefR[0]+inputSampleR+inputSampleR)/3; //two thirds
				lastRefR[3] = inputSampleR; //full
			}
			if (cycleEnd == 2) {
				lastRefL[0] = lastRefL[2]; //start from previous last
				lastRefL[1] = (lastRefL[0] + inputSampleL)/2; //half
				lastRefL[2] = inputSampleL; //full
				lastRefR[0] = lastRefR[2]; //start from previous last
				lastRefR[1] = (lastRefR[0] + inputSampleR)/2; //half
				lastRefR[2] = inputSampleR; //full
			}
			if (cycleEnd == 1) {
				lastRefL[0] = inputSampleL;
				lastRefR[0] = inputSampleR;
			}
			cycle = 0; //reset
			inputSampleL = lastRefL[cycle];
			inputSampleR = lastRefR[cycle];
		} else {
			inputSampleL = lastRefL[cycle];
			inputSampleR = lastRefR[cycle];
			//we are going through our references now
		}
		switch (cycleEnd) //multi-pole average using lastRef[] variables on BigAmp channel only
		{
			case 4:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[7])*0.5f;
				lastRefL[7] = lastRefL[8]; //continue, do not break
			case 3:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[6])*0.5f;
				lastRefL[6] = lastRefL[8]; //continue, do not break
			case 2:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[5])*0.5f;
				lastRefL[5] = lastRefL[8]; //continue, do not break
			case 1:
				break; //no further averaging
		}
		
		inputSampleR = (inputSampleL*0.5f)+(inputSampleR*0.5f);
		
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
		intermediateR[cycleEnd] = inputSampleR;
        inputSampleR = lastSampleR; //Latency is however many samples equals one 44.1k sample
		for (int x = cycleEnd; x > 0; x--) intermediateR[x-1] = intermediateR[x];
		lastSampleR = intermediateR[0]; //run a little buffer to handle this
		
		inputSampleL = inputSampleR;
		
		
		
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
	
	lastASampleL = 0.0;
	lastSlewL = 0.0;
	iirSampleAL = 0.0;
	iirSampleBL = 0.0;
	iirSampleCL = 0.0;
	iirSampleDL = 0.0;
	iirSampleEL = 0.0;
	iirSampleFL = 0.0;
	iirSampleGL = 0.0;
	iirSampleHL = 0.0;
	iirSampleIL = 0.0;
	iirSampleJL = 0.0;
	
	lastASampleR = 0.0;
	lastSlewR = 0.0;
	iirSampleAR = 0.0;
	iirSampleBR = 0.0;
	iirSampleCR = 0.0;
	iirSampleDR = 0.0;
	iirSampleER = 0.0;
	OddAR = 0.0;
	OddBR = 0.0;
	OddCR = 0.0;
	OddDR = 0.0;
	OddER = 0.0;
	EvenAR = 0.0;
	EvenBR = 0.0;
	EvenCR = 0.0;
	EvenDR = 0.0;
	EvenER = 0.0;
	
	for (int fcount = 0; fcount < 257; fcount++) {
		dram->OddL[fcount] = 0.0;
		dram->EvenL[fcount] = 0.0;
	}
	
	count = 0;
	flip = false; //amp
	
	for(int fcount = 0; fcount < 90; fcount++) {
		dram->bL[fcount] = 0;
		dram->bR[fcount] = 0;
	}
	smoothCabAL = 0.0; smoothCabBL = 0.0; lastCabSampleL = 0.0; //cab
	smoothCabAR = 0.0; smoothCabBR = 0.0; lastCabSampleR = 0.0; //cab
	
	for (int fcount = 0; fcount < 9; fcount++) {
		lastRefL[fcount] = 0.0;
		lastRefR[fcount] = 0.0;
	}
	cycle = 0; //undersampling
	
	for (int x = 0; x < fix_total; x++) {
		dram->fixA[x] = 0.0;
		dram->fixB[x] = 0.0;
		dram->fixC[x] = 0.0;
		dram->fixD[x] = 0.0;
		dram->fixE[x] = 0.0;
		dram->fixF[x] = 0.0;
	}	//filtering
	
	lastSampleR = 0.0;
	wasPosClipR = false;
	wasNegClipR = false;
	for (int x = 0; x < 16; x++) intermediateR[x] = 0.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
