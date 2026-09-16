#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "AngleFilter"
#define AIRWINDOWS_DESCRIPTION "The synth-style extension of AngleEQ."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','n','r' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Hard", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Poles", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		float primaryA, primaryB;
		float secondaryA, secondaryB;
		float tertiaryA, tertiaryB;
		uint32_t fpd;
	
	struct _dram {
			float iirPosition[37];
		float iirAngle[37];
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

	primaryA = primaryB; primaryB = powf(GetParameter( kParam_A ),overallscale+2.0f);
	secondaryA = secondaryB; secondaryB = GetParameter( kParam_B );
	tertiaryA = tertiaryB; tertiaryB = fmin(primaryB+GetParameter( kParam_C ),1.0f);
	float poles = GetParameter( kParam_D );

	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float primary = (primaryA*temp)+(primaryB*(1.0f-temp));
		float secondary = (secondaryA*temp)+(secondaryB*(1.0f-temp));
		float tertiary = (tertiaryA*temp)+(tertiaryB*(1.0f-temp));
		
		for(int count = 0; count < poles*32; count++) {
			float FT = fmax(primary+(fabs(inputSample)*(secondary-0.5f)),0.0f);
			float FR = (FT*(1.0f-tertiary))+tertiary;
			dram->iirAngle[count] = (dram->iirAngle[count]*(1.0f-FT))+((inputSample-dram->iirPosition[count])*FT);
			inputSample = ((dram->iirPosition[count]+(dram->iirAngle[count]*FT*FR))*(1.0f-FT))+(inputSample*FT);
			dram->iirPosition[count] = ((dram->iirPosition[count]+(dram->iirAngle[count]*FT*FR))*(1.0f-FT))+(inputSample*FT);
		}
		inputSample = sin(inputSample); //straight up just wavefold it

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 36; count++) {dram->iirPosition[count] = 0.0; dram->iirAngle[count] = 0.0;}
	primaryA = primaryB = 0.0;
	secondaryA = secondaryB = 0.0;
	tertiaryA = tertiaryB = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
