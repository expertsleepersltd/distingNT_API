#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Acceleration"
#define AIRWINDOWS_DESCRIPTION "An acceleration limiter that tames edge, leaves brightness."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','c','n' )
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
{ .name = "Limit", .min = 0, .max = 1000, .def = 320, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 ataLastOutL;
		Float32 s1L;
		Float32 s2L;
		Float32 s3L;
		Float32 o1L;
		Float32 o2L;
		Float32 o3L;
		Float32 m1L;
		Float32 m2L;
		Float32 desL;
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
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();

	Float32 intensity = powf(GetParameter( kParam_One ),3)*(32/overallscale);
	Float32 wet = GetParameter( kParam_Two );
	//removed unnecessary dry variable
	
	Float32 senseL;
	Float32 smoothL;
	Float32 accumulatorSample;
	
	float drySample;
	float inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		

		s3L = s2L;
		s2L = s1L;
		s1L = inputSample;
		smoothL = (s3L + s2L + s1L) / 3.0f;
		m1L = (s1L-s2L)*((s1L-s2L)/1.3f);
		m2L = (s2L-s3L)*((s1L-s2L)/1.3f);
		senseL = fabs(m1L-m2L);
		senseL = (intensity*intensity*senseL);
		o3L = o2L;
		o2L = o1L;
		o1L = senseL;
		if (o2L > senseL) senseL = o2L;
		if (o3L > senseL) senseL = o3L;
		//sense on the most intense
		if (senseL > 1.0f) senseL = 1.0f;
		inputSample *= (1.0f-senseL);
		inputSample += (smoothL*senseL);
		senseL /= 2.0f;
		accumulatorSample = (ataLastOutL*senseL)+(inputSample*(1.0f-senseL));
		ataLastOutL = inputSample;
		inputSample = accumulatorSample;
		
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	ataLastOutL = 0.0;
	s1L = s2L = s3L = 0.0;
	o1L = o2L = o3L = 0.0;
	m1L = m2L = desL = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
