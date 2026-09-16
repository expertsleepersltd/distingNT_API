#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "FastDistance"
#define AIRWINDOWS_DESCRIPTION "A sort of ultra-efficient lowpass filter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','a','e' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
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
{ .name = "Distnce", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

				
		float firstDry,firstAvg;
		float psA,psB,psC,psD,psE,psF,psG,psH,psI,psJ,psK,psL,psM;
		float psN,psO,psP,psQ,psR,psS,psT,psU,psV,psW,psX,psY,psZ;
		float psa,psb,psc,psd,pse,psf,psg,psh,psi,psj,psk,psl,psm;
		float psn,pso,psp,psq,psr,pss,pst,psu,psv,psw,psx,psy,psz;
		int prevDistance;

		float pointCycle, pointAL, pointBL;
		//this is a simple linear interpolation sample rate routine for the plugin version
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
	overallscale /= 48000.0f; //we are treating both 44.1k and 48k as '1X'
	overallscale *= GetSampleRate(); //and expecting to get 2X, 4X as needed
	float pointDerez = 1.0f / overallscale; //forcing it to be simple
	
	float distance = powf(GetParameter( kParam_A )*7.28f,2.0f); //0 to 52.9984f
	int distanceSteps = (int)distance; //52 maximum
	distance -= (float)distanceSteps; //0.9984f
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		pointCycle += pointDerez;
		if (pointCycle > 0.9999999f) {
			//this is sample rate handling for the plugin, so that
			//a more primitive reverb node can run at any rate.
			
			//begin just the distance filter part
			firstDry = inputSample; //start by doing the interpolation
			inputSample += firstAvg; inputSample *= 0.5f; firstAvg = inputSample;
			inputSample = (firstDry*(1.0f-distance)) + (inputSample*distance);
			//having done at least one interpolation we can now do the integer number of stages
			if (prevDistance < distanceSteps) {
				switch (prevDistance)
				{
					case  0: psA = inputSample; //if we've changed the setting
					case  1: psB = inputSample; //reset the ones being put back into play
					case  2: psC = inputSample; //this won't be happening constantly,
					case  3: psD = inputSample; //but since we don't have an array
					case  4: psE = inputSample; //this is the structure by which we
					case  5: psF = inputSample; //get the new averagings going
					case  6: psG = inputSample; //without pops or clicks
					case  7: psH = inputSample;
					case  8: psI = inputSample;
					case  9: psJ = inputSample;
					case 10: psK = inputSample;
					case 11: psL = inputSample;
					case 12: psM = inputSample;
					case 13: psN = inputSample;
					case 14: psO = inputSample;
					case 15: psP = inputSample;
					case 16: psQ = inputSample;
					case 17: psR = inputSample;
					case 18: psS = inputSample;
					case 19: psT = inputSample;
					case 20: psU = inputSample;
					case 21: psV = inputSample;
					case 22: psW = inputSample;
					case 23: psX = inputSample;
					case 24: psY = inputSample;
					case 25: psZ = inputSample;
					case 26: psa = inputSample;
					case 27: psb = inputSample;
					case 28: psc = inputSample;
					case 29: psd = inputSample;
					case 30: pse = inputSample;
					case 31: psf = inputSample;
					case 32: psg = inputSample;
					case 33: psh = inputSample;
					case 34: psi = inputSample;
					case 35: psj = inputSample;
					case 36: psk = inputSample;
					case 37: psl = inputSample;
					case 38: psm = inputSample;
					case 39: psn = inputSample;
					case 40: pso = inputSample;
					case 41: psp = inputSample;
					case 42: psq = inputSample;
					case 43: psr = inputSample;
					case 44: pss = inputSample;
					case 45: pst = inputSample;
					case 46: psu = inputSample;
					case 47: psv = inputSample;
					case 48: psw = inputSample;
					case 49: psx = inputSample;
					case 50: psy = inputSample;
					case 51: psz = inputSample;
					case 52: prevDistance = distanceSteps;
				}
			}
			switch (52-distanceSteps)
			{ //apply the stack of filter steps to produce the distance filter
				case  0: inputSample += psz; inputSample *= 0.5f; psz = inputSample;
				case  1: inputSample += psy; inputSample *= 0.5f; psy = inputSample;
				case  2: inputSample += psx; inputSample *= 0.5f; psx = inputSample;
				case  3: inputSample += psw; inputSample *= 0.5f; psw = inputSample;
				case  4: inputSample += psv; inputSample *= 0.5f; psv = inputSample;
				case  5: inputSample += psu; inputSample *= 0.5f; psu = inputSample;
				case  6: inputSample += pst; inputSample *= 0.5f; pst = inputSample;
				case  7: inputSample += pss; inputSample *= 0.5f; pss = inputSample;
				case  8: inputSample += psr; inputSample *= 0.5f; psr = inputSample;
				case  9: inputSample += psq; inputSample *= 0.5f; psq = inputSample;
				case 10: inputSample += psp; inputSample *= 0.5f; psp = inputSample;
				case 11: inputSample += pso; inputSample *= 0.5f; pso = inputSample;
				case 12: inputSample += psn; inputSample *= 0.5f; psn = inputSample;
				case 13: inputSample += psm; inputSample *= 0.5f; psm = inputSample;
				case 14: inputSample += psl; inputSample *= 0.5f; psl = inputSample;
				case 15: inputSample += psk; inputSample *= 0.5f; psk = inputSample;
				case 16: inputSample += psj; inputSample *= 0.5f; psj = inputSample;
				case 17: inputSample += psi; inputSample *= 0.5f; psi = inputSample;
				case 18: inputSample += psh; inputSample *= 0.5f; psh = inputSample;
				case 19: inputSample += psg; inputSample *= 0.5f; psg = inputSample;
				case 20: inputSample += psf; inputSample *= 0.5f; psf = inputSample;
				case 21: inputSample += pse; inputSample *= 0.5f; pse = inputSample;
				case 22: inputSample += psd; inputSample *= 0.5f; psd = inputSample;
				case 23: inputSample += psc; inputSample *= 0.5f; psc = inputSample;
				case 24: inputSample += psb; inputSample *= 0.5f; psb = inputSample;
				case 25: inputSample += psa; inputSample *= 0.5f; psa = inputSample;
				case 26: inputSample += psZ; inputSample *= 0.5f; psZ = inputSample;
				case 27: inputSample += psY; inputSample *= 0.5f; psY = inputSample;
				case 28: inputSample += psX; inputSample *= 0.5f; psX = inputSample;
				case 29: inputSample += psW; inputSample *= 0.5f; psW = inputSample;
				case 30: inputSample += psV; inputSample *= 0.5f; psV = inputSample;
				case 31: inputSample += psU; inputSample *= 0.5f; psU = inputSample;
				case 32: inputSample += psT; inputSample *= 0.5f; psT = inputSample;
				case 33: inputSample += psS; inputSample *= 0.5f; psS = inputSample;
				case 34: inputSample += psR; inputSample *= 0.5f; psR = inputSample;
				case 35: inputSample += psQ; inputSample *= 0.5f; psQ = inputSample;
				case 36: inputSample += psP; inputSample *= 0.5f; psP = inputSample;
				case 37: inputSample += psO; inputSample *= 0.5f; psO = inputSample;
				case 38: inputSample += psN; inputSample *= 0.5f; psN = inputSample;
				case 39: inputSample += psM; inputSample *= 0.5f; psM = inputSample;
				case 40: inputSample += psL; inputSample *= 0.5f; psL = inputSample;
				case 41: inputSample += psK; inputSample *= 0.5f; psK = inputSample;
				case 42: inputSample += psJ; inputSample *= 0.5f; psJ = inputSample;
				case 43: inputSample += psI; inputSample *= 0.5f; psI = inputSample;
				case 44: inputSample += psH; inputSample *= 0.5f; psH = inputSample;
				case 45: inputSample += psG; inputSample *= 0.5f; psG = inputSample;
				case 46: inputSample += psF; inputSample *= 0.5f; psF = inputSample;
				case 47: inputSample += psE; inputSample *= 0.5f; psE = inputSample;
				case 48: inputSample += psD; inputSample *= 0.5f; psD = inputSample;
				case 49: inputSample += psC; inputSample *= 0.5f; psC = inputSample;
				case 50: inputSample += psB; inputSample *= 0.5f; psB = inputSample;
				case 51: inputSample += psA; inputSample *= 0.5f; psA = inputSample;
				case 52: break;
			}
			//end distance filter
			
			pointAL = pointBL;
			pointBL = inputSample;
			pointCycle = 0.0f; //interpolation point values
			//this is the sample rate adjustment for the plugin version
			//game engine may well always run at 44.1k or whatever			
		}
		inputSample = pointAL+((pointBL-pointAL)*pointCycle);
		//this is the linear interpolation between reverb points
		//governed by sample rate, to work as a plugin
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{	
	firstDry = firstAvg = 0.0f;
	psA = psB = psC = psD = psE = psF = psG = psH = psI = psJ = psK = psL = psM = 0.0f;
	psN = psO = psP = psQ = psR = psS = psT = psU = psV = psW = psX = psY = psZ = 0.0f;
	psa = psb = psc = psd = pse = psf = psg = psh = psi = psj = psk = psl = psm = 0.0f;
	psn = pso = psp = psq = psr = pss = pst = psu = psv = psw = psx = psy = psz = 0.0f;	
	prevDistance = 0;
	pointCycle = pointAL = pointBL = 0.0f;

	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
