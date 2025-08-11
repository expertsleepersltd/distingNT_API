#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DeBez"
#define AIRWINDOWS_DESCRIPTION "DeBez"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','C' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "DeBez", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DeRez", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
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
			bez_InL,
			bez_UnInL,
			bez_SampL,
			bez_AvgInSampL,
			bez_AvgOutSampL,
			bez_cycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		
		float rezA;
		float rezB;
		float bitA;
		float bitB;
		float wetA;
		float wetB;
		
		uint32_t fpd;
	
	struct _dram {
			float bezF[bez_total];
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
	
	rezA = rezB; rezB = GetParameter( kParam_A )*2.0f;
	bitA = bitB; bitB = GetParameter( kParam_B )*2.0f;
	wetA = wetB; wetB = GetParameter( kParam_C )*2.0f;
	
	bool steppedFreq = true; // Revised Bezier Undersampling
	if (rezB > 1.0f) {  // has full rez at center, stepped
		steppedFreq = false; // to left, continuous to right
		rezB = 1.0f-(rezB-1.0f);
	} //if it's set up like that it's the revised algorithm
	rezB = fmin(fmax(powf(rezB,3.0f),0.0005f),1.0f);
	int bezFreqFraction = (int)(1.0f/rezB);
	float bezFreqTrim = (float)bezFreqFraction/(bezFreqFraction+1.0f);
	if (steppedFreq) { //this hard-locks derez to exact subdivisions of 1.0f
		rezB = 1.0f / bezFreqFraction;
		bezFreqTrim = 1.0f-(rezB*bezFreqTrim);
	} else { //this makes it match the 1.0f case using stepped
		bezFreqTrim = 1.0f-powf(rezB*0.5f,1.0f/(rezB*0.5f));
	} //the revision more accurately connects the bezier curves
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		float drySampleL = inputSampleL;
		float temp = (float)nSampleFrames/inFramesToProcess;
		float rez = (rezA*temp)+(rezB*(1.0f-temp));
		float bit = ((bitA*temp)+(bitB*(1.0f-temp)))-1.0f;
		float wet = ((wetA*temp)+(wetB*(1.0f-temp)))-1.0f;
		float dry = 1.0f - wet;
		if (wet > 1.0f) wet = 1.0f;
		if (wet < -1.0f) wet = -1.0f;
		if (dry > 1.0f) dry = 1.0f;
		if (dry < 0.0f) dry = 0.0f;
		//this bitcrush can be subtracted.
		
		dram->bezF[bez_cycle] += rez;
		dram->bezF[bez_SampL] += (inputSampleL * rez);
		if (dram->bezF[bez_cycle] > 1.0f) {
			if (steppedFreq) dram->bezF[bez_cycle] = 0.0f;
			else dram->bezF[bez_cycle] -= 1.0f;
			
			inputSampleL = (dram->bezF[bez_SampL]+dram->bezF[bez_AvgInSampL])*0.5f;
			dram->bezF[bez_AvgInSampL] = dram->bezF[bez_SampL];

			bool crushGate = (bit < 0.0f);
			bit = 1.0f-fabs(bit);
			bit = fmin(fmax(bit*16.0f,0.5f),16.0f);
			float bitFactor = powf(2.0f,bit);
			inputSampleL *= bitFactor;		
			inputSampleL = floor(inputSampleL+(crushGate?0.5f/bitFactor:0.0f));
			inputSampleL /= bitFactor;
			//derez inside debez
			dram->bezF[bez_CL] = dram->bezF[bez_BL];
			dram->bezF[bez_BL] = dram->bezF[bez_AL];
			dram->bezF[bez_AL] = inputSampleL;
			dram->bezF[bez_SampL] = 0.0f;
		}
		float X = dram->bezF[bez_cycle]*bezFreqTrim;
		float CBLfreq = (dram->bezF[bez_CL]*(1.0f-X))+(dram->bezF[bez_BL]*X);
		float BALfreq = (dram->bezF[bez_BL]*(1.0f-X))+(dram->bezF[bez_AL]*X);
		float CBALfreq = (dram->bezF[bez_BL]+(CBLfreq*(1.0f-X))+(BALfreq*X))*0.125f;
		inputSampleL = CBALfreq+dram->bezF[bez_AvgOutSampL]; dram->bezF[bez_AvgOutSampL] = CBALfreq;
		
		inputSampleL = (wet*inputSampleL)+(dry*drySampleL);
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bez_total; x++) dram->bezF[x] = 0.0;
	dram->bezF[bez_cycle] = 1.0;
	
	rezA = 0.5; rezB = 0.5;
	bitA = 0.5; bitB = 0.5;
	wetA = 1.0; wetB = 1.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
