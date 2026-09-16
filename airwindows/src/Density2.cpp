#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Density2"
#define AIRWINDOWS_DESCRIPTION "A different color for Density, some old code I had that people wanted."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','6' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Density", .min = -1000, .max = 4000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Highpass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Level", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float last3Sample;
		float last2Sample;
		float last1Sample;
		float ataA;
		float ataB;
		float ataC;
		float lastDiffSample;
		float iirSampleA;
		float iirSampleB;
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
	Float32 density = GetParameter( kParam_One );
	Float32 out = fabs(density);
	while (out > 1.0f) out = out - 1.0f;
	density = density * fabs(density);
	Float32 iirAmount = powf(GetParameter( kParam_Two ),3)/overallscale;
	Float32 output = GetParameter( kParam_Three );
	Float32 wet = GetParameter( kParam_Four );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		float halfwaySample = (inputSample + last1Sample + ((-last2Sample + last3Sample) * 0.0414213562373095048801688f)) / 2.0f;
		float halfDrySample = halfwaySample;
		
		last3Sample = last2Sample; last2Sample = last1Sample; last1Sample = inputSample;

		iirSampleB = (iirSampleB * (1.0f - iirAmount)) + (halfwaySample * iirAmount); halfwaySample -= iirSampleB; //highpass section
		
		float count = density;
		float bridgerectifier;
		while (count > 1.0f) {
			bridgerectifier = fabs(halfwaySample)*1.57079633f;
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			bridgerectifier = sin(bridgerectifier);
			if (halfwaySample > 0.0f) halfwaySample = bridgerectifier;
			else halfwaySample = -bridgerectifier;
			count = count - 1.0f;
		}
		bridgerectifier = fabs(halfwaySample)*1.57079633f;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		if (density > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier); //produce either boosted or starved version
		if (halfwaySample > 0) halfwaySample = (halfwaySample*(1.0f-out))+(bridgerectifier*out);
		else halfwaySample = (halfwaySample*(1.0f-out))-(bridgerectifier*out); //blend according to density control
		
		ataC = halfwaySample - halfDrySample;
		ataA *= 0.915965594177219015f; ataB *= 0.915965594177219015f;
		ataB += ataC; ataA -= ataC; ataC = ataB;
		float halfDiffSample = ataC * 0.915965594177219015f;
		
		iirSampleA = (iirSampleA * (1.0f - iirAmount)) + (inputSample * iirAmount); inputSample -= iirSampleA; //highpass section
		
		count = density;
		while (count > 1.0f) {
			bridgerectifier = fabs(inputSample)*1.57079633f;
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			bridgerectifier = sin(bridgerectifier);
			if (inputSample > 0.0f) inputSample = bridgerectifier;
			else inputSample = -bridgerectifier;
			count = count - 1.0f;
		}
		bridgerectifier = fabs(inputSample)*1.57079633f;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		if (density > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier); //produce either boosted or starved version
		if (inputSample > 0) inputSample = (inputSample*(1-out))+(bridgerectifier*out);
		else inputSample = (inputSample*(1-out))-(bridgerectifier*out); //blend according to density control
		
		ataC = inputSample - drySample;
		ataA *= 0.915965594177219015f; ataB *= 0.915965594177219015f;
		ataA += ataC; ataB -= ataC; ataC = ataA;
		float diffSample = ataC * 0.915965594177219015f; 
		
		inputSample = drySample + ((diffSample + halfDiffSample + lastDiffSample) / 1.187f);
		lastDiffSample = diffSample / 2.0f;
		
		inputSample *= output;
		
		inputSample = (drySample*(1.0f-wet))+(inputSample*wet);
				
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	ataA = ataB = ataC = lastDiffSample = 0.0;
	iirSampleA = iirSampleB = last3Sample = last2Sample = last1Sample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
