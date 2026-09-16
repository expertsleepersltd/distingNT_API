#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Point"
#define AIRWINDOWS_DESCRIPTION "An explosive transient designer."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','u' )
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
{ .name = "Input Trim", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Point", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reaction Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 nibA;
		Float32 nobA;
		Float32 nibB;
		Float32 nobB;
		uint32_t fpd;
		bool fpFlip;
	
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
	
	
	//***********************************************************************
	//     REMEMBER TO CHANGE THE NAME OF THE COMPONENT TO 'POINT'
	//***********************************************************************
	
	
	
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	Float32 gaintrim = powf(10.0f,GetParameter( kParam_One )/20);
	Float32 nibDiv = 1 / powf(GetParameter( kParam_Three )+0.2f,7);
	nibDiv /= overallscale;
	Float32 nobDiv;
	if (GetParameter( kParam_Two ) > 0) nobDiv = nibDiv / (1.001f-GetParameter( kParam_Two ));
	else nobDiv = nibDiv * (1.001f-powf(GetParameter( kParam_Two )*0.75f,2));
	Float32 nibnobFactor = 0.0f; //start with the fallthrough value, why not
	float inputSample;
	Float32 absolute;

	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		
		sourceP += inNumChannels;
		inputSample *= gaintrim;
		absolute = fabs(inputSample);
		
		if (fpFlip)
		{
		nibA = nibA + (absolute / nibDiv);
		nibA = nibA / (1 + (1/nibDiv));
		nobA = nobA + (absolute / nobDiv);
		nobA = nobA / (1 + (1/nobDiv));
		if (nobA > 0)
			{
			nibnobFactor = nibA / nobA;
			}
		}
		else
		{
		nibB = nibB + (absolute / nibDiv);
		nibB = nibB / (1 + (1/nibDiv));
		nobB = nobB + (absolute / nobDiv);
		nobB = nobB / (1 + (1/nobDiv));
		if (nobB > 0)
			{
			nibnobFactor = nibB / nobB;
			}		
		}
		
		inputSample *= nibnobFactor;
		fpFlip = !fpFlip;
		
		
		
		*destP = inputSample;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	nibA = 0.0;
	nobA = 0.0;
	nibB = 0.0;
	nobB = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = true;
}
};
