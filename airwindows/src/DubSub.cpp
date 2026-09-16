#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DubSub"
#define AIRWINDOWS_DESCRIPTION "A fully featured bass doctor."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','b' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	kParam_Seven =6,
	kParam_Eight =7,
	kParam_Nine =8,
	kParam_Ten =9,
	//Add your parameters here...
	kNumberOfParameters=10
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Treble Grind", .min = 0, .max = 1000, .def = 900, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Grind Inv/Out", .min = -1000, .max = 1000, .def = 330, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Crossover", .min = 0, .max = 1000, .def = 740, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Drive", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Voicing", .min = 0, .max = 1000, .def = 950, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Inv/Out", .min = -1000, .max = 1000, .def = 490, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sub Drive", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sub Voicing", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sub Inv/Out", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
		
		Float32 iirDriveSampleA;
		Float32 iirDriveSampleB;
		Float32 iirDriveSampleC;
		Float32 iirDriveSampleD;
		Float32 iirDriveSampleE;
		Float32 iirDriveSampleF;
		bool flip; //drive things

		int bflip;
		bool WasNegative;
		bool SubOctave;
		Float32 iirHeadBumpA;
		Float32 iirHeadBumpB;
		Float32 iirHeadBumpC;
		
		Float32 iirSubBumpA;
		Float32 iirSubBumpB;
		Float32 iirSubBumpC;
		
		Float32 lastHeadBump;
		Float32 lastSubBump;
		
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 iirSampleC;
		Float32 iirSampleD;
		Float32 iirSampleE;
		Float32 iirSampleF;
		Float32 iirSampleG;
		Float32 iirSampleH;
		Float32 iirSampleI;
		Float32 iirSampleJ;
		Float32 iirSampleK;
		Float32 iirSampleL;
		Float32 iirSampleM;
		Float32 iirSampleN;
		Float32 iirSampleO;
		Float32 iirSampleP;
		Float32 iirSampleQ;
		Float32 iirSampleR;
		Float32 iirSampleS;
		Float32 iirSampleT;
		Float32 iirSampleU;
		Float32 iirSampleV;
		Float32 iirSampleW;
		Float32 iirSampleX;
		Float32 iirSampleY;
		Float32 iirSampleZ;		
		Float32 oscGate;
		
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
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 driveone = powf(GetParameter( kParam_One )*3.0f,2);
	Float32 driveoutput = GetParameter( kParam_Two );
	Float32 iirAmount = ((GetParameter( kParam_Three )*0.33f)+0.1f)/overallscale;
	Float32 ataLowpass;
	Float32 randy;
	Float32 invrandy;
	Float32 HeadBump = 0.0f;
	Float32 BassGain = GetParameter( kParam_Four ) * 0.1f;
	Float32 HeadBumpFreq = ((GetParameter( kParam_Five )*0.1f)+0.0001f)/overallscale;
	Float32 iirBmount = HeadBumpFreq/44.1f;
	Float32 altBmount = 1.0f - iirBmount;
	Float32 BassOutGain = GetParameter( kParam_Six );
	Float32 SubBump = 0.0f;
	Float32 SubGain = GetParameter( kParam_Seven ) * 0.1f;
	Float32 SubBumpFreq = ((GetParameter( kParam_Eight )*0.1f)+0.0001f)/overallscale;
	Float32 iirCmount = SubBumpFreq/44.1f;
	Float32 altCmount = 1.0f - iirCmount;
	Float32 SubOutGain = GetParameter( kParam_Nine );
	Float32 clamp = 0.0f;
	Float32 out;
	Float32 fuzz = 0.111f;
	Float32 wet = GetParameter( kParam_Ten );
	Float32 dry = 1.0f-wet;
	Float32 glitch = 0.60f;
	Float32 tempSample;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		// here's the plan.
		// Grind Boost
		// Grind Output Level
		// Bass Split Freq
		// Bass Drive
		// Bass Voicing
		// Bass Output Level
		// Sub Oct Drive
		// Sub Voicing
		// Sub Output Level
		// Dry/Wet
		
		oscGate += fabs(inputSample * 10.0f);
		oscGate -= 0.001f;
		if (oscGate > 1.0f) oscGate = 1.0f;
		if (oscGate < 0) oscGate = 0;
		//got a value that only goes down low when there's silence or near silence on input
		clamp = 1.0f-oscGate;
		clamp *= 0.00001f;
		//set up the thing to choke off oscillations- belt and suspenders affair
		
		if (flip)
		{
			tempSample = inputSample;
			iirDriveSampleA = (iirDriveSampleA * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirDriveSampleA;
			iirDriveSampleC = (iirDriveSampleC * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirDriveSampleC;
			iirDriveSampleE = (iirDriveSampleE * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirDriveSampleE;
			ataLowpass = tempSample - inputSample;
		}
		else
		{
			tempSample = inputSample;
			iirDriveSampleB = (iirDriveSampleB * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirDriveSampleB;
			iirDriveSampleD = (iirDriveSampleD * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirDriveSampleD;
			iirDriveSampleF = (iirDriveSampleF * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirDriveSampleF;
			ataLowpass = tempSample - inputSample;
		}
		//highpass section
		if (inputSample > 1.0f) {inputSample = 1.0f;}
		if (inputSample < -1.0f) {inputSample = -1.0f;}
		out = driveone;
		while (out > glitch)
		{
			out -= glitch;
			inputSample -= (inputSample * (fabs(inputSample) * glitch) * (fabs(inputSample) * glitch) );
			inputSample *= (1.0f+glitch);
		}
		//that's taken care of the really high gain stuff
		
		inputSample -= (inputSample * (fabs(inputSample) * out) * (fabs(inputSample) * out) );
		inputSample *= (1.0f+out);
				
		if (ataLowpass > 0)
		{if (WasNegative){SubOctave = !SubOctave;} WasNegative = false;}
		else {WasNegative = true;}
		//set up polarities for sub-bass version
		randy = (float(fpd)/UINT32_MAX)*fuzz; //0 to 1 the noise, may not be needed
		invrandy = (1.0f-randy);
		randy /= 2.0f;
		//set up the noise

		iirSampleA = (iirSampleA * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleA;
		iirSampleB = (iirSampleB * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleB;
		iirSampleC = (iirSampleC * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleC;
		iirSampleD = (iirSampleD * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleD;
		iirSampleE = (iirSampleE * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleE;
		iirSampleF = (iirSampleF * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleF;
		iirSampleG = (iirSampleG * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleG;
		iirSampleH = (iirSampleH * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleH;
		iirSampleI = (iirSampleI * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleI;
		iirSampleJ = (iirSampleJ * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleJ;
		iirSampleK = (iirSampleK * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleK;
		iirSampleL = (iirSampleL * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleL;
		iirSampleM = (iirSampleM * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleM;
		iirSampleN = (iirSampleN * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleN;
		iirSampleO = (iirSampleO * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleO;
		iirSampleP = (iirSampleP * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleP;
		iirSampleQ = (iirSampleQ * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleQ;
		iirSampleR = (iirSampleR * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleR;
		iirSampleS = (iirSampleS * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleS;
		iirSampleT = (iirSampleT * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleT;
		iirSampleU = (iirSampleU * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleU;
		iirSampleV = (iirSampleV * altBmount) + (ataLowpass * iirBmount); ataLowpass -= iirSampleV;
		
		switch (bflip)
		{
			case 1:				
				iirHeadBumpA += (ataLowpass * BassGain);
				iirHeadBumpA -= (iirHeadBumpA * iirHeadBumpA * iirHeadBumpA * HeadBumpFreq);
				iirHeadBumpA = (invrandy * iirHeadBumpA) + (randy * iirHeadBumpB) + (randy * iirHeadBumpC);
				if (iirHeadBumpA > 0) iirHeadBumpA -= clamp;
				if (iirHeadBumpA < 0) iirHeadBumpA += clamp;
				HeadBump = iirHeadBumpA;
				break;
			case 2:
				iirHeadBumpB += (ataLowpass * BassGain);
				iirHeadBumpB -= (iirHeadBumpB * iirHeadBumpB * iirHeadBumpB * HeadBumpFreq);
				iirHeadBumpB = (randy * iirHeadBumpA) + (invrandy * iirHeadBumpB) + (randy * iirHeadBumpC);
				if (iirHeadBumpB > 0) iirHeadBumpB -= clamp;
				if (iirHeadBumpB < 0) iirHeadBumpB += clamp;
				HeadBump = iirHeadBumpB;
				break;
			case 3:
				iirHeadBumpC += (ataLowpass * BassGain);
				iirHeadBumpC -= (iirHeadBumpC * iirHeadBumpC * iirHeadBumpC * HeadBumpFreq);
				iirHeadBumpC = (randy * iirHeadBumpA) + (randy * iirHeadBumpB) + (invrandy * iirHeadBumpC);
				if (iirHeadBumpC > 0) iirHeadBumpC -= clamp;
				if (iirHeadBumpC < 0) iirHeadBumpC += clamp;
				HeadBump = iirHeadBumpC;
				break;
		}

		iirSampleW = (iirSampleW * altBmount) + (HeadBump * iirBmount); HeadBump -= iirSampleW;
		iirSampleX = (iirSampleX * altBmount) + (HeadBump * iirBmount); HeadBump -= iirSampleX;
		
		SubBump = HeadBump;
		iirSampleY = (iirSampleY * altCmount) + (SubBump * iirCmount); SubBump -= iirSampleY;
		
		SubBump = fabs(SubBump);
		if (SubOctave == false) {SubBump = -SubBump;}
		
		switch (bflip)
		{
			case 1:				
				iirSubBumpA += (SubBump * SubGain);
				iirSubBumpA -= (iirSubBumpA * iirSubBumpA * iirSubBumpA * SubBumpFreq);
				iirSubBumpA = (invrandy * iirSubBumpA) + (randy * iirSubBumpB) + (randy * iirSubBumpC);
				if (iirSubBumpA > 0) iirSubBumpA -= clamp;
				if (iirSubBumpA < 0) iirSubBumpA += clamp;
				SubBump = iirSubBumpA;
				break;
			case 2:
				iirSubBumpB += (SubBump * SubGain);
				iirSubBumpB -= (iirSubBumpB * iirSubBumpB * iirSubBumpB * SubBumpFreq);
				iirSubBumpB = (randy * iirSubBumpA) + (invrandy * iirSubBumpB) + (randy * iirSubBumpC);
				if (iirSubBumpB > 0) iirSubBumpB -= clamp;
				if (iirSubBumpB < 0) iirSubBumpB += clamp;
				SubBump = iirSubBumpB;
				break;
			case 3:
				iirSubBumpC += (SubBump * SubGain);
				iirSubBumpC -= (iirSubBumpC * iirSubBumpC * iirSubBumpC * SubBumpFreq);
				iirSubBumpC = (randy * iirSubBumpA) + (randy * iirSubBumpB) + (invrandy * iirSubBumpC);
				if (iirSubBumpC > 0) iirSubBumpC -= clamp;
				if (iirSubBumpC < 0) iirSubBumpC += clamp;
				SubBump = iirSubBumpC;
				break;
		}
		
		iirSampleZ = (iirSampleZ * altCmount) + (SubBump * iirCmount); SubBump -= iirSampleZ;
		
		inputSample *= driveoutput; //start with the drive section then add lows and subs
		
		inputSample += ((HeadBump + lastHeadBump) * BassOutGain);
		inputSample += ((SubBump + lastSubBump) * SubOutGain);
		
		lastHeadBump = HeadBump;
		lastSubBump = SubBump;
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		flip = !flip;
		bflip++;
		if (bflip < 1 || bflip > 3) bflip = 1;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	WasNegative = false;
	SubOctave = false;
	flip = false;
	bflip = 0;
	iirDriveSampleA = 0.0;
	iirDriveSampleB = 0.0;
	iirDriveSampleC = 0.0;
	iirDriveSampleD = 0.0;
	iirDriveSampleE = 0.0;
	iirDriveSampleF = 0.0;

	iirHeadBumpA = 0.0;
	iirHeadBumpB = 0.0;
	iirHeadBumpC = 0.0;
	
	iirSubBumpA = 0.0;
	iirSubBumpB = 0.0;
	iirSubBumpC = 0.0;
	
	lastHeadBump = 0.0;
	lastSubBump = 0.0;

	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	iirSampleG = 0.0;
	iirSampleH = 0.0;
	iirSampleI = 0.0;
	iirSampleJ = 0.0;
	iirSampleK = 0.0;
	iirSampleL = 0.0;
	iirSampleM = 0.0;
	iirSampleN = 0.0;
	iirSampleO = 0.0;
	iirSampleP = 0.0;
	iirSampleQ = 0.0;
	iirSampleR = 0.0;
	iirSampleS = 0.0;
	iirSampleT = 0.0;
	iirSampleU = 0.0;
	iirSampleV = 0.0;
	iirSampleW = 0.0;
	iirSampleX = 0.0;
	iirSampleY = 0.0;
	iirSampleZ = 0.0;	
	
	oscGate = 1.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
