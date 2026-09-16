#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BezEQ2"
#define AIRWINDOWS_DESCRIPTION "A unique, subtle three-band shelving EQ."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','e','2' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
const int predelay = 4096;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "x", .min = 0, .max = 1000, .def = 750, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "x", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
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
			bez_SampL,
			bez_cycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		
		//float aR[predelay+5];
		int countA;
		//float bR[predelay+5];
		int countB;
		
		uint32_t fpd;
	
	struct _dram {
			float bezA[bez_total];
		float bezB[bez_total];
		float aL[predelay+5];
		float bL[predelay+5];
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
	
	float trebleGain = (GetParameter( kParam_A ) * 2.0f);
	float derezA = GetParameter( kParam_B )/overallscale;
	if (derezA < 0.00001f) derezA = 0.00001f; if (derezA > 1.0f) derezA = 1.0f;
	int midDelay = (int)(1.0f/derezA);
	if (midDelay > 4096) midDelay = 4096;
	derezA = 1.0f / midDelay;
	float midTrim = (float)midDelay/(midDelay+1.0f);
	midTrim = 1.0f-(derezA*midTrim);
	midDelay = (int)(midDelay*0.5f*midTrim);
	float midGain = (GetParameter( kParam_C ) * 2.0f);
	
	float derezB = powf(GetParameter( kParam_D ),2.0f)/overallscale;
	if (derezB < 0.00001f) derezB = 0.00001f; if (derezB > 1.0f) derezB = 1.0f;
	int bassDelay = (int)(1.0f/derezB);
	if (bassDelay > 4096) bassDelay = 4096;
	derezB = 1.0f / bassDelay;
	float bassTrim = (float)bassDelay/(bassDelay+1.0f);
	bassTrim = 1.0f-(derezB*bassTrim);
	bassDelay = (int)(bassDelay*0.5f*bassTrim);
	float bassGain = (GetParameter( kParam_E ) * 2.0f);
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
				
		dram->bezA[bez_cycle] += derezA;
		dram->bezA[bez_SampL] += (inputSampleL * derezA);
		
		if (dram->bezA[bez_cycle] > 1.0f) {
			dram->bezA[bez_cycle] = 0.0f;
			dram->bezA[bez_CL] = dram->bezA[bez_BL];
			dram->bezA[bez_BL] = dram->bezA[bez_AL];
			dram->bezA[bez_AL] = dram->bezA[bez_SampL];
			dram->bezA[bez_SampL] = 0.0f;
		}
		float X = dram->bezA[bez_cycle]*midTrim;
		float CBL = (dram->bezA[bez_CL]*(1.0f-X))+(dram->bezA[bez_BL]*X);
		float BAL = (dram->bezA[bez_BL]*(1.0f-X))+(dram->bezA[bez_AL]*X);
		float midL = (dram->bezA[bez_BL]+(CBL*(1.0f-X))+(BAL*X))*0.25f;
		
		//predelay
		dram->aL[countA] = inputSampleL;
		countA++; if (countA < 0 || countA > midDelay) countA = 0;
		inputSampleL = dram->aL[countA-((countA > midDelay)?midDelay+1:0)];
		//end predelay
		
		float trebleL = inputSampleL - midL;
		
		dram->bezB[bez_cycle] += derezB;
		dram->bezB[bez_SampL] += (midL * derezB);
		
		if (dram->bezB[bez_cycle] > 1.0f) {
			dram->bezB[bez_cycle] = 0.0f;
			dram->bezB[bez_CL] = dram->bezB[bez_BL];
			dram->bezB[bez_BL] = dram->bezB[bez_AL];
			dram->bezB[bez_AL] = dram->bezB[bez_SampL];
			dram->bezB[bez_SampL] = 0.0f;
		}
		X = dram->bezB[bez_cycle]*bassTrim;
		CBL = (dram->bezB[bez_CL]*(1.0f-X))+(dram->bezB[bez_BL]*X);
		BAL = (dram->bezB[bez_BL]*(1.0f-X))+(dram->bezB[bez_AL]*X);
		float bassL = (dram->bezB[bez_BL]+(CBL*(1.0f-X))+(BAL*X))*0.25f;
		
		if (midGain > 0.0001f) midL += (trebleL*(trebleGain/midGain));
		
		//predelay
		dram->bL[countB] = midL;
		countB++; if (countB < 0 || countB > bassDelay) countB = 0;
		midL = dram->bL[countB-((countB > bassDelay)?bassDelay+1:0)];
		//end predelay
		
		midL -= bassL;
		inputSampleL = (bassL*bassGain) + (midL*midGain);
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bez_total; x++) {dram->bezA[x] = 0.0; dram->bezB[x] = 0.0;}
	dram->bezA[bez_cycle] = 1.0; dram->bezB[bez_cycle] = 1.0;
	for(int count = 0; count < predelay+2; count++) {dram->aL[count] = 0.0; dram->bL[count] = 0.0;}
	countA = 1;	countB = 1;

	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
