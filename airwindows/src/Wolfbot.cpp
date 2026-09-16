#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Wolfbot"
#define AIRWINDOWS_DESCRIPTION "An aggressive Kalman bandpass with evil in its heart."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','W','o','t' )
#define AIRWINDOWS_KERNELS
enum {

	//Add your parameters here...
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
 
		enum {
			prevSampL1,
			prevSlewL1,
			accSlewL1,
			prevSampL2,
			prevSlewL2,
			accSlewL2,
			prevSampL3,
			prevSlewL3,
			accSlewL3,
			kalGainL,
			kalOutL,
			kal_total
		};

		uint32_t fpd;
	
	struct _dram {
			float kHP[kal_total];
		float kLP[kal_total];
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
	
	float kalHP = 1.0f-(0.004225f/overallscale);
	float kalLP = 1.0f-(0.954529f/overallscale);
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		//begin Kalman Filter
		float dryKal = inputSample = inputSample*(1.0f-kalHP)*0.777f;
		inputSample *= (1.0f-kalHP);
		//set up gain levels to control the beast
		dram->kHP[prevSlewL3] += dram->kHP[prevSampL3] - dram->kHP[prevSampL2]; dram->kHP[prevSlewL3] *= 0.5f;
		dram->kHP[prevSlewL2] += dram->kHP[prevSampL2] - dram->kHP[prevSampL1]; dram->kHP[prevSlewL2] *= 0.5f;
		dram->kHP[prevSlewL1] += dram->kHP[prevSampL1] - inputSample; dram->kHP[prevSlewL1] *= 0.5f;
		//make slews from each set of samples used
		dram->kHP[accSlewL2] += dram->kHP[prevSlewL3] - dram->kHP[prevSlewL2]; dram->kHP[accSlewL2] *= 0.5f;
		dram->kHP[accSlewL1] += dram->kHP[prevSlewL2] - dram->kHP[prevSlewL1]; dram->kHP[accSlewL1] *= 0.5f;
		//differences between slews: rate of change of rate of change
		dram->kHP[accSlewL3] += (dram->kHP[accSlewL2] - dram->kHP[accSlewL1]); dram->kHP[accSlewL3] *= 0.5f;
		//entering the abyss, what even is this
		dram->kHP[kalOutL] += dram->kHP[prevSampL1] + dram->kHP[prevSlewL2] + dram->kHP[accSlewL3]; dram->kHP[kalOutL] *= 0.5f;
		//resynthesizing predicted result (all iir smoothed)
		dram->kHP[kalGainL] += fabs(dryKal-dram->kHP[kalOutL])*kalHP*8.0f; dram->kHP[kalGainL] *= 0.5f;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kHP[kalGainL] > kalHP*0.5f) dram->kHP[kalGainL] = kalHP*0.5f;
		//attempts to avoid explosions
		dram->kHP[kalOutL] += (dryKal*(1.0f-(0.68f+(kalHP*0.157f))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kHP[prevSampL3] = dram->kHP[prevSampL2];
		dram->kHP[prevSampL2] = dram->kHP[prevSampL1];
		dram->kHP[prevSampL1] = (dram->kHP[kalGainL] * dram->kHP[kalOutL]) + ((1.0f-dram->kHP[kalGainL])*dryKal);
		//feed the chain of previous samples
		if (dram->kHP[prevSampL1] > 1.0f) dram->kHP[prevSampL1] = 1.0f;
		if (dram->kHP[prevSampL1] < -1.0f) dram->kHP[prevSampL1] = -1.0f;
		//end Kalman Filter, except for trim on output		
		inputSample = drySample+(dram->kHP[kalOutL]*-0.777f); //highpass
		
		//begin Kalman Filter
		dryKal = inputSample = inputSample*(1.0f-kalLP)*0.777f;
		inputSample *= (1.0f-kalLP);
		//set up gain levels to control the beast
		dram->kLP[prevSlewL3] += dram->kLP[prevSampL3] - dram->kLP[prevSampL2]; dram->kLP[prevSlewL3] *= 0.5f;
		dram->kLP[prevSlewL2] += dram->kLP[prevSampL2] - dram->kLP[prevSampL1]; dram->kLP[prevSlewL2] *= 0.5f;
		dram->kLP[prevSlewL1] += dram->kLP[prevSampL1] - inputSample; dram->kLP[prevSlewL1] *= 0.5f;
		//make slews from each set of samples used
		dram->kLP[accSlewL2] += dram->kLP[prevSlewL3] - dram->kLP[prevSlewL2]; dram->kLP[accSlewL2] *= 0.5f;
		dram->kLP[accSlewL1] += dram->kLP[prevSlewL2] - dram->kLP[prevSlewL1]; dram->kLP[accSlewL1] *= 0.5f;
		//differences between slews: rate of change of rate of change
		dram->kLP[accSlewL3] += (dram->kLP[accSlewL2] - dram->kLP[accSlewL1]); dram->kLP[accSlewL3] *= 0.5f;
		//entering the abyss, what even is this
		dram->kLP[kalOutL] += dram->kLP[prevSampL1] + dram->kLP[prevSlewL2] + dram->kLP[accSlewL3]; dram->kLP[kalOutL] *= 0.5f;
		//resynthesizing predicted result (all iir smoothed)
		dram->kLP[kalGainL] += fabs(dryKal-dram->kLP[kalOutL])*kalLP*8.0f; dram->kLP[kalGainL] *= 0.5f;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kLP[kalGainL] > kalLP*0.5f) dram->kLP[kalGainL] = kalLP*0.5f;
		//attempts to avoid explosions
		dram->kLP[kalOutL] += (dryKal*(1.0f-(0.68f+(kalLP*0.157f))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kLP[prevSampL3] = dram->kLP[prevSampL2];
		dram->kLP[prevSampL2] = dram->kLP[prevSampL1];
		dram->kLP[prevSampL1] = (dram->kLP[kalGainL] * dram->kLP[kalOutL]) + ((1.0f-dram->kLP[kalGainL])*dryKal);
		//feed the chain of previous samples
		if (dram->kLP[prevSampL1] > 1.0f) dram->kLP[prevSampL1] = 1.0f;
		if (dram->kLP[prevSampL1] < -1.0f) dram->kLP[prevSampL1] = -1.0f;
		//end Kalman Filter, except for trim on output		
		inputSample = sin(dram->kLP[kalOutL]*0.7943f)*1.2589f; //lowpass
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < kal_total; x++) {dram->kHP[x] = 0.0; dram->kLP[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
