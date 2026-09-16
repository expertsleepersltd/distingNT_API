#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Baxandall2"
#define AIRWINDOWS_DESCRIPTION "Baxandall, extended and made more powerful."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','a','2' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "Treble", .min = -24000, .max = 24000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = -24000, .max = 24000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float trebleA[9];
		float trebleB[9];
		float bassA[9];
		float bassB[9];
		bool flip;
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
	
	float trebleGain = powf(10.0f,GetParameter( kParam_One )/20.0f);
	float trebleFreq = (4410.0f*trebleGain)/GetSampleRate();
	if (trebleFreq > 0.45f) trebleFreq = 0.45f;
	trebleA[0] = trebleB[0] = trebleFreq;
	float bassGain = powf(10.0f,GetParameter( kParam_Two )/20.0f);
	float bassFreq = powf(10.0f,-GetParameter( kParam_Two )/20.0f);
	bassFreq = (8820.0f*bassFreq)/GetSampleRate();
	if (bassFreq > 0.45f) bassFreq = 0.45f;
	bassA[0] = bassB[0] = bassFreq;
    trebleA[1] = trebleB[1] = 0.4f;
    bassA[1] = bassB[1] = 0.2f;
	
	float K = tan(M_PI * trebleA[0]);
	float norm = 1.0f / (1.0f + K / trebleA[1] + K * K);
	trebleB[2] = trebleA[2] = K * K * norm;
	trebleB[3] = trebleA[3] = 2.0f * trebleA[2];
	trebleB[4] = trebleA[4] = trebleA[2];
	trebleB[5] = trebleA[5] = 2.0f * (K * K - 1.0f) * norm;
	trebleB[6] = trebleA[6] = (1.0f - K / trebleA[1] + K * K) * norm;
	
	K = tan(M_PI * bassA[0]);
	norm = 1.0f / (1.0f + K / bassA[1] + K * K);
	bassB[2] = bassA[2] = K * K * norm;
	bassB[3] = bassA[3] = 2.0f * bassA[2];
	bassB[4] = bassA[4] = bassA[2];
	bassB[5] = bassA[5] = 2.0f * (K * K - 1.0f) * norm;
	bassB[6] = bassA[6] = (1.0f - K / bassA[1] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
						
		float trebleSample;
		float bassSample;
		if (flip)
		{
			trebleSample = (inputSample * trebleA[2]) + trebleA[7];
			trebleA[7] = (inputSample * trebleA[3]) - (trebleSample * trebleA[5]) + trebleA[8];
			trebleA[8] = (inputSample * trebleA[4]) - (trebleSample * trebleA[6]);
			trebleSample = inputSample - trebleSample;
			
			bassSample = (inputSample * bassA[2]) + bassA[7];
			bassA[7] = (inputSample * bassA[3]) - (bassSample * bassA[5]) + bassA[8];
			bassA[8] = (inputSample * bassA[4]) - (bassSample * bassA[6]);
		}
		else
		{
			trebleSample = (inputSample * trebleB[2]) + trebleB[7];
			trebleB[7] = (inputSample * trebleB[3]) - (trebleSample * trebleB[5]) + trebleB[8];
			trebleB[8] = (inputSample * trebleB[4]) - (trebleSample * trebleB[6]);
			trebleSample = inputSample - trebleSample;
			
			bassSample = (inputSample * bassB[2]) + bassB[7];
			bassB[7] = (inputSample * bassB[3]) - (bassSample * bassB[5]) + bassB[8];
			bassB[8] = (inputSample * bassB[4]) - (bassSample * bassB[6]);
		}
		flip = !flip;
		
		trebleSample *= trebleGain;
		bassSample *= bassGain;
		inputSample = bassSample + trebleSample; //interleaved biquad
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 9; x++) {
		trebleA[x] = 0.0;
		trebleB[x] = 0.0;
		bassA[x] = 0.0;
		bassB[x] = 0.0;
	}
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
