#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "MultiBandDistortion"
#define AIRWINDOWS_DESCRIPTION "An old weird gnarly sound wrecker :)"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','u','n' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	kParam_Seven =6,
	kParam_Eight =7,
	kParam_Nine =8,
	//Add your parameters here...
	kNumberOfParameters=9
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Frequency", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "High Gain", .min = 0, .max = 4800, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Low Gain", .min = 0, .max = 4800, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "High Hardness", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low Hardness", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "High Matrix", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low Matrix", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Stabilize", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Trim", .min = -4800, .max = 0, .def = -1200, .unit = kNT_unitDb, .scaling = kNT_scaling100, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 ataLast3Sample;
		Float32 ataLast2Sample;
		Float32 ataLast1Sample;
		Float32 ataHalfwaySample;
		Float32 ataHalfDrySample;
		Float32 ataHalfDiffSample;
		Float32 ataA;
		Float32 ataB;
		Float32 ataC;
		Float32 ataDecay;
		Float32 ataUpsampleHighTweak;
		Float32 ataDrySample;
		Float32 ataDiffSample;
		Float32 ataPrevDiffSample;
		bool ataFlip; //end defining of antialiasing variables
		Float32 iirSampleA;
		Float32 iirSampleB;
	
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
	
	Float32 iirAmount = powf(GetParameter( kParam_One ),3)/overallscale;
	Float32 gainH = powf(10.0f,GetParameter( kParam_Two )/20);

	Float32 thresholdH = GetParameter( kParam_Four );
	Float32 hardnessH;
	if (thresholdH < 1.0f) hardnessH = 1.0f / (1.0f-thresholdH);
	else hardnessH = 999999999999999999999.0f;

	Float32 gainL = powf(10.0f,GetParameter( kParam_Three )/20);
	Float32 thresholdL = GetParameter( kParam_Five );
	Float32 hardnessL;
	if (thresholdL < 1.0f) hardnessL = 1.0f / (1.0f-thresholdL);
	else hardnessL = 999999999999999999999.0f;
	
	Float32 breakup = 1.5707963267949f;
	Float32 bridgerectifier;

	Float32 outputH = GetParameter( kParam_Six );
	Float32 outputL = GetParameter( kParam_Seven );
	Float32 outputD = GetParameter( kParam_Eight )*0.597f;
	Float32 outtrim = outputH + outputL + outputD;
	outputH *= outtrim;
	outputL *= outtrim;
	outputD *= outtrim;
	Float32 outputGlobal = powf(10.0f,GetParameter( kParam_Nine )/20);
	

	Float32 inputSample;
	Float32 tempH;
	Float32 tempL;
	
	while (nSampleFrames-- > 0) {
		ataDrySample = inputSample = *sourceP;
		ataHalfDrySample = ataHalfwaySample = (inputSample + ataLast1Sample + ((-ataLast2Sample + ataLast3Sample) * ataUpsampleHighTweak)) / 2.0f;
		ataLast3Sample = ataLast2Sample; ataLast2Sample = ataLast1Sample; ataLast1Sample = inputSample;
		//setting up oversampled special antialiasing
		
	//pre-center code on inputSample and halfwaySample in parallel
//begin interpolated sample- change inputSample -> ataHalfwaySample, ataDrySample -> ataHalfDrySample
		tempL = iirSampleA = (iirSampleA * (1 - iirAmount)) + (ataHalfwaySample * iirAmount);
		tempH = ataHalfwaySample - iirSampleA;
		//highpass section


		tempH *= gainH;
		if (fabs(tempH) > thresholdH)
			{
				bridgerectifier = (fabs(tempH)-thresholdH)*hardnessH;
				//skip flat area if any, scale to distortion limit
				if (bridgerectifier > breakup) bridgerectifier = breakup;
				//max value for sine function, 'breakup' modeling for trashed console tone
				//more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
				bridgerectifier = sin(bridgerectifier)/hardnessH;
				//do the sine factor, scale back to proper amount
				if (tempH > 0) tempH = bridgerectifier+thresholdH;
				else tempH = -(bridgerectifier+thresholdH);
			}
			//ADClip
		tempL *= gainL;
		if (fabs(tempL) > thresholdL)
			{
				bridgerectifier = (fabs(tempL)-thresholdL)*hardnessL;
				//skip flat area if any, scale to distortion limit
				if (bridgerectifier > breakup) bridgerectifier = breakup;
				//max value for sine function, 'breakup' modeling for trashed console tone
				//more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
				bridgerectifier = sin(bridgerectifier)/hardnessL;
				//do the sine factor, scale back to proper amount
				if (tempL > 0) tempL = bridgerectifier+thresholdL;
				else tempL = -(bridgerectifier+thresholdL);
			}
			//ADClip
			ataHalfwaySample = (tempL * outputL) + (tempH * outputH);
	//end interpolated sample


//begin raw sample- inputSample and ataDrySample handled separately here
		tempL = iirSampleB = (iirSampleB * (1 - iirAmount)) + (inputSample * iirAmount);
		tempH = inputSample - iirSampleB;
		//highpass section

		tempH *= gainH;
		if (fabs(tempH) > thresholdH)
			{
				bridgerectifier = (fabs(tempH)-thresholdH)*hardnessH;
				//skip flat area if any, scale to distortion limit
				if (bridgerectifier > breakup) bridgerectifier = breakup;
				//max value for sine function, 'breakup' modeling for trashed console tone
				//more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
				bridgerectifier = sin(bridgerectifier)/hardnessH;
				//do the sine factor, scale back to proper amount
				if (tempH > 0) tempH = bridgerectifier+thresholdH;
				else tempH = -(bridgerectifier+thresholdH);
			}
			//ADClip
		tempL *= gainL;
		if (fabs(tempL) > thresholdL)
			{
				bridgerectifier = (fabs(tempL)-thresholdL)*hardnessL;
				//skip flat area if any, scale to distortion limit
				if (bridgerectifier > breakup) bridgerectifier = breakup;
				//max value for sine function, 'breakup' modeling for trashed console tone
				//more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
				bridgerectifier = sin(bridgerectifier)/hardnessL;
				//do the sine factor, scale back to proper amount
				if (tempL > 0) tempL = bridgerectifier+thresholdL;
				else tempL = -(bridgerectifier+thresholdL);
			}
			//ADClip
			inputSample = (tempL * outputL) + (tempH * outputH);
	//end raw sample
	
		
//begin center code handling conv stuff tied to 44.1K, or stuff in time domain like delays
		//ataHalfwaySample -= inputSample;
		//retain only difference with raw signal
		
		//inputSample += convolutionstuff[count];

		//ataHalfwaySample += inputSample;
		//restore interpolated signal including time domain stuff now
	//end center code for handling timedomain/conv stuff
		
	//post-center code on inputSample and halfwaySample in parallel
//begin raw sample- inputSample and ataDrySample handled separately here
		//inputSample *= gain;
	//end raw sample
	
//begin interpolated sample- change inputSample -> ataHalfwaySample, ataDrySample -> ataHalfDrySample
		//ataHalfwaySample *= gain;
	//end interpolated sample
	
		//begin antialiasing section for halfway sample
		ataC = ataHalfwaySample - ataHalfDrySample;
		if (ataFlip) {ataA *= ataDecay; ataB *= ataDecay; ataA += ataC; ataB -= ataC; ataC = ataA;}
		else {ataB *= ataDecay; ataA *= ataDecay; ataB += ataC; ataA -= ataC; ataC = ataB;}
		ataHalfDiffSample = (ataC * ataDecay);
		//end antialiasing section for halfway sample
		
		//begin antialiasing section for raw sample
		ataC = inputSample - ataDrySample;
		if (ataFlip) {ataA *= ataDecay; ataB *= ataDecay; ataA += ataC; ataB -= ataC; ataC = ataA;}
		else {ataB *= ataDecay; ataA *= ataDecay; ataB += ataC; ataA -= ataC; ataC = ataB;}
		ataDiffSample = (ataC * ataDecay);
		//end antialiasing section for input sample
		
		ataFlip = !ataFlip;
		
		inputSample = ataDrySample*outputD; inputSample += (ataDiffSample + ataHalfDiffSample);
		//apply processing as difference to non-oversampled raw input
		
		
		
		//inputSample *= output; *destP = (ataDrySample*dry)+(inputSample*wet);
		//built in output trim and dry/wet if desired
		*destP = inputSample*outputGlobal;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	ataLast3Sample = ataLast2Sample = ataLast1Sample = 0.0;
	ataHalfwaySample = ataHalfDrySample = ataHalfDiffSample = 0.0;
	ataA = ataB = ataC = ataDrySample = ataDiffSample = ataPrevDiffSample = 0.0;
	ataUpsampleHighTweak = 0.0414213562373095048801688; //more adds treble to upsampling
	ataDecay = 0.915965594177219015; //Catalan's constant, more adds focus and clarity
	ataFlip = false; //end reset of antialias parameters
	iirSampleA = 0; iirSampleB = 0;
}
};
