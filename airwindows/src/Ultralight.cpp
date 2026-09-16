#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Ultralight"
#define AIRWINDOWS_DESCRIPTION "The most low-CPU matrix reverb I can make."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','U','l','t' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	//Add your parameters here...
	kNumberOfParameters=7
};
static const int kDefaultValue_ParamA = 6;
static const int kDefaultValue_ParamB = 3;
static const int kDefaultValue_ParamC = 3;
static const int kDefaultValue_ParamD = 3;
static const int kDefaultValue_ParamE = 1194;
const int d4A = 17; const int d4B = 1832; const int d4C = 1189; const int d4D = 1207; const int d4E = 172; const int d4F = 1740; const int d4G = 165; const int d4H = 43; const int d4I = 47; const int d4J = 53; const int d4K = 644; const int d4L = 61; const int d4M = 67; const int d4N = 71; const int d4O = 73; const int d4P = 79; //3 to 97 ms, 305 seat club Ultralight
const int predelay = 4097;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "RT60", .min = 0, .max = 9000, .def = 6000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Furnish", .min = 0, .max = 4000, .def = 3000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SolidWl", .min = 0, .max = 4000, .def = 3000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "ReflcWl", .min = 0, .max = 4000, .def = 3000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Predlay", .min = 1, .max = 4096, .def = 1194, .unit = kNT_unitNone, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Distnce", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		int cZ;
		
		int cA,cB,cC,cD,cE,cF,cG,cH;
		int cI,cJ,cK,cL,cM,cN,cO,cP;		
		float fA,fB,fC,fD;
		float fhA,fhB,fhC,fhD;
		float flA,flB,flC,flD;
		float hA, hB, hC, hD, hE; //do not allocate in audio code
		
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
			float aZ[predelay+5];
		float aA[d4A+5];
		float aB[d4B+5];
		float aC[d4C+5];
		float aD[d4D+5];
		float aE[d4E+5];
		float aF[d4F+5];
		float aG[d4G+5];
		float aH[d4H+5];
		float aI[d4I+5];
		float aJ[d4J+5];
		float aK[d4K+5];
		float aL[d4L+5];
		float aM[d4M+5];
		float aN[d4N+5];
		float aO[d4O+5];
		float aP[d4P+5];
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
	
	//these become presets and are built into the reverb when used
	float rtSixty = 0.0625f-(GetParameter( kParam_A )*0.003125f);
	int furnish = 4-GetParameter( kParam_B ); //0 through 4 are valid
	int wallDns = GetParameter( kParam_C ); //0 through 4 are valid
	int reflect = GetParameter( kParam_D ); //0 through 4 are valid
	int preDlay = GetParameter( kParam_E ); //0 through 4096 are valid
	//making five additional const ints adjusting the node in question
		
	float wet = GetParameter( kParam_F );
	float distance = powf(GetParameter( kParam_G )*7.28f,2.0f); //0 to 52.9984f
	int distanceSteps = (int)distance; //52 maximum
	distance -= (float)distanceSteps; //0.9984f
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		pointCycle += pointDerez;
		if (pointCycle > 0.9999999f) {
			//this is sample rate handling for the plugin, so that
			//a more primitive reverb node can run at any rate.
			
			float drySample = inputSample;
			//predelay for plugin: not suitable for game engine,
			//as it doesn't handle changing delays gracefully.
			//However, it might be worth designing a FIXED predelay for
			//each verb node, on the grounds that if you're right next to it,
			//you'll hear maximum delay before the sound bounces off the first wall.
			//As such it'd always be the same delay, and so it'd not pose a problem,
			//even for sound events that are moving around in the verb area.
			//it'd also mean you could set zero for other purposes.
			dram->aZ[cZ] = inputSample;
			cZ++; if (cZ < 0 || cZ > preDlay) cZ = 0;
			inputSample = dram->aZ[cZ-((cZ > preDlay)?preDlay+1:0)];
			//end predelay
			
			//begin just what's in reverb node
			
			if (furnish < 4) {
				hA = inputSample - (dram->aA[(cA+1)-((cA+1 > d4A)?d4A+1:0)]*0.5f);
				hA += (fA * rtSixty); dram->aA[cA] = hA; hA *= 0.5f;
				cA++;
				if (cA > d4A) cA = 0;
				hA += dram->aA[cA];
			} else {
				dram->aA[cA] = inputSample + (fA * rtSixty);
				cA++;
				if (cA > d4A) cA = 0;
				hA = dram->aA[cA];
			}
			if (furnish < 3) {
				hB = inputSample - (dram->aB[(cB+1)-((cB+1 > d4B)?d4B+1:0)]*0.5f);
				hB += (fB * rtSixty); dram->aB[cB] = hB; hB *= 0.5f;
				cB++;
				if (cB > d4B) cB = 0;
				hB += dram->aB[cB];
			} else {
				dram->aB[cB] = inputSample + (fB * rtSixty);
				cB++;
				if (cB > d4B) cB = 0;
				hB = dram->aB[cB];
			}
			if (furnish < 2) {
				hC = inputSample - (dram->aC[(cC+1)-((cC+1 > d4C)?d4C+1:0)]*0.5f);
				hC += (fC * rtSixty); dram->aC[cC] = hC; hC *= 0.5f;
				cC++;
				if (cC > d4C) cC = 0;
				hC += dram->aC[cC];
			} else {
				dram->aC[cC] = inputSample + (fC * rtSixty);
				cC++;
				if (cC > d4C) cC = 0;
				hC = dram->aC[cC];
			}
			if (furnish < 1) {
				hD = inputSample - (dram->aD[(cD+1)-((cD+1 > d4D)?d4D+1:0)]*0.5f);
				hD += (fD * rtSixty); dram->aD[cD] = hD; hD *= 0.5f;
				cD++;
				if (cD > d4D) cD = 0;
				hD += dram->aD[cD];
			} else {
				dram->aD[cD] = inputSample + (fD * rtSixty);
				cD++;
				if (cD > d4D) cD = 0;
				hD = dram->aD[cD];
			}
			hE = -(hA + hB + hC + hD);
			dram->aE[cE] = fma(hA,2.0f,hE);
			dram->aF[cF] = fma(hB,2.0f,hE);
			dram->aG[cG] = fma(hC,2.0f,hE);
			dram->aH[cH] = fma(hD,2.0f,hE);
			cE++; cF++; cG++; cH++;
			if (cE > d4E) cE = 0;
			if (cF > d4F) cF = 0;
			if (cG > d4G) cG = 0;
			if (cH > d4H) cH = 0;
			hA = dram->aE[cE]; hB = dram->aF[cF]; hC = dram->aG[cG]; hD = dram->aH[cH];
			hE = -(hA + hB + hC + hD);
			dram->aI[cI] = fma(hA,2.0f,hE);
			dram->aJ[cJ] = fma(hB,2.0f,hE);
			dram->aK[cK] = fma(hC,2.0f,hE);
			dram->aL[cL] = fma(hD,2.0f,hE);
			cI++; cJ++; cK++; cL++;
			if (cI > d4I) cI = 0;
			if (cJ > d4J) cJ = 0;
			if (cK > d4K) cK = 0;
			if (cL > d4L) cL = 0;
			hA = dram->aI[cI]; hB = dram->aJ[cJ]; hC = dram->aK[cK]; hD = dram->aL[cL];
			hE = -(hA + hB + hC + hD);
			dram->aM[cM] = fma(hA,2.0f,hE);
			dram->aN[cN] = fma(hB,2.0f,hE);
			dram->aO[cO] = fma(hC,2.0f,hE);
			dram->aP[cP] = fma(hD,2.0f,hE);
			cM++; cN++; cO++; cP++;
			if (cM > d4M) cM = 0;
			if (cN > d4N) cN = 0;
			if (cO > d4O) cO = 0;
			if (cP > d4P) cP = 0;
			hA = dram->aM[cM]; hB = dram->aN[cN]; hC = dram->aO[cO]; hD = dram->aP[cP];
			hE = -(hA + hB + hC + hD);
			fA = fma(hA,2.0f,hE);				
			fB = fma(hB,2.0f,hE);
			fC = fma(hC,2.0f,hE);
			fD = fma(hD,2.0f,hE);
			switch (reflect)
			{
				case 0:
					fA = (fA+flA)*0.5f; flA = fA;
				case 1:
					fB = (fB+flB)*0.5f; flB = fB;
				case 2:
					fC = (fC+flC)*0.5f; flC = fC;
				case 3:
					fD = (fD+flC)*0.5f; flD = fD;
				case 4:
					break; //This applies ALL processing
					//after the starting point, meaning the
					//cases intentionally fall through
			}
			switch (wallDns)
			{
				case 0:
					fhA *= 0.5f; fA -= fhA*0.0625f; fhA += fA;
				case 1:
					fhB *= 0.5f; fB -= fhB*0.0625f; fhB += fB;
				case 2:
					fhC *= 0.5f; fC -= fhC*0.0625f; fhC += fC;
				case 3:
					fhD *= 0.5f; fD -= fhD*0.0625f; fhD += fD;
				case 4:
					break; //This applies ALL processing
					//after the starting point, meaning the
					//cases intentionally fall through
			}
			inputSample = (hA + hB + hC + hD)*0.0625f;
			//end of just the reverb node part
			
			//dry/wet is not part of the game engine reverb node
			inputSample = (inputSample * wet)+(drySample * (1.0f-wet));
			//plugin only: game engine one would have just the reverb,
			//and every sound source could have the following Distance filter,
			//including the localized reverb nodes. The reason it's here
			//is so the plugin can preview how dry+reverb, both distant, will sound
			
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
	for(int x = 0; x < predelay+2; x++) dram->aZ[x] = 0.0f;
	cZ = 1;
	
	for(int x = 0; x < d4A+2; x++) dram->aA[x] = 0.0f;
	for(int x = 0; x < d4B+2; x++) dram->aB[x] = 0.0f;
	for(int x = 0; x < d4C+2; x++) dram->aC[x] = 0.0f;
	for(int x = 0; x < d4D+2; x++) dram->aD[x] = 0.0f;
	for(int x = 0; x < d4E+2; x++) dram->aE[x] = 0.0f;
	for(int x = 0; x < d4F+2; x++) dram->aF[x] = 0.0f;
	for(int x = 0; x < d4G+2; x++) dram->aG[x] = 0.0f;
	for(int x = 0; x < d4H+2; x++) dram->aH[x] = 0.0f;
	for(int x = 0; x < d4I+2; x++) dram->aI[x] = 0.0f;
	for(int x = 0; x < d4J+2; x++) dram->aJ[x] = 0.0f;
	for(int x = 0; x < d4K+2; x++) dram->aK[x] = 0.0f;
	for(int x = 0; x < d4L+2; x++) dram->aL[x] = 0.0f;
	for(int x = 0; x < d4M+2; x++) dram->aM[x] = 0.0f;
	for(int x = 0; x < d4N+2; x++) dram->aN[x] = 0.0f;
	for(int x = 0; x < d4O+2; x++) dram->aO[x] = 0.0f;
	for(int x = 0; x < d4P+2; x++) dram->aP[x] = 0.0f;
	cA = cB = cC = cD = cE = cF = cG = cH = 1;
	cI = cJ = cK = cL = cM = cN = cO = cP = 1;
	fA = fB = fC = fD = 0.0f;
	fhA = fhB = fhC = fhD = 0.0f;
	flA = flB = flC = flD = 0.0f;
	hA = hB = hC = hD = hE = 0.0f;

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
