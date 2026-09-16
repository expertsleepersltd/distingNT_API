#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "FireAmp"
#define AIRWINDOWS_DESCRIPTION "A bright, loud, tubey amp sim for leads and dirt guitar."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','i','q' )
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
 
		Float32 lastSample;
		Float32 storeSample;
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
		Float32 smoothL;
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 iirSampleC;
		Float32 iirSampleD;
		Float32 iirSampleE;
		Float32 iirSampleF;
		Float32 iirSampleG;
		Float32 iirSampleH;
		Float32 iirSampleI;
		Float32 iirSampleJ;
		Float32 iirSampleK;
		Float32 iirSampleL;
		Float32 iirLowpass;
		Float32 iirSpkA;
		Float32 iirSpkB;
		Float32 iirSub;
		bool flip;
		int count; //amp
		
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
			Float32 Odd[257];
		Float32 Even[257];
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
	
	float bassfill = GetParameter( kParam_One );
	float outputlevel = GetParameter( kParam_Three );
	float wet = GetParameter( kParam_Four );
	
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check		
	
	float startlevel = bassfill;
	float samplerate = GetSampleRate();
	float basstrim = bassfill / 16.0f;
	float toneEQ = (GetParameter( kParam_Two ) / samplerate)*22050.0f;
	float EQ = (basstrim / samplerate)*22050.0f;
	float bleed = outputlevel/16.0f;
	float bassfactor = 1.0f-(basstrim*basstrim);
	float BEQ = (bleed / samplerate)*22050.0f;
	int diagonal = (int)(0.000861678f*samplerate);
	if (diagonal > 127) diagonal = 127;
	int side = (int)(diagonal/1.4142135623730951f);
	int down = (side + diagonal)/2;
	//now we've got down, side and diagonal as offsets and we also use three successive samples upfront
	
	float cutoff = (15000.0f+(GetParameter( kParam_Two )*10000.0f)) / GetSampleRate();
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
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		float basscut = 0.98f;
		//we're going to be shifting this as the stages progress
		float inputlevel = startlevel;
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleA = (iirSampleA * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleA*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		float bridgerectifier = (smoothA + inputSample);
		smoothA = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleB = (iirSampleB * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleB*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		bridgerectifier = (smoothB + inputSample);
		smoothB = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		
		outSample = (inputSample * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
		dram->fixB[fix_sL1] = (inputSample * dram->fixB[fix_a1]) - (outSample * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
		dram->fixB[fix_sL2] = (inputSample * dram->fixB[fix_a2]) - (outSample * dram->fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleC = (iirSampleC * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleC*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		bridgerectifier = (smoothC + inputSample);
		smoothC = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleD = (iirSampleD * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleD*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		bridgerectifier = (smoothD + inputSample);
		smoothD = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixC[fix_a0]) + dram->fixC[fix_sL1];
		dram->fixC[fix_sL1] = (inputSample * dram->fixC[fix_a1]) - (outSample * dram->fixC[fix_b1]) + dram->fixC[fix_sL2];
		dram->fixC[fix_sL2] = (inputSample * dram->fixC[fix_a2]) - (outSample * dram->fixC[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleE = (iirSampleE * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleE*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		bridgerectifier = (smoothE + inputSample);
		smoothE = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleF = (iirSampleF * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleF*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		bridgerectifier = (smoothF + inputSample);
		smoothF = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixD[fix_a0]) + dram->fixD[fix_sL1];
		dram->fixD[fix_sL1] = (inputSample * dram->fixD[fix_a1]) - (outSample * dram->fixD[fix_b1]) + dram->fixD[fix_sL2];
		dram->fixD[fix_sL2] = (inputSample * dram->fixD[fix_a2]) - (outSample * dram->fixD[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleG = (iirSampleG * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleG*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		bridgerectifier = (smoothG + inputSample);
		smoothG = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleH = (iirSampleH * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleH*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		bridgerectifier = (smoothH + inputSample);
		smoothH = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixE[fix_a0]) + dram->fixE[fix_sL1];
		dram->fixE[fix_sL1] = (inputSample * dram->fixE[fix_a1]) - (outSample * dram->fixE[fix_b1]) + dram->fixE[fix_sL2];
		dram->fixE[fix_sL2] = (inputSample * dram->fixE[fix_a2]) - (outSample * dram->fixE[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleI = (iirSampleI * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleI*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		bridgerectifier = (smoothI + inputSample);
		smoothI = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleJ = (iirSampleJ * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleJ*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		bridgerectifier = (smoothJ + inputSample);
		smoothJ = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixF[fix_a0]) + dram->fixF[fix_sL1];
		dram->fixF[fix_sL1] = (inputSample * dram->fixF[fix_a1]) - (outSample * dram->fixF[fix_b1]) + dram->fixF[fix_sL2];
		dram->fixF[fix_sL2] = (inputSample * dram->fixF[fix_a2]) - (outSample * dram->fixF[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleK = (iirSampleK * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleK*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		bridgerectifier = (smoothK + inputSample);
		smoothK = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleL = (iirSampleL * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleL*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654f) * (fabs(inputSample) * 0.654f) );
		//overdrive
		bridgerectifier = (smoothL + inputSample);
		smoothL = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		iirLowpass = (iirLowpass * (1 - toneEQ)) + (inputSample * toneEQ);
		inputSample = iirLowpass;
		//lowpass. The only one of this type.
		
		iirSpkA = (iirSpkA * (1 -  BEQ)) + (inputSample * BEQ);
		//extra lowpass for 4*12" speakers
		if (count < 0 || count > 128) {count = 128;}
		float resultB = 0.0f;
		if (flip)
		{
			dram->Odd[count+128] = dram->Odd[count] = iirSpkA;
			resultB = (dram->Odd[count+down] + dram->Odd[count+side] + dram->Odd[count+diagonal]);
		}
		else
		{
			dram->Even[count+128] = dram->Even[count] = iirSpkA;
			resultB = (dram->Even[count+down] + dram->Even[count+side] + dram->Even[count+diagonal]);
		}
		count--;
		iirSpkB = (iirSpkB * (1 - BEQ)) + (resultB * BEQ);
		inputSample += (iirSpkB * bleed);
		//extra lowpass for 4*12" speakers
		
		bridgerectifier = fabs(inputSample*outputlevel);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		
		iirSub = (iirSub * (1 - BEQ)) + (inputSample * BEQ);
		inputSample += (iirSub * bassfill * outputlevel);
		
		float randy = ((rand()/(float)RAND_MAX)*0.053f);
		inputSample = ((inputSample*(1-randy))+(storeSample*randy))*outputlevel;
		storeSample = inputSample;
		flip = !flip;
		
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
			
			dram->b[84] = dram->b[83]; dram->b[83] = dram->b[82]; dram->b[82] = dram->b[81]; dram->b[81] = dram->b[80]; dram->b[80] = dram->b[79]; 
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
			inputSample += (dram->b[1] * (1.31698250313308396f  - (0.08140616497621633f*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (1.47229016949915326f  - (0.27680278993637253f*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (1.30410109086044956f  - (0.35629113432046489f*fabs(dram->b[3]))));
			inputSample += (dram->b[4] * (0.81766210474551260f  - (0.26808782337659753f*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (0.19868872545506663f  - (0.11105517193919669f*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.39115909132567039f  - (0.12630622002682679f*fabs(dram->b[6]))));
			inputSample -= (dram->b[7] * (0.76881891559343574f  - (0.40879849500403143f*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.87146861782680340f  - (0.59529560488000599f*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.79504575932563670f  - (0.60877047551611796f*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.61653017622406314f  - (0.47662851438557335f*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.40718195794382067f  - (0.24955839378539713f*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.31794900040616203f  - (0.04169792259600613f*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.41075032540217843f  + (0.00368483996076280f*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.56901352922170667f  - (0.11027360805893105f*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.62443222391889264f  - (0.22198075154245228f*fabs(dram->b[15]))));
			inputSample -= (dram->b[16] * (0.53462856723129204f  - (0.22933544545324852f*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.34441703361995046f  - (0.12956809502269492f*fabs(dram->b[17]))));
			inputSample -= (dram->b[18] * (0.13947052337867882f  + (0.00339775055962799f*fabs(dram->b[18]))));
			inputSample += (dram->b[19] * (0.03771252648928484f  - (0.10863931549251718f*fabs(dram->b[19]))));
			inputSample += (dram->b[20] * (0.18280210770271693f  - (0.17413646599296417f*fabs(dram->b[20]))));
			inputSample += (dram->b[21] * (0.24621986701761467f  - (0.14547053270435095f*fabs(dram->b[21]))));
			inputSample += (dram->b[22] * (0.22347075142737360f  - (0.02493869490104031f*fabs(dram->b[22]))));
			inputSample += (dram->b[23] * (0.14346348482123716f  + (0.11284054747963246f*fabs(dram->b[23]))));
			inputSample += (dram->b[24] * (0.00834364862916028f  + (0.24284684053733926f*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.11559740296078347f  - (0.32623054435304538f*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.18067604561283060f  - (0.32311481551122478f*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.22927997789035612f  - (0.26991539052832925f*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.28487666578669446f  - (0.22437227250279349f*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.31992973037153838f  - (0.15289876100963865f*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.35174606303520733f  - (0.05656293023086628f*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.36894898011375254f  + (0.04333925421463558f*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.32567576055307507f  + (0.14594589410921388f*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.27440135050585784f  + (0.15529667398122521f*fabs(dram->b[33]))));
			inputSample -= (dram->b[34] * (0.21998973785078091f  + (0.05083553737157104f*fabs(dram->b[34]))));
			inputSample -= (dram->b[35] * (0.10323624876862457f  - (0.04651829594199963f*fabs(dram->b[35]))));
			inputSample += (dram->b[36] * (0.02091603687851074f  + (0.12000046818439322f*fabs(dram->b[36]))));
			inputSample += (dram->b[37] * (0.11344930914138468f  + (0.17697142512225839f*fabs(dram->b[37]))));
			inputSample += (dram->b[38] * (0.22766779627643968f  + (0.13645102964003858f*fabs(dram->b[38]))));
			inputSample += (dram->b[39] * (0.38378309953638229f  - (0.01997653307333791f*fabs(dram->b[39]))));
			inputSample += (dram->b[40] * (0.52789400804568076f  - (0.21409137428422448f*fabs(dram->b[40]))));
			inputSample += (dram->b[41] * (0.55444630296938280f  - (0.32331980931576626f*fabs(dram->b[41]))));
			inputSample += (dram->b[42] * (0.42333237669264601f  - (0.26855847463044280f*fabs(dram->b[42]))));
			inputSample += (dram->b[43] * (0.21942831522035078f  - (0.12051365248820624f*fabs(dram->b[43]))));
			inputSample -= (dram->b[44] * (0.00584169427830633f  - (0.03706970171280329f*fabs(dram->b[44]))));
			inputSample -= (dram->b[45] * (0.24279799124660351f  - (0.17296440491477982f*fabs(dram->b[45]))));
			inputSample -= (dram->b[46] * (0.40173760787507085f  - (0.21717989835163351f*fabs(dram->b[46]))));
			inputSample -= (dram->b[47] * (0.43930035724188155f  - (0.16425928481378199f*fabs(dram->b[47]))));
			inputSample -= (dram->b[48] * (0.41067765934041811f  - (0.10390115786636855f*fabs(dram->b[48]))));
			inputSample -= (dram->b[49] * (0.34409235547165967f  - (0.07268159377411920f*fabs(dram->b[49]))));
			inputSample -= (dram->b[50] * (0.26542883122568151f  - (0.05483457497365785f*fabs(dram->b[50]))));
			inputSample -= (dram->b[51] * (0.22024754776138800f  - (0.06484897950087598f*fabs(dram->b[51]))));
			inputSample -= (dram->b[52] * (0.20394367993632415f  - (0.08746309731952180f*fabs(dram->b[52]))));
			inputSample -= (dram->b[53] * (0.17565242431124092f  - (0.07611309538078760f*fabs(dram->b[53]))));
			inputSample -= (dram->b[54] * (0.10116623231246825f  - (0.00642818706295112f*fabs(dram->b[54]))));
			inputSample -= (dram->b[55] * (0.00782648272053632f  + (0.08004141267685004f*fabs(dram->b[55]))));
			inputSample += (dram->b[56] * (0.05059046006747323f  - (0.12436676387548490f*fabs(dram->b[56]))));
			inputSample += (dram->b[57] * (0.06241531553254467f  - (0.11530779547021434f*fabs(dram->b[57]))));
			inputSample += (dram->b[58] * (0.04952694587101836f  - (0.08340945324333944f*fabs(dram->b[58]))));
			inputSample += (dram->b[59] * (0.00843873294401687f  - (0.03279659052562903f*fabs(dram->b[59]))));
			inputSample -= (dram->b[60] * (0.05161338949440241f  - (0.03428181149163798f*fabs(dram->b[60]))));
			inputSample -= (dram->b[61] * (0.08165520146902012f  - (0.08196746092283110f*fabs(dram->b[61]))));
			inputSample -= (dram->b[62] * (0.06639532849935320f  - (0.09797462781896329f*fabs(dram->b[62]))));
			inputSample -= (dram->b[63] * (0.02953430910661621f  - (0.09175612938515763f*fabs(dram->b[63]))));
			inputSample += (dram->b[64] * (0.00741058547442938f  + (0.05442091048731967f*fabs(dram->b[64]))));
			inputSample += (dram->b[65] * (0.01832866125391727f  + (0.00306243693643687f*fabs(dram->b[65]))));
			inputSample += (dram->b[66] * (0.00526964230373573f  - (0.04364102661136410f*fabs(dram->b[66]))));
			inputSample -= (dram->b[67] * (0.00300984373848200f  + (0.09742737841278880f*fabs(dram->b[67]))));
			inputSample -= (dram->b[68] * (0.00413616769576694f  + (0.14380661694523073f*fabs(dram->b[68]))));
			inputSample -= (dram->b[69] * (0.00588769034931419f  + (0.16012843578892538f*fabs(dram->b[69]))));
			inputSample -= (dram->b[70] * (0.00688588239450581f  + (0.14074464279305798f*fabs(dram->b[70]))));
			inputSample -= (dram->b[71] * (0.02277307992926315f  + (0.07914752191801366f*fabs(dram->b[71]))));
			inputSample -= (dram->b[72] * (0.04627166091180877f  - (0.00192787268067208f*fabs(dram->b[72]))));
			inputSample -= (dram->b[73] * (0.05562045897455786f  - (0.05932868727665747f*fabs(dram->b[73]))));
			inputSample -= (dram->b[74] * (0.05134243784922165f  - (0.08245334798868090f*fabs(dram->b[74]))));
			inputSample -= (dram->b[75] * (0.04719409472239919f  - (0.07498680629253825f*fabs(dram->b[75]))));
			inputSample -= (dram->b[76] * (0.05889738914266415f  - (0.06116127018043697f*fabs(dram->b[76]))));
			inputSample -= (dram->b[77] * (0.09428363535111127f  - (0.06535868867863834f*fabs(dram->b[77]))));
			inputSample -= (dram->b[78] * (0.15181756953225126f  - (0.08982979655234427f*fabs(dram->b[78]))));
			inputSample -= (dram->b[79] * (0.20878969456036670f  - (0.10761070891499538f*fabs(dram->b[79]))));
			inputSample -= (dram->b[80] * (0.22647885581813790f  - (0.08462542510349125f*fabs(dram->b[80]))));
			inputSample -= (dram->b[81] * (0.19723482443646323f  - (0.02665160920736287f*fabs(dram->b[81]))));
			inputSample -= (dram->b[82] * (0.16441643451155163f  + (0.02314691954338197f*fabs(dram->b[82]))));
			inputSample -= (dram->b[83] * (0.15201914054931515f  + (0.04424903493886839f*fabs(dram->b[83]))));
			inputSample -= (dram->b[84] * (0.15454370641307855f  + (0.04223203797913008f*fabs(dram->b[84]))));
			
			temp = (inputSample + smoothCabB)/3.0f;
			smoothCabB = inputSample;
			inputSample = temp/4.0f;
			
			
			randy = ((float(fpd)/UINT32_MAX)*0.057f);
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
	lastSample = 0.0;
	storeSample = 0.0;
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
	smoothL = 0.0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	iirSampleG = 0.0;
	iirSampleH = 0.0;
	iirSampleI = 0.0;
	iirSampleJ = 0.0;
	iirSampleK = 0.0;
	iirSampleL = 0.0;
	iirLowpass = 0.0;
	iirSpkA = 0.0;
	iirSpkB = 0.0;
	iirSub = 0.0;
	register int fcount;
	for (fcount = 0; fcount < 257; fcount++) {dram->Odd[fcount] = 0.0; dram->Even[fcount] = 0.0;}
	count = 0;
	flip = false; //amp
	
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
