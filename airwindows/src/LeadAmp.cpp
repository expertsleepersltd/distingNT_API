#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "LeadAmp"
#define AIRWINDOWS_DESCRIPTION "An amp sim with a clear, penetrating, vocal tone."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','L','e','p' )
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
		Float32 iirLowpass;
		Float32 iirSpkA;
		Float32 iirSpkB;
		Float32 iirSub;
		bool flip;
		int count;
		
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
	float startlevel = bassfill;
	float samplerate = GetSampleRate();
	float basstrim = bassfill / 10.0f;
	float toneEQ = (GetParameter( kParam_Two ) / samplerate)*22050.0f;
	float EQ = (basstrim / samplerate)*22050.0f;
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
				
		float basscut = 0.99f;
		//we're going to be shifting this as the stages progress
		float inputlevel = startlevel;
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleA = (iirSampleA * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleA*basscut);
		//highpass
		float bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//overdrive
		bridgerectifier = (smoothA + inputSample);
		smoothA = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		
		outSample = (inputSample * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
		dram->fixB[fix_sL1] = (inputSample * dram->fixB[fix_a1]) - (outSample * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
		dram->fixB[fix_sL2] = (inputSample * dram->fixB[fix_a2]) - (outSample * dram->fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleB = (iirSampleB * (1.0f - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleB*basscut);
		//highpass
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothB + inputSample);
		smoothB = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixC[fix_a0]) + dram->fixC[fix_sL1];
		dram->fixC[fix_sL1] = (inputSample * dram->fixC[fix_a1]) - (outSample * dram->fixC[fix_b1]) + dram->fixC[fix_sL2];
		dram->fixC[fix_sL2] = (inputSample * dram->fixC[fix_a2]) - (outSample * dram->fixC[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleC = (iirSampleC * (1.0f - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleC*basscut);
		//highpass
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothC + inputSample);
		smoothC = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleD = (iirSampleD * (1.0f - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleD*basscut);
		//highpass
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothD + inputSample);
		smoothD = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixD[fix_a0]) + dram->fixD[fix_sL1];
		dram->fixD[fix_sL1] = (inputSample * dram->fixD[fix_a1]) - (outSample * dram->fixD[fix_b1]) + dram->fixD[fix_sL2];
		dram->fixD[fix_sL2] = (inputSample * dram->fixD[fix_a2]) - (outSample * dram->fixD[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleE = (iirSampleE * (1.0f - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleE*basscut);
		//highpass
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothE + inputSample);
		smoothE = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleF = (iirSampleF * (1.0f - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleF*basscut);
		//highpass
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothF + inputSample);
		smoothF = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixE[fix_a0]) + dram->fixE[fix_sL1];
		dram->fixE[fix_sL1] = (inputSample * dram->fixE[fix_a1]) - (outSample * dram->fixE[fix_b1]) + dram->fixE[fix_sL2];
		dram->fixE[fix_sL2] = (inputSample * dram->fixE[fix_a2]) - (outSample * dram->fixE[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleG = (iirSampleG * (1.0f - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleG*basscut);
		//highpass
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothG + inputSample);
		smoothG = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleH = (iirSampleH * (1.0f - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleH*basscut);
		//highpass
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothH + inputSample);
		smoothH = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixF[fix_a0]) + dram->fixF[fix_sL1];
		dram->fixF[fix_sL1] = (inputSample * dram->fixF[fix_a1]) - (outSample * dram->fixF[fix_b1]) + dram->fixF[fix_sL2];
		dram->fixF[fix_sL2] = (inputSample * dram->fixF[fix_a2]) - (outSample * dram->fixF[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleI = (iirSampleI * (1.0f - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleI*basscut);
		//highpass
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothI + inputSample);
		smoothI = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleJ = (iirSampleJ * (1.0f - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleJ*basscut);
		//highpass
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothJ + inputSample);
		smoothJ = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0f)+1.0f)/8.0f;
		iirSampleK = (iirSampleK * (1.0f - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleK*basscut);
		//highpass
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothK + inputSample);
		smoothK = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		iirLowpass = (iirLowpass * (1.0f - toneEQ)) + (inputSample * toneEQ);
		inputSample = iirLowpass;
		//lowpass. The only one of this type.
		
		iirSpkA = (iirSpkA * (1.0f -  BEQ)) + (inputSample * BEQ);
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
		inputSample += (iirSpkB*bleed);
		//extra lowpass for 4*12" speakers
		
		iirSub = (iirSub * (1 - BEQ)) + (inputSample * BEQ);
		inputSample += (iirSub * bassfill);
		
		
		bridgerectifier = fabs(inputSample*outputlevel);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0.0f) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		
		float randy = ((float(fpd)/UINT32_MAX)*0.084f);
		inputSample = ((inputSample*(1.0f-randy))+(storeSample*randy))*outputlevel;
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
			
			dram->b[85] = dram->b[84]; dram->b[84] = dram->b[83]; dram->b[83] = dram->b[82]; dram->b[82] = dram->b[81]; dram->b[81] = dram->b[80]; dram->b[80] = dram->b[79]; 
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
			inputSample += (dram->b[1] * (1.30406584776167445f  - (0.01410622186823351f*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (1.09350974154373559f  + (0.34478044709202327f*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (0.52285510059938256f  + (0.84225842837363574f*fabs(dram->b[3]))));
			inputSample -= (dram->b[4] * (0.00018126260714707f  - (1.02446537989058117f*fabs(dram->b[4]))));
			inputSample -= (dram->b[5] * (0.34943699771860115f  - (0.84094709567790016f*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.53068048407937285f  - (0.49231169327705593f*fabs(dram->b[6]))));
			inputSample -= (dram->b[7] * (0.48631669406792399f  - (0.08965111766223610f*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.28099201947014130f  + (0.23921137841068607f*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.10333290012666248f  + (0.35058962687321482f*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.06605032198166226f  + (0.23447405567823365f*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.10485808661261729f  + (0.05025985449763527f*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.13231190973014911f  - (0.05484648240248013f*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.12926184767180304f  - (0.04054223744746116f*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.13802696739839460f  + (0.01876754906568237f*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.16548980700926913f  + (0.06772130758771169f*fabs(dram->b[15]))));
			inputSample -= (dram->b[16] * (0.14469310965751475f  + (0.10590928840978781f*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.07838457396093310f  + (0.13120101199677947f*fabs(dram->b[17]))));
			inputSample -= (dram->b[18] * (0.05123031606187391f  + (0.13883400806512292f*fabs(dram->b[18]))));
			inputSample -= (dram->b[19] * (0.08906103481939850f  + (0.07840461228402337f*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.13939265522625241f  + (0.01194366471800457f*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.14957600717294034f  + (0.07687598594361914f*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.14112708654047090f  + (0.20118461131186977f*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.14961020766492997f  + (0.30005716443826147f*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.16130382224652270f  + (0.40459872030013055f*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.15679868471080052f  + (0.47292767226083465f*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.16456530552807727f  + (0.45182121471666481f*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.16852385701909278f  + (0.38272684270752266f*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.13317562760966850f  + (0.28829580273670768f*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.09396196532150952f  + (0.18886898332071317f*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.10133496956734221f  + (0.11158788414137354f*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.16097596389376778f  + (0.02621299102374547f*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.21419006394821866f  - (0.03585678078834797f*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.21273234570555244f  - (0.02574469802924526f*fabs(dram->b[33]))));
			inputSample -= (dram->b[34] * (0.16934948798707830f  + (0.01354331184333835f*fabs(dram->b[34]))));
			inputSample -= (dram->b[35] * (0.11970436472852493f  + (0.04242183865883427f*fabs(dram->b[35]))));
			inputSample -= (dram->b[36] * (0.09329023656747724f  + (0.06890873292358397f*fabs(dram->b[36]))));
			inputSample -= (dram->b[37] * (0.10255328436608116f  + (0.11482972519137427f*fabs(dram->b[37]))));
			inputSample -= (dram->b[38] * (0.13883223352796811f  + (0.18016014431438840f*fabs(dram->b[38]))));
			inputSample -= (dram->b[39] * (0.16532844286979087f  + (0.24521957638633446f*fabs(dram->b[39]))));
			inputSample -= (dram->b[40] * (0.16254607738965438f  + (0.25669472097572482f*fabs(dram->b[40]))));
			inputSample -= (dram->b[41] * (0.15353207135544752f  + (0.15048064682912729f*fabs(dram->b[41]))));
			inputSample -= (dram->b[42] * (0.13039046390746015f  - (0.00200335414623601f*fabs(dram->b[42]))));
			inputSample -= (dram->b[43] * (0.06707245032180627f  - (0.06498125592578702f*fabs(dram->b[43]))));
			inputSample += (dram->b[44] * (0.01427326441869788f  + (0.01940451360783622f*fabs(dram->b[44]))));
			inputSample += (dram->b[45] * (0.06151238306578224f  - (0.07335755969763329f*fabs(dram->b[45]))));
			inputSample += (dram->b[46] * (0.04685840498892526f  - (0.14258849371688248f*fabs(dram->b[46]))));
			inputSample -= (dram->b[47] * (0.00950136304466093f  + (0.14379354707665129f*fabs(dram->b[47]))));
			inputSample -= (dram->b[48] * (0.06245771575493557f  + (0.07639718586346110f*fabs(dram->b[48]))));
			inputSample -= (dram->b[49] * (0.07159593175777741f  - (0.00595536565276915f*fabs(dram->b[49]))));
			inputSample -= (dram->b[50] * (0.03167929390245019f  - (0.03856769526301793f*fabs(dram->b[50]))));
			inputSample += (dram->b[51] * (0.01890898565110766f  + (0.00760539424271147f*fabs(dram->b[51]))));
			inputSample += (dram->b[52] * (0.04926161137832240f  - (0.06411014430053390f*fabs(dram->b[52]))));
			inputSample += (dram->b[53] * (0.05768814623421683f  - (0.15068618173358578f*fabs(dram->b[53]))));
			inputSample += (dram->b[54] * (0.06144258297076708f  - (0.21200636329120301f*fabs(dram->b[54]))));
			inputSample += (dram->b[55] * (0.06348341960185613f  - (0.19620269813094307f*fabs(dram->b[55]))));
			inputSample += (dram->b[56] * (0.04877736350310589f  - (0.11864999881200111f*fabs(dram->b[56]))));
			inputSample += (dram->b[57] * (0.01010950997574472f  - (0.02630070679113791f*fabs(dram->b[57]))));
			inputSample -= (dram->b[58] * (0.02929178864801191f  - (0.04439260202207482f*fabs(dram->b[58]))));
			inputSample -= (dram->b[59] * (0.03484517126321562f  - (0.04508635396034735f*fabs(dram->b[59]))));
			inputSample -= (dram->b[60] * (0.00547176780437610f  - (0.00205637806941426f*fabs(dram->b[60]))));
			inputSample += (dram->b[61] * (0.02278296865283977f  - (0.00063732526427685f*fabs(dram->b[61]))));
			inputSample += (dram->b[62] * (0.02688982591366477f  + (0.05333738901586284f*fabs(dram->b[62]))));
			inputSample += (dram->b[63] * (0.01942012754957055f  + (0.10942832669749143f*fabs(dram->b[63]))));
			inputSample += (dram->b[64] * (0.01572585258756565f  + (0.11189204189054594f*fabs(dram->b[64]))));
			inputSample += (dram->b[65] * (0.01490550715016034f  + (0.04449822818925343f*fabs(dram->b[65]))));
			inputSample += (dram->b[66] * (0.01715683226376727f  - (0.06944648050933899f*fabs(dram->b[66]))));
			inputSample += (dram->b[67] * (0.02822659878011318f  - (0.17843652160132820f*fabs(dram->b[67]))));
			inputSample += (dram->b[68] * (0.03758307610456144f  - (0.21986013433664692f*fabs(dram->b[68]))));
			inputSample += (dram->b[69] * (0.03275008021608433f  - (0.15869878676112170f*fabs(dram->b[69]))));
			inputSample += (dram->b[70] * (0.01855749786752354f  - (0.02337224995718105f*fabs(dram->b[70]))));
			inputSample += (dram->b[71] * (0.00217095395782931f  + (0.10971764224593601f*fabs(dram->b[71]))));
			inputSample -= (dram->b[72] * (0.01851381451105007f  - (0.17214910008793413f*fabs(dram->b[72]))));
			inputSample -= (dram->b[73] * (0.04722574936345419f  - (0.14341588977845254f*fabs(dram->b[73]))));
			inputSample -= (dram->b[74] * (0.07151540514482006f  - (0.04684695724814321f*fabs(dram->b[74]))));
			inputSample -= (dram->b[75] * (0.06827195484995092f  + (0.07022207121861397f*fabs(dram->b[75]))));
			inputSample -= (dram->b[76] * (0.03290227240464227f  + (0.16328400808152735f*fabs(dram->b[76]))));
			inputSample += (dram->b[77] * (0.01043861198275382f  - (0.20184486126076279f*fabs(dram->b[77]))));
			inputSample += (dram->b[78] * (0.03236563559476477f  - (0.17125821306380920f*fabs(dram->b[78]))));
			inputSample += (dram->b[79] * (0.02040121529932702f  - (0.09103660189829657f*fabs(dram->b[79]))));
			inputSample -= (dram->b[80] * (0.00509649513318102f  + (0.01170360991547489f*fabs(dram->b[80]))));
			inputSample -= (dram->b[81] * (0.01388353426600228f  - (0.03588955538451771f*fabs(dram->b[81]))));
			inputSample -= (dram->b[82] * (0.00523671715033842f  - (0.07068798057534148f*fabs(dram->b[82]))));
			inputSample += (dram->b[83] * (0.00665852487721137f  + (0.11666210640054926f*fabs(dram->b[83]))));
			inputSample += (dram->b[84] * (0.01593540832939290f  + (0.15844892856402149f*fabs(dram->b[84]))));
			inputSample += (dram->b[85] * (0.02080509201836796f  + (0.17186274420065850f*fabs(dram->b[85]))));
			
			temp = (inputSample + smoothCabB)/3.0f;
			smoothCabB = inputSample;
			inputSample = temp/4.0f;
			
			randy = ((float(fpd)/UINT32_MAX)*0.079f);
			drySample = ((((inputSample*(1.0f-randy))+(lastCabSample*randy))*wet)+(drySample*(1.0f-wet)))*outputlevel;
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
