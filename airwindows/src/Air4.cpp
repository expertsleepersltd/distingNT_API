#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Air4"
#define AIRWINDOWS_DESCRIPTION "Extends Air3 with controllable high frequency limiting."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','i','4' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
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
{ .name = "Air", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gnd", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DarkF", .min = 0, .max = 1000, .def = 520, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Ratio", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		enum {
			pvAL1,
			pvSL1,
			accSL1,
			acc2SL1,
			pvAL2,
			pvSL2,
			accSL2,
			acc2SL2,
			pvAL3,
			pvSL3,
			accSL3,
			pvAL4,
			pvSL4,
			gndavgL,
			outAL,
			gainAL,
			lastSL,
			air_total
		};
		uint32_t fpd;
	
	struct _dram {
			float air[air_total];
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
	
	float airGain = GetParameter( kParam_A )*2.0f;
	if (airGain > 1.0f) airGain = powf(airGain,3.0f+sqrt(overallscale));
	float gndGain = GetParameter( kParam_B )*2.0f;
	float threshSinew = powf(GetParameter( kParam_C ),2)/overallscale;
	float depthSinew = GetParameter( kParam_D );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		dram->air[pvSL4] = dram->air[pvAL4] - dram->air[pvAL3];
		dram->air[pvSL3] = dram->air[pvAL3] - dram->air[pvAL2];
		dram->air[pvSL2] = dram->air[pvAL2] - dram->air[pvAL1];
		dram->air[pvSL1] = dram->air[pvAL1] - inputSample;
		
		dram->air[accSL3] = dram->air[pvSL4] - dram->air[pvSL3];
		dram->air[accSL2] = dram->air[pvSL3] - dram->air[pvSL2];
		dram->air[accSL1] = dram->air[pvSL2] - dram->air[pvSL1];
		
		dram->air[acc2SL2] = dram->air[accSL3] - dram->air[accSL2];
		dram->air[acc2SL1] = dram->air[accSL2] - dram->air[accSL1];		
		
		dram->air[outAL] = -(dram->air[pvAL1] + dram->air[pvSL3] + dram->air[acc2SL2] - ((dram->air[acc2SL2] + dram->air[acc2SL1])*0.5f));
		
		dram->air[gainAL] *= 0.5f; 
		dram->air[gainAL] += fabs(drySample-dram->air[outAL])*0.5f;
		if (dram->air[gainAL] > 0.3f*sqrt(overallscale)) dram->air[gainAL] = 0.3f*sqrt(overallscale);
		dram->air[pvAL4] = dram->air[pvAL3];
		dram->air[pvAL3] = dram->air[pvAL2];
		dram->air[pvAL2] = dram->air[pvAL1];		
		dram->air[pvAL1] = (dram->air[gainAL] * dram->air[outAL]) + drySample;
		
		float gnd = drySample - ((dram->air[outAL]*0.5f)+(drySample*(0.457f-(0.017f*overallscale))));
		float temp = (gnd + dram->air[gndavgL])*0.5f; dram->air[gndavgL] = gnd; gnd = temp;
		inputSample = ((drySample-gnd)*airGain)+(gnd*gndGain);
		
		temp = inputSample; if (temp > 1.0f) temp = 1.0f; if (temp < -1.0f) temp = -1.0f;
		float sinew = threshSinew * cos(dram->air[lastSL]*dram->air[lastSL]);
		if (temp - dram->air[lastSL] > sinew) temp = dram->air[lastSL] + sinew;
		if (-(temp - dram->air[lastSL]) > sinew) temp = dram->air[lastSL] - sinew;
		dram->air[lastSL] = temp;
		if (dram->air[lastSL] > 1.0f) dram->air[lastSL] = 1.0f; 
		if (dram->air[lastSL] < -1.0f) dram->air[lastSL] = -1.0f; 
		inputSample = (inputSample * (1.0f-depthSinew))+(dram->air[lastSL]*depthSinew);
		//run Sinew to stop excess slews, but run a dry/wet to allow a range of brights		
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < air_total; x++) dram->air[x] = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
