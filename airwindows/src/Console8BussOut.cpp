#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Console8BussOut"
#define AIRWINDOWS_DESCRIPTION "Moves to a channel/submix/buss topology and adds analog simulation."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o',' ' )
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
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float inTrimA;
		float inTrimB;
		bool hsr;
		enum {
			fix_freq,
			fix_reso,
			fix_a0,
			fix_a1,
			fix_a2,
			fix_b1,
			fix_b2,
			fix_sL1,
			fix_sL2,
			fix_sR1,
			fix_sR2,
			fix_total
		}; //fixed frequency biquad filter for ultrasonics, stereo
		float lastSample;
		bool wasPosClip;
		bool wasNegClip;
		int spacing; //ClipOnly2
		uint32_t fpd;
	
	struct _dram {
			float fix[fix_total];
		float intermediate[18];
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
	
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_One )*2.0f;
	//0.5f is unity gain, and we can attenuate to silence or boost slightly over 12dB
	//into softclip and ADClip in case we need intense loudness bursts on transients.
	//24 bit is 8388608.0f, 16 bit is 32768.0f
	if (GetSampleRate() > 49000.0f) hsr = true;
	else hsr = false;
	dram->fix[fix_freq] = 24000.0f / GetSampleRate();
    dram->fix[fix_reso] = 0.52110856f;
	float K = tan(M_PI * dram->fix[fix_freq]); //lowpass
	float norm = 1.0f / (1.0f + K / dram->fix[fix_reso] + K * K);
	dram->fix[fix_a0] = K * K * norm;
	dram->fix[fix_a1] = 2.0f * dram->fix[fix_a0];
	dram->fix[fix_a2] = dram->fix[fix_a0];
	dram->fix[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fix[fix_b2] = (1.0f - K / dram->fix[fix_reso] + K * K) * norm;
	//this is the fixed biquad distributed anti-aliasing filter
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();	
	spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16; //ADClip2
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		
		float position = (float)nSampleFrames/inFramesToProcess;
		float inTrim = (inTrimA*position)+(inTrimB*(1.0f-position));
		//presence smoothed to cut out zipper noise
		inputSample *= inTrim;
		if (inputSample > 1.57079633f) inputSample = 1.57079633f; if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//Console8 gain stage clips at exactly 1.0f post-sin()
		if (hsr){
			float outSample = (inputSample * dram->fix[fix_a0]) + dram->fix[fix_sL1];
			dram->fix[fix_sL1] = (inputSample * dram->fix[fix_a1]) - (outSample * dram->fix[fix_b1]) + dram->fix[fix_sL2];
			dram->fix[fix_sL2] = (inputSample * dram->fix[fix_a2]) - (outSample * dram->fix[fix_b2]);
			inputSample = outSample;
		} //fixed biquad filtering ultrasonics
		inputSample *= inTrim;
		//the final output fader, before ClipOnly2
		
		if (inputSample > 4.0f) inputSample = 4.0f; if (inputSample < -4.0f) inputSample = -4.0f;
		if (wasPosClip) { //current will be over
			if (inputSample<lastSample) lastSample=0.7058208f+(inputSample*0.2609148f);
			else lastSample = 0.2491717f+(lastSample*0.7390851f);
		} wasPosClip = false;
		if (inputSample>0.9549925859f) {wasPosClip=true;inputSample=0.7058208f+(lastSample*0.2609148f);}
		if (wasNegClip) { //current will be -over
			if (inputSample > lastSample) lastSample=-0.7058208f+(inputSample*0.2609148f);
			else lastSample=-0.2491717f+(lastSample*0.7390851f);
		} wasNegClip = false;
		if (inputSample<-0.9549925859f) {wasNegClip=true;inputSample=-0.7058208f+(lastSample*0.2609148f);}
		dram->intermediate[spacing] = inputSample;
        inputSample = lastSample; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) dram->intermediate[x-1] = dram->intermediate[x];
		lastSample = dram->intermediate[0]; //run a little buffer to handle this
		//ClipOnly2
				
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	inTrimA = 0.5; inTrimB = 0.5;
	for (int x = 0; x < fix_total; x++) dram->fix[x] = 0.0;
	lastSample = 0.0;
	wasPosClip = false;
	wasNegClip = false;
	for (int x = 0; x < 17; x++) dram->intermediate[x] = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
