#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ClipOnly3"
#define AIRWINDOWS_DESCRIPTION "Just plain stinkin' loud."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','l','3' )
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
 
		float lastSampleL;
		bool wasPosClipL;
		bool wasNegClipL;
		uint32_t fpdL;
	
	struct _dram {
			float intermediateL[18];
		float slewL[34];
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
	
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;

	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		
		//begin ClipOnly3 as a little, compressed chunk that can be dropped into code
		float noise = 1.0f-((float(fpdL)/UINT32_MAX)*0.076f);
		if (wasPosClipL == true) { //current will be over
			if (inputSampleL<lastSampleL) lastSampleL=(0.9085097f*noise)+(inputSampleL*(1.0f-noise));
			else lastSampleL = 0.94f; //~-0.2dB to nearly match ClipOnly and ClipOnly2
		} wasPosClipL = false;
		if (inputSampleL>0.9085097f) {wasPosClipL=true;inputSampleL=(0.9085097f*noise)+(lastSampleL*(1.0f-noise));}
		if (wasNegClipL == true) { //current will be -over
			if (inputSampleL > lastSampleL) lastSampleL=(-0.9085097f*noise)+(inputSampleL*(1.0f-noise));
			else lastSampleL = -0.94f;
		} wasNegClipL = false;
		if (inputSampleL<-0.9085097f) {wasNegClipL=true;inputSampleL=(-0.9085097f*noise)+(lastSampleL*(1.0f-noise));}
		dram->slewL[spacing*2] = fabs(lastSampleL-inputSampleL);
		for (int x = spacing*2; x > 0; x--) dram->slewL[x-1] = dram->slewL[x];
		dram->intermediateL[spacing] = inputSampleL; inputSampleL = lastSampleL;
		//Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) {dram->intermediateL[x-1] = dram->intermediateL[x];} lastSampleL = dram->intermediateL[0];
		if (wasPosClipL || wasNegClipL) {
			for (int x = spacing; x > 0; x--) lastSampleL += dram->intermediateL[x];
			lastSampleL /= spacing;
		} float finalSlew = 0.0f;
		for (int x = spacing*2; x >= 0; x--) if (finalSlew < dram->slewL[x]) finalSlew = dram->slewL[x];
		float postclip = 0.94f / (1.0f+(finalSlew*1.3986013f));
		if (inputSampleL > postclip) inputSampleL = postclip; if (inputSampleL < -postclip) inputSampleL = -postclip;
		//end ClipOnly3 as a little, compressed chunk that can be dropped into code
		
		
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	for (int x = 0; x < 17; x++) dram->intermediateL[x] = 0.0;
	for (int x = 0; x < 33; x++) dram->slewL[x] = 0.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
}
};
