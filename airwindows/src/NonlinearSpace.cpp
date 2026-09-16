#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "NonlinearSpace"
#define AIRWINDOWS_DESCRIPTION "A flexible reverb plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','N','o','f' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
static const int k16 = 1;
static const int k32 = 2;
static const int k44 = 3;
static const int k48 = 4;
static const int k64 = 5;
static const int k88 = 6;
static const int k96 = 7;
static const int kDefaultValue_ParamOne = k44;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static char const * const enumStrings0[] = { "", "16K", "32K", "44.1K", "48K", "64K", "88.2K", "96K", };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Sample Rate", .min = 1, .max = 7, .def = 3, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "Liveness", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Nonlin", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	Float32 avgInputL;
	Float32 avgOutputL;
	Float32 avg2InputL;
	Float32 avg2OutputL;
	Float32 avgInputR;
	Float32 avgOutputR;
	Float32 avg2InputR;
	Float32 avg2OutputR;
	Float32 a2vgInputL;
	Float32 a2vgOutputL;
	Float32 a2vg2InputL;
	Float32 a2vg2OutputL;
	Float32 a2vgInputR;
	Float32 a2vgOutputR;
	Float32 a2vg2InputR;
	Float32 a2vg2OutputR;
	
	Float32 verboutL;
	Float32 verboutR;
	Float32 iirCCSampleL;
	Float32 iirCCSampleR;
	Float32 iirSampleL;
	Float32 iirSampleR;
	
	
	
	
	Float32 interpolA, pitchshiftA; //7575
	Float32 interpolB, pitchshiftB; //7309
	Float32 interpolC, pitchshiftC; //7179
	Float32 interpolD, pitchshiftD; //6909		
	Float32 interpolE, pitchshiftE; //6781
	Float32 interpolF, pitchshiftF; //6523
	Float32 interpolG, pitchshiftG; //5983
	Float32 interpolH, pitchshiftH; //5565
	Float32 interpolI, pitchshiftI; //5299
	Float32 interpolJ, pitchshiftJ; //4905
	Float32 interpolK, pitchshiftK; //4761
	Float32 interpolL, pitchshiftL; //4491
	Float32 interpolM, pitchshiftM; //4393
	Float32 interpolN, pitchshiftN; //4231
	Float32 interpolO, pitchshiftO; //4155
	Float32 interpolP, pitchshiftP; //3991
	Float32 interpolQ, pitchshiftQ; //3661
	Float32 interpolR, pitchshiftR; //3409
	Float32 interpolS, pitchshiftS; //3253
	Float32 interpolT, pitchshiftT; //3001
	Float32 interpolU, pitchshiftU; //2919
	Float32 interpolV, pitchshiftV; //2751
	Float32 interpolW, pitchshiftW; //2505
	Float32 interpolX, pitchshiftX; //2425
	Float32 interpolY, pitchshiftY; //2148
	Float32 interpolZ, pitchshiftZ; //2090
	
	int oneMid, delayMid, maxdelayMid;
	int oneSide, delaySide, maxdelaySide;
	int oneLeft, delayLeft, maxdelayLeft;
	int oneRight, delayRight, maxdelayRight;
	
	int onepre, delaypre, maxdelaypre;
	
	int oneA, twoA, treA, delayA, maxdelayA;
	int oneB, twoB, treB, delayB, maxdelayB;
	int oneC, twoC, treC, delayC, maxdelayC;
	int oneD, twoD, treD, delayD, maxdelayD;
	int oneE, twoE, treE, delayE, maxdelayE;
	int oneF, twoF, treF, delayF, maxdelayF;
	int oneG, twoG, treG, delayG, maxdelayG;
	int oneH, twoH, treH, delayH, maxdelayH;
	int oneI, twoI, treI, delayI, maxdelayI;
	int oneJ, twoJ, treJ, delayJ, maxdelayJ;
	int oneK, twoK, treK, delayK, maxdelayK;
	int oneL, twoL, treL, delayL, maxdelayL;
	int oneM, twoM, treM, delayM, maxdelayM;
	int oneN, twoN, treN, delayN, maxdelayN;
	int oneO, twoO, treO, delayO, maxdelayO;
	int oneP, twoP, treP, delayP, maxdelayP;
	int oneQ, twoQ, treQ, delayQ, maxdelayQ;
	int oneR, twoR, treR, delayR, maxdelayR;
	int oneS, twoS, treS, delayS, maxdelayS;
	int oneT, twoT, treT, delayT, maxdelayT;
	int oneU, twoU, treU, delayU, maxdelayU;
	int oneV, twoV, treV, delayV, maxdelayV;
	int oneW, twoW, treW, delayW, maxdelayW;
	int oneX, twoX, treX, delayX, maxdelayX;
	int oneY, twoY, treY, delayY, maxdelayY;
	int oneZ, twoZ, treZ, delayZ, maxdelayZ;
	Float32 savedPredelay;
	Float32 savedRoomsize;
	int countdown;
	
	Float32 lowpassSampleAA;
	Float32 lowpassSampleAB;
	Float32 lowpassSampleBA;
	Float32 lowpassSampleBB;
	Float32 lowpassSampleCA;
	Float32 lowpassSampleCB;
	Float32 lowpassSampleDA;
	Float32 lowpassSampleDB;
	Float32 lowpassSampleE;
	Float32 lowpassSampleF;
	Float32 lowpassSampleG;
	
	Float32 rowpassSampleAA;
	Float32 rowpassSampleAB;
	Float32 rowpassSampleBA;
	Float32 rowpassSampleBB;
	Float32 rowpassSampleCA;
	Float32 rowpassSampleCB;
	Float32 rowpassSampleDA;
	Float32 rowpassSampleDB;
	Float32 rowpassSampleE;
	Float32 rowpassSampleF;
	Float32 rowpassSampleG;
	
	bool flip;
	
	Float32 nonlin;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		Float32 dMid[2348];
	Float32 dSide[1334];
	Float32 dLeft[5924];
	Float32 dRight[5926];
	Float32 dpreR[7575];
	Float32 dpreL[7575];
	Float32 dA[7575];
	Float32 dB[7309];
	Float32 dC[7179];
	Float32 dD[6909];		
	Float32 dE[6781];
	Float32 dF[6523];
	Float32 dG[5983];
	Float32 dH[5565];
	Float32 dI[5299];
	Float32 dJ[4905];
	Float32 dK[4761];
	Float32 dL[4491];
	Float32 dM[4393];
	Float32 dN[4231];
	Float32 dO[4155];
	Float32 dP[3991];
	Float32 dQ[3661];
	Float32 dR[3409];
	Float32 dS[3253];
	Float32 dT[3001];
	Float32 dU[2919];
	Float32 dV[2751];
	Float32 dW[2505];
	Float32 dX[2425];
	Float32 dY[2148];
	Float32 dZ[2090];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float drySampleL;
	float drySampleR;
	float inputSampleL;
	float inputSampleR;
	float mid;
	float side;
	Float32 overallscale = 1.0f;
	int samplerate = (int) GetParameter( kParam_One );
	switch (samplerate)
	{
		case 1: overallscale *= (16/44.1f); break; //16
		case 2: overallscale *= (32/44.1f); break; //32
		case 3: overallscale *= 1.0f; break; //44.1f
		case 4: overallscale *= (48/44.1f); break; //48
		case 5: overallscale *= (64/44.1f); break; //64
		case 6: overallscale *= 2.0f; break; //88.2f
		case 7: overallscale *= (96/44.1f); break; //96
	}
	nonlin *= 0.001f; //scale suitably to apply to our liveness value
	Float32 basefeedback = 0.45f + (nonlin * powf(GetParameter( kParam_Five ),3)); //nonlin from previous sample, positive adds liveness when loud
	nonlin = 0.0f; //reset it here for setting up again next time
	Float32 tankfeedback = basefeedback + (powf(GetParameter( kParam_Two ),2) * 0.05f);
	//liveness
	if (tankfeedback > 0.5f) tankfeedback = 0.5f;
	if (tankfeedback < 0.4f) tankfeedback = 0.4f;
	Float32 iirAmountC = 1.0f-powf(1.0f-GetParameter( kParam_Three ),2);
	//most of the range is up at the top end
	iirAmountC += (iirAmountC/overallscale);
	iirAmountC /= 2.0f;
	if (iirAmountC > 1.1f) iirAmountC = 1.1f;
	//lowpass, check to see if it's working reasonably at 96K
	Float32 iirAmount = (((1.0f-powf(GetParameter( kParam_Four ),2)) * 0.09f)/overallscale)+0.001f;
	if (iirAmount > 1.0f) iirAmount = 1.0f;
	if (iirAmount < 0.001f) iirAmount = 0.001f;
	Float32 wetness = GetParameter( kParam_Six );
	Float32 dryness = 1.0f - wetness;
	Float32 roomsize = overallscale*0.203f;
	Float32 lean = 0.125f;
	Float32 invlean = 1.0f - lean;
	Float32 pspeed = 0.145f;
	Float32 outcouple = 0.5f - tankfeedback;
	Float32 constallpass = 0.618033988749894848204586f; //golden ratio!
	Float32 temp;
	int allpasstemp;
	Float32 predelay = 0.222f * overallscale;
	
	//reverb setup
	
	delayA = (int(maxdelayA * roomsize));
	delayB = (int(maxdelayB * roomsize));
	delayC = (int(maxdelayC * roomsize));
	delayD = (int(maxdelayD * roomsize));
	delayE = (int(maxdelayE * roomsize));
	delayF = (int(maxdelayF * roomsize));
	delayG = (int(maxdelayG * roomsize));
	delayH = (int(maxdelayH * roomsize));
	delayI = (int(maxdelayI * roomsize));
	delayJ = (int(maxdelayJ * roomsize));
	delayK = (int(maxdelayK * roomsize));
	delayL = (int(maxdelayL * roomsize));
	delayM = (int(maxdelayM * roomsize));
	delayN = (int(maxdelayN * roomsize));
	delayO = (int(maxdelayO * roomsize));
	delayP = (int(maxdelayP * roomsize));
	delayQ = (int(maxdelayQ * roomsize));
	delayR = (int(maxdelayR * roomsize));
	delayS = (int(maxdelayS * roomsize));
	delayT = (int(maxdelayT * roomsize));
	delayU = (int(maxdelayU * roomsize));
	delayV = (int(maxdelayV * roomsize));
	delayW = (int(maxdelayW * roomsize));
	delayX = (int(maxdelayX * roomsize));
	delayY = (int(maxdelayY * roomsize));
	delayZ = (int(maxdelayZ * roomsize));
	delayMid = (int(maxdelayMid * roomsize));
	delaySide = (int(maxdelaySide * roomsize));
	delayLeft = (int(maxdelayLeft * roomsize));
	delayRight = (int(maxdelayRight * roomsize));
	delaypre = (int(maxdelaypre * predelay));
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;
		
		
		dram->dpreL[onepre] = inputSampleL;
		dram->dpreR[onepre] = inputSampleR;
		onepre--; if (onepre < 0 || onepre > delaypre) {onepre = delaypre;}
		inputSampleL = (dram->dpreL[onepre]);
		inputSampleR = (dram->dpreR[onepre]);
		//predelay
		
		interpolA += pitchshiftA*pspeed;
		interpolB += pitchshiftB*pspeed;
		interpolC += pitchshiftC*pspeed;
		interpolD += pitchshiftD*pspeed;
		interpolE += pitchshiftE*pspeed;
		interpolF += pitchshiftF*pspeed;
		interpolG += pitchshiftG*pspeed;
		interpolH += pitchshiftH*pspeed;
		interpolI += pitchshiftI*pspeed;
		interpolJ += pitchshiftJ*pspeed;
		interpolK += pitchshiftK*pspeed;
		interpolL += pitchshiftL*pspeed;
		interpolM += pitchshiftM*pspeed;
		interpolN += pitchshiftN*pspeed;
		interpolO += pitchshiftO*pspeed;
		interpolP += pitchshiftP*pspeed;
		interpolQ += pitchshiftQ*pspeed;
		interpolR += pitchshiftR*pspeed;
		interpolS += pitchshiftS*pspeed;
		interpolT += pitchshiftT*pspeed;
		interpolU += pitchshiftU*pspeed;
		interpolV += pitchshiftV*pspeed;
		interpolW += pitchshiftW*pspeed;
		interpolX += pitchshiftX*pspeed;
		interpolY += pitchshiftY*pspeed;
		interpolZ += pitchshiftZ*pspeed;
		//increment all the sub-sample offsets for the pitch shifting of combs
		
		if (interpolA > 1.0f) {pitchshiftA = -fabs(pitchshiftA); interpolA += pitchshiftA*pspeed;}
		if (interpolB > 1.0f) {pitchshiftB = -fabs(pitchshiftB); interpolB += pitchshiftB*pspeed;}
		if (interpolC > 1.0f) {pitchshiftC = -fabs(pitchshiftC); interpolC += pitchshiftC*pspeed;}
		if (interpolD > 1.0f) {pitchshiftD = -fabs(pitchshiftD); interpolD += pitchshiftD*pspeed;}
		if (interpolE > 1.0f) {pitchshiftE = -fabs(pitchshiftE); interpolE += pitchshiftE*pspeed;}
		if (interpolF > 1.0f) {pitchshiftF = -fabs(pitchshiftF); interpolF += pitchshiftF*pspeed;}
		if (interpolG > 1.0f) {pitchshiftG = -fabs(pitchshiftG); interpolG += pitchshiftG*pspeed;}
		if (interpolH > 1.0f) {pitchshiftH = -fabs(pitchshiftH); interpolH += pitchshiftH*pspeed;}
		if (interpolI > 1.0f) {pitchshiftI = -fabs(pitchshiftI); interpolI += pitchshiftI*pspeed;}
		if (interpolJ > 1.0f) {pitchshiftJ = -fabs(pitchshiftJ); interpolJ += pitchshiftJ*pspeed;}
		if (interpolK > 1.0f) {pitchshiftK = -fabs(pitchshiftK); interpolK += pitchshiftK*pspeed;}
		if (interpolL > 1.0f) {pitchshiftL = -fabs(pitchshiftL); interpolL += pitchshiftL*pspeed;}
		if (interpolM > 1.0f) {pitchshiftM = -fabs(pitchshiftM); interpolM += pitchshiftM*pspeed;}
		if (interpolN > 1.0f) {pitchshiftN = -fabs(pitchshiftN); interpolN += pitchshiftN*pspeed;}
		if (interpolO > 1.0f) {pitchshiftO = -fabs(pitchshiftO); interpolO += pitchshiftO*pspeed;}
		if (interpolP > 1.0f) {pitchshiftP = -fabs(pitchshiftP); interpolP += pitchshiftP*pspeed;}
		if (interpolQ > 1.0f) {pitchshiftQ = -fabs(pitchshiftQ); interpolQ += pitchshiftQ*pspeed;}
		if (interpolR > 1.0f) {pitchshiftR = -fabs(pitchshiftR); interpolR += pitchshiftR*pspeed;}
		if (interpolS > 1.0f) {pitchshiftS = -fabs(pitchshiftS); interpolS += pitchshiftS*pspeed;}
		if (interpolT > 1.0f) {pitchshiftT = -fabs(pitchshiftT); interpolT += pitchshiftT*pspeed;}
		if (interpolU > 1.0f) {pitchshiftU = -fabs(pitchshiftU); interpolU += pitchshiftU*pspeed;}
		if (interpolV > 1.0f) {pitchshiftV = -fabs(pitchshiftV); interpolV += pitchshiftV*pspeed;}
		if (interpolW > 1.0f) {pitchshiftW = -fabs(pitchshiftW); interpolW += pitchshiftW*pspeed;}
		if (interpolX > 1.0f) {pitchshiftX = -fabs(pitchshiftX); interpolX += pitchshiftX*pspeed;}
		if (interpolY > 1.0f) {pitchshiftY = -fabs(pitchshiftY); interpolY += pitchshiftY*pspeed;}
		if (interpolZ > 1.0f) {pitchshiftZ = -fabs(pitchshiftZ); interpolZ += pitchshiftZ*pspeed;}
		
		if (interpolA < 0.0f) {pitchshiftA = fabs(pitchshiftA); interpolA += pitchshiftA*pspeed;}
		if (interpolB < 0.0f) {pitchshiftB = fabs(pitchshiftB); interpolB += pitchshiftB*pspeed;}
		if (interpolC < 0.0f) {pitchshiftC = fabs(pitchshiftC); interpolC += pitchshiftC*pspeed;}
		if (interpolD < 0.0f) {pitchshiftD = fabs(pitchshiftD); interpolD += pitchshiftD*pspeed;}
		if (interpolE < 0.0f) {pitchshiftE = fabs(pitchshiftE); interpolE += pitchshiftE*pspeed;}
		if (interpolF < 0.0f) {pitchshiftF = fabs(pitchshiftF); interpolF += pitchshiftF*pspeed;}
		if (interpolG < 0.0f) {pitchshiftG = fabs(pitchshiftG); interpolG += pitchshiftG*pspeed;}
		if (interpolH < 0.0f) {pitchshiftH = fabs(pitchshiftH); interpolH += pitchshiftH*pspeed;}
		if (interpolI < 0.0f) {pitchshiftI = fabs(pitchshiftI); interpolI += pitchshiftI*pspeed;}
		if (interpolJ < 0.0f) {pitchshiftJ = fabs(pitchshiftJ); interpolJ += pitchshiftJ*pspeed;}
		if (interpolK < 0.0f) {pitchshiftK = fabs(pitchshiftK); interpolK += pitchshiftK*pspeed;}
		if (interpolL < 0.0f) {pitchshiftL = fabs(pitchshiftL); interpolL += pitchshiftL*pspeed;}
		if (interpolM < 0.0f) {pitchshiftM = fabs(pitchshiftM); interpolM += pitchshiftM*pspeed;}
		if (interpolN < 0.0f) {pitchshiftN = fabs(pitchshiftN); interpolN += pitchshiftN*pspeed;}
		if (interpolO < 0.0f) {pitchshiftO = fabs(pitchshiftO); interpolO += pitchshiftO*pspeed;}
		if (interpolP < 0.0f) {pitchshiftP = fabs(pitchshiftP); interpolP += pitchshiftP*pspeed;}
		if (interpolQ < 0.0f) {pitchshiftQ = fabs(pitchshiftQ); interpolQ += pitchshiftQ*pspeed;}
		if (interpolR < 0.0f) {pitchshiftR = fabs(pitchshiftR); interpolR += pitchshiftR*pspeed;}
		if (interpolS < 0.0f) {pitchshiftS = fabs(pitchshiftS); interpolS += pitchshiftS*pspeed;}
		if (interpolT < 0.0f) {pitchshiftT = fabs(pitchshiftT); interpolT += pitchshiftT*pspeed;}
		if (interpolU < 0.0f) {pitchshiftU = fabs(pitchshiftU); interpolU += pitchshiftU*pspeed;}
		if (interpolV < 0.0f) {pitchshiftV = fabs(pitchshiftV); interpolV += pitchshiftV*pspeed;}
		if (interpolW < 0.0f) {pitchshiftW = fabs(pitchshiftW); interpolW += pitchshiftW*pspeed;}
		if (interpolX < 0.0f) {pitchshiftX = fabs(pitchshiftX); interpolX += pitchshiftX*pspeed;}
		if (interpolY < 0.0f) {pitchshiftY = fabs(pitchshiftY); interpolY += pitchshiftY*pspeed;}
		if (interpolZ < 0.0f) {pitchshiftZ = fabs(pitchshiftZ); interpolZ += pitchshiftZ*pspeed;}
		//all of the sanity checks for interpol for all combs
		
		if (verboutR > 1.0f) verboutR = 1.0f;
		if (verboutR < -1.0f) verboutR = -1.0f;
		if (verboutL > 1.0f) verboutL = 1.0f;
		if (verboutL < -1.0f) verboutL = -1.0f;
		
		inputSampleL += verboutR;
		inputSampleR += verboutL;
		verboutL = 0.0f;
		verboutR = 0.0f;
		//here we add in the cross-coupling- output of L tank to R, output of R tank to L
		
		
		mid = inputSampleL + inputSampleR;
		side = inputSampleL - inputSampleR;
		//assign mid and side.	
		
		allpasstemp = oneMid - 1;
		if (allpasstemp < 0 || allpasstemp > delayMid) {allpasstemp = delayMid;}
		mid -= dram->dMid[allpasstemp]*constallpass;
		dram->dMid[oneMid] = mid;
		mid *= constallpass;
		oneMid--; if (oneMid < 0 || oneMid > delayMid) {oneMid = delayMid;}
		mid += (dram->dMid[oneMid]);
		nonlin += fabs(dram->dMid[oneMid]);
		//allpass filter mid		
		
		allpasstemp = oneSide - 1;
		if (allpasstemp < 0 || allpasstemp > delaySide) {allpasstemp = delaySide;}
		side -= dram->dSide[allpasstemp]*constallpass;
		dram->dSide[oneSide] = side;
		side *= constallpass;
		oneSide--; if (oneSide < 0 || oneSide > delaySide) {oneSide = delaySide;}
		side += (dram->dSide[oneSide]);
		nonlin += fabs(dram->dSide[oneSide]);
		//allpass filter side
		
		//here we do allpasses on the mid and side
		
		allpasstemp = oneLeft - 1;
		if (allpasstemp < 0 || allpasstemp > delayLeft) {allpasstemp = delayLeft;}
		inputSampleL -= dram->dLeft[allpasstemp]*constallpass;
		dram->dLeft[oneLeft] = verboutL;
		inputSampleL *= constallpass;
		oneLeft--; if (oneLeft < 0 || oneLeft > delayLeft) {oneLeft = delayLeft;}
		inputSampleL += (dram->dLeft[oneLeft]);
		nonlin += fabs(dram->dLeft[oneLeft]);
		//allpass filter left
		
		
		allpasstemp = oneRight - 1;
		if (allpasstemp < 0 || allpasstemp > delayRight) {allpasstemp = delayRight;}
		inputSampleR -= dram->dRight[allpasstemp]*constallpass;
		dram->dRight[oneRight] = verboutR;
		inputSampleR *= constallpass;
		oneRight--; if (oneRight < 0 || oneRight > delayRight) {oneRight = delayRight;}
		inputSampleR += (dram->dRight[oneRight]);
		nonlin += fabs(dram->dRight[oneRight]);
		//allpass filter right
		
		
		inputSampleL += (mid+side)/2.0f;
		inputSampleR += (mid-side)/2.0f;
		//here we get back to a L/R topology by adding the mid/side in parallel with L/R
		
		
		
		temp = (dram->dA[oneA]*interpolA );
		temp += (dram->dA[treA]*( 1.0f - interpolA ));
		temp += ((dram->dA[twoA]));
		dram->dA[treA] = (temp*tankfeedback);
		dram->dA[treA] += inputSampleL;
		oneA--; if (oneA < 0 || oneA > delayA) {oneA = delayA;}
		twoA--; if (twoA < 0 || twoA > delayA) {twoA = delayA;}
		treA--; if (treA < 0 || treA > delayA) {treA = delayA;}
		temp = (dram->dA[oneA]*interpolA );
		temp += (dram->dA[treA]*( 1.0f - interpolA ));
		temp *=  (invlean + (lean*fabs(dram->dA[twoA])));
   		verboutL += temp;
		//comb filter A
		temp = (dram->dC[oneC]*interpolC );
		temp += (dram->dC[treC]*( 1.0f - interpolC ));
		temp += ((dram->dC[twoC]));
		dram->dC[treC] = (temp*tankfeedback);
		dram->dC[treC] += inputSampleL;
		oneC--; if (oneC < 0 || oneC > delayC) {oneC = delayC;}
		twoC--; if (twoC < 0 || twoC > delayC) {twoC = delayC;}
		treC--; if (treC < 0 || treC > delayC) {treC = delayC;}
		temp = (dram->dC[oneC]*interpolC );
		temp += (dram->dC[treC]*( 1.0f - interpolC ));
		temp *=  (invlean + (lean*fabs(dram->dC[twoC])));
   		verboutL += temp;
		//comb filter C
		temp = (dram->dE[oneE]*interpolE );
		temp += (dram->dE[treE]*( 1.0f - interpolE ));
		temp += ((dram->dE[twoE]));
		dram->dE[treE] = (temp*tankfeedback);
		dram->dE[treE] += inputSampleL;
		oneE--; if (oneE < 0 || oneE > delayE) {oneE = delayE;}
		twoE--; if (twoE < 0 || twoE > delayE) {twoE = delayE;}
		treE--; if (treE < 0 || treE > delayE) {treE = delayE;}
		temp = (dram->dE[oneE]*interpolE );
		temp += (dram->dE[treE]*( 1.0f - interpolE ));
		temp *=  (invlean + (lean*fabs(dram->dE[twoE])));
   		verboutL += temp;
		//comb filter E
		temp = (dram->dG[oneG]*interpolG );
		temp += (dram->dG[treG]*( 1.0f - interpolG ));
		temp += ((dram->dG[twoG]));
		dram->dG[treG] = (temp*tankfeedback);
		dram->dG[treG] += inputSampleL;
		oneG--; if (oneG < 0 || oneG > delayG) {oneG = delayG;}
		twoG--; if (twoG < 0 || twoG > delayG) {twoG = delayG;}
		treG--; if (treG < 0 || treG > delayG) {treG = delayG;}
		temp = (dram->dG[oneG]*interpolG );
		temp += (dram->dG[treG]*( 1.0f - interpolG ));
		temp *=  (invlean + (lean*fabs(dram->dG[twoG])));
   		verboutL += temp;
		//comb filter G
		temp = (dram->dI[oneI]*interpolI );
		temp += (dram->dI[treI]*( 1.0f - interpolI ));
		temp += ((dram->dI[twoI]));
		dram->dI[treI] = (temp*tankfeedback);
		dram->dI[treI] += inputSampleL;
		oneI--; if (oneI < 0 || oneI > delayI) {oneI = delayI;}
		twoI--; if (twoI < 0 || twoI > delayI) {twoI = delayI;}
		treI--; if (treI < 0 || treI > delayI) {treI = delayI;}
		temp = (dram->dI[oneI]*interpolI );
		temp += (dram->dI[treI]*( 1.0f - interpolI ));
		temp *=  (invlean + (lean*fabs(dram->dI[twoI])));
  		verboutL += temp;
		//comb filter I
		temp = (dram->dK[oneK]*interpolK );
		temp += (dram->dK[treK]*( 1.0f - interpolK ));
		temp += ((dram->dK[twoK]));
		dram->dK[treK] = (temp*tankfeedback);
		dram->dK[treK] += inputSampleL;
		oneK--; if (oneK < 0 || oneK > delayK) {oneK = delayK;}
		twoK--; if (twoK < 0 || twoK > delayK) {twoK = delayK;}
		treK--; if (treK < 0 || treK > delayK) {treK = delayK;}
		temp = (dram->dK[oneK]*interpolK );
		temp += (dram->dK[treK]*( 1.0f - interpolK ));
		temp *=  (invlean + (lean*fabs(dram->dK[twoK])));
  		verboutL += temp;
		//comb filter K
		temp = (dram->dM[oneM]*interpolM );
		temp += (dram->dM[treM]*( 1.0f - interpolM ));
		temp += ((dram->dM[twoM]));
		dram->dM[treM] = (temp*tankfeedback);
		dram->dM[treM] += inputSampleL;
		oneM--; if (oneM < 0 || oneM > delayM) {oneM = delayM;}
		twoM--; if (twoM < 0 || twoM > delayM) {twoM = delayM;}
		treM--; if (treM < 0 || treM > delayM) {treM = delayM;}
		temp = (dram->dM[oneM]*interpolM );
		temp += (dram->dM[treM]*( 1.0f - interpolM ));
		temp *=  (invlean + (lean*fabs(dram->dM[twoM])));
   		verboutL += temp;
		//comb filter M
		temp = (dram->dO[oneO]*interpolO );
		temp += (dram->dO[treO]*( 1.0f - interpolO ));
		temp += ((dram->dO[twoO]));
		dram->dO[treO] = (temp*tankfeedback);
		dram->dO[treO] += inputSampleL;
		oneO--; if (oneO < 0 || oneO > delayO) {oneO = delayO;}
		twoO--; if (twoO < 0 || twoO > delayO) {twoO = delayO;}
		treO--; if (treO < 0 || treO > delayO) {treO = delayO;}
		temp = (dram->dO[oneO]*interpolO );
		temp += (dram->dO[treO]*( 1.0f - interpolO ));
		temp *=  (invlean + (lean*fabs(dram->dO[twoO])));
  		verboutL += temp;
		//comb filter O
		temp = (dram->dQ[oneQ]*interpolQ );
		temp += (dram->dQ[treQ]*( 1.0f - interpolQ ));
		temp += ((dram->dQ[twoQ]));
		dram->dQ[treQ] = (temp*tankfeedback);
		dram->dQ[treQ] += inputSampleL;
		oneQ--; if (oneQ < 0 || oneQ > delayQ) {oneQ = delayQ;}
		twoQ--; if (twoQ < 0 || twoQ > delayQ) {twoQ = delayQ;}
		treQ--; if (treQ < 0 || treQ > delayQ) {treQ = delayQ;}
		temp = (dram->dQ[oneQ]*interpolQ );
		temp += (dram->dQ[treQ]*( 1.0f - interpolQ ));
		temp *=  (invlean + (lean*fabs(dram->dQ[twoQ])));
  		verboutL += temp;
		//comb filter Q
		temp = (dram->dS[oneS]*interpolS );
		temp += (dram->dS[treS]*( 1.0f - interpolS ));
		temp += ((dram->dS[twoS]));
		dram->dS[treS] = (temp*tankfeedback);
		dram->dS[treS] += inputSampleL;
		oneS--; if (oneS < 0 || oneS > delayS) {oneS = delayS;}
		twoS--; if (twoS < 0 || twoS > delayS) {twoS = delayS;}
		treS--; if (treS < 0 || treS > delayS) {treS = delayS;}
		temp = (dram->dS[oneS]*interpolS );
		temp += (dram->dS[treS]*( 1.0f - interpolS ));
		temp *=  (invlean + (lean*fabs(dram->dS[twoS])));
   		verboutL += temp;
		//comb filter S
		temp = (dram->dU[oneU]*interpolU );
		temp += (dram->dU[treU]*( 1.0f - interpolU ));
		temp += ((dram->dU[twoU]));
		dram->dU[treU] = (temp*tankfeedback);
		dram->dU[treU] += inputSampleL;
		oneU--; if (oneU < 0 || oneU > delayU) {oneU = delayU;}
		twoU--; if (twoU < 0 || twoU > delayU) {twoU = delayU;}
		treU--; if (treU < 0 || treU > delayU) {treU = delayU;}
		temp = (dram->dU[oneU]*interpolU );
		temp += (dram->dU[treU]*( 1.0f - interpolU ));
		temp *=  (invlean + (lean*fabs(dram->dU[twoU])));
  		verboutL += temp;
		//comb filter U
		temp = (dram->dW[oneW]*interpolW );
		temp += (dram->dW[treW]*( 1.0f - interpolW ));
		temp += ((dram->dW[twoW]));
		dram->dW[treW] = (temp*tankfeedback);
		dram->dW[treW] += inputSampleL;
		oneW--; if (oneW < 0 || oneW > delayW) {oneW = delayW;}
		twoW--; if (twoW < 0 || twoW > delayW) {twoW = delayW;}
		treW--; if (treW < 0 || treW > delayW) {treW = delayW;}
		temp = (dram->dW[oneW]*interpolW );
		temp += (dram->dW[treW]*( 1.0f - interpolW ));
		temp *=  (invlean + (lean*fabs(dram->dW[twoW])));
  		verboutL += temp;
		//comb filter W
		temp = (dram->dY[oneY]*interpolY );
		temp += (dram->dY[treY]*( 1.0f - interpolY ));
		temp += ((dram->dY[twoY]));
		dram->dY[treY] = (temp*tankfeedback);
		dram->dY[treY] += inputSampleL;
		oneY--; if (oneY < 0 || oneY > delayY) {oneY = delayY;}
		twoY--; if (twoY < 0 || twoY > delayY) {twoY = delayY;}
		treY--; if (treY < 0 || treY > delayY) {treY = delayY;}
		temp = (dram->dY[oneY]*interpolY );
		temp += (dram->dY[treY]*( 1.0f - interpolY ));
		temp *=  (invlean + (lean*fabs(dram->dY[twoY])));
  		verboutL += temp;
		//comb filter Y
		//here we do the L delay tank, every other letter A C E G I
		
		temp = (dram->dB[oneB]*interpolB );
		temp += (dram->dB[treB]*( 1.0f - interpolB ));
		temp += ((dram->dB[twoB]));
		dram->dB[treB] = (temp*tankfeedback);
		dram->dB[treB] += inputSampleR;
		oneB--; if (oneB < 0 || oneB > delayB) {oneB = delayB;}
		twoB--; if (twoB < 0 || twoB > delayB) {twoB = delayB;}
		treB--; if (treB < 0 || treB > delayB) {treB = delayB;}
		temp = (dram->dB[oneB]*interpolB );
		temp += (dram->dB[treB]*( 1.0f - interpolB ));
		temp *=  (invlean + (lean*fabs(dram->dB[twoB])));
		verboutR += temp;
		//comb filter B		
		temp = (dram->dD[oneD]*interpolD );
		temp += (dram->dD[treD]*( 1.0f - interpolD ));
		temp += ((dram->dD[twoD]));
		dram->dD[treD] = (temp*tankfeedback);
		dram->dD[treD] += inputSampleR;
		oneD--; if (oneD < 0 || oneD > delayD) {oneD = delayD;}
		twoD--; if (twoD < 0 || twoD > delayD) {twoD = delayD;}
		treD--; if (treD < 0 || treD > delayD) {treD = delayD;}
		temp = (dram->dD[oneD]*interpolD );
		temp += (dram->dD[treD]*( 1.0f - interpolD ));
		temp *=  (invlean + (lean*fabs(dram->dD[twoD])));
   		verboutR += temp;
		//comb filter D
		temp = (dram->dF[oneF]*interpolF );
		temp += (dram->dF[treF]*( 1.0f - interpolF ));
		temp += ((dram->dF[twoF]));
		dram->dF[treF] = (temp*tankfeedback);
		dram->dF[treF] += inputSampleR;
		oneF--; if (oneF < 0 || oneF > delayF) {oneF = delayF;}
		twoF--; if (twoF < 0 || twoF > delayF) {twoF = delayF;}
		treF--; if (treF < 0 || treF > delayF) {treF = delayF;}
		temp = (dram->dF[oneF]*interpolF );
		temp += (dram->dF[treF]*( 1.0f - interpolF ));
		temp *=  (invlean + (lean*fabs(dram->dF[twoF])));
   		verboutR += temp;
		//comb filter F
		temp = (dram->dH[oneH]*interpolH );
		temp += (dram->dH[treH]*( 1.0f - interpolH ));
		temp += ((dram->dH[twoH]));
		dram->dH[treH] = (temp*tankfeedback);
		dram->dH[treH] += inputSampleR;
		oneH--; if (oneH < 0 || oneH > delayH) {oneH = delayH;}
		twoH--; if (twoH < 0 || twoH > delayH) {twoH = delayH;}
		treH--; if (treH < 0 || treH > delayH) {treH = delayH;}
		temp = (dram->dH[oneH]*interpolH );
		temp += (dram->dH[treH]*( 1.0f - interpolH ));
		temp *=  (invlean + (lean*fabs(dram->dH[twoH])));
   		verboutR += temp;
		//comb filter H
		temp = (dram->dJ[oneJ]*interpolJ );
		temp += (dram->dJ[treJ]*( 1.0f - interpolJ ));
		temp += ((dram->dJ[twoJ]));
		dram->dJ[treJ] = (temp*tankfeedback);
		dram->dJ[treJ] += inputSampleR;
		oneJ--; if (oneJ < 0 || oneJ > delayJ) {oneJ = delayJ;}
		twoJ--; if (twoJ < 0 || twoJ > delayJ) {twoJ = delayJ;}
		treJ--; if (treJ < 0 || treJ > delayJ) {treJ = delayJ;}
		temp = (dram->dJ[oneJ]*interpolJ );
		temp += (dram->dJ[treJ]*( 1.0f - interpolJ ));
		temp *=  (invlean + (lean*fabs(dram->dJ[twoJ])));
   		verboutR += temp;
		//comb filter J
		temp = (dram->dL[oneL]*interpolL );
		temp += (dram->dL[treL]*( 1.0f - interpolL ));
		temp += ((dram->dL[twoL]));
		dram->dL[treL] = (temp*tankfeedback);
		dram->dL[treL] += inputSampleR;
		oneL--; if (oneL < 0 || oneL > delayL) {oneL = delayL;}
		twoL--; if (twoL < 0 || twoL > delayL) {twoL = delayL;}
		treL--; if (treL < 0 || treL > delayL) {treL = delayL;}
		temp = (dram->dL[oneL]*interpolL );
		temp += (dram->dL[treL]*( 1.0f - interpolL ));
		temp *=  (invlean + (lean*fabs(dram->dL[twoL])));
   		verboutR += temp;
		//comb filter L
		temp = (dram->dN[oneN]*interpolN );
		temp += (dram->dN[treN]*( 1.0f - interpolN ));
		temp += ((dram->dN[twoN]));
		dram->dN[treN] = (temp*tankfeedback);
		dram->dN[treN] += inputSampleR;
		oneN--; if (oneN < 0 || oneN > delayN) {oneN = delayN;}
		twoN--; if (twoN < 0 || twoN > delayN) {twoN = delayN;}
		treN--; if (treN < 0 || treN > delayN) {treN = delayN;}
		temp = (dram->dN[oneN]*interpolN );
		temp += (dram->dN[treN]*( 1.0f - interpolN ));
		temp *=  (invlean + (lean*fabs(dram->dN[twoN])));
   		verboutR += temp;
		//comb filter N
		temp = (dram->dP[oneP]*interpolP );
		temp += (dram->dP[treP]*( 1.0f - interpolP ));
		temp += ((dram->dP[twoP]));
		dram->dP[treP] = (temp*tankfeedback);
		dram->dP[treP] += inputSampleR;
		oneP--; if (oneP < 0 || oneP > delayP) {oneP = delayP;}
		twoP--; if (twoP < 0 || twoP > delayP) {twoP = delayP;}
		treP--; if (treP < 0 || treP > delayP) {treP = delayP;}
		temp = (dram->dP[oneP]*interpolP );
		temp += (dram->dP[treP]*( 1.0f - interpolP ));
		temp *=  (invlean + (lean*fabs(dram->dP[twoP])));
   		verboutR += temp;
		//comb filter P
		temp = (dram->dR[oneR]*interpolR );
		temp += (dram->dR[treR]*( 1.0f - interpolR ));
		temp += ((dram->dR[twoR]));
		dram->dR[treR] = (temp*tankfeedback);
		dram->dR[treR] += inputSampleR;
		oneR--; if (oneR < 0 || oneR > delayR) {oneR = delayR;}
		twoR--; if (twoR < 0 || twoR > delayR) {twoR = delayR;}
		treR--; if (treR < 0 || treR > delayR) {treR = delayR;}
		temp = (dram->dR[oneR]*interpolR );
		temp += (dram->dR[treR]*( 1.0f - interpolR ));
		temp *=  (invlean + (lean*fabs(dram->dR[twoR])));
   		verboutR += temp;
		//comb filter R
		temp = (dram->dT[oneT]*interpolT );
		temp += (dram->dT[treT]*( 1.0f - interpolT ));
		temp += ((dram->dT[twoT]));
		dram->dT[treT] = (temp*tankfeedback);
		dram->dT[treT] += inputSampleR;
		oneT--; if (oneT < 0 || oneT > delayT) {oneT = delayT;}
		twoT--; if (twoT < 0 || twoT > delayT) {twoT = delayT;}
		treT--; if (treT < 0 || treT > delayT) {treT = delayT;}
		temp = (dram->dT[oneT]*interpolT );
		temp += (dram->dT[treT]*( 1.0f - interpolT ));
		temp *=  (invlean + (lean*fabs(dram->dT[twoT])));
   		verboutR += temp;
		//comb filter T
		temp = (dram->dV[oneV]*interpolV );
		temp += (dram->dV[treV]*( 1.0f - interpolV ));
		temp += ((dram->dV[twoV]));
		dram->dV[treV] = (temp*tankfeedback);
		dram->dV[treV] += inputSampleR;
		oneV--; if (oneV < 0 || oneV > delayV) {oneV = delayV;}
		twoV--; if (twoV < 0 || twoV > delayV) {twoV = delayV;}
		treV--; if (treV < 0 || treV > delayV) {treV = delayV;}
		temp = (dram->dV[oneV]*interpolV );
		temp += (dram->dV[treV]*( 1.0f - interpolV ));
		temp *=  (invlean + (lean*fabs(dram->dV[twoV])));
   		verboutR += temp;
		//comb filter V
		temp = (dram->dX[oneX]*interpolX );
		temp += (dram->dX[treX]*( 1.0f - interpolX ));
		temp += ((dram->dX[twoX]));
		dram->dX[treX] = (temp*tankfeedback);
		dram->dX[treX] += inputSampleR;
		oneX--; if (oneX < 0 || oneX > delayX) {oneX = delayX;}
		twoX--; if (twoX < 0 || twoX > delayX) {twoX = delayX;}
		treX--; if (treX < 0 || treX > delayX) {treX = delayX;}
		temp = (dram->dX[oneX]*interpolX );
		temp += (dram->dX[treX]*( 1.0f - interpolX ));
		temp *=  (invlean + (lean*fabs(dram->dX[twoX])));
   		verboutR += temp;
		//comb filter X
		temp = (dram->dZ[oneZ]*interpolZ );
		temp += (dram->dZ[treZ]*( 1.0f - interpolZ ));
		temp += ((dram->dZ[twoZ]));
		dram->dZ[treZ] = (temp*tankfeedback);
		dram->dZ[treZ] += inputSampleR;
		oneZ--; if (oneZ < 0 || oneZ > delayZ) {oneZ = delayZ;}
		twoZ--; if (twoZ < 0 || twoZ > delayZ) {twoZ = delayZ;}
		treZ--; if (treZ < 0 || treZ > delayZ) {treZ = delayZ;}
		temp = (dram->dZ[oneZ]*interpolZ );
		temp += (dram->dZ[treZ]*( 1.0f - interpolZ ));
		temp *=  (invlean + (lean*fabs(dram->dZ[twoZ])));
   		verboutR += temp;
		//comb filter Z
		//here we do the R delay tank, every other letter B D F H J
		
		verboutL /= 8;
		verboutR /= 8;
		
		iirSampleL = (iirSampleL * (1 - iirAmount)) + (verboutL * iirAmount);
		verboutL = verboutL - iirSampleL;
		
		iirSampleR = (iirSampleR * (1 - iirAmount)) + (verboutR * iirAmount);
		verboutR = verboutR - iirSampleR;
		//we need to highpass the crosscoupling, it's making DC runaway
		
		verboutL *=  (invlean + (lean*fabs(verboutL)));
		verboutR *=  (invlean + (lean*fabs(verboutR)));
		//scale back the verb tank the same way we scaled the combs
		
		inputSampleL = verboutL;
		inputSampleR = verboutR;
		
		//EQ lowpass is after all processing like the compressor that might produce hash
		if (flip)
		{
			lowpassSampleAA = (lowpassSampleAA * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
			inputSampleL = lowpassSampleAA;
			lowpassSampleBA = (lowpassSampleBA * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
			inputSampleL = lowpassSampleBA;
			lowpassSampleCA = (lowpassSampleCA * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
			inputSampleL = lowpassSampleCA;
			lowpassSampleDA = (lowpassSampleDA * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
			inputSampleL = lowpassSampleDA;
			lowpassSampleE = (lowpassSampleE * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
			inputSampleL = lowpassSampleE;
		}
		else
		{
			lowpassSampleAB = (lowpassSampleAB * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
			inputSampleL = lowpassSampleAB;
			lowpassSampleBB = (lowpassSampleBB * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
			inputSampleL = lowpassSampleBB;
			lowpassSampleCB = (lowpassSampleCB * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
			inputSampleL = lowpassSampleCB;
			lowpassSampleDB = (lowpassSampleDB * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
			inputSampleL = lowpassSampleDB;
			lowpassSampleF = (lowpassSampleF * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
			inputSampleL = lowpassSampleF;			
		}
		lowpassSampleG = (lowpassSampleG * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
		inputSampleL = (lowpassSampleG * (1 - iirAmountC)) + (inputSampleL * iirAmountC);
		
		
		if (flip)
		{
			rowpassSampleAA = (rowpassSampleAA * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
			inputSampleR = rowpassSampleAA;
			rowpassSampleBA = (rowpassSampleBA * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
			inputSampleR = rowpassSampleBA;
			rowpassSampleCA = (rowpassSampleCA * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
			inputSampleR = rowpassSampleCA;
			rowpassSampleDA = (rowpassSampleDA * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
			inputSampleR = rowpassSampleDA;
			rowpassSampleE = (rowpassSampleE * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
			inputSampleR = rowpassSampleE;
		}
		else
		{
			rowpassSampleAB = (rowpassSampleAB * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
			inputSampleR = rowpassSampleAB;
			rowpassSampleBB = (rowpassSampleBB * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
			inputSampleR = rowpassSampleBB;
			rowpassSampleCB = (rowpassSampleCB * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
			inputSampleR = rowpassSampleCB;
			rowpassSampleDB = (rowpassSampleDB * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
			inputSampleR = rowpassSampleDB;
			rowpassSampleF = (rowpassSampleF * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
			inputSampleR = rowpassSampleF;			
		}
		rowpassSampleG = (rowpassSampleG * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
		inputSampleR = (rowpassSampleG * (1 - iirAmountC)) + (inputSampleR * iirAmountC);
		
		iirCCSampleL = (iirCCSampleL * (1 - iirAmount)) + (verboutL * iirAmount);
		verboutL = verboutL - iirCCSampleL;
		
		iirCCSampleR = (iirCCSampleR * (1 - iirAmount)) + (verboutR * iirAmount);
		verboutR = verboutR - iirCCSampleR;
		//we need to highpass the crosscoupling, it's making DC runaway
		
		verboutL *=  (invlean + (lean*fabs(verboutL)));
		verboutR *=  (invlean + (lean*fabs(verboutR)));
		//scale back the crosscouple the same way we scaled the combs
		verboutL = (inputSampleL) * outcouple;
		verboutR = (inputSampleR) * outcouple;
		//send it off to the input again
		
		nonlin += fabs(verboutL);
		nonlin += fabs(verboutR);//post highpassing and a lot of processing
		
		drySampleL *= dryness;
		drySampleR *= dryness;
		
		inputSampleL *= wetness;
		inputSampleR *= wetness;
		
		drySampleL += inputSampleL;
		drySampleR += inputSampleR;
		//here we combine the tanks with the dry signal
		
		
		flip = !flip;
		
		*outputL = drySampleL;
		*outputR = drySampleR;
		//here we mix the reverb output with the dry input
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	int count;
	for(count = 0; count < 2347; count++) {dram->dMid[count] = 0.0;}
	for(count = 0; count < 1333; count++) {dram->dSide[count] = 0.0;}
	for(count = 0; count < 5923; count++) {dram->dLeft[count] = 0.0;}
	for(count = 0; count < 5925; count++) {dram->dRight[count] = 0.0;}
	
	for(count = 0; count < 7574; count++) {dram->dpreR[count] = 0.0;}
	for(count = 0; count < 7574; count++) {dram->dpreL[count] = 0.0;}
	
	for(count = 0; count < 7574; count++) {dram->dA[count] = 0.0;}
	for(count = 0; count < 7308; count++) {dram->dB[count] = 0.0;}
	for(count = 0; count < 7178; count++) {dram->dC[count] = 0.0;}
	for(count = 0; count < 6908; count++) {dram->dD[count] = 0.0;}
	for(count = 0; count < 6780; count++) {dram->dE[count] = 0.0;}
	for(count = 0; count < 6522; count++) {dram->dF[count] = 0.0;}
	for(count = 0; count < 5982; count++) {dram->dG[count] = 0.0;}
	for(count = 0; count < 5564; count++) {dram->dH[count] = 0.0;}
	for(count = 0; count < 5298; count++) {dram->dI[count] = 0.0;}
	for(count = 0; count < 4904; count++) {dram->dJ[count] = 0.0;}
	for(count = 0; count < 4760; count++) {dram->dK[count] = 0.0;}
	for(count = 0; count < 4490; count++) {dram->dL[count] = 0.0;}
	for(count = 0; count < 4392; count++) {dram->dM[count] = 0.0;}
	for(count = 0; count < 4230; count++) {dram->dN[count] = 0.0;}
	for(count = 0; count < 4154; count++) {dram->dO[count] = 0.0;}
	for(count = 0; count < 3990; count++) {dram->dP[count] = 0.0;}
	for(count = 0; count < 3660; count++) {dram->dQ[count] = 0.0;}
	for(count = 0; count < 3408; count++) {dram->dR[count] = 0.0;}
	for(count = 0; count < 3252; count++) {dram->dS[count] = 0.0;}
	for(count = 0; count < 3000; count++) {dram->dT[count] = 0.0;}
	for(count = 0; count < 2918; count++) {dram->dU[count] = 0.0;}
	for(count = 0; count < 2750; count++) {dram->dV[count] = 0.0;}
	for(count = 0; count < 2504; count++) {dram->dW[count] = 0.0;}
	for(count = 0; count < 2424; count++) {dram->dX[count] = 0.0;}
	for(count = 0; count < 2147; count++) {dram->dY[count] = 0.0;}
	for(count = 0; count < 2089; count++) {dram->dZ[count] = 0.0;}
	
	oneMid = 1; delayMid = 2346; maxdelayMid = 2346;
	oneSide = 1; delaySide = 1332; maxdelaySide = 1332;
	oneLeft = 1; delayLeft = 5922; maxdelayLeft = 5922;
	oneRight = 1; delayRight = 5924; maxdelayRight = 5924;
	onepre = 1; delaypre = 7573; maxdelaypre = 7573;
	
	oneA = 1; twoA = 2; treA = 3; delayA = 7573; maxdelayA = 7573;
	oneB = 1; twoB = 2; treB = 3; delayB = 7307; maxdelayB = 7307;
	oneC = 1; twoC = 2; treC = 3; delayC = 7177; maxdelayC = 7177;
	oneD = 1; twoD = 2; treD = 3; delayD = 6907; maxdelayD = 6907;
	oneE = 1; twoE = 2; treE = 3; delayE = 6779; maxdelayE = 6779;
	oneF = 1; twoF = 2; treF = 3; delayF = 6521; maxdelayF = 6521;
	oneG = 1; twoG = 2; treG = 3; delayG = 5981; maxdelayG = 5981;
	oneH = 1; twoH = 2; treH = 3; delayH = 5563; maxdelayH = 5563;
	oneI = 1; twoI = 2; treI = 3; delayI = 5297; maxdelayI = 5297;
	oneJ = 1; twoJ = 2; treJ = 3; delayJ = 4903; maxdelayJ = 4903;
	oneK = 1; twoK = 2; treK = 3; delayK = 4759; maxdelayK = 4759;
	oneL = 1; twoL = 2; treL = 3; delayL = 4489; maxdelayL = 4489;
	oneM = 1; twoM = 2; treM = 3; delayM = 4391; maxdelayM = 4391;
	oneN = 1; twoN = 2; treN = 3; delayN = 4229; maxdelayN = 4229;
	oneO = 1; twoO = 2; treO = 3; delayO = 4153; maxdelayO = 4153;
	oneP = 1; twoP = 2; treP = 3; delayP = 3989; maxdelayP = 3989;
	oneQ = 1; twoQ = 2; treQ = 3; delayQ = 3659; maxdelayQ = 3659;
	oneR = 1; twoR = 2; treR = 3; delayR = 3407; maxdelayR = 3407;
	oneS = 1; twoS = 2; treS = 3; delayS = 3251; maxdelayS = 3251;
	oneT = 1; twoT = 2; treT = 3; delayT = 2999; maxdelayT = 2999;
	oneU = 1; twoU = 2; treU = 3; delayU = 2917; maxdelayU = 2917;
	oneV = 1; twoV = 2; treV = 3; delayV = 2749; maxdelayV = 2749;
	oneW = 1; twoW = 2; treW = 3; delayW = 2503; maxdelayW = 2503;
	oneX = 1; twoX = 2; treX = 3; delayX = 2423; maxdelayX = 2423;
	oneY = 1; twoY = 2; treY = 3; delayY = 2146; maxdelayY = 2146;
	oneZ = 1; twoZ = 2; treZ = 3; delayZ = 2088; maxdelayZ = 2088;
	
	avgInputL = 0.0;
	avgInputR = 0.0;
	avgOutputL = 0.0;
	avgOutputR = 0.0;
	avg2InputL = 0.0;
	avg2InputR = 0.0;
	avg2OutputL = 0.0;
	avg2OutputR = 0.0;
	a2vgInputL = 0.0;
	a2vgInputR = 0.0;
	a2vgOutputL = 0.0;
	a2vgOutputR = 0.0;
	a2vg2InputL = 0.0;
	a2vg2InputR = 0.0;
	a2vg2OutputL = 0.0;
	a2vg2OutputR = 0.0;
	
	lowpassSampleAA = 0.0;
	lowpassSampleAB = 0.0;
	lowpassSampleBA = 0.0;
	lowpassSampleBB = 0.0;
	lowpassSampleCA = 0.0;
	lowpassSampleCB = 0.0;
	lowpassSampleDA = 0.0;
	lowpassSampleDB = 0.0;
	lowpassSampleE = 0.0;
	lowpassSampleF = 0.0;
	lowpassSampleG = 0.0;
	
	rowpassSampleAA = 0.0;
	rowpassSampleAB = 0.0;
	rowpassSampleBA = 0.0;
	rowpassSampleBB = 0.0;
	rowpassSampleCA = 0.0;
	rowpassSampleCB = 0.0;
	rowpassSampleDA = 0.0;
	rowpassSampleDB = 0.0;
	rowpassSampleE = 0.0;
	rowpassSampleF = 0.0;
	rowpassSampleG = 0.0;
	
	interpolA = 0.0;
	interpolB = 0.0;
	interpolC = 0.0;
	interpolD = 0.0;
	interpolE = 0.0;
	interpolF = 0.0;
	interpolG = 0.0;
	interpolH = 0.0;
	interpolI = 0.0;
	interpolJ = 0.0;
	interpolK = 0.0;
	interpolL = 0.0;
	interpolM = 0.0;
	interpolN = 0.0;
	interpolO = 0.0;
	interpolP = 0.0;
	interpolQ = 0.0;
	interpolR = 0.0;
	interpolS = 0.0;
	interpolT = 0.0;
	interpolU = 0.0;
	interpolV = 0.0;
	interpolW = 0.0;
	interpolX = 0.0;
	interpolY = 0.0;
	interpolZ = 0.0;
	
	pitchshiftA = 1.0 / maxdelayA;
	pitchshiftB = 1.0 / maxdelayB;
	pitchshiftC = 1.0 / maxdelayC;
	pitchshiftD = 1.0 / maxdelayD;
	pitchshiftE = 1.0 / maxdelayE;
	pitchshiftF = 1.0 / maxdelayF;
	pitchshiftG = 1.0 / maxdelayG;
	pitchshiftH = 1.0 / maxdelayH;
	pitchshiftI = 1.0 / maxdelayI;
	pitchshiftJ = 1.0 / maxdelayJ;
	pitchshiftK = 1.0 / maxdelayK;
	pitchshiftL = 1.0 / maxdelayL;
	pitchshiftM = 1.0 / maxdelayM;
	pitchshiftN = 1.0 / maxdelayN;
	pitchshiftO = 1.0 / maxdelayO;
	pitchshiftP = 1.0 / maxdelayP;
	pitchshiftQ = 1.0 / maxdelayQ;
	pitchshiftR = 1.0 / maxdelayR;
	pitchshiftS = 1.0 / maxdelayS;
	pitchshiftT = 1.0 / maxdelayT;
	pitchshiftU = 1.0 / maxdelayU;
	pitchshiftV = 1.0 / maxdelayV;
	pitchshiftW = 1.0 / maxdelayW;
	pitchshiftX = 1.0 / maxdelayX;
	pitchshiftY = 1.0 / maxdelayY;
	pitchshiftZ = 1.0 / maxdelayZ;
	
	nonlin = 0.0;
	
	verboutL = 0.0;
	verboutR = 0.0;
	iirCCSampleL = 0.0;
	iirCCSampleR = 0.0;
	iirSampleL = 0.0;
	iirSampleR = 0.0;
	savedRoomsize = -1.0; //force update to begin
	countdown = -1;
	flip = true;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}


};
