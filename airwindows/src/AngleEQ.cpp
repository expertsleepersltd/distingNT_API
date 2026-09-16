#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "AngleEQ"
#define AIRWINDOWS_DESCRIPTION "A strange and colorful EQ."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','n','Q' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	kParam_H =7,
	//Add your parameters here...
	kNumberOfParameters=8
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "High", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 400, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HiFreq", .min = 0, .max = 1000, .def = 800, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MHiFreq", .min = 0, .max = 1000, .def = 800, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MLoFreq", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LoFreq", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		uint32_t fpd;
	
	struct _dram {
			float iirHPosition[37];
		float iirHAngle[37];
		float iirMHPosition[37];
		float iirMHAngle[37];
		float iirMLPosition[37];
		float iirMLAngle[37];
		float iirLPosition[37];
		float iirLAngle[37];
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

	float highVol = powf(GetParameter( kParam_A )*2.0f,2.0f);
	float midVol = powf(GetParameter( kParam_B )*2.0f,2.0f);
	float lowVol = powf(GetParameter( kParam_C )*2.0f,2.0f);
	
	float hFreq = powf(GetParameter( kParam_D ),overallscale);
	float mhFreq = powf(GetParameter( kParam_E ),overallscale);
	float mlFreq = powf(GetParameter( kParam_F ),overallscale+3.0f);
	float lFreq = powf(GetParameter( kParam_G ),overallscale+3.0f);
	
	float dry = 2.0f-(GetParameter( kParam_H )*2.0f);
	if (dry > 1.0f) dry = 1.0f; //full dry for use with inv, to 0.0f at full wet
	float wet = (GetParameter( kParam_H )*2.0f)-1.0f; //inv-dry-wet for highpass
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		float midSample = inputSample;
		
		float highSample = inputSample;
		for(int count = 0; count < (3.0f+(hFreq*32.0f)); count++) {
			dram->iirHAngle[count] = (dram->iirHAngle[count]*(1.0f-hFreq))+((inputSample-dram->iirHPosition[count])*hFreq);
			inputSample = ((dram->iirHPosition[count]+(dram->iirHAngle[count]*hFreq))*(1.0f-hFreq))+(inputSample*hFreq);
			dram->iirHPosition[count] = ((dram->iirHPosition[count]+(dram->iirHAngle[count]*hFreq))*(1.0f-hFreq))+(inputSample*hFreq);
			highSample -= (inputSample * (1.0f/(3.0f+(hFreq*32.0f))) );
		} //highpass point of treble band
		
		inputSample = midSample; //restore for second highpass
		for(int count = 0; count < (3.0f+(mlFreq*32.0f)); count++) {
			dram->iirMHAngle[count] = (dram->iirMHAngle[count]*(1.0f-mlFreq))+((inputSample-dram->iirMHPosition[count])*mlFreq);
			inputSample = ((dram->iirMHPosition[count]+(dram->iirMHAngle[count]*mlFreq))*(1.0f-mlFreq))+(inputSample*mlFreq);
			dram->iirMHPosition[count] = ((dram->iirMHPosition[count]+(dram->iirMHAngle[count]*mlFreq))*(1.0f-mlFreq))+(inputSample*mlFreq);
			midSample -= (inputSample * (1.0f/(3.0f+(mlFreq*32.0f))) );
		} //highpass point of mid-to-low band
		
		for(int count = 0; count < (3.0f+(mhFreq*32.0f)); count++) {
			dram->iirMLAngle[count] = (dram->iirMLAngle[count]*(1.0f-mhFreq))+((midSample-dram->iirMLPosition[count])*mhFreq);
			midSample = ((dram->iirMLPosition[count]+(dram->iirMLAngle[count]*mhFreq))*(1.0f-mhFreq))+(midSample*mhFreq);
			dram->iirMLPosition[count] = ((dram->iirMLPosition[count]+(dram->iirMLAngle[count]*mhFreq))*(1.0f-mhFreq))+(midSample*mhFreq);
		} //lowpass point of mid-to-high band
		
		float lowSample = inputSample;
		for(int count = 0; count < (3.0f+(lFreq*32.0f)); count++) {
			dram->iirLAngle[count] = (dram->iirLAngle[count]*(1.0f-lFreq))+((lowSample-dram->iirLPosition[count])*lFreq);
			lowSample = ((dram->iirLPosition[count]+(dram->iirLAngle[count]*lFreq))*(1.0f-lFreq))+(lowSample*lFreq);
			dram->iirLPosition[count] = ((dram->iirLPosition[count]+(dram->iirLAngle[count]*lFreq))*(1.0f-lFreq))+(lowSample*lFreq);
		} //lowpass point of low band
		
		inputSample = (drySample*dry)+(((highSample*highVol)+(midSample*midVol)+(lowSample*lowVol))*wet);
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 36; count++) {
		dram->iirHPosition[count] = 0.0;
		dram->iirHAngle[count] = 0.0;
		dram->iirMHPosition[count] = 0.0;
		dram->iirMHAngle[count] = 0.0;
		dram->iirMLPosition[count] = 0.0;
		dram->iirMLAngle[count] = 0.0;
		dram->iirLPosition[count] = 0.0;
		dram->iirLAngle[count] = 0.0;
	}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
