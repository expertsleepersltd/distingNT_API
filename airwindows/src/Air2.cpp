#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Air2"
#define AIRWINDOWS_DESCRIPTION "A different bright EQ with silk tone and high sample rate support."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','i','s' )
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
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Hiss", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Glitter", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Air", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Silk", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		Float64 airPrevA;
		Float64 airEvenA;
		Float64 airOddA;
		Float64 airFactorA;
		bool flipA;
		Float64 airPrevB;
		Float64 airEvenB;
		Float64 airOddB;
		Float64 airFactorB;
		bool flipB;
		Float64 airPrevC;
		Float64 airEvenC;
		Float64 airOddC;
		Float64 airFactorC;
		bool flop;
		Float64 tripletPrev;
		Float64 tripletMid;
		Float64 tripletA;
		Float64 tripletB;
		Float64 tripletC;
		Float64 tripletFactor;
		int count;
		double postsine;
		
		double lastRef[10];
		int cycle;
		
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check	
	
	Float64 hiIntensity = GetParameter( kParam_One );
	Float64 tripletIntensity = GetParameter( kParam_Two );
	Float64 airIntensity = GetParameter( kParam_Three );
	if (hiIntensity < 0.0) hiIntensity *= 0.57525;
	if (tripletIntensity < 0.0) tripletIntensity *= 0.71325;
	if (airIntensity < 0.0) airIntensity *= 0.5712;
	hiIntensity = -pow(hiIntensity,3);
	tripletIntensity = -pow(tripletIntensity,3);
	airIntensity = -pow(airIntensity,3);
	Float64 hiQ = 1.5+fabs(hiIntensity*0.5);
	Float64 tripletQ = 1.5+fabs(tripletIntensity*0.5);
	Float64 airQ = 1.5+fabs(airIntensity*0.5);
	Float64 intensity = (pow(GetParameter( kParam_Four ),3)*4.0) + 0.0001;
	Float64 mix = GetParameter( kParam_Five );
	Float64 drymix = (1.0-mix)*4.0;
	if (drymix > 1.0) drymix = 1.0;
	
	//all types of air band are running in parallel, not series
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do an Air sample
			double correction = 0.0;
			if (fabs(hiIntensity) > 0.0001) {
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
				airOddC = (airOddC - ((airOddC - airEvenC)/256.0)) / hiQ;
				airEvenC = (airEvenC - ((airEvenC - airOddC)/256.0)) / hiQ;
				airPrevC = inputSample;
				correction = correction + airFactorC;
				flop = !flop;
			}//22k
			
			if (fabs(tripletIntensity) > 0.0001) {
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
			
			if (fabs(airIntensity) > 0.0001) {
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
					airOddA = (airOddA - ((airOddA - airEvenA)/256.0)) / airQ;
					airEvenA = (airEvenA - ((airEvenA - airOddA)/256.0)) / airQ;
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
					airOddB = (airOddB - ((airOddB - airEvenB)/256.0)) / airQ;
					airEvenB = (airEvenB - ((airEvenB - airOddB)/256.0)) / airQ;
					airPrevB = inputSample;
					correction = correction + airFactorB;
					flipB = !flipB;
				}
			}//11k
			
			correction *= intensity;
			correction -= 1.0;
			double bridgerectifier = fabs(correction);
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (correction > 0) correction = bridgerectifier;
			else correction = -bridgerectifier;
			correction += postsine;
			correction /= intensity;
			inputSample = correction * 4.0 * mix;
			
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
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[7])*0.5;
				lastRef[7] = lastRef[8]; //continue, do not break
			case 3:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[6])*0.5;
				lastRef[6] = lastRef[8]; //continue, do not break
			case 2:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[5])*0.5;
				lastRef[5] = lastRef[8]; //continue, do not break
			case 1:
				break; //no further averaging
		}
		
		if (drymix < 1.0) drySample *= drymix;
		inputSample += drySample;
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
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
