#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Deckwrecka"
#define AIRWINDOWS_DESCRIPTION "Fattens and dirties up beats."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','a' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Wreck", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
	
		int bflip;
		Float32 iirHeadBumpA;
		Float32 iirHeadBumpB;
		Float32 iirHeadBumpC;
				
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
	
	Float32 wreck = powf(GetParameter( kParam_One ),2);
	Float32 randy;
	Float32 HeadBump = 0.0f;
	Float32 HeadBumpFreq = 0.01f/overallscale;
	Float32 iirHPAmount = 0.0000014f/overallscale;
	Float32 altHPAmount = 1.0f - iirHPAmount;
	Float32 inputSample;
	Float32 tempSample;
	Float32 bridgerectifier;
	Float32 correction;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		randy = (float(fpd)/UINT32_MAX);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		randy += (float(fpd)/UINT32_MAX);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		randy += (float(fpd)/UINT32_MAX);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		randy += (float(fpd)/UINT32_MAX);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		randy += (float(fpd)/UINT32_MAX);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		randy += (float(fpd)/UINT32_MAX);
		randy /= 6.0f;
		randy *= wreck; //0 to 1 the noise, may not be needed
		//set up the noise
		
		switch (bflip)
		{
			case 1:				
				iirHeadBumpA += (inputSample * randy);
				iirHeadBumpA -= (iirHeadBumpA * iirHeadBumpA * iirHeadBumpA * HeadBumpFreq);
				break;
			case 2:
				iirHeadBumpB += (inputSample * randy);
				iirHeadBumpB -= (iirHeadBumpB * iirHeadBumpB * iirHeadBumpB * HeadBumpFreq);
				break;
			case 3:
				iirHeadBumpC += (inputSample * randy);
				iirHeadBumpC -= (iirHeadBumpC * iirHeadBumpC * iirHeadBumpC * HeadBumpFreq);
				break;
		}

		HeadBump = iirHeadBumpA + iirHeadBumpB + iirHeadBumpC;
		
		bflip++;
		if (bflip < 1 || bflip > 3) bflip = 1;
		
		tempSample = HeadBump;
		
		iirSampleA = (iirSampleA * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleA; correction = iirSampleA;
		iirSampleB = (iirSampleB * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleB; correction += iirSampleB;
		iirSampleC = (iirSampleC * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleC; correction += iirSampleC;
		iirSampleD = (iirSampleD * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleD; correction += iirSampleD;
		iirSampleE = (iirSampleE * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleE; correction += iirSampleE;
		iirSampleF = (iirSampleF * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleF; correction += iirSampleF;
		iirSampleG = (iirSampleG * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleG; correction += iirSampleG;
		iirSampleH = (iirSampleH * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleH; correction += iirSampleH;
		iirSampleI = (iirSampleI * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleI; correction += iirSampleI;
		iirSampleJ = (iirSampleJ * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleJ; correction += iirSampleJ;
		iirSampleK = (iirSampleK * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleK; correction += iirSampleK;
		iirSampleL = (iirSampleL * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleL; correction += iirSampleL;
		iirSampleM = (iirSampleM * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleM; correction += iirSampleM;
		iirSampleN = (iirSampleN * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleN; correction += iirSampleN;
		iirSampleO = (iirSampleO * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleO; correction += iirSampleO;
		iirSampleP = (iirSampleP * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleP; correction += iirSampleP;
		iirSampleQ = (iirSampleQ * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleQ; correction += iirSampleQ;
		iirSampleR = (iirSampleR * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleR; correction += iirSampleR;
		iirSampleS = (iirSampleS * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleS; correction += iirSampleS;
		iirSampleT = (iirSampleT * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleT; correction += iirSampleT;
		iirSampleU = (iirSampleU * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleU; correction += iirSampleU;
		iirSampleV = (iirSampleV * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleV; correction += iirSampleV;
		iirSampleW = (iirSampleW * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleW; correction += iirSampleW;
		iirSampleX = (iirSampleX * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleX; correction += iirSampleX;
		iirSampleY = (iirSampleY * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleY; correction += iirSampleY;
		iirSampleZ = (iirSampleZ * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleZ; correction += iirSampleZ;
		//do the IIR on a dummy sample, and store up the correction in a variable at the same scale as the very low level
		//numbers being used. Don't keep doing it against the possibly high level signal number.
		HeadBump -= correction;
		//apply stored up tiny corrections
		
		HeadBump *= (1.0f - (wreck/2));
		
		bridgerectifier = fabs(HeadBump);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (HeadBump > 0.0f) HeadBump = bridgerectifier;
		else HeadBump = -bridgerectifier;
		//clamp HeadBump to -1 to 1 with distortion		

		inputSample *= (1.0f - wreck);
		inputSample += (HeadBump * wreck);
		
		
		
		*destP = inputSample;
		//built in output trim and dry/wet by default
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	bflip = 1;
	iirHeadBumpA = 0.0;
	iirHeadBumpB = 0.0;
	iirHeadBumpC = 0.0;
		
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
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
