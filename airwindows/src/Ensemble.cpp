#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Ensemble"
#define AIRWINDOWS_DESCRIPTION "A weird flangey little modulation effect."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','E','n','e' )
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
{ .name = "Ensemble", .min = 200, .max = 4800, .def = 800, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Fullness", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Brighten", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		const static int totalsamples = 65540;
		int gcount;
		Float32 airPrev;
		Float32 airEven;
		Float32 airOdd;
		Float32 airFactor;
		uint32_t fpd;
		bool fpFlip;
	
	struct _dram {
			Float32 p[totalsamples];
		Float32 sweep[49];
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
	
	Float32 spd = powf(0.4f+(GetParameter( kParam_Two )/12),10);
	spd *= overallscale;
	Float32 depth = 0.002f / spd;
	Float32 tupi = 3.141592653589793238f * 2.0f;
	Float32 taps = GetParameter( kParam_One );
	Float32 brighten = GetParameter( kParam_Three );
	Float32 wet = GetParameter( kParam_Four );
	//removed unnecessary dry variable
	Float32 hapi = 3.141592653589793238f / taps;
	Float32 offset;
	Float32 floffset;
	Float32 start[49];
	Float32 sinoffset[49];	
	Float32 speed[49];
	int count;
	int ensemble;
	Float32 temp;
	float inputSample;
	Float32 drySample;
	//now we'll precalculate some stuff that needn't be in every sample
	
	for(count = 1; count <= taps; count++)
		{
		start[count] = depth * count;
		sinoffset[count] = hapi * (count-1);
		speed[count] = spd / (1 + (count/taps));
		}
	//that's for speeding up things in the sample-processing area
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;

		airFactor = airPrev - inputSample;
		if (fpFlip) {airEven += airFactor; airOdd -= airFactor; airFactor = airEven;}
		else {airOdd += airFactor; airEven -= airFactor; airFactor = airOdd;}
		airOdd = (airOdd - ((airOdd - airEven)/256.0f)) / 1.0001f;
		airEven = (airEven - ((airEven - airOdd)/256.0f)) / 1.0001f;
		airPrev = inputSample;
		inputSample += (airFactor*brighten);
		//air, compensates for loss of highs in flanger's interpolation
		if (gcount < 1 || gcount > 32767) {gcount = 32767;}
		count = gcount;
		dram->p[count+32767] = dram->p[count] = temp = inputSample;
		//float buffer

		for(ensemble = 1; ensemble <= taps; ensemble++)
			{
				offset = start[ensemble] + (depth * sin(dram->sweep[ensemble]+sinoffset[ensemble]));
				floffset = offset-floor(offset);
				count = gcount + (int)floor(offset);
				temp += dram->p[count] * (1-floffset); //less as value moves away from .0
				temp += dram->p[count+1]; //we can assume always using this in one way or another?
				temp += dram->p[count+2] * floffset; //greater as value moves away from .0
				temp -= ((dram->p[count]-dram->p[count+1])-(dram->p[count+1]-dram->p[count+2]))/50; //interpolation hacks 'r us
				dram->sweep[ensemble] += speed[ensemble];
				if (dram->sweep[ensemble] > tupi){dram->sweep[ensemble] -= tupi;}
			}
		gcount--;
		//still scrolling through the samples, remember
		
		inputSample = temp/(4.0f*sqrt(taps));


		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		fpFlip = !fpFlip;
		
		
		
		*destP = inputSample;
		destP += inNumChannels;
		sourceP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 65539; count++) {dram->p[count] = 0;}
	for(int count = 0; count < 49; count++) {dram->sweep[count] = 3.141592653589793238 / 2.0;}
	gcount = 0;
	airPrev = 0.0;
	airEven = 0.0;
	airOdd = 0.0;
	airFactor = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = true;
}
};
