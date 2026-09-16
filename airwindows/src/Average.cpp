#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Average"
#define AIRWINDOWS_DESCRIPTION "A distinctive sort of lowpass filter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','v','e' )
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
{ .name = "Average", .min = 1000, .max = 10000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 b[11];
		Float32 f[11];		
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
	Float32 inputSample;
	Float32 correctionSample;
	Float32 accumulatorSample;
	Float32 drySample;
	Float32 overallscale = GetParameter( kParam_One );
	Float32 wet = GetParameter( kParam_Two );
	//removed unnecessary dry variable
	Float32 gain = overallscale;
	
	if (gain > 1.0f) {f[0] = 1.0f; gain -= 1.0f;} else {f[0] = gain; gain = 0.0f;}
	if (gain > 1.0f) {f[1] = 1.0f; gain -= 1.0f;} else {f[1] = gain; gain = 0.0f;}
	if (gain > 1.0f) {f[2] = 1.0f; gain -= 1.0f;} else {f[2] = gain; gain = 0.0f;}
	if (gain > 1.0f) {f[3] = 1.0f; gain -= 1.0f;} else {f[3] = gain; gain = 0.0f;}
	if (gain > 1.0f) {f[4] = 1.0f; gain -= 1.0f;} else {f[4] = gain; gain = 0.0f;}
	if (gain > 1.0f) {f[5] = 1.0f; gain -= 1.0f;} else {f[5] = gain; gain = 0.0f;}
	if (gain > 1.0f) {f[6] = 1.0f; gain -= 1.0f;} else {f[6] = gain; gain = 0.0f;}
	if (gain > 1.0f) {f[7] = 1.0f; gain -= 1.0f;} else {f[7] = gain; gain = 0.0f;}
	if (gain > 1.0f) {f[8] = 1.0f; gain -= 1.0f;} else {f[8] = gain; gain = 0.0f;}
	if (gain > 1.0f) {f[9] = 1.0f; gain -= 1.0f;} else {f[9] = gain; gain = 0.0f;}
	//there, now we have a neat little moving average with remainders
		
	if (overallscale < 1.0f) overallscale = 1.0f;
	f[0] /= overallscale;
	f[1] /= overallscale;
	f[2] /= overallscale;
	f[3] /= overallscale;
	f[4] /= overallscale;
	f[5] /= overallscale;
	f[6] /= overallscale;
	f[7] /= overallscale;
	f[8] /= overallscale;
	f[9] /= overallscale;
	//and now it's neatly scaled, too

	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		b[9] = b[8]; b[8] = b[7]; b[7] = b[6]; b[6] = b[5];
		b[5] = b[4]; b[4] = b[3]; b[3] = b[2]; b[2] = b[1];
		b[1] = b[0]; b[0] = accumulatorSample = inputSample;
		//primitive way of doing this: for larger batches of samples, you might
		//try using a circular buffer like in a reverb. If you add the new sample
		//and subtract the one on the end you can keep a running tally of the samples
		//between. Beware of tiny floating-point math errors eventually screwing up
		//your system, though!
		
		accumulatorSample *= f[0];
		accumulatorSample += (b[1] * f[1]);
		accumulatorSample += (b[2] * f[2]);
		accumulatorSample += (b[3] * f[3]);
		accumulatorSample += (b[4] * f[4]);
		accumulatorSample += (b[5] * f[5]);
		accumulatorSample += (b[6] * f[6]);
		accumulatorSample += (b[7] * f[7]);
		accumulatorSample += (b[8] * f[8]);
		accumulatorSample += (b[9] * f[9]);
		//we are doing our repetitive calculations on a separate value
		
		correctionSample = inputSample - accumulatorSample;
		//we're gonna apply the total effect of all these calculations as a single subtract
		
		inputSample -= correctionSample;
		//our one math operation on the input data coming in
		
		if (wet < 1.0f) inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		//dry/wet control only applies if you're using it. We don't do a multiply by 1.0f
		//if it 'won't change anything' but our sample might be at a very different scaling
		//in the floating point system.
		
		

		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	register UInt32 count;
	for(count = 0; count < 11; count++) {b[count] = 0.0; f[count] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
