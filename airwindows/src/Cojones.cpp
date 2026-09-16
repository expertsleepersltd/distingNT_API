#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Cojones"
#define AIRWINDOWS_DESCRIPTION "A new kind of distorty."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','t' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One=0,
	kParam_Two=1,
	kParam_Three=2,
	kParam_Four=3,
	kParam_Five=4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Breathy", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Cojones", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Body", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float stored[2];
		float diff[6];
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

	Float32 breathy = GetParameter( kParam_One );
	Float32 cojones = GetParameter( kParam_Two );
	Float32 body = GetParameter( kParam_Three );
	Float32 output = GetParameter( kParam_Four );
	Float32 wet = GetParameter( kParam_Five );
	Float32 average[5];
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		stored[1] = stored[0];
		stored[0] = inputSample;
		diff[5] = diff[4];
		diff[4] = diff[3];
		diff[3] = diff[2];
		diff[2] = diff[1];
		diff[1] = diff[0];
		diff[0] = stored[0] - stored[1];
		
		average[0] = diff[0] + diff[1];
		average[1] = average[0] + diff[2];
		average[2] = average[1] + diff[3];
		average[3] = average[2] + diff[4];
		average[4] = average[3] + diff[5];
		
		average[0] /= 2.0f;
		average[1] /= 3.0f;
		average[2] /= 4.0f;
		average[3] /= 5.0f;
		average[4] /= 6.0f;
		
		float meanA = diff[0];
		float meanB = diff[0];
		if (fabs(average[4]) < fabs(meanB)) {meanA = meanB; meanB = average[4];}
		if (fabs(average[3]) < fabs(meanB)) {meanA = meanB; meanB = average[3];}
		if (fabs(average[2]) < fabs(meanB)) {meanA = meanB; meanB = average[2];}
		if (fabs(average[1]) < fabs(meanB)) {meanA = meanB; meanB = average[1];}
		if (fabs(average[0]) < fabs(meanB)) {meanA = meanB; meanB = average[0];}
		float meanOut = ((meanA+meanB)/2.0f);
		stored[0] = (stored[1] + meanOut);
		
		float outputSample = stored[0]*body;
		//presubtract cojones
		outputSample += (((inputSample - stored[0])-average[1])*cojones);
		
		outputSample += (average[1]*breathy);
		
		inputSample = outputSample;
		
		if (output < 1.0f) {
			inputSample *= output;
		}
		
		if (wet < 1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	stored[0] = stored[1] = 0.0;
	diff[0] = diff[1] = diff[2] = diff[3] = diff[4] = diff[5] = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
