#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "GrindAmp"
#define AIRWINDOWS_DESCRIPTION "A heavier amp sim for many purposes."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','r','p' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Gain", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tone", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 800, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 smoothA;
		Float32 smoothB;
		Float32 smoothC;
		Float32 smoothD;
		Float32 smoothE;
		Float32 smoothF;
		Float32 smoothG;
		Float32 smoothH;
		Float32 smoothI;
		Float32 smoothJ;
		Float32 smoothK;
		Float32 secondA;
		Float32 secondB;
		Float32 secondC;
		Float32 secondD;
		Float32 secondE;
		Float32 secondF;
		Float32 secondG;
		Float32 secondH;
		Float32 secondI;
		Float32 secondJ;
		Float32 secondK;
		Float32 thirdA;
		Float32 thirdB;
		Float32 thirdC;
		Float32 thirdD;
		Float32 thirdE;
		Float32 thirdF;
		Float32 thirdG;
		Float32 thirdH;
		Float32 thirdI;
		Float32 thirdJ;
		Float32 thirdK;
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 iirSampleC;
		Float32 iirSampleD;
		Float32 iirSampleE;
		Float32 iirSampleF;
		Float32 iirSampleG;
		Float32 iirSampleH;
		Float32 iirSampleI;
		Float32 iirLowpass;
		Float32 iirSub;
		Float32 storeSample; //amp
		
		float lastCabSample;
		float smoothCabA;
		float smoothCabB; //cab
		
		float lastRef[10];
		int cycle;	//undersampling
		
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
		uint32_t fpd;
	
	struct _dram {
			float b[90];
		float fixA[fix_total];
		float fixB[fix_total];
		float fixC[fix_total];
		float fixD[fix_total];
		float fixE[fix_total];
		float fixF[fix_total]; //filtering
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
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check		
	
	float inputlevel = powf(GetParameter( kParam_One ),2);
	float samplerate = GetSampleRate();
	float trimEQ = 1.1f-GetParameter( kParam_Two );
	float toneEQ = trimEQ/1.2f;
	trimEQ /= 50.0f;
	trimEQ += 0.165f;
	float EQ = ((trimEQ-(toneEQ/6.1f)) / samplerate)*22050.0f;
	float BEQ = ((trimEQ+(toneEQ/2.1f)) / samplerate)*22050.0f;
	float outputlevel = GetParameter( kParam_Three );
	float wet = GetParameter( kParam_Four );
	float bassdrive = 1.57079633f*(2.5f-toneEQ);
	
	float cutoff = (18000.0f+(GetParameter( kParam_Two )*1000.0f)) / GetSampleRate();
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
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		float outSample = (inputSample * dram->fixA[fix_a0]) + dram->fixA[fix_sL1];
		dram->fixA[fix_sL1] = (inputSample * dram->fixA[fix_a1]) - (outSample * dram->fixA[fix_b1]) + dram->fixA[fix_sL2];
		dram->fixA[fix_sL2] = (inputSample * dram->fixA[fix_a2]) - (outSample * dram->fixA[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		iirSampleA = (iirSampleA * (1 - EQ)) + (inputSample *  EQ);
		inputSample = inputSample - (iirSampleA*0.92f);
		//highpass
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		float bridgerectifier = fabs(inputSample);
		float inverse = (bridgerectifier+1.0f)/2.0f;
		bridgerectifier = (smoothA + (secondA*inverse) + (thirdA*bridgerectifier) + inputSample);
		thirdA = secondA;
		secondA = smoothA;
		smoothA = inputSample;
		float basscatch = inputSample = bridgerectifier;
		//three-sample averaging lowpass
		
		outSample = (inputSample * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
		dram->fixB[fix_sL1] = (inputSample * dram->fixB[fix_a1]) - (outSample * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
		dram->fixB[fix_sL2] = (inputSample * dram->fixB[fix_a2]) - (outSample * dram->fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		iirSampleB = (iirSampleB * (1 - EQ)) + (inputSample *  EQ);
		inputSample = inputSample - (iirSampleB*0.79f);
		//highpass
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		//overdrive
		bridgerectifier = fabs(inputSample);
		inverse = (bridgerectifier+1.0f)/2.0f;
		bridgerectifier = (smoothB + (secondB*inverse) + (thirdB*bridgerectifier) + inputSample);
		thirdB = secondB;
		secondB = smoothB;
		smoothB = inputSample;
		inputSample = bridgerectifier;
		//three-sample averaging lowpass

		iirSampleC = (iirSampleC * (1 - BEQ)) + (basscatch * BEQ);
		basscatch = iirSampleC*bassdrive;
		bridgerectifier = fabs(basscatch);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (basscatch > 0.0f) basscatch = bridgerectifier;
		else basscatch = -bridgerectifier;
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		//overdrive
		inverse = (bridgerectifier+1.0f)/2.0f;
		bridgerectifier = (smoothC + (secondC*inverse) + (thirdC*bridgerectifier) + inputSample);
		thirdC = secondC;
		secondC = smoothC;
		smoothC = inputSample;
		inputSample = bridgerectifier;
		//three-sample averaging lowpass
		
		outSample = (inputSample * dram->fixC[fix_a0]) + dram->fixC[fix_sL1];
		dram->fixC[fix_sL1] = (inputSample * dram->fixC[fix_a1]) - (outSample * dram->fixC[fix_b1]) + dram->fixC[fix_sL2];
		dram->fixC[fix_sL2] = (inputSample * dram->fixC[fix_a2]) - (outSample * dram->fixC[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		iirSampleD = (iirSampleD * (1 - BEQ)) + (basscatch * BEQ);
		basscatch = iirSampleD*bassdrive;
		bridgerectifier = fabs(basscatch);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (basscatch > 0.0f) basscatch = bridgerectifier;
		else basscatch = -bridgerectifier;
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		//overdrive
		inverse = (bridgerectifier+1.0f)/2.0f;
		bridgerectifier = (smoothD + (secondD*inverse) + (thirdD*bridgerectifier) + inputSample);
		thirdD = secondD;
		secondD = smoothD;
		smoothD = inputSample;
		inputSample = bridgerectifier;
		//three-sample averaging lowpass
		
		outSample = (inputSample * dram->fixD[fix_a0]) + dram->fixD[fix_sL1];
		dram->fixD[fix_sL1] = (inputSample * dram->fixD[fix_a1]) - (outSample * dram->fixD[fix_b1]) + dram->fixD[fix_sL2];
		dram->fixD[fix_sL2] = (inputSample * dram->fixD[fix_a2]) - (outSample * dram->fixD[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		iirSampleE = (iirSampleE * (1 - BEQ)) + (basscatch * BEQ);
		basscatch = iirSampleE*bassdrive;
		bridgerectifier = fabs(basscatch);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (basscatch > 0.0f) basscatch = bridgerectifier;
		else basscatch = -bridgerectifier;
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		//overdrive
		inverse = (bridgerectifier+1.0f)/2.0f;
		bridgerectifier = (smoothE + (secondE*inverse) + (thirdE*bridgerectifier) + inputSample);
		thirdE = secondE;
		secondE = smoothE;
		smoothE = inputSample;
		inputSample = bridgerectifier;
		//three-sample averaging lowpass
		
		iirSampleF = (iirSampleF * (1 - BEQ)) + (basscatch * BEQ);
		basscatch = iirSampleF*bassdrive;
		bridgerectifier = fabs(basscatch);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (basscatch > 0.0f) basscatch = bridgerectifier;
		else basscatch = -bridgerectifier;
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		//overdrive
		inverse = (bridgerectifier+1.0f)/2.0f;
		bridgerectifier = (smoothF + (secondF*inverse) + (thirdF*bridgerectifier) + inputSample);
		thirdF = secondF;
		secondF = smoothF;
		smoothF = inputSample;
		inputSample = bridgerectifier;
		//three-sample averaging lowpass
		
		outSample = (inputSample * dram->fixE[fix_a0]) + dram->fixE[fix_sL1];
		dram->fixE[fix_sL1] = (inputSample * dram->fixE[fix_a1]) - (outSample * dram->fixE[fix_b1]) + dram->fixE[fix_sL2];
		dram->fixE[fix_sL2] = (inputSample * dram->fixE[fix_a2]) - (outSample * dram->fixE[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		iirSampleG = (iirSampleG * (1 - BEQ)) + (basscatch * BEQ);
		basscatch = iirSampleG*bassdrive;
		bridgerectifier = fabs(basscatch);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (basscatch > 0.0f) basscatch = bridgerectifier;
		else basscatch = -bridgerectifier;
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		//overdrive
		inverse = (bridgerectifier+1.0f)/2.0f;
		bridgerectifier = (smoothG + (secondG*inverse) + (thirdG*bridgerectifier) + inputSample);
		thirdG = secondG;
		secondG = smoothG;
		smoothG = inputSample;
		inputSample = bridgerectifier;
		//three-sample averaging lowpass
		
		iirSampleH = (iirSampleH * (1 - BEQ)) + (basscatch * BEQ);
		basscatch = iirSampleH*bassdrive;
		bridgerectifier = fabs(basscatch);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (basscatch > 0.0f) basscatch = bridgerectifier;
		else basscatch = -bridgerectifier;
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		//overdrive
		inverse = (bridgerectifier+1.0f)/2.0f;
		bridgerectifier = (smoothH + (secondH*inverse) + (thirdH*bridgerectifier) + inputSample);
		thirdH = secondH;
		secondH = smoothH;
		smoothH = inputSample;
		inputSample = bridgerectifier;
		//three-sample averaging lowpass
		
		outSample = (inputSample * dram->fixF[fix_a0]) + dram->fixF[fix_sL1];
		dram->fixF[fix_sL1] = (inputSample * dram->fixF[fix_a1]) - (outSample * dram->fixF[fix_b1]) + dram->fixF[fix_sL2];
		dram->fixF[fix_sL2] = (inputSample * dram->fixF[fix_a2]) - (outSample * dram->fixF[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		iirSampleI = (iirSampleI * (1 - BEQ)) + (basscatch * BEQ);
		basscatch = iirSampleI*bassdrive;
		bridgerectifier = fabs(basscatch);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (basscatch > 0.0f) basscatch = bridgerectifier;
		else basscatch = -bridgerectifier;
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		//overdrive
		inverse = (bridgerectifier+1.0f)/2.0f;
		bridgerectifier = (smoothI + (secondI*inverse) + (thirdI*bridgerectifier) + inputSample);
		thirdI = secondI;
		secondI = smoothI;
		smoothI = inputSample;
		inputSample = bridgerectifier;
		//three-sample averaging lowpass
		bridgerectifier = fabs(inputSample);
		inverse = (bridgerectifier+1.0f)/2.0f;
		bridgerectifier = (smoothJ + (secondJ*inverse) + (thirdJ*bridgerectifier) + inputSample);
		thirdJ = secondJ;
		secondJ = smoothJ;
		smoothJ = inputSample;
		inputSample = bridgerectifier;
		//three-sample averaging lowpass
		bridgerectifier = fabs(inputSample);
		inverse = (bridgerectifier+1.0f)/2.0f;
		bridgerectifier = (smoothK + (secondK*inverse) + (thirdK*bridgerectifier) + inputSample);
		thirdK = secondK;
		secondK = smoothK;
		smoothK = inputSample;
		inputSample = bridgerectifier;
		//three-sample averaging lowpass
		
		basscatch /= 2.0f;
		inputSample = (inputSample*toneEQ)+basscatch;
		//extra lowpass for 4*12" speakers		
		
		bridgerectifier = fabs(inputSample*outputlevel);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		inputSample += basscatch;
		//split bass between overdrive and clean
		inputSample /= (1.0f+toneEQ);
		
		float randy = ((float(fpd)/UINT32_MAX)*0.061f);
		inputSample = ((inputSample*(1-randy))+(storeSample*randy))*outputlevel;
		storeSample = inputSample;
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider
		//amp
		
		cycle++;
		if (cycle == cycleEnd) {
			
			//drySample = inputSample;
			float temp = (inputSample + smoothCabA)/3.0f;
			smoothCabA = inputSample;
			inputSample = temp;
			
			dram->b[83] = dram->b[82]; dram->b[82] = dram->b[81]; dram->b[81] = dram->b[80]; dram->b[80] = dram->b[79]; 
			dram->b[79] = dram->b[78]; dram->b[78] = dram->b[77]; dram->b[77] = dram->b[76]; dram->b[76] = dram->b[75]; dram->b[75] = dram->b[74]; dram->b[74] = dram->b[73]; dram->b[73] = dram->b[72]; dram->b[72] = dram->b[71]; 
			dram->b[71] = dram->b[70]; dram->b[70] = dram->b[69]; dram->b[69] = dram->b[68]; dram->b[68] = dram->b[67]; dram->b[67] = dram->b[66]; dram->b[66] = dram->b[65]; dram->b[65] = dram->b[64]; dram->b[64] = dram->b[63]; 
			dram->b[63] = dram->b[62]; dram->b[62] = dram->b[61]; dram->b[61] = dram->b[60]; dram->b[60] = dram->b[59]; dram->b[59] = dram->b[58]; dram->b[58] = dram->b[57]; dram->b[57] = dram->b[56]; dram->b[56] = dram->b[55]; 
			dram->b[55] = dram->b[54]; dram->b[54] = dram->b[53]; dram->b[53] = dram->b[52]; dram->b[52] = dram->b[51]; dram->b[51] = dram->b[50]; dram->b[50] = dram->b[49]; dram->b[49] = dram->b[48]; dram->b[48] = dram->b[47]; 
			dram->b[47] = dram->b[46]; dram->b[46] = dram->b[45]; dram->b[45] = dram->b[44]; dram->b[44] = dram->b[43]; dram->b[43] = dram->b[42]; dram->b[42] = dram->b[41]; dram->b[41] = dram->b[40]; dram->b[40] = dram->b[39]; 
			dram->b[39] = dram->b[38]; dram->b[38] = dram->b[37]; dram->b[37] = dram->b[36]; dram->b[36] = dram->b[35]; dram->b[35] = dram->b[34]; dram->b[34] = dram->b[33]; dram->b[33] = dram->b[32]; dram->b[32] = dram->b[31]; 
			dram->b[31] = dram->b[30]; dram->b[30] = dram->b[29]; dram->b[29] = dram->b[28]; dram->b[28] = dram->b[27]; dram->b[27] = dram->b[26]; dram->b[26] = dram->b[25]; dram->b[25] = dram->b[24]; dram->b[24] = dram->b[23]; 
			dram->b[23] = dram->b[22]; dram->b[22] = dram->b[21]; dram->b[21] = dram->b[20]; dram->b[20] = dram->b[19]; dram->b[19] = dram->b[18]; dram->b[18] = dram->b[17]; dram->b[17] = dram->b[16]; dram->b[16] = dram->b[15]; 
			dram->b[15] = dram->b[14]; dram->b[14] = dram->b[13]; dram->b[13] = dram->b[12]; dram->b[12] = dram->b[11]; dram->b[11] = dram->b[10]; dram->b[10] = dram->b[9]; dram->b[9] = dram->b[8]; dram->b[8] = dram->b[7]; 
			dram->b[7] = dram->b[6]; dram->b[6] = dram->b[5]; dram->b[5] = dram->b[4]; dram->b[4] = dram->b[3]; dram->b[3] = dram->b[2]; dram->b[2] = dram->b[1]; dram->b[1] = dram->b[0]; dram->b[0] = inputSample;
			inputSample += (dram->b[1] * (1.29550481610475132f  + (0.19713872057074355f*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (1.42302569895462616f  + (0.30599505521284787f*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (1.28728195804197565f  + (0.23168333460446133f*fabs(dram->b[3]))));
			inputSample += (dram->b[4] * (0.88553784290822690f  + (0.14263256172918892f*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (0.37129054918432319f  + (0.00150040944205920f*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.12150959412556320f  + (0.32776273620569107f*fabs(dram->b[6]))));
			inputSample -= (dram->b[7] * (0.44900065463203775f  + (0.74101214925298819f*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.54058781908186482f  + (1.07821707459008387f*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.49361966401791391f  + (1.23540109014850508f*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.39819495093078133f  + (1.11247213730917749f*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.31379279985435521f  + (0.80330360359638298f*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.30744359242808555f  + (0.42132528876858205f*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.33943170284673974f  + (0.09183418349389982f*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.33838775119286391f  - (0.06453051658561271f*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.30682305697961665f  - (0.09549380253249232f*fabs(dram->b[15]))));
			inputSample -= (dram->b[16] * (0.23408741339295336f  - (0.08083404732361277f*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.10411746814025019f  + (0.00253651281245780f*fabs(dram->b[17]))));
			inputSample += (dram->b[18] * (0.00133623776084696f  - (0.04447267870865820f*fabs(dram->b[18]))));
			inputSample += (dram->b[19] * (0.02461903992114161f  + (0.07530671732655550f*fabs(dram->b[19]))));
			inputSample += (dram->b[20] * (0.02086715842475373f  + (0.22795860236804899f*fabs(dram->b[20]))));
			inputSample += (dram->b[21] * (0.02761433637100917f  + (0.26108320417844094f*fabs(dram->b[21]))));
			inputSample += (dram->b[22] * (0.04475285369162533f  + (0.19160705011061663f*fabs(dram->b[22]))));
			inputSample += (dram->b[23] * (0.09447338372862381f  + (0.03681550508743799f*fabs(dram->b[23]))));
			inputSample += (dram->b[24] * (0.13445890343722280f  - (0.13713036462146147f*fabs(dram->b[24]))));
			inputSample += (dram->b[25] * (0.13872868945088121f  - (0.22401242373298191f*fabs(dram->b[25]))));
			inputSample += (dram->b[26] * (0.14915650097434549f  - (0.26718804981526367f*fabs(dram->b[26]))));
			inputSample += (dram->b[27] * (0.12766643217091783f  - (0.27745664795660430f*fabs(dram->b[27]))));
			inputSample += (dram->b[28] * (0.03675849788393101f  - (0.18338278173550679f*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.06307306864232835f  + (0.06089480869040766f*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.14947389348962944f  + (0.04642103054798480f*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.25235266566401526f  + (0.08423062596460507f*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.33496344048679683f  + (0.09808328256677995f*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.36590030482175445f  + (0.10622650888958179f*fabs(dram->b[33]))));
			inputSample -= (dram->b[34] * (0.35015197011464372f  + (0.08982043516016047f*fabs(dram->b[34]))));
			inputSample -= (dram->b[35] * (0.26808437585665090f  + (0.00735561860229533f*fabs(dram->b[35]))));
			inputSample -= (dram->b[36] * (0.11624318543291220f  - (0.07142484314510467f*fabs(dram->b[36]))));
			inputSample += (dram->b[37] * (0.05617084165377551f  + (0.11785854050350089f*fabs(dram->b[37]))));
			inputSample += (dram->b[38] * (0.20540028692589385f  + (0.20479174663329586f*fabs(dram->b[38]))));
			inputSample += (dram->b[39] * (0.30455415003043818f  + (0.29074864580096849f*fabs(dram->b[39]))));
			inputSample += (dram->b[40] * (0.33810750937829476f  + (0.29182307921316802f*fabs(dram->b[40]))));
			inputSample += (dram->b[41] * (0.31936133365277430f  + (0.26535537727394987f*fabs(dram->b[41]))));
			inputSample += (dram->b[42] * (0.27388548321981876f  + (0.19735049990538350f*fabs(dram->b[42]))));
			inputSample += (dram->b[43] * (0.21454597517994098f  + (0.06415909270247236f*fabs(dram->b[43]))));
			inputSample += (dram->b[44] * (0.15001045817707717f  - (0.03831118543404573f*fabs(dram->b[44]))));
			inputSample += (dram->b[45] * (0.07283437284653138f  - (0.09281952429543777f*fabs(dram->b[45]))));
			inputSample -= (dram->b[46] * (0.03917872184241358f  + (0.14306291461398810f*fabs(dram->b[46]))));
			inputSample -= (dram->b[47] * (0.16695932032148642f  + (0.19138995946950504f*fabs(dram->b[47]))));
			inputSample -= (dram->b[48] * (0.27055854466909462f  + (0.22531296466343192f*fabs(dram->b[48]))));
			inputSample -= (dram->b[49] * (0.33256357307578271f  + (0.23305840475692102f*fabs(dram->b[49]))));
			inputSample -= (dram->b[50] * (0.33459770116834442f  + (0.24091822618917569f*fabs(dram->b[50]))));
			inputSample -= (dram->b[51] * (0.27156687236338090f  + (0.24062938573512443f*fabs(dram->b[51]))));
			inputSample -= (dram->b[52] * (0.17197093288412094f  + (0.19083085091993421f*fabs(dram->b[52]))));
			inputSample -= (dram->b[53] * (0.06738628195910543f  + (0.10268609751019808f*fabs(dram->b[53]))));
			inputSample += (dram->b[54] * (0.00222429218204290f  + (0.01439664435720548f*fabs(dram->b[54]))));
			inputSample += (dram->b[55] * (0.01346992803494091f  + (0.15947137113534526f*fabs(dram->b[55]))));
			inputSample -= (dram->b[56] * (0.02038911881377448f  - (0.26763170752416160f*fabs(dram->b[56]))));
			inputSample -= (dram->b[57] * (0.08233579178189687f  - (0.29415931086406055f*fabs(dram->b[57]))));
			inputSample -= (dram->b[58] * (0.15447855089824883f  - (0.26489186990840807f*fabs(dram->b[58]))));
			inputSample -= (dram->b[59] * (0.20518281113362655f  - (0.16135382257522859f*fabs(dram->b[59]))));
			inputSample -= (dram->b[60] * (0.22244686050232007f  + (0.00847180390247432f*fabs(dram->b[60]))));
			inputSample -= (dram->b[61] * (0.21849243134998034f  + (0.14460595245753741f*fabs(dram->b[61]))));
			inputSample -= (dram->b[62] * (0.20256105734574054f  + (0.18932793221831667f*fabs(dram->b[62]))));
			inputSample -= (dram->b[63] * (0.18604070054295399f  + (0.17250665610927965f*fabs(dram->b[63]))));
			inputSample -= (dram->b[64] * (0.17222844322058231f  + (0.12992472027850357f*fabs(dram->b[64]))));
			inputSample -= (dram->b[65] * (0.14447856616566443f  + (0.09089219002147308f*fabs(dram->b[65]))));
			inputSample -= (dram->b[66] * (0.10385520794251019f  + (0.08600465834570559f*fabs(dram->b[66]))));
			inputSample -= (dram->b[67] * (0.07124435678265063f  + (0.09071532210549428f*fabs(dram->b[67]))));
			inputSample -= (dram->b[68] * (0.05216857461197572f  + (0.06794061706070262f*fabs(dram->b[68]))));
			inputSample -= (dram->b[69] * (0.05235381920184123f  + (0.02818101717909346f*fabs(dram->b[69]))));
			inputSample -= (dram->b[70] * (0.07569701245553526f  - (0.00634228544764946f*fabs(dram->b[70]))));
			inputSample -= (dram->b[71] * (0.10320125382718826f  - (0.02751486906644141f*fabs(dram->b[71]))));
			inputSample -= (dram->b[72] * (0.12122120969079088f  - (0.05434007312178933f*fabs(dram->b[72]))));
			inputSample -= (dram->b[73] * (0.13438969117200902f  - (0.09135218559713874f*fabs(dram->b[73]))));
			inputSample -= (dram->b[74] * (0.13534390437529981f  - (0.10437672041458675f*fabs(dram->b[74]))));
			inputSample -= (dram->b[75] * (0.11424128854188388f  - (0.08693450726462598f*fabs(dram->b[75]))));
			inputSample -= (dram->b[76] * (0.08166894518596159f  - (0.06949989431475120f*fabs(dram->b[76]))));
			inputSample -= (dram->b[77] * (0.04293976378555305f  - (0.05718625137421843f*fabs(dram->b[77]))));
			inputSample += (dram->b[78] * (0.00933076320644409f  + (0.01728285211520138f*fabs(dram->b[78]))));
			inputSample += (dram->b[79] * (0.06450430362918153f  - (0.02492994833691022f*fabs(dram->b[79]))));
			inputSample += (dram->b[80] * (0.10187400687649277f  - (0.03578455940532403f*fabs(dram->b[80]))));
			inputSample += (dram->b[81] * (0.11039763294094571f  - (0.03995523517573508f*fabs(dram->b[81]))));
			inputSample += (dram->b[82] * (0.08557960776024547f  - (0.03482514309492527f*fabs(dram->b[82]))));
			inputSample += (dram->b[83] * (0.02730881850805332f  - (0.00514750108411127f*fabs(dram->b[83]))));
			
			temp = (inputSample + smoothCabB)/3.0f;
			smoothCabB = inputSample;
			inputSample = temp/4.0f;
			
			
			randy = ((float(fpd)/UINT32_MAX)*0.044f);
			drySample = ((((inputSample*(1-randy))+(lastCabSample*randy))*wet)+(drySample*(1.0f-wet)))*outputlevel;
			lastCabSample = inputSample;
			inputSample = drySample; //cab
			
			if (cycleEnd == 4) {
				lastRef[0] = lastRef[4]; //start from previous last
				lastRef[2] = (lastRef[0] + inputSample)/2; //half
				lastRef[1] = (lastRef[0] + lastRef[2])/2; //one quarter
				lastRef[3] = (lastRef[2] + inputSample)/2; //three quarters
				lastRef[4] = inputSample; //full
			}
			if (cycleEnd == 3) {
				lastRef[0] = lastRef[3]; //start from previous last
				lastRef[2] = (lastRef[0]+lastRef[0]+inputSample)/3; //third
				lastRef[1] = (lastRef[0]+inputSample+inputSample)/3; //two thirds
				lastRef[3] = inputSample; //full
			}
			if (cycleEnd == 2) {
				lastRef[0] = lastRef[2]; //start from previous last
				lastRef[1] = (lastRef[0] + inputSample)/2; //half
				lastRef[2] = inputSample; //full
			}
			if (cycleEnd == 1) lastRef[0] = inputSample;
			cycle = 0; //reset
			inputSample = lastRef[cycle];
		} else {
			inputSample = lastRef[cycle];
			//we are going through our references now
		}
		
		switch (cycleEnd) //multi-pole average using lastRef[] variables
		{
			case 4:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[7])*0.5f;
				lastRef[7] = lastRef[8]; //continue, do not break
			case 3:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[6])*0.5f;
				lastRef[6] = lastRef[8]; //continue, do not break
			case 2:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[5])*0.5f;
				lastRef[5] = lastRef[8]; //continue, do not break
			case 1:
				break; //no further averaging
		} //undersampling
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
 
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	smoothA = 0.0;
	smoothB = 0.0;
	smoothC = 0.0;
	smoothD = 0.0;
	smoothE = 0.0;
	smoothF = 0.0;
	smoothG = 0.0;
	smoothH = 0.0;
	smoothI = 0.0;
	smoothJ = 0.0;
	smoothK = 0.0;
	secondA = 0.0;
	secondB = 0.0;
	secondC = 0.0;
	secondD = 0.0;
	secondE = 0.0;
	secondF = 0.0;
	secondG = 0.0;
	secondH = 0.0;
	secondI = 0.0;
	secondJ = 0.0;
	secondK = 0.0;
	thirdA = 0.0;
	thirdB = 0.0;
	thirdC = 0.0;
	thirdD = 0.0;
	thirdE = 0.0;
	thirdF = 0.0;
	thirdG = 0.0;
	thirdH = 0.0;
	thirdI = 0.0;
	thirdJ = 0.0;
	thirdK = 0.0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	iirSampleG = 0.0;
	iirSampleH = 0.0;
	iirSampleI = 0.0;
	iirLowpass = 0.0;
	iirSub = 0.0;
	storeSample = 0.0; //amp
	
	for(int fcount = 0; fcount < 90; fcount++) {dram->b[fcount] = 0;}
	smoothCabA = 0.0; smoothCabB = 0.0; lastCabSample = 0.0; //cab
	
	for (int fcount = 0; fcount < 9; fcount++) {lastRef[fcount] = 0.0;}
	cycle = 0; //undersampling
	
	for (int x = 0; x < fix_total; x++) {
		dram->fixA[x] = 0.0;
		dram->fixB[x] = 0.0;
		dram->fixC[x] = 0.0;
		dram->fixD[x] = 0.0;
		dram->fixE[x] = 0.0;
		dram->fixF[x] = 0.0;
	}	//filtering
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;	
}
};
