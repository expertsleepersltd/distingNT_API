#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DeEss"
#define AIRWINDOWS_DESCRIPTION "The best de-essing solution there is. A go-to utility plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','t' )
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
{ .name = "Intensity", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Max DeEss", .min = -4800, .max = 0, .def = -2400, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Frequency", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 s1;
		Float32 s2;
		Float32 s3;
		Float32 s4;
		Float32 s5;
		Float32 s6;
		Float32 s7;
		Float32 m1;
		Float32 m2;
		Float32 m3;
		Float32 m4;
		Float32 m5;
		Float32 m6;
		Float32 c1;
		Float32 c2;
		Float32 c3;
		Float32 c4;
		Float32 c5;
		Float32 ratioA;
		Float32 ratioB;
		Float32 iirSampleA;
		Float32 iirSampleB;
		bool flip;
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
	Float32 intensity = powf(GetParameter( kParam_One ),5)*(8192/overallscale);
	Float32 maxdess = 1.0f / powf(10.0f,GetParameter( kParam_Two )/20);
	Float32 iirAmount = powf(GetParameter( kParam_Three ),2)/overallscale;
	Float32 offset;
	Float32 sense;
	Float32 recovery;
	Float32 attackspeed;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		s3 = s2;
		s2 = s1;
		s1 = inputSample;
		m1 = (s1-s2)*((s1-s2)/1.3f);
		m2 = (s2-s3)*((s1-s2)/1.3f);
		sense = fabs((m1-m2)*((m1-m2)/1.3f));
		//this will be 0 for smooth, high for SSS
		attackspeed = 7.0f+(sense*1024);
		//this does not vary with intensity, but it does react to onset transients
		
		sense = 1.0f+(intensity*intensity*sense);
		if (sense > intensity) {sense = intensity;}
		//this will be 1 for smooth, 'intensity' for SSS
		recovery = 1.0f+(0.01f/sense);
		//this will be 1.1f for smooth, 1.0000000f...1 for SSS
		
		offset = 1.0f-fabs(inputSample);
		
		if (flip) {
			iirSampleA = (iirSampleA * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
			if (ratioA < sense)
			{ratioA = ((ratioA*attackspeed)+sense)/(attackspeed+1.0f);}
			else
			{ratioA = 1.0f+((ratioA-1.0f)/recovery);}
			//returny to 1/1 code
			if (ratioA > maxdess){ratioA = maxdess;}
			inputSample = iirSampleA+((inputSample-iirSampleA)/ratioA);
		}
		else {
			iirSampleB = (iirSampleB * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));	
			if (ratioB < sense)
			{ratioB = ((ratioB*attackspeed)+sense)/(attackspeed+1.0f);}
			else
			{ratioB = 1.0f+((ratioB-1.0f)/recovery);}
			//returny to 1/1 code
			if (ratioB > maxdess){ratioB = maxdess;}
			inputSample = iirSampleB+((inputSample-iirSampleB)/ratioB);
		} //have the ratio chase Sense
		
		flip = !flip;

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	s1 = s2 = s3 = s4 = s5 = s6 = s7 = 0.0;
	m1 = m2 = m3 = m4 = m5 = m6 = 0.0;
	c1 = c2 = c3 = c4 = c5 = 0.0;
	ratioA = ratioB = 1.0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
