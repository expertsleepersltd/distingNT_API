#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "TubeDesk"
#define AIRWINDOWS_DESCRIPTION "A tube recording console type tone coloring."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','u','k' )
#define AIRWINDOWS_KERNELS
enum {

	kNumberOfParameters=0
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
};
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 control;
		int gcount;
		float lastSample;
		float lastOutSample;
		Float32 lastSlew;
		uint32_t fpd;
	
	struct _dram {
			Float32 d[5000];
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
	
	Float32 intensity = 0.4384938f;
	Float32 depthA = 549.0f;
	int offsetA = (int)(depthA * overallscale);
	if (offsetA < 1) offsetA = 1;
	if (offsetA > 2440) offsetA = 2440;
	
	Float32 clamp;
	Float32 thickness;
	Float32 out;
	Float32 gain = 0.5f;
	Float32 slewgain = 0.128f;	
	Float32 prevslew = 0.105f;
	Float32 balanceB = 0.0001f;
	slewgain *= overallscale;
	prevslew *= overallscale;
	balanceB /= overallscale;
	Float32 balanceA = 1.0f - balanceB;
	Float32 slew;
	Float32 bridgerectifier;
	Float32 combSample;
	float inputSample;
	float drySample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		if (gcount < 0 || gcount > 2450) {gcount = 2450;}
		
		dram->d[gcount+2450] = dram->d[gcount] = fabs(inputSample)*intensity;
		control += (dram->d[gcount] / offsetA);
		control -= (dram->d[gcount+offsetA] / offsetA);
		control -= 0.000001f;
		
		clamp = 1;
		if (control < 0) {control = 0;}
		if (control > 1) {clamp -= (control - 1); control = 1;}
		if (clamp < 0.5f) {clamp = 0.5f;}
		
		gcount--;
		
		//control = 0 to 1
		thickness = ((1.0f - control) * 2.0f) - 1.0f;
		out = fabs(thickness);		
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSample > 0) inputSample = (inputSample*(1-out))+(bridgerectifier*out);
		else inputSample = (inputSample*(1-out))-(bridgerectifier*out);
		//blend according to density control
		
		inputSample *= clamp;
		
		slew = inputSample - lastSample;
		lastSample = inputSample;
		//Set up direct reference for slew
		
		
		bridgerectifier = fabs(slew*slewgain);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.0f;
		else bridgerectifier = sin(bridgerectifier);
		if (slew > 0) slew = bridgerectifier/slewgain;
		else slew = -(bridgerectifier/slewgain);
		
		inputSample = (lastOutSample*balanceA) + (lastSample*balanceB) + slew;
		//go from last slewed, but include some raw values
		lastOutSample = inputSample;
		//Set up slewed reference
		
		combSample = fabs(drySample*lastSample);
		if (combSample > 1.0f) combSample = 1.0f;
		//bailout for very high input gains
		inputSample -= (lastSlew * combSample * prevslew);
		lastSlew = slew;
		//slew interaction with previous slew
		
		inputSample *= gain;
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.0f;
		else bridgerectifier = sin(bridgerectifier);
		
		if (inputSample > 0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//drive section
		inputSample /= gain;
		//end of Desk section
		
		
		
		*destP = inputSample;
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	int count;
	for(count = 0; count < 4999; count++) {dram->d[count] = 0;}
	control = 0;
	gcount = 0;
	lastSample = 0.0;
	lastOutSample = 0.0;
	lastSlew = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
