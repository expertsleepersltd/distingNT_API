#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "MackEQ"
#define AIRWINDOWS_DESCRIPTION "The Mackie distortion but with treble and bass controls added."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','a','Q' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "Trim", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Hi", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lo", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gain", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		
		float iirSampleA;
		float iirSampleB;
		float iirSampleC;
		float iirSampleD;
		float iirSampleE;
		float iirSampleF;
		float biquadA[11];
		float biquadB[11];
		float biquadC[11];
		float biquadD[11];
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
	
	float inTrim = GetParameter( kParam_One )*10.0f;
	inTrim *= inTrim;
	
	float gainHigh = powf(GetParameter( kParam_Two ),2)*4.0f;
	float outHigh = sqrt(GetParameter( kParam_Two ));
	float gainBass = powf(GetParameter( kParam_Three ),2)*4.0f;
	float outBass = sqrt(GetParameter( kParam_Three ));
	float outPad = GetParameter( kParam_Four );
	float wet = GetParameter( kParam_Five );
	
	float iirAmountA = 0.001860867f/overallscale;
	float iirAmountB = 0.000287496f/overallscale;
	float iirBassMid = 0.159f/overallscale;
	float iirMidHigh = 0.236f/overallscale;
	
	biquadD[0] = biquadC[0] = biquadB[0] = biquadA[0] = 19160.0f / GetSampleRate();
    biquadA[1] = 0.431684981684982f;
	biquadB[1] = 1.1582298f;
	biquadC[1] = 0.657027382751269f;
	biquadD[1] = 1.076210852946577f;
	
	float K = tan(M_PI * biquadA[0]); //lowpass
	float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
	biquadA[2] = K * K * norm;
	biquadA[3] = 2.0f * biquadA[2];
	biquadA[4] = biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	
	K = tan(M_PI * biquadB[0]);
	norm = 1.0f / (1.0f + K / biquadB[1] + K * K);
	biquadB[2] = K * K * norm;
	biquadB[3] = 2.0f * biquadB[2];
	biquadB[4] = biquadB[2];
	biquadB[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadB[6] = (1.0f - K / biquadB[1] + K * K) * norm;
	
	K = tan(M_PI * biquadC[0]);
	norm = 1.0f / (1.0f + K / biquadC[1] + K * K);
	biquadC[2] = K * K * norm;
	biquadC[3] = 2.0f * biquadC[2];
	biquadC[4] = biquadC[2];
	biquadC[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadC[6] = (1.0f - K / biquadC[1] + K * K) * norm;
	
	K = tan(M_PI * biquadD[0]);
	norm = 1.0f / (1.0f + K / biquadD[1] + K * K);
	biquadD[2] = K * K * norm;
	biquadD[3] = 2.0f * biquadD[2];
	biquadD[4] = biquadD[2];
	biquadD[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadD[6] = (1.0f - K / biquadD[1] + K * K) * norm;
		
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		if (fabs(iirSampleA)<1.18e-37f) iirSampleA = 0.0f;
		iirSampleA = (iirSampleA * (1.0f - iirAmountA)) + (inputSample * iirAmountA);
		inputSample -= iirSampleA;
		
		if (inTrim != 1.0f) inputSample *= inTrim;
		
		//begin Mackity input stage
		float outSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
		biquadA[8] = biquadA[7]; biquadA[7] = inputSample; inputSample = outSample; biquadA[10] = biquadA[9]; biquadA[9] = inputSample; //DF1		
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		inputSample -= powf(inputSample,5)*0.1768f;
		
		outSample = biquadB[2]*inputSample+biquadB[3]*biquadB[7]+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
		biquadB[8] = biquadB[7]; biquadB[7] = inputSample; inputSample = outSample; biquadB[10] = biquadB[9]; biquadB[9] = inputSample; //DF1
		
		if (fabs(iirSampleB)<1.18e-37f) iirSampleB = 0.0f;
		iirSampleB = (iirSampleB * (1.0f - iirAmountB)) + (inputSample * iirAmountB);
		inputSample -= iirSampleB;
		//end Mackity input stage
		
		//begin EQ section
		if (fabs(iirSampleC)<1.18e-37f) iirSampleC = 0.0f;
		iirSampleC = (iirSampleC * (1.0f - iirBassMid)) + (inputSample * iirBassMid);
		
		float bassSample = iirSampleC;
		float midSample = inputSample - bassSample;

		if (gainBass != 1.0f) bassSample *= gainBass;
		if (bassSample > 1.0f) bassSample = 1.0f;
		if (bassSample < -1.0f) bassSample = -1.0f;
		bassSample -= powf(bassSample,5)*0.1768f;
		
		if (fabs(iirSampleD)<1.18e-37f) iirSampleD = 0.0f;
		iirSampleD = (iirSampleD * (1.0f - iirBassMid)) + (bassSample * iirBassMid);
		bassSample = iirSampleD;
		//we've taken the bass sample, made the mids from it, distorted it
		//and hit it with another pole of darkening.
		//mid sample is still normal from undistorted bass
		
		if (fabs(iirSampleE)<1.18e-37f) iirSampleE = 0.0f;
		iirSampleE = (iirSampleE * (1.0f - iirMidHigh)) + (midSample * iirMidHigh);
		float highSample = midSample - iirSampleE;
		midSample = iirSampleE;
		//here is where we make the high sample out of the mid, and take highs
		//away from the mid.
		
		if (fabs(iirSampleF)<1.18e-37f) iirSampleF = 0.0f;
		iirSampleF = (iirSampleF * (1.0f - iirMidHigh)) + (highSample * iirMidHigh);
		highSample -= iirSampleF;
		
		if (gainHigh != 1.0f) highSample *= gainHigh;
		if (highSample > 1.0f) highSample = 1.0f;
		if (highSample < -1.0f) highSample = -1.0f;
		highSample -= powf(highSample,5)*0.1768f;
		//highpassing HighSample another stage, before distorting it
		
		inputSample = ((bassSample*outBass) + midSample + (highSample*outHigh))*4.0f;
		//end EQ section
		
		outSample = biquadC[2]*inputSample+biquadC[3]*biquadC[7]+biquadC[4]*biquadC[8]-biquadC[5]*biquadC[9]-biquadC[6]*biquadC[10];
		biquadC[8] = biquadC[7]; biquadC[7] = inputSample; inputSample = outSample; biquadC[10] = biquadC[9]; biquadC[9] = inputSample; //DF1
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		inputSample -= powf(inputSample,5)*0.1768f;
		
		outSample = biquadD[2]*inputSample+biquadD[3]*biquadD[7]+biquadD[4]*biquadD[8]-biquadD[5]*biquadD[9]-biquadD[6]*biquadD[10];
		biquadD[8] = biquadD[7]; biquadD[7] = inputSample; inputSample = outSample; biquadD[10] = biquadD[9]; biquadD[9] = inputSample; //DF1
				
		if (outPad != 1.0f) inputSample *= outPad;

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
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	for (int x = 0; x < 11; x++) {biquadA[x] = 0.0; biquadB[x] = 0.0; biquadC[x] = 0.0; biquadD[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
