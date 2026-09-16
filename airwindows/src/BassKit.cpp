#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BassKit"
#define AIRWINDOWS_DESCRIPTION "Centered bass reinforcement with subs fill. Clean and controllable."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','a','t' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Voicing", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Inv/Out", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sub Inv/Out", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
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
	
	Float32 ataLowpass;
	Float32 randy;
	Float32 invrandy;
	Float32 HeadBump = 0.0f;
	Float32 BassGain = GetParameter( kParam_One ) * 0.1f;
	Float32 HeadBumpFreq = ((GetParameter( kParam_Two )*0.1f)+0.02f)/overallscale;
	Float32 iirAmount = HeadBumpFreq/44.1f;
	Float32 BassOutGain = GetParameter( kParam_Three )*fabs(GetParameter( kParam_Three ));
	Float32 SubBump = 0.0f;
	Float32 SubOutGain = GetParameter( kParam_Four )*fabs(GetParameter( kParam_Four ))*4.0f;
	Float32 clamp = 0.0f;
	Float32 fuzz = 0.111f;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		ataLowpass = (inputSampleL + inputSampleR) / 2.0f;
		iirDriveSampleA = (iirDriveSampleA * (1.0f - HeadBumpFreq)) + (ataLowpass * HeadBumpFreq); ataLowpass = iirDriveSampleA;
		iirDriveSampleB = (iirDriveSampleB * (1.0f - HeadBumpFreq)) + (ataLowpass * HeadBumpFreq); ataLowpass = iirDriveSampleB;

				
		oscGate += fabs(ataLowpass * 10.0f);
		oscGate -= 0.001f;
		if (oscGate > 1.0f) oscGate = 1.0f;
		if (oscGate < 0) oscGate = 0;
		//got a value that only goes down low when there's silence or near silence on input
		clamp = 1.0f-oscGate;
		clamp *= 0.00001f;
		//set up the thing to choke off oscillations- belt and suspenders affair
		
		if (ataLowpass > 0)
		{if (WasNegative){SubOctave = !SubOctave;} WasNegative = false;}
		else {WasNegative = true;}
		//set up polarities for sub-bass version
		randy = (float(fpdL)/UINT32_MAX)*fuzz; //0 to 1 the noise, may not be needed
		invrandy = (1.0f-randy);
		randy /= 2.0f;
		//set up the noise
		
		iirSampleA = (iirSampleA * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleA;
		iirSampleB = (iirSampleB * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleB;
		iirSampleC = (iirSampleC * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleC;
		iirSampleD = (iirSampleD * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleD;
		iirSampleE = (iirSampleE * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleE;
		iirSampleF = (iirSampleF * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleF;
		iirSampleG = (iirSampleG * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleG;
		iirSampleH = (iirSampleH * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleH;
		iirSampleI = (iirSampleI * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleI;
		iirSampleJ = (iirSampleJ * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleJ;
		iirSampleK = (iirSampleK * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleK;
		iirSampleL = (iirSampleL * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleL;
		iirSampleM = (iirSampleM * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleM;
		iirSampleN = (iirSampleN * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleN;
		iirSampleO = (iirSampleO * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleO;
		iirSampleP = (iirSampleP * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleP;
		iirSampleQ = (iirSampleQ * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleQ;
		iirSampleR = (iirSampleR * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleR;
		iirSampleS = (iirSampleS * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleS;
		iirSampleT = (iirSampleT * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleT;
		iirSampleU = (iirSampleU * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleU;
		iirSampleV = (iirSampleV * (1.0f - iirAmount)) + (ataLowpass * iirAmount); ataLowpass -= iirSampleV;
		
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
		
		iirSampleW = (iirSampleW * (1.0f - iirAmount)) + (HeadBump * iirAmount); HeadBump -= iirSampleW;
		iirSampleX = (iirSampleX * (1.0f - iirAmount)) + (HeadBump * iirAmount); HeadBump -= iirSampleX;
		
		SubBump = HeadBump;
		iirSampleY = (iirSampleY * (1.0f - iirAmount)) + (SubBump * iirAmount); SubBump -= iirSampleY;
		
		iirDriveSampleC = (iirDriveSampleC * (1.0f - HeadBumpFreq)) + (SubBump * HeadBumpFreq); SubBump = iirDriveSampleC;
		iirDriveSampleD = (iirDriveSampleD * (1.0f - HeadBumpFreq)) + (SubBump * HeadBumpFreq); SubBump = iirDriveSampleD;
		
		
		SubBump = fabs(SubBump);
		if (SubOctave == false) {SubBump = -SubBump;}
		
		switch (bflip)
		{
			case 1:				
				iirSubBumpA += SubBump;// * BassGain);
				iirSubBumpA -= (iirSubBumpA * iirSubBumpA * iirSubBumpA * HeadBumpFreq);
				iirSubBumpA = (invrandy * iirSubBumpA) + (randy * iirSubBumpB) + (randy * iirSubBumpC);
				if (iirSubBumpA > 0) iirSubBumpA -= clamp;
				if (iirSubBumpA < 0) iirSubBumpA += clamp;
				SubBump = iirSubBumpA;
				break;
			case 2:
				iirSubBumpB += SubBump;// * BassGain);
				iirSubBumpB -= (iirSubBumpB * iirSubBumpB * iirSubBumpB * HeadBumpFreq);
				iirSubBumpB = (randy * iirSubBumpA) + (invrandy * iirSubBumpB) + (randy * iirSubBumpC);
				if (iirSubBumpB > 0) iirSubBumpB -= clamp;
				if (iirSubBumpB < 0) iirSubBumpB += clamp;
				SubBump = iirSubBumpB;
				break;
			case 3:
				iirSubBumpC += SubBump;// * BassGain);
				iirSubBumpC -= (iirSubBumpC * iirSubBumpC * iirSubBumpC * HeadBumpFreq);
				iirSubBumpC = (randy * iirSubBumpA) + (randy * iirSubBumpB) + (invrandy * iirSubBumpC);
				if (iirSubBumpC > 0) iirSubBumpC -= clamp;
				if (iirSubBumpC < 0) iirSubBumpC += clamp;
				SubBump = iirSubBumpC;
				break;
		}
		
		iirSampleZ = (iirSampleZ * (1.0f - HeadBumpFreq)) + (SubBump * HeadBumpFreq); SubBump = iirSampleZ;
		iirDriveSampleE = (iirDriveSampleE * (1.0f - iirAmount)) + (SubBump * iirAmount); SubBump = iirDriveSampleE;
		iirDriveSampleF = (iirDriveSampleF * (1.0f - iirAmount)) + (SubBump * iirAmount); SubBump = iirDriveSampleF;

		
		inputSampleL += (HeadBump * BassOutGain);
		inputSampleL += (SubBump * SubOutGain);
		
		inputSampleR += (HeadBump * BassOutGain);
		inputSampleR += (SubBump * SubOutGain);
				

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
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
