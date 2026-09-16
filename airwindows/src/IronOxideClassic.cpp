#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "IronOxideClassic"
#define AIRWINDOWS_DESCRIPTION "The purer, simpler, early form of Iron Oxide before all the features."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','r','c' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
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
{ .name = "Input Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tape Speed", .min = 150, .max = 15000, .def = 1500, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Output Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 fastIIRA;
		Float32 fastIIRB;
		Float32 slowIIRA;
		Float32 slowIIRB;
		SInt32 gcount;
		
		uint32_t fpd;
		bool fpFlip;
	
	struct _dram {
			Float32 d[264];
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
	Float32 outputgain = powf(10.0f,GetParameter( kParam_Three )/20.0f);
	Float32 ips = GetParameter( kParam_Two ) * 1.1f;
	//slight correction to dial in convincing ips settings
	if (ips < 1 || ips > 200){ips=33.0f;}
	//sanity checks are always key
	Float32 iirAmount = ips/430.0f; //for low leaning
	Float32 bridgerectifier;
	Float32 fastTaper = ips/15.0f;
	Float32 slowTaper = 2.0f/(ips*ips);
	Float32 lowspeedscale = (5.0f/ips);
	float inputSample;
	SInt32 count;
	Float32 temp;
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	if (overallscale == 0) {fastTaper += 1.0f; slowTaper += 1.0f;}
	else
	{
		iirAmount /= overallscale;
		lowspeedscale *= overallscale;
		fastTaper = 1.0f + (fastTaper / overallscale);
		slowTaper = 1.0f + (slowTaper / overallscale);
	}
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
				
		if (fpFlip)
		{
			iirSampleA = (iirSampleA * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirSampleA;
		}
		else
		{
			iirSampleB = (iirSampleB * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirSampleB;
		}
		//do IIR highpass for leaning out
		
		if (inputgain != 1.0f) inputSample *= inputgain;
		
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0.0f) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;		
		
		if (gcount < 0 || gcount > 131) {gcount = 131;}
		count = gcount;
		//increment the counter
		
		dram->d[count+131] = dram->d[count] = inputSample;
		if (fpFlip)
		{
			fastIIRA = fastIIRA/fastTaper;
			slowIIRA = slowIIRA/slowTaper;
			//scale stuff down
			fastIIRA += dram->d[count];
			count += 3;
			temp = dram->d[count+127];
			temp += dram->d[count+113];
			temp += dram->d[count+109];
			temp += dram->d[count+107];
			temp += dram->d[count+103];
			temp += dram->d[count+101];
			temp += dram->d[count+97];
			temp += dram->d[count+89];
			temp += dram->d[count+83];
			temp /= 2;
			temp += dram->d[count+79];
			temp += dram->d[count+73];
			temp += dram->d[count+71];
			temp += dram->d[count+67];
			temp += dram->d[count+61];
			temp += dram->d[count+59];
			temp += dram->d[count+53];
			temp += dram->d[count+47];
			temp += dram->d[count+43];
			temp += dram->d[count+41];
			temp += dram->d[count+37];
			temp += dram->d[count+31];
			temp += dram->d[count+29];
			temp /= 2;
			temp += dram->d[count+23];
			temp += dram->d[count+19];
			temp += dram->d[count+17];
			temp += dram->d[count+13];
			temp += dram->d[count+11];
			temp /= 2;
			temp += dram->d[count+7];
			temp += dram->d[count+5];
			temp += dram->d[count+3];
			temp /= 2;
			temp += dram->d[count+2];
			temp += dram->d[count+1];
			slowIIRA += (temp/128);
			inputSample = fastIIRA - (slowIIRA / slowTaper);
		}
		else
		{
			fastIIRB = fastIIRB/fastTaper;
			slowIIRB = slowIIRB/slowTaper;
			//scale stuff down
			fastIIRB += dram->d[count];
			count += 3;
			temp = dram->d[count+127];
			temp += dram->d[count+113];
			temp += dram->d[count+109];
			temp += dram->d[count+107];
			temp += dram->d[count+103];
			temp += dram->d[count+101];
			temp += dram->d[count+97];
			temp += dram->d[count+89];
			temp += dram->d[count+83];
			temp /= 2;
			temp += dram->d[count+79];
			temp += dram->d[count+73];
			temp += dram->d[count+71];
			temp += dram->d[count+67];
			temp += dram->d[count+61];
			temp += dram->d[count+59];
			temp += dram->d[count+53];
			temp += dram->d[count+47];
			temp += dram->d[count+43];
			temp += dram->d[count+41];
			temp += dram->d[count+37];
			temp += dram->d[count+31];
			temp += dram->d[count+29];
			temp /= 2;
			temp += dram->d[count+23];
			temp += dram->d[count+19];
			temp += dram->d[count+17];
			temp += dram->d[count+13];
			temp += dram->d[count+11];
			temp /= 2;
			temp += dram->d[count+7];
			temp += dram->d[count+5];
			temp += dram->d[count+3];
			temp /= 2;
			temp += dram->d[count+2];
			temp += dram->d[count+1];
			slowIIRB += (temp/128);
			inputSample = fastIIRB - (slowIIRB / slowTaper);
		}
		inputSample /= fastTaper;
		inputSample /= lowspeedscale;
		
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		//can use as an output limiter
		if (inputSample > 0.0f) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//second stage of overdrive to prevent overs and allow bloody loud extremeness
		
		if (outputgain != 1.0f) inputSample *= outputgain;
		fpFlip = !fpFlip;
		
		
		
		*destP = inputSample;

		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int temp = 0; temp < 263; temp++) {dram->d[temp] = 0.0;}
	gcount = 0;
	fastIIRA = fastIIRB = slowIIRA = slowIIRB = 0.0;
	iirSampleA = iirSampleB = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = true;
}
};
