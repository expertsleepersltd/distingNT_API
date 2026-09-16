#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SubTight"
#define AIRWINDOWS_DESCRIPTION "Uses a variation on the Creature algorithm to tighten sub-lows."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','u','t' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "Trim", .min = 0, .max = 1000, .def = 300, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Steep", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		uint32_t fpd;
	
	struct _dram {
			float sub[22]; //probably worth just using a number here
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
	
	int subStages = powf(GetParameter( kParam_Two ),2)*16.0f;
	if (subStages < 1) subStages = 1;
	float subTrim = powf((GetParameter( kParam_One )*0.3f)+(powf(GetParameter( kParam_Two ),2)*0.2f),subStages)/overallscale;
	//to use this as an analog modeler for restricting digital lows, find set values that still show bass
	//Note that this is best used sparingly, on the 'not enough subtraction' side of the node.
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		//you want subStages and subTrim to be hardcoded values when embedding this into something else
		//then it only needs the dram->sub[] array, and to have it initialized to 0.0f
		
		//begin SubTight section
		float subSample = inputSample * subTrim;
		for (int x = 0; x < subStages; x++) {
			float scale = 0.5f+fabs(subSample*0.5f);
			subSample = (dram->sub[x]+(sin(dram->sub[x]-subSample)*scale));
			dram->sub[x] = subSample*scale;
		}
		if (subStages % 2 > 0) subSample = -subSample;
		if (subSample > 0.25f) subSample = 0.25f;
		if (subSample < -0.25f) subSample = -0.25f;
		inputSample -= (subSample*16.0f);
		//end SubTight section
		
		//cut the level WAY down, then the modified Creature code blows up subs.
		//the adjustment of scale destabilizes the routine to blow up more DC.
		//this is boosted by 24dB and subtracted from the dry signal
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 21; x++) dram->sub[x] = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
