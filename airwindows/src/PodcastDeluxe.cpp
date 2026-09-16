#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PodcastDeluxe"
#define AIRWINDOWS_DESCRIPTION "A pile of compressors (curve style) and phase rotators."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','e' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Boost", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		int tap1, tap2, tap3, tap4, tap5, maxdelay1, maxdelay2, maxdelay3, maxdelay4, maxdelay5;
		//the phase rotator
		
		Float32 c1;
		Float32 c2;
		Float32 c3;
		Float32 c4;
		Float32 c5;
		//the compressor
				
		Float32 lastSample;
		Float32 lastOutSample;

		uint32_t fpd;
	
	struct _dram {
			Float32 d1[503];
		Float32 d2[503];
		Float32 d3[503];
		Float32 d4[503];
		Float32 d5[503];
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
	
	int allpasstemp;
	Float32 outallpass = 0.618033988749894848204586f;
	
	Float32 compress = 1.0f+powf(GetParameter( kParam_One )*0.8f,2);
	
	Float32 speed1 = 128.0f / powf(compress,2);
	speed1 *= overallscale;
	Float32 speed2 = speed1 * 1.4f;
	Float32 speed3 = speed2 * 1.5f;
	Float32 speed4 = speed3 * 1.6f;
	Float32 speed5 = speed4 * 1.7f;
	
	maxdelay1 = (int)(23.0f*overallscale);
	maxdelay2 = (int)(19.0f*overallscale);
	maxdelay3 = (int)(17.0f*overallscale);
	maxdelay4 = (int)(13.0f*overallscale);
	maxdelay5 = (int)(11.0f*overallscale);
	//set up the prime delays
	
	Float32 refclip = 0.999f;
	Float32 softness = 0.435f;
	Float32 invsoft = 0.56f;
	Float32 outsoft = 0.545f;
	Float32 trigger;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
								
		allpasstemp = tap1 - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay1) {allpasstemp = maxdelay1;}
		//set up the delay position
		//using 'tap' and 'allpasstemp' to position the tap		
		inputSample -= dram->d1[allpasstemp]*outallpass;
		dram->d1[tap1] = inputSample;
		inputSample *= outallpass;
		inputSample += (dram->d1[allpasstemp]);
		//allpass stage
		tap1--; if (tap1 < 0 || tap1 > maxdelay1) {tap1 = maxdelay1;}
		//decrement the position for reals
		
		inputSample *= c1;
		trigger = fabs(inputSample)*4.7f;
		if (trigger > 4.7f) trigger = 4.7f;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0f;
		if (trigger < -4.2f) trigger = -4.2f;
		c1 += trigger/speed5;
		if (c1 > compress) c1 = compress;
		//compress stage
				
		allpasstemp = tap2 - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay2) {allpasstemp = maxdelay2;}
		//set up the delay position
		//using 'tap' and 'allpasstemp' to position the tap
		inputSample -= dram->d2[allpasstemp]*outallpass;
		dram->d2[tap2] = inputSample;
		inputSample *= outallpass;
		inputSample += (dram->d2[allpasstemp]);
		//allpass stage
		tap2--; if (tap2 < 0 || tap2 > maxdelay2) {tap2 = maxdelay2;}
		//decrement the position for reals
		
		inputSample *= c2;
		trigger = fabs(inputSample)*4.7f;
		if (trigger > 4.7f) trigger = 4.7f;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0f;
		if (trigger < -4.2f) trigger = -4.2f;
		c2 += trigger/speed4;
		if (c2 > compress) c2 = compress;
		//compress stage
				
		allpasstemp = tap3 - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay3) {allpasstemp = maxdelay3;}
		//set up the delay position
		//using 'tap' and 'allpasstemp' to position the tap
		inputSample -= dram->d3[allpasstemp]*outallpass;
		dram->d3[tap3] = inputSample;
		inputSample *= outallpass;
		inputSample += (dram->d3[allpasstemp]);
		//allpass stage
		tap3--; if (tap3 < 0 || tap3 > maxdelay3) {tap3 = maxdelay3;}
		//decrement the position for reals
		
		inputSample *= c3;
		trigger = fabs(inputSample)*4.7f;
		if (trigger > 4.7f) trigger = 4.7f;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0f;
		if (trigger < -4.2f) trigger = -4.2f;
		c3 += trigger/speed3;
		if (c3 > compress) c3 = compress;
		//compress stage
				
		allpasstemp = tap4 - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay4) {allpasstemp = maxdelay4;}
		//set up the delay position
		//using 'tap' and 'allpasstemp' to position the tap
		inputSample -= dram->d4[allpasstemp]*outallpass;
		dram->d4[tap4] = inputSample;
		inputSample *= outallpass;
		inputSample += (dram->d4[allpasstemp]);
		//allpass stage
		tap4--; if (tap4 < 0 || tap4 > maxdelay4) {tap4 = maxdelay4;}
		//decrement the position for reals
		
		inputSample *= c4;
		trigger = fabs(inputSample)*4.7f;
		if (trigger > 4.7f) trigger = 4.7f;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0f;
		if (trigger < -4.2f) trigger = -4.2f;
		c4 += trigger/speed2;
		if (c4 > compress) c4 = compress;
		//compress stage
				
		allpasstemp = tap5 - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay5) {allpasstemp = maxdelay5;}
		//set up the delay position
		//using 'tap' and 'allpasstemp' to position the tap
		inputSample -= dram->d5[allpasstemp]*outallpass;
		dram->d5[tap5] = inputSample;
		inputSample *= outallpass;
		inputSample += (dram->d5[allpasstemp]);
		//allpass stage
		tap5--; if (tap5 < 0 || tap5 > maxdelay5) {tap5 = maxdelay5;}
		//decrement the position for reals
		
		inputSample *= c5;
		trigger = fabs(inputSample)*4.7f;
		if (trigger > 4.7f) trigger = 4.7f;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0f;
		if (trigger < -4.2f) trigger = -4.2f;
		c5 += trigger/speed1;
		if (c5 > compress) c5 = compress;
		//compress stage
		
		if (compress > 1.0f) inputSample /= compress;
		
		if (lastSample >= refclip)
		{
			if (inputSample < refclip)
			{
				lastSample = (outsoft + (inputSample * softness));
			}
			else lastSample = refclip;
		}
		
		if (lastSample <= -refclip)
		{
			if (inputSample > -refclip)
			{
				lastSample = (-outsoft + (inputSample * softness));
			}
			else lastSample = -refclip;
		}
		
		if (inputSample > refclip)
		{
			if (lastSample < refclip)
			{
				inputSample = (invsoft + (lastSample * softness));
			}
			else inputSample = refclip;
		}
		
		if (inputSample < -refclip)
		{
			if (lastSample > -refclip)
			{
				inputSample = (-invsoft + (lastSample * softness));
			}
			else inputSample = -refclip;
		}

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 502; count++) {dram->d1[count] = 0.0; dram->d2[count] = 0.0; dram->d3[count] = 0.0; dram->d4[count] = 0.0; dram->d5[count] = 0.0;}
	tap1 = 1; tap2 = 1; tap3 = 1; tap4 = 1; tap5 = 1;
	maxdelay1 = 9001; maxdelay2 = 9001; maxdelay3 = 9001; maxdelay4 = 9001; maxdelay5 = 9001;
	c1 = 2.0; c2 = 2.0; c3 = 2.0; c4 = 2.0; c5 = 2.0; //startup comp gains
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
