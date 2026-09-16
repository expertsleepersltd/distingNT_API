#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Hombre"
#define AIRWINDOWS_DESCRIPTION "Atmosphere and texture (through very short delays)."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','o','e' )
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
{ .name = "Voicing", .min = 0, .max = 1000, .def = 421, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Intensity", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 slide;
		int gcount;
		uint32_t fpd;
	
	struct _dram {
			Float32 p[4001];
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
	Float32 target = GetParameter( kParam_One );
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 offsetA;
	Float32 offsetB;
	int widthA = (int)(1.0f*overallscale);
	int widthB = (int)(7.0f*overallscale); //max 364 at 44.1f, 792 at 96K
	Float32 wet = GetParameter( kParam_Two );
	//removed unnecessary dry variable
	float inputSample;
	Float32 drySample;
	Float32 total;
	int count;	
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;

		slide = (slide * 0.9997f)+(target*0.0003f);

		offsetA = ((powf(slide,2)) * 77)+3.2f;
		offsetB = (3.85f * offsetA)+41;
		offsetA *= overallscale;
		offsetB *= overallscale;
		//adjust for sample rate

		if (gcount < 1 || gcount > 2000) {gcount = 2000;}
		count = gcount;

		dram->p[count+2000] = dram->p[count] = inputSample;
		//float buffer
		
		count = (int)(gcount+floor(offsetA));

		total = dram->p[count] * 0.391f; //less as value moves away from .0
		total += dram->p[count+widthA]; //we can assume always using this in one way or another?
		total += dram->p[count+widthA+widthA] * 0.391f; //greater as value moves away from .0

		inputSample += ((total * 0.274f));

		count = (int)(gcount+floor(offsetB));

		total = dram->p[count] * 0.918f; //less as value moves away from .0
		total += dram->p[count+widthB]; //we can assume always using this in one way or another?
		total += dram->p[count+widthB+widthB] * 0.918f; //greater as value moves away from .0

		inputSample -= ((total * 0.629f));

		inputSample /= 4;
		
		gcount--;
		//still scrolling through the samples, remember

		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 4000; count++) {dram->p[count] = 0.0;}
	gcount = 0;
	slide = 0.421;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
