#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Smooth"
#define AIRWINDOWS_DESCRIPTION "Can tame pointy sounds or make drums explode."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','m','h' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
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
{ .name = "Smooth", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		
		Float32 lastSampleA;
		Float32 gainA;
		Float32 lastSampleB;
		Float32 gainB;
		Float32 lastSampleC;
		Float32 gainC;
		Float32 lastSampleD;
		Float32 gainD;
		Float32 lastSampleE;
		Float32 gainE;
		uint32_t fpd;
	
	struct _dram {
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
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 clamp;
	Float32 chase = powf(GetParameter( kParam_One ),2);
	Float32 makeup = (1.0f+(chase*1.6f)) * GetParameter( kParam_Two );
	Float32 ratio = chase * 24.0f;
	chase /= overallscale;
	chase *= 0.083f; // set up the ratio of new val to old
	Float32 wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		clamp = fabs(inputSample - lastSampleA);
		clamp = sin(clamp*ratio);
		lastSampleA = inputSample;
		gainA *= (1.0f - chase);
		gainA += ((1.0f-clamp) * chase);
		if (gainA > 1.0f) gainA = 1.0f;
		if (gainA < 0.0f) gainA = 0.0f;
		if (gainA != 1.0f) inputSample *= gainA;
		
		clamp = fabs(inputSample - lastSampleB);
		clamp = sin(clamp*ratio);
		lastSampleB = inputSample;
		gainB *= (1.0f - chase);
		gainB += ((1.0f-clamp) * chase);
		if (gainB > 1.0f) gainB = 1.0f;
		if (gainB < 0.0f) gainB = 0.0f;
		if (gainB != 1.0f) inputSample *= gainB;
		
		clamp = fabs(inputSample - lastSampleC);
		clamp = sin(clamp*ratio);
		lastSampleC = inputSample;
		gainC *= (1.0f - chase);
		gainC += ((1.0f-clamp) * chase);
		if (gainC > 1.0f) gainC = 1.0f;
		if (gainC < 0.0f) gainC = 0.0f;
		if (gainC != 1.0f) inputSample *= gainC;
		
		clamp = fabs(inputSample - lastSampleD);
		clamp = sin(clamp*ratio);
		lastSampleD = inputSample;
		gainD *= (1.0f - chase);
		gainD += ((1.0f-clamp) * chase);
		if (gainD > 1.0f) gainD = 1.0f;
		if (gainD < 0.0f) gainD = 0.0f;
		if (gainD != 1.0f) inputSample *= gainD;
		
		clamp = fabs(inputSample - lastSampleE);
		clamp = sin(clamp*ratio);
		lastSampleE = inputSample;
		gainE *= (1.0f - chase);
		gainE += ((1.0f-clamp) * chase);
		if (gainE > 1.0f) gainE = 1.0f;
		if (gainE < 0.0f) gainE = 0.0f;
		if (gainE != 1.0f) inputSample *= gainE;
		
		if (makeup !=1.0f) {
			inputSample *= makeup;
		}

		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		
		
		*destP = inputSample;
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSampleA = 0.0;
	gainA = 1.0;
	lastSampleB = 0.0;
	gainB = 1.0;
	lastSampleC = 0.0;
	gainC = 1.0;
	lastSampleD = 0.0;
	gainD = 1.0;
	lastSampleE = 0.0;
	gainE = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
