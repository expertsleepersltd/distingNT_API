#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "AQuickVoiceClip"
#define AIRWINDOWS_DESCRIPTION "Softens headset mic recordings that have been super hard clipped on capture."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','Q','p' )
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
{ .name = "Highpass", .min = 300, .max = 30000, .def = 1860, .unit = kNT_unitNone, .scaling = kNT_scaling10, .enumStrings = NULL },
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
 
		Float32 ataLast6Sample;
		Float32 ataLast5Sample;
		Float32 ataLast4Sample;
		Float32 ataLast3Sample;
		Float32 ataLast2Sample;
		Float32 ataLast1Sample;
		Float32 ataHalfwaySample;
		Float32 ataHalfDrySample;
		Float32 ataHalfDiffSample;
		Float32 ataLastDiffSample;
		Float32 ataDrySample;
		Float32 ataDiffSample;
		Float32 ataPrevDiffSample;
		Float32 ataK1;
		Float32 ataK2;
		Float32 ataK3;
		Float32 ataK4;
		Float32 ataK5;
		Float32 ataK6;
		Float32 ataK7;
		Float32 ataK8; //end antialiasing variables
		Float32 lastSample;
		Float32 lastOutSample;
		Float32 lastOut2Sample;
		Float32 lastOut3Sample;
		Float32 lpDepth;
		Float32 overshoot;
		Float32 overall;
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 iirSampleC;
		Float32 iirSampleD;
		bool flip;
		Float32 fpNShape;
		uint32_t fpd;
	
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
	
	Float32 softness = 0.484416f;
	Float32 hardness = 1.0f - softness;
	
	Float32 iirAmount = GetParameter( kParam_One )/8000.0f;
	
	iirAmount /= overallscale;
	Float32 altAmount = (1.0f - iirAmount);
	
	Float32 cancelnew = 0.0682276f;
	Float32 cancelold = 1.0f - cancelnew;
	
	Float32 maxRecent;
	
	Float32 lpSpeed = 0.0009f;

	Float32 cliplevel = 0.98f;
	
	Float32 refclip = 0.5f; //preset to cut out gain quite a lot. 91%? no touchy unless clip
	
	Float32 inputSample;
	Float32 passThrough;
	Float32 outputSample;
	bool clipOnset;
	Float32 drySample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		passThrough = ataDrySample = inputSample;
		
		
		ataHalfDrySample = ataHalfwaySample = (inputSample + ataLast1Sample + (ataLast2Sample*ataK1) + (ataLast3Sample*ataK2) + (ataLast4Sample*ataK6) + (ataLast5Sample*ataK7) + (ataLast6Sample*ataK8)) / 2.0f;
		ataLast6Sample = ataLast5Sample; ataLast5Sample = ataLast4Sample; ataLast4Sample = ataLast3Sample; ataLast3Sample = ataLast2Sample; ataLast2Sample = ataLast1Sample; ataLast1Sample = inputSample;
		//setting up oversampled special antialiasing
		clipOnset = false;
		
		
		maxRecent = fabs( ataLast6Sample );
		if (fabs( ataLast5Sample ) > maxRecent ) maxRecent = fabs( ataLast5Sample );
		if (fabs( ataLast4Sample ) > maxRecent ) maxRecent = fabs( ataLast4Sample );
		if (fabs( ataLast3Sample ) > maxRecent ) maxRecent = fabs( ataLast3Sample );
		if (fabs( ataLast2Sample ) > maxRecent ) maxRecent = fabs( ataLast2Sample );
		if (fabs( ataLast1Sample ) > maxRecent ) maxRecent = fabs( ataLast1Sample );
		if (fabs( inputSample ) > maxRecent ) maxRecent = fabs( inputSample );
		//this gives us something that won't cut out in zero crossings, to interpolate with
		
		maxRecent *= 2.0f;
		//by refclip this is 1.0f and fully into the antialiasing
		if (maxRecent > 1.0f) maxRecent = 1.0f;
		//and it tops out at 1. Higher means more antialiasing, lower blends into passThrough without antialiasing
		
		ataHalfwaySample -= overall;
		//subtract dist-cancel from input after getting raw input, before doing anything
		
		drySample = ataHalfwaySample;
		

		if (lastSample >= refclip)
		{
			lpDepth += 0.1f;
			if (ataHalfwaySample < refclip)
			{
				lastSample = ((refclip*hardness) + (ataHalfwaySample * softness));
			}
			else lastSample = refclip;
		}
		
		if (lastSample <= -refclip)
		{
			lpDepth += 0.1f;
			if (ataHalfwaySample > -refclip)
			{
				lastSample = ((-refclip*hardness) + (ataHalfwaySample * softness));
			}
			else lastSample = -refclip;
		}
		
		if (ataHalfwaySample > refclip)
		{
			lpDepth += 0.1f;
			if (lastSample < refclip)
			{
				ataHalfwaySample = ((refclip*hardness) + (lastSample * softness));
			}
			else ataHalfwaySample = refclip;
		}
		
		if (ataHalfwaySample < -refclip)
		{
			lpDepth += 0.1f;
			if (lastSample > -refclip)
			{
				ataHalfwaySample = ((-refclip*hardness) + (lastSample * softness));
			}
			else ataHalfwaySample = -refclip;
		}
				
        outputSample = lastSample;
		lastSample = ataHalfwaySample;
		ataHalfwaySample = outputSample;
		//swap around in a circle for one final ADClip,
		//this time not tracking overshoot anymore
		
		//end interpolated sample
		
		//begin raw sample- inputSample and ataDrySample handled separately here
		
		inputSample -= overall;
		//subtract dist-cancel from input after getting raw input, before doing anything
		
		drySample = inputSample;
		
		if (lastSample >= refclip)
		{
			lpDepth += 0.1f;
			if (inputSample < refclip)
			{
				lastSample = ((refclip*hardness) + (inputSample * softness));
			}
			else lastSample = refclip;
		}
		
		if (lastSample <= -refclip)
		{
			lpDepth += 0.1f;
			if (inputSample > -refclip)
			{
				lastSample = ((-refclip*hardness) + (inputSample * softness));
			}
			else lastSample = -refclip;
		}
		
		if (inputSample > refclip)
		{
			lpDepth += 0.1f;
			if (lastSample < refclip)
			{
				inputSample = ((refclip*hardness) + (lastSample * softness));
			}
			else inputSample = refclip;
		}
		
		if (inputSample < -refclip)
		{
			lpDepth += 0.1f;
			if (lastSample > -refclip)
			{
				inputSample = ((-refclip*hardness) + (lastSample * softness));
			}
			else inputSample = -refclip;
		}
		
        outputSample = lastSample;
		lastSample = inputSample;
		inputSample = outputSample;
		
		//end raw sample
		
		ataHalfDrySample = (ataDrySample*ataK3)+(ataHalfDrySample*ataK4);
		ataHalfDiffSample = (ataHalfwaySample - ataHalfDrySample)/2.0f;
		ataLastDiffSample = ataDiffSample*ataK5;
		ataDiffSample = (inputSample - ataDrySample)/2.0f;
		ataDiffSample += ataHalfDiffSample;
		ataDiffSample -= ataLastDiffSample;
		inputSample = ataDrySample;
		inputSample += ataDiffSample;
		
		
		overall = (overall * cancelold) + (ataDiffSample * cancelnew);
		//apply all the diffs to a lowpassed IIR
		
		if (flip)
		{
			iirSampleA = (iirSampleA * altAmount) + (inputSample * iirAmount);
			inputSample -= iirSampleA;
			iirSampleC = (iirSampleC * altAmount) + (passThrough * iirAmount);
			passThrough -= iirSampleC;
		}
		else
		{
			iirSampleB = (iirSampleB * altAmount) + (inputSample * iirAmount);
			inputSample -= iirSampleB;
			iirSampleD = (iirSampleD * altAmount) + (passThrough * iirAmount);
			passThrough -= iirSampleD;
		}
		flip = !flip;
		//highpass section 
		
		
		lastOut3Sample = lastOut2Sample;
		lastOut2Sample = lastOutSample;
		lastOutSample = inputSample;
		
		lpDepth -= lpSpeed;
		if (lpDepth > 0.0f)
		{
			if (lpDepth > 1.0f) lpDepth = 1.0f;
			inputSample *= (1.0f-lpDepth);
			inputSample += (((lastOutSample + lastOut2Sample + lastOut3Sample) / 3.6f)*lpDepth);
		}
		if (lpDepth < 0.0f) lpDepth = 0.0f;
				
		inputSample *= (1.0f-maxRecent);
		inputSample += (passThrough * maxRecent);
		//there's our raw signal, without antialiasing. Brings up low level stuff and softens more when hot
		
		if (inputSample > cliplevel) inputSample = cliplevel;
		if (inputSample < -cliplevel) inputSample = -cliplevel;
		//final iron bar
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	ataLast6Sample = ataLast5Sample = ataLast4Sample = ataLast3Sample = ataLast2Sample = ataLast1Sample = 0.0;
	ataHalfwaySample = ataHalfDrySample = ataHalfDiffSample = 0.0;
	ataLastDiffSample = ataDrySample = ataDiffSample = ataPrevDiffSample = 0.0;
	ataK1 = -0.646; //first FIR shaping of interpolated sample, brightens
	ataK2 = 0.311; //second FIR shaping of interpolated sample, thickens
	ataK6 = -0.093; //third FIR shaping of interpolated sample, brings air
	ataK7 = 0.057; //fourth FIR shaping of interpolated sample, thickens
	ataK8 = -0.023; //fifth FIR shaping of interpolated sample, brings air
	ataK3 = 0.114; //add raw to interpolated dry, toughens
	ataK4 = 0.886; //remainder of interpolated dry, adds up to 1.0
	ataK5 = 0.431; //subtract this much prev. diff sample, brightens.  0.431 becomes flat
	lastSample = 0.0;
	lastOutSample = 0.0;
	lastOut2Sample = 0.0;
	lastOut3Sample = 0.0;
	lpDepth = 0.0;
	overshoot = 0.0;
	overall = 0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
