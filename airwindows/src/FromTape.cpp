#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "FromTape"
#define AIRWINDOWS_DESCRIPTION "A minimalist, cleaner analog tape emulation."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','r','f' )
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
{ .name = "Louder", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Softer", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Weight", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 iirMidRollerA;
		Float32 iirMidRollerB;
		Float32 iirMidRollerC;
		
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
		int flip;
		
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
	Float32 inputgain = GetParameter( kParam_One );
	Float32 SoftenControl = GetParameter( kParam_Two );
	Float32 RollAmount = (1.0f-SoftenControl)/overallscale;
	Float32 iirAmount = (0.004f*(1.0f-GetParameter( kParam_Three )))/overallscale;
	Float32 altAmount = 1.0f - iirAmount;
	Float32 outputgain = GetParameter( kParam_Four );
	Float32 wet = GetParameter( kParam_Five );
	//removed unnecessary dry variable
	Float32 HighsSample = 0.0f;
	Float32 Subtract;
	Float32 bridgerectifier;
	Float32 randy;
	Float32 invrandy;
	Float32 tempSample;
	Float32 drySample;
	
	
	float inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		
		if (inputgain != 1.0f) {
			inputSample *= inputgain;
		}		
		
		randy = (float(fpd)/UINT32_MAX) * SoftenControl; //for soften
		invrandy = (1.0f-randy);
		randy /= 2.0f;
		//we've set up so that we dial in the amount of the alt sections (in pairs) with invrandy being the source section
		
		Subtract = 0.0f;
		tempSample = inputSample;		
		iirSampleA = (iirSampleA * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleA; Subtract += iirSampleA;
		iirSampleB = (iirSampleB * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleB; Subtract += iirSampleB;
		iirSampleC = (iirSampleC * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleC; Subtract += iirSampleC;
		iirSampleD = (iirSampleD * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleD; Subtract += iirSampleD;
		iirSampleE = (iirSampleE * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleE; Subtract += iirSampleE;
		iirSampleF = (iirSampleF * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleF; Subtract += iirSampleF;
		iirSampleG = (iirSampleG * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleG; Subtract += iirSampleG;
		iirSampleH = (iirSampleH * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleH; Subtract += iirSampleH;
		iirSampleI = (iirSampleI * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleI; Subtract += iirSampleI;
		iirSampleJ = (iirSampleJ * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleJ; Subtract += iirSampleJ;
		iirSampleK = (iirSampleK * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleK; Subtract += iirSampleK;
		iirSampleL = (iirSampleL * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleL; Subtract += iirSampleL;
		iirSampleM = (iirSampleM * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleM; Subtract += iirSampleM;
		iirSampleN = (iirSampleN * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleN; Subtract += iirSampleN;
		iirSampleO = (iirSampleO * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleO; Subtract += iirSampleO;
		iirSampleP = (iirSampleP * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleP; Subtract += iirSampleP;
		iirSampleQ = (iirSampleQ * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleQ; Subtract += iirSampleQ;
		iirSampleR = (iirSampleR * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleR; Subtract += iirSampleR;
		iirSampleS = (iirSampleS * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleS; Subtract += iirSampleS;
		iirSampleT = (iirSampleT * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleT; Subtract += iirSampleT;
		iirSampleU = (iirSampleU * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleU; Subtract += iirSampleU;
		iirSampleV = (iirSampleV * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleV; Subtract += iirSampleV;
		iirSampleW = (iirSampleW * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleW; Subtract += iirSampleW;
		iirSampleX = (iirSampleX * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleX; Subtract += iirSampleX;
		iirSampleY = (iirSampleY * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleY; Subtract += iirSampleY;
		iirSampleZ = (iirSampleZ * altAmount) + (tempSample * iirAmount); tempSample -= iirSampleZ; Subtract += iirSampleZ;
		//do the IIR on a dummy sample, and store up the correction in a variable at the same scale as the very low level
		//numbers being used. Don't keep doing it against the possibly high level signal number.
		//This has been known to add a resonant quality to the cutoff, which we're using on purpose.
		inputSample -= Subtract;
		//apply stored up tiny corrections.
		
				
		if (flip < 1 || flip > 3) flip = 1;
		switch (flip)
		{
			case 1:				
				iirMidRollerA = (iirMidRollerA * (1.0f - RollAmount)) + (inputSample * RollAmount);
				iirMidRollerA = (invrandy * iirMidRollerA) + (randy * iirMidRollerB) + (randy * iirMidRollerC);
				HighsSample = inputSample - iirMidRollerA;
				break;
			case 2:
				iirMidRollerB = (iirMidRollerB * (1.0f - RollAmount)) + (inputSample * RollAmount);
				iirMidRollerB = (randy * iirMidRollerA) + (invrandy * iirMidRollerB) + (randy * iirMidRollerC);
				HighsSample = inputSample - iirMidRollerB;
				break;
			case 3:
				iirMidRollerC = (iirMidRollerC * (1.0f - RollAmount)) + (inputSample * RollAmount);
				iirMidRollerC = (randy * iirMidRollerA) + (randy * iirMidRollerB) + (invrandy * iirMidRollerC);
				HighsSample = inputSample - iirMidRollerC;
				break;
		}
		flip++; //increment the triplet counter
		
		Subtract = HighsSample;		
		bridgerectifier = fabs(Subtract)*1.57079633f;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = 1-cos(bridgerectifier);
		if (Subtract > 0) Subtract = bridgerectifier;
		if (Subtract < 0) Subtract = -bridgerectifier;
		inputSample -= Subtract;

		//Soften works using the MidRoller stuff, defining a bright parallel channel that we apply negative Density
		//to, and then subtract from the main audio. That makes the 'highs channel subtract' hit only the loudest
		//transients, plus we are subtracting any artifacts we got from the negative Density.
		
		
		if (outputgain != 1.0f) {
			inputSample *= outputgain;
		}
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirMidRollerA = 0.0;
	iirMidRollerB = 0.0;
	iirMidRollerC = 0.0;
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
	flip = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
