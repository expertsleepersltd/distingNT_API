#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "MidAmp"
#define AIRWINDOWS_DESCRIPTION "A clean amp sim meant to work like a loud Twin or something of that nature."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','i','p' )
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
		Float32 lastSlew;
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 iirSampleC;
		Float32 iirSampleD;
		Float32 iirSampleE;
		Float32 iirSampleF;
		Float32 iirSampleG;
		Float32 iirSampleH;
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

	Float32 inputlevel = GetParameter( kParam_One )*3.0f;
	Float32 samplerate = GetSampleRate();
	Float32 EQ = (GetParameter( kParam_Two )/ samplerate)*22050.0f;
	Float32 basstrim = GetParameter( kParam_Two );
	Float32 outputlevel = GetParameter( kParam_Three );
	float wet = GetParameter( kParam_Four );
	
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= samplerate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check		
	
	Float32 bleed = outputlevel/6.0f;
	Float32 BEQ = (bleed / samplerate)*44100.0f;
	Float32 resultB;
	int side = (int)(0.0006092985f*samplerate);
	if (side > 126) side = 126;
	int down = side + 1;
	inputlevel = (inputlevel + (inputlevel*basstrim))/2.0f;
	
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
		
		outSample = (inputSample * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
		dram->fixB[fix_sL1] = (inputSample * dram->fixB[fix_a1]) - (outSample * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
		dram->fixB[fix_sL2] = (inputSample * dram->fixB[fix_a2]) - (outSample * dram->fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		float offset = (1.0f - EQ) + (fabs(inputSample)*EQ);
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		iirSampleA = (iirSampleA * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - (iirSampleA*0.8f);
		//highpass
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skew;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633f+skew))+(bridgerectifier*(1.57079633f+skew));
		else inputSample = (inputSample*(-0.57079633f+skew))-(bridgerectifier*(1.57079633f+skew));
		//overdrive
		iirSampleC = (iirSampleC * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - (iirSampleC*0.6f);
		//highpass
		skew /= 1.57079633f;
		//finished first gain stage
		
		outSample = (inputSample * dram->fixC[fix_a0]) + dram->fixC[fix_sL1];
		dram->fixC[fix_sL1] = (inputSample * dram->fixC[fix_a1]) - (outSample * dram->fixC[fix_b1]) + dram->fixC[fix_sL2];
		dram->fixC[fix_sL2] = (inputSample * dram->fixC[fix_a2]) - (outSample * dram->fixC[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		offset = (1.0f + offset) / 2.0f;
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633f+skew))+(bridgerectifier*(1.57079633f+skew));
		else inputSample = (inputSample*(-0.57079633f+skew))-(bridgerectifier*(1.57079633f+skew));
		//overdrive
		iirSampleG = (iirSampleG * (1 - EQ)) + (inputSample * EQ);
		inputSample = inputSample - (iirSampleG*0.4f);
		//highpass. no further basscut, no more highpasses
		iirSampleD = (iirSampleD * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = iirSampleD;
		skew /= 1.57079633f;
		//lowpass. Use offset from before gain stage
		
		outSample = (inputSample * dram->fixD[fix_a0]) + dram->fixD[fix_sL1];
		dram->fixD[fix_sL1] = (inputSample * dram->fixD[fix_a1]) - (outSample * dram->fixD[fix_b1]) + dram->fixD[fix_sL2];
		dram->fixD[fix_sL2] = (inputSample * dram->fixD[fix_a2]) - (outSample * dram->fixD[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633f+skew))+(bridgerectifier*(1.57079633f+skew));
		else inputSample = (inputSample*(-0.57079633f+skew))-(bridgerectifier*(1.57079633f+skew));
		//output stage has less gain, no highpass, straight lowpass
		iirSampleE = (iirSampleE * (1 - EQ)) + (inputSample * EQ);
		inputSample = iirSampleE;
		//lowpass. Use offset from before gain stage
		
		outSample = (inputSample * dram->fixE[fix_a0]) + dram->fixE[fix_sL1];
		dram->fixE[fix_sL1] = (inputSample * dram->fixE[fix_a1]) - (outSample * dram->fixE[fix_b1]) + dram->fixE[fix_sL2];
		dram->fixE[fix_sL2] = (inputSample * dram->fixE[fix_a2]) - (outSample * dram->fixE[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		iirSampleF = (iirSampleF * (1 - (offset * BEQ))) + (inputSample * (offset * BEQ));
		//extra lowpass for 4*12" speakers
		if (count < 0 || count > 128) {count = 128;}
		if (flip)
		{
			dram->Odd[count+128] = dram->Odd[count] = iirSampleF;
			resultB = (dram->Odd[count+down] + dram->Odd[count+side]);
		}
		else
		{
			dram->Even[count+128] = dram->Even[count] = iirSampleF;
			resultB = (dram->Even[count+down] + dram->Even[count+side]);
		}
		count--;
		
		
		iirSampleB = (iirSampleB * (1 - (offset * BEQ))) + (resultB * (offset * BEQ));
		inputSample += (iirSampleB*bleed);
		//extra lowpass for 4*12" speakers
		
		iirSampleH = (iirSampleH * (1 - (offset * BEQ))) + (inputSample * (offset * BEQ));
		inputSample += iirSampleH;
		
inputSample = sin(inputSample*outputlevel);		
		float randy = ((float(fpd)/UINT32_MAX)*0.047f);
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
			
			dram->b[87] = dram->b[86]; dram->b[86] = dram->b[85]; dram->b[85] = dram->b[84]; dram->b[84] = dram->b[83]; dram->b[83] = dram->b[82]; dram->b[82] = dram->b[81]; dram->b[81] = dram->b[80]; dram->b[80] = dram->b[79]; 
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
			inputSample += (dram->b[1] * (1.31819680801404560f  + (0.00362521700518292f*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (1.37738284126127919f  + (0.14134596126256205f*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (1.09957637225311622f  + (0.33199581815501555f*fabs(dram->b[3]))));
			inputSample += (dram->b[4] * (0.62025358899656258f  + (0.37476042042088142f*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (0.12926194402137478f  + (0.24702655568406759f*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.28927985011367602f  - (0.13289168298307708f*fabs(dram->b[6]))));
			inputSample -= (dram->b[7] * (0.56518146339033448f  - (0.11026641793526121f*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.59843200696815069f  - (0.10139909232154271f*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.45219971861789204f  - (0.13313355255903159f*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.32520490032331351f  - (0.29009061730364216f*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.29773131872442909f  - (0.45307495356996669f*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.31738895975218867f  - (0.43198591958928922f*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.33336150604703757f  - (0.24240412850274029f*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.32461638442042151f  - (0.02779297492397464f*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.27812829473787770f  + (0.15565718905032455f*fabs(dram->b[15]))));
			inputSample -= (dram->b[16] * (0.19413454458668097f  + (0.32087693535188599f*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.12378036344480114f  + (0.37736575956794161f*fabs(dram->b[17]))));
			inputSample -= (dram->b[18] * (0.12550494837257106f  + (0.25593811142722300f*fabs(dram->b[18]))));
			inputSample -= (dram->b[19] * (0.17725736033713696f  + (0.07708896413593636f*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.22023699647700670f  - (0.01600371273599124f*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.21987645486953747f  + (0.00973336938352798f*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.15014276479707978f  + (0.11602269600138954f*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.05176520203073560f  + (0.20383164255692698f*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.04276687165294867f  + (0.17785002166834518f*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.15951546388137597f  + (0.06748854885822464f*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.30211952144352616f  - (0.03440494237025149f*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.36462803375134506f  - (0.05874284362202409f*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.32283960219377539f  + (0.01189623197958362f*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.19245178663350720f  + (0.11088858383712991f*fabs(dram->b[29]))));
			inputSample += (dram->b[30] * (0.00681589563349590f  - (0.16314250963457660f*fabs(dram->b[30]))));
			inputSample += (dram->b[31] * (0.20927798345622584f  - (0.16952981620487462f*fabs(dram->b[31]))));
			inputSample += (dram->b[32] * (0.25638846543430976f  - (0.11462562122281153f*fabs(dram->b[32]))));
			inputSample += (dram->b[33] * (0.04584495673888605f  + (0.04669671229804190f*fabs(dram->b[33]))));
			inputSample -= (dram->b[34] * (0.25221561978187662f  - (0.19250758741703761f*fabs(dram->b[34]))));
			inputSample -= (dram->b[35] * (0.35662801992424953f  - (0.12244680002787561f*fabs(dram->b[35]))));
			inputSample -= (dram->b[36] * (0.21498114329314663f  + (0.12152120956991189f*fabs(dram->b[36]))));
			inputSample += (dram->b[37] * (0.00968605571673376f  - (0.30597812512858558f*fabs(dram->b[37]))));
			inputSample += (dram->b[38] * (0.18029119870614621f  - (0.31569386468576782f*fabs(dram->b[38]))));
			inputSample += (dram->b[39] * (0.22744437185251629f  - (0.18028438460422197f*fabs(dram->b[39]))));
			inputSample += (dram->b[40] * (0.09725687638959078f  + (0.05479918522830433f*fabs(dram->b[40]))));
			inputSample -= (dram->b[41] * (0.17970389267353537f  - (0.29222750363124067f*fabs(dram->b[41]))));
			inputSample -= (dram->b[42] * (0.42371969704763018f  - (0.34924957781842314f*fabs(dram->b[42]))));
			inputSample -= (dram->b[43] * (0.43313266755788055f  - (0.11503731970288061f*fabs(dram->b[43]))));
			inputSample -= (dram->b[44] * (0.22178165627851801f  + (0.25002925550036226f*fabs(dram->b[44]))));
			inputSample -= (dram->b[45] * (0.00410198176852576f  + (0.43283281457037676f*fabs(dram->b[45]))));
			inputSample += (dram->b[46] * (0.09072426344812032f  - (0.35318250460706446f*fabs(dram->b[46]))));
			inputSample += (dram->b[47] * (0.08405839183965140f  - (0.16936391987143717f*fabs(dram->b[47]))));
			inputSample -= (dram->b[48] * (0.01110419756114383f  - (0.01247164991313877f*fabs(dram->b[48]))));
			inputSample -= (dram->b[49] * (0.18593334646855278f  - (0.14513260199423966f*fabs(dram->b[49]))));
			inputSample -= (dram->b[50] * (0.33665010871497486f  - (0.14456206192169668f*fabs(dram->b[50]))));
			inputSample -= (dram->b[51] * (0.32644968491439380f  + (0.01594380759082303f*fabs(dram->b[51]))));
			inputSample -= (dram->b[52] * (0.14855437679485431f  + (0.23555511219002742f*fabs(dram->b[52]))));
			inputSample += (dram->b[53] * (0.05113019250820622f  - (0.35556617126595202f*fabs(dram->b[53]))));
			inputSample += (dram->b[54] * (0.12915754942362243f  - (0.28571671825750300f*fabs(dram->b[54]))));
			inputSample += (dram->b[55] * (0.07406865846069306f  - (0.10543886479975995f*fabs(dram->b[55]))));
			inputSample -= (dram->b[56] * (0.03669573814193980f  - (0.03194267657582078f*fabs(dram->b[56]))));
			inputSample -= (dram->b[57] * (0.13429103278009327f  - (0.06145796486786051f*fabs(dram->b[57]))));
			inputSample -= (dram->b[58] * (0.17884021749974641f  - (0.00156626902982124f*fabs(dram->b[58]))));
			inputSample -= (dram->b[59] * (0.16138212225178239f  + (0.09402070836837134f*fabs(dram->b[59]))));
			inputSample -= (dram->b[60] * (0.10867028245257521f  + (0.15407963447815898f*fabs(dram->b[60]))));
			inputSample -= (dram->b[61] * (0.06312416389213464f  + (0.11241095544179526f*fabs(dram->b[61]))));
			inputSample -= (dram->b[62] * (0.05826376574081994f  - (0.03635253545701986f*fabs(dram->b[62]))));
			inputSample -= (dram->b[63] * (0.07991631148258237f  - (0.18041947557579863f*fabs(dram->b[63]))));
			inputSample -= (dram->b[64] * (0.07777397532506500f  - (0.20817156738202205f*fabs(dram->b[64]))));
			inputSample -= (dram->b[65] * (0.03812528734394271f  - (0.13679963125162486f*fabs(dram->b[65]))));
			inputSample += (dram->b[66] * (0.00204900323943951f  + (0.04009000730101046f*fabs(dram->b[66]))));
			inputSample += (dram->b[67] * (0.01779599498119764f  - (0.04218637577942354f*fabs(dram->b[67]))));
			inputSample += (dram->b[68] * (0.00950301949319113f  - (0.07908911305044238f*fabs(dram->b[68]))));
			inputSample -= (dram->b[69] * (0.04283600714814891f  + (0.02716262334097985f*fabs(dram->b[69]))));
			inputSample -= (dram->b[70] * (0.14478320837041933f  - (0.08823515277628832f*fabs(dram->b[70]))));
			inputSample -= (dram->b[71] * (0.23250267035795688f  - (0.15334197814956568f*fabs(dram->b[71]))));
			inputSample -= (dram->b[72] * (0.22369031446225857f  - (0.08550989980799503f*fabs(dram->b[72]))));
			inputSample -= (dram->b[73] * (0.11142757883989868f  + (0.08321482928259660f*fabs(dram->b[73]))));
			inputSample += (dram->b[74] * (0.02752318631713307f  - (0.25252906099212968f*fabs(dram->b[74]))));
			inputSample += (dram->b[75] * (0.11940028414727490f  - (0.34358127205009553f*fabs(dram->b[75]))));
			inputSample += (dram->b[76] * (0.12702057126698307f  - (0.31808560130583663f*fabs(dram->b[76]))));
			inputSample += (dram->b[77] * (0.03639067777025356f  - (0.17970282734717846f*fabs(dram->b[77]))));
			inputSample -= (dram->b[78] * (0.11389848143835518f  + (0.00470616711331971f*fabs(dram->b[78]))));
			inputSample -= (dram->b[79] * (0.23024072979374310f  - (0.09772245468884058f*fabs(dram->b[79]))));
			inputSample -= (dram->b[80] * (0.24389015061112601f  - (0.09600959885615798f*fabs(dram->b[80]))));
			inputSample -= (dram->b[81] * (0.16680269075295703f  - (0.05194978963662898f*fabs(dram->b[81]))));
			inputSample -= (dram->b[82] * (0.05108041495077725f  - (0.01796071525570735f*fabs(dram->b[82]))));
			inputSample += (dram->b[83] * (0.06489835353859555f  - (0.00808013770331126f*fabs(dram->b[83]))));
			inputSample += (dram->b[84] * (0.15481511440233464f  - (0.02674063848284838f*fabs(dram->b[84]))));
			inputSample += (dram->b[85] * (0.18620867857907253f  - (0.01786423699465214f*fabs(dram->b[85]))));
			inputSample += (dram->b[86] * (0.13879832139055756f  + (0.01584446839973597f*fabs(dram->b[86]))));
			inputSample += (dram->b[87] * (0.04878235109120615f  + (0.02962866516075816f*fabs(dram->b[87]))));
			
			temp = (inputSample + smoothCabB)/3.0f;
			smoothCabB = inputSample;
			inputSample = temp/4.0f;
			
			
			randy = ((float(fpd)/UINT32_MAX)*0.039f);
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
