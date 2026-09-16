#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "GrooveWear"
#define AIRWINDOWS_DESCRIPTION "For scrubbing highs off mechanically like a stylus would."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','r','s' )
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
{ .name = "Wear", .min = 0, .max = 1000, .def = 64, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		Float32 aMidPrev;
		Float32 bMidPrev;
		Float32 cMidPrev;
		Float32 dMidPrev;
		
		uint32_t fpd;
	
	struct _dram {
			Float32 aMid[21];
		Float32 bMid[21];
		Float32 cMid[21];
		Float32 dMid[21];
		Float32 fMid[21];		
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

	Float32 overallscale = (powf(GetParameter( kParam_One ),2)*19.0f)+1.0f;
	Float32 gain = overallscale;
	//mid groove wear
	if (gain > 1.0f) {dram->fMid[0] = 1.0f; gain -= 1.0f;} else {dram->fMid[0] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[1] = 1.0f; gain -= 1.0f;} else {dram->fMid[1] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[2] = 1.0f; gain -= 1.0f;} else {dram->fMid[2] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[3] = 1.0f; gain -= 1.0f;} else {dram->fMid[3] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[4] = 1.0f; gain -= 1.0f;} else {dram->fMid[4] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[5] = 1.0f; gain -= 1.0f;} else {dram->fMid[5] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[6] = 1.0f; gain -= 1.0f;} else {dram->fMid[6] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[7] = 1.0f; gain -= 1.0f;} else {dram->fMid[7] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[8] = 1.0f; gain -= 1.0f;} else {dram->fMid[8] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[9] = 1.0f; gain -= 1.0f;} else {dram->fMid[9] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[10] = 1.0f; gain -= 1.0f;} else {dram->fMid[10] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[11] = 1.0f; gain -= 1.0f;} else {dram->fMid[11] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[12] = 1.0f; gain -= 1.0f;} else {dram->fMid[12] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[13] = 1.0f; gain -= 1.0f;} else {dram->fMid[13] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[14] = 1.0f; gain -= 1.0f;} else {dram->fMid[14] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[15] = 1.0f; gain -= 1.0f;} else {dram->fMid[15] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[16] = 1.0f; gain -= 1.0f;} else {dram->fMid[16] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[17] = 1.0f; gain -= 1.0f;} else {dram->fMid[17] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[18] = 1.0f; gain -= 1.0f;} else {dram->fMid[18] = gain; gain = 0.0f;}
	if (gain > 1.0f) {dram->fMid[19] = 1.0f; gain -= 1.0f;} else {dram->fMid[19] = gain; gain = 0.0f;}
	//there, now we have a neat little moving average with remainders
	
	if (overallscale < 1.0f) overallscale = 1.0f;
	dram->fMid[0] /= overallscale;
	dram->fMid[1] /= overallscale;
	dram->fMid[2] /= overallscale;
	dram->fMid[3] /= overallscale;
	dram->fMid[4] /= overallscale;
	dram->fMid[5] /= overallscale;
	dram->fMid[6] /= overallscale;
	dram->fMid[7] /= overallscale;
	dram->fMid[8] /= overallscale;
	dram->fMid[9] /= overallscale;
	dram->fMid[10] /= overallscale;
	dram->fMid[11] /= overallscale;
	dram->fMid[12] /= overallscale;
	dram->fMid[13] /= overallscale;
	dram->fMid[14] /= overallscale;
	dram->fMid[15] /= overallscale;
	dram->fMid[16] /= overallscale;
	dram->fMid[17] /= overallscale;
	dram->fMid[18] /= overallscale;
	dram->fMid[19] /= overallscale;
	//and now it's neatly scaled, too

	Float32 aWet = 1.0f;
	Float32 bWet = 1.0f;
	Float32 cWet = 1.0f;
	Float32 dWet = GetParameter( kParam_Two )*4.0f;
	//four-stage wet/dry control using progressive stages that bypass when not engaged
	if (dWet < 1.0f) {aWet = dWet; bWet = 0.0f; cWet = 0.0f; dWet = 0.0f;}
	else if (dWet < 2.0f) {bWet = dWet - 1.0f; cWet = 0.0f; dWet = 0.0f;}
	else if (dWet < 3.0f) {cWet = dWet - 2.0f; dWet = 0.0f;}
	else {dWet -= 3.0f;}
	//this is one way to make a little set of dry/wet stages that are successively added to the
	//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	//beyond that point: this is a way to progressively add a 'black box' sound processing
	//which lets you fall through to simpler processing at lower settings.
	
	//now we set them up so each full intensity one is blended evenly with dry for each stage.
	//That's because the GrooveWear algorithm works best combined with dry.
	aWet *= 0.5f;
	bWet *= 0.5f;
	cWet *= 0.5f;
	dWet *= 0.5f;
	//if you are using a more typical algorithm (like a sin() or something) you won't use this part

	Float32 aDry = 1.0f - aWet;
	Float32 bDry = 1.0f - bWet;
	Float32 cDry = 1.0f - cWet;
	Float32 dDry = 1.0f - dWet;
	
	Float32 drySample;
	float inputSample;
	Float32 accumulatorSample;
	Float32 correction;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;		
		
		if (aWet > 0.0f) {
			dram->aMid[19] = dram->aMid[18]; dram->aMid[18] = dram->aMid[17]; dram->aMid[17] = dram->aMid[16]; dram->aMid[16] = dram->aMid[15];
			dram->aMid[15] = dram->aMid[14]; dram->aMid[14] = dram->aMid[13]; dram->aMid[13] = dram->aMid[12]; dram->aMid[12] = dram->aMid[11];
			dram->aMid[11] = dram->aMid[10]; dram->aMid[10] = dram->aMid[9];
			dram->aMid[9] = dram->aMid[8]; dram->aMid[8] = dram->aMid[7]; dram->aMid[7] = dram->aMid[6]; dram->aMid[6] = dram->aMid[5];
			dram->aMid[5] = dram->aMid[4]; dram->aMid[4] = dram->aMid[3]; dram->aMid[3] = dram->aMid[2]; dram->aMid[2] = dram->aMid[1];
			dram->aMid[1] = dram->aMid[0]; dram->aMid[0] = accumulatorSample = (inputSample-aMidPrev);
			//this is different from Aura because that is accumulating rates of change OF the rate of change
			accumulatorSample *= dram->fMid[0];
			accumulatorSample += (dram->aMid[1] * dram->fMid[1]);
			accumulatorSample += (dram->aMid[2] * dram->fMid[2]);
			accumulatorSample += (dram->aMid[3] * dram->fMid[3]);
			accumulatorSample += (dram->aMid[4] * dram->fMid[4]);
			accumulatorSample += (dram->aMid[5] * dram->fMid[5]);
			accumulatorSample += (dram->aMid[6] * dram->fMid[6]);
			accumulatorSample += (dram->aMid[7] * dram->fMid[7]);
			accumulatorSample += (dram->aMid[8] * dram->fMid[8]);
			accumulatorSample += (dram->aMid[9] * dram->fMid[9]);
			accumulatorSample += (dram->aMid[10] * dram->fMid[10]);
			accumulatorSample += (dram->aMid[11] * dram->fMid[11]);
			accumulatorSample += (dram->aMid[12] * dram->fMid[12]);
			accumulatorSample += (dram->aMid[13] * dram->fMid[13]);
			accumulatorSample += (dram->aMid[14] * dram->fMid[14]);
			accumulatorSample += (dram->aMid[15] * dram->fMid[15]);
			accumulatorSample += (dram->aMid[16] * dram->fMid[16]);
			accumulatorSample += (dram->aMid[17] * dram->fMid[17]);
			accumulatorSample += (dram->aMid[18] * dram->fMid[18]);
			accumulatorSample += (dram->aMid[19] * dram->fMid[19]);
			//we are doing our repetitive calculations on a separate value
			correction = (inputSample-aMidPrev) - accumulatorSample;
			aMidPrev = inputSample;		
			inputSample -= correction;
			inputSample = (inputSample * aWet) + (drySample * aDry);
			drySample = inputSample;		
		}
		
		if (bWet > 0.0f) {
			dram->bMid[19] = dram->bMid[18]; dram->bMid[18] = dram->bMid[17]; dram->bMid[17] = dram->bMid[16]; dram->bMid[16] = dram->bMid[15];
			dram->bMid[15] = dram->bMid[14]; dram->bMid[14] = dram->bMid[13]; dram->bMid[13] = dram->bMid[12]; dram->bMid[12] = dram->bMid[11];
			dram->bMid[11] = dram->bMid[10]; dram->bMid[10] = dram->bMid[9];
			dram->bMid[9] = dram->bMid[8]; dram->bMid[8] = dram->bMid[7]; dram->bMid[7] = dram->bMid[6]; dram->bMid[6] = dram->bMid[5];
			dram->bMid[5] = dram->bMid[4]; dram->bMid[4] = dram->bMid[3]; dram->bMid[3] = dram->bMid[2]; dram->bMid[2] = dram->bMid[1];
			dram->bMid[1] = dram->bMid[0]; dram->bMid[0] = accumulatorSample = (inputSample-bMidPrev);
			
			accumulatorSample *= dram->fMid[0];
			accumulatorSample += (dram->bMid[1] * dram->fMid[1]);
			accumulatorSample += (dram->bMid[2] * dram->fMid[2]);
			accumulatorSample += (dram->bMid[3] * dram->fMid[3]);
			accumulatorSample += (dram->bMid[4] * dram->fMid[4]);
			accumulatorSample += (dram->bMid[5] * dram->fMid[5]);
			accumulatorSample += (dram->bMid[6] * dram->fMid[6]);
			accumulatorSample += (dram->bMid[7] * dram->fMid[7]);
			accumulatorSample += (dram->bMid[8] * dram->fMid[8]);
			accumulatorSample += (dram->bMid[9] * dram->fMid[9]);
			accumulatorSample += (dram->bMid[10] * dram->fMid[10]);
			accumulatorSample += (dram->bMid[11] * dram->fMid[11]);
			accumulatorSample += (dram->bMid[12] * dram->fMid[12]);
			accumulatorSample += (dram->bMid[13] * dram->fMid[13]);
			accumulatorSample += (dram->bMid[14] * dram->fMid[14]);
			accumulatorSample += (dram->bMid[15] * dram->fMid[15]);
			accumulatorSample += (dram->bMid[16] * dram->fMid[16]);
			accumulatorSample += (dram->bMid[17] * dram->fMid[17]);
			accumulatorSample += (dram->bMid[18] * dram->fMid[18]);
			accumulatorSample += (dram->bMid[19] * dram->fMid[19]);
			//we are doing our repetitive calculations on a separate value
			correction = (inputSample-bMidPrev) - accumulatorSample;
			bMidPrev = inputSample;
			inputSample -= correction;
			inputSample = (inputSample * bWet) + (drySample * bDry);
			drySample = inputSample;		
		}
		
		if (cWet > 0.0f) {
			dram->cMid[19] = dram->cMid[18]; dram->cMid[18] = dram->cMid[17]; dram->cMid[17] = dram->cMid[16]; dram->cMid[16] = dram->cMid[15];
			dram->cMid[15] = dram->cMid[14]; dram->cMid[14] = dram->cMid[13]; dram->cMid[13] = dram->cMid[12]; dram->cMid[12] = dram->cMid[11];
			dram->cMid[11] = dram->cMid[10]; dram->cMid[10] = dram->cMid[9];
			dram->cMid[9] = dram->cMid[8]; dram->cMid[8] = dram->cMid[7]; dram->cMid[7] = dram->cMid[6]; dram->cMid[6] = dram->cMid[5];
			dram->cMid[5] = dram->cMid[4]; dram->cMid[4] = dram->cMid[3]; dram->cMid[3] = dram->cMid[2]; dram->cMid[2] = dram->cMid[1];
			dram->cMid[1] = dram->cMid[0]; dram->cMid[0] = accumulatorSample = (inputSample-cMidPrev);
			
			accumulatorSample *= dram->fMid[0];
			accumulatorSample += (dram->cMid[1] * dram->fMid[1]);
			accumulatorSample += (dram->cMid[2] * dram->fMid[2]);
			accumulatorSample += (dram->cMid[3] * dram->fMid[3]);
			accumulatorSample += (dram->cMid[4] * dram->fMid[4]);
			accumulatorSample += (dram->cMid[5] * dram->fMid[5]);
			accumulatorSample += (dram->cMid[6] * dram->fMid[6]);
			accumulatorSample += (dram->cMid[7] * dram->fMid[7]);
			accumulatorSample += (dram->cMid[8] * dram->fMid[8]);
			accumulatorSample += (dram->cMid[9] * dram->fMid[9]);
			accumulatorSample += (dram->cMid[10] * dram->fMid[10]);
			accumulatorSample += (dram->cMid[11] * dram->fMid[11]);
			accumulatorSample += (dram->cMid[12] * dram->fMid[12]);
			accumulatorSample += (dram->cMid[13] * dram->fMid[13]);
			accumulatorSample += (dram->cMid[14] * dram->fMid[14]);
			accumulatorSample += (dram->cMid[15] * dram->fMid[15]);
			accumulatorSample += (dram->cMid[16] * dram->fMid[16]);
			accumulatorSample += (dram->cMid[17] * dram->fMid[17]);
			accumulatorSample += (dram->cMid[18] * dram->fMid[18]);
			accumulatorSample += (dram->cMid[19] * dram->fMid[19]);
			//we are doing our repetitive calculations on a separate value
			correction = (inputSample-cMidPrev) - accumulatorSample;
			cMidPrev = inputSample;
			inputSample -= correction;
			inputSample = (inputSample * cWet) + (drySample * cDry);
			drySample = inputSample;		
		}
		
		if (dWet > 0.0f) {
			dram->dMid[19] = dram->dMid[18]; dram->dMid[18] = dram->dMid[17]; dram->dMid[17] = dram->dMid[16]; dram->dMid[16] = dram->dMid[15];
			dram->dMid[15] = dram->dMid[14]; dram->dMid[14] = dram->dMid[13]; dram->dMid[13] = dram->dMid[12]; dram->dMid[12] = dram->dMid[11];
			dram->dMid[11] = dram->dMid[10]; dram->dMid[10] = dram->dMid[9];
			dram->dMid[9] = dram->dMid[8]; dram->dMid[8] = dram->dMid[7]; dram->dMid[7] = dram->dMid[6]; dram->dMid[6] = dram->dMid[5];
			dram->dMid[5] = dram->dMid[4]; dram->dMid[4] = dram->dMid[3]; dram->dMid[3] = dram->dMid[2]; dram->dMid[2] = dram->dMid[1];
			dram->dMid[1] = dram->dMid[0]; dram->dMid[0] = accumulatorSample = (inputSample-dMidPrev);
			
			accumulatorSample *= dram->fMid[0];
			accumulatorSample += (dram->dMid[1] * dram->fMid[1]);
			accumulatorSample += (dram->dMid[2] * dram->fMid[2]);
			accumulatorSample += (dram->dMid[3] * dram->fMid[3]);
			accumulatorSample += (dram->dMid[4] * dram->fMid[4]);
			accumulatorSample += (dram->dMid[5] * dram->fMid[5]);
			accumulatorSample += (dram->dMid[6] * dram->fMid[6]);
			accumulatorSample += (dram->dMid[7] * dram->fMid[7]);
			accumulatorSample += (dram->dMid[8] * dram->fMid[8]);
			accumulatorSample += (dram->dMid[9] * dram->fMid[9]);
			accumulatorSample += (dram->dMid[10] * dram->fMid[10]);
			accumulatorSample += (dram->dMid[11] * dram->fMid[11]);
			accumulatorSample += (dram->dMid[12] * dram->fMid[12]);
			accumulatorSample += (dram->dMid[13] * dram->fMid[13]);
			accumulatorSample += (dram->dMid[14] * dram->fMid[14]);
			accumulatorSample += (dram->dMid[15] * dram->fMid[15]);
			accumulatorSample += (dram->dMid[16] * dram->fMid[16]);
			accumulatorSample += (dram->dMid[17] * dram->fMid[17]);
			accumulatorSample += (dram->dMid[18] * dram->fMid[18]);
			accumulatorSample += (dram->dMid[19] * dram->fMid[19]);
			//we are doing our repetitive calculations on a separate value
			correction = (inputSample-dMidPrev) - accumulatorSample;
			dMidPrev = inputSample;
			inputSample -= correction;
			inputSample = (inputSample * dWet) + (drySample * dDry);
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 21; count++) {
		dram->aMid[count] = 0.0;
		dram->bMid[count] = 0.0;
		dram->cMid[count] = 0.0;
		dram->dMid[count] = 0.0;
		dram->fMid[count] = 0.0;
	}
	aMidPrev = 0.0;
	bMidPrev = 0.0;
	cMidPrev = 0.0;
	dMidPrev = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
