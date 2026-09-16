#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "WoodenBox"
#define AIRWINDOWS_DESCRIPTION "Like a miniature reverb for converting DI to acoustic."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','W','o','x' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
const int d4A = 173; 
const int d4B = 82; 
const int d4C = 240; 
const int d4D = 191; 
const int d4E = 196; 
const int d4F = 257; 
const int d4G = 203; 
const int d4H = 252; 
const int d4I = 207; 
const int d4J = 203; 
const int d4K = 250; 
const int d4L = 220; 
const int d4M = 261; 
const int d4N = 235; 
const int d4O = 161; 
const int d4P = 161; 
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Select", .min = 0, .max = 16000, .def = 5000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Depth", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	int c4AL,c4BL,c4CL,c4DL,c4EL,c4FL,c4GL,c4HL;
	int c4IL,c4JL,c4KL,c4LL,c4ML,c4NL,c4OL,c4PL;		
	int c4AR,c4BR,c4CR,c4DR,c4ER,c4FR,c4GR,c4HR;
	int c4IR,c4JR,c4KR,c4LR,c4MR,c4NR,c4OR,c4PR;
	//base stereo reverb
	float g4AL,g4BL,g4CL,g4DL,g4DR,g4HR,g4LR,g4PR;
	//changed letter is the dual mono, with rearranged grid
	
	enum {
		bez_AL,
		bez_AR,
		bez_BL,
		bez_BR,
		bez_CL,
		bez_CR,	
		bez_SampL,
		bez_SampR,
		bez_cycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
		
	int shortA;
	int shortB;
	int shortC;
	int shortD;
	int shortE;
	int shortF;
	int shortG;
	int shortH;
	int shortI;
	int shortJ;
	int shortK;
	int shortL;
	int shortM;
	int shortN;
	int shortO;
	int shortP;
	
	int prevclearcoat;
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float b4AL[d4A+5];
	float b4BL[d4B+5];
	float b4CL[d4C+5];
	float b4DL[d4D+5];
	float b4EL[d4E+5];
	float b4FL[d4F+5];
	float b4GL[d4G+5];
	float b4HL[d4H+5];
	float b4IL[d4I+5];
	float b4JL[d4J+5];
	float b4KL[d4K+5];
	float b4LL[d4L+5];
	float b4ML[d4M+5];
	float b4NL[d4N+5];
	float b4OL[d4O+5];
	float b4PL[d4P+5];
	float b4AR[d4A+5];
	float b4BR[d4B+5];
	float b4CR[d4C+5];
	float b4DR[d4D+5];
	float b4ER[d4E+5];
	float b4FR[d4F+5];
	float b4GR[d4G+5];
	float b4HR[d4H+5];
	float b4IR[d4I+5];
	float b4JR[d4J+5];
	float b4KR[d4K+5];
	float b4LR[d4L+5];
	float b4MR[d4M+5];
	float b4NR[d4N+5];
	float b4OR[d4O+5];
	float b4PR[d4P+5];
	float bez[bez_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	int clearcoat = GetParameter( kParam_A );
	if (clearcoat != prevclearcoat) {
		for(int count = 0; count < d4A+2; count++) {dram->b4AL[count] = 0.0f; dram->b4AR[count] = 0.0f;}
		for(int count = 0; count < d4B+2; count++) {dram->b4BL[count] = 0.0f; dram->b4BR[count] = 0.0f;}
		for(int count = 0; count < d4C+2; count++) {dram->b4CL[count] = 0.0f; dram->b4CR[count] = 0.0f;}
		for(int count = 0; count < d4D+2; count++) {dram->b4DL[count] = 0.0f; dram->b4DR[count] = 0.0f;}
		for(int count = 0; count < d4E+2; count++) {dram->b4EL[count] = 0.0f; dram->b4ER[count] = 0.0f;}
		for(int count = 0; count < d4F+2; count++) {dram->b4FL[count] = 0.0f; dram->b4FR[count] = 0.0f;}
		for(int count = 0; count < d4G+2; count++) {dram->b4GL[count] = 0.0f; dram->b4GR[count] = 0.0f;}
		for(int count = 0; count < d4H+2; count++) {dram->b4HL[count] = 0.0f; dram->b4HR[count] = 0.0f;}
		for(int count = 0; count < d4I+2; count++) {dram->b4IL[count] = 0.0f; dram->b4IR[count] = 0.0f;}
		for(int count = 0; count < d4J+2; count++) {dram->b4JL[count] = 0.0f; dram->b4JR[count] = 0.0f;}
		for(int count = 0; count < d4K+2; count++) {dram->b4KL[count] = 0.0f; dram->b4KR[count] = 0.0f;}
		for(int count = 0; count < d4L+2; count++) {dram->b4LL[count] = 0.0f; dram->b4LR[count] = 0.0f;}
		for(int count = 0; count < d4M+2; count++) {dram->b4ML[count] = 0.0f; dram->b4MR[count] = 0.0f;}
		for(int count = 0; count < d4N+2; count++) {dram->b4NL[count] = 0.0f; dram->b4NR[count] = 0.0f;}
		for(int count = 0; count < d4O+2; count++) {dram->b4OL[count] = 0.0f; dram->b4OR[count] = 0.0f;}
		for(int count = 0; count < d4P+2; count++) {dram->b4PL[count] = 0.0f; dram->b4PR[count] = 0.0f;}		
		c4AL = 1;
		c4BL = 1;
		c4CL = 1;
		c4DL = 1;	
		c4EL = 1;
		c4FL = 1;
		c4GL = 1;
		c4HL = 1;
		c4IL = 1;
		c4JL = 1;
		c4KL = 1;
		c4LL = 1;
		c4ML = 1;
		c4NL = 1;
		c4OL = 1;
		c4PL = 1;
		
		c4AR = 1;
		c4BR = 1;
		c4CR = 1;
		c4DR = 1;	
		c4ER = 1;
		c4FR = 1;
		c4GR = 1;
		c4HR = 1;
		c4IR = 1;
		c4JR = 1;
		c4KR = 1;
		c4LR = 1;
		c4MR = 1;
		c4NR = 1;
		c4OR = 1;
		c4PR = 1;
		switch (clearcoat)
		{
			case 0:
				shortA = 17; shortB = 10; shortC = 23; shortD = 3; shortE = 8; shortF = 7; shortG = 41; shortH = 6; shortI = 3; shortJ = 6; shortK = 59; shortL = 61; shortM = 4; shortN = 71; shortO = 5; shortP = 4; break; //0 to 4 ms, 0 seat room  
			case 1:
				shortA = 12; shortB = 19; shortC = 89; shortD = 25; shortE = 92; shortF = 8; shortG = 41; shortH = 11; shortI = 80; shortJ = 27; shortK = 6; shortL = 4; shortM = 3; shortN = 21; shortO = 7; shortP = 63; break; //0 to 7 ms, 1 seat room  
			case 2:
				shortA = 35; shortB = 19; shortC = 5; shortD = 7; shortE = 15; shortF = 7; shortG = 41; shortH = 191; shortI = 177; shortJ = 3; shortK = 6; shortL = 22; shortM = 23; shortN = 118; shortO = 4; shortP = 79; break; //0 to 11 ms, 4 seat room  
			case 3:
				shortA = 17; shortB = 19; shortC = 105; shortD = 135; shortE = 31; shortF = 86; shortG = 41; shortH = 16; shortI = 3; shortJ = 16; shortK = 6; shortL = 151; shortM = 147; shortN = 26; shortO = 3; shortP = 10; break; //0 to 11 ms, 4 seat room  
			case 4:
				shortA = 134; shortB = 13; shortC = 26; shortD = 10; shortE = 34; shortF = 24; shortG = 4; shortH = 60; shortI = 88; shortJ = 9; shortK = 155; shortL = 11; shortM = 3; shortN = 18; shortO = 9; shortP = 161; break; //0 to 11 ms, 4 seat room  
			case 5:
				shortA = 17; shortB = 82; shortC = 23; shortD = 29; shortE = 133; shortF = 3; shortG = 41; shortH = 27; shortI = 10; shortJ = 177; shortK = 6; shortL = 37; shortM = 14; shortN = 145; shortO = 4; shortP = 9; break; //0 to 12 ms, 4 seat room  
			case 6:
				shortA = 31; shortB = 19; shortC = 3; shortD = 29; shortE = 196; shortF = 11; shortG = 10; shortH = 65; shortI = 21; shortJ = 3; shortK = 148; shortL = 4; shortM = 26; shortN = 7; shortO = 161; shortP = 155; break; //0 to 12 ms, 4 seat room  
			case 7:
				shortA = 17; shortB = 8; shortC = 3; shortD = 37; shortE = 3; shortF = 19; shortG = 41; shortH = 15; shortI = 7; shortJ = 197; shortK = 178; shortL = 22; shortM = 26; shortN = 97; shortO = 16; shortP = 156; break; //0 to 12 ms, 5 seat room  
			case 8:
				shortA = 17; shortB = 3; shortC = 8; shortD = 29; shortE = 39; shortF = 156; shortG = 7; shortH = 43; shortI = 101; shortJ = 8; shortK = 15; shortL = 169; shortM = 67; shortN = 39; shortO = 154; shortP = 4; break; //0 to 13 ms, 5 seat room  
			case 9:
				shortA = 18; shortB = 19; shortC = 23; shortD = 5; shortE = 176; shortF = 3; shortG = 41; shortH = 147; shortI = 7; shortJ = 148; shortK = 5; shortL = 15; shortM = 10; shortN = 30; shortO = 119; shortP = 19; break; //0 to 13 ms, 5 seat room  
			case 10:
				shortA = 173; shortB = 19; shortC = 23; shortD = 27; shortE = 8; shortF = 37; shortG = 7; shortH = 202; shortI = 8; shortJ = 13; shortK = 3; shortL = 174; shortM = 67; shortN = 21; shortO = 73; shortP = 14; break; //0 to 14 ms, 6 seat room  
			case 11:
				shortA = 17; shortB = 19; shortC = 23; shortD = 25; shortE = 19; shortF = 145; shortG = 9; shortH = 43; shortI = 47; shortJ = 203; shortK = 18; shortL = 180; shortM = 226; shortN = 3; shortO = 73; shortP = 12; break; //0 to 15 ms, 7 seat room  
			case 12:
				shortA = 17; shortB = 19; shortC = 23; shortD = 3; shortE = 3; shortF = 20; shortG = 203; shortH = 99; shortI = 207; shortJ = 15; shortK = 10; shortL = 61; shortM = 20; shortN = 174; shortO = 33; shortP = 77; break; //0 to 15 ms, 7 seat room  
			case 13:
				shortA = 17; shortB = 19; shortC = 23; shortD = 29; shortE = 3; shortF = 210; shortG = 183; shortH = 43; shortI = 13; shortJ = 12; shortK = 26; shortL = 220; shortM = 67; shortN = 235; shortO = 11; shortP = 23; break; //0 to 15 ms, 8 seat room  
			case 14:
				shortA = 17; shortB = 3; shortC = 21; shortD = 191; shortE = 31; shortF = 10; shortG = 41; shortH = 218; shortI = 15; shortJ = 6; shortK = 111; shortL = 29; shortM = 129; shortN = 206; shortO = 4; shortP = 7; break; //0 to 16 ms, 8 seat room  
			case 15:
				shortA = 17; shortB = 25; shortC = 240; shortD = 29; shortE = 4; shortF = 18; shortG = 41; shortH = 43; shortI = 29; shortJ = 28; shortK = 250; shortL = 12; shortM = 261; shortN = 9; shortO = 5; shortP = 79; break; //0 to 18 ms, 10 seat room  
			case 16:
			default:
				shortA = 5; shortB = 3; shortC = 23; shortD = 29; shortE = 3; shortF = 257; shortG = 199; shortH = 252; shortI = 132; shortJ = 18; shortK = 11; shortL = 6; shortM = 30; shortN = 27; shortO = 7; shortP = 8; break; //0 to 19 ms, 11 seat room  
		}
		prevclearcoat = clearcoat;
	}
	float reg4n = (1.0f-powf(1.0f-GetParameter( kParam_B ),2.0f))*0.0336f;
	float derez = 1.0f;
	derez = fmin(fmax(derez/overallscale,0.0001f),1.0f);
	int bezFraction = (int)(1.0f/derez);
	float bezTrim = (float)bezFraction/(bezFraction+1.0f);
	derez = 1.0f / bezFraction;
	bezTrim = 1.0f-(derez*bezTrim);
	//the revision more accurately connects the bezier curves
	float wet = 1.0f-powf(1.0f-GetParameter( kParam_C ),2.0f);
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		dram->bez[bez_cycle] += derez;
		dram->bez[bez_SampL] += (inputSampleR*derez);
		dram->bez[bez_SampR] += (inputSampleL*derez); //stereo got reversed somewhere?
		if (dram->bez[bez_cycle] > 1.0f) { //hit the end point and we do a reverb sample
			dram->bez[bez_cycle] = 0.0f;
			//left verbs
			float dualmonoSampleL =  dram->bez[bez_SampL];			
			dram->b4AL[c4AL] = dualmonoSampleL + (g4AL * reg4n);
			dram->b4BL[c4BL] = dualmonoSampleL + (g4BL * reg4n);
			dram->b4CL[c4CL] = dualmonoSampleL + (g4CL * reg4n);
			dram->b4DL[c4DL] = dualmonoSampleL + (g4DL * reg4n);
			
			c4AL++; if (c4AL < 0 || c4AL > shortA) c4AL = 0;
			c4BL++; if (c4BL < 0 || c4BL > shortB) c4BL = 0;
			c4CL++; if (c4CL < 0 || c4CL > shortC) c4CL = 0;
			c4DL++; if (c4DL < 0 || c4DL > shortD) c4DL = 0;
			
			float hA = dram->b4AL[c4AL-((c4AL > shortA)?shortA+1:0)];
			float hB = dram->b4BL[c4BL-((c4BL > shortB)?shortB+1:0)];
			float hC = dram->b4CL[c4CL-((c4CL > shortC)?shortC+1:0)];
			float hD = dram->b4DL[c4DL-((c4DL > shortD)?shortD+1:0)];
			dram->b4EL[c4EL] = hA - (hB + hC + hD);
			dram->b4FL[c4FL] = hB - (hA + hC + hD);
			dram->b4GL[c4GL] = hC - (hA + hB + hD);
			dram->b4HL[c4HL] = hD - (hA + hB + hC);
			
			c4EL++; if (c4EL < 0 || c4EL > shortE) c4EL = 0;
			c4FL++; if (c4FL < 0 || c4FL > shortF) c4FL = 0;
			c4GL++; if (c4GL < 0 || c4GL > shortG) c4GL = 0;
			c4HL++; if (c4HL < 0 || c4HL > shortH) c4HL = 0;
			
			hA = dram->b4EL[c4EL-((c4EL > shortE)?shortE+1:0)];
			hB = dram->b4FL[c4FL-((c4FL > shortF)?shortF+1:0)];
			hC = dram->b4GL[c4GL-((c4GL > shortG)?shortG+1:0)];
			hD = dram->b4HL[c4HL-((c4HL > shortH)?shortH+1:0)];
			dram->b4IL[c4IL] = hA - (hB + hC + hD);
			dram->b4JL[c4JL] = hB - (hA + hC + hD);
			dram->b4KL[c4KL] = hC - (hA + hB + hD);
			dram->b4LL[c4LL] = hD - (hA + hB + hC);
			
			c4IL++; if (c4IL < 0 || c4IL > shortI) c4IL = 0;
			c4JL++; if (c4JL < 0 || c4JL > shortJ) c4JL = 0;
			c4KL++; if (c4KL < 0 || c4KL > shortK) c4KL = 0;
			c4LL++; if (c4LL < 0 || c4LL > shortL) c4LL = 0;
			
			hA = dram->b4IL[c4IL-((c4IL > shortI)?shortI+1:0)];
			hB = dram->b4JL[c4JL-((c4JL > shortJ)?shortJ+1:0)];
			hC = dram->b4KL[c4KL-((c4KL > shortK)?shortK+1:0)];
			hD = dram->b4LL[c4LL-((c4LL > shortL)?shortL+1:0)];
			dram->b4ML[c4ML] = hA - (hB + hC + hD);
			dram->b4NL[c4NL] = hB - (hA + hC + hD);
			dram->b4OL[c4OL] = hC - (hA + hB + hD);
			dram->b4PL[c4PL] = hD - (hA + hB + hC);
			
			c4ML++; if (c4ML < 0 || c4ML > shortM) c4ML = 0;
			c4NL++; if (c4NL < 0 || c4NL > shortN) c4NL = 0;
			c4OL++; if (c4OL < 0 || c4OL > shortO) c4OL = 0;
			c4PL++; if (c4PL < 0 || c4PL > shortP) c4PL = 0;
			
			hA = dram->b4ML[c4ML-((c4ML > shortM)?shortM+1:0)];
			hB = dram->b4NL[c4NL-((c4NL > shortN)?shortN+1:0)];
			hC = dram->b4OL[c4OL-((c4OL > shortO)?shortO+1:0)];
			hD = dram->b4PL[c4PL-((c4PL > shortP)?shortP+1:0)];
			g4AL = hA - (hB + hC + hD);
			g4BL = hB - (hA + hC + hD);
			g4CL = hC - (hA + hB + hD);
			g4DL = hD - (hA + hB + hC);			
			dualmonoSampleL = (hA + hB + hC + hD)*0.125f;
			
			//right verbs
			float dualmonoSampleR =  dram->bez[bez_SampR];
			dram->b4DR[c4DR] = dualmonoSampleR + (g4DR * reg4n);
			dram->b4HR[c4HR] = dualmonoSampleR + (g4HR * reg4n);
			dram->b4LR[c4LR] = dualmonoSampleR + (g4LR * reg4n);
			dram->b4PR[c4PR] = dualmonoSampleR + (g4PR * reg4n);
			
			c4DR++; if (c4DR < 0 || c4DR > shortD) c4DR = 0;
			c4HR++; if (c4HR < 0 || c4HR > shortH) c4HR = 0;
			c4LR++; if (c4LR < 0 || c4LR > shortL) c4LR = 0;
			c4PR++; if (c4PR < 0 || c4PR > shortP) c4PR = 0;
			
			hA = dram->b4DR[c4DR-((c4DR > shortD)?shortD+1:0)];
			hB = dram->b4HR[c4HR-((c4HR > shortH)?shortH+1:0)];
			hC = dram->b4LR[c4LR-((c4LR > shortL)?shortL+1:0)];
			hD = dram->b4PR[c4PR-((c4PR > shortP)?shortP+1:0)];
			dram->b4CR[c4CR] = hA - (hB + hC + hD);
			dram->b4GR[c4GR] = hB - (hA + hC + hD);
			dram->b4KR[c4KR] = hC - (hA + hB + hD);
			dram->b4OR[c4OR] = hD - (hA + hB + hC);
			
			c4CR++; if (c4CR < 0 || c4CR > shortC) c4CR = 0;
			c4GR++; if (c4GR < 0 || c4GR > shortG) c4GR = 0;
			c4KR++; if (c4KR < 0 || c4KR > shortK) c4KR = 0;
			c4OR++; if (c4OR < 0 || c4OR > shortO) c4OR = 0;
			
			hA = dram->b4CR[c4CR-((c4CR > shortC)?shortC+1:0)];
			hB = dram->b4GR[c4GR-((c4GR > shortG)?shortG+1:0)];
			hC = dram->b4KR[c4KR-((c4KR > shortK)?shortK+1:0)];
			hD = dram->b4OR[c4OR-((c4OR > shortO)?shortO+1:0)];
			dram->b4BR[c4BR] = hA - (hB + hC + hD);
			dram->b4FR[c4FR] = hB - (hA + hC + hD);
			dram->b4JR[c4JR] = hC - (hA + hB + hD);
			dram->b4NR[c4NR] = hD - (hA + hB + hC);
			
			c4BR++; if (c4BR < 0 || c4BR > shortB) c4BR = 0;
			c4FR++; if (c4FR < 0 || c4FR > shortF) c4FR = 0;
			c4JR++; if (c4JR < 0 || c4JR > shortJ) c4JR = 0;
			c4NR++; if (c4NR < 0 || c4NR > shortN) c4NR = 0;
			
			hA = dram->b4BR[c4BR-((c4BR > shortB)?shortB+1:0)];
			hB = dram->b4FR[c4FR-((c4FR > shortF)?shortF+1:0)];
			hC = dram->b4JR[c4JR-((c4JR > shortJ)?shortJ+1:0)];
			hD = dram->b4NR[c4NR-((c4NR > shortN)?shortN+1:0)];
			dram->b4AR[c4AR] = hA - (hB + hC + hD);
			dram->b4ER[c4ER] = hB - (hA + hC + hD);
			dram->b4IR[c4IR] = hC - (hA + hB + hD);
			dram->b4MR[c4MR] = hD - (hA + hB + hC);
			
			c4AR++; if (c4AR < 0 || c4AR > shortA) c4AR = 0;
			c4ER++; if (c4ER < 0 || c4ER > shortE) c4ER = 0;
			c4IR++; if (c4IR < 0 || c4IR > shortI) c4IR = 0;
			c4MR++; if (c4MR < 0 || c4MR > shortM) c4MR = 0;
			
			hA = dram->b4AR[c4AR-((c4AR > shortA)?shortA+1:0)];
			hB = dram->b4ER[c4ER-((c4ER > shortE)?shortE+1:0)];
			hC = dram->b4IR[c4IR-((c4IR > shortI)?shortI+1:0)];
			hD = dram->b4MR[c4MR-((c4MR > shortM)?shortM+1:0)];
			g4DR = hA - (hB + hC + hD);				
			g4HR = hB - (hA + hC + hD);
			g4LR = hC - (hA + hB + hD);
			g4PR = hD - (hA + hB + hC);
			dualmonoSampleR = (hA + hB + hC + hD)*0.125f;
			
			dram->bez[bez_CL] = dram->bez[bez_BL];
			dram->bez[bez_BL] = dram->bez[bez_AL];
			dram->bez[bez_AL] = dualmonoSampleR;
			dram->bez[bez_SampL] = 0.0f;
			
			dram->bez[bez_CR] = dram->bez[bez_BR];
			dram->bez[bez_BR] = dram->bez[bez_AR];
			dram->bez[bez_AR] = dualmonoSampleL;
			dram->bez[bez_SampR] = 0.0f;
		}
		float X = dram->bez[bez_cycle]*bezTrim;
		float CBL = (dram->bez[bez_CL]*(1.0f-X))+(dram->bez[bez_BL]*X);
		float CBR = (dram->bez[bez_CR]*(1.0f-X))+(dram->bez[bez_BR]*X);
		float BAL = (dram->bez[bez_BL]*(1.0f-X))+(dram->bez[bez_AL]*X);
		float BAR = (dram->bez[bez_BR]*(1.0f-X))+(dram->bez[bez_AR]*X);
		inputSampleL = (dram->bez[bez_BL]+(CBL*(1.0f-X))+(BAL*X))*0.125f;
		inputSampleR = (dram->bez[bez_BR]+(CBR*(1.0f-X))+(BAR*X))*0.125f;
				
		inputSampleL = (inputSampleL * wet)+(drySampleL * (1.0f-wet));
		inputSampleR = (inputSampleR * wet)+(drySampleR * (1.0f-wet));
		
		
		
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
	c4AL = c4BL = c4CL = c4DL = c4EL = c4FL = c4GL = c4HL = 1;
	c4IL = c4JL = c4KL = c4LL = c4ML = c4NL = c4OL = c4PL = 1;
	c4AR = c4BR = c4CR = c4DR = c4ER = c4FR = c4GR = c4HR = 1;
	c4IR = c4JR = c4KR = c4LR = c4MR = c4NR = c4OR = c4PR = 1;	
	for(int x = 0; x < d4A+2; x++) {dram->b4AL[x] = 0.0; dram->b4AR[x] = 0.0;}
	for(int x = 0; x < d4B+2; x++) {dram->b4BL[x] = 0.0; dram->b4BR[x] = 0.0;}
	for(int x = 0; x < d4C+2; x++) {dram->b4CL[x] = 0.0; dram->b4CR[x] = 0.0;}
	for(int x = 0; x < d4D+2; x++) {dram->b4DL[x] = 0.0; dram->b4DR[x] = 0.0;}
	for(int x = 0; x < d4E+2; x++) {dram->b4EL[x] = 0.0; dram->b4ER[x] = 0.0;}
	for(int x = 0; x < d4F+2; x++) {dram->b4FL[x] = 0.0; dram->b4FR[x] = 0.0;}
	for(int x = 0; x < d4G+2; x++) {dram->b4GL[x] = 0.0; dram->b4GR[x] = 0.0;}
	for(int x = 0; x < d4H+2; x++) {dram->b4HL[x] = 0.0; dram->b4HR[x] = 0.0;}
	for(int x = 0; x < d4I+2; x++) {dram->b4IL[x] = 0.0; dram->b4IR[x] = 0.0;}
	for(int x = 0; x < d4J+2; x++) {dram->b4JL[x] = 0.0; dram->b4JR[x] = 0.0;}
	for(int x = 0; x < d4K+2; x++) {dram->b4KL[x] = 0.0; dram->b4KR[x] = 0.0;}
	for(int x = 0; x < d4L+2; x++) {dram->b4LL[x] = 0.0; dram->b4LR[x] = 0.0;}
	for(int x = 0; x < d4M+2; x++) {dram->b4ML[x] = 0.0; dram->b4MR[x] = 0.0;}
	for(int x = 0; x < d4N+2; x++) {dram->b4NL[x] = 0.0; dram->b4NR[x] = 0.0;}
	for(int x = 0; x < d4O+2; x++) {dram->b4OL[x] = 0.0; dram->b4OR[x] = 0.0;}
	for(int x = 0; x < d4P+2; x++) {dram->b4PL[x] = 0.0; dram->b4PR[x] = 0.0;}
	g4AL = g4BL = g4CL = g4DL = 0.0;
	g4DR = g4HR = g4LR = g4PR = 0.0;
	
	for (int x = 0; x < bez_total; x++) dram->bez[x] = 0.0;
	dram->bez[bez_cycle] = 1.0;
	
	shortA = 173;
	shortB = 82;
	shortC = 240;
	shortD = 191;
	shortE = 196;
	shortF = 257;
	shortG = 203;
	shortH = 252;
	shortI = 207;
	shortJ = 203;
	shortK = 250;
	shortL = 220;
	shortM = 261;
	shortN = 235;
	shortO = 161;
	shortP = 161;
	prevclearcoat = -1;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
