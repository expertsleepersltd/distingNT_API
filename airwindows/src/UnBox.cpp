#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "UnBox"
#define AIRWINDOWS_DESCRIPTION "A distortion where only the harmonics that don't alias are allowed to distort."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','U','n','x' )
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
{ .name = "Input", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "UnBox", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float a[5];
		float b[5];
		float c[11];
		float e[5];
		float f[11];
		float iirSampleA;
		float iirSampleB;
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

	Float32 input = GetParameter( kParam_One );
	Float32 unbox = GetParameter( kParam_Two )+1.0f;
	unbox *= unbox; //let's get some more gain into this
	Float32 iirAmount = (unbox*0.00052f)/overallscale;
	Float32 output = GetParameter( kParam_Three );
	
	Float32 treble = unbox; //averaging taps 1-4
	Float32 gain = treble;
	if (gain > 1.0f) {e[0] = 1.0f; gain -= 1.0f;} else {e[0] = gain; gain = 0.0f;}
	if (gain > 1.0f) {e[1] = 1.0f; gain -= 1.0f;} else {e[1] = gain; gain = 0.0f;}
	if (gain > 1.0f) {e[2] = 1.0f; gain -= 1.0f;} else {e[2] = gain; gain = 0.0f;}
	if (gain > 1.0f) {e[3] = 1.0f; gain -= 1.0f;} else {e[3] = gain; gain = 0.0f;}
	if (gain > 1.0f) {e[4] = 1.0f; gain -= 1.0f;} else {e[4] = gain; gain = 0.0f;}
	//there, now we have a neat little moving average with remainders
	if (treble < 1.0f) treble = 1.0f;
	e[0] /= treble;
	e[1] /= treble;
	e[2] /= treble;
	e[3] /= treble;
	e[4] /= treble;
	//and now it's neatly scaled, too
	
	treble = unbox*2.0f; //averaging taps 1-8
	gain = treble;
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
	if (treble < 1.0f) treble = 1.0f;
	f[0] /= treble;
	f[1] /= treble;
	f[2] /= treble;
	f[3] /= treble;
	f[4] /= treble;
	f[5] /= treble;
	f[6] /= treble;
	f[7] /= treble;
	f[8] /= treble;
	f[9] /= treble;
	//and now it's neatly scaled, too
	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		
		if (input != 1.0f) inputSample *= input;

		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		a[4] = a[3]; a[3] = a[2]; a[2] = a[1];
		a[1] = a[0]; a[0] = inputSample;
		inputSample *= e[0];
		inputSample += (a[1] * e[1]);
		inputSample += (a[2] * e[2]);
		inputSample += (a[3] * e[3]);
		inputSample += (a[4] * e[4]);
		//this is now an average of inputSample
		
		b[4] = b[3]; b[3] = b[2]; b[2] = b[1];
		b[1] = b[0]; b[0] = inputSample;
		inputSample *= e[0];
		inputSample += (b[1] * e[1]);
		inputSample += (b[2] * e[2]);
		inputSample += (b[3] * e[3]);
		inputSample += (b[4] * e[4]);
		//this is now an average of an average of inputSample. Two poles
		
		inputSample *= unbox;
		//clip to 1.2533141373155f to reach maximum output
		if (inputSample > 1.2533141373155f) inputSample = 1.2533141373155f;
		if (inputSample < -1.2533141373155f) inputSample = -1.2533141373155f;
		inputSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0f) ?1:fabs(inputSample));
		inputSample /= unbox;	
		//now we have a distorted inputSample at the correct volume relative to drySample
		
		float accumulatorSample = (drySample - inputSample);
		c[9] = c[8]; c[8] = c[7]; c[7] = c[6]; c[6] = c[5];
		c[5] = c[4]; c[4] = c[3]; c[3] = c[2]; c[2] = c[1];
		c[1] = c[0]; c[0] = accumulatorSample;
		accumulatorSample *= f[0];
		accumulatorSample += (c[1] * f[1]);
		accumulatorSample += (c[2] * f[2]);
		accumulatorSample += (c[3] * f[3]);
		accumulatorSample += (c[4] * f[4]);
		accumulatorSample += (c[5] * f[5]);
		accumulatorSample += (c[6] * f[6]);
		accumulatorSample += (c[7] * f[7]);
		accumulatorSample += (c[8] * f[8]);
		accumulatorSample += (c[9] * f[9]);
		//this is now an average of all the recent variances from dry
		
		iirSampleA = (iirSampleA * (1 - iirAmount)) + (accumulatorSample * iirAmount);
		accumulatorSample -= iirSampleA;
		//two poles of IIR
		iirSampleB = (iirSampleB * (1 - iirAmount)) + (accumulatorSample * iirAmount);
		accumulatorSample -= iirSampleB;
		//highpass section
		//this is now a highpassed average of all the recent variances from dry

		inputSample = drySample - accumulatorSample;
		//we apply it as one operation, to get the result.
		
		if (output != 1.0f) inputSample *= output;
		
		

		*destP = inputSample;
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	for(int count = 0; count < 5; count++) {a[count] = 0.0; b[count] = 0.0; e[count] = 0.0;}
	for(int count = 0; count < 11; count++) {c[count] = 0.0; f[count] = 0.0;}
	iirSampleA = 0.0;
	iirSampleB = 0.0;
}
};
