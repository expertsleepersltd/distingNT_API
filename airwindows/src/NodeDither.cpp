#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "NodeDither"
#define AIRWINDOWS_DESCRIPTION "Adjusta-TPDF-dither, like a dither flanger. Can do Paul and Tape settings."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','N','o','r' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One = 0,
	kParam_Two = 1,
	//Add your parameters here...
	kNumberOfParameters=2
};
static const int kOut = 0;
static const int kIn = 1;
static const int kDefaultValue_ParamTwo = kOut;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, };
static char const * const enumStrings1[] = { "Out", "In", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Node Spacing", .min = 0, .max = 10000, .def = 400, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Phase", .min = 0, .max = 1, .def = 0, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings1 },
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
 
		int gcount;
		uint32_t fpd;
	
	struct _dram {
			Float32 d[5000];
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
	
	Float32 depthA = GetParameter( kParam_One );
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	int offsetA = (int)(depthA * overallscale);
	if (offsetA < 1) offsetA = 1;
	if (offsetA > 2440) offsetA = 2440;
	
	int phase = GetParameter( kParam_Two );
	//0 default is out of phase, 1 is in phase
	
	float inputSample;
	Float32 currentDither;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		
		
		inputSample *= 8388608.0f;
		//0-1 is now one bit, now we dither
		
		if (gcount < 0 || gcount > 2450) {gcount = 2450;}
				
		currentDither = (float(fpd)/UINT32_MAX);
		inputSample += currentDither;
		
		if (phase == 1) {
			inputSample -= 1.0f;
			inputSample += dram->d[gcount+offsetA];
		} else {
			inputSample -= dram->d[gcount+offsetA];
		}
		//in phase means adding, otherwise we subtract
		
		
		inputSample = floor(inputSample);
		dram->d[gcount+2450] = dram->d[gcount] = currentDither;
		
		gcount--;
		
		inputSample /= 8388608.0f;

		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		//pseudorandom number updater
		
		*destP = inputSample;
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 4999; count++) {dram->d[count] = 0;}
	gcount = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
