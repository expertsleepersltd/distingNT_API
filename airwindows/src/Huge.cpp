#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Huge"
#define AIRWINDOWS_DESCRIPTION "A dirty, dirty loudenator."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','u','e' )
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
{ .name = "Huge", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Nasty", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		float prev3;
		float prev5;
		float prev7;
		float prev9;
		float prev11;
		float prev13;
		float prevOut;
		float limit3;
		float limit5;
		float limit7;
		float limit9;
		float limit11;
		float limit13;
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
	
	float huge = GetParameter( kParam_One );
	float threshold = (GetParameter( kParam_Two ) * 0.05f) + 0.05f;
	float attack = (threshold * 8.0f) / overallscale;
	float release = (threshold * 8.0f) / overallscale;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		inputSample *= huge;
		
		float inP3 = inputSample * inputSample * inputSample;
		float outP3 = ((4.0f*inP3)-(3.0f*inputSample))*-0.36f;
		float inP5 = inP3 * inputSample * inputSample;
		float outP5 = ((16.0f*inP5)-(20.0f*inP3)+(5.0f*inputSample))*0.2025f;
		float inP7 = inP5 * inputSample * inputSample;
		float outP7 = ((64.0f*inP7)-(112.0f*inP5)+(56.0f*inP3)-(7.0f*inputSample))*-0.1444f;
		float inP9 = inP7 * inputSample * inputSample;
		float outP9 = ((256.0f*inP9)-(576.0f*inP7)+(432.0f*inP5)-(120.0f*inP3)+(9.0f*inputSample))*0.1225f;
		float inP11 = inP9 * inputSample * inputSample;
		float outP11 = ((1024.0f*inP11)-(2816.0f*inP9)+(2816.0f*inP7)-(1232.0f*inP5)+(220.0f*inP3)-(11.0f*inputSample))*-0.1024f;
		float inP13 = inP11 * inputSample * inputSample;
		float outP13 = ((4096.0f*inP13)-(13312.0f*inP11)+(16640.0f*inP9)-(9984.0f*inP7)+(2912.0f*inP5)-(364.0f*inP3)+(13.0f*inputSample))*0.09f;

		float slew3 = fabs(outP3 - prev3); prev3 = outP3;
		float slew5 = fabs(outP5 - prev5); prev5 = outP5;
		float slew7 = fabs(outP7 - prev7); prev7 = outP7;
		float slew9 = fabs(outP9 - prev9); prev9 = outP9;
		float slew11 = fabs(outP11 - prev11); prev11 = outP11;
		float slew13 = fabs(outP13 - prev13); prev13 = outP13;
		
		if (slew3 < threshold) limit3 += release;
		else limit3 -= attack;
		if (slew5 < threshold) limit5 += release;
		else limit5 -= attack;
		if (slew7 < threshold) limit7 += release;
		else limit7 -= attack;
		if (slew9 < threshold) limit9 += release;
		else limit9 -= attack;
		if (slew11 < threshold) limit11 += release;
		else limit11 -= attack;
		if (slew13 < threshold) limit13 += release;
		else limit13 -= attack;
		
		if (limit3 > 1.0f)  {limit3 = 1.0f;}
		if (limit5 > 1.0f)  {limit3 = 1.0f; limit5 = 1.0f;}
		if (limit7 > 1.0f)  {limit3 = 1.0f; limit5 = 1.0f; limit7 = 1.0f;}
		if (limit9 > 1.0f)  {limit3 = 1.0f; limit5 = 1.0f; limit7 = 1.0f; limit9 = 1.0f;}
		if (limit11 > 1.0f) {limit3 = 1.0f; limit5 = 1.0f; limit7 = 1.0f; limit9 = 1.0f; limit11 = 1.0f;}
		if (limit13 > 1.0f) {limit3 = 1.0f; limit5 = 1.0f; limit7 = 1.0f; limit9 = 1.0f; limit11 = 1.0f; limit13 = 1.0f;}
		if (limit3 < 0.0f) {limit3 = 0.0f; limit5 = 0.0f; limit7 = 0.0f; limit9 = 0.0f; limit11 = 0.0f; limit13 = 0.0f;}
		if (limit5 < 0.0f) {limit5 = 0.0f; limit7 = 0.0f; limit9 = 0.0f; limit11 = 0.0f; limit13 = 0.0f;}
		if (limit7 < 0.0f) {limit7 = 0.0f; limit9 = 0.0f; limit11 = 0.0f; limit13 = 0.0f;}
		if (limit9 < 0.0f) {limit9 = 0.0f; limit11 = 0.0f; limit13 = 0.0f;}
		if (limit11 < 0.0f) {limit11 = 0.0f; limit13 = 0.0f;}
		if (limit13 < 0.0f) {limit13 = 0.0f;}
		
		float chebyshev = (outP3 * limit3);
		chebyshev += (outP5 * limit5);
		chebyshev += (outP7 * limit7);
		chebyshev += (outP9 * limit9);
		chebyshev += (outP11 * limit11);
		chebyshev += (outP13 * limit13);
		inputSample += ((chebyshev+prevOut)*0.5f);
		prevOut = chebyshev;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	prev3 = prev5 = prev7 = prev9 = prev11 = prev13 = prevOut = 0.0;
	limit3 = limit5 = limit7 = limit9 = limit11 = limit13 = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
