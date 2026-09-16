#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BeziCompMono"
#define AIRWINDOWS_DESCRIPTION "BeziCompMono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','e','o' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
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
{ .name = "Comp", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		enum {
			bez_AL,
			bez_BL,
			bez_CL,
			bez_InL,
			bez_UnInL,
			bez_SampL,
			bez_cycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		
		float lastSampleL;
		float intermediateL[16];
		bool wasPosClipL;
		bool wasNegClipL;

		uint32_t fpd;
	
	struct _dram {
			float bezComp[bez_total];
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

	float bezCThresh = powf(GetParameter( kParam_A ),2.0f) * 64.0f;
	float bezMakeUp = sqrt(bezCThresh+1.0f);
	float bezRez = (powf(GetParameter( kParam_B ),6.0f)+0.0001f)/overallscale; if (bezRez > 1.0f) bezRez = 1.0f;
	float wet = GetParameter( kParam_C );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		float drySampleL = inputSampleL;
		
		dram->bezComp[bez_cycle] += bezRez;
		dram->bezComp[bez_SampL] += (fabs(inputSampleL) * bezRez);
		
		if (dram->bezComp[bez_cycle] > 1.0f) {
			dram->bezComp[bez_cycle] -= 1.0f;
			dram->bezComp[bez_CL] = dram->bezComp[bez_BL];
			dram->bezComp[bez_BL] = dram->bezComp[bez_AL];
			dram->bezComp[bez_AL] = dram->bezComp[bez_SampL];
			dram->bezComp[bez_SampL] = 0.0f;
		}
		float CBL = (dram->bezComp[bez_CL]*(1.0f-dram->bezComp[bez_cycle]))+(dram->bezComp[bez_BL]*dram->bezComp[bez_cycle]);
		float BAL = (dram->bezComp[bez_BL]*(1.0f-dram->bezComp[bez_cycle]))+(dram->bezComp[bez_AL]*dram->bezComp[bez_cycle]);
		float CBAL = (dram->bezComp[bez_BL]+(CBL*(1.0f-dram->bezComp[bez_cycle]))+(BAL*dram->bezComp[bez_cycle]))*0.5f;
		inputSampleL *= 1.0f-(fmin(CBAL*bezCThresh,1.0f));
		inputSampleL *= bezMakeUp;
		
		if (wet < 1.0f) inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
		
		//begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0f) inputSampleL = 4.0f; if (inputSampleL < -4.0f) inputSampleL = -4.0f;
		if (wasPosClipL == true) { //current will be over
			if (inputSampleL<lastSampleL) lastSampleL=0.7058208f+(inputSampleL*0.2609148f);
			else lastSampleL = 0.2491717f+(lastSampleL*0.7390851f);
		} wasPosClipL = false;
		if (inputSampleL>0.9549925859f) {wasPosClipL=true;inputSampleL=0.7058208f+(lastSampleL*0.2609148f);}
		if (wasNegClipL == true) { //current will be -over
			if (inputSampleL > lastSampleL) lastSampleL=-0.7058208f+(inputSampleL*0.2609148f);
			else lastSampleL=-0.2491717f+(lastSampleL*0.7390851f);
		} wasNegClipL = false;
		if (inputSampleL<-0.9549925859f) {wasNegClipL=true;inputSampleL=-0.7058208f+(lastSampleL*0.2609148f);}
		intermediateL[spacing] = inputSampleL;
        inputSampleL = lastSampleL; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediateL[x-1] = intermediateL[x];
		lastSampleL = intermediateL[0]; //run a little buffer to handle this
		//end ClipOnly2 mono as a little, compressed chunk that can be dropped into code*/
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bez_total; x++) {dram->bezComp[x] = 0.0;}
	dram->bezComp[bez_cycle] = 1.0;

	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	for (int x = 0; x < 16; x++) {intermediateL[x] = 0.0;}

	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
