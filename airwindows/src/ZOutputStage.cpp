#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ZOutputStage"
#define AIRWINDOWS_DESCRIPTION "The output clipping from the Emu e6400 style Z filters."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','Z','O','e' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
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
{ .name = "Input", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float biquadE[11];
		float biquadF[11];
		float iirSampleA;
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
		
	//opamp stuff
	float inTrim = GetParameter( kParam_One )*10.0f;
	inTrim *= inTrim; inTrim *= inTrim;
	float outPad = GetParameter( kParam_Two );
	float iirAmountA = 0.00069f/overallscale;
	biquadF[0] = biquadE[0] = 15500.0f / GetSampleRate();
    biquadF[1] = biquadE[1] = 0.935f;
	float K = tan(M_PI * biquadE[0]); //lowpass
	float norm = 1.0f / (1.0f + K / biquadE[1] + K * K);
	biquadE[2] = K * K * norm;
	biquadE[3] = 2.0f * biquadE[2];
	biquadE[4] = biquadE[2];
	biquadE[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadE[6] = (1.0f - K / biquadE[1] + K * K) * norm;
	for (int x = 0; x < 7; x++) biquadF[x] = biquadE[x];
	//end opamp stuff	
		
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (inTrim != 1.0f) inputSample *= inTrim;

		//opamp stage
		if (fabs(iirSampleA)<1.18e-37f) iirSampleA = 0.0f;
		iirSampleA = (iirSampleA * (1.0f - iirAmountA)) + (inputSample * iirAmountA);
		inputSample -= iirSampleA;
		
		float outSample = biquadE[2]*inputSample+biquadE[3]*biquadE[7]+biquadE[4]*biquadE[8]-biquadE[5]*biquadE[9]-biquadE[6]*biquadE[10];
		biquadE[8] = biquadE[7]; biquadE[7] = inputSample; inputSample = outSample; biquadE[10] = biquadE[9]; biquadE[9] = inputSample; //DF1		
		
		if (inputSample > 1.0f) inputSample = 1.0f; if (inputSample < -1.0f) inputSample = -1.0f;
		inputSample -= (inputSample*inputSample*inputSample*inputSample*inputSample*0.1768f);
		
		outSample = biquadF[2]*inputSample+biquadF[3]*biquadF[7]+biquadF[4]*biquadF[8]-biquadF[5]*biquadF[9]-biquadF[6]*biquadF[10];
		biquadF[8] = biquadF[7]; biquadF[7] = inputSample; inputSample = outSample; biquadF[10] = biquadF[9]; biquadF[9] = inputSample; //DF1
		
		if (outPad != 1.0f) inputSample *= outPad;		
		//end opamp stage
				
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 11; x++) {biquadE[x] = 0.0; biquadF[x] = 0.0;}
	iirSampleA = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
