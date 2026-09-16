#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "CreamCoat"
#define AIRWINDOWS_DESCRIPTION "A swiss army knife reverb with soft undersampling."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','r','t' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
const int predelay = 15000;
const int kshortA = 350;
const int kshortB = 1710;
const int kshortC = 1610;
const int kshortD = 835;
const int kshortE = 700;
const int kshortF = 1260;
const int kshortG = 1110;
const int kshortH = 1768;
const int kshortI = 280;
const int kshortJ = 2645;
const int kshortK = 1410;
const int kshortL = 1175;
const int kshortM = 12;
const int kshortN = 3110;
const int kshortO = 120;
const int kshortP = 2370;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Selection", .min = 0, .max = 16000, .def = 10000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Regen", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DeRez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Predelay", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wetness", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	
	
	
	float feedbackAL;
	float feedbackBL;
	float feedbackCL;
	float feedbackDL;
	
	float feedbackDR;
	float feedbackHR;
	float feedbackLR;
	float feedbackPR;
	
	float previousAL;
	float previousBL;
	float previousCL;
	float previousDL;
	float previousEL;
	
	float previousAR;
	float previousBR;
	float previousCR;
	float previousDR;
	float previousER;
		
	int countAL;
	int countBL;
	int countCL;
	int countDL;
	int countEL;
	int countFL;
	int countGL;
	int countHL;
	int countIL;
	int countJL;
	int countKL;
	int countLL;		
	int countML;		
	int countNL;		
	int countOL;		
	int countPL;		
	
	int countAR;
	int countBR;
	int countCR;
	int countDR;
	int countER;
	int countFR;
	int countGR;
	int countHR;
	int countIR;
	int countJR;
	int countKR;
	int countLR;		
	int countMR;		
	int countNR;		
	int countOR;		
	int countPR;	
	
	int countZ;		

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
	
	enum {
		bez_AL,
		bez_AR,
		bez_BL,
		bez_BR,
		bez_CL,
		bez_CR,	
		bez_InL,
		bez_InR,
		bez_UnInL,
		bez_UnInR,
		bez_SampL,
		bez_SampR,
		bez_cycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float aAL[kshortA+5];
	float aBL[kshortB+5];
	float aCL[kshortC+5];
	float aDL[kshortD+5];
	float aEL[kshortE+5];
	float aFL[kshortF+5];
	float aGL[kshortG+5];
	float aHL[kshortH+5];
	float aIL[kshortI+5];
	float aJL[kshortJ+5];
	float aKL[kshortK+5];
	float aLL[kshortL+5];
	float aML[kshortM+5];
	float aNL[kshortN+5];
	float aOL[kshortO+5];
	float aPL[kshortP+5];
	float aAR[kshortA+5];
	float aBR[kshortB+5];
	float aCR[kshortC+5];
	float aDR[kshortD+5];
	float aER[kshortE+5];
	float aFR[kshortF+5];
	float aGR[kshortG+5];
	float aHR[kshortH+5];
	float aIR[kshortI+5];
	float aJR[kshortJ+5];
	float aKR[kshortK+5];
	float aLR[kshortL+5];
	float aMR[kshortM+5];
	float aNR[kshortN+5];
	float aOR[kshortO+5];
	float aPR[kshortP+5];
	float aZL[predelay+5];
	float aZR[predelay+5];
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
		for(int count = 0; count < kshortA+2; count++) {dram->aAL[count] = 0.0f; dram->aAR[count] = 0.0f;}
		for(int count = 0; count < kshortB+2; count++) {dram->aBL[count] = 0.0f; dram->aBR[count] = 0.0f;}
		for(int count = 0; count < kshortC+2; count++) {dram->aCL[count] = 0.0f; dram->aCR[count] = 0.0f;}
		for(int count = 0; count < kshortD+2; count++) {dram->aDL[count] = 0.0f; dram->aDR[count] = 0.0f;}
		for(int count = 0; count < kshortE+2; count++) {dram->aEL[count] = 0.0f; dram->aER[count] = 0.0f;}
		for(int count = 0; count < kshortF+2; count++) {dram->aFL[count] = 0.0f; dram->aFR[count] = 0.0f;}
		for(int count = 0; count < kshortG+2; count++) {dram->aGL[count] = 0.0f; dram->aGR[count] = 0.0f;}
		for(int count = 0; count < kshortH+2; count++) {dram->aHL[count] = 0.0f; dram->aHR[count] = 0.0f;}
		for(int count = 0; count < kshortI+2; count++) {dram->aIL[count] = 0.0f; dram->aIR[count] = 0.0f;}
		for(int count = 0; count < kshortJ+2; count++) {dram->aJL[count] = 0.0f; dram->aJR[count] = 0.0f;}
		for(int count = 0; count < kshortK+2; count++) {dram->aKL[count] = 0.0f; dram->aKR[count] = 0.0f;}
		for(int count = 0; count < kshortL+2; count++) {dram->aLL[count] = 0.0f; dram->aLR[count] = 0.0f;}
		for(int count = 0; count < kshortM+2; count++) {dram->aML[count] = 0.0f; dram->aMR[count] = 0.0f;}
		for(int count = 0; count < kshortN+2; count++) {dram->aNL[count] = 0.0f; dram->aNR[count] = 0.0f;}
		for(int count = 0; count < kshortO+2; count++) {dram->aOL[count] = 0.0f; dram->aOR[count] = 0.0f;}
		for(int count = 0; count < kshortP+2; count++) {dram->aPL[count] = 0.0f; dram->aPR[count] = 0.0f;}		
		countAL = 1;
		countBL = 1;
		countCL = 1;
		countDL = 1;	
		countEL = 1;
		countFL = 1;
		countGL = 1;
		countHL = 1;
		countIL = 1;
		countJL = 1;
		countKL = 1;
		countLL = 1;
		countML = 1;
		countNL = 1;
		countOL = 1;
		countPL = 1;
		
		countAR = 1;
		countBR = 1;
		countCR = 1;
		countDR = 1;	
		countER = 1;
		countFR = 1;
		countGR = 1;
		countHR = 1;
		countIR = 1;
		countJR = 1;
		countKR = 1;
		countLR = 1;
		countMR = 1;
		countNR = 1;
		countOR = 1;
		countPR = 1;
		switch (clearcoat)
		{
			case 0:
				shortA = 65; shortB = 124; shortC = 83; shortD = 180; shortE = 200; shortF = 291; shortG = 108; shortH = 189; shortI = 73; shortJ = 410; shortK = 479; shortL = 310; shortM = 11; shortN = 928; shortO = 23; shortP = 654; break; //5 to 51 ms, 96 seat room. Scarcity, 1 in 125324
				//Short96
			case 1:
				shortA = 114; shortB = 205; shortC = 498; shortD = 195; shortE = 205; shortF = 318; shortG = 143; shortH = 254; shortI = 64; shortJ = 721; shortK = 512; shortL = 324; shortM = 11; shortN = 782; shortO = 26; shortP = 394; break; //7 to 52 ms, 107 seat club. Scarcity, 1 in 65537
				//Short107
			case 2:
				shortA = 118; shortB = 272; shortC = 292; shortD = 145; shortE = 200; shortF = 241; shortG = 204; shortH = 504; shortI = 50; shortJ = 678; shortK = 424; shortL = 412; shortM = 11; shortN = 1124; shortO = 47; shortP = 766; break; //8 to 58 ms, 135 seat club. Scarcity, 1 in 196272
				//Short135
			case 3:
				shortA = 19; shortB = 474; shortC = 301; shortD = 275; shortE = 260; shortF = 321; shortG = 371; shortH = 571; shortI = 50; shortJ = 410; shortK = 697; shortL = 414; shortM = 11; shortN = 986; shortO = 47; shortP = 522; break; //7 to 61 ms, 143 seat club. Scarcity, 1 in 165738
				//Short143
			case 4:
				shortA = 112; shortB = 387; shortC = 452; shortD = 289; shortE = 173; shortF = 476; shortG = 321; shortH = 593; shortI = 73; shortJ = 343; shortK = 829; shortL = 91; shortM = 11; shortN = 1055; shortO = 43; shortP = 862; break; //8 to 66 ms, 166 seat club. Scarcity, 1 in 158437
				//Short166
			case 5:
				shortA = 60; shortB = 368; shortC = 295; shortD = 272; shortE = 210; shortF = 284; shortG = 326; shortH = 830; shortI = 125; shortJ = 236; shortK = 737; shortL = 486; shortM = 11; shortN = 1178; shortO = 75; shortP = 902; break; //9 to 70 ms, 189 seat club. Scarcity, 1 in 94790
				//Short189
			case 6:
				shortA = 73; shortB = 311; shortC = 472; shortD = 251; shortE = 134; shortF = 509; shortG = 393; shortH = 591; shortI = 124; shortJ = 1070; shortK = 340; shortL = 525; shortM = 11; shortN = 1367; shortO = 75; shortP = 816; break; //7 to 79 ms, 225 seat club. Scarcity, 1 in 257803
				//Short225
			case 7:
				shortA = 159; shortB = 518; shortC = 514; shortD = 165; shortE = 275; shortF = 494; shortG = 296; shortH = 667; shortI = 75; shortJ = 1101; shortK = 116; shortL = 414; shortM = 11; shortN = 1261; shortO = 79; shortP = 998; break; //11 to 80 ms, 252 seat club. Scarcity, 1 in 175192
				//Short252
			case 8:
				shortA = 41; shortB = 741; shortC = 274; shortD = 59; shortE = 306; shortF = 332; shortG = 291; shortH = 767; shortI = 42; shortJ = 881; shortK = 959; shortL = 422; shortM = 11; shortN = 1237; shortO = 45; shortP = 958; break; //8 to 83 ms, 255 seat club. Scarcity, 1 in 185708
				//Short255
			case 9:
				shortA = 251; shortB = 437; shortC = 783; shortD = 189; shortE = 130; shortF = 272; shortG = 244; shortH = 761; shortI = 128; shortJ = 1190; shortK = 320; shortL = 491; shortM = 11; shortN = 1409; shortO = 58; shortP = 455; break; //10 to 93 ms, 323 seat club. Scarcity, 1 in 334044
				//Short323
			case 10:
				shortA = 316; shortB = 510; shortC = 1087; shortD = 349; shortE = 359; shortF = 74; shortG = 79; shortH = 1269; shortI = 34; shortJ = 693; shortK = 749; shortL = 511; shortM = 11; shortN = 1751; shortO = 93; shortP = 403; break; //9 to 110 ms, 427 seat theater. Scarcity, 1 in 200715
				//Short427
			case 11:
				shortA = 254; shortB = 651; shortC = 845; shortD = 316; shortE = 373; shortF = 267; shortG = 182; shortH = 857; shortI = 215; shortJ = 1535; shortK = 1127; shortL = 315; shortM = 11; shortN = 1649; shortO = 97; shortP = 829; break; //15 to 110 ms, 470 seat theater. Scarcity, 1 in 362673
				//Short470
			case 12:
				shortA = 113; shortB = 101; shortC = 673; shortD = 357; shortE = 340; shortF = 229; shortG = 278; shortH = 1008; shortI = 265; shortJ = 1890; shortK = 155; shortL = 267; shortM = 11; shortN = 2233; shortO = 116; shortP = 600; break; //11 to 131 ms, 606 seat theater. Scarcity, 1 in 238058
				//Short606
			case 13:
				shortA = 218; shortB = 1058; shortC = 862; shortD = 505; shortE = 297; shortF = 580; shortG = 532; shortH = 1387; shortI = 120; shortJ = 576; shortK = 1409; shortL = 473; shortM = 11; shortN = 1991; shortO = 76; shortP = 685; break; //14 to 132 ms, 643 seat theater. Scarcity, 1 in 193432
				//Short643
			case 14:
				shortA = 78; shortB = 760; shortC = 982; shortD = 528; shortE = 445; shortF = 1128; shortG = 130; shortH = 708; shortI = 22; shortJ = 2144; shortK = 354; shortL = 1169; shortM = 11; shortN = 2782; shortO = 58; shortP = 1515; break; //5 to 159 ms, 809 seat hall. Scarcity, 1 in 212274
				//Short809
			case 15:
				shortA = 330; shortB = 107; shortC = 1110; shortD = 371; shortE = 620; shortF = 143; shortG = 1014; shortH = 1763; shortI = 184; shortJ = 2068; shortK = 1406; shortL = 595; shortM = 11; shortN = 2639; shortO = 33; shortP = 1594; break; //10 to 171 ms, 984 seat hall. Scarcity, 1 in 226499
				//Short984
			case 16:
			default:
				shortA = 336; shortB = 1660; shortC = 386; shortD = 623; shortE = 693; shortF = 1079; shortG = 891; shortH = 1574; shortI = 24; shortJ = 2641; shortK = 1239; shortL = 775; shortM = 11; shortN = 3104; shortO = 55; shortP = 2366; break; //24 to 203 ms, 1541 seat hall. Scarcity, 1 in 275025
				//Short1541
		}
		prevclearcoat = clearcoat;
	}
	
	
	float regen = (1.0f-powf(1.0f-GetParameter( kParam_B ),2.0f))*0.0625f;
	float derez = GetParameter( kParam_C )/overallscale;
	if (derez < 0.0005f) derez = 0.0005f; if (derez > 1.0f) derez = 1.0f;
	derez = 1.0f / ((int)(1.0f/derez));
	//this hard-locks derez to exact subdivisions of 1.0f
	int adjPredelay = predelay*GetParameter( kParam_D )*derez;	
	float wet = GetParameter( kParam_E )*2.0f;
	float dry = 2.0f - wet;
	if (wet > 1.0f) wet = 1.0f; else wet *= wet;
	if (wet < 0.0f) wet = 0.0f;
	if (dry > 1.0f) dry = 1.0f;
	if (dry < 0.0f) dry = 0.0f;
	//this reverb makes 50% full dry AND full wet, not crossfaded.
	//that's so it can be on submixes without cutting back dry channel when adjusted:
	//unless you go super heavy, you are only adjusting the added verb loudness.
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
				
		dram->bez[bez_cycle] += derez;
		dram->bez[bez_SampL] += ((inputSampleL+dram->bez[bez_InL]) * derez);
		dram->bez[bez_SampR] += ((inputSampleR+dram->bez[bez_InR]) * derez);
		dram->bez[bez_InL] = inputSampleL; dram->bez[bez_InR] = inputSampleR;
		if (dram->bez[bez_cycle] > 1.0f) { //hit the end point and we do a reverb sample
			dram->bez[bez_cycle] = 0.0f;
			
			//predelay
			dram->aZL[countZ] = dram->bez[bez_SampL];
			dram->aZR[countZ] = dram->bez[bez_SampR];
			countZ++; if (countZ < 0 || countZ > adjPredelay) countZ = 0;
			dram->bez[bez_SampL] = dram->aZL[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			dram->bez[bez_SampR] = dram->aZR[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			//end predelay
			
			dram->aAL[countAL] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackAL * regen);
			dram->aBL[countBL] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackBL * regen);
			dram->aCL[countCL] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackCL * regen);
			dram->aDL[countDL] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackDL * regen);
			dram->bez[bez_UnInL] = dram->bez[bez_SampL];
			
			dram->aDR[countDR] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackDR * regen);
			dram->aHR[countHR] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackHR * regen);
			dram->aLR[countLR] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackLR * regen);
			dram->aPR[countPR] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackPR * regen);
			dram->bez[bez_UnInR] = dram->bez[bez_SampR];
			
			countAL++; if (countAL < 0 || countAL > shortA) countAL = 0;
			countBL++; if (countBL < 0 || countBL > shortB) countBL = 0;
			countCL++; if (countCL < 0 || countCL > shortC) countCL = 0;
			countDL++; if (countDL < 0 || countDL > shortD) countDL = 0;
			
			countDR++; if (countDR < 0 || countDR > shortD) countDR = 0;
			countHR++; if (countHR < 0 || countHR > shortH) countHR = 0;
			countLR++; if (countLR < 0 || countLR > shortL) countLR = 0;
			countPR++; if (countPR < 0 || countPR > shortP) countPR = 0;
			
			float outAL = dram->aAL[countAL-((countAL > shortA)?shortA+1:0)];
			float outBL = dram->aBL[countBL-((countBL > shortB)?shortB+1:0)];
			float outCL = dram->aCL[countCL-((countCL > shortC)?shortC+1:0)];
			float outDL = dram->aDL[countDL-((countDL > shortD)?shortD+1:0)];
			
			float outDR = dram->aDR[countDR-((countDR > shortD)?shortD+1:0)];
			float outHR = dram->aHR[countHR-((countHR > shortH)?shortH+1:0)];
			float outLR = dram->aLR[countLR-((countLR > shortL)?shortL+1:0)];
			float outPR = dram->aPR[countPR-((countPR > shortP)?shortP+1:0)];
			
			dram->aEL[countEL] = outAL - (outBL + outCL + outDL);
			dram->aFL[countFL] = outBL - (outAL + outCL + outDL);
			dram->aGL[countGL] = outCL - (outAL + outBL + outDL);
			dram->aHL[countHL] = outDL - (outAL + outBL + outCL);
			
			dram->aCR[countCR] = outDR - (outHR + outLR + outPR);
			dram->aGR[countGR] = outHR - (outDR + outLR + outPR);
			dram->aKR[countKR] = outLR - (outDR + outHR + outPR);
			dram->aOR[countOR] = outPR - (outDR + outHR + outLR);
			
			countEL++; if (countEL < 0 || countEL > shortE) countEL = 0;
			countFL++; if (countFL < 0 || countFL > shortF) countFL = 0;
			countGL++; if (countGL < 0 || countGL > shortG) countGL = 0;
			countHL++; if (countHL < 0 || countHL > shortH) countHL = 0;
			
			countCR++; if (countCR < 0 || countCR > shortC) countCR = 0;
			countGR++; if (countGR < 0 || countGR > shortG) countGR = 0;
			countKR++; if (countKR < 0 || countKR > shortK) countKR = 0;
			countOR++; if (countOR < 0 || countOR > shortO) countOR = 0;
			
			float outEL = dram->aEL[countEL-((countEL > shortE)?shortE+1:0)];
			float outFL = dram->aFL[countFL-((countFL > shortF)?shortF+1:0)];
			float outGL = dram->aGL[countGL-((countGL > shortG)?shortG+1:0)];
			float outHL = dram->aHL[countHL-((countHL > shortH)?shortH+1:0)];
			
			float outCR = dram->aCR[countCR-((countCR > shortC)?shortC+1:0)];
			float outGR = dram->aGR[countGR-((countGR > shortG)?shortG+1:0)];
			float outKR = dram->aKR[countKR-((countKR > shortK)?shortK+1:0)];
			float outOR = dram->aOR[countOR-((countOR > shortO)?shortO+1:0)];
			
			dram->aIL[countIL] = outEL - (outFL + outGL + outHL);
			dram->aJL[countJL] = outFL - (outEL + outGL + outHL);
			dram->aKL[countKL] = outGL - (outEL + outFL + outHL);
			dram->aLL[countLL] = outHL - (outEL + outFL + outGL);
			
			dram->aBR[countBR] = outCR - (outGR + outKR + outOR);
			dram->aFR[countFR] = outGR - (outCR + outKR + outOR);
			dram->aJR[countJR] = outKR - (outCR + outGR + outOR);
			dram->aNR[countNR] = outOR - (outCR + outGR + outKR);
			
			countIL++; if (countIL < 0 || countIL > shortI) countIL = 0;
			countJL++; if (countJL < 0 || countJL > shortJ) countJL = 0;
			countKL++; if (countKL < 0 || countKL > shortK) countKL = 0;
			countLL++; if (countLL < 0 || countLL > shortL) countLL = 0;
			
			countBR++; if (countBR < 0 || countBR > shortB) countBR = 0;
			countFR++; if (countFR < 0 || countFR > shortF) countFR = 0;
			countJR++; if (countJR < 0 || countJR > shortJ) countJR = 0;
			countNR++; if (countNR < 0 || countNR > shortN) countNR = 0;
			
			float outIL = dram->aIL[countIL-((countIL > shortI)?shortI+1:0)];
			float outJL = dram->aJL[countJL-((countJL > shortJ)?shortJ+1:0)];
			float outKL = dram->aKL[countKL-((countKL > shortK)?shortK+1:0)];
			float outLL = dram->aLL[countLL-((countLL > shortL)?shortL+1:0)];
			
			float outBR = dram->aBR[countBR-((countBR > shortB)?shortB+1:0)];
			float outFR = dram->aFR[countFR-((countFR > shortF)?shortF+1:0)];
			float outJR = dram->aJR[countJR-((countJR > shortJ)?shortJ+1:0)];
			float outNR = dram->aNR[countNR-((countNR > shortN)?shortN+1:0)];
			
			dram->aML[countML] = outIL - (outJL + outKL + outLL);
			dram->aNL[countNL] = outJL - (outIL + outKL + outLL);
			dram->aOL[countOL] = outKL - (outIL + outJL + outLL);
			dram->aPL[countPL] = outLL - (outIL + outJL + outKL);
			
			dram->aAR[countAR] = outBR - (outFR + outJR + outNR);
			dram->aER[countER] = outFR - (outBR + outJR + outNR);
			dram->aIR[countIR] = outJR - (outBR + outFR + outNR);
			dram->aMR[countMR] = outNR - (outBR + outFR + outJR);
			
			countML++; if (countML < 0 || countML > shortM) countML = 0;
			countNL++; if (countNL < 0 || countNL > shortN) countNL = 0;
			countOL++; if (countOL < 0 || countOL > shortO) countOL = 0;
			countPL++; if (countPL < 0 || countPL > shortP) countPL = 0;
			
			countAR++; if (countAR < 0 || countAR > shortA) countAR = 0;
			countER++; if (countER < 0 || countER > shortE) countER = 0;
			countIR++; if (countIR < 0 || countIR > shortI) countIR = 0;
			countMR++; if (countMR < 0 || countMR > shortM) countMR = 0;
			
			float outML = dram->aML[countML-((countML > shortM)?shortM+1:0)];
			float outNL = dram->aNL[countNL-((countNL > shortN)?shortN+1:0)];
			float outOL = dram->aOL[countOL-((countOL > shortO)?shortO+1:0)];
			float outPL = dram->aPL[countPL-((countPL > shortP)?shortP+1:0)];
			
			float outAR = dram->aAR[countAR-((countAR > shortA)?shortA+1:0)];
			float outER = dram->aER[countER-((countER > shortE)?shortE+1:0)];
			float outIR = dram->aIR[countIR-((countIR > shortI)?shortI+1:0)];
			float outMR = dram->aMR[countMR-((countMR > shortM)?shortM+1:0)];			
			
			feedbackAL = outML - (outNL + outOL + outPL);
			feedbackDR = outAR - (outER + outIR + outMR);
			feedbackBL = outNL - (outML + outOL + outPL);
			feedbackHR = outER - (outAR + outIR + outMR);
			feedbackCL = outOL - (outML + outNL + outPL);
			feedbackLR = outIR - (outAR + outER + outMR);
			feedbackDL = outPL - (outML + outNL + outOL);
			feedbackPR = outMR - (outAR + outER + outIR);
			//which we need to feed back into the input again, a bit
			
			inputSampleL = (outML + outNL + outOL + outPL)/32.0f;
			inputSampleR = (outAR + outER + outIR + outMR)/32.0f;
			//and take the final combined sum of outputs, corrected for Householder gain and averaging
			
			dram->bez[bez_CL] = dram->bez[bez_BL];
			dram->bez[bez_BL] = dram->bez[bez_AL];
			dram->bez[bez_AL] = inputSampleL;
			dram->bez[bez_SampL] = 0.0f;
			
			dram->bez[bez_CR] = dram->bez[bez_BR];
			dram->bez[bez_BR] = dram->bez[bez_AR];
			dram->bez[bez_AR] = inputSampleR;
			dram->bez[bez_SampR] = 0.0f;
		}
		float CBL = (dram->bez[bez_CL]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_BL]*dram->bez[bez_cycle]);
		float CBR = (dram->bez[bez_CR]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_BR]*dram->bez[bez_cycle]);
		float BAL = (dram->bez[bez_BL]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_AL]*dram->bez[bez_cycle]);
		float BAR = (dram->bez[bez_BR]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_AR]*dram->bez[bez_cycle]);
		float CBAL = (dram->bez[bez_BL]+(CBL*(1.0f-dram->bez[bez_cycle]))+(BAL*dram->bez[bez_cycle]))*0.125f;
		float CBAR = (dram->bez[bez_BR]+(CBR*(1.0f-dram->bez[bez_cycle]))+(BAR*dram->bez[bez_cycle]))*0.125f;
		inputSampleL = CBAL;
		inputSampleR = CBAR;
		
		if (inputSampleL > 1.0f) inputSampleL = 1.0f;
		if (inputSampleL < -1.0f) inputSampleL = -1.0f;
		if (inputSampleR > 1.0f) inputSampleR = 1.0f;
		if (inputSampleR < -1.0f) inputSampleR = -1.0f;
		
		if (wet < 1.0f) {inputSampleL *= wet; inputSampleR *= wet;}
		if (dry < 1.0f) {drySampleL *= dry; drySampleR *= dry;}
		inputSampleL += drySampleL; inputSampleR += drySampleR;
		//this is our submix verb dry/wet: 0.5f is BOTH at FULL VOLUME
		//purpose is that, if you're adding verb, you're not altering other balances
		
		
		
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
	for(int count = 0; count < kshortA+2; count++) {dram->aAL[count] = 0.0; dram->aAR[count] = 0.0;}
	for(int count = 0; count < kshortB+2; count++) {dram->aBL[count] = 0.0; dram->aBR[count] = 0.0;}
	for(int count = 0; count < kshortC+2; count++) {dram->aCL[count] = 0.0; dram->aCR[count] = 0.0;}
	for(int count = 0; count < kshortD+2; count++) {dram->aDL[count] = 0.0; dram->aDR[count] = 0.0;}
	for(int count = 0; count < kshortE+2; count++) {dram->aEL[count] = 0.0; dram->aER[count] = 0.0;}
	for(int count = 0; count < kshortF+2; count++) {dram->aFL[count] = 0.0; dram->aFR[count] = 0.0;}
	for(int count = 0; count < kshortG+2; count++) {dram->aGL[count] = 0.0; dram->aGR[count] = 0.0;}
	for(int count = 0; count < kshortH+2; count++) {dram->aHL[count] = 0.0; dram->aHR[count] = 0.0;}
	for(int count = 0; count < kshortI+2; count++) {dram->aIL[count] = 0.0; dram->aIR[count] = 0.0;}
	for(int count = 0; count < kshortJ+2; count++) {dram->aJL[count] = 0.0; dram->aJR[count] = 0.0;}
	for(int count = 0; count < kshortK+2; count++) {dram->aKL[count] = 0.0; dram->aKR[count] = 0.0;}
	for(int count = 0; count < kshortL+2; count++) {dram->aLL[count] = 0.0; dram->aLR[count] = 0.0;}
	for(int count = 0; count < kshortM+2; count++) {dram->aML[count] = 0.0; dram->aMR[count] = 0.0;}
	for(int count = 0; count < kshortN+2; count++) {dram->aNL[count] = 0.0; dram->aNR[count] = 0.0;}
	for(int count = 0; count < kshortO+2; count++) {dram->aOL[count] = 0.0; dram->aOR[count] = 0.0;}
	for(int count = 0; count < kshortP+2; count++) {dram->aPL[count] = 0.0; dram->aPR[count] = 0.0;}
	
	for(int count = 0; count < predelay+2; count++) {dram->aZL[count] = 0.0; dram->aZR[count] = 0.0;}

	feedbackAL = 0.0;
	feedbackBL = 0.0;
	feedbackCL = 0.0;
	feedbackDL = 0.0;
	
	previousAL = 0.0;
	previousBL = 0.0;
	previousCL = 0.0;
	previousDL = 0.0;
	previousEL = 0.0;
	
	feedbackDR = 0.0;
	feedbackHR = 0.0;
	feedbackLR = 0.0;
	feedbackPR = 0.0;
	
	previousAR = 0.0;
	previousBR = 0.0;
	previousCR = 0.0;
	previousDR = 0.0;
	previousER = 0.0;
		
	countAL = 1;
	countBL = 1;
	countCL = 1;
	countDL = 1;	
	countEL = 1;
	countFL = 1;
	countGL = 1;
	countHL = 1;
	countIL = 1;
	countJL = 1;
	countKL = 1;
	countLL = 1;
	countML = 1;
	countNL = 1;
	countOL = 1;
	countPL = 1;
	
	countAR = 1;
	countBR = 1;
	countCR = 1;
	countDR = 1;	
	countER = 1;
	countFR = 1;
	countGR = 1;
	countHR = 1;
	countIR = 1;
	countJR = 1;
	countKR = 1;
	countLR = 1;
	countMR = 1;
	countNR = 1;
	countOR = 1;
	countPR = 1;
	
	countZ = 1;
	
	shortA = 336;
	shortB = 1660;
	shortC = 386;
	shortD = 623;
	shortE = 693;
	shortF = 1079;
	shortG = 891;
	shortH = 1574;
	shortI = 24;
	shortJ = 2641;
	shortK = 1239;
	shortL = 775;
	shortM = 11;
	shortN = 3104;
	shortO = 55;
	shortP = 2366;
	prevclearcoat = -1;
	
	for (int x = 0; x < bez_total; x++) dram->bez[x] = 0.0;
	dram->bez[bez_cycle] = 1.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
