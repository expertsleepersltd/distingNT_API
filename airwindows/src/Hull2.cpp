#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Hull2"
#define AIRWINDOWS_DESCRIPTION "A very clear three-band EQ."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','u','2' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		int hullp;
		float hullb[5];
		
		uint32_t fpd;
	
	struct _dram {
			float hull[225];	
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

	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//max out at 4x, 192k
	
	float treble = GetParameter( kParam_One )*2.0f;
	float mid = GetParameter( kParam_Two )*2.0f;
	float bass = GetParameter( kParam_Three )*2.0f;
	float iirAmount = 0.125f/cycleEnd;
	int limit = 4*cycleEnd;
	float divisor = 2.0f/limit;

	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		//begin Hull2 treble crossover
		hullp--; if (hullp < 0) hullp += 60;
		dram->hull[hullp] = dram->hull[hullp+60] = inputSample;
		int x = hullp;
		float midSample = 0.0f;
		while (x < hullp+(limit/2)) {
			midSample += dram->hull[x] * divisor;
			x++;
		}
		midSample += midSample * 0.125f;
		while (x < hullp+limit) {
			midSample -= dram->hull[x] * 0.125f * divisor;
			x++;
		}
		dram->hull[hullp+20] = dram->hull[hullp+80] = midSample;
		x = hullp+20;
		midSample = 0.0f;
		while (x < hullp+20+(limit/2)) {
			midSample += dram->hull[x] * divisor;
			x++;
		}
		midSample += midSample * 0.125f;
		while (x < hullp+20+limit) {
			midSample -= dram->hull[x] * 0.125f * divisor;
			x++;
		}
		dram->hull[hullp+40] = dram->hull[hullp+100] = midSample;
		x = hullp+40;
		midSample = 0.0f;
		while (x < hullp+40+(limit/2)) {
			midSample += dram->hull[x] * divisor;
			x++;
		}
		midSample += midSample * 0.125f;
		while (x < hullp+40+limit) {
			midSample -= dram->hull[x] * 0.125f * divisor;
			x++;
		}
		float trebleSample = drySample - midSample;
		//end Hull2 treble crossover
		
		//begin Hull2 midbass crossover
		float bassSample = midSample; x = 0;
		while (x < 3) {
			hullb[x] = (hullb[x] * (1.0f - iirAmount)) + (bassSample * iirAmount);
			if (fabs(hullb[x])<1.18e-37f) hullb[x] = 0.0f;
			bassSample = hullb[x];
			x++;
		}
		midSample -= bassSample;
		//end Hull2 midbass crossover
		
		inputSample = (bassSample * bass) + (midSample * mid) + (trebleSample * treble);
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 222; count++) {dram->hull[count] = 0.0;}
	for(int count = 0; count < 4; count++) {hullb[count] = 0.0;}
	hullp = 1;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
