#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PocketVerbs"
#define AIRWINDOWS_DESCRIPTION "My popular old special effects reverbs plugin!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','s' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
static const int kChamber = 1;
static const int kSpring = 2;
static const int kTiled = 3;
static const int kRoom = 4;
static const int kStretch = 5;
static const int kZarathustra = 6;
static const int kDefaultValue_ParamOne = kChamber;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static char const * const enumStrings0[] = { "", "Chamber", "Spring", "Tiled", "Room", "Stretch", "Zarathustra", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Type", .min = 1, .max = 6, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "Size", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gating", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 dA[7];
		Float32 dB[7];
		Float32 dC[7];
		Float32 dD[7];		
		Float32 dE[7];
		Float32 dF[7];
		Float32 dG[7];
		Float32 dH[7];
		Float32 dI[7];
		Float32 dJ[7];
		Float32 dK[7];
		Float32 dL[7];
		Float32 dM[7];
		Float32 dN[7];
		Float32 dO[7];
		Float32 dP[7];
		Float32 dQ[7];
		Float32 dR[7];
		Float32 dS[7];
		Float32 dT[7];
		Float32 dU[7];
		Float32 dV[7];
		Float32 dW[7];
		Float32 dX[7];
		Float32 dY[7];
		Float32 dZ[7];
		
		
		
		int outA, alpA, maxdelayA, delayA;
		int outB, alpB, maxdelayB, delayB;
		int outC, alpC, maxdelayC, delayC;
		int outD, alpD, maxdelayD, delayD;
		int outE, alpE, maxdelayE, delayE;
		int outF, alpF, maxdelayF, delayF;
		int outG, alpG, maxdelayG, delayG;
		int outH, alpH, maxdelayH, delayH;
		int outI, alpI, maxdelayI, delayI;
		int outJ, alpJ, maxdelayJ, delayJ;
		int outK, alpK, maxdelayK, delayK;
		int outL, alpL, maxdelayL, delayL;
		int outM, alpM, maxdelayM, delayM;
		int outN, alpN, maxdelayN, delayN;
		int outO, alpO, maxdelayO, delayO;
		int outP, alpP, maxdelayP, delayP;
		int outQ, alpQ, maxdelayQ, delayQ;
		int outR, alpR, maxdelayR, delayR;
		int outS, alpS, maxdelayS, delayS;
		int outT, alpT, maxdelayT, delayT;
		int outU, alpU, maxdelayU, delayU;
		int outV, alpV, maxdelayV, delayV;
		int outW, alpW, maxdelayW, delayW;
		int outX, alpX, maxdelayX, delayX;
		int outY, alpY, maxdelayY, delayY;
		int outZ, alpZ, maxdelayZ, delayZ;
		
		Float32 savedRoomsize;
		int countdown;
		Float32 peak;
		uint32_t fpd;

	
	struct _dram {
			Float32 aA[15150];
		Float32 aB[14618];
		Float32 aC[14358];
		Float32 aD[13818];		
		Float32 aE[13562];
		Float32 aF[13046];
		Float32 aG[11966];
		Float32 aH[11130];
		Float32 aI[10598];
		Float32 aJ[9810];
		Float32 aK[9522];
		Float32 aL[8982];
		Float32 aM[8786];
		Float32 aN[8462];
		Float32 aO[8310];
		Float32 aP[7982];
		Float32 aQ[7322];
		Float32 aR[6818];
		Float32 aS[6506];
		Float32 aT[6002];
		Float32 aU[5838];
		Float32 aV[5502];
		Float32 aW[5010];
		Float32 aX[4850];
		Float32 aY[4296];
		Float32 aZ[4180];
		Float32 oA[15150];
		Float32 oB[14618];
		Float32 oC[14358];
		Float32 oD[13818];		
		Float32 oE[13562];
		Float32 oF[13046];
		Float32 oG[11966];
		Float32 oH[11130];
		Float32 oI[10598];
		Float32 oJ[9810];
		Float32 oK[9522];
		Float32 oL[8982];
		Float32 oM[8786];
		Float32 oN[8462];
		Float32 oO[8310];
		Float32 oP[7982];
		Float32 oQ[7322];
		Float32 oR[6818];
		Float32 oS[6506];
		Float32 oT[6002];
		Float32 oU[5838];
		Float32 oV[5502];
		Float32 oW[5010];
		Float32 oX[4850];
		Float32 oY[4296];
		Float32 oZ[4180];
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
	int verbtype = GetParameter( kParam_One );
	Float32 roomsize = (powf(GetParameter( kParam_Two ),2)*1.9f)+0.1f;
	
	Float32 release = 0.00008f * powf(GetParameter( kParam_Three ),3);
	if (release == 0.0f) peak = 1.0f;
	Float32 wetnesstarget = GetParameter( kParam_Four );
	Float32 dryness = (1.0f - wetnesstarget);
	//verbs use base wetness value internally
	Float32 wetness = wetnesstarget;
	Float32 constallpass = 0.618033988749894848204586f; //golden ratio!
	int allpasstemp;
	int count;
	int max = 70; //biggest divisor to test primes against
	Float32 bridgerectifier;
	Float32 gain = 0.5f+(wetnesstarget*0.5f); //dryer for less verb drive
	//used as an aux, saturates when fed high levels
		
	//remap values to primes input number in question is 'i'
	//max is the largest prime we care about- HF interactions more interesting than the big numbers
	//pushing values larger and larger until we have a result
	//for (primetest=2; primetest <= max; primetest++) {if ( i!=primetest && i % primetest == 0 ) {i += 1; primetest=2;}}
	
	if (savedRoomsize != roomsize) {savedRoomsize = roomsize; countdown = 26;} //kick off the adjustment which will take 26 zippernoise refreshes to complete
	if (countdown > 0) {switch (countdown)
		{
			case 1:
				delayA = (int(maxdelayA * roomsize));
				for (count=2; count <= max; count++) {if ( delayA != count && delayA % count == 0 ) {delayA += 1; count=2;}} //try for primeish As
				if (delayA > maxdelayA) delayA = maxdelayA; //insanitycheck
				for(count = alpA; count < 15149; count++) {dram->aA[count] = 0.0f;}
				for(count = outA; count < 15149; count++) {dram->oA[count] = 0.0f;}
				break;
				
			case 2:
				delayB = (int(maxdelayB * roomsize));
				for (count=2; count <= max; count++) {if ( delayB != count && delayB % count == 0 ) {delayB += 1; count=2;}} //try for primeish Bs
				if (delayB > maxdelayB) delayB = maxdelayB; //insanitycheck
				for(count = alpB; count < 14617; count++) {dram->aB[count] = 0.0f;}
				for(count = outB; count < 14617; count++) {dram->oB[count] = 0.0f;}
				break;
				
			case 3:
				delayC = (int(maxdelayC * roomsize));
				for (count=2; count <= max; count++) {if ( delayC != count && delayC % count == 0 ) {delayC += 1; count=2;}} //try for primeish Cs
				if (delayC > maxdelayC) delayC = maxdelayC; //insanitycheck
				for(count = alpC; count < 14357; count++) {dram->aC[count] = 0.0f;}
				for(count = outC; count < 14357; count++) {dram->oC[count] = 0.0f;}
				break;
				
			case 4:
				delayD = (int(maxdelayD * roomsize));
				for (count=2; count <= max; count++) {if ( delayD != count && delayD % count == 0 ) {delayD += 1; count=2;}} //try for primeish Ds
				if (delayD > maxdelayD) delayD = maxdelayD; //insanitycheck
				for(count = alpD; count < 13817; count++) {dram->aD[count] = 0.0f;}
				for(count = outD; count < 13817; count++) {dram->oD[count] = 0.0f;}
				break;
				
			case 5:
				delayE = (int(maxdelayE * roomsize));
				for (count=2; count <= max; count++) {if ( delayE != count && delayE % count == 0 ) {delayE += 1; count=2;}} //try for primeish Es
				if (delayE > maxdelayE) delayE = maxdelayE; //insanitycheck
				for(count = alpE; count < 13561; count++) {dram->aE[count] = 0.0f;}
				for(count = outE; count < 13561; count++) {dram->oE[count] = 0.0f;}
				break;
				
			case 6:
				delayF = (int(maxdelayF * roomsize));
				for (count=2; count <= max; count++) {if ( delayF != count && delayF % count == 0 ) {delayF += 1; count=2;}} //try for primeish Fs
				if (delayF > maxdelayF) delayF = maxdelayF; //insanitycheck
				for(count = alpF; count < 13045; count++) {dram->aF[count] = 0.0f;}
				for(count = outF; count < 13045; count++) {dram->oF[count] = 0.0f;}
				break;
				
			case 7:
				delayG = (int(maxdelayG * roomsize));
				for (count=2; count <= max; count++) {if ( delayG != count && delayG % count == 0 ) {delayG += 1; count=2;}} //try for primeish Gs
				if (delayG > maxdelayG) delayG = maxdelayG; //insanitycheck
				for(count = alpG; count < 11965; count++) {dram->aG[count] = 0.0f;}
				for(count = outG; count < 11965; count++) {dram->oG[count] = 0.0f;}
				break;
				
			case 8:
				delayH = (int(maxdelayH * roomsize));
				for (count=2; count <= max; count++) {if ( delayH != count && delayH % count == 0 ) {delayH += 1; count=2;}} //try for primeish Hs
				if (delayH > maxdelayH) delayH = maxdelayH; //insanitycheck
				for(count = alpH; count < 11129; count++) {dram->aH[count] = 0.0f;}
				for(count = outH; count < 11129; count++) {dram->oH[count] = 0.0f;}
				break;
				
			case 9:
				delayI = (int(maxdelayI * roomsize));
				for (count=2; count <= max; count++) {if ( delayI != count && delayI % count == 0 ) {delayI += 1; count=2;}} //try for primeish Is
				if (delayI > maxdelayI) delayI = maxdelayI; //insanitycheck
				for(count = alpI; count < 10597; count++) {dram->aI[count] = 0.0f;}
				for(count = outI; count < 10597; count++) {dram->oI[count] = 0.0f;}
				break;
				
			case 10:
				delayJ = (int(maxdelayJ * roomsize));
				for (count=2; count <= max; count++) {if ( delayJ != count && delayJ % count == 0 ) {delayJ += 1; count=2;}} //try for primeish Js
				if (delayJ > maxdelayJ) delayJ = maxdelayJ; //insanitycheck
				for(count = alpJ; count < 9809; count++) {dram->aJ[count] = 0.0f;}
				for(count = outJ; count < 9809; count++) {dram->oJ[count] = 0.0f;}
				break;
				
			case 11:
				delayK = (int(maxdelayK * roomsize));
				for (count=2; count <= max; count++) {if ( delayK != count && delayK % count == 0 ) {delayK += 1; count=2;}} //try for primeish Ks
				if (delayK > maxdelayK) delayK = maxdelayK; //insanitycheck
				for(count = alpK; count < 9521; count++) {dram->aK[count] = 0.0f;}
				for(count = outK; count < 9521; count++) {dram->oK[count] = 0.0f;}
				break;
				
			case 12:
				delayL = (int(maxdelayL * roomsize));
				for (count=2; count <= max; count++) {if ( delayL != count && delayL % count == 0 ) {delayL += 1; count=2;}} //try for primeish Ls
				if (delayL > maxdelayL) delayL = maxdelayL; //insanitycheck
				for(count = alpL; count < 8981; count++) {dram->aL[count] = 0.0f;}
				for(count = outL; count < 8981; count++) {dram->oL[count] = 0.0f;}
				break;
				
			case 13:
				delayM = (int(maxdelayM * roomsize));
				for (count=2; count <= max; count++) {if ( delayM != count && delayM % count == 0 ) {delayM += 1; count=2;}} //try for primeish Ms
				if (delayM > maxdelayM) delayM = maxdelayM; //insanitycheck
				for(count = alpM; count < 8785; count++) {dram->aM[count] = 0.0f;}
				for(count = outM; count < 8785; count++) {dram->oM[count] = 0.0f;}
				break;
				
			case 14:
				delayN = (int(maxdelayN * roomsize));
				for (count=2; count <= max; count++) {if ( delayN != count && delayN % count == 0 ) {delayN += 1; count=2;}} //try for primeish Ns
				if (delayN > maxdelayN) delayN = maxdelayN; //insanitycheck
				for(count = alpN; count < 8461; count++) {dram->aN[count] = 0.0f;}
				for(count = outN; count < 8461; count++) {dram->oN[count] = 0.0f;}
				break;
				
			case 15:
				delayO = (int(maxdelayO * roomsize));
				for (count=2; count <= max; count++) {if ( delayO != count && delayO % count == 0 ) {delayO += 1; count=2;}} //try for primeish Os
				if (delayO > maxdelayO) delayO = maxdelayO; //insanitycheck
				for(count = alpO; count < 8309; count++) {dram->aO[count] = 0.0f;}
				for(count = outO; count < 8309; count++) {dram->oO[count] = 0.0f;}
				break;
				
			case 16:
				delayP = (int(maxdelayP * roomsize));
				for (count=2; count <= max; count++) {if ( delayP != count && delayP % count == 0 ) {delayP += 1; count=2;}} //try for primeish Ps
				if (delayP > maxdelayP) delayP = maxdelayP; //insanitycheck
				for(count = alpP; count < 7981; count++) {dram->aP[count] = 0.0f;}
				for(count = outP; count < 7981; count++) {dram->oP[count] = 0.0f;}
				break;
				
			case 17:
				delayQ = (int(maxdelayQ * roomsize));
				for (count=2; count <= max; count++) {if ( delayQ != count && delayQ % count == 0 ) {delayQ += 1; count=2;}} //try for primeish Qs
				if (delayQ > maxdelayQ) delayQ = maxdelayQ; //insanitycheck
				for(count = alpQ; count < 7321; count++) {dram->aQ[count] = 0.0f;}
				for(count = outQ; count < 7321; count++) {dram->oQ[count] = 0.0f;}
				break;
				
			case 18:
				delayR = (int(maxdelayR * roomsize));
				for (count=2; count <= max; count++) {if ( delayR != count && delayR % count == 0 ) {delayR += 1; count=2;}} //try for primeish Rs
				if (delayR > maxdelayR) delayR = maxdelayR; //insanitycheck
				for(count = alpR; count < 6817; count++) {dram->aR[count] = 0.0f;}
				for(count = outR; count < 6817; count++) {dram->oR[count] = 0.0f;}
				break;
				
			case 19:
				delayS = (int(maxdelayS * roomsize));
				for (count=2; count <= max; count++) {if ( delayS != count && delayS % count == 0 ) {delayS += 1; count=2;}} //try for primeish Ss
				if (delayS > maxdelayS) delayS = maxdelayS; //insanitycheck
				for(count = alpS; count < 6505; count++) {dram->aS[count] = 0.0f;}
				for(count = outS; count < 6505; count++) {dram->oS[count] = 0.0f;}
				break;
				
			case 20:
				delayT = (int(maxdelayT * roomsize));
				for (count=2; count <= max; count++) {if ( delayT != count && delayT % count == 0 ) {delayT += 1; count=2;}} //try for primeish Ts
				if (delayT > maxdelayT) delayT = maxdelayT; //insanitycheck
				for(count = alpT; count < 6001; count++) {dram->aT[count] = 0.0f;}
				for(count = outT; count < 6001; count++) {dram->oT[count] = 0.0f;}
				break;
				
			case 21:
				delayU = (int(maxdelayU * roomsize));
				for (count=2; count <= max; count++) {if ( delayU != count && delayU % count == 0 ) {delayU += 1; count=2;}} //try for primeish Us
				if (delayU > maxdelayU) delayU = maxdelayU; //insanitycheck
				for(count = alpU; count < 5837; count++) {dram->aU[count] = 0.0f;}
				for(count = outU; count < 5837; count++) {dram->oU[count] = 0.0f;}
				break;
				
			case 22:
				delayV = (int(maxdelayV * roomsize));
				for (count=2; count <= max; count++) {if ( delayV != count && delayV % count == 0 ) {delayV += 1; count=2;}} //try for primeish Vs
				if (delayV > maxdelayV) delayV = maxdelayV; //insanitycheck
				for(count = alpV; count < 5501; count++) {dram->aV[count] = 0.0f;}
				for(count = outV; count < 5501; count++) {dram->oV[count] = 0.0f;}
				break;
				
			case 23:
				delayW = (int(maxdelayW * roomsize));
				for (count=2; count <= max; count++) {if ( delayW != count && delayW % count == 0 ) {delayW += 1; count=2;}} //try for primeish Ws
				if (delayW > maxdelayW) delayW = maxdelayW; //insanitycheck
				for(count = alpW; count < 5009; count++) {dram->aW[count] = 0.0f;}
				for(count = outW; count < 5009; count++) {dram->oW[count] = 0.0f;}
				break;
				
			case 24:
				delayX = (int(maxdelayX * roomsize));
				for (count=2; count <= max; count++) {if ( delayX != count && delayX % count == 0 ) {delayX += 1; count=2;}} //try for primeish Xs
				if (delayX > maxdelayX) delayX = maxdelayX; //insanitycheck
				for(count = alpX; count < 4849; count++) {dram->aX[count] = 0.0f;}
				for(count = outX; count < 4849; count++) {dram->oX[count] = 0.0f;}
				break;
				
			case 25:
				delayY = (int(maxdelayY * roomsize));
				for (count=2; count <= max; count++) {if ( delayY != count && delayY % count == 0 ) {delayY += 1; count=2;}} //try for primeish Ys
				if (delayY > maxdelayY) delayY = maxdelayY; //insanitycheck
				for(count = alpY; count < 4295; count++) {dram->aY[count] = 0.0f;}
				for(count = outY; count < 4295; count++) {dram->oY[count] = 0.0f;}
				break;
				
			case 26:
				delayZ = (int(maxdelayZ * roomsize));
				for (count=2; count <= max; count++) {if ( delayZ != count && delayZ % count == 0 ) {delayZ += 1; count=2;}} //try for primeish Zs
				if (delayZ > maxdelayZ) delayZ = maxdelayZ; //insanitycheck
				for(count = alpZ; count < 4179; count++) {dram->aZ[count] = 0.0f;}	
				for(count = outZ; count < 4179; count++) {dram->oZ[count] = 0.0f;}
				break;
		} //end of switch statement
		countdown--;
	}
	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		peak -= release;
		if (peak < fabs(inputSample*2.0f)) peak = fabs(inputSample*2.0f);
		if (peak > 1.0f) peak = 1.0f;
		//chase the maximum signal to incorporate the wetter/louder behavior
		//boost for more extreme effect when in use, cap it
		
		inputSample *= gain;
		bridgerectifier = fabs(inputSample);
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//here we apply the ADT2 console-on-steroids trick		
						
		switch (verbtype)
		{

				
case 1://Chamber
				allpasstemp = alpA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->aA[allpasstemp]*constallpass;
				dram->aA[alpA] = inputSample;
				inputSample *= constallpass;
				alpA--; if (alpA < 0 || alpA > delayA) {alpA = delayA;}
				inputSample += (dram->aA[alpA]);
				//allpass filter A		
				
				dA[3] = dA[2];
				dA[2] = dA[1];
				dA[1] = inputSample;
				inputSample = (dA[1] + dA[2] + dA[3])/3.0f;
				
				allpasstemp = alpB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->aB[allpasstemp]*constallpass;
				dram->aB[alpB] = inputSample;
				inputSample *= constallpass;
				alpB--; if (alpB < 0 || alpB > delayB) {alpB = delayB;}
				inputSample += (dram->aB[alpB]);
				//allpass filter B
				
				dB[3] = dB[2];
				dB[2] = dB[1];
				dB[1] = inputSample;
				inputSample = (dB[1] + dB[2] + dB[3])/3.0f;
				
				allpasstemp = alpC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->aC[allpasstemp]*constallpass;
				dram->aC[alpC] = inputSample;
				inputSample *= constallpass;
				alpC--; if (alpC < 0 || alpC > delayC) {alpC = delayC;}
				inputSample += (dram->aC[alpC]);
				//allpass filter C
				
				dC[3] = dC[2];
				dC[2] = dC[1];
				dC[1] = inputSample;
				inputSample = (dA[1] + dC[2] + dC[3])/3.0f;
				
				allpasstemp = alpD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->aD[allpasstemp]*constallpass;
				dram->aD[alpD] = inputSample;
				inputSample *= constallpass;
				alpD--; if (alpD < 0 || alpD > delayD) {alpD = delayD;}
				inputSample += (dram->aD[alpD]);
				//allpass filter D
				
				dD[3] = dD[2];
				dD[2] = dD[1];
				dD[1] = inputSample;
				inputSample = (dD[1] + dD[2] + dD[3])/3.0f;
				
				allpasstemp = alpE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->aE[allpasstemp]*constallpass;
				dram->aE[alpE] = inputSample;
				inputSample *= constallpass;
				alpE--; if (alpE < 0 || alpE > delayE) {alpE = delayE;}
				inputSample += (dram->aE[alpE]);
				//allpass filter E
				
				dE[3] = dE[2];
				dE[2] = dE[1];
				dE[1] = inputSample;
				inputSample = (dA[1] + dE[2] + dE[3])/3.0f;
				
				allpasstemp = alpF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->aF[allpasstemp]*constallpass;
				dram->aF[alpF] = inputSample;
				inputSample *= constallpass;
				alpF--; if (alpF < 0 || alpF > delayF) {alpF = delayF;}
				inputSample += (dram->aF[alpF]);
				//allpass filter F
				
				dF[3] = dF[2];
				dF[2] = dF[1];
				dF[1] = inputSample;
				inputSample = (dF[1] + dF[2] + dF[3])/3.0f;
				
				allpasstemp = alpG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->aG[allpasstemp]*constallpass;
				dram->aG[alpG] = inputSample;
				inputSample *= constallpass;
				alpG--; if (alpG < 0 || alpG > delayG) {alpG = delayG;}
				inputSample += (dram->aG[alpG]);
				//allpass filter G
				
				dG[3] = dG[2];
				dG[2] = dG[1];
				dG[1] = inputSample;
				inputSample = (dA[1] + dG[2] + dG[3])/3.0f;
				
				allpasstemp = alpH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->aH[allpasstemp]*constallpass;
				dram->aH[alpH] = inputSample;
				inputSample *= constallpass;
				alpH--; if (alpH < 0 || alpH > delayH) {alpH = delayH;}
				inputSample += (dram->aH[alpH]);
				//allpass filter H
				
				dH[3] = dH[2];
				dH[2] = dH[1];
				dH[1] = inputSample;
				inputSample = (dH[1] + dH[2] + dH[3])/3.0f;
				
				allpasstemp = alpI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->aI[allpasstemp]*constallpass;
				dram->aI[alpI] = inputSample;
				inputSample *= constallpass;
				alpI--; if (alpI < 0 || alpI > delayI) {alpI = delayI;}
				inputSample += (dram->aI[alpI]);
				//allpass filter I
				
				dI[3] = dI[2];
				dI[2] = dI[1];
				dI[1] = inputSample;
				inputSample = (dI[1] + dI[2] + dI[3])/3.0f;
				
				allpasstemp = alpJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->aJ[allpasstemp]*constallpass;
				dram->aJ[alpJ] = inputSample;
				inputSample *= constallpass;
				alpJ--; if (alpJ < 0 || alpJ > delayJ) {alpJ = delayJ;}
				inputSample += (dram->aJ[alpJ]);
				//allpass filter J
				
				dJ[3] = dJ[2];
				dJ[2] = dJ[1];
				dJ[1] = inputSample;
				inputSample = (dJ[1] + dJ[2] + dJ[3])/3.0f;
				
				allpasstemp = alpK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->aK[allpasstemp]*constallpass;
				dram->aK[alpK] = inputSample;
				inputSample *= constallpass;
				alpK--; if (alpK < 0 || alpK > delayK) {alpK = delayK;}
				inputSample += (dram->aK[alpK]);
				//allpass filter K
				
				dK[3] = dK[2];
				dK[2] = dK[1];
				dK[1] = inputSample;
				inputSample = (dA[1] + dK[2] + dK[3])/3.0f;
				
				allpasstemp = alpL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->aL[allpasstemp]*constallpass;
				dram->aL[alpL] = inputSample;
				inputSample *= constallpass;
				alpL--; if (alpL < 0 || alpL > delayL) {alpL = delayL;}
				inputSample += (dram->aL[alpL]);
				//allpass filter L
				
				dL[3] = dL[2];
				dL[2] = dL[1];
				dL[1] = inputSample;
				inputSample = (dL[1] + dL[2] + dL[3])/3.0f;
				
				allpasstemp = alpM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->aM[allpasstemp]*constallpass;
				dram->aM[alpM] = inputSample;
				inputSample *= constallpass;
				alpM--; if (alpM < 0 || alpM > delayM) {alpM = delayM;}
				inputSample += (dram->aM[alpM]);
				//allpass filter M
				
				dM[3] = dM[2];
				dM[2] = dM[1];
				dM[1] = inputSample;
				inputSample = (dA[1] + dM[2] + dM[3])/3.0f;
				
				allpasstemp = alpN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->aN[allpasstemp]*constallpass;
				dram->aN[alpN] = inputSample;
				inputSample *= constallpass;
				alpN--; if (alpN < 0 || alpN > delayN) {alpN = delayN;}
				inputSample += (dram->aN[alpN]);
				//allpass filter N
				
				dN[3] = dN[2];
				dN[2] = dN[1];
				dN[1] = inputSample;
				inputSample = (dN[1] + dN[2] + dN[3])/3.0f;
				
				allpasstemp = alpO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->aO[allpasstemp]*constallpass;
				dram->aO[alpO] = inputSample;
				inputSample *= constallpass;
				alpO--; if (alpO < 0 || alpO > delayO) {alpO = delayO;}
				inputSample += (dram->aO[alpO]);
				//allpass filter O
				
				dO[3] = dO[2];
				dO[2] = dO[1];
				dO[1] = inputSample;
				inputSample = (dO[1] + dO[2] + dO[3])/3.0f;
				
				allpasstemp = alpP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->aP[allpasstemp]*constallpass;
				dram->aP[alpP] = inputSample;
				inputSample *= constallpass;
				alpP--; if (alpP < 0 || alpP > delayP) {alpP = delayP;}
				inputSample += (dram->aP[alpP]);
				//allpass filter P
				
				dP[3] = dP[2];
				dP[2] = dP[1];
				dP[1] = inputSample;
				inputSample = (dP[1] + dP[2] + dP[3])/3.0f;
				
				allpasstemp = alpQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->aQ[allpasstemp]*constallpass;
				dram->aQ[alpQ] = inputSample;
				inputSample *= constallpass;
				alpQ--; if (alpQ < 0 || alpQ > delayQ) {alpQ = delayQ;}
				inputSample += (dram->aQ[alpQ]);
				//allpass filter Q
				
				dQ[3] = dQ[2];
				dQ[2] = dQ[1];
				dQ[1] = inputSample;
				inputSample = (dA[1] + dQ[2] + dQ[3])/3.0f;
				
				allpasstemp = alpR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->aR[allpasstemp]*constallpass;
				dram->aR[alpR] = inputSample;
				inputSample *= constallpass;
				alpR--; if (alpR < 0 || alpR > delayR) {alpR = delayR;}
				inputSample += (dram->aR[alpR]);
				//allpass filter R
				
				dR[3] = dR[2];
				dR[2] = dR[1];
				dR[1] = inputSample;
				inputSample = (dR[1] + dR[2] + dR[3])/3.0f;
				
				allpasstemp = alpS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->aS[allpasstemp]*constallpass;
				dram->aS[alpS] = inputSample;
				inputSample *= constallpass;
				alpS--; if (alpS < 0 || alpS > delayS) {alpS = delayS;}
				inputSample += (dram->aS[alpS]);
				//allpass filter S
				
				dS[3] = dS[2];
				dS[2] = dS[1];
				dS[1] = inputSample;
				inputSample = (dA[1] + dS[2] + dS[3])/3.0f;
				
				allpasstemp = alpT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->aT[allpasstemp]*constallpass;
				dram->aT[alpT] = inputSample;
				inputSample *= constallpass;
				alpT--; if (alpT < 0 || alpT > delayT) {alpT = delayT;}
				inputSample += (dram->aT[alpT]);
				//allpass filter T
				
				dT[3] = dT[2];
				dT[2] = dT[1];
				dT[1] = inputSample;
				inputSample = (dT[1] + dT[2] + dT[3])/3.0f;
				
				allpasstemp = alpU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->aU[allpasstemp]*constallpass;
				dram->aU[alpU] = inputSample;
				inputSample *= constallpass;
				alpU--; if (alpU < 0 || alpU > delayU) {alpU = delayU;}
				inputSample += (dram->aU[alpU]);
				//allpass filter U
				
				dU[3] = dU[2];
				dU[2] = dU[1];
				dU[1] = inputSample;
				inputSample = (dU[1] + dU[2] + dU[3])/3.0f;
				
				allpasstemp = alpV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->aV[allpasstemp]*constallpass;
				dram->aV[alpV] = inputSample;
				inputSample *= constallpass;
				alpV--; if (alpV < 0 || alpV > delayV) {alpV = delayV;}
				inputSample += (dram->aV[alpV]);
				//allpass filter V
				
				dV[3] = dV[2];
				dV[2] = dV[1];
				dV[1] = inputSample;
				inputSample = (dV[1] + dV[2] + dV[3])/3.0f;
				
				allpasstemp = alpW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->aW[allpasstemp]*constallpass;
				dram->aW[alpW] = inputSample;
				inputSample *= constallpass;
				alpW--; if (alpW < 0 || alpW > delayW) {alpW = delayW;}
				inputSample += (dram->aW[alpW]);
				//allpass filter W
				
				dW[3] = dW[2];
				dW[2] = dW[1];
				dW[1] = inputSample;
				inputSample = (dA[1] + dW[2] + dW[3])/3.0f;
				
				allpasstemp = alpX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->aX[allpasstemp]*constallpass;
				dram->aX[alpX] = inputSample;
				inputSample *= constallpass;
				alpX--; if (alpX < 0 || alpX > delayX) {alpX = delayX;}
				inputSample += (dram->aX[alpX]);
				//allpass filter X
				
				dX[3] = dX[2];
				dX[2] = dX[1];
				dX[1] = inputSample;
				inputSample = (dX[1] + dX[2] + dX[3])/3.0f;
				
				allpasstemp = alpY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->aY[allpasstemp]*constallpass;
				dram->aY[alpY] = inputSample;
				inputSample *= constallpass;
				alpY--; if (alpY < 0 || alpY > delayY) {alpY = delayY;}
				inputSample += (dram->aY[alpY]);
				//allpass filter Y
				
				dY[3] = dY[2];
				dY[2] = dY[1];
				dY[1] = inputSample;
				inputSample = (dY[1] + dY[2] + dY[3])/3.0f;
				
				allpasstemp = alpZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->aZ[allpasstemp]*constallpass;
				dram->aZ[alpZ] = inputSample;
				inputSample *= constallpass;
				alpZ--; if (alpZ < 0 || alpZ > delayZ) {alpZ = delayZ;}
				inputSample += (dram->aZ[alpZ]);
				//allpass filter Z
				
				dZ[3] = dZ[2];
				dZ[2] = dZ[1];
				dZ[1] = inputSample;
				inputSample = (dZ[1] + dZ[2] + dZ[3])/3.0f;
				
				// now the second stage using the 'out' bank of allpasses		
				
				allpasstemp = outA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->oA[allpasstemp]*constallpass;
				dram->oA[outA] = inputSample;
				inputSample *= constallpass;
				outA--; if (outA < 0 || outA > delayA) {outA = delayA;}
				inputSample += (dram->oA[outA]);
				//allpass filter A		
				
				dA[6] = dA[5];
				dA[5] = dA[4];
				dA[4] = inputSample;
				inputSample = (dA[4] + dA[5] + dA[6])/3.0f;
				
				allpasstemp = outB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->oB[allpasstemp]*constallpass;
				dram->oB[outB] = inputSample;
				inputSample *= constallpass;
				outB--; if (outB < 0 || outB > delayB) {outB = delayB;}
				inputSample += (dram->oB[outB]);
				//allpass filter B
				
				dB[6] = dB[5];
				dB[5] = dB[4];
				dB[4] = inputSample;
				inputSample = (dB[4] + dB[5] + dB[6])/3.0f;
				
				allpasstemp = outC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->oC[allpasstemp]*constallpass;
				dram->oC[outC] = inputSample;
				inputSample *= constallpass;
				outC--; if (outC < 0 || outC > delayC) {outC = delayC;}
				inputSample += (dram->oC[outC]);
				//allpass filter C
				
				dC[6] = dC[5];
				dC[5] = dC[4];
				dC[4] = inputSample;
				inputSample = (dA[1] + dC[5] + dC[6])/3.0f;
				
				allpasstemp = outD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->oD[allpasstemp]*constallpass;
				dram->oD[outD] = inputSample;
				inputSample *= constallpass;
				outD--; if (outD < 0 || outD > delayD) {outD = delayD;}
				inputSample += (dram->oD[outD]);
				//allpass filter D
				
				dD[6] = dD[5];
				dD[5] = dD[4];
				dD[4] = inputSample;
				inputSample = (dD[4] + dD[5] + dD[6])/3.0f;
				
				allpasstemp = outE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->oE[allpasstemp]*constallpass;
				dram->oE[outE] = inputSample;
				inputSample *= constallpass;
				outE--; if (outE < 0 || outE > delayE) {outE = delayE;}
				inputSample += (dram->oE[outE]);
				//allpass filter E
				
				dE[6] = dE[5];
				dE[5] = dE[4];
				dE[4] = inputSample;
				inputSample = (dA[1] + dE[5] + dE[6])/3.0f;
				
				allpasstemp = outF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->oF[allpasstemp]*constallpass;
				dram->oF[outF] = inputSample;
				inputSample *= constallpass;
				outF--; if (outF < 0 || outF > delayF) {outF = delayF;}
				inputSample += (dram->oF[outF]);
				//allpass filter F
				
				dF[6] = dF[5];
				dF[5] = dF[4];
				dF[4] = inputSample;
				inputSample = (dF[4] + dF[5] + dF[6])/3.0f;
				
				allpasstemp = outG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->oG[allpasstemp]*constallpass;
				dram->oG[outG] = inputSample;
				inputSample *= constallpass;
				outG--; if (outG < 0 || outG > delayG) {outG = delayG;}
				inputSample += (dram->oG[outG]);
				//allpass filter G
				
				dG[6] = dG[5];
				dG[5] = dG[4];
				dG[4] = inputSample;
				inputSample = (dG[4] + dG[5] + dG[6])/3.0f;
				
				allpasstemp = outH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->oH[allpasstemp]*constallpass;
				dram->oH[outH] = inputSample;
				inputSample *= constallpass;
				outH--; if (outH < 0 || outH > delayH) {outH = delayH;}
				inputSample += (dram->oH[outH]);
				//allpass filter H
				
				dH[6] = dH[5];
				dH[5] = dH[4];
				dH[4] = inputSample;
				inputSample = (dH[4] + dH[5] + dH[6])/3.0f;
				
				allpasstemp = outI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->oI[allpasstemp]*constallpass;
				dram->oI[outI] = inputSample;
				inputSample *= constallpass;
				outI--; if (outI < 0 || outI > delayI) {outI = delayI;}
				inputSample += (dram->oI[outI]);
				//allpass filter I
				
				dI[6] = dI[5];
				dI[5] = dI[4];
				dI[4] = inputSample;
				inputSample = (dI[4] + dI[5] + dI[6])/3.0f;
				
				allpasstemp = outJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->oJ[allpasstemp]*constallpass;
				dram->oJ[outJ] = inputSample;
				inputSample *= constallpass;
				outJ--; if (outJ < 0 || outJ > delayJ) {outJ = delayJ;}
				inputSample += (dram->oJ[outJ]);
				//allpass filter J
				
				dJ[6] = dJ[5];
				dJ[5] = dJ[4];
				dJ[4] = inputSample;
				inputSample = (dJ[4] + dJ[5] + dJ[6])/3.0f;
				
				allpasstemp = outK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->oK[allpasstemp]*constallpass;
				dram->oK[outK] = inputSample;
				inputSample *= constallpass;
				outK--; if (outK < 0 || outK > delayK) {outK = delayK;}
				inputSample += (dram->oK[outK]);
				//allpass filter K
				
				dK[6] = dK[5];
				dK[5] = dK[4];
				dK[4] = inputSample;
				inputSample = (dA[1] + dK[5] + dK[6])/3.0f;
				
				allpasstemp = outL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->oL[allpasstemp]*constallpass;
				dram->oL[outL] = inputSample;
				inputSample *= constallpass;
				outL--; if (outL < 0 || outL > delayL) {outL = delayL;}
				inputSample += (dram->oL[outL]);
				//allpass filter L
				
				dL[6] = dL[5];
				dL[5] = dL[4];
				dL[4] = inputSample;
				inputSample = (dL[4] + dL[5] + dL[6])/3.0f;
				
				allpasstemp = outM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->oM[allpasstemp]*constallpass;
				dram->oM[outM] = inputSample;
				inputSample *= constallpass;
				outM--; if (outM < 0 || outM > delayM) {outM = delayM;}
				inputSample += (dram->oM[outM]);
				//allpass filter M
				
				dM[6] = dM[5];
				dM[5] = dM[4];
				dM[4] = inputSample;
				inputSample = (dM[4] + dM[5] + dM[6])/3.0f;
				
				allpasstemp = outN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->oN[allpasstemp]*constallpass;
				dram->oN[outN] = inputSample;
				inputSample *= constallpass;
				outN--; if (outN < 0 || outN > delayN) {outN = delayN;}
				inputSample += (dram->oN[outN]);
				//allpass filter N
				
				dN[6] = dN[5];
				dN[5] = dN[4];
				dN[4] = inputSample;
				inputSample = (dN[4] + dN[5] + dN[6])/3.0f;
				
				allpasstemp = outO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->oO[allpasstemp]*constallpass;
				dram->oO[outO] = inputSample;
				inputSample *= constallpass;
				outO--; if (outO < 0 || outO > delayO) {outO = delayO;}
				inputSample += (dram->oO[outO]);
				//allpass filter O
				
				dO[6] = dO[5];
				dO[5] = dO[4];
				dO[4] = inputSample;
				inputSample = (dA[1] + dO[5] + dO[6])/3.0f;
				
				allpasstemp = outP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->oP[allpasstemp]*constallpass;
				dram->oP[outP] = inputSample;
				inputSample *= constallpass;
				outP--; if (outP < 0 || outP > delayP) {outP = delayP;}
				inputSample += (dram->oP[outP]);
				//allpass filter P
				
				dP[6] = dP[5];
				dP[5] = dP[4];
				dP[4] = inputSample;
				inputSample = (dP[4] + dP[5] + dP[6])/3.0f;
				
				allpasstemp = outQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->oQ[allpasstemp]*constallpass;
				dram->oQ[outQ] = inputSample;
				inputSample *= constallpass;
				outQ--; if (outQ < 0 || outQ > delayQ) {outQ = delayQ;}
				inputSample += (dram->oQ[outQ]);
				//allpass filter Q
				
				dQ[6] = dQ[5];
				dQ[5] = dQ[4];
				dQ[4] = inputSample;
				inputSample = (dA[1] + dQ[5] + dQ[6])/3.0f;
				
				allpasstemp = outR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->oR[allpasstemp]*constallpass;
				dram->oR[outR] = inputSample;
				inputSample *= constallpass;
				outR--; if (outR < 0 || outR > delayR) {outR = delayR;}
				inputSample += (dram->oR[outR]);
				//allpass filter R
				
				dR[6] = dR[5];
				dR[5] = dR[4];
				dR[4] = inputSample;
				inputSample = (dR[4] + dR[5] + dR[6])/3.0f;
				
				allpasstemp = outS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->oS[allpasstemp]*constallpass;
				dram->oS[outS] = inputSample;
				inputSample *= constallpass;
				outS--; if (outS < 0 || outS > delayS) {outS = delayS;}
				inputSample += (dram->oS[outS]);
				//allpass filter S
				
				dS[6] = dS[5];
				dS[5] = dS[4];
				dS[4] = inputSample;
				inputSample = (dS[4] + dS[5] + dS[6])/3.0f;
				
				allpasstemp = outT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->oT[allpasstemp]*constallpass;
				dram->oT[outT] = inputSample;
				inputSample *= constallpass;
				outT--; if (outT < 0 || outT > delayT) {outT = delayT;}
				inputSample += (dram->oT[outT]);
				//allpass filter T
				
				dT[6] = dT[5];
				dT[5] = dT[4];
				dT[4] = inputSample;
				inputSample = (dT[4] + dT[5] + dT[6])/3.0f;
				
				allpasstemp = outU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->oU[allpasstemp]*constallpass;
				dram->oU[outU] = inputSample;
				inputSample *= constallpass;
				outU--; if (outU < 0 || outU > delayU) {outU = delayU;}
				inputSample += (dram->oU[outU]);
				//allpass filter U
				
				dU[6] = dU[5];
				dU[5] = dU[4];
				dU[4] = inputSample;
				inputSample = (dA[1] + dU[5] + dU[6])/3.0f;
				
				allpasstemp = outV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->oV[allpasstemp]*constallpass;
				dram->oV[outV] = inputSample;
				inputSample *= constallpass;
				outV--; if (outV < 0 || outV > delayV) {outV = delayV;}
				inputSample += (dram->oV[outV]);
				//allpass filter V
				
				dV[6] = dV[5];
				dV[5] = dV[4];
				dV[4] = inputSample;
				inputSample = (dV[4] + dV[5] + dV[6])/3.0f;
				
				allpasstemp = outW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->oW[allpasstemp]*constallpass;
				dram->oW[outW] = inputSample;
				inputSample *= constallpass;
				outW--; if (outW < 0 || outW > delayW) {outW = delayW;}
				inputSample += (dram->oW[outW]);
				//allpass filter W
				
				dW[6] = dW[5];
				dW[5] = dW[4];
				dW[4] = inputSample;
				inputSample = (dW[4] + dW[5] + dW[6])/3.0f;
				
				allpasstemp = outX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->oX[allpasstemp]*constallpass;
				dram->oX[outX] = inputSample;
				inputSample *= constallpass;
				outX--; if (outX < 0 || outX > delayX) {outX = delayX;}
				inputSample += (dram->oX[outX]);
				//allpass filter X
				
				dX[6] = dX[5];
				dX[5] = dX[4];
				dX[4] = inputSample;
				inputSample = (dX[4] + dX[5] + dX[6])/3.0f;
				
				allpasstemp = outY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->oY[allpasstemp]*constallpass;
				dram->oY[outY] = inputSample;
				inputSample *= constallpass;
				outY--; if (outY < 0 || outY > delayY) {outY = delayY;}
				inputSample += (dram->oY[outY]);
				//allpass filter Y
				
				dY[6] = dY[5];
				dY[5] = dY[4];
				dY[4] = inputSample;
				inputSample = (dY[4] + dY[5] + dY[6])/3.0f;
				
				allpasstemp = outZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->oZ[allpasstemp]*constallpass;
				dram->oZ[outZ] = inputSample;
				inputSample *= constallpass;
				outZ--; if (outZ < 0 || outZ > delayZ) {outZ = delayZ;}
				inputSample += (dram->oZ[outZ]);
				//allpass filter Z
				
				dZ[6] = dZ[5];
				dZ[5] = dZ[4];
				dZ[4] = inputSample;
				inputSample = (dZ[4] + dZ[5] + dZ[6]);		
				//output Chamber
				break;

				
				
				
				
			case 2:	//Spring
				
				allpasstemp = alpA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->aA[allpasstemp]*constallpass;
				dram->aA[alpA] = inputSample;
				inputSample *= constallpass;
				alpA--; if (alpA < 0 || alpA > delayA) {alpA = delayA;}
				inputSample += (dram->aA[alpA]);
				//allpass filter A		
				
				dA[3] = dA[2];
				dA[2] = dA[1];
				dA[1] = inputSample;
				inputSample = (dA[1] + dA[2] + dA[3])/3.0f;
				
				allpasstemp = alpB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->aB[allpasstemp]*constallpass;
				dram->aB[alpB] = inputSample;
				inputSample *= constallpass;
				alpB--; if (alpB < 0 || alpB > delayB) {alpB = delayB;}
				inputSample += (dram->aB[alpB]);
				//allpass filter B
				
				dB[3] = dB[2];
				dB[2] = dB[1];
				dB[1] = inputSample;
				inputSample = (dB[1] + dB[2] + dB[3])/3.0f;
				
				allpasstemp = alpC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->aC[allpasstemp]*constallpass;
				dram->aC[alpC] = inputSample;
				inputSample *= constallpass;
				alpC--; if (alpC < 0 || alpC > delayC) {alpC = delayC;}
				inputSample += (dram->aC[alpC]);
				//allpass filter C
				
				dC[3] = dC[2];
				dC[2] = dC[1];
				dC[1] = inputSample;
				inputSample = (dC[1] + dC[2] + dC[3])/3.0f;
				
				allpasstemp = alpD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->aD[allpasstemp]*constallpass;
				dram->aD[alpD] = inputSample;
				inputSample *= constallpass;
				alpD--; if (alpD < 0 || alpD > delayD) {alpD = delayD;}
				inputSample += (dram->aD[alpD]);
				//allpass filter D
				
				dD[3] = dD[2];
				dD[2] = dD[1];
				dD[1] = inputSample;
				inputSample = (dD[1] + dD[2] + dD[3])/3.0f;
				
				allpasstemp = alpE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->aE[allpasstemp]*constallpass;
				dram->aE[alpE] = inputSample;
				inputSample *= constallpass;
				alpE--; if (alpE < 0 || alpE > delayE) {alpE = delayE;}
				inputSample += (dram->aE[alpE]);
				//allpass filter E
				
				dE[3] = dE[2];
				dE[2] = dE[1];
				dE[1] = inputSample;
				inputSample = (dE[1] + dE[2] + dE[3])/3.0f;
				
				allpasstemp = alpF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->aF[allpasstemp]*constallpass;
				dram->aF[alpF] = inputSample;
				inputSample *= constallpass;
				alpF--; if (alpF < 0 || alpF > delayF) {alpF = delayF;}
				inputSample += (dram->aF[alpF]);
				//allpass filter F
				
				dF[3] = dF[2];
				dF[2] = dF[1];
				dF[1] = inputSample;
				inputSample = (dF[1] + dF[2] + dF[3])/3.0f;
				
				allpasstemp = alpG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->aG[allpasstemp]*constallpass;
				dram->aG[alpG] = inputSample;
				inputSample *= constallpass;
				alpG--; if (alpG < 0 || alpG > delayG) {alpG = delayG;}
				inputSample += (dram->aG[alpG]);
				//allpass filter G
				
				dG[3] = dG[2];
				dG[2] = dG[1];
				dG[1] = inputSample;
				inputSample = (dG[1] + dG[2] + dG[3])/3.0f;
				
				allpasstemp = alpH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->aH[allpasstemp]*constallpass;
				dram->aH[alpH] = inputSample;
				inputSample *= constallpass;
				alpH--; if (alpH < 0 || alpH > delayH) {alpH = delayH;}
				inputSample += (dram->aH[alpH]);
				//allpass filter H
				
				dH[3] = dH[2];
				dH[2] = dH[1];
				dH[1] = inputSample;
				inputSample = (dH[1] + dH[2] + dH[3])/3.0f;
				
				allpasstemp = alpI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->aI[allpasstemp]*constallpass;
				dram->aI[alpI] = inputSample;
				inputSample *= constallpass;
				alpI--; if (alpI < 0 || alpI > delayI) {alpI = delayI;}
				inputSample += (dram->aI[alpI]);
				//allpass filter I
				
				dI[3] = dI[2];
				dI[2] = dI[1];
				dI[1] = inputSample;
				inputSample = (dI[1] + dI[2] + dI[3])/3.0f;
				
				allpasstemp = alpJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->aJ[allpasstemp]*constallpass;
				dram->aJ[alpJ] = inputSample;
				inputSample *= constallpass;
				alpJ--; if (alpJ < 0 || alpJ > delayJ) {alpJ = delayJ;}
				inputSample += (dram->aJ[alpJ]);
				//allpass filter J
				
				dJ[3] = dJ[2];
				dJ[2] = dJ[1];
				dJ[1] = inputSample;
				inputSample = (dJ[1] + dJ[2] + dJ[3])/3.0f;
				
				allpasstemp = alpK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->aK[allpasstemp]*constallpass;
				dram->aK[alpK] = inputSample;
				inputSample *= constallpass;
				alpK--; if (alpK < 0 || alpK > delayK) {alpK = delayK;}
				inputSample += (dram->aK[alpK]);
				//allpass filter K
				
				dK[3] = dK[2];
				dK[2] = dK[1];
				dK[1] = inputSample;
				inputSample = (dK[1] + dK[2] + dK[3])/3.0f;
				
				allpasstemp = alpL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->aL[allpasstemp]*constallpass;
				dram->aL[alpL] = inputSample;
				inputSample *= constallpass;
				alpL--; if (alpL < 0 || alpL > delayL) {alpL = delayL;}
				inputSample += (dram->aL[alpL]);
				//allpass filter L
				
				dL[3] = dL[2];
				dL[2] = dL[1];
				dL[1] = inputSample;
				inputSample = (dL[1] + dL[2] + dL[3])/3.0f;
				
				allpasstemp = alpM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->aM[allpasstemp]*constallpass;
				dram->aM[alpM] = inputSample;
				inputSample *= constallpass;
				alpM--; if (alpM < 0 || alpM > delayM) {alpM = delayM;}
				inputSample += (dram->aM[alpM]);
				//allpass filter M
				
				dM[3] = dM[2];
				dM[2] = dM[1];
				dM[1] = inputSample;
				inputSample = (dM[1] + dM[2] + dM[3])/3.0f;
				
				allpasstemp = alpN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->aN[allpasstemp]*constallpass;
				dram->aN[alpN] = inputSample;
				inputSample *= constallpass;
				alpN--; if (alpN < 0 || alpN > delayN) {alpN = delayN;}
				inputSample += (dram->aN[alpN]);
				//allpass filter N
				
				dN[3] = dN[2];
				dN[2] = dN[1];
				dN[1] = inputSample;
				inputSample = (dN[1] + dN[2] + dN[3])/3.0f;
				
				allpasstemp = alpO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->aO[allpasstemp]*constallpass;
				dram->aO[alpO] = inputSample;
				inputSample *= constallpass;
				alpO--; if (alpO < 0 || alpO > delayO) {alpO = delayO;}
				inputSample += (dram->aO[alpO]);
				//allpass filter O
				
				dO[3] = dO[2];
				dO[2] = dO[1];
				dO[1] = inputSample;
				inputSample = (dO[1] + dO[2] + dO[3])/3.0f;
				
				allpasstemp = alpP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->aP[allpasstemp]*constallpass;
				dram->aP[alpP] = inputSample;
				inputSample *= constallpass;
				alpP--; if (alpP < 0 || alpP > delayP) {alpP = delayP;}
				inputSample += (dram->aP[alpP]);
				//allpass filter P
				
				dP[3] = dP[2];
				dP[2] = dP[1];
				dP[1] = inputSample;
				inputSample = (dP[1] + dP[2] + dP[3])/3.0f;
				
				allpasstemp = alpQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->aQ[allpasstemp]*constallpass;
				dram->aQ[alpQ] = inputSample;
				inputSample *= constallpass;
				alpQ--; if (alpQ < 0 || alpQ > delayQ) {alpQ = delayQ;}
				inputSample += (dram->aQ[alpQ]);
				//allpass filter Q
				
				dQ[3] = dQ[2];
				dQ[2] = dQ[1];
				dQ[1] = inputSample;
				inputSample = (dQ[1] + dQ[2] + dQ[3])/3.0f;
				
				allpasstemp = alpR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->aR[allpasstemp]*constallpass;
				dram->aR[alpR] = inputSample;
				inputSample *= constallpass;
				alpR--; if (alpR < 0 || alpR > delayR) {alpR = delayR;}
				inputSample += (dram->aR[alpR]);
				//allpass filter R
				
				dR[3] = dR[2];
				dR[2] = dR[1];
				dR[1] = inputSample;
				inputSample = (dR[1] + dR[2] + dR[3])/3.0f;
				
				allpasstemp = alpS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->aS[allpasstemp]*constallpass;
				dram->aS[alpS] = inputSample;
				inputSample *= constallpass;
				alpS--; if (alpS < 0 || alpS > delayS) {alpS = delayS;}
				inputSample += (dram->aS[alpS]);
				//allpass filter S
				
				dS[3] = dS[2];
				dS[2] = dS[1];
				dS[1] = inputSample;
				inputSample = (dS[1] + dS[2] + dS[3])/3.0f;
				
				allpasstemp = alpT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->aT[allpasstemp]*constallpass;
				dram->aT[alpT] = inputSample;
				inputSample *= constallpass;
				alpT--; if (alpT < 0 || alpT > delayT) {alpT = delayT;}
				inputSample += (dram->aT[alpT]);
				//allpass filter T
				
				dT[3] = dT[2];
				dT[2] = dT[1];
				dT[1] = inputSample;
				inputSample = (dT[1] + dT[2] + dT[3])/3.0f;
				
				allpasstemp = alpU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->aU[allpasstemp]*constallpass;
				dram->aU[alpU] = inputSample;
				inputSample *= constallpass;
				alpU--; if (alpU < 0 || alpU > delayU) {alpU = delayU;}
				inputSample += (dram->aU[alpU]);
				//allpass filter U
				
				dU[3] = dU[2];
				dU[2] = dU[1];
				dU[1] = inputSample;
				inputSample = (dU[1] + dU[2] + dU[3])/3.0f;
				
				allpasstemp = alpV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->aV[allpasstemp]*constallpass;
				dram->aV[alpV] = inputSample;
				inputSample *= constallpass;
				alpV--; if (alpV < 0 || alpV > delayV) {alpV = delayV;}
				inputSample += (dram->aV[alpV]);
				//allpass filter V
				
				dV[3] = dV[2];
				dV[2] = dV[1];
				dV[1] = inputSample;
				inputSample = (dV[1] + dV[2] + dV[3])/3.0f;
				
				allpasstemp = alpW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->aW[allpasstemp]*constallpass;
				dram->aW[alpW] = inputSample;
				inputSample *= constallpass;
				alpW--; if (alpW < 0 || alpW > delayW) {alpW = delayW;}
				inputSample += (dram->aW[alpW]);
				//allpass filter W
				
				dW[3] = dW[2];
				dW[2] = dW[1];
				dW[1] = inputSample;
				inputSample = (dW[1] + dW[2] + dW[3])/3.0f;
				
				allpasstemp = alpX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->aX[allpasstemp]*constallpass;
				dram->aX[alpX] = inputSample;
				inputSample *= constallpass;
				alpX--; if (alpX < 0 || alpX > delayX) {alpX = delayX;}
				inputSample += (dram->aX[alpX]);
				//allpass filter X
				
				dX[3] = dX[2];
				dX[2] = dX[1];
				dX[1] = inputSample;
				inputSample = (dX[1] + dX[2] + dX[3])/3.0f;
				
				allpasstemp = alpY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->aY[allpasstemp]*constallpass;
				dram->aY[alpY] = inputSample;
				inputSample *= constallpass;
				alpY--; if (alpY < 0 || alpY > delayY) {alpY = delayY;}
				inputSample += (dram->aY[alpY]);
				//allpass filter Y
				
				dY[3] = dY[2];
				dY[2] = dY[1];
				dY[1] = inputSample;
				inputSample = (dY[1] + dY[2] + dY[3])/3.0f;
				
				allpasstemp = alpZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->aZ[allpasstemp]*constallpass;
				dram->aZ[alpZ] = inputSample;
				inputSample *= constallpass;
				alpZ--; if (alpZ < 0 || alpZ > delayZ) {alpZ = delayZ;}
				inputSample += (dram->aZ[alpZ]);
				//allpass filter Z
				
				dZ[3] = dZ[2];
				dZ[2] = dZ[1];
				dZ[1] = inputSample;
				inputSample = (dZ[1] + dZ[2] + dZ[3])/3.0f;
				
				// now the second stage using the 'out' bank of allpasses		
				
				allpasstemp = outA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->oA[allpasstemp]*constallpass;
				dram->oA[outA] = inputSample;
				inputSample *= constallpass;
				outA--; if (outA < 0 || outA > delayA) {outA = delayA;}
				inputSample += (dram->oA[outA]);
				//allpass filter A		
				
				dA[6] = dA[5];
				dA[5] = dA[4];
				dA[4] = inputSample;
				inputSample = (dY[1] + dA[5] + dA[6])/3.0f;
				
				allpasstemp = outB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->oB[allpasstemp]*constallpass;
				dram->oB[outB] = inputSample;
				inputSample *= constallpass;
				outB--; if (outB < 0 || outB > delayB) {outB = delayB;}
				inputSample += (dram->oB[outB]);
				//allpass filter B
				
				dB[6] = dB[5];
				dB[5] = dB[4];
				dB[4] = inputSample;
				inputSample = (dX[1] + dB[5] + dB[6])/3.0f;
				
				allpasstemp = outC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->oC[allpasstemp]*constallpass;
				dram->oC[outC] = inputSample;
				inputSample *= constallpass;
				outC--; if (outC < 0 || outC > delayC) {outC = delayC;}
				inputSample += (dram->oC[outC]);
				//allpass filter C
				
				dC[6] = dC[5];
				dC[5] = dC[4];
				dC[4] = inputSample;
				inputSample = (dW[1] + dC[5] + dC[6])/3.0f;
				
				allpasstemp = outD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->oD[allpasstemp]*constallpass;
				dram->oD[outD] = inputSample;
				inputSample *= constallpass;
				outD--; if (outD < 0 || outD > delayD) {outD = delayD;}
				inputSample += (dram->oD[outD]);
				//allpass filter D
				
				dD[6] = dD[5];
				dD[5] = dD[4];
				dD[4] = inputSample;
				inputSample = (dV[1] + dD[5] + dD[6])/3.0f;
				
				allpasstemp = outE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->oE[allpasstemp]*constallpass;
				dram->oE[outE] = inputSample;
				inputSample *= constallpass;
				outE--; if (outE < 0 || outE > delayE) {outE = delayE;}
				inputSample += (dram->oE[outE]);
				//allpass filter E
				
				dE[6] = dE[5];
				dE[5] = dE[4];
				dE[4] = inputSample;
				inputSample = (dU[1] + dE[5] + dE[6])/3.0f;
				
				allpasstemp = outF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->oF[allpasstemp]*constallpass;
				dram->oF[outF] = inputSample;
				inputSample *= constallpass;
				outF--; if (outF < 0 || outF > delayF) {outF = delayF;}
				inputSample += (dram->oF[outF]);
				//allpass filter F
				
				dF[6] = dF[5];
				dF[5] = dF[4];
				dF[4] = inputSample;
				inputSample = (dT[1] + dF[5] + dF[6])/3.0f;
				
				allpasstemp = outG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->oG[allpasstemp]*constallpass;
				dram->oG[outG] = inputSample;
				inputSample *= constallpass;
				outG--; if (outG < 0 || outG > delayG) {outG = delayG;}
				inputSample += (dram->oG[outG]);
				//allpass filter G
				
				dG[6] = dG[5];
				dG[5] = dG[4];
				dG[4] = inputSample;
				inputSample = (dS[1] + dG[5] + dG[6])/3.0f;
				
				allpasstemp = outH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->oH[allpasstemp]*constallpass;
				dram->oH[outH] = inputSample;
				inputSample *= constallpass;
				outH--; if (outH < 0 || outH > delayH) {outH = delayH;}
				inputSample += (dram->oH[outH]);
				//allpass filter H
				
				dH[6] = dH[5];
				dH[5] = dH[4];
				dH[4] = inputSample;
				inputSample = (dR[1] + dH[5] + dH[6])/3.0f;
				
				allpasstemp = outI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->oI[allpasstemp]*constallpass;
				dram->oI[outI] = inputSample;
				inputSample *= constallpass;
				outI--; if (outI < 0 || outI > delayI) {outI = delayI;}
				inputSample += (dram->oI[outI]);
				//allpass filter I
				
				dI[6] = dI[5];
				dI[5] = dI[4];
				dI[4] = inputSample;
				inputSample = (dQ[1] + dI[5] + dI[6])/3.0f;
				
				allpasstemp = outJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->oJ[allpasstemp]*constallpass;
				dram->oJ[outJ] = inputSample;
				inputSample *= constallpass;
				outJ--; if (outJ < 0 || outJ > delayJ) {outJ = delayJ;}
				inputSample += (dram->oJ[outJ]);
				//allpass filter J
				
				dJ[6] = dJ[5];
				dJ[5] = dJ[4];
				dJ[4] = inputSample;
				inputSample = (dP[1] + dJ[5] + dJ[6])/3.0f;
				
				allpasstemp = outK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->oK[allpasstemp]*constallpass;
				dram->oK[outK] = inputSample;
				inputSample *= constallpass;
				outK--; if (outK < 0 || outK > delayK) {outK = delayK;}
				inputSample += (dram->oK[outK]);
				//allpass filter K
				
				dK[6] = dK[5];
				dK[5] = dK[4];
				dK[4] = inputSample;
				inputSample = (dO[1] + dK[5] + dK[6])/3.0f;
				
				allpasstemp = outL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->oL[allpasstemp]*constallpass;
				dram->oL[outL] = inputSample;
				inputSample *= constallpass;
				outL--; if (outL < 0 || outL > delayL) {outL = delayL;}
				inputSample += (dram->oL[outL]);
				//allpass filter L
				
				dL[6] = dL[5];
				dL[5] = dL[4];
				dL[4] = inputSample;
				inputSample = (dN[1] + dL[5] + dL[6])/3.0f;
				
				allpasstemp = outM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->oM[allpasstemp]*constallpass;
				dram->oM[outM] = inputSample;
				inputSample *= constallpass;
				outM--; if (outM < 0 || outM > delayM) {outM = delayM;}
				inputSample += (dram->oM[outM]);
				//allpass filter M
				
				dM[6] = dM[5];
				dM[5] = dM[4];
				dM[4] = inputSample;
				inputSample = (dM[1] + dM[5] + dM[6])/3.0f;
				
				allpasstemp = outN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->oN[allpasstemp]*constallpass;
				dram->oN[outN] = inputSample;
				inputSample *= constallpass;
				outN--; if (outN < 0 || outN > delayN) {outN = delayN;}
				inputSample += (dram->oN[outN]);
				//allpass filter N
				
				dN[6] = dN[5];
				dN[5] = dN[4];
				dN[4] = inputSample;
				inputSample = (dL[1] + dN[5] + dN[6])/3.0f;
				
				allpasstemp = outO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->oO[allpasstemp]*constallpass;
				dram->oO[outO] = inputSample;
				inputSample *= constallpass;
				outO--; if (outO < 0 || outO > delayO) {outO = delayO;}
				inputSample += (dram->oO[outO]);
				//allpass filter O
				
				dO[6] = dO[5];
				dO[5] = dO[4];
				dO[4] = inputSample;
				inputSample = (dK[1] + dO[5] + dO[6])/3.0f;
				
				allpasstemp = outP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->oP[allpasstemp]*constallpass;
				dram->oP[outP] = inputSample;
				inputSample *= constallpass;
				outP--; if (outP < 0 || outP > delayP) {outP = delayP;}
				inputSample += (dram->oP[outP]);
				//allpass filter P
				
				dP[6] = dP[5];
				dP[5] = dP[4];
				dP[4] = inputSample;
				inputSample = (dJ[1] + dP[5] + dP[6])/3.0f;
				
				allpasstemp = outQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->oQ[allpasstemp]*constallpass;
				dram->oQ[outQ] = inputSample;
				inputSample *= constallpass;
				outQ--; if (outQ < 0 || outQ > delayQ) {outQ = delayQ;}
				inputSample += (dram->oQ[outQ]);
				//allpass filter Q
				
				dQ[6] = dQ[5];
				dQ[5] = dQ[4];
				dQ[4] = inputSample;
				inputSample = (dI[1] + dQ[5] + dQ[6])/3.0f;
				
				allpasstemp = outR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->oR[allpasstemp]*constallpass;
				dram->oR[outR] = inputSample;
				inputSample *= constallpass;
				outR--; if (outR < 0 || outR > delayR) {outR = delayR;}
				inputSample += (dram->oR[outR]);
				//allpass filter R
				
				dR[6] = dR[5];
				dR[5] = dR[4];
				dR[4] = inputSample;
				inputSample = (dH[1] + dR[5] + dR[6])/3.0f;
				
				allpasstemp = outS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->oS[allpasstemp]*constallpass;
				dram->oS[outS] = inputSample;
				inputSample *= constallpass;
				outS--; if (outS < 0 || outS > delayS) {outS = delayS;}
				inputSample += (dram->oS[outS]);
				//allpass filter S
				
				dS[6] = dS[5];
				dS[5] = dS[4];
				dS[4] = inputSample;
				inputSample = (dG[1] + dS[5] + dS[6])/3.0f;
				
				allpasstemp = outT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->oT[allpasstemp]*constallpass;
				dram->oT[outT] = inputSample;
				inputSample *= constallpass;
				outT--; if (outT < 0 || outT > delayT) {outT = delayT;}
				inputSample += (dram->oT[outT]);
				//allpass filter T
				
				dT[6] = dT[5];
				dT[5] = dT[4];
				dT[4] = inputSample;
				inputSample = (dF[1] + dT[5] + dT[6])/3.0f;
				
				allpasstemp = outU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->oU[allpasstemp]*constallpass;
				dram->oU[outU] = inputSample;
				inputSample *= constallpass;
				outU--; if (outU < 0 || outU > delayU) {outU = delayU;}
				inputSample += (dram->oU[outU]);
				//allpass filter U
				
				dU[6] = dU[5];
				dU[5] = dU[4];
				dU[4] = inputSample;
				inputSample = (dE[1] + dU[5] + dU[6])/3.0f;
				
				allpasstemp = outV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->oV[allpasstemp]*constallpass;
				dram->oV[outV] = inputSample;
				inputSample *= constallpass;
				outV--; if (outV < 0 || outV > delayV) {outV = delayV;}
				inputSample += (dram->oV[outV]);
				//allpass filter V
				
				dV[6] = dV[5];
				dV[5] = dV[4];
				dV[4] = inputSample;
				inputSample = (dD[1] + dV[5] + dV[6])/3.0f;
				
				allpasstemp = outW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->oW[allpasstemp]*constallpass;
				dram->oW[outW] = inputSample;
				inputSample *= constallpass;
				outW--; if (outW < 0 || outW > delayW) {outW = delayW;}
				inputSample += (dram->oW[outW]);
				//allpass filter W
				
				dW[6] = dW[5];
				dW[5] = dW[4];
				dW[4] = inputSample;
				inputSample = (dC[1] + dW[5] + dW[6])/3.0f;
				
				allpasstemp = outX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->oX[allpasstemp]*constallpass;
				dram->oX[outX] = inputSample;
				inputSample *= constallpass;
				outX--; if (outX < 0 || outX > delayX) {outX = delayX;}
				inputSample += (dram->oX[outX]);
				//allpass filter X
				
				dX[6] = dX[5];
				dX[5] = dX[4];
				dX[4] = inputSample;
				inputSample = (dA[1] + dX[5] + dX[6])/3.0f;
				
				allpasstemp = outY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->oY[allpasstemp]*constallpass;
				dram->oY[outY] = inputSample;
				inputSample *= constallpass;
				outY--; if (outY < 0 || outY > delayY) {outY = delayY;}
				inputSample += (dram->oY[outY]);
				//allpass filter Y
				
				dY[6] = dY[5];
				dY[5] = dY[4];
				dY[4] = inputSample;
				inputSample = (dB[1] + dY[5] + dY[6])/3.0f;
				
				allpasstemp = outZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->oZ[allpasstemp]*constallpass;
				dram->oZ[outZ] = inputSample;
				inputSample *= constallpass;
				outZ--; if (outZ < 0 || outZ > delayZ) {outZ = delayZ;}
				inputSample += (dram->oZ[outZ]);
				//allpass filter Z
				
				dZ[6] = dZ[5];
				dZ[5] = dZ[4];
				dZ[4] = inputSample;
				inputSample = (dZ[5] + dZ[6]);
				//output Spring
				break;
				
				
case 3:	//Tiled
				allpasstemp = alpA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->aA[allpasstemp]*constallpass;
				dram->aA[alpA] = inputSample;
				inputSample *= constallpass;
				alpA--; if (alpA < 0 || alpA > delayA) {alpA = delayA;}
				inputSample += (dram->aA[alpA]);
				//allpass filter A		
				
				dA[2] = dA[1];
				dA[1] = inputSample;
				inputSample = (dA[1] + dA[2])/2.0f;
				
				allpasstemp = alpB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->aB[allpasstemp]*constallpass;
				dram->aB[alpB] = inputSample;
				inputSample *= constallpass;
				alpB--; if (alpB < 0 || alpB > delayB) {alpB = delayB;}
				inputSample += (dram->aB[alpB]);
				//allpass filter B
				
				dB[2] = dB[1];
				dB[1] = inputSample;
				inputSample = (dB[1] + dB[2])/2.0f;
				
				allpasstemp = alpC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->aC[allpasstemp]*constallpass;
				dram->aC[alpC] = inputSample;
				inputSample *= constallpass;
				alpC--; if (alpC < 0 || alpC > delayC) {alpC = delayC;}
				inputSample += (dram->aC[alpC]);
				//allpass filter C
				
				dC[2] = dC[1];
				dC[1] = inputSample;
				inputSample = (dA[1] + dC[2])/2.0f;
				
				allpasstemp = alpD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->aD[allpasstemp]*constallpass;
				dram->aD[alpD] = inputSample;
				inputSample *= constallpass;
				alpD--; if (alpD < 0 || alpD > delayD) {alpD = delayD;}
				inputSample += (dram->aD[alpD]);
				//allpass filter D
				
				dD[2] = dD[1];
				dD[1] = inputSample;
				inputSample = (dD[1] + dD[2])/2.0f;
				
				allpasstemp = alpE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->aE[allpasstemp]*constallpass;
				dram->aE[alpE] = inputSample;
				inputSample *= constallpass;
				alpE--; if (alpE < 0 || alpE > delayE) {alpE = delayE;}
				inputSample += (dram->aE[alpE]);
				//allpass filter E
				
				dE[2] = dE[1];
				dE[1] = inputSample;
				inputSample = (dA[1] + dE[2])/2.0f;
				
				allpasstemp = alpF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->aF[allpasstemp]*constallpass;
				dram->aF[alpF] = inputSample;
				inputSample *= constallpass;
				alpF--; if (alpF < 0 || alpF > delayF) {alpF = delayF;}
				inputSample += (dram->aF[alpF]);
				//allpass filter F
				
				dF[2] = dF[1];
				dF[1] = inputSample;
				inputSample = (dF[1] + dF[2])/2.0f;
				
				allpasstemp = alpG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->aG[allpasstemp]*constallpass;
				dram->aG[alpG] = inputSample;
				inputSample *= constallpass;
				alpG--; if (alpG < 0 || alpG > delayG) {alpG = delayG;}
				inputSample += (dram->aG[alpG]);
				//allpass filter G
				
				dG[2] = dG[1];
				dG[1] = inputSample;
				inputSample = (dA[1] + dG[2])/2.0f;
				
				allpasstemp = alpH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->aH[allpasstemp]*constallpass;
				dram->aH[alpH] = inputSample;
				inputSample *= constallpass;
				alpH--; if (alpH < 0 || alpH > delayH) {alpH = delayH;}
				inputSample += (dram->aH[alpH]);
				//allpass filter H
				
				dH[2] = dH[1];
				dH[1] = inputSample;
				inputSample = (dH[1] + dH[2])/2.0f;
				
				allpasstemp = alpI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->aI[allpasstemp]*constallpass;
				dram->aI[alpI] = inputSample;
				inputSample *= constallpass;
				alpI--; if (alpI < 0 || alpI > delayI) {alpI = delayI;}
				inputSample += (dram->aI[alpI]);
				//allpass filter I
				
				dI[2] = dI[1];
				dI[1] = inputSample;
				inputSample = (dI[1] + dI[2])/2.0f;
				
				allpasstemp = alpJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->aJ[allpasstemp]*constallpass;
				dram->aJ[alpJ] = inputSample;
				inputSample *= constallpass;
				alpJ--; if (alpJ < 0 || alpJ > delayJ) {alpJ = delayJ;}
				inputSample += (dram->aJ[alpJ]);
				//allpass filter J
				
				dJ[2] = dJ[1];
				dJ[1] = inputSample;
				inputSample = (dJ[1] + dJ[2])/2.0f;
				
				allpasstemp = alpK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->aK[allpasstemp]*constallpass;
				dram->aK[alpK] = inputSample;
				inputSample *= constallpass;
				alpK--; if (alpK < 0 || alpK > delayK) {alpK = delayK;}
				inputSample += (dram->aK[alpK]);
				//allpass filter K
				
				dK[2] = dK[1];
				dK[1] = inputSample;
				inputSample = (dA[1] + dK[2])/2.0f;
				
				allpasstemp = alpL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->aL[allpasstemp]*constallpass;
				dram->aL[alpL] = inputSample;
				inputSample *= constallpass;
				alpL--; if (alpL < 0 || alpL > delayL) {alpL = delayL;}
				inputSample += (dram->aL[alpL]);
				//allpass filter L
				
				dL[2] = dL[1];
				dL[1] = inputSample;
				inputSample = (dL[1] + dL[2])/2.0f;
				
				allpasstemp = alpM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->aM[allpasstemp]*constallpass;
				dram->aM[alpM] = inputSample;
				inputSample *= constallpass;
				alpM--; if (alpM < 0 || alpM > delayM) {alpM = delayM;}
				inputSample += (dram->aM[alpM]);
				//allpass filter M
				
				dM[2] = dM[1];
				dM[1] = inputSample;
				inputSample = (dA[1] + dM[2])/2.0f;
				
				allpasstemp = alpN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->aN[allpasstemp]*constallpass;
				dram->aN[alpN] = inputSample;
				inputSample *= constallpass;
				alpN--; if (alpN < 0 || alpN > delayN) {alpN = delayN;}
				inputSample += (dram->aN[alpN]);
				//allpass filter N
				
				dN[2] = dN[1];
				dN[1] = inputSample;
				inputSample = (dN[1] + dN[2])/2.0f;
				
				allpasstemp = alpO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->aO[allpasstemp]*constallpass;
				dram->aO[alpO] = inputSample;
				inputSample *= constallpass;
				alpO--; if (alpO < 0 || alpO > delayO) {alpO = delayO;}
				inputSample += (dram->aO[alpO]);
				//allpass filter O
				
				dO[2] = dO[1];
				dO[1] = inputSample;
				inputSample = (dO[1] + dO[2])/2.0f;
				
				allpasstemp = alpP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->aP[allpasstemp]*constallpass;
				dram->aP[alpP] = inputSample;
				inputSample *= constallpass;
				alpP--; if (alpP < 0 || alpP > delayP) {alpP = delayP;}
				inputSample += (dram->aP[alpP]);
				//allpass filter P
				
				dP[2] = dP[1];
				dP[1] = inputSample;
				inputSample = (dP[1] + dP[2])/2.0f;
				
				allpasstemp = alpQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->aQ[allpasstemp]*constallpass;
				dram->aQ[alpQ] = inputSample;
				inputSample *= constallpass;
				alpQ--; if (alpQ < 0 || alpQ > delayQ) {alpQ = delayQ;}
				inputSample += (dram->aQ[alpQ]);
				//allpass filter Q
				
				dQ[2] = dQ[1];
				dQ[1] = inputSample;
				inputSample = (dA[1] + dQ[2])/2.0f;
				
				allpasstemp = alpR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->aR[allpasstemp]*constallpass;
				dram->aR[alpR] = inputSample;
				inputSample *= constallpass;
				alpR--; if (alpR < 0 || alpR > delayR) {alpR = delayR;}
				inputSample += (dram->aR[alpR]);
				//allpass filter R
				
				dR[2] = dR[1];
				dR[1] = inputSample;
				inputSample = (dR[1] + dR[2])/2.0f;
				
				allpasstemp = alpS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->aS[allpasstemp]*constallpass;
				dram->aS[alpS] = inputSample;
				inputSample *= constallpass;
				alpS--; if (alpS < 0 || alpS > delayS) {alpS = delayS;}
				inputSample += (dram->aS[alpS]);
				//allpass filter S
				
				dS[2] = dS[1];
				dS[1] = inputSample;
				inputSample = (dA[1] + dS[2])/2.0f;
				
				allpasstemp = alpT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->aT[allpasstemp]*constallpass;
				dram->aT[alpT] = inputSample;
				inputSample *= constallpass;
				alpT--; if (alpT < 0 || alpT > delayT) {alpT = delayT;}
				inputSample += (dram->aT[alpT]);
				//allpass filter T
				
				dT[2] = dT[1];
				dT[1] = inputSample;
				inputSample = (dT[1] + dT[2])/2.0f;
				
				allpasstemp = alpU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->aU[allpasstemp]*constallpass;
				dram->aU[alpU] = inputSample;
				inputSample *= constallpass;
				alpU--; if (alpU < 0 || alpU > delayU) {alpU = delayU;}
				inputSample += (dram->aU[alpU]);
				//allpass filter U
				
				dU[2] = dU[1];
				dU[1] = inputSample;
				inputSample = (dU[1] + dU[2])/2.0f;
				
				allpasstemp = alpV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->aV[allpasstemp]*constallpass;
				dram->aV[alpV] = inputSample;
				inputSample *= constallpass;
				alpV--; if (alpV < 0 || alpV > delayV) {alpV = delayV;}
				inputSample += (dram->aV[alpV]);
				//allpass filter V
				
				dV[2] = dV[1];
				dV[1] = inputSample;
				inputSample = (dV[1] + dV[2])/2.0f;
				
				allpasstemp = alpW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->aW[allpasstemp]*constallpass;
				dram->aW[alpW] = inputSample;
				inputSample *= constallpass;
				alpW--; if (alpW < 0 || alpW > delayW) {alpW = delayW;}
				inputSample += (dram->aW[alpW]);
				//allpass filter W
				
				dW[2] = dW[1];
				dW[1] = inputSample;
				inputSample = (dA[1] + dW[2])/2.0f;
				
				allpasstemp = alpX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->aX[allpasstemp]*constallpass;
				dram->aX[alpX] = inputSample;
				inputSample *= constallpass;
				alpX--; if (alpX < 0 || alpX > delayX) {alpX = delayX;}
				inputSample += (dram->aX[alpX]);
				//allpass filter X
				
				dX[2] = dX[1];
				dX[1] = inputSample;
				inputSample = (dX[1] + dX[2])/2.0f;
				
				allpasstemp = alpY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->aY[allpasstemp]*constallpass;
				dram->aY[alpY] = inputSample;
				inputSample *= constallpass;
				alpY--; if (alpY < 0 || alpY > delayY) {alpY = delayY;}
				inputSample += (dram->aY[alpY]);
				//allpass filter Y
				
				dY[2] = dY[1];
				dY[1] = inputSample;
				inputSample = (dY[1] + dY[2])/2.0f;
				
				allpasstemp = alpZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->aZ[allpasstemp]*constallpass;
				dram->aZ[alpZ] = inputSample;
				inputSample *= constallpass;
				alpZ--; if (alpZ < 0 || alpZ > delayZ) {alpZ = delayZ;}
				inputSample += (dram->aZ[alpZ]);
				//allpass filter Z
				
				dZ[2] = dZ[1];
				dZ[1] = inputSample;
				inputSample = (dZ[1] + dZ[2])/2.0f;
				
				// now the second stage using the 'out' bank of allpasses		
				
				allpasstemp = outA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->oA[allpasstemp]*constallpass;
				dram->oA[outA] = inputSample;
				inputSample *= constallpass;
				outA--; if (outA < 0 || outA > delayA) {outA = delayA;}
				inputSample += (dram->oA[outA]);
				//allpass filter A		
				
				dA[5] = dA[4];
				dA[4] = inputSample;
				inputSample = (dA[4] + dA[5])/2.0f;
				
				allpasstemp = outB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->oB[allpasstemp]*constallpass;
				dram->oB[outB] = inputSample;
				inputSample *= constallpass;
				outB--; if (outB < 0 || outB > delayB) {outB = delayB;}
				inputSample += (dram->oB[outB]);
				//allpass filter B
				
				dB[5] = dB[4];
				dB[4] = inputSample;
				inputSample = (dB[4] + dB[5])/2.0f;
				
				allpasstemp = outC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->oC[allpasstemp]*constallpass;
				dram->oC[outC] = inputSample;
				inputSample *= constallpass;
				outC--; if (outC < 0 || outC > delayC) {outC = delayC;}
				inputSample += (dram->oC[outC]);
				//allpass filter C
				
				dC[5] = dC[4];
				dC[4] = inputSample;
				inputSample = (dA[1] + dC[5])/2.0f;
				
				allpasstemp = outD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->oD[allpasstemp]*constallpass;
				dram->oD[outD] = inputSample;
				inputSample *= constallpass;
				outD--; if (outD < 0 || outD > delayD) {outD = delayD;}
				inputSample += (dram->oD[outD]);
				//allpass filter D
				
				dD[5] = dD[4];
				dD[4] = inputSample;
				inputSample = (dD[4] + dD[5])/2.0f;
				
				allpasstemp = outE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->oE[allpasstemp]*constallpass;
				dram->oE[outE] = inputSample;
				inputSample *= constallpass;
				outE--; if (outE < 0 || outE > delayE) {outE = delayE;}
				inputSample += (dram->oE[outE]);
				//allpass filter E
				
				dE[5] = dE[4];
				dE[4] = inputSample;
				inputSample = (dA[1] + dE[5])/2.0f;
				
				allpasstemp = outF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->oF[allpasstemp]*constallpass;
				dram->oF[outF] = inputSample;
				inputSample *= constallpass;
				outF--; if (outF < 0 || outF > delayF) {outF = delayF;}
				inputSample += (dram->oF[outF]);
				//allpass filter F
				
				dF[5] = dF[4];
				dF[4] = inputSample;
				inputSample = (dF[4] + dF[5])/2.0f;
				
				allpasstemp = outG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->oG[allpasstemp]*constallpass;
				dram->oG[outG] = inputSample;
				inputSample *= constallpass;
				outG--; if (outG < 0 || outG > delayG) {outG = delayG;}
				inputSample += (dram->oG[outG]);
				//allpass filter G
				
				dG[5] = dG[4];
				dG[4] = inputSample;
				inputSample = (dG[4] + dG[5])/2.0f;
				
				allpasstemp = outH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->oH[allpasstemp]*constallpass;
				dram->oH[outH] = inputSample;
				inputSample *= constallpass;
				outH--; if (outH < 0 || outH > delayH) {outH = delayH;}
				inputSample += (dram->oH[outH]);
				//allpass filter H
				
				dH[5] = dH[4];
				dH[4] = inputSample;
				inputSample = (dH[4] + dH[5])/2.0f;
				
				allpasstemp = outI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->oI[allpasstemp]*constallpass;
				dram->oI[outI] = inputSample;
				inputSample *= constallpass;
				outI--; if (outI < 0 || outI > delayI) {outI = delayI;}
				inputSample += (dram->oI[outI]);
				//allpass filter I
				
				dI[5] = dI[4];
				dI[4] = inputSample;
				inputSample = (dI[4] + dI[5])/2.0f;
				
				allpasstemp = outJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->oJ[allpasstemp]*constallpass;
				dram->oJ[outJ] = inputSample;
				inputSample *= constallpass;
				outJ--; if (outJ < 0 || outJ > delayJ) {outJ = delayJ;}
				inputSample += (dram->oJ[outJ]);
				//allpass filter J
				
				dJ[5] = dJ[4];
				dJ[4] = inputSample;
				inputSample = (dJ[4] + dJ[5])/2.0f;
				
				allpasstemp = outK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->oK[allpasstemp]*constallpass;
				dram->oK[outK] = inputSample;
				inputSample *= constallpass;
				outK--; if (outK < 0 || outK > delayK) {outK = delayK;}
				inputSample += (dram->oK[outK]);
				//allpass filter K
				
				dK[5] = dK[4];
				dK[4] = inputSample;
				inputSample = (dA[1] + dK[5])/2.0f;
				
				allpasstemp = outL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->oL[allpasstemp]*constallpass;
				dram->oL[outL] = inputSample;
				inputSample *= constallpass;
				outL--; if (outL < 0 || outL > delayL) {outL = delayL;}
				inputSample += (dram->oL[outL]);
				//allpass filter L
				
				dL[5] = dL[4];
				dL[4] = inputSample;
				inputSample = (dL[4] + dL[5])/2.0f;
				
				allpasstemp = outM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->oM[allpasstemp]*constallpass;
				dram->oM[outM] = inputSample;
				inputSample *= constallpass;
				outM--; if (outM < 0 || outM > delayM) {outM = delayM;}
				inputSample += (dram->oM[outM]);
				//allpass filter M
				
				dM[5] = dM[4];
				dM[4] = inputSample;
				inputSample = (dM[4] + dM[5])/2.0f;
				
				allpasstemp = outN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->oN[allpasstemp]*constallpass;
				dram->oN[outN] = inputSample;
				inputSample *= constallpass;
				outN--; if (outN < 0 || outN > delayN) {outN = delayN;}
				inputSample += (dram->oN[outN]);
				//allpass filter N
				
				dN[5] = dN[4];
				dN[4] = inputSample;
				inputSample = (dN[4] + dN[5])/2.0f;
				
				allpasstemp = outO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->oO[allpasstemp]*constallpass;
				dram->oO[outO] = inputSample;
				inputSample *= constallpass;
				outO--; if (outO < 0 || outO > delayO) {outO = delayO;}
				inputSample += (dram->oO[outO]);
				//allpass filter O
				
				dO[5] = dO[4];
				dO[4] = inputSample;
				inputSample = (dA[1] + dO[5])/2.0f;
				
				allpasstemp = outP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->oP[allpasstemp]*constallpass;
				dram->oP[outP] = inputSample;
				inputSample *= constallpass;
				outP--; if (outP < 0 || outP > delayP) {outP = delayP;}
				inputSample += (dram->oP[outP]);
				//allpass filter P
				
				dP[5] = dP[4];
				dP[4] = inputSample;
				inputSample = (dP[4] + dP[5])/2.0f;
				
				allpasstemp = outQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->oQ[allpasstemp]*constallpass;
				dram->oQ[outQ] = inputSample;
				inputSample *= constallpass;
				outQ--; if (outQ < 0 || outQ > delayQ) {outQ = delayQ;}
				inputSample += (dram->oQ[outQ]);
				//allpass filter Q
				
				dQ[5] = dQ[4];
				dQ[4] = inputSample;
				inputSample = (dA[1] + dQ[5])/2.0f;
				
				allpasstemp = outR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->oR[allpasstemp]*constallpass;
				dram->oR[outR] = inputSample;
				inputSample *= constallpass;
				outR--; if (outR < 0 || outR > delayR) {outR = delayR;}
				inputSample += (dram->oR[outR]);
				//allpass filter R
				
				dR[5] = dR[4];
				dR[4] = inputSample;
				inputSample = (dR[4] + dR[5])/2.0f;
				
				allpasstemp = outS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->oS[allpasstemp]*constallpass;
				dram->oS[outS] = inputSample;
				inputSample *= constallpass;
				outS--; if (outS < 0 || outS > delayS) {outS = delayS;}
				inputSample += (dram->oS[outS]);
				//allpass filter S
				
				dS[5] = dS[4];
				dS[4] = inputSample;
				inputSample = (dS[4] + dS[5])/2.0f;
				
				allpasstemp = outT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->oT[allpasstemp]*constallpass;
				dram->oT[outT] = inputSample;
				inputSample *= constallpass;
				outT--; if (outT < 0 || outT > delayT) {outT = delayT;}
				inputSample += (dram->oT[outT]);
				//allpass filter T
				
				dT[5] = dT[4];
				dT[4] = inputSample;
				inputSample = (dT[4] + dT[5])/2.0f;
				
				allpasstemp = outU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->oU[allpasstemp]*constallpass;
				dram->oU[outU] = inputSample;
				inputSample *= constallpass;
				outU--; if (outU < 0 || outU > delayU) {outU = delayU;}
				inputSample += (dram->oU[outU]);
				//allpass filter U
				
				dU[5] = dU[4];
				dU[4] = inputSample;
				inputSample = (dA[1] + dU[5])/2.0f;
				
				allpasstemp = outV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->oV[allpasstemp]*constallpass;
				dram->oV[outV] = inputSample;
				inputSample *= constallpass;
				outV--; if (outV < 0 || outV > delayV) {outV = delayV;}
				inputSample += (dram->oV[outV]);
				//allpass filter V
				
				dV[5] = dV[4];
				dV[4] = inputSample;
				inputSample = (dV[4] + dV[5])/2.0f;
				
				allpasstemp = outW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->oW[allpasstemp]*constallpass;
				dram->oW[outW] = inputSample;
				inputSample *= constallpass;
				outW--; if (outW < 0 || outW > delayW) {outW = delayW;}
				inputSample += (dram->oW[outW]);
				//allpass filter W
				
				dW[5] = dW[4];
				dW[4] = inputSample;
				inputSample = (dW[4] + dW[5])/2.0f;
				
				allpasstemp = outX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->oX[allpasstemp]*constallpass;
				dram->oX[outX] = inputSample;
				inputSample *= constallpass;
				outX--; if (outX < 0 || outX > delayX) {outX = delayX;}
				inputSample += (dram->oX[outX]);
				//allpass filter X
				
				dX[5] = dX[4];
				dX[4] = inputSample;
				inputSample = (dX[4] + dX[5])/2.0f;
				
				allpasstemp = outY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->oY[allpasstemp]*constallpass;
				dram->oY[outY] = inputSample;
				inputSample *= constallpass;
				outY--; if (outY < 0 || outY > delayY) {outY = delayY;}
				inputSample += (dram->oY[outY]);
				//allpass filter Y
				
				dY[5] = dY[4];
				dY[4] = inputSample;
				inputSample = (dY[4] + dY[5])/2.0f;
				
				allpasstemp = outZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->oZ[allpasstemp]*constallpass;
				dram->oZ[outZ] = inputSample;
				inputSample *= constallpass;
				outZ--; if (outZ < 0 || outZ > delayZ) {outZ = delayZ;}
				inputSample += (dram->oZ[outZ]);
				//allpass filter Z
				
				dZ[5] = dZ[4];
				dZ[4] = inputSample;
				inputSample = (dZ[4] + dZ[5]);		
				//output Tiled
				break;
				

case 4://Room
				allpasstemp = alpA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->aA[allpasstemp]*constallpass;
				dram->aA[alpA] = inputSample;
				inputSample *= constallpass;
				alpA--; if (alpA < 0 || alpA > delayA) {alpA = delayA;}
				inputSample += (dram->aA[alpA]);
				//allpass filter A		
				
				dA[2] = dA[1];
				dA[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->aB[allpasstemp]*constallpass;
				dram->aB[alpB] = inputSample;
				inputSample *= constallpass;
				alpB--; if (alpB < 0 || alpB > delayB) {alpB = delayB;}
				inputSample += (dram->aB[alpB]);
				//allpass filter B
				
				dB[2] = dB[1];
				dB[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->aC[allpasstemp]*constallpass;
				dram->aC[alpC] = inputSample;
				inputSample *= constallpass;
				alpC--; if (alpC < 0 || alpC > delayC) {alpC = delayC;}
				inputSample += (dram->aC[alpC]);
				//allpass filter C
				
				dC[2] = dC[1];
				dC[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->aD[allpasstemp]*constallpass;
				dram->aD[alpD] = inputSample;
				inputSample *= constallpass;
				alpD--; if (alpD < 0 || alpD > delayD) {alpD = delayD;}
				inputSample += (dram->aD[alpD]);
				//allpass filter D
				
				dD[2] = dD[1];
				dD[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->aE[allpasstemp]*constallpass;
				dram->aE[alpE] = inputSample;
				inputSample *= constallpass;
				alpE--; if (alpE < 0 || alpE > delayE) {alpE = delayE;}
				inputSample += (dram->aE[alpE]);
				//allpass filter E
				
				dE[2] = dE[1];
				dE[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->aF[allpasstemp]*constallpass;
				dram->aF[alpF] = inputSample;
				inputSample *= constallpass;
				alpF--; if (alpF < 0 || alpF > delayF) {alpF = delayF;}
				inputSample += (dram->aF[alpF]);
				//allpass filter F
				
				dF[2] = dF[1];
				dF[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->aG[allpasstemp]*constallpass;
				dram->aG[alpG] = inputSample;
				inputSample *= constallpass;
				alpG--; if (alpG < 0 || alpG > delayG) {alpG = delayG;}
				inputSample += (dram->aG[alpG]);
				//allpass filter G
				
				dG[2] = dG[1];
				dG[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->aH[allpasstemp]*constallpass;
				dram->aH[alpH] = inputSample;
				inputSample *= constallpass;
				alpH--; if (alpH < 0 || alpH > delayH) {alpH = delayH;}
				inputSample += (dram->aH[alpH]);
				//allpass filter H
				
				dH[2] = dH[1];
				dH[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->aI[allpasstemp]*constallpass;
				dram->aI[alpI] = inputSample;
				inputSample *= constallpass;
				alpI--; if (alpI < 0 || alpI > delayI) {alpI = delayI;}
				inputSample += (dram->aI[alpI]);
				//allpass filter I
				
				dI[2] = dI[1];
				dI[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->aJ[allpasstemp]*constallpass;
				dram->aJ[alpJ] = inputSample;
				inputSample *= constallpass;
				alpJ--; if (alpJ < 0 || alpJ > delayJ) {alpJ = delayJ;}
				inputSample += (dram->aJ[alpJ]);
				//allpass filter J
				
				dJ[2] = dJ[1];
				dJ[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->aK[allpasstemp]*constallpass;
				dram->aK[alpK] = inputSample;
				inputSample *= constallpass;
				alpK--; if (alpK < 0 || alpK > delayK) {alpK = delayK;}
				inputSample += (dram->aK[alpK]);
				//allpass filter K
				
				dK[2] = dK[1];
				dK[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->aL[allpasstemp]*constallpass;
				dram->aL[alpL] = inputSample;
				inputSample *= constallpass;
				alpL--; if (alpL < 0 || alpL > delayL) {alpL = delayL;}
				inputSample += (dram->aL[alpL]);
				//allpass filter L
				
				dL[2] = dL[1];
				dL[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->aM[allpasstemp]*constallpass;
				dram->aM[alpM] = inputSample;
				inputSample *= constallpass;
				alpM--; if (alpM < 0 || alpM > delayM) {alpM = delayM;}
				inputSample += (dram->aM[alpM]);
				//allpass filter M
				
				dM[2] = dM[1];
				dM[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->aN[allpasstemp]*constallpass;
				dram->aN[alpN] = inputSample;
				inputSample *= constallpass;
				alpN--; if (alpN < 0 || alpN > delayN) {alpN = delayN;}
				inputSample += (dram->aN[alpN]);
				//allpass filter N
				
				dN[2] = dN[1];
				dN[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->aO[allpasstemp]*constallpass;
				dram->aO[alpO] = inputSample;
				inputSample *= constallpass;
				alpO--; if (alpO < 0 || alpO > delayO) {alpO = delayO;}
				inputSample += (dram->aO[alpO]);
				//allpass filter O
				
				dO[2] = dO[1];
				dO[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->aP[allpasstemp]*constallpass;
				dram->aP[alpP] = inputSample;
				inputSample *= constallpass;
				alpP--; if (alpP < 0 || alpP > delayP) {alpP = delayP;}
				inputSample += (dram->aP[alpP]);
				//allpass filter P
				
				dP[2] = dP[1];
				dP[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->aQ[allpasstemp]*constallpass;
				dram->aQ[alpQ] = inputSample;
				inputSample *= constallpass;
				alpQ--; if (alpQ < 0 || alpQ > delayQ) {alpQ = delayQ;}
				inputSample += (dram->aQ[alpQ]);
				//allpass filter Q
				
				dQ[2] = dQ[1];
				dQ[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->aR[allpasstemp]*constallpass;
				dram->aR[alpR] = inputSample;
				inputSample *= constallpass;
				alpR--; if (alpR < 0 || alpR > delayR) {alpR = delayR;}
				inputSample += (dram->aR[alpR]);
				//allpass filter R
				
				dR[2] = dR[1];
				dR[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->aS[allpasstemp]*constallpass;
				dram->aS[alpS] = inputSample;
				inputSample *= constallpass;
				alpS--; if (alpS < 0 || alpS > delayS) {alpS = delayS;}
				inputSample += (dram->aS[alpS]);
				//allpass filter S
				
				dS[2] = dS[1];
				dS[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->aT[allpasstemp]*constallpass;
				dram->aT[alpT] = inputSample;
				inputSample *= constallpass;
				alpT--; if (alpT < 0 || alpT > delayT) {alpT = delayT;}
				inputSample += (dram->aT[alpT]);
				//allpass filter T
				
				dT[2] = dT[1];
				dT[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->aU[allpasstemp]*constallpass;
				dram->aU[alpU] = inputSample;
				inputSample *= constallpass;
				alpU--; if (alpU < 0 || alpU > delayU) {alpU = delayU;}
				inputSample += (dram->aU[alpU]);
				//allpass filter U
				
				dU[2] = dU[1];
				dU[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->aV[allpasstemp]*constallpass;
				dram->aV[alpV] = inputSample;
				inputSample *= constallpass;
				alpV--; if (alpV < 0 || alpV > delayV) {alpV = delayV;}
				inputSample += (dram->aV[alpV]);
				//allpass filter V
				
				dV[2] = dV[1];
				dV[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->aW[allpasstemp]*constallpass;
				dram->aW[alpW] = inputSample;
				inputSample *= constallpass;
				alpW--; if (alpW < 0 || alpW > delayW) {alpW = delayW;}
				inputSample += (dram->aW[alpW]);
				//allpass filter W
				
				dW[2] = dW[1];
				dW[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->aX[allpasstemp]*constallpass;
				dram->aX[alpX] = inputSample;
				inputSample *= constallpass;
				alpX--; if (alpX < 0 || alpX > delayX) {alpX = delayX;}
				inputSample += (dram->aX[alpX]);
				//allpass filter X
				
				dX[2] = dX[1];
				dX[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->aY[allpasstemp]*constallpass;
				dram->aY[alpY] = inputSample;
				inputSample *= constallpass;
				alpY--; if (alpY < 0 || alpY > delayY) {alpY = delayY;}
				inputSample += (dram->aY[alpY]);
				//allpass filter Y
				
				dY[2] = dY[1];
				dY[1] = inputSample;
				inputSample = drySample;
				
				allpasstemp = alpZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->aZ[allpasstemp]*constallpass;
				dram->aZ[alpZ] = inputSample;
				inputSample *= constallpass;
				alpZ--; if (alpZ < 0 || alpZ > delayZ) {alpZ = delayZ;}
				inputSample += (dram->aZ[alpZ]);
				//allpass filter Z
				
				dZ[2] = dZ[1];
				dZ[1] = inputSample;
				inputSample = drySample;
				
				// now the second stage using the 'out' bank of allpasses		
				
				allpasstemp = outA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->oA[allpasstemp]*constallpass;
				dram->oA[outA] = inputSample;
				inputSample *= constallpass;
				outA--; if (outA < 0 || outA > delayA) {outA = delayA;}
				inputSample += (dram->oA[outA]);
				//allpass filter A		
				
				dA[5] = dA[4];
				dA[4] = inputSample;
				inputSample = (dA[1]+dA[2])/2.0f;
				
				allpasstemp = outB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->oB[allpasstemp]*constallpass;
				dram->oB[outB] = inputSample;
				inputSample *= constallpass;
				outB--; if (outB < 0 || outB > delayB) {outB = delayB;}
				inputSample += (dram->oB[outB]);
				//allpass filter B
				
				dB[5] = dB[4];
				dB[4] = inputSample;
				inputSample = (dB[1]+dB[2])/2.0f;
				
				allpasstemp = outC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->oC[allpasstemp]*constallpass;
				dram->oC[outC] = inputSample;
				inputSample *= constallpass;
				outC--; if (outC < 0 || outC > delayC) {outC = delayC;}
				inputSample += (dram->oC[outC]);
				//allpass filter C
				
				dC[5] = dC[4];
				dC[4] = inputSample;
				inputSample = (dC[1]+dC[2])/2.0f;
				
				allpasstemp = outD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->oD[allpasstemp]*constallpass;
				dram->oD[outD] = inputSample;
				inputSample *= constallpass;
				outD--; if (outD < 0 || outD > delayD) {outD = delayD;}
				inputSample += (dram->oD[outD]);
				//allpass filter D
				
				dD[5] = dD[4];
				dD[4] = inputSample;
				inputSample = (dD[1]+dD[2])/2.0f;
				
				allpasstemp = outE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->oE[allpasstemp]*constallpass;
				dram->oE[outE] = inputSample;
				inputSample *= constallpass;
				outE--; if (outE < 0 || outE > delayE) {outE = delayE;}
				inputSample += (dram->oE[outE]);
				//allpass filter E
				
				dE[5] = dE[4];
				dE[4] = inputSample;
				inputSample = (dE[1]+dE[2])/2.0f;
				
				allpasstemp = outF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->oF[allpasstemp]*constallpass;
				dram->oF[outF] = inputSample;
				inputSample *= constallpass;
				outF--; if (outF < 0 || outF > delayF) {outF = delayF;}
				inputSample += (dram->oF[outF]);
				//allpass filter F
				
				dF[5] = dF[4];
				dF[4] = inputSample;
				inputSample = (dF[1]+dF[2])/2.0f;
				
				allpasstemp = outG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->oG[allpasstemp]*constallpass;
				dram->oG[outG] = inputSample;
				inputSample *= constallpass;
				outG--; if (outG < 0 || outG > delayG) {outG = delayG;}
				inputSample += (dram->oG[outG]);
				//allpass filter G
				
				dG[5] = dG[4];
				dG[4] = inputSample;
				inputSample = (dG[1]+dG[2])/2.0f;
				
				allpasstemp = outH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->oH[allpasstemp]*constallpass;
				dram->oH[outH] = inputSample;
				inputSample *= constallpass;
				outH--; if (outH < 0 || outH > delayH) {outH = delayH;}
				inputSample += (dram->oH[outH]);
				//allpass filter H
				
				dH[5] = dH[4];
				dH[4] = inputSample;
				inputSample = (dH[1]+dH[2])/2.0f;
				
				allpasstemp = outI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->oI[allpasstemp]*constallpass;
				dram->oI[outI] = inputSample;
				inputSample *= constallpass;
				outI--; if (outI < 0 || outI > delayI) {outI = delayI;}
				inputSample += (dram->oI[outI]);
				//allpass filter I
				
				dI[5] = dI[4];
				dI[4] = inputSample;
				inputSample = (dI[1]+dI[2])/2.0f;
				
				allpasstemp = outJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->oJ[allpasstemp]*constallpass;
				dram->oJ[outJ] = inputSample;
				inputSample *= constallpass;
				outJ--; if (outJ < 0 || outJ > delayJ) {outJ = delayJ;}
				inputSample += (dram->oJ[outJ]);
				//allpass filter J
				
				dJ[5] = dJ[4];
				dJ[4] = inputSample;
				inputSample = (dJ[1]+dJ[2])/2.0f;
				
				allpasstemp = outK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->oK[allpasstemp]*constallpass;
				dram->oK[outK] = inputSample;
				inputSample *= constallpass;
				outK--; if (outK < 0 || outK > delayK) {outK = delayK;}
				inputSample += (dram->oK[outK]);
				//allpass filter K
				
				dK[5] = dK[4];
				dK[4] = inputSample;
				inputSample = (dK[1]+dK[2])/2.0f;
				
				allpasstemp = outL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->oL[allpasstemp]*constallpass;
				dram->oL[outL] = inputSample;
				inputSample *= constallpass;
				outL--; if (outL < 0 || outL > delayL) {outL = delayL;}
				inputSample += (dram->oL[outL]);
				//allpass filter L
				
				dL[5] = dL[4];
				dL[4] = inputSample;
				inputSample = (dL[1]+dL[2])/2.0f;
				
				allpasstemp = outM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->oM[allpasstemp]*constallpass;
				dram->oM[outM] = inputSample;
				inputSample *= constallpass;
				outM--; if (outM < 0 || outM > delayM) {outM = delayM;}
				inputSample += (dram->oM[outM]);
				//allpass filter M
				
				dM[5] = dM[4];
				dM[4] = inputSample;
				inputSample = (dM[1]+dM[2])/2.0f;
				
				allpasstemp = outN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->oN[allpasstemp]*constallpass;
				dram->oN[outN] = inputSample;
				inputSample *= constallpass;
				outN--; if (outN < 0 || outN > delayN) {outN = delayN;}
				inputSample += (dram->oN[outN]);
				//allpass filter N
				
				dN[5] = dN[4];
				dN[4] = inputSample;
				inputSample = (dN[1]+dN[2])/2.0f;
				
				allpasstemp = outO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->oO[allpasstemp]*constallpass;
				dram->oO[outO] = inputSample;
				inputSample *= constallpass;
				outO--; if (outO < 0 || outO > delayO) {outO = delayO;}
				inputSample += (dram->oO[outO]);
				//allpass filter O
				
				dO[5] = dO[4];
				dO[4] = inputSample;
				inputSample = (dO[1]+dO[2])/2.0f;
				
				allpasstemp = outP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->oP[allpasstemp]*constallpass;
				dram->oP[outP] = inputSample;
				inputSample *= constallpass;
				outP--; if (outP < 0 || outP > delayP) {outP = delayP;}
				inputSample += (dram->oP[outP]);
				//allpass filter P
				
				dP[5] = dP[4];
				dP[4] = inputSample;
				inputSample = (dP[1]+dP[2])/2.0f;
				
				allpasstemp = outQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->oQ[allpasstemp]*constallpass;
				dram->oQ[outQ] = inputSample;
				inputSample *= constallpass;
				outQ--; if (outQ < 0 || outQ > delayQ) {outQ = delayQ;}
				inputSample += (dram->oQ[outQ]);
				//allpass filter Q
				
				dQ[5] = dQ[4];
				dQ[4] = inputSample;
				inputSample = (dQ[1]+dQ[2])/2.0f;
				
				allpasstemp = outR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->oR[allpasstemp]*constallpass;
				dram->oR[outR] = inputSample;
				inputSample *= constallpass;
				outR--; if (outR < 0 || outR > delayR) {outR = delayR;}
				inputSample += (dram->oR[outR]);
				//allpass filter R
				
				dR[5] = dR[4];
				dR[4] = inputSample;
				inputSample = (dR[1]+dR[2])/2.0f;
				
				allpasstemp = outS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->oS[allpasstemp]*constallpass;
				dram->oS[outS] = inputSample;
				inputSample *= constallpass;
				outS--; if (outS < 0 || outS > delayS) {outS = delayS;}
				inputSample += (dram->oS[outS]);
				//allpass filter S
				
				dS[5] = dS[4];
				dS[4] = inputSample;
				inputSample = (dS[1]+dS[2])/2.0f;
				
				allpasstemp = outT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->oT[allpasstemp]*constallpass;
				dram->oT[outT] = inputSample;
				inputSample *= constallpass;
				outT--; if (outT < 0 || outT > delayT) {outT = delayT;}
				inputSample += (dram->oT[outT]);
				//allpass filter T
				
				dT[5] = dT[4];
				dT[4] = inputSample;
				inputSample = (dT[1]+dT[2])/2.0f;
				
				allpasstemp = outU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->oU[allpasstemp]*constallpass;
				dram->oU[outU] = inputSample;
				inputSample *= constallpass;
				outU--; if (outU < 0 || outU > delayU) {outU = delayU;}
				inputSample += (dram->oU[outU]);
				//allpass filter U
				
				dU[5] = dU[4];
				dU[4] = inputSample;
				inputSample = (dU[1]+dU[2])/2.0f;
				
				allpasstemp = outV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->oV[allpasstemp]*constallpass;
				dram->oV[outV] = inputSample;
				inputSample *= constallpass;
				outV--; if (outV < 0 || outV > delayV) {outV = delayV;}
				inputSample += (dram->oV[outV]);
				//allpass filter V
				
				dV[5] = dV[4];
				dV[4] = inputSample;
				inputSample = (dV[1]+dV[2])/2.0f;
				
				allpasstemp = outW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->oW[allpasstemp]*constallpass;
				dram->oW[outW] = inputSample;
				inputSample *= constallpass;
				outW--; if (outW < 0 || outW > delayW) {outW = delayW;}
				inputSample += (dram->oW[outW]);
				//allpass filter W
				
				dW[5] = dW[4];
				dW[4] = inputSample;
				inputSample = (dW[1]+dW[2])/2.0f;
				
				allpasstemp = outX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->oX[allpasstemp]*constallpass;
				dram->oX[outX] = inputSample;
				inputSample *= constallpass;
				outX--; if (outX < 0 || outX > delayX) {outX = delayX;}
				inputSample += (dram->oX[outX]);
				//allpass filter X
				
				dX[5] = dX[4];
				dX[4] = inputSample;
				inputSample = (dX[1]+dX[2])/2.0f;
				
				allpasstemp = outY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->oY[allpasstemp]*constallpass;
				dram->oY[outY] = inputSample;
				inputSample *= constallpass;
				outY--; if (outY < 0 || outY > delayY) {outY = delayY;}
				inputSample += (dram->oY[outY]);
				//allpass filter Y
				
				dY[5] = dY[4];
				dY[4] = inputSample;
				inputSample = (dY[1]+dY[2])/2.0f;
				
				allpasstemp = outZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->oZ[allpasstemp]*constallpass;
				dram->oZ[outZ] = inputSample;
				inputSample *= constallpass;
				outZ--; if (outZ < 0 || outZ > delayZ) {outZ = delayZ;}
				inputSample += (dram->oZ[outZ]);
				//allpass filter Z
				
				dZ[5] = dZ[4];
				dZ[4] = inputSample;
				inputSample = (dB[4] * dryness);		
				inputSample += (dC[4] * dryness);		
				inputSample += dD[4];		
				inputSample += dE[4];		
				inputSample += dF[4];		
				inputSample += dG[4];		
				inputSample += dH[4];		
				inputSample += dI[4];		
				inputSample += dJ[4];		
				inputSample += dK[4];		
				inputSample += dL[4];		
				inputSample += dM[4];		
				inputSample += dN[4];		
				inputSample += dO[4];		
				inputSample += dP[4];		
				inputSample += dQ[4];		
				inputSample += dR[4];		
				inputSample += dS[4];		
				inputSample += dT[4];		
				inputSample += dU[4];		
				inputSample += dV[4];		
				inputSample += dW[4];		
				inputSample += dX[4];		
				inputSample += dY[4];		
				inputSample += (dZ[4] * wetness);
				
				inputSample += (dB[5] * dryness);		
				inputSample += (dC[5] * dryness);		
				inputSample += dD[5];		
				inputSample += dE[5];		
				inputSample += dF[5];		
				inputSample += dG[5];		
				inputSample += dH[5];		
				inputSample += dI[5];		
				inputSample += dJ[5];		
				inputSample += dK[5];		
				inputSample += dL[5];		
				inputSample += dM[5];		
				inputSample += dN[5];		
				inputSample += dO[5];		
				inputSample += dP[5];		
				inputSample += dQ[5];		
				inputSample += dR[5];		
				inputSample += dS[5];		
				inputSample += dT[5];		
				inputSample += dU[5];		
				inputSample += dV[5];		
				inputSample += dW[5];		
				inputSample += dX[5];		
				inputSample += dY[5];		
				inputSample += (dZ[5] * wetness);
				
				inputSample /= (26.0f + (wetness * 4.0f));
				//output Room effect
				break;
				
				
				
				
				
				
case 5:	//Stretch	
				allpasstemp = alpA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->aA[allpasstemp]*constallpass;
				dram->aA[alpA] = inputSample;
				inputSample *= constallpass;
				alpA--; if (alpA < 0 || alpA > delayA) {alpA = delayA;}
				inputSample += (dram->aA[alpA]);
				//allpass filter A		
				
				dA[2] = dA[1];
				dA[1] = inputSample;
				inputSample = (dA[1] + dA[2])/2.0f;
				
				allpasstemp = alpB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->aB[allpasstemp]*constallpass;
				dram->aB[alpB] = inputSample;
				inputSample *= constallpass;
				alpB--; if (alpB < 0 || alpB > delayB) {alpB = delayB;}
				inputSample += (dram->aB[alpB]);
				//allpass filter B
				
				dB[2] = dB[1];
				dB[1] = inputSample;
				inputSample = (dB[1] + dB[2])/2.0f;
				
				allpasstemp = alpC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->aC[allpasstemp]*constallpass;
				dram->aC[alpC] = inputSample;
				inputSample *= constallpass;
				alpC--; if (alpC < 0 || alpC > delayC) {alpC = delayC;}
				inputSample += (dram->aC[alpC]);
				//allpass filter C
				
				dC[2] = dC[1];
				dC[1] = inputSample;
				inputSample = (dC[1] + dC[2])/2.0f;
				
				allpasstemp = alpD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->aD[allpasstemp]*constallpass;
				dram->aD[alpD] = inputSample;
				inputSample *= constallpass;
				alpD--; if (alpD < 0 || alpD > delayD) {alpD = delayD;}
				inputSample += (dram->aD[alpD]);
				//allpass filter D
				
				dD[2] = dD[1];
				dD[1] = inputSample;
				inputSample = (dD[1] + dD[2])/2.0f;
				
				allpasstemp = alpE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->aE[allpasstemp]*constallpass;
				dram->aE[alpE] = inputSample;
				inputSample *= constallpass;
				alpE--; if (alpE < 0 || alpE > delayE) {alpE = delayE;}
				inputSample += (dram->aE[alpE]);
				//allpass filter E
				
				dE[2] = dE[1];
				dE[1] = inputSample;
				inputSample = (dE[1] + dE[2])/2.0f;
				
				allpasstemp = alpF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->aF[allpasstemp]*constallpass;
				dram->aF[alpF] = inputSample;
				inputSample *= constallpass;
				alpF--; if (alpF < 0 || alpF > delayF) {alpF = delayF;}
				inputSample += (dram->aF[alpF]);
				//allpass filter F
				
				dF[2] = dF[1];
				dF[1] = inputSample;
				inputSample = (dF[1] + dF[2])/2.0f;
				
				allpasstemp = alpG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->aG[allpasstemp]*constallpass;
				dram->aG[alpG] = inputSample;
				inputSample *= constallpass;
				alpG--; if (alpG < 0 || alpG > delayG) {alpG = delayG;}
				inputSample += (dram->aG[alpG]);
				//allpass filter G
				
				dG[2] = dG[1];
				dG[1] = inputSample;
				inputSample = (dG[1] + dG[2])/2.0f;
				
				allpasstemp = alpH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->aH[allpasstemp]*constallpass;
				dram->aH[alpH] = inputSample;
				inputSample *= constallpass;
				alpH--; if (alpH < 0 || alpH > delayH) {alpH = delayH;}
				inputSample += (dram->aH[alpH]);
				//allpass filter H
				
				dH[2] = dH[1];
				dH[1] = inputSample;
				inputSample = (dH[1] + dH[2])/2.0f;
				
				allpasstemp = alpI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->aI[allpasstemp]*constallpass;
				dram->aI[alpI] = inputSample;
				inputSample *= constallpass;
				alpI--; if (alpI < 0 || alpI > delayI) {alpI = delayI;}
				inputSample += (dram->aI[alpI]);
				//allpass filter I
				
				dI[2] = dI[1];
				dI[1] = inputSample;
				inputSample = (dI[1] + dI[2])/2.0f;
				
				allpasstemp = alpJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->aJ[allpasstemp]*constallpass;
				dram->aJ[alpJ] = inputSample;
				inputSample *= constallpass;
				alpJ--; if (alpJ < 0 || alpJ > delayJ) {alpJ = delayJ;}
				inputSample += (dram->aJ[alpJ]);
				//allpass filter J
				
				dJ[2] = dJ[1];
				dJ[1] = inputSample;
				inputSample = (dJ[1] + dJ[2])/2.0f;
				
				allpasstemp = alpK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->aK[allpasstemp]*constallpass;
				dram->aK[alpK] = inputSample;
				inputSample *= constallpass;
				alpK--; if (alpK < 0 || alpK > delayK) {alpK = delayK;}
				inputSample += (dram->aK[alpK]);
				//allpass filter K
				
				dK[2] = dK[1];
				dK[1] = inputSample;
				inputSample = (dK[1] + dK[2])/2.0f;
				
				allpasstemp = alpL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->aL[allpasstemp]*constallpass;
				dram->aL[alpL] = inputSample;
				inputSample *= constallpass;
				alpL--; if (alpL < 0 || alpL > delayL) {alpL = delayL;}
				inputSample += (dram->aL[alpL]);
				//allpass filter L
				
				dL[2] = dL[1];
				dL[1] = inputSample;
				inputSample = (dL[1] + dL[2])/2.0f;
				
				allpasstemp = alpM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->aM[allpasstemp]*constallpass;
				dram->aM[alpM] = inputSample;
				inputSample *= constallpass;
				alpM--; if (alpM < 0 || alpM > delayM) {alpM = delayM;}
				inputSample += (dram->aM[alpM]);
				//allpass filter M
				
				dM[2] = dM[1];
				dM[1] = inputSample;
				inputSample = (dM[1] + dM[2])/2.0f;
				
				allpasstemp = alpN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->aN[allpasstemp]*constallpass;
				dram->aN[alpN] = inputSample;
				inputSample *= constallpass;
				alpN--; if (alpN < 0 || alpN > delayN) {alpN = delayN;}
				inputSample += (dram->aN[alpN]);
				//allpass filter N
				
				dN[2] = dN[1];
				dN[1] = inputSample;
				inputSample = (dN[1] + dN[2])/2.0f;
				
				allpasstemp = alpO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->aO[allpasstemp]*constallpass;
				dram->aO[alpO] = inputSample;
				inputSample *= constallpass;
				alpO--; if (alpO < 0 || alpO > delayO) {alpO = delayO;}
				inputSample += (dram->aO[alpO]);
				//allpass filter O
				
				dO[2] = dO[1];
				dO[1] = inputSample;
				inputSample = (dO[1] + dO[2])/2.0f;
				
				allpasstemp = alpP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->aP[allpasstemp]*constallpass;
				dram->aP[alpP] = inputSample;
				inputSample *= constallpass;
				alpP--; if (alpP < 0 || alpP > delayP) {alpP = delayP;}
				inputSample += (dram->aP[alpP]);
				//allpass filter P
				
				dP[2] = dP[1];
				dP[1] = inputSample;
				inputSample = (dP[1] + dP[2])/2.0f;
				
				allpasstemp = alpQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->aQ[allpasstemp]*constallpass;
				dram->aQ[alpQ] = inputSample;
				inputSample *= constallpass;
				alpQ--; if (alpQ < 0 || alpQ > delayQ) {alpQ = delayQ;}
				inputSample += (dram->aQ[alpQ]);
				//allpass filter Q
				
				dQ[2] = dQ[1];
				dQ[1] = inputSample;
				inputSample = (dQ[1] + dQ[2])/2.0f;
				
				allpasstemp = alpR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->aR[allpasstemp]*constallpass;
				dram->aR[alpR] = inputSample;
				inputSample *= constallpass;
				alpR--; if (alpR < 0 || alpR > delayR) {alpR = delayR;}
				inputSample += (dram->aR[alpR]);
				//allpass filter R
				
				dR[2] = dR[1];
				dR[1] = inputSample;
				inputSample = (dR[1] + dR[2])/2.0f;
				
				allpasstemp = alpS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->aS[allpasstemp]*constallpass;
				dram->aS[alpS] = inputSample;
				inputSample *= constallpass;
				alpS--; if (alpS < 0 || alpS > delayS) {alpS = delayS;}
				inputSample += (dram->aS[alpS]);
				//allpass filter S
				
				dS[2] = dS[1];
				dS[1] = inputSample;
				inputSample = (dS[1] + dS[2])/2.0f;
				
				allpasstemp = alpT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->aT[allpasstemp]*constallpass;
				dram->aT[alpT] = inputSample;
				inputSample *= constallpass;
				alpT--; if (alpT < 0 || alpT > delayT) {alpT = delayT;}
				inputSample += (dram->aT[alpT]);
				//allpass filter T
				
				dT[2] = dT[1];
				dT[1] = inputSample;
				inputSample = (dT[1] + dT[2])/2.0f;
				
				allpasstemp = alpU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->aU[allpasstemp]*constallpass;
				dram->aU[alpU] = inputSample;
				inputSample *= constallpass;
				alpU--; if (alpU < 0 || alpU > delayU) {alpU = delayU;}
				inputSample += (dram->aU[alpU]);
				//allpass filter U
				
				dU[2] = dU[1];
				dU[1] = inputSample;
				inputSample = (dU[1] + dU[2])/2.0f;
				
				allpasstemp = alpV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->aV[allpasstemp]*constallpass;
				dram->aV[alpV] = inputSample;
				inputSample *= constallpass;
				alpV--; if (alpV < 0 || alpV > delayV) {alpV = delayV;}
				inputSample += (dram->aV[alpV]);
				//allpass filter V
				
				dV[2] = dV[1];
				dV[1] = inputSample;
				inputSample = (dV[1] + dV[2])/2.0f;
				
				allpasstemp = alpW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->aW[allpasstemp]*constallpass;
				dram->aW[alpW] = inputSample;
				inputSample *= constallpass;
				alpW--; if (alpW < 0 || alpW > delayW) {alpW = delayW;}
				inputSample += (dram->aW[alpW]);
				//allpass filter W
				
				dW[2] = dW[1];
				dW[1] = inputSample;
				inputSample = (dW[1] + dW[2])/2.0f;
				
				allpasstemp = alpX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->aX[allpasstemp]*constallpass;
				dram->aX[alpX] = inputSample;
				inputSample *= constallpass;
				alpX--; if (alpX < 0 || alpX > delayX) {alpX = delayX;}
				inputSample += (dram->aX[alpX]);
				//allpass filter X
				
				dX[2] = dX[1];
				dX[1] = inputSample;
				inputSample = (dX[1] + dX[2])/2.0f;
				
				allpasstemp = alpY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->aY[allpasstemp]*constallpass;
				dram->aY[alpY] = inputSample;
				inputSample *= constallpass;
				alpY--; if (alpY < 0 || alpY > delayY) {alpY = delayY;}
				inputSample += (dram->aY[alpY]);
				//allpass filter Y
				
				dY[2] = dY[1];
				dY[1] = inputSample;
				inputSample = (dY[1] + dY[2])/2.0f;
				
				allpasstemp = alpZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->aZ[allpasstemp]*constallpass;
				dram->aZ[alpZ] = inputSample;
				inputSample *= constallpass;
				alpZ--; if (alpZ < 0 || alpZ > delayZ) {alpZ = delayZ;}
				inputSample += (dram->aZ[alpZ]);
				//allpass filter Z
				
				dZ[2] = dZ[1];
				dZ[1] = inputSample;
				inputSample = (dZ[1] + dZ[2])/2.0f;
				
				// now the second stage using the 'out' bank of allpasses		
				
				allpasstemp = outA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->oA[allpasstemp]*constallpass;
				dram->oA[outA] = inputSample;
				inputSample *= constallpass;
				outA--; if (outA < 0 || outA > delayA) {outA = delayA;}
				inputSample += (dram->oA[outA]);
				//allpass filter A		
				
				dA[5] = dA[4];
				dA[4] = inputSample;
				inputSample = (dA[4] + dA[5])/2.0f;
				
				allpasstemp = outB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->oB[allpasstemp]*constallpass;
				dram->oB[outB] = inputSample;
				inputSample *= constallpass;
				outB--; if (outB < 0 || outB > delayB) {outB = delayB;}
				inputSample += (dram->oB[outB]);
				//allpass filter B
				
				dB[5] = dB[4];
				dB[4] = inputSample;
				inputSample = (dB[4] + dB[5])/2.0f;
				
				allpasstemp = outC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->oC[allpasstemp]*constallpass;
				dram->oC[outC] = inputSample;
				inputSample *= constallpass;
				outC--; if (outC < 0 || outC > delayC) {outC = delayC;}
				inputSample += (dram->oC[outC]);
				//allpass filter C
				
				dC[5] = dC[4];
				dC[4] = inputSample;
				inputSample = (dC[4] + dC[5])/2.0f;
				
				allpasstemp = outD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->oD[allpasstemp]*constallpass;
				dram->oD[outD] = inputSample;
				inputSample *= constallpass;
				outD--; if (outD < 0 || outD > delayD) {outD = delayD;}
				inputSample += (dram->oD[outD]);
				//allpass filter D
				
				dD[5] = dD[4];
				dD[4] = inputSample;
				inputSample = (dD[4] + dD[5])/2.0f;
				
				allpasstemp = outE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->oE[allpasstemp]*constallpass;
				dram->oE[outE] = inputSample;
				inputSample *= constallpass;
				outE--; if (outE < 0 || outE > delayE) {outE = delayE;}
				inputSample += (dram->oE[outE]);
				//allpass filter E
				
				dE[5] = dE[4];
				dE[4] = inputSample;
				inputSample = (dE[4] + dE[5])/2.0f;
				
				allpasstemp = outF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->oF[allpasstemp]*constallpass;
				dram->oF[outF] = inputSample;
				inputSample *= constallpass;
				outF--; if (outF < 0 || outF > delayF) {outF = delayF;}
				inputSample += (dram->oF[outF]);
				//allpass filter F
				
				dF[5] = dF[4];
				dF[4] = inputSample;
				inputSample = (dF[4] + dF[5])/2.0f;
				
				allpasstemp = outG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->oG[allpasstemp]*constallpass;
				dram->oG[outG] = inputSample;
				inputSample *= constallpass;
				outG--; if (outG < 0 || outG > delayG) {outG = delayG;}
				inputSample += (dram->oG[outG]);
				//allpass filter G
				
				dG[5] = dG[4];
				dG[4] = inputSample;
				inputSample = (dG[4] + dG[5])/2.0f;
				
				allpasstemp = outH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->oH[allpasstemp]*constallpass;
				dram->oH[outH] = inputSample;
				inputSample *= constallpass;
				outH--; if (outH < 0 || outH > delayH) {outH = delayH;}
				inputSample += (dram->oH[outH]);
				//allpass filter H
				
				dH[5] = dH[4];
				dH[4] = inputSample;
				inputSample = (dH[4] + dH[5])/2.0f;
				
				allpasstemp = outI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->oI[allpasstemp]*constallpass;
				dram->oI[outI] = inputSample;
				inputSample *= constallpass;
				outI--; if (outI < 0 || outI > delayI) {outI = delayI;}
				inputSample += (dram->oI[outI]);
				//allpass filter I
				
				dI[5] = dI[4];
				dI[4] = inputSample;
				inputSample = (dI[4] + dI[5])/2.0f;
				
				allpasstemp = outJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->oJ[allpasstemp]*constallpass;
				dram->oJ[outJ] = inputSample;
				inputSample *= constallpass;
				outJ--; if (outJ < 0 || outJ > delayJ) {outJ = delayJ;}
				inputSample += (dram->oJ[outJ]);
				//allpass filter J
				
				dJ[5] = dJ[4];
				dJ[4] = inputSample;
				inputSample = (dJ[4] + dJ[5])/2.0f;
				
				allpasstemp = outK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->oK[allpasstemp]*constallpass;
				dram->oK[outK] = inputSample;
				inputSample *= constallpass;
				outK--; if (outK < 0 || outK > delayK) {outK = delayK;}
				inputSample += (dram->oK[outK]);
				//allpass filter K
				
				dK[5] = dK[4];
				dK[4] = inputSample;
				inputSample = (dK[4] + dK[5])/2.0f;
				
				allpasstemp = outL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->oL[allpasstemp]*constallpass;
				dram->oL[outL] = inputSample;
				inputSample *= constallpass;
				outL--; if (outL < 0 || outL > delayL) {outL = delayL;}
				inputSample += (dram->oL[outL]);
				//allpass filter L
				
				dL[5] = dL[4];
				dL[4] = inputSample;
				inputSample = (dL[4] + dL[5])/2.0f;
				
				allpasstemp = outM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->oM[allpasstemp]*constallpass;
				dram->oM[outM] = inputSample;
				inputSample *= constallpass;
				outM--; if (outM < 0 || outM > delayM) {outM = delayM;}
				inputSample += (dram->oM[outM]);
				//allpass filter M
				
				dM[5] = dM[4];
				dM[4] = inputSample;
				inputSample = (dM[4] + dM[5])/2.0f;
				
				allpasstemp = outN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->oN[allpasstemp]*constallpass;
				dram->oN[outN] = inputSample;
				inputSample *= constallpass;
				outN--; if (outN < 0 || outN > delayN) {outN = delayN;}
				inputSample += (dram->oN[outN]);
				//allpass filter N
				
				dN[5] = dN[4];
				dN[4] = inputSample;
				inputSample = (dN[4] + dN[5])/2.0f;
				
				allpasstemp = outO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->oO[allpasstemp]*constallpass;
				dram->oO[outO] = inputSample;
				inputSample *= constallpass;
				outO--; if (outO < 0 || outO > delayO) {outO = delayO;}
				inputSample += (dram->oO[outO]);
				//allpass filter O
				
				dO[5] = dO[4];
				dO[4] = inputSample;
				inputSample = (dO[4] + dO[5])/2.0f;
				
				allpasstemp = outP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->oP[allpasstemp]*constallpass;
				dram->oP[outP] = inputSample;
				inputSample *= constallpass;
				outP--; if (outP < 0 || outP > delayP) {outP = delayP;}
				inputSample += (dram->oP[outP]);
				//allpass filter P
				
				dP[5] = dP[4];
				dP[4] = inputSample;
				inputSample = (dP[4] + dP[5])/2.0f;
				
				allpasstemp = outQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->oQ[allpasstemp]*constallpass;
				dram->oQ[outQ] = inputSample;
				inputSample *= constallpass;
				outQ--; if (outQ < 0 || outQ > delayQ) {outQ = delayQ;}
				inputSample += (dram->oQ[outQ]);
				//allpass filter Q
				
				dQ[5] = dQ[4];
				dQ[4] = inputSample;
				inputSample = (dQ[4] + dQ[5])/2.0f;
				
				allpasstemp = outR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->oR[allpasstemp]*constallpass;
				dram->oR[outR] = inputSample;
				inputSample *= constallpass;
				outR--; if (outR < 0 || outR > delayR) {outR = delayR;}
				inputSample += (dram->oR[outR]);
				//allpass filter R
				
				dR[5] = dR[4];
				dR[4] = inputSample;
				inputSample = (dR[4] + dR[5])/2.0f;
				
				allpasstemp = outS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->oS[allpasstemp]*constallpass;
				dram->oS[outS] = inputSample;
				inputSample *= constallpass;
				outS--; if (outS < 0 || outS > delayS) {outS = delayS;}
				inputSample += (dram->oS[outS]);
				//allpass filter S
				
				dS[5] = dS[4];
				dS[4] = inputSample;
				inputSample = (dS[4] + dS[5])/2.0f;
				
				allpasstemp = outT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->oT[allpasstemp]*constallpass;
				dram->oT[outT] = inputSample;
				inputSample *= constallpass;
				outT--; if (outT < 0 || outT > delayT) {outT = delayT;}
				inputSample += (dram->oT[outT]);
				//allpass filter T
				
				dT[5] = dT[4];
				dT[4] = inputSample;
				inputSample = (dT[4] + dT[5])/2.0f;
				
				allpasstemp = outU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->oU[allpasstemp]*constallpass;
				dram->oU[outU] = inputSample;
				inputSample *= constallpass;
				outU--; if (outU < 0 || outU > delayU) {outU = delayU;}
				inputSample += (dram->oU[outU]);
				//allpass filter U
				
				dU[5] = dU[4];
				dU[4] = inputSample;
				inputSample = (dU[4] + dU[5])/2.0f;
				
				allpasstemp = outV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->oV[allpasstemp]*constallpass;
				dram->oV[outV] = inputSample;
				inputSample *= constallpass;
				outV--; if (outV < 0 || outV > delayV) {outV = delayV;}
				inputSample += (dram->oV[outV]);
				//allpass filter V
				
				dV[5] = dV[4];
				dV[4] = inputSample;
				inputSample = (dV[4] + dV[5])/2.0f;
				
				allpasstemp = outW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->oW[allpasstemp]*constallpass;
				dram->oW[outW] = inputSample;
				inputSample *= constallpass;
				outW--; if (outW < 0 || outW > delayW) {outW = delayW;}
				inputSample += (dram->oW[outW]);
				//allpass filter W
				
				dW[5] = dW[4];
				dW[4] = inputSample;
				inputSample = (dW[4] + dW[5])/2.0f;
				
				allpasstemp = outX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->oX[allpasstemp]*constallpass;
				dram->oX[outX] = inputSample;
				inputSample *= constallpass;
				outX--; if (outX < 0 || outX > delayX) {outX = delayX;}
				inputSample += (dram->oX[outX]);
				//allpass filter X
				
				dX[5] = dX[4];
				dX[4] = inputSample;
				inputSample = (dX[4] + dX[5])/2.0f;
				
				allpasstemp = outY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->oY[allpasstemp]*constallpass;
				dram->oY[outY] = inputSample;
				inputSample *= constallpass;
				outY--; if (outY < 0 || outY > delayY) {outY = delayY;}
				inputSample += (dram->oY[outY]);
				//allpass filter Y
				
				dY[5] = dY[4];
				dY[4] = inputSample;
				inputSample = (dY[4] + dY[5])/2.0f;
				
				allpasstemp = outZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->oZ[allpasstemp]*constallpass;
				dram->oZ[outZ] = inputSample;
				inputSample *= constallpass;
				outZ--; if (outZ < 0 || outZ > delayZ) {outZ = delayZ;}
				inputSample += (dram->oZ[outZ]);
				//allpass filter Z
				
				dZ[5] = dZ[4];
				dZ[4] = inputSample;
				inputSample = (dZ[4] + dZ[5])/2.0f;		
				//output Stretch unrealistic but smooth fake Paulstretch
				break;				
				
			
case 6:	//Zarathustra	
				allpasstemp = alpA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->aA[allpasstemp]*constallpass;
				dram->aA[alpA] = inputSample;
				inputSample *= constallpass;
				alpA--; if (alpA < 0 || alpA > delayA) {alpA = delayA;}
				inputSample += (dram->aA[alpA]);
				//allpass filter A		
				
				dA[3] = dA[2];
				dA[2] = dA[1];
				dA[1] = inputSample;
				inputSample = (dA[1] + dA[2] + dZ[3])/3.0f; //add feedback
				
				allpasstemp = alpB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->aB[allpasstemp]*constallpass;
				dram->aB[alpB] = inputSample;
				inputSample *= constallpass;
				alpB--; if (alpB < 0 || alpB > delayB) {alpB = delayB;}
				inputSample += (dram->aB[alpB]);
				//allpass filter B
				
				dB[3] = dB[2];
				dB[2] = dB[1];
				dB[1] = inputSample;
				inputSample = (dB[1] + dB[2] + dB[3])/3.0f;
				
				allpasstemp = alpC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->aC[allpasstemp]*constallpass;
				dram->aC[alpC] = inputSample;
				inputSample *= constallpass;
				alpC--; if (alpC < 0 || alpC > delayC) {alpC = delayC;}
				inputSample += (dram->aC[alpC]);
				//allpass filter C
				
				dC[3] = dC[2];
				dC[2] = dC[1];
				dC[1] = inputSample;
				inputSample = (dC[1] + dC[2] + dC[3])/3.0f;
				
				allpasstemp = alpD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->aD[allpasstemp]*constallpass;
				dram->aD[alpD] = inputSample;
				inputSample *= constallpass;
				alpD--; if (alpD < 0 || alpD > delayD) {alpD = delayD;}
				inputSample += (dram->aD[alpD]);
				//allpass filter D
				
				dD[3] = dD[2];
				dD[2] = dD[1];
				dD[1] = inputSample;
				inputSample = (dD[1] + dD[2] + dD[3])/3.0f;
				
				allpasstemp = alpE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->aE[allpasstemp]*constallpass;
				dram->aE[alpE] = inputSample;
				inputSample *= constallpass;
				alpE--; if (alpE < 0 || alpE > delayE) {alpE = delayE;}
				inputSample += (dram->aE[alpE]);
				//allpass filter E
				
				dE[3] = dE[2];
				dE[2] = dE[1];
				dE[1] = inputSample;
				inputSample = (dE[1] + dE[2] + dE[3])/3.0f;
				
				allpasstemp = alpF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->aF[allpasstemp]*constallpass;
				dram->aF[alpF] = inputSample;
				inputSample *= constallpass;
				alpF--; if (alpF < 0 || alpF > delayF) {alpF = delayF;}
				inputSample += (dram->aF[alpF]);
				//allpass filter F
				
				dF[3] = dF[2];
				dF[2] = dF[1];
				dF[1] = inputSample;
				inputSample = (dF[1] + dF[2] + dF[3])/3.0f;
				
				allpasstemp = alpG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->aG[allpasstemp]*constallpass;
				dram->aG[alpG] = inputSample;
				inputSample *= constallpass;
				alpG--; if (alpG < 0 || alpG > delayG) {alpG = delayG;}
				inputSample += (dram->aG[alpG]);
				//allpass filter G
				
				dG[3] = dG[2];
				dG[2] = dG[1];
				dG[1] = inputSample;
				inputSample = (dG[1] + dG[2] + dG[3])/3.0f;
				
				allpasstemp = alpH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->aH[allpasstemp]*constallpass;
				dram->aH[alpH] = inputSample;
				inputSample *= constallpass;
				alpH--; if (alpH < 0 || alpH > delayH) {alpH = delayH;}
				inputSample += (dram->aH[alpH]);
				//allpass filter H
				
				dH[3] = dH[2];
				dH[2] = dH[1];
				dH[1] = inputSample;
				inputSample = (dH[1] + dH[2] + dH[3])/3.0f;
				
				allpasstemp = alpI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->aI[allpasstemp]*constallpass;
				dram->aI[alpI] = inputSample;
				inputSample *= constallpass;
				alpI--; if (alpI < 0 || alpI > delayI) {alpI = delayI;}
				inputSample += (dram->aI[alpI]);
				//allpass filter I
				
				dI[3] = dI[2];
				dI[2] = dI[1];
				dI[1] = inputSample;
				inputSample = (dI[1] + dI[2] + dI[3])/3.0f;
				
				allpasstemp = alpJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->aJ[allpasstemp]*constallpass;
				dram->aJ[alpJ] = inputSample;
				inputSample *= constallpass;
				alpJ--; if (alpJ < 0 || alpJ > delayJ) {alpJ = delayJ;}
				inputSample += (dram->aJ[alpJ]);
				//allpass filter J
				
				dJ[3] = dJ[2];
				dJ[2] = dJ[1];
				dJ[1] = inputSample;
				inputSample = (dJ[1] + dJ[2] + dJ[3])/3.0f;
				
				allpasstemp = alpK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->aK[allpasstemp]*constallpass;
				dram->aK[alpK] = inputSample;
				inputSample *= constallpass;
				alpK--; if (alpK < 0 || alpK > delayK) {alpK = delayK;}
				inputSample += (dram->aK[alpK]);
				//allpass filter K
				
				dK[3] = dK[2];
				dK[2] = dK[1];
				dK[1] = inputSample;
				inputSample = (dK[1] + dK[2] + dK[3])/3.0f;
				
				allpasstemp = alpL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->aL[allpasstemp]*constallpass;
				dram->aL[alpL] = inputSample;
				inputSample *= constallpass;
				alpL--; if (alpL < 0 || alpL > delayL) {alpL = delayL;}
				inputSample += (dram->aL[alpL]);
				//allpass filter L
				
				dL[3] = dL[2];
				dL[2] = dL[1];
				dL[1] = inputSample;
				inputSample = (dL[1] + dL[2] + dL[3])/3.0f;
				
				allpasstemp = alpM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->aM[allpasstemp]*constallpass;
				dram->aM[alpM] = inputSample;
				inputSample *= constallpass;
				alpM--; if (alpM < 0 || alpM > delayM) {alpM = delayM;}
				inputSample += (dram->aM[alpM]);
				//allpass filter M
				
				dM[3] = dM[2];
				dM[2] = dM[1];
				dM[1] = inputSample;
				inputSample = (dM[1] + dM[2] + dM[3])/3.0f;
				
				allpasstemp = alpN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->aN[allpasstemp]*constallpass;
				dram->aN[alpN] = inputSample;
				inputSample *= constallpass;
				alpN--; if (alpN < 0 || alpN > delayN) {alpN = delayN;}
				inputSample += (dram->aN[alpN]);
				//allpass filter N
				
				dN[3] = dN[2];
				dN[2] = dN[1];
				dN[1] = inputSample;
				inputSample = (dN[1] + dN[2] + dN[3])/3.0f;
				
				allpasstemp = alpO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->aO[allpasstemp]*constallpass;
				dram->aO[alpO] = inputSample;
				inputSample *= constallpass;
				alpO--; if (alpO < 0 || alpO > delayO) {alpO = delayO;}
				inputSample += (dram->aO[alpO]);
				//allpass filter O
				
				dO[3] = dO[2];
				dO[2] = dO[1];
				dO[1] = inputSample;
				inputSample = (dO[1] + dO[2] + dO[3])/3.0f;
				
				allpasstemp = alpP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->aP[allpasstemp]*constallpass;
				dram->aP[alpP] = inputSample;
				inputSample *= constallpass;
				alpP--; if (alpP < 0 || alpP > delayP) {alpP = delayP;}
				inputSample += (dram->aP[alpP]);
				//allpass filter P
				
				dP[3] = dP[2];
				dP[2] = dP[1];
				dP[1] = inputSample;
				inputSample = (dP[1] + dP[2] + dP[3])/3.0f;
				
				allpasstemp = alpQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->aQ[allpasstemp]*constallpass;
				dram->aQ[alpQ] = inputSample;
				inputSample *= constallpass;
				alpQ--; if (alpQ < 0 || alpQ > delayQ) {alpQ = delayQ;}
				inputSample += (dram->aQ[alpQ]);
				//allpass filter Q
				
				dQ[3] = dQ[2];
				dQ[2] = dQ[1];
				dQ[1] = inputSample;
				inputSample = (dQ[1] + dQ[2] + dQ[3])/3.0f;
				
				allpasstemp = alpR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->aR[allpasstemp]*constallpass;
				dram->aR[alpR] = inputSample;
				inputSample *= constallpass;
				alpR--; if (alpR < 0 || alpR > delayR) {alpR = delayR;}
				inputSample += (dram->aR[alpR]);
				//allpass filter R
				
				dR[3] = dR[2];
				dR[2] = dR[1];
				dR[1] = inputSample;
				inputSample = (dR[1] + dR[2] + dR[3])/3.0f;
				
				allpasstemp = alpS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->aS[allpasstemp]*constallpass;
				dram->aS[alpS] = inputSample;
				inputSample *= constallpass;
				alpS--; if (alpS < 0 || alpS > delayS) {alpS = delayS;}
				inputSample += (dram->aS[alpS]);
				//allpass filter S
				
				dS[3] = dS[2];
				dS[2] = dS[1];
				dS[1] = inputSample;
				inputSample = (dS[1] + dS[2] + dS[3])/3.0f;
				
				allpasstemp = alpT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->aT[allpasstemp]*constallpass;
				dram->aT[alpT] = inputSample;
				inputSample *= constallpass;
				alpT--; if (alpT < 0 || alpT > delayT) {alpT = delayT;}
				inputSample += (dram->aT[alpT]);
				//allpass filter T
				
				dT[3] = dT[2];
				dT[2] = dT[1];
				dT[1] = inputSample;
				inputSample = (dT[1] + dT[2] + dT[3])/3.0f;
				
				allpasstemp = alpU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->aU[allpasstemp]*constallpass;
				dram->aU[alpU] = inputSample;
				inputSample *= constallpass;
				alpU--; if (alpU < 0 || alpU > delayU) {alpU = delayU;}
				inputSample += (dram->aU[alpU]);
				//allpass filter U
				
				dU[3] = dU[2];
				dU[2] = dU[1];
				dU[1] = inputSample;
				inputSample = (dU[1] + dU[2] + dU[3])/3.0f;
				
				allpasstemp = alpV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->aV[allpasstemp]*constallpass;
				dram->aV[alpV] = inputSample;
				inputSample *= constallpass;
				alpV--; if (alpV < 0 || alpV > delayV) {alpV = delayV;}
				inputSample += (dram->aV[alpV]);
				//allpass filter V
				
				dV[3] = dV[2];
				dV[2] = dV[1];
				dV[1] = inputSample;
				inputSample = (dV[1] + dV[2] + dV[3])/3.0f;
				
				allpasstemp = alpW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->aW[allpasstemp]*constallpass;
				dram->aW[alpW] = inputSample;
				inputSample *= constallpass;
				alpW--; if (alpW < 0 || alpW > delayW) {alpW = delayW;}
				inputSample += (dram->aW[alpW]);
				//allpass filter W
				
				dW[3] = dW[2];
				dW[2] = dW[1];
				dW[1] = inputSample;
				inputSample = (dW[1] + dW[2] + dW[3])/3.0f;
				
				allpasstemp = alpX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->aX[allpasstemp]*constallpass;
				dram->aX[alpX] = inputSample;
				inputSample *= constallpass;
				alpX--; if (alpX < 0 || alpX > delayX) {alpX = delayX;}
				inputSample += (dram->aX[alpX]);
				//allpass filter X
				
				dX[3] = dX[2];
				dX[2] = dX[1];
				dX[1] = inputSample;
				inputSample = (dX[1] + dX[2] + dX[3])/3.0f;
				
				allpasstemp = alpY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->aY[allpasstemp]*constallpass;
				dram->aY[alpY] = inputSample;
				inputSample *= constallpass;
				alpY--; if (alpY < 0 || alpY > delayY) {alpY = delayY;}
				inputSample += (dram->aY[alpY]);
				//allpass filter Y
				
				dY[3] = dY[2];
				dY[2] = dY[1];
				dY[1] = inputSample;
				inputSample = (dY[1] + dY[2] + dY[3])/3.0f;
				
				allpasstemp = alpZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->aZ[allpasstemp]*constallpass;
				dram->aZ[alpZ] = inputSample;
				inputSample *= constallpass;
				alpZ--; if (alpZ < 0 || alpZ > delayZ) {alpZ = delayZ;}
				inputSample += (dram->aZ[alpZ]);
				//allpass filter Z
				
				dZ[3] = dZ[2];
				dZ[2] = dZ[1];
				dZ[1] = inputSample;
				inputSample = (dZ[1] + dZ[2] + dZ[3])/3.0f;
				
				// now the second stage using the 'out' bank of allpasses		
				
				allpasstemp = outA - 1;
				if (allpasstemp < 0 || allpasstemp > delayA) {allpasstemp = delayA;}
				inputSample -= dram->oA[allpasstemp]*constallpass;
				dram->oA[outA] = inputSample;
				inputSample *= constallpass;
				outA--; if (outA < 0 || outA > delayA) {outA = delayA;}
				inputSample += (dram->oA[outA]);
				//allpass filter A		
				
				dA[6] = dA[5];
				dA[5] = dA[4];
				dA[4] = inputSample;
				inputSample = (dC[1] + dA[5] + dA[6])/3.0f;  //note, feeding in dry again for a little more clarity!
				
				allpasstemp = outB - 1;
				if (allpasstemp < 0 || allpasstemp > delayB) {allpasstemp = delayB;}
				inputSample -= dram->oB[allpasstemp]*constallpass;
				dram->oB[outB] = inputSample;
				inputSample *= constallpass;
				outB--; if (outB < 0 || outB > delayB) {outB = delayB;}
				inputSample += (dram->oB[outB]);
				//allpass filter B
				
				dB[6] = dB[5];
				dB[5] = dB[4];
				dB[4] = inputSample;
				inputSample = (dB[4] + dB[5] + dB[6])/3.0f;
				
				allpasstemp = outC - 1;
				if (allpasstemp < 0 || allpasstemp > delayC) {allpasstemp = delayC;}
				inputSample -= dram->oC[allpasstemp]*constallpass;
				dram->oC[outC] = inputSample;
				inputSample *= constallpass;
				outC--; if (outC < 0 || outC > delayC) {outC = delayC;}
				inputSample += (dram->oC[outC]);
				//allpass filter C
				
				dC[6] = dC[5];
				dC[5] = dC[4];
				dC[4] = inputSample;
				inputSample = (dC[4] + dC[5] + dC[6])/3.0f;
				
				allpasstemp = outD - 1;
				if (allpasstemp < 0 || allpasstemp > delayD) {allpasstemp = delayD;}
				inputSample -= dram->oD[allpasstemp]*constallpass;
				dram->oD[outD] = inputSample;
				inputSample *= constallpass;
				outD--; if (outD < 0 || outD > delayD) {outD = delayD;}
				inputSample += (dram->oD[outD]);
				//allpass filter D
				
				dD[6] = dD[5];
				dD[5] = dD[4];
				dD[4] = inputSample;
				inputSample = (dD[4] + dD[5] + dD[6])/3.0f;
				
				allpasstemp = outE - 1;
				if (allpasstemp < 0 || allpasstemp > delayE) {allpasstemp = delayE;}
				inputSample -= dram->oE[allpasstemp]*constallpass;
				dram->oE[outE] = inputSample;
				inputSample *= constallpass;
				outE--; if (outE < 0 || outE > delayE) {outE = delayE;}
				inputSample += (dram->oE[outE]);
				//allpass filter E
				
				dE[6] = dE[5];
				dE[5] = dE[4];
				dE[4] = inputSample;
				inputSample = (dE[4] + dE[5] + dE[6])/3.0f;
				
				allpasstemp = outF - 1;
				if (allpasstemp < 0 || allpasstemp > delayF) {allpasstemp = delayF;}
				inputSample -= dram->oF[allpasstemp]*constallpass;
				dram->oF[outF] = inputSample;
				inputSample *= constallpass;
				outF--; if (outF < 0 || outF > delayF) {outF = delayF;}
				inputSample += (dram->oF[outF]);
				//allpass filter F
				
				dF[6] = dF[5];
				dF[5] = dF[4];
				dF[4] = inputSample;
				inputSample = (dF[4] + dF[5] + dF[6])/3.0f;
				
				allpasstemp = outG - 1;
				if (allpasstemp < 0 || allpasstemp > delayG) {allpasstemp = delayG;}
				inputSample -= dram->oG[allpasstemp]*constallpass;
				dram->oG[outG] = inputSample;
				inputSample *= constallpass;
				outG--; if (outG < 0 || outG > delayG) {outG = delayG;}
				inputSample += (dram->oG[outG]);
				//allpass filter G
				
				dG[6] = dG[5];
				dG[5] = dG[4];
				dG[4] = inputSample;
				inputSample = (dG[4] + dG[5] + dG[6])/3.0f;
				
				allpasstemp = outH - 1;
				if (allpasstemp < 0 || allpasstemp > delayH) {allpasstemp = delayH;}
				inputSample -= dram->oH[allpasstemp]*constallpass;
				dram->oH[outH] = inputSample;
				inputSample *= constallpass;
				outH--; if (outH < 0 || outH > delayH) {outH = delayH;}
				inputSample += (dram->oH[outH]);
				//allpass filter H
				
				dH[6] = dH[5];
				dH[5] = dH[4];
				dH[4] = inputSample;
				inputSample = (dH[4] + dH[5] + dH[6])/3.0f;
				
				allpasstemp = outI - 1;
				if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = delayI;}
				inputSample -= dram->oI[allpasstemp]*constallpass;
				dram->oI[outI] = inputSample;
				inputSample *= constallpass;
				outI--; if (outI < 0 || outI > delayI) {outI = delayI;}
				inputSample += (dram->oI[outI]);
				//allpass filter I
				
				dI[6] = dI[5];
				dI[5] = dI[4];
				dI[4] = inputSample;
				inputSample = (dI[4] + dI[5] + dI[6])/3.0f;
				
				allpasstemp = outJ - 1;
				if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = delayJ;}
				inputSample -= dram->oJ[allpasstemp]*constallpass;
				dram->oJ[outJ] = inputSample;
				inputSample *= constallpass;
				outJ--; if (outJ < 0 || outJ > delayJ) {outJ = delayJ;}
				inputSample += (dram->oJ[outJ]);
				//allpass filter J
				
				dJ[6] = dJ[5];
				dJ[5] = dJ[4];
				dJ[4] = inputSample;
				inputSample = (dJ[4] + dJ[5] + dJ[6])/3.0f;
				
				allpasstemp = outK - 1;
				if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = delayK;}
				inputSample -= dram->oK[allpasstemp]*constallpass;
				dram->oK[outK] = inputSample;
				inputSample *= constallpass;
				outK--; if (outK < 0 || outK > delayK) {outK = delayK;}
				inputSample += (dram->oK[outK]);
				//allpass filter K
				
				dK[6] = dK[5];
				dK[5] = dK[4];
				dK[4] = inputSample;
				inputSample = (dK[4] + dK[5] + dK[6])/3.0f;
				
				allpasstemp = outL - 1;
				if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = delayL;}
				inputSample -= dram->oL[allpasstemp]*constallpass;
				dram->oL[outL] = inputSample;
				inputSample *= constallpass;
				outL--; if (outL < 0 || outL > delayL) {outL = delayL;}
				inputSample += (dram->oL[outL]);
				//allpass filter L
				
				dL[6] = dL[5];
				dL[5] = dL[4];
				dL[4] = inputSample;
				inputSample = (dL[4] + dL[5] + dL[6])/3.0f;
				
				allpasstemp = outM - 1;
				if (allpasstemp < 0 || allpasstemp > delayM) {allpasstemp = delayM;}
				inputSample -= dram->oM[allpasstemp]*constallpass;
				dram->oM[outM] = inputSample;
				inputSample *= constallpass;
				outM--; if (outM < 0 || outM > delayM) {outM = delayM;}
				inputSample += (dram->oM[outM]);
				//allpass filter M
				
				dM[6] = dM[5];
				dM[5] = dM[4];
				dM[4] = inputSample;
				inputSample = (dM[4] + dM[5] + dM[6])/3.0f;
				
				allpasstemp = outN - 1;
				if (allpasstemp < 0 || allpasstemp > delayN) {allpasstemp = delayN;}
				inputSample -= dram->oN[allpasstemp]*constallpass;
				dram->oN[outN] = inputSample;
				inputSample *= constallpass;
				outN--; if (outN < 0 || outN > delayN) {outN = delayN;}
				inputSample += (dram->oN[outN]);
				//allpass filter N
				
				dN[6] = dN[5];
				dN[5] = dN[4];
				dN[4] = inputSample;
				inputSample = (dN[4] + dN[5] + dN[6])/3.0f;
				
				allpasstemp = outO - 1;
				if (allpasstemp < 0 || allpasstemp > delayO) {allpasstemp = delayO;}
				inputSample -= dram->oO[allpasstemp]*constallpass;
				dram->oO[outO] = inputSample;
				inputSample *= constallpass;
				outO--; if (outO < 0 || outO > delayO) {outO = delayO;}
				inputSample += (dram->oO[outO]);
				//allpass filter O
				
				dO[6] = dO[5];
				dO[5] = dO[4];
				dO[4] = inputSample;
				inputSample = (dO[4] + dO[5] + dO[6])/3.0f;
				
				allpasstemp = outP - 1;
				if (allpasstemp < 0 || allpasstemp > delayP) {allpasstemp = delayP;}
				inputSample -= dram->oP[allpasstemp]*constallpass;
				dram->oP[outP] = inputSample;
				inputSample *= constallpass;
				outP--; if (outP < 0 || outP > delayP) {outP = delayP;}
				inputSample += (dram->oP[outP]);
				//allpass filter P
				
				dP[6] = dP[5];
				dP[5] = dP[4];
				dP[4] = inputSample;
				inputSample = (dP[4] + dP[5] + dP[6])/3.0f;
				
				allpasstemp = outQ - 1;
				if (allpasstemp < 0 || allpasstemp > delayQ) {allpasstemp = delayQ;}
				inputSample -= dram->oQ[allpasstemp]*constallpass;
				dram->oQ[outQ] = inputSample;
				inputSample *= constallpass;
				outQ--; if (outQ < 0 || outQ > delayQ) {outQ = delayQ;}
				inputSample += (dram->oQ[outQ]);
				//allpass filter Q
				
				dQ[6] = dQ[5];
				dQ[5] = dQ[4];
				dQ[4] = inputSample;
				inputSample = (dQ[4] + dQ[5] + dQ[6])/3.0f;
				
				allpasstemp = outR - 1;
				if (allpasstemp < 0 || allpasstemp > delayR) {allpasstemp = delayR;}
				inputSample -= dram->oR[allpasstemp]*constallpass;
				dram->oR[outR] = inputSample;
				inputSample *= constallpass;
				outR--; if (outR < 0 || outR > delayR) {outR = delayR;}
				inputSample += (dram->oR[outR]);
				//allpass filter R
				
				dR[6] = dR[5];
				dR[5] = dR[4];
				dR[4] = inputSample;
				inputSample = (dR[4] + dR[5] + dR[6])/3.0f;
				
				allpasstemp = outS - 1;
				if (allpasstemp < 0 || allpasstemp > delayS) {allpasstemp = delayS;}
				inputSample -= dram->oS[allpasstemp]*constallpass;
				dram->oS[outS] = inputSample;
				inputSample *= constallpass;
				outS--; if (outS < 0 || outS > delayS) {outS = delayS;}
				inputSample += (dram->oS[outS]);
				//allpass filter S
				
				dS[6] = dS[5];
				dS[5] = dS[4];
				dS[4] = inputSample;
				inputSample = (dS[4] + dS[5] + dS[6])/3.0f;
				
				allpasstemp = outT - 1;
				if (allpasstemp < 0 || allpasstemp > delayT) {allpasstemp = delayT;}
				inputSample -= dram->oT[allpasstemp]*constallpass;
				dram->oT[outT] = inputSample;
				inputSample *= constallpass;
				outT--; if (outT < 0 || outT > delayT) {outT = delayT;}
				inputSample += (dram->oT[outT]);
				//allpass filter T
				
				dT[6] = dT[5];
				dT[5] = dT[4];
				dT[4] = inputSample;
				inputSample = (dT[4] + dT[5] + dT[6])/3.0f;
				
				allpasstemp = outU - 1;
				if (allpasstemp < 0 || allpasstemp > delayU) {allpasstemp = delayU;}
				inputSample -= dram->oU[allpasstemp]*constallpass;
				dram->oU[outU] = inputSample;
				inputSample *= constallpass;
				outU--; if (outU < 0 || outU > delayU) {outU = delayU;}
				inputSample += (dram->oU[outU]);
				//allpass filter U
				
				dU[6] = dU[5];
				dU[5] = dU[4];
				dU[4] = inputSample;
				inputSample = (dU[4] + dU[5] + dU[6])/3.0f;
				
				allpasstemp = outV - 1;
				if (allpasstemp < 0 || allpasstemp > delayV) {allpasstemp = delayV;}
				inputSample -= dram->oV[allpasstemp]*constallpass;
				dram->oV[outV] = inputSample;
				inputSample *= constallpass;
				outV--; if (outV < 0 || outV > delayV) {outV = delayV;}
				inputSample += (dram->oV[outV]);
				//allpass filter V
				
				dV[6] = dV[5];
				dV[5] = dV[4];
				dV[4] = inputSample;
				inputSample = (dV[4] + dV[5] + dV[6])/3.0f;
				
				allpasstemp = outW - 1;
				if (allpasstemp < 0 || allpasstemp > delayW) {allpasstemp = delayW;}
				inputSample -= dram->oW[allpasstemp]*constallpass;
				dram->oW[outW] = inputSample;
				inputSample *= constallpass;
				outW--; if (outW < 0 || outW > delayW) {outW = delayW;}
				inputSample += (dram->oW[outW]);
				//allpass filter W
				
				dW[6] = dW[5];
				dW[5] = dW[4];
				dW[4] = inputSample;
				inputSample = (dW[4] + dW[5] + dW[6])/3.0f;
				
				allpasstemp = outX - 1;
				if (allpasstemp < 0 || allpasstemp > delayX) {allpasstemp = delayX;}
				inputSample -= dram->oX[allpasstemp]*constallpass;
				dram->oX[outX] = inputSample;
				inputSample *= constallpass;
				outX--; if (outX < 0 || outX > delayX) {outX = delayX;}
				inputSample += (dram->oX[outX]);
				//allpass filter X
				
				dX[6] = dX[5];
				dX[5] = dX[4];
				dX[4] = inputSample;
				inputSample = (dX[4] + dX[5] + dX[6])/3.0f;
				
				allpasstemp = outY - 1;
				if (allpasstemp < 0 || allpasstemp > delayY) {allpasstemp = delayY;}
				inputSample -= dram->oY[allpasstemp]*constallpass;
				dram->oY[outY] = inputSample;
				inputSample *= constallpass;
				outY--; if (outY < 0 || outY > delayY) {outY = delayY;}
				inputSample += (dram->oY[outY]);
				//allpass filter Y
				
				dY[6] = dY[5];
				dY[5] = dY[4];
				dY[4] = inputSample;
				inputSample = (dY[4] + dY[5] + dY[6])/3.0f;
				
				allpasstemp = outZ - 1;
				if (allpasstemp < 0 || allpasstemp > delayZ) {allpasstemp = delayZ;}
				inputSample -= dram->oZ[allpasstemp]*constallpass;
				dram->oZ[outZ] = inputSample;
				inputSample *= constallpass;
				outZ--; if (outZ < 0 || outZ > delayZ) {outZ = delayZ;}
				inputSample += (dram->oZ[outZ]);
				//allpass filter Z
				
				dZ[6] = dZ[5];
				dZ[5] = dZ[4];
				dZ[4] = inputSample;
				inputSample = (dZ[4] + dZ[5] + dZ[6]);		
				//output Zarathustra infinite space verb
				break;
				
		}
		//end big switch for verb type
		
		
		
		bridgerectifier = fabs(inputSample);
		bridgerectifier = 1.0f-cos(bridgerectifier);
		if (inputSample > 0) inputSample -= bridgerectifier;
		else inputSample += bridgerectifier;
		inputSample /= gain;		
		//here we apply the ADT2 'console on steroids' trick

		wetness = wetnesstarget;
		//setting up verb wetness to be manipulated by gate and peak
		
		wetness *= peak;
		//but we only use peak (indirect) to deal with dry/wet, so that it'll manipulate the dry/wet like we want
		
		drySample *= (1.0f-wetness);
		inputSample *= wetness;
		inputSample += drySample;
		//here we combine the tanks with the dry signal

		
		

		*destP = inputSample;
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	int count;
	for(count = 0; count < 6; count++) {dA[count] = 0.0; dB[count] = 0.0; dC[count] = 0.0; dD[count] = 0.0; dE[count] = 0.0;
	dF[count] = 0.0; dG[count] = 0.0; dH[count] = 0.0; dI[count] = 0.0; dJ[count] = 0.0; dK[count] = 0.0; dL[count] = 0.0;
	dM[count] = 0.0; dN[count] = 0.0; dO[count] = 0.0; dP[count] = 0.0; dQ[count] = 0.0; dR[count] = 0.0; dS[count] = 0.0;
	dT[count] = 0.0; dU[count] = 0.0; dV[count] = 0.0; dW[count] = 0.0; dX[count] = 0.0; dY[count] = 0.0; dZ[count] = 0.0;}
	
	for(count = 0; count < 15149; count++) {dram->aA[count] = 0.0;}
	for(count = 0; count < 15149; count++) {dram->oA[count] = 0.0;}
	for(count = 0; count < 14617; count++) {dram->aB[count] = 0.0;}
	for(count = 0; count < 14617; count++) {dram->oB[count] = 0.0;}
	for(count = 0; count < 14357; count++) {dram->aC[count] = 0.0;}
	for(count = 0; count < 14357; count++) {dram->oC[count] = 0.0;}
	for(count = 0; count < 13817; count++) {dram->aD[count] = 0.0;}
	for(count = 0; count < 13817; count++) {dram->oD[count] = 0.0;}
	for(count = 0; count < 13561; count++) {dram->aE[count] = 0.0;}
	for(count = 0; count < 13561; count++) {dram->oE[count] = 0.0;}
	for(count = 0; count < 13045; count++) {dram->aF[count] = 0.0;}
	for(count = 0; count < 13045; count++) {dram->oF[count] = 0.0;}
	for(count = 0; count < 11965; count++) {dram->aG[count] = 0.0;}
	for(count = 0; count < 11965; count++) {dram->oG[count] = 0.0;}
	for(count = 0; count < 11129; count++) {dram->aH[count] = 0.0;}
	for(count = 0; count < 11129; count++) {dram->oH[count] = 0.0;}
	for(count = 0; count < 10597; count++) {dram->aI[count] = 0.0;}
	for(count = 0; count < 10597; count++) {dram->oI[count] = 0.0;}
	for(count = 0; count < 9809; count++) {dram->aJ[count] = 0.0;}
	for(count = 0; count < 9809; count++) {dram->oJ[count] = 0.0;}
	for(count = 0; count < 9521; count++) {dram->aK[count] = 0.0;}
	for(count = 0; count < 9521; count++) {dram->oK[count] = 0.0;}
	for(count = 0; count < 8981; count++) {dram->aL[count] = 0.0;}
	for(count = 0; count < 8981; count++) {dram->oL[count] = 0.0;}
	for(count = 0; count < 8785; count++) {dram->aM[count] = 0.0;}
	for(count = 0; count < 8785; count++) {dram->oM[count] = 0.0;}
	for(count = 0; count < 8461; count++) {dram->aN[count] = 0.0;}
	for(count = 0; count < 8461; count++) {dram->oN[count] = 0.0;}
	for(count = 0; count < 8309; count++) {dram->aO[count] = 0.0;}
	for(count = 0; count < 8309; count++) {dram->oO[count] = 0.0;}
	for(count = 0; count < 7981; count++) {dram->aP[count] = 0.0;}
	for(count = 0; count < 7981; count++) {dram->oP[count] = 0.0;}
	for(count = 0; count < 7321; count++) {dram->aQ[count] = 0.0;}
	for(count = 0; count < 7321; count++) {dram->oQ[count] = 0.0;}
	for(count = 0; count < 6817; count++) {dram->aR[count] = 0.0;}
	for(count = 0; count < 6817; count++) {dram->oR[count] = 0.0;}
	for(count = 0; count < 6505; count++) {dram->aS[count] = 0.0;}
	for(count = 0; count < 6505; count++) {dram->oS[count] = 0.0;}
	for(count = 0; count < 6001; count++) {dram->aT[count] = 0.0;}
	for(count = 0; count < 6001; count++) {dram->oT[count] = 0.0;}
	for(count = 0; count < 5837; count++) {dram->aU[count] = 0.0;}
	for(count = 0; count < 5837; count++) {dram->oU[count] = 0.0;}
	for(count = 0; count < 5501; count++) {dram->aV[count] = 0.0;}
	for(count = 0; count < 5501; count++) {dram->oV[count] = 0.0;}
	for(count = 0; count < 5009; count++) {dram->aW[count] = 0.0;}
	for(count = 0; count < 5009; count++) {dram->oW[count] = 0.0;}
	for(count = 0; count < 4849; count++) {dram->aX[count] = 0.0;}
	for(count = 0; count < 4849; count++) {dram->oX[count] = 0.0;}
	for(count = 0; count < 4295; count++) {dram->aY[count] = 0.0;}
	for(count = 0; count < 4295; count++) {dram->oY[count] = 0.0;}
	for(count = 0; count < 4179; count++) {dram->aZ[count] = 0.0;}	
	for(count = 0; count < 4179; count++) {dram->oZ[count] = 0.0;}
	
	
	outA = 1; alpA = 1; delayA = 4; maxdelayA = 7573;
	outB = 1; alpB = 1; delayB = 4; maxdelayB = 7307;
	outC = 1; alpC = 1; delayC = 4; maxdelayC = 7177;
	outD = 1; alpD = 1; delayD = 4; maxdelayD = 6907;
	outE = 1; alpE = 1; delayE = 4; maxdelayE = 6779;
	outF = 1; alpF = 1; delayF = 4; maxdelayF = 6521;
	outG = 1; alpG = 1; delayG = 4; maxdelayG = 5981;
	outH = 1; alpH = 1; delayH = 4; maxdelayH = 5563;
	outI = 1; alpI = 1; delayI = 4; maxdelayI = 5297;
	outJ = 1; alpJ = 1; delayJ = 4; maxdelayJ = 4903;
	outK = 1; alpK = 1; delayK = 4; maxdelayK = 4759;
	outL = 1; alpL = 1; delayL = 4; maxdelayL = 4489;
	outM = 1; alpM = 1; delayM = 4; maxdelayM = 4391;
	outN = 1; alpN = 1; delayN = 4; maxdelayN = 4229;
	outO = 1; alpO = 1; delayO = 4; maxdelayO = 4153;
	outP = 1; alpP = 1; delayP = 4; maxdelayP = 3989;
	outQ = 1; alpQ = 1; delayQ = 4; maxdelayQ = 3659;
	outR = 1; alpR = 1; delayR = 4; maxdelayR = 3407;
	outS = 1; alpS = 1; delayS = 4; maxdelayS = 3251;
	outT = 1; alpT = 1; delayT = 4; maxdelayT = 2999;
	outU = 1; alpU = 1; delayU = 4; maxdelayU = 2917;
	outV = 1; alpV = 1; delayV = 4; maxdelayV = 2749;
	outW = 1; alpW = 1; delayW = 4; maxdelayW = 2503;
	outX = 1; alpX = 1; delayX = 4; maxdelayX = 2423;
	outY = 1; alpY = 1; delayY = 4; maxdelayY = 2146;
	outZ = 1; alpZ = 1; delayZ = 4; maxdelayZ = 2088;
	
	savedRoomsize = -1.0; //force update to begin
	countdown = -1;
	peak = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
