#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DarkNoise"
#define AIRWINDOWS_DESCRIPTION "An alternative method for producing bassier noise directly. Sound design stuff."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','a','e' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
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
{ .name = "Dark", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		Float32 b[11][11];
		Float32 f[11];		
		int freq;
		Float32 lastRandy;
		Float32 outputNoise;
		uint32_t fpd;
	
	struct _dram {
			Float32 r[8193];
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

	int freqTarget = (int)(powf(GetParameter( kParam_One ),3)*8192.0f);
	if (freqTarget < 2) freqTarget = 2;
	Float32 volumeScale = (1.0f/freqTarget) * sqrt(freqTarget);
	
	Float32 overalltaps = (powf(GetParameter( kParam_One ),2)*8.0f)+2.0f;
	Float32 taps = overalltaps;
	//this is our averaging, which is not integer but continuous
	
	Float32 overallpoles = (GetParameter( kParam_One )*9.0f)+1.0f;
	//this is the poles of the filter, also not integer but continuous
	int yLimit = floor(overallpoles)+1;
	Float32 yPartial = overallpoles - floor(overallpoles);
	//now we can do a for loop, and also apply the final pole continuously
	int xLimit = 1;
	for(int x = 0; x < 11; x++) {
		if (taps > 1.0f) {
			f[x] = 1.0f;
			taps -= 1.0f;
			xLimit++;
		} else {
			f[x] = taps;
			taps = 0.0f;
		}
	} //there, now we have a neat little moving average with remainders
	if (xLimit > 9) xLimit = 9;
	
	if (overalltaps < 1.0f) overalltaps = 1.0f;
	for(int x = 0; x < xLimit; x++) {
		f[x] /= overalltaps;
	} //and now it's neatly scaled, too	
	
	Float32 dark  = GetParameter( kParam_Two );
	Float32 out  = GetParameter( kParam_Three )*0.5f;
	Float32 wet = GetParameter( kParam_Four );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		if (freqTarget < freq) {
			outputNoise = ((outputNoise-0.5f)*(1.0f-(1.0f/freq)))+0.5f;
			dram->r[freq] = 0.5f;
			freq--;
		}
		if (freqTarget > freq) freq++;
		//our tone control chases the input one bin at a time
		
		Float32 randy = (Float32)fpd / UINT32_MAX;
		// 0 to 1 random value
		
		int replacementBin = randy * freq;
		outputNoise -= dram->r[replacementBin];
		dram->r[replacementBin] = lastRandy;
		outputNoise += lastRandy;
		lastRandy = randy;
		//we update only one of the slots we're using
		
		inputSample = (-0.5f + outputNoise) * volumeScale;
		
		Float32 nondarkSample = inputSample;
		
		float previousPole = 0;		
		for (int y = 0; y < yLimit; y++) {
			for (int x = xLimit; x >= 0; x--) b[x+1][y] = b[x][y];
			b[0][y] = previousPole = inputSample;
			inputSample = 0.0f;
			for (int x = 0; x < xLimit; x++) inputSample += (b[x][y] * f[x]);
		}
		inputSample = (previousPole * (1.0f-yPartial)) + (inputSample * yPartial);
		//in this way we can blend in the final pole
		
		
		if (dark !=1.0f) {
			inputSample = (inputSample * dark) + (nondarkSample * (1.0f-dark));
		}
		if (out !=1.0f) {
			inputSample *= out;
		}
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
	for(int count = 0; count < 8192; count++) dram->r[count] = 0.5;
	for(int x = 0; x < 11; x++) {
		f[x] = 0.0;
		for (int y = 0; y < 11; y++) b[x][y] = 0.0;
	}
	
	freq = 1;
	lastRandy = 0.0;
	outputNoise = 0.5;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
