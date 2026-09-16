#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ResEQ2"
#define AIRWINDOWS_DESCRIPTION "A single, sharp, sonorous mid peak."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','R','e','2' )
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
{ .name = "MSweep", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MBoost", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		float prevfreqMPeak;
		float prevamountMPeak;
		int mpc;
		
		uint32_t fpd;
	
	struct _dram {
			float mpk[2005];
		float f[66];
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
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	
	//begin ResEQ2 Mid Boost
	float freqMPeak = powf(GetParameter( kParam_One )+0.15f,3);
	float amountMPeak = powf(GetParameter( kParam_Two ),2);
	int maxMPeak = (amountMPeak*63.0f)+1;
	if ((freqMPeak != prevfreqMPeak)||(amountMPeak != prevamountMPeak)) {
	for (int x = 0; x < maxMPeak; x++) {
		if (((float)x*freqMPeak) < M_PI_4) dram->f[x] = sin(((float)x*freqMPeak)*4.0f)*freqMPeak*sin(((float)(maxMPeak-x)/(float)maxMPeak)*M_PI_2);
		else dram->f[x] = cos((float)x*freqMPeak)*freqMPeak*sin(((float)(maxMPeak-x)/(float)maxMPeak)*M_PI_2);
	}
	prevfreqMPeak = freqMPeak; prevamountMPeak = amountMPeak;
	}//end ResEQ2 Mid Boost
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		//begin ResEQ2 Mid Boost
		mpc++; if (mpc < 1 || mpc > 2001) mpc = 1;
		dram->mpk[mpc] = inputSample;
		float midMPeak = 0.0f;
		for (int x = 0; x < maxMPeak; x++) {
			int y = x*cycleEnd;
			switch (cycleEnd)
			{
				case 1: 
					midMPeak += (dram->mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x]); break;
				case 2: 
					midMPeak += ((dram->mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.5f); y--;
					midMPeak += ((dram->mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.5f); break;
				case 3: 
					midMPeak += ((dram->mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333f); y--;
					midMPeak += ((dram->mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333f); y--;
					midMPeak += ((dram->mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333f); break;
				case 4: 
					midMPeak += ((dram->mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f); y--;
					midMPeak += ((dram->mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f); y--;
					midMPeak += ((dram->mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f); y--;
					midMPeak += ((dram->mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f); //break
			}
		}
		inputSample = (midMPeak*amountMPeak)+((1.5f-amountMPeak>1.0f)?inputSample:inputSample*(1.5f-amountMPeak));
		//end ResEQ2 Mid Boost
	
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 2004; count++) {dram->mpk[count] = 0.0;}
	for(int count = 0; count < 65; count++) {dram->f[count] = 0.0;}
	prevfreqMPeak = -1;
	prevamountMPeak = -1;
	mpc = 1;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
