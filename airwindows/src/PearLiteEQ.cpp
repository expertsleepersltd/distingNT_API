#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PearLiteEQ"
#define AIRWINDOWS_DESCRIPTION "PearLiteEQ"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','e','e' )
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
{ .name = "High", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
			prevSampL1,
			prevSlewL1,
			prevSampL2,
			prevSlewL2,
			prevSampL3,
			prevSlewL3,
			prevSampL4,
			prevSlewL4,
			prevSampL5,
			prevSlewL5,
			prevSampL6,
			prevSlewL6,
			prevSampL7,
			prevSlewL7,
			prevSampL8,
			prevSlewL8,
			pear_max,
			figL,
			gndL,
			slew,
			freq,
			levl,
			pear_total
		}; //new pear filter
		
				
		uint32_t fpd;
	
	struct _dram {
			float pearA[pear_total];
		float pearB[pear_total];
		float pearC[pear_total];
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
	
	float topLevl = sqrt(GetParameter( kParam_A )+0.5f);
	dram->pearA[levl] = sqrt(GetParameter( kParam_B )+0.5f);
	dram->pearB[levl] = sqrt(GetParameter( kParam_C )+0.5f);
	dram->pearC[levl] = sqrt(GetParameter( kParam_D )+0.5f);
	
	float freqFactor = sqrt(overallscale)+(overallscale*0.5f);
	dram->pearA[freq] = powf(0.564f,freqFactor+0.85f);
	dram->pearB[freq] = powf(0.564f,freqFactor+4.1f);
	dram->pearC[freq] = powf(0.564f,freqFactor+7.1f);
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		for (int x = 0; x < pear_max; x += 2) {
			//begin Pear filter stages
			dram->pearA[figL] = inputSampleL;
			dram->pearA[slew] = ((dram->pearA[figL]-dram->pearA[x])+dram->pearA[x+1])*dram->pearA[freq]*0.5f;
			dram->pearA[x] = dram->pearA[figL] = (dram->pearA[freq]*dram->pearA[figL])+((1.0f-dram->pearA[freq])*(dram->pearA[x]+dram->pearA[x+1]));
			dram->pearA[x+1] = dram->pearA[slew];
			inputSampleL -= dram->pearA[figL];
			
			dram->pearB[figL] = dram->pearA[figL];
			dram->pearB[slew] = ((dram->pearB[figL]-dram->pearB[x])+dram->pearB[x+1])*dram->pearB[freq]*0.5f;
			dram->pearB[x] = dram->pearB[figL] = (dram->pearB[freq]*dram->pearA[figL])+((1.0f-dram->pearB[freq])*(dram->pearB[x]+dram->pearB[x+1]));
			dram->pearB[x+1] = dram->pearB[slew];
			dram->pearA[figL] -= dram->pearB[figL];
			
			dram->pearC[figL] = dram->pearB[figL];
			dram->pearC[slew] = ((dram->pearC[figL]-dram->pearC[x])+dram->pearC[x+1])*dram->pearC[freq]*0.5f;
			dram->pearC[x] = dram->pearC[figL] = (dram->pearC[freq]*dram->pearB[figL])+((1.0f-dram->pearC[freq])*(dram->pearC[x]+dram->pearC[x+1]));
			dram->pearC[x+1] = dram->pearC[slew];
			dram->pearB[figL] -= dram->pearC[figL];
			
			inputSampleL *= topLevl;
			inputSampleL += (dram->pearA[figL]*dram->pearA[levl]);
			inputSampleL += (dram->pearB[figL]*dram->pearB[levl]);
			inputSampleL += (dram->pearC[figL]*dram->pearC[levl]);
		}
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < pear_total; x++) {
		dram->pearA[x] = 0.0;
		dram->pearB[x] = 0.0;
		dram->pearC[x] = 0.0;
	}
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
