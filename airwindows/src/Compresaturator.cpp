#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Compresaturator"
#define AIRWINDOWS_DESCRIPTION "Fades between compressing and soft clipping."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','r' )
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
{ .name = "Input Drive", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Clampedness", .min = 0, .max = 10000, .def = 5000, .unit = kNT_unitPercent, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Expansiveness", .min = 50, .max = 5000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		int dCount;
		int lastWidth;
		Float32 padFactor;
		uint32_t fpd;
	
	struct _dram {
			Float32 d[11000];
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

	Float32 inputgain = powf(10.0f,GetParameter( kParam_One )/20.0f);
	Float32 satComp = GetParameter( kParam_Two ) / 50;
	int widestRange = GetParameter( kParam_Three );
	satComp += (((Float32)widestRange/3000.0f)*satComp);
	//set the max wideness of comp zone, minimum range boosted (too much?)
	Float32 output = GetParameter( kParam_Four );
	Float32 wet = GetParameter( kParam_Five );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		float temp = inputSample;

		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		if (dCount < 1 || dCount > 5000) {dCount = 5000;}

		Float32 variSpeed = 1.0f + ((padFactor/lastWidth)*satComp);
		if (variSpeed < 1.0f) variSpeed = 1.0f;
		Float32 totalgain = inputgain / variSpeed;
		if (totalgain != 1.0f) {
			inputSample *= totalgain;
			if (totalgain < 1.0f) {
				temp *= totalgain;
				//no boosting beyond unity please
			}
		}
		
		float bridgerectifier = fabs(inputSample);
		Float32 overspill = 0;
		int targetWidth = widestRange;
		//we now have defaults and an absolute input value to work with
		if (bridgerectifier < 0.01f) padFactor *= 0.9999f;
		//in silences we bring back padFactor if it got out of hand		
		if (bridgerectifier > 1.57079633f) {
			bridgerectifier = 1.57079633f;
			targetWidth = 8;
		}
		//if our output's gone beyond saturating to distorting, we begin chasing the
		//buffer size smaller. Anytime we don't have that, we expand (smoothest sound, only adding to an increasingly subdivided buffer)
		
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) {
			inputSample = bridgerectifier;
			overspill = temp - bridgerectifier;
		}
		
		if (inputSample < 0) {
			inputSample = -bridgerectifier;
			overspill = (-temp) - bridgerectifier;
		} 
		//drive section
		
		dram->d[dCount + 5000] = dram->d[dCount] = overspill * satComp;
		//we now have a big buffer to draw from, which is always positive amount of overspill
		dCount--;
		
		padFactor += dram->d[dCount];
		Float32 randy = (float(fpd)/UINT32_MAX);
		if ((targetWidth*randy) > lastWidth) {
			//we are expanding the buffer so we don't remove this trailing sample
			lastWidth += 1;
		} else {
			padFactor -= dram->d[dCount+lastWidth];
			//zero change, or target is smaller and we are shrinking
			if (targetWidth < lastWidth) {
				lastWidth -= 1;
				if (lastWidth < 2) lastWidth = 2;
				//sanity check as randy can give us target zero
				padFactor -= dram->d[dCount+lastWidth];
			}
		}
		//variable attack/release speed more rapid as comp intensity increases
		//implemented in a way where we're repeatedly not altering the buffer as it expands, which makes the comp artifacts smoother
		if (padFactor < 0) padFactor = 0;
		

		if (output < 1.0f) {
			inputSample *= output;
		}
		
		if (wet < 1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 10990; count++) {dram->d[count] = 0.0;}
	dCount = 0;
	lastWidth = 500;
	padFactor = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
