#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Disintegrate"
#define AIRWINDOWS_DESCRIPTION "Discontinuity on steroids."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i','u' )
#define AIRWINDOWS_TAGS kNT_tagEffect
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
const int dscBufMax = 180;
const int layersMax = 22;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Top dB", .min = 6000, .max = 14000, .def = 10000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "BufferSize", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Layers", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Filter", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		float dBaL[dscBufMax+5][layersMax];
		int dBaXL[layersMax];
		float outFilterL;
				
		uint32_t fpd;
	
	struct _dram {
			float dBaPosL[layersMax];
		float dBaPosBL[layersMax];
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
	
	float refdB = GetParameter( kParam_One );
	float topdB = 0.000000064f * powf(10.0f,refdB/20.0f) * overallscale;
	int dscBuf = (GetParameter( kParam_Two )*(float)(dscBufMax-1))+1;
	int layers = (GetParameter( kParam_Three )*20.0f);
	float f = powf(GetParameter( kParam_Four ),2);
	float boost = 1.0f + (f/(layers+1));
	if (f == 0.0f) f = 0.000001f;
	float wet = GetParameter( kParam_Five );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		float drySampleL = inputSampleL;
		
		inputSampleL *= topdB;

		for (int x = 0; x < layers; x++) {
			inputSampleL *= boost;
			if (inputSampleL < -0.222f) inputSampleL = -0.222f;
			if (inputSampleL > 0.222f) inputSampleL = 0.222f;
			dBaL[dBaXL[x]][x] = inputSampleL;
			dram->dBaPosL[x] *= (1.0f-f); dram->dBaPosL[x] += (dram->dBaPosBL[x]*f); 
			dram->dBaPosBL[x] *= (1.0f-f); dram->dBaPosBL[x] += fabs((inputSampleL*((inputSampleL*0.25f)-0.5f))*f);
			int dBdly = floor(dram->dBaPosL[x]*dscBuf);
			float dBi = (dram->dBaPosL[x]*dscBuf)-dBdly;
			inputSampleL  = dBaL[dBaXL[x]-dBdly+((dBaXL[x]-dBdly<0)?dscBuf:0)][x] * (1.0f-dBi);
			dBdly++; inputSampleL += dBaL[dBaXL[x]-dBdly+((dBaXL[x]-dBdly<0)?dscBuf:0)][x] * dBi;
			dBaXL[x]++; if (dBaXL[x] < 0 || dBaXL[x] >= dscBuf) dBaXL[x] = 0;
		}		
		outFilterL *= f; outFilterL += (inputSampleL*(1.0f-f)); inputSampleL = outFilterL;
		inputSampleL /= topdB;
		inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int stage = 0; stage < layersMax; stage++) {
		for (int count = 0; count < dscBufMax+2; count++) {
			dBaL[count][stage] = 0.0;
		}
		dram->dBaPosL[stage] = 0.0;
		dram->dBaPosBL[stage] = 0.0;
		dBaXL[stage] = 1;
	}
	outFilterL = 0.0;

	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
