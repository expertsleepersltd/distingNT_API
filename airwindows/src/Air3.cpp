#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Air3"
#define AIRWINDOWS_DESCRIPTION "Creates a new form of air-band EQ based on Kalman filtering."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','i','3' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Air", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gnd", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
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
			air_total
		};
		//mono version for AU
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
	
	float airGain = GetParameter( kParam_One )*2.0f;
	if (airGain > 1.0f) airGain = powf(airGain,3.0f+sqrt(overallscale));
	float gndGain = GetParameter( kParam_Two )*2.0f;
	
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
