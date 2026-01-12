#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Bite"
#define AIRWINDOWS_DESCRIPTION "An unusual edge-maker."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','&' )
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
{ .name = "Bite", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Level", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 sampleA;
		Float32 sampleB;
		Float32 sampleC;
		Float32 sampleD;
		Float32 sampleE;
		Float32 sampleF;
		Float32 sampleG;
		Float32 sampleH;
		Float32 sampleI;
		Float32 fpNShape;	
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
	Float32 overallscale = 1.3f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 gain = GetParameter( kParam_One )*overallscale;
	Float32 outputgain = GetParameter( kParam_Two );
	Float32 midA;
	Float32 midB;
	Float32 midC;
	Float32 midD;
	Float32 trigger;
	float result;
	
	while (nSampleFrames-- > 0) {
		sampleI = sampleH;
		sampleH = sampleG;
		sampleG = sampleF;
		sampleF = sampleE;
		sampleE = sampleD;
		sampleD = sampleC;
		sampleC = sampleB;
		sampleB = sampleA;
		sampleA = *sourceP;
		if (fabs(sampleA)<1.18e-23f) sampleA = fpd * 1.18e-17f;
		
		
		//rotate the buffer in primitive fashion
		
		midA = sampleA - sampleE;
		midB = sampleI - sampleE;
		//define as difference from sample C
		//amount to get to A or E.

		midC = sampleC - sampleE;
		midD = sampleG - sampleE;
		//define as difference from sample C
		//amount to get to A or E.
		
		midA *= ((((sampleB + sampleC + sampleD)/3) - ((sampleA + sampleE)/2.0f))*gain);
		midB *= ((((sampleF + sampleG + sampleH)/3) - ((sampleE + sampleI)/2.0f))*gain);
		//this is the disparity between the real midsample and the averaged one
		//covering a broader range we average the midsamples as well as the ends
		
		midC *= ((sampleD - ((sampleC + sampleE)/2.0f))*gain);
		midD *= ((sampleF - ((sampleE + sampleG)/2.0f))*gain);
		//this is the disparity between the real midsample and the averaged one
		
		trigger = sin(midA + midB + midC + midD);
		//we're doing a thing where we are shifting the 'click' value and excessive amounts don't smash into clipping
		
		result = sampleE + (trigger*8.0f);
		//trigger is already zero from earlier gain applications at zero settings
		
		if (outputgain != 1.0f) result *= outputgain;

		//32 bit dither, made small and tidy.
		int expon; frexpf((Float32)result, &expon);
		float dither = (rand()/(RAND_MAX*7.737125245533627e+25))*powf(2,expon+62);
		result += (dither-fpNShape); fpNShape = dither;
		//end 32 bit dither
		
		*destP = result;
		//output.
		
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	sampleA = 0.0;
	sampleB = 0.0;
	sampleC = 0.0;
	sampleD = 0.0;
	sampleE = 0.0;
	sampleF = 0.0;
	sampleG = 0.0;
	sampleH = 0.0;
	sampleI = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
