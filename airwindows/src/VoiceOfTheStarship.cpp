#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "VoiceOfTheStarship"
#define AIRWINDOWS_DESCRIPTION "A deep noise tone source."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','V','o','p' )
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
{ .name = "Filter", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Algorithm", .min = 0, .max = 16000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 noiseA;
		Float32 noiseB;
		Float32 noiseC;
		int position;
		int quadratic;
		bool flip;
		bool filterflip;
		Float32 b[11];
		Float32 f[11];
		int lastAlgorithm;
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
	//This code will pass-thru the audio data.
	//This is where you want to process data to produce an effect.

	
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	Float32 cutoff = powf((GetParameter( kParam_One )*0.89f)+0.1f,3);
	if (cutoff > 1.0f) cutoff = 1.0f;
	Float32 invcutoff = 1.0f - cutoff;
	//this is the lowpass
	
	Float32 overallscale = ((1.0f-GetParameter( kParam_One ))*9.0f)+1.0f;
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
	//this is the moving average with remainders
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

	int lowcut = GetParameter( kParam_Two );
	if (lastAlgorithm != lowcut) {
		noiseA = 0.0f; noiseB = 0.0f; noiseC = 0.0f;
		for(int count = 0; count < 11; count++) {b[count] = 0.0f;}
		lastAlgorithm = lowcut;
	}
	//cuts the noise back to 0 if we are changing algorithms,
	//because that also changes gains and can make loud pops.
	//We still get pops, but they'd be even worse
	int dcut;
	if (lowcut > 15) {lowcut = 1151; dcut= 11517;}
	if (lowcut == 15) {lowcut = 113; dcut= 1151;}
	if (lowcut == 14) {lowcut = 71; dcut= 719;}
	if (lowcut == 13) {lowcut = 53; dcut= 541;}
	if (lowcut == 12) {lowcut = 31; dcut= 311;}
	if (lowcut == 11) {lowcut = 23; dcut= 233;}
	if (lowcut == 10) {lowcut = 19; dcut= 191;}
	if (lowcut == 9) {lowcut = 17; dcut= 173;}
	if (lowcut == 8) {lowcut = 13; dcut= 131;}
	if (lowcut == 7) {lowcut = 11; dcut= 113;}
	if (lowcut == 6) {lowcut = 7; dcut= 79;}
	if (lowcut == 5) {lowcut = 6; dcut= 67;}
	if (lowcut == 4) {lowcut = 5; dcut= 59;}
	if (lowcut == 3) {lowcut = 4; dcut= 43;}
	if (lowcut == 2) {lowcut = 3; dcut= 37;}
	if (lowcut == 1) {lowcut = 2; dcut= 23;}
	if (lowcut < 1) {lowcut = 1; dcut= 11;}
	//this is the mechanism for cutting back subs without filtering
	
	Float32 rumbletrim = sqrt(lowcut);
	//this among other things is just to give volume compensation
	Float32 inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		//we then ignore this!
		
		quadratic -= 1;
		if (quadratic < 0)
		{
			position += 1;		
			quadratic = position * position;
			quadratic = quadratic % 170003; //% is C++ mod operator
			quadratic *= quadratic;
			quadratic = quadratic % 17011; //% is C++ mod operator
			quadratic *= quadratic;
			quadratic = quadratic % 1709; //% is C++ mod operator
			quadratic *= quadratic;
			quadratic = quadratic % dcut; //% is C++ mod operator
			quadratic *= quadratic;
			quadratic = quadratic % lowcut;
			//sets density of the centering force
			if (noiseA < 0) {flip = true;}
			else {flip = false;}
			//every time we come here, we force the random walk to be
			//toward the center of the waveform. Without this,
			//it's a pure random walk that will generate DC.
		}
		
		if (flip) noiseA += (float(fpd)/UINT32_MAX);
		else noiseA -= (float(fpd)/UINT32_MAX);
		//here's the guts of the random walk
	
		if (filterflip)
			{noiseB *= invcutoff; noiseB += (noiseA*cutoff); inputSample = noiseB;}
		else
			{noiseC *= invcutoff; noiseC += (noiseA*cutoff); inputSample = noiseC;}
		//now we have the output of the filter as inputSample.
		//this filter is shallower than a straight IIR: it's interleaved
	
		
		b[9] = b[8]; b[8] = b[7]; b[7] = b[6]; b[6] = b[5];
		b[5] = b[4]; b[4] = b[3]; b[3] = b[2]; b[2] = b[1];
		b[1] = b[0]; b[0] = inputSample;
		
		inputSample *= f[0];
		inputSample += (b[1] * f[1]);
		inputSample += (b[2] * f[2]);
		inputSample += (b[3] * f[3]);
		inputSample += (b[4] * f[4]);
		inputSample += (b[5] * f[5]);
		inputSample += (b[6] * f[6]);
		inputSample += (b[7] * f[7]);
		inputSample += (b[8] * f[8]);
		inputSample += (b[9] * f[9]);
		
		inputSample *= 0.1f;
		inputSample *= invcutoff;
		inputSample /= rumbletrim;
		
		flip = !flip;
		filterflip = !filterflip;
		
		

		*destP = inputSample;
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	position = 99999999;
	quadratic = 0;
	noiseA = 0.0;
	noiseB = 0.0;
	noiseC = 0.0;
	flip = false;
	filterflip = false;
	for(int count = 0; count < 11; count++) {b[count] = 0.0; f[count] = 0.0;}
	lastAlgorithm = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
