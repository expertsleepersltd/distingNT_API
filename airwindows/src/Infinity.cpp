#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Infinity"
#define AIRWINDOWS_DESCRIPTION "A MatrixVerb specifically designed to sustain and layer sounds forever."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','n','y' )
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
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Filter", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Damping", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Size", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		float biquadA[11];
		float biquadB[11];
		float biquadC[11];
		
		
		int countA, delayA;
		int countB, delayB;
		int countC, delayC;
		int countD, delayD;
		int countE, delayE;
		int countF, delayF;
		int countG, delayG;
		int countH, delayH;
		int countI, delayI;
		int countJ, delayJ;
		int countK, delayK;
		int countL, delayL;
		int countM, delayM;
		
		Float32 feedbackA;
		Float32 feedbackB;
		Float32 feedbackC;
		Float32 feedbackD;
		Float32 feedbackE;
		Float32 feedbackF;
		Float32 feedbackG;
		Float32 feedbackH;
		
		uint32_t fpd;
	
	struct _dram {
			Float32 aA[8111];
		Float32 aB[7511];
		Float32 aC[7311];
		Float32 aD[6911];		
		Float32 aE[6311];
		Float32 aF[6111];
		Float32 aG[5511];
		Float32 aH[4911];
		Float32 aI[4511];
		Float32 aJ[4311];
		Float32 aK[3911];
		Float32 aL[3311];
		Float32 aM[3111];
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
	
	biquadC[0] = biquadB[0] = biquadA[0] = ((powf(GetParameter( kParam_One ),2)*9900.0f)+100.0f) / GetSampleRate();
	biquadA[1] = 0.618033988749894848204586f;
	biquadB[1] = (GetParameter( kParam_One )*0.5f)+0.118033988749894848204586f;
    biquadC[1] = 0.5f;
	
	float K = tan(M_PI * biquadA[0]); //lowpass
	float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
	biquadA[2] = K * K * norm;
	biquadA[3] = 2.0f * biquadA[2];
	biquadA[4] = biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	
	K = tan(M_PI * biquadA[0]);
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
	
	Float32 damping = powf(GetParameter( kParam_Two ),2)*0.5f;
	
	Float32 size = (powf(GetParameter( kParam_Three ),2)*90.0f)+10.0f;

	Float32 wet = GetParameter( kParam_Four );
	
	
	delayA = 79*size;
	delayB = 73*size;
	delayC = 71*size;
	delayD = 67*size;
	delayE = 61*size;
	delayF = 59*size;
	delayG = 53*size;
	delayH = 47*size;
	
	delayI = 43*size;
	delayJ = 41*size;
	delayK = 37*size;
	delayL = 31*size;
	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		float tempSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
		biquadA[8] = biquadA[7]; biquadA[7] = inputSample; inputSample = tempSample; 
		biquadA[10] = biquadA[9]; biquadA[9] = inputSample; //DF1
		
		inputSample *= wet;
		//we're going to use this as a kind of balance since the reverb buildup can be so large
		inputSample *= 0.5f;
		
		float allpassI = inputSample;
		float allpassJ = inputSample;
		float allpassK = inputSample;
		float allpassL = inputSample;
		
		int allpasstemp = countI + 1;
		if (allpasstemp < 0 || allpasstemp > delayI) {allpasstemp = 0;}
		allpassI -= dram->aI[allpasstemp]*0.5f;
		dram->aI[countI] = allpassI;
		allpassI *= 0.5f;
		countI++; if (countI < 0 || countI > delayI) {countI = 0;}		
		allpassI += (dram->aI[countI]);
		
		allpasstemp = countJ + 1;
		if (allpasstemp < 0 || allpasstemp > delayJ) {allpasstemp = 0;}
		allpassJ -= dram->aJ[allpasstemp]*0.5f;
		dram->aJ[countJ] = allpassJ;
		allpassJ *= 0.5f;
		countJ++; if (countJ < 0 || countJ > delayJ) {countJ = 0;}		
		allpassJ += (dram->aJ[countJ]);
		
		allpasstemp = countK + 1;
		if (allpasstemp < 0 || allpasstemp > delayK) {allpasstemp = 0;}
		allpassK -= dram->aK[allpasstemp]*0.5f;
		dram->aK[countK] = allpassK;
		allpassK *= 0.5f;
		countK++; if (countK < 0 || countK > delayK) {countK = 0;}		
		allpassK += (dram->aK[countK]);
		
		allpasstemp = countL + 1;
		if (allpasstemp < 0 || allpasstemp > delayL) {allpasstemp = 0;}
		allpassL -= dram->aL[allpasstemp]*0.5f;
		dram->aL[countL] = allpassL;
		allpassL *= 0.5f;
		countL++; if (countL < 0 || countL > delayL) {countL = 0;}		
		allpassL += (dram->aL[countL]);		
		//the big allpass in front of everything
		
		
		dram->aA[countA] = allpassI + feedbackA;
		dram->aB[countB] = allpassJ + feedbackB;
		dram->aC[countC] = allpassK + feedbackC;
		dram->aD[countD] = allpassL + feedbackD;
		dram->aE[countE] = allpassI + feedbackE;
		dram->aF[countF] = allpassJ + feedbackF;
		dram->aG[countG] = allpassK + feedbackG;
		dram->aH[countH] = allpassL + feedbackH;
		
		countA++; if (countA < 0 || countA > delayA) {countA = 0;}
		countB++; if (countB < 0 || countB > delayB) {countB = 0;}
		countC++; if (countC < 0 || countC > delayC) {countC = 0;}
		countD++; if (countD < 0 || countD > delayD) {countD = 0;}
		countE++; if (countE < 0 || countE > delayE) {countE = 0;}
		countF++; if (countF < 0 || countF > delayF) {countF = 0;}
		countG++; if (countG < 0 || countG > delayG) {countG = 0;}
		countH++; if (countH < 0 || countH > delayH) {countH = 0;}
		//the Householder matrices
		
		Float32 infiniteA = (dram->aA[countA-((countA > delayA)?delayA+1:0)] * (1-(damping-floor(damping))) );
		infiniteA += (dram->aA[countA+1-((countA+1 > delayA)?delayA+1:0)] * ((damping-floor(damping))) );
		Float32 infiniteB = dram->aB[countB-((countB > delayB)?delayB+1:0)];
		Float32 infiniteC = dram->aC[countC-((countC > delayC)?delayC+1:0)];
		Float32 infiniteD = dram->aD[countD-((countD > delayD)?delayD+1:0)];
		
		Float32 infiniteE = (dram->aE[countE-((countE > delayE)?delayE+1:0)] * (1-(damping-floor(damping))) );
		infiniteE += (dram->aE[countE+1-((countE+1 > delayE)?delayE+1:0)] * ((damping-floor(damping))) );
		Float32 infiniteF = dram->aF[countF-((countF > delayF)?delayF+1:0)];
		Float32 infiniteG = dram->aG[countG-((countG > delayG)?delayG+1:0)];
		Float32 infiniteH = dram->aH[countH-((countH > delayH)?delayH+1:0)];
		
		Float32 dialBackA = 0.5f;
		Float32 dialBackE = 0.5f;
		Float32 dialBackDry = 0.5f;
		if (fabs(infiniteA)>0.4f) dialBackA -= ((fabs(infiniteA)-0.4f)*0.2f);
		if (fabs(infiniteE)>0.4f) dialBackE -= ((fabs(infiniteE)-0.4f)*0.2f);
		if (fabs(drySample)>0.4f) dialBackDry -= ((fabs(drySample)-0.4f)*0.2f);
		//we're compressing things subtly so we can feed energy in and not overload
		
		feedbackA = (infiniteA - (infiniteB + infiniteC + infiniteD))*dialBackA;
		feedbackB = (infiniteB - (infiniteA + infiniteC + infiniteD))*dialBackDry;
		feedbackC = (infiniteC - (infiniteA + infiniteB + infiniteD))*dialBackDry;
		feedbackD = (infiniteD - (infiniteA + infiniteB + infiniteC))*dialBackDry;
		
		feedbackE = (infiniteE - (infiniteF + infiniteG + infiniteH))*dialBackE;
		feedbackF = (infiniteF - (infiniteE + infiniteG + infiniteH))*dialBackDry;
		feedbackG = (infiniteG - (infiniteE + infiniteF + infiniteH))*dialBackDry;
		feedbackH = (infiniteH - (infiniteE + infiniteF + infiniteG))*dialBackDry;
		
		inputSample = (infiniteA + infiniteB + infiniteC + infiniteD + infiniteE + infiniteF + infiniteG + infiniteH)/8.0f;
		
		tempSample = biquadB[2]*inputSample+biquadB[3]*biquadB[7]+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
		biquadB[8] = biquadB[7]; biquadB[7] = inputSample; inputSample = tempSample; 
		biquadB[10] = biquadB[9]; biquadB[9] = inputSample; //DF1
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		
		inputSample = asin(inputSample);
		
		tempSample = biquadC[2]*inputSample+biquadC[3]*biquadC[7]+biquadC[4]*biquadC[8]-biquadC[5]*biquadC[9]-biquadC[6]*biquadC[10];
		biquadC[8] = biquadC[7]; biquadC[7] = inputSample; inputSample = tempSample; 
		biquadC[10] = biquadC[9]; biquadC[9] = inputSample; //DF1
		
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
	for (int x = 0; x < 11; x++) {biquadA[x] = 0.0;biquadB[x] = 0.0;biquadC[x] = 0.0;}
	
	feedbackA = 0.0;
	feedbackB = 0.0;
	feedbackC = 0.0;
	feedbackD = 0.0;
	feedbackE = 0.0;
	feedbackF = 0.0;
	feedbackG = 0.0;
	feedbackH = 0.0;
	
	int count;
	for(count = 0; count < 8110; count++) {dram->aA[count] = 0.0;}
	for(count = 0; count < 7510; count++) {dram->aB[count] = 0.0;}
	for(count = 0; count < 7310; count++) {dram->aC[count] = 0.0;}
	for(count = 0; count < 6910; count++) {dram->aD[count] = 0.0;}
	for(count = 0; count < 6310; count++) {dram->aE[count] = 0.0;}
	for(count = 0; count < 6110; count++) {dram->aF[count] = 0.0;}
	for(count = 0; count < 5510; count++) {dram->aG[count] = 0.0;}
	for(count = 0; count < 4910; count++) {dram->aH[count] = 0.0;}
	//maximum value needed will be delay * 100, plus 206 (absolute max vibrato depth)
	for(count = 0; count < 4510; count++) {dram->aI[count] = 0.0;}
	for(count = 0; count < 4310; count++) {dram->aJ[count] = 0.0;}
	for(count = 0; count < 3910; count++) {dram->aK[count] = 0.0;}
	for(count = 0; count < 3310; count++) {dram->aL[count] = 0.0;}
	
	countA = 1; delayA = 79;
	countB = 1; delayB = 73;
	countC = 1; delayC = 71;
	countD = 1; delayD = 67;	
	countE = 1; delayE = 61;
	countF = 1; delayF = 59;
	countG = 1; delayG = 53;
	countH = 1; delayH = 47;
	//the householder matrices
	countI = 1; delayI = 43;
	countJ = 1; delayJ = 41;
	countK = 1; delayK = 37;
	countL = 1; delayL = 31;
	//the allpasses

	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
