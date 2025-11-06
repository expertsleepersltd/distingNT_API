#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Cabs2"
#define AIRWINDOWS_DESCRIPTION "Uses really phasey filters to be a speaker cabinet simulator."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','a','c' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
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
{ .name = "Lowpass", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Hipass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		bool hBypass;
		float iirLPosition[15];
		float iirLAngle[15];
		bool lBypass;
		
		uint32_t fpd;
	
	struct _dram {
			float iirHPosition[23];
		float iirHAngle[23];
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
	
	float lFreq = powf(fmax(GetParameter( kParam_A ),0.002f),overallscale); //the lowpass
	float hFreq = powf(GetParameter( kParam_B ),overallscale+2.0f); //the highpass
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (hFreq > 0.0f) {
			float lowSample = inputSample;
			for(int count = 0; count < 21; count++) {
				dram->iirHAngle[count] = (dram->iirHAngle[count]*(1.0f-hFreq))+((lowSample-dram->iirHPosition[count])*hFreq);
				lowSample = ((dram->iirHPosition[count]+(dram->iirHAngle[count]*hFreq))*(1.0f-hFreq))+(lowSample*hFreq);
				dram->iirHPosition[count] = ((dram->iirHPosition[count]+(dram->iirHAngle[count]*hFreq))*(1.0f-hFreq))+(lowSample*hFreq);
				inputSample -= (lowSample * (1.0f/21.0f));
			} //the highpass
			hBypass = false;
		} else {
			if (!hBypass) {
				hBypass = true;
				for(int count = 0; count < 22; count++) {
					dram->iirHPosition[count] = 0.0f;
					dram->iirHAngle[count] = 0.0f;
				}
			} //blank out highpass if jut switched off
		}
		
		if (lFreq < 1.0f) {
			for(int count = 0; count < 13; count++) {
				iirLAngle[count] = (iirLAngle[count]*(1.0f-lFreq))+((inputSample-iirLPosition[count])*lFreq);
				inputSample = ((iirLPosition[count]+(iirLAngle[count]*lFreq))*(1.0f-lFreq))+(inputSample*lFreq);
				iirLPosition[count] = ((iirLPosition[count]+(iirLAngle[count]*lFreq))*(1.0f-lFreq))+(inputSample*lFreq);
			} //the lowpass
			lBypass = false;
		} else {
			if (!lBypass) {
				lBypass = true;
				for(int count = 0; count < 14; count++) {
					iirLPosition[count] = 0.0f;
					iirLAngle[count] = 0.0f;
				}
			} //blank out lowpass if just switched off
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 22; count++) {
		dram->iirHPosition[count] = 0.0;
		dram->iirHAngle[count] = 0.0;
	}
	hBypass = false;

	for(int count = 0; count < 14; count++) {
		iirLPosition[count] = 0.0;
		iirLAngle[count] = 0.0;
	}
	lBypass = false;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
