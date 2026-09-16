#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Focus"
#define AIRWINDOWS_DESCRIPTION "Brings out clarity by distorting. Aggressive, subtle, flexible."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','o','s' )
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
static const int kDENSITY = 0;
static const int kDRIVE = 1;
static const int kSPIRAL = 2;
static const int kMOJO = 3;
static const int kDYNO = 4;
static const int kDefaultValue_ParamThree = kSPIRAL;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static char const * const enumStrings2[] = { "Density", "Drive", "Spiral", "Mojo", "Dyno", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Boost", .min = 0, .max = 12000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Focus", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mode", .min = 0, .max = 4, .def = 2, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings2 },
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

		float figure[9];
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

	//[0] is frequency: 0.000001f to 0.499999f is near-zero to near-Nyquist
	//[1] is resonance, 0.7071f is Butterworth. Also can't be zero
	Float32 boost = powf(10.0f,GetParameter( kParam_One )/20.0f);
	figure[0] = 3515.775f/GetSampleRate(); //fixed frequency, 3.515775k
	figure[1] = powf(powf(GetParameter( kParam_Two ),3)*2,2)+0.0001f; //resonance
	int mode = (int) GetParameter( kParam_Three );
	Float32 output = GetParameter( kParam_Four );
	Float32 wet = GetParameter( kParam_Five );
	
	
	float K = tan(M_PI * figure[0]);
	float norm = 1.0f / (1.0f + K / figure[1] + K * K);
	figure[2] = K / figure[1] * norm;
	figure[4] = -figure[2];
	figure[5] = 2.0f * (K * K - 1.0f) * norm;
	figure[6] = (1.0f - K / figure[1] + K * K) * norm;

	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		
		inputSample = sin(inputSample);
		//encode Console5: good cleanness
		
		float tempSample = (inputSample * figure[2]) + figure[7];
		figure[7] = -(tempSample * figure[5]) + figure[8];
		figure[8] = (inputSample * figure[4]) - (tempSample * figure[6]);
		inputSample = tempSample;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSample = asin(inputSample);
		//decode Console5
		
		float groundSample = drySample - inputSample; //set up UnBox
		inputSample *= boost; //now, focussed area gets cranked before distort
		
		switch (mode)
		{
			case 0: //Density
				if (inputSample > 1.570796326794897f) inputSample = 1.570796326794897f;
				if (inputSample < -1.570796326794897f) inputSample = -1.570796326794897f;
				//clip to 1.570796326794897f to reach maximum output
				inputSample = sin(inputSample);
				break;
			case 1: //Drive				
				if (inputSample > 1.0f) inputSample = 1.0f;
				if (inputSample < -1.0f) inputSample = -1.0f;
				inputSample -= (inputSample * (fabs(inputSample) * 0.6f) * (fabs(inputSample) * 0.6f));
				inputSample *= 1.6f;
				break;
			case 2: //Spiral
				if (inputSample > 1.2533141373155f) inputSample = 1.2533141373155f;
				if (inputSample < -1.2533141373155f) inputSample = -1.2533141373155f;
				//clip to 1.2533141373155f to reach maximum output
				inputSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0f) ?1:fabs(inputSample));
				break;
			case 3: //Mojo
				float mojo; mojo = powf(fabs(inputSample),0.25f);
				if (mojo > 0.0f) inputSample = (sin(inputSample * mojo * M_PI * 0.5f) / mojo) * 0.987654321f;
				//mojo is the one that flattens WAAAAY out very softly before wavefolding				
				break;
			case 4: //Dyno
				float dyno; dyno = powf(fabs(inputSample),4);
				if (dyno > 0.0f) inputSample = (sin(inputSample * dyno) / dyno) * 1.1654321f;
				//dyno is the one that tries to raise peak energy				
				break;
		}				
		
		if (output != 1.0f) {
			inputSample *= output;
		}
		
		inputSample += groundSample; //effectively UnBox
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 9; x++) {figure[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
