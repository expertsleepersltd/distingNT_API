#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Air2"
#define AIRWINDOWS_DESCRIPTION "A different bright EQ with silk tone and high sample rate support."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','i','2' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Hiss", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Glitter", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Air", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Silk", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 airPrevA;
		Float32 airEvenA;
		Float32 airOddA;
		Float32 airFactorA;
		bool flipA;
		Float32 airPrevB;
		Float32 airEvenB;
		Float32 airOddB;
		Float32 airFactorB;
		bool flipB;
		Float32 airPrevC;
		Float32 airEvenC;
		Float32 airOddC;
		Float32 airFactorC;
		bool flop;
		Float32 tripletPrev;
		Float32 tripletMid;
		Float32 tripletA;
		Float32 tripletB;
		Float32 tripletC;
		Float32 tripletFactor;
		int count;
		float postsine;
		
		float lastRef[10];
		int cycle;
		
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
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check	
	
	Float32 hiIntensity = GetParameter( kParam_One );
	Float32 tripletIntensity = GetParameter( kParam_Two );
	Float32 airIntensity = GetParameter( kParam_Three );
	if (hiIntensity < 0.0f) hiIntensity *= 0.57525f;
	if (tripletIntensity < 0.0f) tripletIntensity *= 0.71325f;
	if (airIntensity < 0.0f) airIntensity *= 0.5712f;
	hiIntensity = -powf(hiIntensity,3);
	tripletIntensity = -powf(tripletIntensity,3);
	airIntensity = -powf(airIntensity,3);
	Float32 hiQ = 1.5f+fabs(hiIntensity*0.5f);
	Float32 tripletQ = 1.5f+fabs(tripletIntensity*0.5f);
	Float32 airQ = 1.5f+fabs(airIntensity*0.5f);
	Float32 intensity = (powf(GetParameter( kParam_Four ),3)*4.0f) + 0.0001f;
	Float32 mix = GetParameter( kParam_Five );
	Float32 drymix = (1.0f-mix)*4.0f;
	if (drymix > 1.0f) drymix = 1.0f;
	
	//all types of air band are running in parallel, not series
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do an Air sample
			float correction = 0.0f;
			if (fabs(hiIntensity) > 0.0001f) {
				airFactorC = airPrevC - inputSample;
				if (flop)
				{
					airEvenC += airFactorC; airOddC -= airFactorC;
					airFactorC = airEvenC * hiIntensity;
				}
				else
				{
					airOddC += airFactorC; airEvenC -= airFactorC;
					airFactorC = airOddC * hiIntensity;
				}
				airOddC = (airOddC - ((airOddC - airEvenC)/256.0f)) / hiQ;
				airEvenC = (airEvenC - ((airEvenC - airOddC)/256.0f)) / hiQ;
				airPrevC = inputSample;
				correction = correction + airFactorC;
				flop = !flop;
			}//22k
			
			if (fabs(tripletIntensity) > 0.0001f) {
				tripletFactor = tripletPrev - inputSample;
				if (count < 1 || count > 3) count = 1;
				switch (count)
				{
					case 1:
						tripletA += tripletFactor; tripletC -= tripletFactor;
						tripletFactor = tripletA * tripletIntensity;
						tripletPrev = tripletMid; tripletMid = inputSample;
						break;
					case 2:
						tripletB += tripletFactor; tripletA -= tripletFactor;
						tripletFactor = tripletB * tripletIntensity;
						tripletPrev = tripletMid; tripletMid = inputSample;
						break;
					case 3:
						tripletC += tripletFactor; tripletB -= tripletFactor;
						tripletFactor = tripletC * tripletIntensity;
						tripletPrev = tripletMid; tripletMid = inputSample;
						break;
				}
				tripletA /= tripletQ; tripletB /= tripletQ; tripletC /= tripletQ;
				correction = correction + tripletFactor;
				count++;
			}//15K
			
			if (fabs(airIntensity) > 0.0001f) {
				if (flop)
				{
					airFactorA = airPrevA - inputSample;
					if (flipA)
					{
						airEvenA += airFactorA; airOddA -= airFactorA;
						airFactorA = airEvenA * airIntensity;
					}
					else
					{
						airOddA += airFactorA; airEvenA -= airFactorA;
						airFactorA = airOddA * airIntensity;
					}
					airOddA = (airOddA - ((airOddA - airEvenA)/256.0f)) / airQ;
					airEvenA = (airEvenA - ((airEvenA - airOddA)/256.0f)) / airQ;
					airPrevA = inputSample;
					correction = correction + airFactorA;
					flipA = !flipA;
				}
				else
				{
					airFactorB = airPrevB - inputSample;
					if (flipB)
					{
						airEvenB += airFactorB; airOddB -= airFactorB;
						airFactorB = airEvenB * airIntensity;
					}
					else
					{
						airOddB += airFactorB; airEvenB -= airFactorB;
						airFactorB = airOddB * airIntensity;
					}
					airOddB = (airOddB - ((airOddB - airEvenB)/256.0f)) / airQ;
					airEvenB = (airEvenB - ((airEvenB - airOddB)/256.0f)) / airQ;
					airPrevB = inputSample;
					correction = correction + airFactorB;
					flipB = !flipB;
				}
			}//11k
			
			correction *= intensity;
			correction -= 1.0f;
			float bridgerectifier = fabs(correction);
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			bridgerectifier = sin(bridgerectifier);
			if (correction > 0) correction = bridgerectifier;
			else correction = -bridgerectifier;
			correction += postsine;
			correction /= intensity;
			inputSample = correction * 4.0f * mix;
			
			if (cycleEnd == 4) {
				lastRef[0] = lastRef[4]; //start from previous last
				lastRef[2] = (lastRef[0] + inputSample)/2; //half
				lastRef[1] = (lastRef[0] + lastRef[2])/2; //one quarter
				lastRef[3] = (lastRef[2] + inputSample)/2; //three quarters
				lastRef[4] = inputSample; //full
			}
			if (cycleEnd == 3) {
				lastRef[0] = lastRef[3]; //start from previous last
				lastRef[2] = (lastRef[0]+lastRef[0]+inputSample)/3; //third
				lastRef[1] = (lastRef[0]+inputSample+inputSample)/3; //two thirds
				lastRef[3] = inputSample; //full
			}
			if (cycleEnd == 2) {
				lastRef[0] = lastRef[2]; //start from previous last
				lastRef[1] = (lastRef[0] + inputSample)/2; //half
				lastRef[2] = inputSample; //full
			}
			if (cycleEnd == 1) lastRef[0] = inputSample;
			cycle = 0; //reset
			inputSample = lastRef[cycle];
		} else {
			inputSample = lastRef[cycle];
			//we are going through our references now
		}
		switch (cycleEnd) //multi-pole average using lastRef[] variables
		{
			case 4:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[7])*0.5f;
				lastRef[7] = lastRef[8]; //continue, do not break
			case 3:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[6])*0.5f;
				lastRef[6] = lastRef[8]; //continue, do not break
			case 2:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[5])*0.5f;
				lastRef[5] = lastRef[8]; //continue, do not break
			case 1:
				break; //no further averaging
		}
		
		if (drymix < 1.0f) drySample *= drymix;
		inputSample += drySample;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	airPrevA = 0.0;
	airEvenA = 0.0;
	airOddA = 0.0;
	airFactorA = 0.0;
	flipA = false;
	airPrevB = 0.0;
	airEvenB = 0.0;
	airOddB = 0.0;
	airFactorB = 0.0;
	flipB = false;
	airPrevC = 0.0;
	airEvenC = 0.0;
	airOddC = 0.0;
	airFactorC = 0.0;
	flop = false;
	tripletPrev = 0.0;
	tripletMid = 0.0;
	tripletA = 0.0;
	tripletB = 0.0;
	tripletC = 0.0;
	tripletFactor = 0.0;
	count = 1;
	postsine = sin(1.0);
	
	for(int c = 0; c < 9; c++) {lastRef[c] = 0.0;}
	cycle = 0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
