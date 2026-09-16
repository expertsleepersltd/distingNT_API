#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "LilAmp"
#define AIRWINDOWS_DESCRIPTION "A tiny amp sim, like a little bitty practice amp without that much gain."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','L','i','p' )
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
		Float32 OddA;
		Float32 OddB;
		Float32 OddC;
		Float32 OddD;
		Float32 OddE;
		Float32 EvenA;
		Float32 EvenB;
		Float32 EvenC;
		Float32 EvenD;
		Float32 EvenE;
		bool flip; //amp
		
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

	float inputlevel = GetParameter( kParam_One )*6.0f;
	float EQ = (GetParameter( kParam_Two )/ GetSampleRate())*22050;
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
	
	float skewlevel = powf(basstrim,2) * outputlevel;
	
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
		skew *= skewlevel;
		inputSample *= basstrim;
		inputSample *= inputlevel;
		float offset = (1.0f - EQ) + (fabs(inputSample)*EQ);
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		iirSampleA = (iirSampleA * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - iirSampleA;
		//highpass
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skew;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633f+skew))+(bridgerectifier*(1.57079633f+skew));
		else inputSample = (inputSample*(-0.57079633f+skew))-(bridgerectifier*(1.57079633f+skew));
		//overdrive
		iirSampleC = (iirSampleC * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = iirSampleC;
		//lowpass. Use offset from before gain stage
		//finished first gain stage
		
		outSample = (inputSample * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
		dram->fixB[fix_sL1] = (inputSample * dram->fixB[fix_a1]) - (outSample * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
		dram->fixB[fix_sL2] = (inputSample * dram->fixB[fix_a2]) - (outSample * dram->fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		offset = (1.0f + offset) / 2.0f;
		iirSampleB = (iirSampleB * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - iirSampleB;
		//highpass
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633f) + skew;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633f+skew))+(bridgerectifier*(1.57079633f+skew));
		else inputSample = (inputSample*(-0.57079633f+skew))-(bridgerectifier*(1.57079633f+skew));
		//overdrive
		iirSampleD = (iirSampleD * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = iirSampleD;
		//lowpass. Use offset from before gain stage
		
		if (flip)
		{
			OddD = OddC; OddC = OddB; OddB = OddA; OddA = inputSample;
			inputSample = (OddA + OddB + OddC + OddD) / 4.0f;
		}
		else
		{
			EvenD = EvenC; EvenC = EvenB; EvenB = EvenA; EvenA = inputSample;
			inputSample = (EvenA + EvenB + EvenC + EvenD) / 4.0f;
		}
		
		outSample = (inputSample * dram->fixC[fix_a0]) + dram->fixC[fix_sL1];
		dram->fixC[fix_sL1] = (inputSample * dram->fixC[fix_a1]) - (outSample * dram->fixC[fix_b1]) + dram->fixC[fix_sL2];
		dram->fixC[fix_sL2] = (inputSample * dram->fixC[fix_a2]) - (outSample * dram->fixC[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier) * 1.57079633f;
		if (inputSample > 0) inputSample = (inputSample*-0.57079633f)+(bridgerectifier*1.57079633f);
		else inputSample = (inputSample*-0.57079633f)-(bridgerectifier*1.57079633f);
		//output stage has less gain, no highpass, straight lowpass
		iirSampleE = (iirSampleE * (1 - EQ)) + (inputSample * EQ);
		inputSample = iirSampleE;
		//lowpass. Use offset from before gain stage
		
		outSample = (inputSample * dram->fixD[fix_a0]) + dram->fixD[fix_sL1];
		dram->fixD[fix_sL1] = (inputSample * dram->fixD[fix_a1]) - (outSample * dram->fixD[fix_b1]) + dram->fixD[fix_sL2];
		dram->fixD[fix_sL2] = (inputSample * dram->fixD[fix_a2]) - (outSample * dram->fixD[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample = sin(inputSample*outputlevel);
		
		outSample = (inputSample * dram->fixE[fix_a0]) + dram->fixE[fix_sL1];
		dram->fixE[fix_sL1] = (inputSample * dram->fixE[fix_a1]) - (outSample * dram->fixE[fix_b1]) + dram->fixE[fix_sL2];
		dram->fixE[fix_sL2] = (inputSample * dram->fixE[fix_a2]) - (outSample * dram->fixE[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		float randy = ((float(fpd)/UINT32_MAX)*0.034f);
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
			
			dram->b[82] = dram->b[81]; dram->b[81] = dram->b[80]; dram->b[80] = dram->b[79]; 
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
			inputSample += (dram->b[1] * (1.42133070619855229f  - (0.18270903813104500f*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (1.47209686171873821f  - (0.27954009590498585f*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (1.34648011331265294f  - (0.47178343556301960f*fabs(dram->b[3]))));
			inputSample += (dram->b[4] * (0.82133804036124580f  - (0.41060189990353935f*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (0.21628057120466901f  - (0.26062442734317454f*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.30306716082877883f  + (0.10067648425439185f*fabs(dram->b[6]))));
			inputSample -= (dram->b[7] * (0.69484313178531876f  - (0.09655574841702286f*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.88320822356980833f  - (0.26501644327144314f*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.81326147029423723f  - (0.31115926837054075f*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.56728759049069222f  - (0.23304233545561287f*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.33340326645198737f  - (0.12361361388240180f*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.20280263733605616f  - (0.03531960962500105f*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.15864533788751345f  + (0.00355160825317868f*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.12544767480555119f  + (0.01979010423176500f*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.06666788902658917f  + (0.00188830739903378f*fabs(dram->b[15]))));
			inputSample += (dram->b[16] * (0.02977793355081072f  + (0.02304216615605394f*fabs(dram->b[16]))));
			inputSample += (dram->b[17] * (0.12821526330916558f  + (0.02636238376777800f*fabs(dram->b[17]))));
			inputSample += (dram->b[18] * (0.19933812710210136f  - (0.02932657234709721f*fabs(dram->b[18]))));
			inputSample += (dram->b[19] * (0.18346460191225772f  - (0.12859581955080629f*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.00088697526755385f  + (0.15855257539277415f*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.28904286712096761f  + (0.06226286786982616f*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.49133546282552537f  - (0.06512851581813534f*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.52908013030763046f  - (0.13606992188523465f*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.45897241332311706f  - (0.15527194946346906f*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.35535938629924352f  - (0.13634771941703441f*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.26185269405237693f  - (0.08736651482771546f*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.19997351944186473f  - (0.01714565029656306f*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.18894054145105646f  + (0.04557612705740050f*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.24043993691153928f  + (0.05267500387081067f*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.29191852873822671f  + (0.01922151122971644f*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.29399783430587761f  - (0.02238952856106585f*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.26662219155294159f  - (0.07760819463416335f*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.20881206667122221f  - (0.11930017354479640f*fabs(dram->b[33]))));
			inputSample -= (dram->b[34] * (0.12916658879944876f  - (0.11798638949823513f*fabs(dram->b[34]))));
			inputSample -= (dram->b[35] * (0.07678815166012012f  - (0.06826864734598684f*fabs(dram->b[35]))));
			inputSample -= (dram->b[36] * (0.08568505484529348f  - (0.00510459741104792f*fabs(dram->b[36]))));
			inputSample -= (dram->b[37] * (0.13613615872486634f  + (0.02288223583971244f*fabs(dram->b[37]))));
			inputSample -= (dram->b[38] * (0.17426657494209266f  + (0.02723737220296440f*fabs(dram->b[38]))));
			inputSample -= (dram->b[39] * (0.17343619261009030f  + (0.01412920547179825f*fabs(dram->b[39]))));
			inputSample -= (dram->b[40] * (0.14548368977428555f  - (0.02640418940455951f*fabs(dram->b[40]))));
			inputSample -= (dram->b[41] * (0.10485295885802372f  - (0.06334665781931498f*fabs(dram->b[41]))));
			inputSample -= (dram->b[42] * (0.06632268974717079f  - (0.05960343688612868f*fabs(dram->b[42]))));
			inputSample -= (dram->b[43] * (0.06915692039882040f  - (0.03541337869596061f*fabs(dram->b[43]))));
			inputSample -= (dram->b[44] * (0.11889611687783583f  - (0.02250608307287119f*fabs(dram->b[44]))));
			inputSample -= (dram->b[45] * (0.14598456370320673f  + (0.00280345943128246f*fabs(dram->b[45]))));
			inputSample -= (dram->b[46] * (0.12312084125613143f  + (0.04947283933434576f*fabs(dram->b[46]))));
			inputSample -= (dram->b[47] * (0.11379940289994711f  + (0.06590080966570636f*fabs(dram->b[47]))));
			inputSample -= (dram->b[48] * (0.12963290754003182f  + (0.02597647654256477f*fabs(dram->b[48]))));
			inputSample -= (dram->b[49] * (0.12723837402978638f  - (0.04942071966927938f*fabs(dram->b[49]))));
			inputSample -= (dram->b[50] * (0.09185015882996231f  - (0.10420810015956679f*fabs(dram->b[50]))));
			inputSample -= (dram->b[51] * (0.04011592913036545f  - (0.10234174227772008f*fabs(dram->b[51]))));
			inputSample += (dram->b[52] * (0.00992597785057113f  + (0.05674042373836896f*fabs(dram->b[52]))));
			inputSample += (dram->b[53] * (0.04921452178306781f  - (0.00222698867111080f*fabs(dram->b[53]))));
			inputSample += (dram->b[54] * (0.06096504883783566f  - (0.04040426549982253f*fabs(dram->b[54]))));
			inputSample += (dram->b[55] * (0.04113530718724200f  - (0.04190143593049960f*fabs(dram->b[55]))));
			inputSample += (dram->b[56] * (0.01292699017654650f  - (0.01121994018532499f*fabs(dram->b[56]))));
			inputSample -= (dram->b[57] * (0.00437123132431870f  - (0.02482497612289103f*fabs(dram->b[57]))));
			inputSample -= (dram->b[58] * (0.02090571264211918f  - (0.03732746039260295f*fabs(dram->b[58]))));
			inputSample -= (dram->b[59] * (0.04749751678612051f  - (0.02960060937328099f*fabs(dram->b[59]))));
			inputSample -= (dram->b[60] * (0.07675095194206227f  - (0.02241927084099648f*fabs(dram->b[60]))));
			inputSample -= (dram->b[61] * (0.08879414028581609f  - (0.01144281133042115f*fabs(dram->b[61]))));
			inputSample -= (dram->b[62] * (0.07378854974999530f  + (0.02518742701599147f*fabs(dram->b[62]))));
			inputSample -= (dram->b[63] * (0.04677309194488959f  + (0.08984657372223502f*fabs(dram->b[63]))));
			inputSample -= (dram->b[64] * (0.02911874044176449f  + (0.14202665940555093f*fabs(dram->b[64]))));
			inputSample -= (dram->b[65] * (0.02103564720234969f  + (0.14640411976171003f*fabs(dram->b[65]))));
			inputSample -= (dram->b[66] * (0.01940626429101940f  + (0.10867274382865903f*fabs(dram->b[66]))));
			inputSample -= (dram->b[67] * (0.03965401793931531f  + (0.04775225375522835f*fabs(dram->b[67]))));
			inputSample -= (dram->b[68] * (0.08102486457314527f  - (0.03204447425666343f*fabs(dram->b[68]))));
			inputSample -= (dram->b[69] * (0.11794849372825778f  - (0.12755667382696789f*fabs(dram->b[69]))));
			inputSample -= (dram->b[70] * (0.11946469076758266f  - (0.20151394599125422f*fabs(dram->b[70]))));
			inputSample -= (dram->b[71] * (0.07404630324668053f  - (0.21300634351769704f*fabs(dram->b[71]))));
			inputSample -= (dram->b[72] * (0.00477584437144086f  - (0.16864707684978708f*fabs(dram->b[72]))));
			inputSample += (dram->b[73] * (0.05924822014377220f  + (0.09394651445109450f*fabs(dram->b[73]))));
			inputSample += (dram->b[74] * (0.10060989907457370f  + (0.00419196431884887f*fabs(dram->b[74]))));
			inputSample += (dram->b[75] * (0.10817907203844988f  - (0.07459664480796091f*fabs(dram->b[75]))));
			inputSample += (dram->b[76] * (0.08701102204768002f  - (0.11129477437630560f*fabs(dram->b[76]))));
			inputSample += (dram->b[77] * (0.05673785623180162f  - (0.10638640242375266f*fabs(dram->b[77]))));
			inputSample += (dram->b[78] * (0.02944190197442081f  - (0.08499792583420167f*fabs(dram->b[78]))));
			inputSample += (dram->b[79] * (0.01570145445652971f  - (0.06190456843465320f*fabs(dram->b[79]))));
			inputSample += (dram->b[80] * (0.02770233032476748f  - (0.04573713136865480f*fabs(dram->b[80]))));
			inputSample += (dram->b[81] * (0.05417160459175360f  - (0.03965651064634598f*fabs(dram->b[81]))));
			inputSample += (dram->b[82] * (0.06080831637644498f  - (0.02909500789113911f*fabs(dram->b[82]))));
			
			temp = (inputSample + smoothCabB)/3.0f;
			smoothCabB = inputSample;
			inputSample = temp/4.0f;
			
			
			randy = ((float(fpd)/UINT32_MAX)*0.085f);
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
	OddA = 0.0;
	OddB = 0.0;
	OddC = 0.0;
	OddD = 0.0;
	OddE = 0.0;
	EvenA = 0.0;
	EvenB = 0.0;
	EvenC = 0.0;
	EvenD = 0.0;
	EvenE = 0.0;
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
