#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BigAmp"
#define AIRWINDOWS_DESCRIPTION "A very flexible amplike thing with a taste for the bizarre."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','p' )
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
 
		float lastSample;
		float storeSample;
		float lastSlew;
		float iirSampleA;
		float iirSampleB;
		float iirSampleC;
		float iirSampleD;
		float iirSampleE;
		float iirSampleF;
		float iirSampleG;
		float iirSampleH;
		float iirSampleI;
		float iirSampleJ;
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
			float Odd[257];
		float Even[257];
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
	float basstrim = GetParameter( kParam_Two );
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
	float inputlevel = bassfill*3.0f;
	
	float samplerate = GetSampleRate();
	float EQ = (GetParameter( kParam_Two )/ samplerate)*22050.0f;
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
		
		float skew = (inputSample - lastSample);
		lastSample = inputSample;
		//skew will be direction/angle
		float bridgerectifier = fabs(skew);
		if (bridgerectifier > 3.1415926f) bridgerectifier = 3.1415926f;
		//for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skew > 0) skew = bridgerectifier;
		else skew = -bridgerectifier;
		//skew is now sined and clamped and then re-amplified again
		
		skew *= inputSample;
		skew = (skew+(skew*basstrim))/2.0f;
		inputSample *= basstrim;
		float basscut = basstrim;
		//we're going to be shifting this as the stages progress
		
		outSample = (inputSample * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
		dram->fixB[fix_sL1] = (inputSample * dram->fixB[fix_a1]) - (outSample * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
		dram->fixB[fix_sL2] = (inputSample * dram->fixB[fix_a2]) - (outSample * dram->fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		float offset = (1.0f - EQ) + (fabs(inputSample)*EQ);
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		iirSampleA = (iirSampleA * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - (iirSampleA*basscut);
		//highpass
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633f+skew))+(bridgerectifier*(1.57079633f+skew));
		else inputSample = (inputSample*(-0.57079633f+skew))-(bridgerectifier*(1.57079633f+skew));
		//overdrive
		iirSampleB = (iirSampleB * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - (iirSampleB*basscut);
		basscut /= 2.0f;
		//highpass. Use offset from before gain stage
		//finished first gain stage
		
		outSample = (inputSample * dram->fixC[fix_a0]) + dram->fixC[fix_sL1];
		dram->fixC[fix_sL1] = (inputSample * dram->fixC[fix_a1]) - (outSample * dram->fixC[fix_b1]) + dram->fixC[fix_sL2];
		dram->fixC[fix_sL2] = (inputSample * dram->fixC[fix_a2]) - (outSample * dram->fixC[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		skew /= 2.0f;
		offset = (1.0f + offset) / 2.0f;
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633f+skew))+(bridgerectifier*(1.57079633f+skew));
		else inputSample = (inputSample*(-0.57079633f+skew))-(bridgerectifier*(1.57079633f+skew));
		//overdrive
		iirSampleC = (iirSampleC * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - (iirSampleC*basscut);
		basscut /= 2.0f;
		//highpass.
		iirSampleD = (iirSampleD * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = iirSampleD;
		//lowpass. Use offset from before gain stage
		
		outSample = (inputSample * dram->fixD[fix_a0]) + dram->fixD[fix_sL1];
		dram->fixD[fix_sL1] = (inputSample * dram->fixD[fix_a1]) - (outSample * dram->fixD[fix_b1]) + dram->fixD[fix_sL2];
		dram->fixD[fix_sL2] = (inputSample * dram->fixD[fix_a2]) - (outSample * dram->fixD[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		skew /= 2.0f;
		offset = (1.0f + offset) / 2.0f;
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633f+skew))+(bridgerectifier*(1.57079633f+skew));
		else inputSample = (inputSample*(-0.57079633f+skew))-(bridgerectifier*(1.57079633f+skew));
		//overdrive
		iirSampleE = (iirSampleE * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - (iirSampleE*basscut);
		//we don't need to do basscut again, that was the last one
		//highpass.
		iirSampleF = (iirSampleF * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = iirSampleF;
		//lowpass. Use offset from before gain stage
		
		outSample = (inputSample * dram->fixE[fix_a0]) + dram->fixE[fix_sL1];
		dram->fixE[fix_sL1] = (inputSample * dram->fixE[fix_a1]) - (outSample * dram->fixE[fix_b1]) + dram->fixE[fix_sL2];
		dram->fixE[fix_sL2] = (inputSample * dram->fixE[fix_a2]) - (outSample * dram->fixE[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		skew /= 2.0f;
		offset = (1.0f + offset) / 2.0f;
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633f+skew))+(bridgerectifier*(1.57079633f+skew));
		else inputSample = (inputSample*(-0.57079633f+skew))-(bridgerectifier*(1.57079633f+skew));
		//output stage has less gain, no highpass, straight lowpass
		iirSampleG = (iirSampleG * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = iirSampleG;
		//lowpass. Use offset from before gain stage
		
		iirSampleH = (iirSampleH * (1 - (offset * BEQ))) + (inputSample * (offset * BEQ));
		//extra lowpass for 4*12" speakers
		if (count < 0 || count > 128) count = 128;
		float resultB = 0.0f;
		
		if (flip)
		{
			dram->Odd[count+128] = dram->Odd[count] = iirSampleH;
			resultB = (dram->Odd[count+down] + dram->Odd[count+side] + dram->Odd[count+diagonal]);
		} else {
			dram->Even[count+128] = dram->Even[count] = iirSampleH;
			resultB = (dram->Even[count+down] + dram->Even[count+side] + dram->Even[count+diagonal]);
		}
		count--;
		
		iirSampleI = (iirSampleI * (1 - (offset * BEQ))) + (resultB * (offset * BEQ));
		inputSample += (iirSampleI*bleed);
		//extra lowpass for 4*12" speakers
		
		iirSampleJ = (iirSampleJ * (1 - (offset * BEQ))) + (inputSample * (offset * BEQ));
		inputSample += (iirSampleJ * bassfill);
		
		inputSample = sin(inputSample*outputlevel);
		
		float randy = ((float(fpd)/UINT32_MAX)*0.04f);
		inputSample = ((inputSample*(1-randy))+(storeSample*randy))*outputlevel;
		storeSample = inputSample;
		
		outSample = (inputSample * dram->fixF[fix_a0]) + dram->fixF[fix_sL1];
		dram->fixF[fix_sL1] = (inputSample * dram->fixF[fix_a1]) - (outSample * dram->fixF[fix_b1]) + dram->fixF[fix_sL2];
		dram->fixF[fix_sL2] = (inputSample * dram->fixF[fix_a2]) - (outSample * dram->fixF[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
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
			
			dram->b[81] = dram->b[80]; dram->b[80] = dram->b[79]; 
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
			
			inputSample += (dram->b[1] * (1.35472031405494242f  + (0.00220914099195157f*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (1.63534207755253003f  - (0.11406232654509685f*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (1.82334575691525869f  - (0.42647194712964054f*fabs(dram->b[3]))));
			inputSample += (dram->b[4] * (1.86156386235405868f  - (0.76744187887586590f*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (1.67332739338852599f  - (0.95161997324293013f*fabs(dram->b[5]))));
			inputSample += (dram->b[6] * (1.25054130794899021f  - (0.98410433514572859f*fabs(dram->b[6]))));
			inputSample += (dram->b[7] * (0.70049121047281737f  - (0.87375612110718992f*fabs(dram->b[7]))));
			inputSample += (dram->b[8] * (0.15291791448081560f  - (0.61195266024519046f*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.37301992914152693f  + (0.16755422915252094f*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.76568539228498433f  - (0.28554435228965386f*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.95726568749937369f  - (0.61659719162806048f*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (1.01273552193911032f  - (0.81827288407943954f*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.93920108117234447f  - (0.80077111864205874f*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.79831898832953974f  - (0.65814750339694406f*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.64200088100452313f  - (0.46135833001232618f*fabs(dram->b[15]))));
			inputSample -= (dram->b[16] * (0.48807302802822128f  - (0.15506178974799034f*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.36545171501947982f  + (0.16126103769376721f*fabs(dram->b[17]))));
			inputSample -= (dram->b[18] * (0.31469581455759105f  + (0.32250870039053953f*fabs(dram->b[18]))));
			inputSample -= (dram->b[19] * (0.36893534817945800f  + (0.25409418897237473f*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.41092557722975687f  + (0.13114730488878301f*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.38584044480710594f  + (0.06825323739722661f*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.33378434007178670f  + (0.04144255489164217f*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.26144203061699706f  + (0.06031313105098152f*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.25818342000920502f  + (0.03642289242586355f*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.28096018498822661f  + (0.00976973667327174f*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.25845682019095384f  + (0.02749015358080831f*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.26655607865953096f  - (0.00329839675455690f*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.30590085026938518f  - (0.07375043215328811f*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.32875683916470899f  - (0.12454134857516502f*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.38166643180506560f  - (0.19973911428609989f*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.49068186937289598f  - (0.34785166842136384f*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.60274753867622777f  - (0.48685038872711034f*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.65944678627090636f  - (0.49844657885975518f*fabs(dram->b[33]))));
			inputSample -= (dram->b[34] * (0.64488955808717285f  - (0.40514406499806987f*fabs(dram->b[34]))));
			inputSample -= (dram->b[35] * (0.55818730353434354f  - (0.28029870614987346f*fabs(dram->b[35]))));
			inputSample -= (dram->b[36] * (0.43110859113387556f  - (0.15373504582939335f*fabs(dram->b[36]))));
			inputSample -= (dram->b[37] * (0.37726894966096269f  - (0.11570983506028532f*fabs(dram->b[37]))));
			inputSample -= (dram->b[38] * (0.39953242355200935f  - (0.17879231130484088f*fabs(dram->b[38]))));
			inputSample -= (dram->b[39] * (0.36726676379100875f  - (0.22013553023983223f*fabs(dram->b[39]))));
			inputSample -= (dram->b[40] * (0.27187029469227386f  - (0.18461171768478427f*fabs(dram->b[40]))));
			inputSample -= (dram->b[41] * (0.21109334552321635f  - (0.14497481318083569f*fabs(dram->b[41]))));
			inputSample -= (dram->b[42] * (0.19808797405293213f  - (0.14916579928186940f*fabs(dram->b[42]))));
			inputSample -= (dram->b[43] * (0.16287926785495671f  - (0.15146098461120627f*fabs(dram->b[43]))));
			inputSample -= (dram->b[44] * (0.11086621477163359f  - (0.13182973443924018f*fabs(dram->b[44]))));
			inputSample -= (dram->b[45] * (0.07531043236890560f  - (0.08062172796472888f*fabs(dram->b[45]))));
			inputSample -= (dram->b[46] * (0.01747364473230771f  + (0.02201865873632456f*fabs(dram->b[46]))));
			inputSample += (dram->b[47] * (0.03080279125662693f  - (0.08721756240972101f*fabs(dram->b[47]))));
			inputSample += (dram->b[48] * (0.02354148659185142f  - (0.06376361763053796f*fabs(dram->b[48]))));
			inputSample -= (dram->b[49] * (0.02835772372098715f  + (0.00589978513642627f*fabs(dram->b[49]))));
			inputSample -= (dram->b[50] * (0.08983370744565244f  - (0.02350960427706536f*fabs(dram->b[50]))));
			inputSample -= (dram->b[51] * (0.14148947620055380f  - (0.03329826628693369f*fabs(dram->b[51]))));
			inputSample -= (dram->b[52] * (0.17576502674572581f  - (0.06507546651241880f*fabs(dram->b[52]))));
			inputSample -= (dram->b[53] * (0.17168865666573860f  - (0.07734801128437317f*fabs(dram->b[53]))));
			inputSample -= (dram->b[54] * (0.14107027738292105f  - (0.03136459344220402f*fabs(dram->b[54]))));
			inputSample -= (dram->b[55] * (0.12287163395380074f  + (0.01933408169185258f*fabs(dram->b[55]))));
			inputSample -= (dram->b[56] * (0.12276622398112971f  + (0.01983508766241737f*fabs(dram->b[56]))));
			inputSample -= (dram->b[57] * (0.12349721440213673f  - (0.01111031415304768f*fabs(dram->b[57]))));
			inputSample -= (dram->b[58] * (0.08649454142716655f  + (0.02252815645513927f*fabs(dram->b[58]))));
			inputSample -= (dram->b[59] * (0.00953083685474757f  + (0.13778878548343007f*fabs(dram->b[59]))));
			inputSample += (dram->b[60] * (0.06045983158868478f  - (0.23966318224935096f*fabs(dram->b[60]))));
			inputSample += (dram->b[61] * (0.09053229817093242f  - (0.27190119941572544f*fabs(dram->b[61]))));
			inputSample += (dram->b[62] * (0.08112662178843048f  - (0.22456862606452327f*fabs(dram->b[62]))));
			inputSample += (dram->b[63] * (0.07503525686243730f  - (0.14330154410548213f*fabs(dram->b[63]))));
			inputSample += (dram->b[64] * (0.07372595404399729f  - (0.06185193766408734f*fabs(dram->b[64]))));
			inputSample += (dram->b[65] * (0.06073789200080433f  + (0.01261857435786178f*fabs(dram->b[65]))));
			inputSample += (dram->b[66] * (0.04616712695742254f  + (0.05851771967084609f*fabs(dram->b[66]))));
			inputSample += (dram->b[67] * (0.01036235510345900f  + (0.08286534414423796f*fabs(dram->b[67]))));
			inputSample -= (dram->b[68] * (0.03708389413229191f  - (0.06695282381039531f*fabs(dram->b[68]))));
			inputSample -= (dram->b[69] * (0.07092204876981217f  - (0.01915829199112784f*fabs(dram->b[69]))));
			inputSample -= (dram->b[70] * (0.09443579589460312f  + (0.01210082455316246f*fabs(dram->b[70]))));
			inputSample -= (dram->b[71] * (0.07824038577769601f  + (0.06121988546065113f*fabs(dram->b[71]))));
			inputSample -= (dram->b[72] * (0.00854730633079399f  + (0.14468518752295506f*fabs(dram->b[72]))));
			inputSample += (dram->b[73] * (0.06845589924191028f  - (0.18902431382592944f*fabs(dram->b[73]))));
			inputSample += (dram->b[74] * (0.10351569998375465f  - (0.13204443060279647f*fabs(dram->b[74]))));
			inputSample += (dram->b[75] * (0.10513368758532179f  - (0.02993199294485649f*fabs(dram->b[75]))));
			inputSample += (dram->b[76] * (0.08896978950235003f  + (0.04074499273825906f*fabs(dram->b[76]))));
			inputSample += (dram->b[77] * (0.03697537734050980f  + (0.09217751130846838f*fabs(dram->b[77]))));
			inputSample -= (dram->b[78] * (0.04014322441280276f  - (0.14062297149365666f*fabs(dram->b[78]))));
			inputSample -= (dram->b[79] * (0.10505934581398618f  - (0.16988861157275814f*fabs(dram->b[79]))));
			inputSample -= (dram->b[80] * (0.13937661651676272f  - (0.15083294570551492f*fabs(dram->b[80]))));
			inputSample -= (dram->b[81] * (0.13183458845108439f  - (0.06657454442471208f*fabs(dram->b[81]))));
			
			temp = (inputSample + smoothCabB)/3.0f;
			smoothCabB = inputSample;
			inputSample = temp/4.0f;
			
			
			randy = ((float(fpd)/UINT32_MAX)*0.05f);
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
	lastSlew = 0.0;
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
	for (int fcount = 0; fcount < 257; fcount++) {dram->Odd[fcount] = 0.0; dram->Even[fcount] = 0.0;}
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
