#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "CStrip2"
#define AIRWINDOWS_DESCRIPTION "Refines and optimizes CStrip, by request!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','S','2' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
	kParam_Ten =9,
	//Add your parameters here...
	kNumberOfParameters=10
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Treble", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Treble Freq", .min = 1000, .max = 16000, .def = 6000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Freq", .min = 300, .max = 16000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling10, .enumStrings = NULL },
{ .name = "Low Cap", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "High Cap", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Compression", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Comp Speed", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 3000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		uint32_t fpd;
		
		Float32 lastSample;
		Float32 last2Sample;
		
		//begin Capacitor
		Float32 iirHighpassA;
		Float32 iirHighpassB;
		Float32 iirHighpassC;
		Float32 iirHighpassD;
		Float32 iirHighpassE;
		Float32 iirHighpassF;
		Float32 iirLowpassA;
		Float32 iirLowpassB;
		Float32 iirLowpassC;
		Float32 iirLowpassD;
		Float32 iirLowpassE;
		Float32 iirLowpassF;
		int count;
		//end Capacitor
		
		//begin EQ
		Float32 iirHighSampleA;
		Float32 iirHighSampleB;
		Float32 iirHighSampleC;
		Float32 iirHighSampleD;
		Float32 iirHighSampleE;
		Float32 iirLowSampleA;
		Float32 iirLowSampleB;
		Float32 iirLowSampleC;
		Float32 iirLowSampleD;
		Float32 iirLowSampleE;
		Float32 iirHighSample;
		Float32 iirLowSample;
		
		Float32 tripletA;
		Float32 tripletB;
		Float32 tripletC;
		Float32 tripletFactor;
				
		bool flip;
		int flipthree;
		//end EQ
				
		//begin ButterComp
		Float32 controlApos;
		Float32 controlAneg;
		Float32 controlBpos;
		Float32 controlBneg;
		Float32 targetpos;
		Float32 targetneg;
		Float32 avgA;
		Float32 avgB;
		Float32 nvgA;
		Float32 nvgB;
		//end ButterComp
		//flip is already covered in EQ		
	
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
	Float32 compscale = overallscale;
	overallscale = GetSampleRate();
	compscale = compscale * overallscale;
	//compscale is the one that's 1 or something like 2.2f for 96K rates
	float fpOld = 0.618033988749894848204586f; //golden ratio!
	float fpNew = 1.0f - fpOld;
	
	Float32 inputSample;
	Float32 highSample = 0.0f;
	Float32 midSample = 0.0f;
	Float32 bassSample = 0.0f;
	Float32 densityA = GetParameter( kParam_One )/2.0f;
	Float32 densityB = GetParameter( kParam_Two )/2.0f;
	Float32 densityC = GetParameter( kParam_Three )/2.0f;
	bool engageEQ = true;
	if ( (0.0f == densityA) && (0.0f == densityB) && (0.0f == densityC) ) engageEQ = false;
	
	densityA = powf(10.0f,densityA/20.0f)-1.0f;
	densityB = powf(10.0f,densityB/20.0f)-1.0f;
	densityC = powf(10.0f,densityC/20.0f)-1.0f;
	//convert to 0 to X multiplier with 1.0f being O db
	//minus one gives nearly -1 to ? (should top out at 1)
	//calibrate so that X db roughly equals X db with maximum topping out at 1 internally
	Float32 tripletIntensity = -densityA;
	Float32 iirAmountA = (GetParameter( kParam_Four )*1000)/overallscale;
	Float32 iirAmountB = (GetParameter( kParam_Five )*10)/overallscale;
	Float32 bridgerectifier;
	Float32 outA = fabs(densityA);
	Float32 outB = fabs(densityB);
	Float32 outC = fabs(densityC);
	//end EQ
	
	Float32 lowpassAmount = powf(GetParameter( kParam_Six ),2);
	Float32 highpassAmount = powf(GetParameter( kParam_Seven ),2);
	bool engageCapacitor = false;
	if ((lowpassAmount < 1.0f) || (highpassAmount > 0.0f)) engageCapacitor = true;
	//end Capacitor
	
	//begin ButterComp
	Float32 inputpos;
	Float32 inputneg;
	Float32 calcpos;
	Float32 calcneg;
	Float32 outputpos;
	Float32 outputneg;
	Float32 totalmultiplier;
	Float32 inputgain = (powf(GetParameter( kParam_Eight ),4)*35)+1.0f;
	Float32 compoutgain = inputgain;
	compoutgain -= 1.0f;
	compoutgain /= 1.2f;
	compoutgain += 1.0f;
	Float32 divisor = (0.008f * powf(GetParameter( kParam_Nine ),2))+0.0004f;
	//originally 0.012f
	divisor /= compscale;
	Float32 remainder = divisor;
	divisor = 1.0f - divisor;
	bool engageComp = false;
	if (inputgain > 1.0f) engageComp = true;
	//end ButterComp
	
	Float32 outputgain = GetParameter( kParam_Ten ); //0-3
	Float32 density = outputgain-1.0f; //allow for output 0-1 to be clean, 1-3 all boosts
	if (density < 0.0f) density = 0.0f;
	Float32 phattity = density - 1.0f;
	if (density > 1.0f) density = 1.0f; //max out at full wet for Spiral aspect
	if (phattity < 0.0f) phattity = 0.0f;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (engageCapacitor)
		{
			count++; if (count > 5) count = 0;
			switch (count)
			{
				case 0:
					iirHighpassA = (iirHighpassA * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
					iirLowpassA = (iirLowpassA * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
					iirHighpassB = (iirHighpassB * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassB;
					iirLowpassB = (iirLowpassB * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassB;
					iirHighpassD = (iirHighpassD * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassD;
					iirLowpassD = (iirLowpassD * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassD;
					break;
				case 1:
					iirHighpassA = (iirHighpassA * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
					iirLowpassA = (iirLowpassA * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
					iirHighpassC = (iirHighpassC * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassC;
					iirLowpassC = (iirLowpassC * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassC;
					iirHighpassE = (iirHighpassE * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassE;
					iirLowpassE = (iirLowpassE * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassE;
					break;
				case 2:
					iirHighpassA = (iirHighpassA * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
					iirLowpassA = (iirLowpassA * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
					iirHighpassB = (iirHighpassB * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassB;
					iirLowpassB = (iirLowpassB * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassB;
					iirHighpassF = (iirHighpassF * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassF;
					iirLowpassF = (iirLowpassF * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassF;
					break;
				case 3:
					iirHighpassA = (iirHighpassA * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
					iirLowpassA = (iirLowpassA * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
					iirHighpassC = (iirHighpassC * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassC;
					iirLowpassC = (iirLowpassC * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassC;
					iirHighpassD = (iirHighpassD * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassD;
					iirLowpassD = (iirLowpassD * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassD;
					break;
				case 4:
					iirHighpassA = (iirHighpassA * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
					iirLowpassA = (iirLowpassA * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
					iirHighpassB = (iirHighpassB * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassB;
					iirLowpassB = (iirLowpassB * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassB;
					iirHighpassE = (iirHighpassE * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassE;
					iirLowpassE = (iirLowpassE * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassE;
					break;
				case 5:
					iirHighpassA = (iirHighpassA * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
					iirLowpassA = (iirLowpassA * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
					iirHighpassC = (iirHighpassC * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassC;
					iirLowpassC = (iirLowpassC * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassC;
					iirHighpassF = (iirHighpassF * (1.0f-highpassAmount)) + (inputSample * highpassAmount); inputSample -= iirHighpassF;
					iirLowpassF = (iirLowpassF * (1.0f-lowpassAmount)) + (inputSample * lowpassAmount); inputSample = iirLowpassF;
					break;
			}
			//Highpass Filter chunk. This is three poles of IIR highpass, with a 'gearbox' that progressively
			//steepens the filter after minimizing artifacts.
		}
		
		
		//begin compressor
		if (engageComp)
		{
			flip = !flip;
			inputSample *= inputgain;
			
			inputpos = (inputSample * fpOld) + (avgA * fpNew) + 1.0f;
			avgA = inputSample;
			
			if (inputpos < 0.0f) inputpos = 0.0f;
			outputpos = inputpos / 2.0f;
			if (outputpos > 1.0f) outputpos = 1.0f;		
			inputpos *= inputpos;
			targetpos *= divisor;
			targetpos += (inputpos * remainder);
			calcpos = powf((1.0f/targetpos),2);
			
			inputneg = (-inputSample * fpOld) + (nvgA * fpNew) + 1.0f;
			nvgA = -inputSample;
			
			if (inputneg < 0.0f) inputneg = 0.0f;
			outputneg = inputneg / 2.0f;
			if (outputneg > 1.0f) outputneg = 1.0f;		
			inputneg *= inputneg;
			targetneg *= divisor;
			targetneg += (inputneg * remainder);
			calcneg = powf((1.0f/targetneg),2);
			//now we have mirrored targets for comp
			//outputpos and outputneg go from 0 to 1
			
			if (inputSample > 0)
			{ //working on pos
				if (true == flip)
				{
					controlApos *= divisor;
					controlApos += (calcpos*remainder);
					
				}
				else
				{
					controlBpos *= divisor;
					controlBpos += (calcpos*remainder);
				}	
			}
			else
			{ //working on neg
				if (true == flip)
				{
					controlAneg *= divisor;
					controlAneg += (calcneg*remainder);
				}
				else
				{
					controlBneg *= divisor;
					controlBneg += (calcneg*remainder);
				}
			}
			//this causes each of the four to update only when active and in the correct 'flip'
			
			if (true == flip)
			{totalmultiplier = (controlApos * outputpos) + (controlAneg * outputneg);}
			else
			{totalmultiplier = (controlBpos * outputpos) + (controlBneg * outputneg);}
			//this combines the sides according to flip, blending relative to the input value
			
			inputSample *= totalmultiplier;
			inputSample /= compoutgain;
		}
		//end compressor
		
		//begin EQ
		if (engageEQ)
		{		
			last2Sample = lastSample;
			lastSample = inputSample;
			flipthree++;
			if (flipthree < 1 || flipthree > 3) flipthree = 1;
			switch (flipthree)
			{
				case 1:
					tripletFactor = last2Sample - inputSample;
					tripletA += tripletFactor;
					tripletC -= tripletFactor;
					tripletFactor = tripletA * tripletIntensity;
					iirHighSampleC = (iirHighSampleC * (1 - iirAmountA)) + (inputSample * iirAmountA);
					highSample = inputSample - iirHighSampleC;
					iirLowSampleC = (iirLowSampleC * (1 - iirAmountB)) + (inputSample * iirAmountB);
					bassSample = iirLowSampleC;
					break;
				case 2:
					tripletFactor = last2Sample - inputSample;
					tripletB += tripletFactor;
					tripletA -= tripletFactor;
					tripletFactor = tripletB * tripletIntensity;
					iirHighSampleD = (iirHighSampleD * (1 - iirAmountA)) + (inputSample * iirAmountA);
					highSample = inputSample - iirHighSampleD;
					iirLowSampleD = (iirLowSampleD * (1 - iirAmountB)) + (inputSample * iirAmountB);
					bassSample = iirLowSampleD;
					break;
				case 3:
					tripletFactor = last2Sample - inputSample;
					tripletC += tripletFactor;
					tripletB -= tripletFactor;
					tripletFactor = tripletC * tripletIntensity;
					iirHighSampleE = (iirHighSampleE * (1 - iirAmountA)) + (inputSample * iirAmountA);
					highSample = inputSample - iirHighSampleE;
					iirLowSampleE = (iirLowSampleE * (1 - iirAmountB)) + (inputSample * iirAmountB);
					bassSample = iirLowSampleE;
					break;
			}
			tripletA /= 2.0f;
			tripletB /= 2.0f;
			tripletC /= 2.0f;
			highSample = highSample + tripletFactor;
			
			if (flip)
			{
				iirHighSampleA = (iirHighSampleA * (1 - iirAmountA)) + (highSample * iirAmountA);
				highSample = highSample - iirHighSampleA;
				iirLowSampleA = (iirLowSampleA * (1 - iirAmountB)) + (bassSample * iirAmountB);
				bassSample = iirLowSampleA;
			}
			else
			{
				iirHighSampleB = (iirHighSampleB * (1 - iirAmountA)) + (highSample * iirAmountA);
				highSample = highSample - iirHighSampleB;
				iirLowSampleB = (iirLowSampleB * (1 - iirAmountB)) + (bassSample * iirAmountB);
				bassSample = iirLowSampleB;
			}
			iirHighSample = (iirHighSample * (1 - iirAmountA)) + (highSample * iirAmountA);
			highSample = highSample - iirHighSample;
			iirLowSample = (iirLowSample * (1 - iirAmountB)) + (bassSample * iirAmountB);
			bassSample = iirLowSample;
			
			midSample = (inputSample-bassSample)-highSample;
			
			//drive section
			highSample *= (densityA+1.0f);
			bridgerectifier = fabs(highSample)*1.57079633f;
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			//max value for sine function
			if (densityA > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1-cos(bridgerectifier);
			//produce either boosted or starved version
			if (highSample > 0) highSample = (highSample*(1-outA))+(bridgerectifier*outA);
			else highSample = (highSample*(1-outA))-(bridgerectifier*outA);
			//blend according to densityA control
			
			midSample *= (densityB+1.0f);
			bridgerectifier = fabs(midSample)*1.57079633f;
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			//max value for sine function
			if (densityB > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1-cos(bridgerectifier);
			//produce either boosted or starved version
			if (midSample > 0) midSample = (midSample*(1-outB))+(bridgerectifier*outB);
			else midSample = (midSample*(1-outB))-(bridgerectifier*outB);
			//blend according to densityB control
			
			bassSample *= (densityC+1.0f);
			bridgerectifier = fabs(bassSample)*1.57079633f;
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			//max value for sine function
			if (densityC > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1-cos(bridgerectifier);
			//produce either boosted or starved version
			if (bassSample > 0) bassSample = (bassSample*(1-outC))+(bridgerectifier*outC);
			else bassSample = (bassSample*(1-outC))-(bridgerectifier*outC);
			//blend according to densityC control
			
			inputSample = midSample;
			inputSample += highSample;
			inputSample += bassSample;
		}
		//end EQ
		
		if (outputgain != 1.0f) {
			inputSample *= outputgain;
			float rawSample = inputSample; //Spiral crossfades from unity gain to boosted
			if (inputSample > 1.0f) inputSample = 1.0f;
			if (inputSample < -1.0f) inputSample = -1.0f; //all boosts clipped to 0dB
			float phatSample = sin(inputSample * 1.57079633f);
			inputSample *= 1.2533141373155f;
			//clip to 1.2533141373155f to reach maximum output, or 1.57079633f for pure sine 'phat' version
			float distSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0f) ?1.0f:fabs(inputSample));
			inputSample = distSample; //purest form is full Spiral
			if (density < 1.0f) inputSample = (rawSample*(1.0f-density))+(distSample*density); //fade Spiral aspect
			if (phattity > 0.0f) inputSample = (inputSample*(1.0f-phattity))+(phatSample*phattity); //apply original Density on top
			//output section is a pad from 0 to 1, Spiral from 1 to 2, Density from 2 to 3
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	
	iirHighpassA = 0.0;
	iirHighpassB = 0.0;
	iirHighpassC = 0.0;
	iirHighpassD = 0.0;
	iirHighpassE = 0.0;
	iirHighpassF = 0.0;
	iirLowpassA = 0.0;
	iirLowpassB = 0.0;
	iirLowpassC = 0.0;
	iirLowpassD = 0.0;
	iirLowpassE = 0.0;
	iirLowpassF = 0.0;
	count = 0;
	
	lastSample = 0.0;
	last2Sample = 0.0;
	
	iirHighSampleA = 0.0;
	iirHighSampleB = 0.0;
	iirHighSampleC = 0.0;
	iirHighSampleD = 0.0;
	iirHighSampleE = 0.0;
	iirLowSampleA = 0.0;
	iirLowSampleB = 0.0;
	iirLowSampleC = 0.0;
	iirLowSampleD = 0.0;
	iirLowSampleE = 0.0;
	iirHighSample = 0.0;
	iirLowSample = 0.0;
	tripletA = 0.0;
	tripletB = 0.0;
	tripletC = 0.0;
	tripletFactor = 0.0;
	flip = false;
	flipthree = 0;
	//end EQ
	
	//begin ButterComp
	controlApos = 1.0;
	controlAneg = 1.0;
	controlBpos = 1.0;
	controlBneg = 1.0;
	targetpos = 1.0;
	targetneg = 1.0;	
	avgA = avgB = 0.0;
	nvgA = nvgB = 0.0;
	//end ButterComp
}
};
