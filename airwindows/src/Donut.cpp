#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Donut"
#define AIRWINDOWS_DESCRIPTION "In memory of Dilla."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','o','t' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	//Add your parameters here...
	kNumberOfParameters=7
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "BaseFrq", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MoveFrq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "BaseRes", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MoveRes", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		enum {
			bez_A,
			bez_B,
			bez_C,
			bez_Ctrl,
			bez_cycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		float bezMin;
		float low;
		float band;
		float freqA, freqB;
		float resoA, resoB;
		float outA, outB;		
		
		uint32_t fpd;
	
	struct _dram {
			float bezComp[bez_total];
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
	
	float bezRez = powf(1.0f-GetParameter( kParam_A ), 6.0f) / overallscale; 
	float sloRez = powf(1.0f-GetParameter( kParam_B ), 6.0f) / overallscale;
	bezRez = fmin(fmax(bezRez,0.00001f),1.0f);
	sloRez = fmin(fmax(sloRez,0.00001f),1.0f);
	freqA = freqB; resoA = resoB; outA = outB;
	freqB = powf(GetParameter( kParam_C ),overallscale+1.0f)*1.225f;
	float movFreq = (GetParameter( kParam_D )*2.0f)-1.0f;
	resoB = powf(1.0f-GetParameter( kParam_E ),2.0f);
	if (resoB < 0.001f) resoB = 0.001f; // q of 0.0f is just a tone
	float movReso = (GetParameter( kParam_F )*-2.0f)+1.0f;
	outB = GetParameter( kParam_G )/sqrt(resoB);
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		const float temp = (float)nSampleFrames/inFramesToProcess;
		const float freq = (freqA*temp)+(freqB*(1.0f-temp));
		const float reso = (resoA*temp)+(resoB*(1.0f-temp));
		const float out = (outA*temp)+(outB*(1.0f-temp)); //dezippering
		
		float ctrl = fabs(inputSample);
		bezMin = fmax(bezMin-sloRez,ctrl);
		dram->bezComp[bez_cycle] += bezRez;
		dram->bezComp[bez_Ctrl] += (bezMin * bezRez);
		
		if (dram->bezComp[bez_cycle] > 1.0f) {
			dram->bezComp[bez_cycle] -= 1.0f;
			dram->bezComp[bez_C] = dram->bezComp[bez_B];
			dram->bezComp[bez_B] = dram->bezComp[bez_A];
			dram->bezComp[bez_A] = dram->bezComp[bez_Ctrl];
			dram->bezComp[bez_Ctrl] = 0.0f;
		}
		const float CB = (dram->bezComp[bez_C]*(1.0f-dram->bezComp[bez_cycle]))+(dram->bezComp[bez_B]*dram->bezComp[bez_cycle]);
		const float BA = (dram->bezComp[bez_B]*(1.0f-dram->bezComp[bez_cycle]))+(dram->bezComp[bez_A]*dram->bezComp[bez_cycle]);
		const float CBA = (dram->bezComp[bez_B]+(CB*(1.0f-dram->bezComp[bez_cycle]))+(BA*dram->bezComp[bez_cycle]))*0.5f;
		const float mFreq = fmin(fmax(freq+(CBA*movFreq),0.004f/overallscale),1.225f);
		const float mReso = fmin(fmax(reso+(CBA*movReso),0.001f),1.0f);
		low += mFreq*band; band += mFreq*((mReso*inputSample)-low-(mReso*band));
		inputSample = (low-sin(band*0.5f))*out; //airwin-donut	
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bez_total; x++) dram->bezComp[x] = 0.0;
	dram->bezComp[bez_cycle] = 1.0; bezMin = 0.0;
	low = band = 0.0;
	freqA = freqB = 0.5;
	resoA = resoB = 0.5;
	outA = outB = 1.0;		
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
