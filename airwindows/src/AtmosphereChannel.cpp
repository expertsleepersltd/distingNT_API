#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "AtmosphereChannel"
#define AIRWINDOWS_DESCRIPTION "Console5 processing with powerful new acoustic distance effects."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','t','l' )
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
{ .name = "Input Pad", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 gainchase;
		Float32 settingchase;
		Float32 chasespeed;		
		
		uint32_t fpd;
		float lastSampleA;
		float lastSampleB;
		float lastSampleC;
		float lastSampleD;
		float lastSampleE;
		float lastSampleF;
		float lastSampleG;
		float lastSampleH;
		float lastSampleI;
		float lastSampleJ;
		float lastSampleK;
		float lastSampleL;
		float lastSampleM;
		float thresholdA;
		float thresholdB;
		float thresholdC;
		float thresholdD;
		float thresholdE;
		float thresholdF;
		float thresholdG;
		float thresholdH;
		float thresholdI;
		float thresholdJ;
		float thresholdK;
		float thresholdL;
		float thresholdM;
	
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
	float inputSample;
	float drySample;
	float clamp;
	Float32 inputgain = GetParameter( kParam_One );
	if (settingchase != inputgain) {
		chasespeed *= 2.0f;
		settingchase = inputgain;
	}
	if (chasespeed > 2500.0f) chasespeed = 2500.0f;
	if (gainchase < 0.0f) gainchase = inputgain;	
	
	thresholdA = 0.618033988749894f / overallscale;
	thresholdB = 0.679837387624884f / overallscale;
	thresholdC = 0.747821126387373f / overallscale;
	thresholdD = 0.82260323902611f / overallscale;
	thresholdE = 0.904863562928721f / overallscale;
	thresholdF = 0.995349919221593f / overallscale;
	thresholdG = 1.094884911143752f / overallscale;
	thresholdH = 1.204373402258128f / overallscale;
	thresholdI = 1.32481074248394f / overallscale;
	thresholdJ = 1.457291816732335f / overallscale;
	thresholdK = 1.603020998405568f / overallscale;
	thresholdL = 1.763323098246125f / overallscale;
	thresholdM = 1.939655408070737f / overallscale;	
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		chasespeed *= 0.9999f;
		chasespeed -= 0.01f;
		if (chasespeed < 350.0f) chasespeed = 350.0f;
		//we have our chase speed compensated for recent fader activity
		
		gainchase = (((gainchase*chasespeed)+inputgain)/(chasespeed+1.0f));
		//gainchase is chasing the target, as a simple multiply gain factor
		
		if (1.0f != gainchase) inputSample *= gainchase;
		//done with trim control
		
		drySample = inputSample;
		
		inputSample = sin(inputSample);
		//amplitude aspect
				
		clamp = inputSample - lastSampleA;
		if (clamp > thresholdA) inputSample = lastSampleA + thresholdA;
		if (-clamp > thresholdA) inputSample = lastSampleA - thresholdA;

		clamp = inputSample - lastSampleB;
		if (clamp > thresholdB) inputSample = lastSampleB + thresholdB;
		if (-clamp > thresholdB) inputSample = lastSampleB - thresholdB;

		clamp = inputSample - lastSampleC;
		if (clamp > thresholdC) inputSample = lastSampleC + thresholdC;
		if (-clamp > thresholdC) inputSample = lastSampleC - thresholdC;
		
		clamp = inputSample - lastSampleD;
		if (clamp > thresholdD) inputSample = lastSampleD + thresholdD;
		if (-clamp > thresholdD) inputSample = lastSampleD - thresholdD;
		
		clamp = inputSample - lastSampleE;
		if (clamp > thresholdE) inputSample = lastSampleE + thresholdE;
		if (-clamp > thresholdE) inputSample = lastSampleE - thresholdE;
		
		clamp = inputSample - lastSampleF;
		if (clamp > thresholdF) inputSample = lastSampleF + thresholdF;
		if (-clamp > thresholdF) inputSample = lastSampleF - thresholdF;
		
		clamp = inputSample - lastSampleG;
		if (clamp > thresholdG) inputSample = lastSampleG + thresholdG;
		if (-clamp > thresholdG) inputSample = lastSampleG - thresholdG;
		
		clamp = inputSample - lastSampleH;
		if (clamp > thresholdH) inputSample = lastSampleH + thresholdH;
		if (-clamp > thresholdH) inputSample = lastSampleH - thresholdH;
		
		clamp = inputSample - lastSampleI;
		if (clamp > thresholdI) inputSample = lastSampleI + thresholdI;
		if (-clamp > thresholdI) inputSample = lastSampleI - thresholdI;
		
		clamp = inputSample - lastSampleJ;
		if (clamp > thresholdJ) inputSample = lastSampleJ + thresholdJ;
		if (-clamp > thresholdJ) inputSample = lastSampleJ - thresholdJ;
		
		clamp = inputSample - lastSampleK;
		if (clamp > thresholdK) inputSample = lastSampleK + thresholdK;
		if (-clamp > thresholdK) inputSample = lastSampleK - thresholdK;
		
		clamp = inputSample - lastSampleL;
		if (clamp > thresholdL) inputSample = lastSampleL + thresholdL;
		if (-clamp > thresholdL) inputSample = lastSampleL - thresholdL;
		
		clamp = inputSample - lastSampleM;
		if (clamp > thresholdM) inputSample = lastSampleM + thresholdM;
		if (-clamp > thresholdM) inputSample = lastSampleM - thresholdM;
		
		
		lastSampleM = lastSampleL;
		lastSampleL = lastSampleK;
		lastSampleK = lastSampleJ;
		lastSampleJ = lastSampleI;
		lastSampleI = lastSampleH;
		lastSampleH = lastSampleG;
		lastSampleG = lastSampleF;
		lastSampleF = lastSampleE;
		lastSampleE = lastSampleD;
		lastSampleD = lastSampleC;
		lastSampleC = lastSampleB;
		lastSampleB = lastSampleA;
		lastSampleA = drySample;
		//store the raw input sample again for use next time
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	gainchase = -90.0;
	settingchase = -90.0;
	chasespeed = 350.0;

	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	lastSampleA = 0.0;
	lastSampleB = 0.0;
	lastSampleC = 0.0;
	lastSampleD = 0.0;
	lastSampleE = 0.0;
	lastSampleF = 0.0;
	lastSampleG = 0.0;
	lastSampleH = 0.0;
	lastSampleI = 0.0;
	lastSampleJ = 0.0;
	lastSampleK = 0.0;
	lastSampleL = 0.0;
	lastSampleM = 0.0;
	thresholdA = 0.618033988749894;
	thresholdB = 0.679837387624884;
	thresholdC = 0.747821126387373;
	thresholdD = 0.82260323902611;
	thresholdE = 0.904863562928721;
	thresholdF = 0.995349919221593;
	thresholdG = 1.094884911143752;
	thresholdH = 1.204373402258128;
	thresholdI = 1.32481074248394;
	thresholdJ = 1.457291816732335;
	thresholdK = 1.603020998405568;
	thresholdL = 1.763323098246125;
	thresholdM = 1.939655408070737;
}
};
