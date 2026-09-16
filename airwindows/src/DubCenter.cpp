#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DubCenter"
#define AIRWINDOWS_DESCRIPTION "A version of DubSub where the bass reinforcement is purely mono."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','r' )
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
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
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
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	Float32 iirDriveSampleAL;
	Float32 iirDriveSampleBL;
	Float32 iirDriveSampleCL;
	Float32 iirDriveSampleDL;
	Float32 iirDriveSampleEL;
	Float32 iirDriveSampleFL;
	Float32 iirDriveSampleAR;
	Float32 iirDriveSampleBR;
	Float32 iirDriveSampleCR;
	Float32 iirDriveSampleDR;
	Float32 iirDriveSampleER;
	Float32 iirDriveSampleFR;
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
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 driveone = powf(GetParameter( kParam_One )*3.0f,2);
	Float32 driveoutput = GetParameter( kParam_Two );
	Float32 iirAmount = ((GetParameter( kParam_Three )*0.33f)+0.1f)/overallscale;
	Float32 ataLowpassL;
	Float32 ataLowpassR;
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
	Float32 glitch = 0.60f;
	Float32 tempSample;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;

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
		
		oscGate += fabs((inputSampleL + inputSampleR) * 5.0f);
		oscGate -= 0.001f;
		if (oscGate > 1.0f) oscGate = 1.0f;
		if (oscGate < 0) oscGate = 0;
		//got a value that only goes down low when there's silence or near silence on input
		clamp = 1.0f-oscGate;
		clamp *= 0.00001f;
		//set up the thing to choke off oscillations- belt and suspenders affair
		
		
		if (flip)
		{
			tempSample = inputSampleL;
			iirDriveSampleAL = (iirDriveSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL -= iirDriveSampleAL;
			iirDriveSampleCL = (iirDriveSampleCL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL -= iirDriveSampleCL;
			iirDriveSampleEL = (iirDriveSampleEL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL -= iirDriveSampleEL;
			ataLowpassL = tempSample - inputSampleL;

			tempSample = inputSampleR;
			iirDriveSampleAR = (iirDriveSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR -= iirDriveSampleAR;
			iirDriveSampleCR = (iirDriveSampleCR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR -= iirDriveSampleCR;
			iirDriveSampleER = (iirDriveSampleER * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR -= iirDriveSampleER;
			ataLowpassR = tempSample - inputSampleR;
		}
		else
		{
			tempSample = inputSampleL;
			iirDriveSampleBL = (iirDriveSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL -= iirDriveSampleBL;
			iirDriveSampleDL = (iirDriveSampleDL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL -= iirDriveSampleDL;
			iirDriveSampleFL = (iirDriveSampleFL * (1 - iirAmount)) + (inputSampleL * iirAmount);
			inputSampleL -= iirDriveSampleFL;
			ataLowpassL = tempSample - inputSampleL;

			tempSample = inputSampleR;
			iirDriveSampleBR = (iirDriveSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR -= iirDriveSampleBR;
			iirDriveSampleDR = (iirDriveSampleDR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR -= iirDriveSampleDR;
			iirDriveSampleFR = (iirDriveSampleFR * (1 - iirAmount)) + (inputSampleR * iirAmount);
			inputSampleR -= iirDriveSampleFR;
			ataLowpassR = tempSample - inputSampleR;
		}
		//highpass section
		
		if (inputSampleL > 1.0f) {inputSampleL = 1.0f;}
		if (inputSampleL < -1.0f) {inputSampleL = -1.0f;}
		if (inputSampleR > 1.0f) {inputSampleR = 1.0f;}
		if (inputSampleR < -1.0f) {inputSampleR = -1.0f;}
		
		out = driveone;
		while (out > glitch)
		{
			out -= glitch;
			inputSampleL -= (inputSampleL * (fabs(inputSampleL) * glitch) * (fabs(inputSampleL) * glitch) );
			inputSampleL *= (1.0f+glitch);
		}
		//that's taken care of the really high gain stuff
		inputSampleL -= (inputSampleL * (fabs(inputSampleL) * out) * (fabs(inputSampleL) * out) );
		inputSampleL *= (1.0f+out);
		
		
		out = driveone;
		while (out > glitch)
		{
			out -= glitch;
			inputSampleR -= (inputSampleR * (fabs(inputSampleR) * glitch) * (fabs(inputSampleR) * glitch) );
			inputSampleR *= (1.0f+glitch);
		}
		//that's taken care of the really high gain stuff
		inputSampleR -= (inputSampleR * (fabs(inputSampleR) * out) * (fabs(inputSampleR) * out) );
		inputSampleR *= (1.0f+out);
		
		ataLowpass = (ataLowpassL + ataLowpassR) / 2.0f;
		
		if (ataLowpass > 0)
		{if (WasNegative){SubOctave = !SubOctave;} WasNegative = false;}
		else {WasNegative = true;}
		//set up polarities for sub-bass version
		randy = (float(fpdL)/UINT32_MAX)*fuzz; //0 to 1 the noise, may not be needed
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
		
		inputSampleL *= driveoutput; //start with the drive section then add lows and subs
		inputSampleR *= driveoutput; //start with the drive section then add lows and subs
		
		inputSampleL += ((HeadBump + lastHeadBump) * BassOutGain);
		inputSampleL += ((SubBump + lastSubBump) * SubOutGain);
		
		inputSampleR += ((HeadBump + lastHeadBump) * BassOutGain);
		inputSampleR += ((SubBump + lastSubBump) * SubOutGain);
		
		lastHeadBump = HeadBump;
		lastSubBump = SubBump;
		
		if (wet != 1.0f) {
		 inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
		 inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		flip = !flip;
		bflip++;
		if (bflip < 1 || bflip > 3) bflip = 1;
		
		
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//direct stereo out
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	WasNegative = false;
	SubOctave = false;
	flip = false;
	bflip = 0;
	iirDriveSampleAL = 0.0;
	iirDriveSampleBL = 0.0;
	iirDriveSampleCL = 0.0;
	iirDriveSampleDL = 0.0;
	iirDriveSampleEL = 0.0;
	iirDriveSampleFL = 0.0;
	iirDriveSampleAR = 0.0;
	iirDriveSampleBR = 0.0;
	iirDriveSampleCR = 0.0;
	iirDriveSampleDR = 0.0;
	iirDriveSampleER = 0.0;
	iirDriveSampleFR = 0.0;
	
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
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
