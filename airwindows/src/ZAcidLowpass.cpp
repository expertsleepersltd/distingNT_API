#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ZAcidLowpass"
#define AIRWINDOWS_DESCRIPTION "Like if an e6400 went insane and grew shark teeth."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','Z','A','s' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Cutoff", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Over", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Under", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Meltdwn", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		enum {
			bez_AL,
			bez_BL,
			bez_CL,
			bez_DL,
			bez_SampL,
			bez_cycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		float biquadE[11];
		float biquadF[11];
		float iirSampleA;
		
		float cutoffA;
		float cutoffB;
		float overA;
		float overB;
		float underA;
		float underB;
		float meltdownA;
		float meltdownB;
		float inTrimA;
		float inTrimB;
		float outTrimA;
		float outTrimB;
				
		uint32_t fpd;
	
	struct _dram {
			float bezA[bez_total];
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
	
	cutoffA = cutoffB;
	cutoffB = powf(GetParameter( kParam_A ),3.0f)/overallscale;
	cutoffB /= (2.0f/powf(overallscale,0.5f-((overallscale-1.0f)*0.0375f)));
	if (cutoffB < 0.0001f) cutoffB = 0.0001f; if (cutoffB > 1.0f) cutoffB = 1.0f;
	overA = overB;
	overB = GetParameter( kParam_B );
	underA = underB;
	underB = GetParameter( kParam_C );
	meltdownA = meltdownB;
	meltdownB = GetParameter( kParam_D );
	
	//opamp stuff
	inTrimA = inTrimB;
	inTrimB = GetParameter( kParam_E )*10.0f;
	inTrimB *= inTrimB; inTrimB *= inTrimB;
	outTrimA = outTrimB;
	outTrimB = GetParameter( kParam_F );
	
	float iirAmountA = 0.00069f/overallscale;
	biquadF[0] = biquadE[0] = 15500.0f / GetSampleRate();
    biquadF[1] = biquadE[1] = 0.935f;
	float K = tan(M_PI * biquadE[0]); //lowpass
	float norm = 1.0f / (1.0f + K / biquadE[1] + K * K);
	biquadE[2] = K * K * norm;
	biquadE[3] = 2.0f * biquadE[2];
	biquadE[4] = biquadE[2];
	biquadE[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadE[6] = (1.0f - K / biquadE[1] + K * K) * norm;
	for (int x = 0; x < 7; x++) biquadF[x] = biquadE[x];
	//end opamp stuff	
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float cutoff = (cutoffA*temp)+(cutoffB*(1.0f-temp));
		float over = (overA*temp)+(overB*(1.0f-temp));
		float under = (underA*temp)+(underB*(1.0f-temp));
		float meltdown = (meltdownA*temp)+(meltdownB*(1.0f-temp));
		float inTrim = (inTrimA*temp)+(inTrimB*(1.0f-temp));
		float outTrim = (outTrimA*temp)+(outTrimB*(1.0f-temp));
		float acidTrim = 1.0f-powf(cutoff*0.5f,1.0f/(cutoff*0.5f));
		
		dram->bezA[bez_cycle] += cutoff;
		dram->bezA[bez_SampL] += (inputSampleL * cutoff);
		
		if (dram->bezA[bez_cycle] > 1.0f) { //hit the end point and we do a reverb sample
			dram->bezA[bez_cycle] -= 1.0f;
			dram->bezA[bez_DL] = dram->bezA[bez_CL];
			dram->bezA[bez_CL] = dram->bezA[bez_BL];
			dram->bezA[bez_BL] = dram->bezA[bez_AL];
			dram->bezA[bez_AL] = (dram->bezA[bez_SampL]*(1.0f-meltdown))+(inputSampleL*meltdown);
			dram->bezA[bez_SampL] = 0.0f;
		}
		
		float X = dram->bezA[bez_cycle]*acidTrim;
		float midL = dram->bezA[bez_DL] * powf(1.0f-X,3.0f);
		midL += dram->bezA[bez_CL] * 3.0f * powf(1.0f-X,2.0f) * X;
		midL += dram->bezA[bez_BL] * 3.0f * (1.0f-X) * X * X;
		midL += dram->bezA[bez_AL] * powf(X,3.0f);
		
		inputSampleL = (midL*under) + ((inputSampleL - midL)*over);
		
		if (inTrim != 1.0f) inputSampleL *= inTrim;
		
		//opamp stage
		if (fabs(iirSampleA)<1.18e-37f) iirSampleA = 0.0f;
		iirSampleA = (iirSampleA * (1.0f - iirAmountA)) + (inputSampleL * iirAmountA);
		inputSampleL -= iirSampleA;
		
		float outSample = biquadE[2]*inputSampleL+biquadE[3]*biquadE[7]+biquadE[4]*biquadE[8]-biquadE[5]*biquadE[9]-biquadE[6]*biquadE[10];
		biquadE[8] = biquadE[7]; biquadE[7] = inputSampleL; inputSampleL = outSample; biquadE[10] = biquadE[9]; biquadE[9] = inputSampleL; //DF1		
		
		if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
		inputSampleL -= (inputSampleL*inputSampleL*inputSampleL*inputSampleL*inputSampleL*0.1768f);
		
		outSample = biquadF[2]*inputSampleL+biquadF[3]*biquadF[7]+biquadF[4]*biquadF[8]-biquadF[5]*biquadF[9]-biquadF[6]*biquadF[10];
		biquadF[8] = biquadF[7]; biquadF[7] = inputSampleL; inputSampleL = outSample; biquadF[10] = biquadF[9]; biquadF[9] = inputSampleL; //DF1
		
		if (outTrim != 1.0f) inputSampleL *= outTrim;		
		//end opamp stage
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bez_total; x++) {dram->bezA[x] = 0.0;}
	dram->bezA[bez_cycle] = 1.0;
	for (int x = 0; x < 11; x++) {biquadE[x] = 0.0; biquadF[x] = 0.0;}
	iirSampleA = 0.0;
	cutoffA = 0.5; cutoffB = 0.5;
	overA = 0.0; overB = 0.0;
	underA = 1.0; underB = 1.0;
	meltdownA = 0.0; meltdownB = 0.0;
	inTrimA = 0.1; inTrimB = 0.1;
	outTrimA = 1.0; outTrimB = 1.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
