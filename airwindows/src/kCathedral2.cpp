#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "kCathedral2"
#define AIRWINDOWS_DESCRIPTION "A giant reverby space modeled after the Bricasti Cathedral."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','k','C','2' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
const int predelay = 1014; const int vlfpredelay = 11000;
const int shortA = 78; const int shortB = 760; const int shortC = 982; const int shortD = 528; const int shortE = 445; const int shortF = 1128; const int shortG = 130; const int shortH = 708; const int shortI = 22; const int shortJ = 2144; const int shortK = 354; const int shortL = 1169; const int shortM = 11; const int shortN = 2782; const int shortO = 58; const int shortP = 1515; //5 to 159 ms, 809 seat hall. Scarcity, 1 in 212274
const int delayA = 871; const int delayB = 1037; const int delayC = 1205; const int delayD = 297; const int delayE = 467; const int delayF = 884; const int delayG = 173; const int delayH = 1456; const int delayI = 799; const int delayJ = 361; const int delayK = 1432; const int delayL = 338; const int delayM = 186; const int delayN = 1408; const int delayO = 1014; const int delayP = 23; const int delayQ = 807; const int delayR = 501; const int delayS = 1468; const int delayT = 1102; const int delayU = 11; const int delayV = 1119; const int delayW = 1315; const int delayX = 94; const int delayY = 1270; //15 to 155 ms, 874 seat hall  
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Wetness", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float gainOutL;
 	float gainOutR;
	
	
	

	int shortAL;
	int shortBL;
	int shortCL;
	int shortDL;
	int shortEL;
	int shortFL;
	int shortGL;
	int shortHL;
	int shortIL;
	int shortJL;
	int shortKL;
	int shortLL;		
	int shortML;		
	int shortNL;		
	int shortOL;		
	int shortPL;		
	
	int shortAR;
	int shortBR;
	int shortCR;
	int shortDR;
	int shortER;
	int shortFR;
	int shortGR;
	int shortHR;
	int shortIR;
	int shortJR;
	int shortKR;
	int shortLR;		
	int shortMR;		
	int shortNR;		
	int shortOR;		
	int shortPR;	
	
	
	
		
	
	
	
	
	
	float feedbackAL;
	float feedbackBL;
	float feedbackCL;
	float feedbackDL;
	float feedbackEL;
	
	float feedbackER;
	float feedbackJR;
	float feedbackOR;
	float feedbackTR;
	float feedbackYR;
		
	float lastRefL[7];
	float lastRefR[7];
	
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
	int countQL;		
	int countRL;		
	int countSL;		
	int countTL;		
	int countUL;		
	int countVL;		
	int countWL;		
	int countXL;		
	int countYL;		
	
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
	int countQR;		
	int countRR;		
	int countSR;		
	int countTR;		
	int countUR;		
	int countVR;		
	int countWR;		
	int countXR;		
	int countYR;
	
	int countZ;		
	int countVLF;		
	
	int cycle;
	
	enum {
		prevSampL1,
		prevSlewL1,
		prevSampR1,
		prevSlewR1,
		prevSampL2,
		prevSlewL2,
		prevSampR2,
		prevSlewR2,
		prevSampL3,
		prevSlewL3,
		prevSampR3,
		prevSlewR3,
		prevSampL4,
		prevSlewL4,
		prevSampR4,
		prevSlewR4,
		prevSampL5,
		prevSlewL5,
		prevSampR5,
		prevSlewR5,
		prevSampL6,
		prevSlewL6,
		prevSampR6,
		prevSlewR6,
		prevSampL7,
		prevSlewL7,
		prevSampR7,
		prevSlewR7,
		prevSampL8,
		prevSlewL8,
		prevSampR8,
		prevSlewR8,
		prevSampL9,
		prevSlewL9,
		prevSampR9,
		prevSlewR9,
		prevSampL10,
		prevSlewL10,
		prevSampR10,
		prevSlewR10,
		pear_total
	}; //fixed frequency pear filter for ultrasonics, stereo
	
	
	float vibratoL;
	float vibratoR;
	float vibAL;
	float vibAR;
	float vibBL;
	float vibBR;
	
	float subAL;
	float subAR;
	float subBL;
	float subBR;
	float subCL;
	float subCR;
	
	float sbAL;
	float sbAR;
	float sbBL;
	float sbBR;
	float sbCL;
	float sbCR;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float eAL[shortA+5];
	float eBL[shortB+5];
	float eCL[shortC+5];
	float eDL[shortD+5];
	float eEL[shortE+5];
	float eFL[shortF+5];
	float eGL[shortG+5];
	float eHL[shortH+5];
	float eIL[shortI+5];
	float eJL[shortJ+5];
	float eKL[shortK+5];
	float eLL[shortL+5];
	float eML[shortM+5];
	float eNL[shortN+5];
	float eOL[shortO+5];
	float ePL[shortP+5];
	float eAR[shortA+5];
	float eBR[shortB+5];
	float eCR[shortC+5];
	float eDR[shortD+5];
	float eER[shortE+5];
	float eFR[shortF+5];
	float eGR[shortG+5];
	float eHR[shortH+5];
	float eIR[shortI+5];
	float eJR[shortJ+5];
	float eKR[shortK+5];
	float eLR[shortL+5];
	float eMR[shortM+5];
	float eNR[shortN+5];
	float eOR[shortO+5];
	float ePR[shortP+5];
	float aAL[delayA+5];
	float aBL[delayB+5];
	float aCL[delayC+5];
	float aDL[delayD+5];
	float aEL[delayE+5];
	float aFL[delayF+5];
	float aGL[delayG+5];
	float aHL[delayH+5];
	float aIL[delayI+5];
	float aJL[delayJ+5];
	float aKL[delayK+5];
	float aLL[delayL+5];
	float aML[delayM+5];
	float aNL[delayN+5];
	float aOL[delayO+5];
	float aPL[delayP+5];
	float aQL[delayQ+5];
	float aRL[delayR+5];
	float aSL[delayS+5];
	float aTL[delayT+5];
	float aUL[delayU+5];
	float aVL[delayV+5];
	float aWL[delayW+5];
	float aXL[delayX+5];
	float aYL[delayY+5];
	float aAR[delayA+5];
	float aBR[delayB+5];
	float aCR[delayC+5];
	float aDR[delayD+5];
	float aER[delayE+5];
	float aFR[delayF+5];
	float aGR[delayG+5];
	float aHR[delayH+5];
	float aIR[delayI+5];
	float aJR[delayJ+5];
	float aKR[delayK+5];
	float aLR[delayL+5];
	float aMR[delayM+5];
	float aNR[delayN+5];
	float aOR[delayO+5];
	float aPR[delayP+5];
	float aQR[delayQ+5];
	float aRR[delayR+5];
	float aSR[delayS+5];
	float aTR[delayT+5];
	float aUR[delayU+5];
	float aVR[delayV+5];
	float aWR[delayW+5];
	float aXR[delayX+5];
	float aYR[delayY+5];
	float aZL[predelay+5];
	float aZR[predelay+5];
	float aVLFL[vlfpredelay+5];
	float aVLFR[vlfpredelay+5];
	float pearA[pear_total]; //probably worth just using a number here
	float pearB[pear_total]; //probably worth just using a number here
	float pearC[pear_total]; //probably worth just using a number here
	float pearD[pear_total]; //probably worth just using a number here
	float pearE[pear_total]; //probably worth just using a number here
	float pearF[pear_total]; //probably worth just using a number here
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check
	int adjPredelay = predelay;
	int adjSubDelay = vlfpredelay;
	
	int pearStages = 5;
	float pear = 0.388f;
	float pearScaled = (pear*0.388f)/(float)cycleEnd;
	
	float wet = GetParameter( kParam_One )*2.0f; 
	float dry = 2.0f - wet;
	if (wet > 1.0f) wet = 1.0f;
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
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a reverb sample
			//predelay
			dram->aZL[countZ] = inputSampleL;
			dram->aZR[countZ] = inputSampleR;
			countZ++; if (countZ < 0 || countZ > adjPredelay) countZ = 0;
			inputSampleL = dram->aZL[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			inputSampleR = dram->aZR[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			//end predelay
			
			//begin SubTight section
			float outSampleL = inputSampleL * 0.00187f;
			float outSampleR = inputSampleR * 0.00187f;
			float scale = 0.5f+fabs(outSampleL*0.5f);
			outSampleL = (subAL+(sin(subAL-outSampleL)*scale));
			subAL = outSampleL*scale;
			scale = 0.5f+fabs(outSampleR*0.5f);
			outSampleR = (subAR+(sin(subAR-outSampleR)*scale));
			subAR = outSampleR*scale;
			scale = 0.5f+fabs(outSampleL*0.5f);
			outSampleL = (subBL+(sin(subBL-outSampleL)*scale));
			subBL = outSampleL*scale;
			scale = 0.5f+fabs(outSampleR*0.5f);
			outSampleR = (subBR+(sin(subBR-outSampleR)*scale));
			subBR = outSampleR*scale;
			scale = 0.5f+fabs(outSampleL*0.5f);
			outSampleL = (subCL+(sin(subCL-outSampleL)*scale));
			subCL = outSampleL*scale;
			scale = 0.5f+fabs(outSampleR*0.5f);
			outSampleR = (subCR+(sin(subCR-outSampleR)*scale));
			subCR = outSampleR*scale;
			outSampleL = -outSampleL; outSampleR = -outSampleR;
			if (outSampleL > 0.25f) outSampleL = 0.25f; if (outSampleL < -0.25f) outSampleL = -0.25f;
			if (outSampleR > 0.25f) outSampleR = 0.25f; if (outSampleR < -0.25f) outSampleR = -0.25f;
			outSampleL *= 16.0f;
			outSampleR *= 16.0f;
			inputSampleL -= outSampleL;
			inputSampleR -= outSampleR;
			//end SubTight section
			
			float earlyAL = inputSampleL - (dram->eAL[(shortAL+1)-((shortAL+1 > shortA)?shortA+1:0)]*0.618033988749894848204586f);
			float earlyBL = inputSampleL - (dram->eBL[(shortBL+1)-((shortBL+1 > shortB)?shortB+1:0)]*0.618033988749894848204586f);
			float earlyCL = inputSampleL - (dram->eCL[(shortCL+1)-((shortCL+1 > shortC)?shortC+1:0)]*0.618033988749894848204586f);
			float earlyDL = inputSampleL - (dram->eDL[(shortDL+1)-((shortDL+1 > shortD)?shortD+1:0)]*0.618033988749894848204586f);
			float earlyDR = inputSampleR - (dram->eDR[(shortDR+1)-((shortDR+1 > shortD)?shortD+1:0)]*0.618033988749894848204586f);
			float earlyHR = inputSampleR - (dram->eHR[(shortHR+1)-((shortHR+1 > shortH)?shortH+1:0)]*0.618033988749894848204586f);
			float earlyLR = inputSampleR - (dram->eLR[(shortLR+1)-((shortLR+1 > shortL)?shortL+1:0)]*0.618033988749894848204586f);
			float earlyPR = inputSampleR - (dram->ePR[(shortPR+1)-((shortPR+1 > shortP)?shortP+1:0)]*0.618033988749894848204586f);
						
			dram->eAL[shortAL] = earlyAL; earlyAL *= 0.618033988749894848204586f;
			dram->eBL[shortBL] = earlyBL; earlyBL *= 0.618033988749894848204586f;
			dram->eCL[shortCL] = earlyCL; earlyCL *= 0.618033988749894848204586f;
			dram->eDL[shortDL] = earlyDL; earlyDL *= 0.618033988749894848204586f;
			dram->eDR[shortDR] = earlyDR; earlyDR *= 0.618033988749894848204586f;
			dram->eHR[shortHR] = earlyHR; earlyHR *= 0.618033988749894848204586f;
			dram->eLR[shortLR] = earlyLR; earlyLR *= 0.618033988749894848204586f;
			dram->ePR[shortPR] = earlyPR; earlyPR *= 0.618033988749894848204586f;
			
			shortAL++; if (shortAL < 0 || shortAL > shortA) shortAL = 0;
			shortBL++; if (shortBL < 0 || shortBL > shortB) shortBL = 0;
			shortCL++; if (shortCL < 0 || shortCL > shortC) shortCL = 0;
			shortDL++; if (shortDL < 0 || shortDL > shortD) shortDL = 0;
			shortDR++; if (shortDR < 0 || shortDR > shortD) shortDR = 0;
			shortHR++; if (shortHR < 0 || shortHR > shortH) shortHR = 0;
			shortLR++; if (shortLR < 0 || shortLR > shortL) shortLR = 0;
			shortPR++; if (shortPR < 0 || shortPR > shortP) shortPR = 0;
			
			earlyAL += dram->eAL[shortAL-((shortAL > shortA)?shortA+1:0)];
			earlyBL += dram->eBL[shortBL-((shortBL > shortB)?shortB+1:0)];
			earlyCL += dram->eCL[shortCL-((shortCL > shortC)?shortC+1:0)];
			earlyDL += dram->eDL[shortDL-((shortDL > shortD)?shortD+1:0)];
			earlyDR += dram->eDR[shortDR-((shortDR > shortD)?shortD+1:0)];
			earlyHR += dram->eHR[shortHR-((shortHR > shortH)?shortH+1:0)];
			earlyLR += dram->eLR[shortLR-((shortLR > shortL)?shortL+1:0)];
			earlyPR += dram->ePR[shortPR-((shortPR > shortP)?shortP+1:0)];
			
			float earlyEL = (earlyAL - (earlyBL + earlyCL + earlyDL)) - (dram->eEL[(shortEL+1)-((shortEL+1 > shortE)?shortE+1:0)]*0.618033988749894848204586f);
			float earlyFL = (earlyBL - (earlyAL + earlyCL + earlyDL)) - (dram->eFL[(shortFL+1)-((shortFL+1 > shortF)?shortF+1:0)]*0.618033988749894848204586f);
			float earlyGL = (earlyCL - (earlyAL + earlyBL + earlyDL)) - (dram->eGL[(shortGL+1)-((shortGL+1 > shortG)?shortG+1:0)]*0.618033988749894848204586f);
			float earlyHL = (earlyDL - (earlyAL + earlyBL + earlyCL)) - (dram->eHL[(shortHL+1)-((shortHL+1 > shortH)?shortH+1:0)]*0.618033988749894848204586f);
			float earlyCR = (earlyDR - (earlyHR + earlyLR + earlyPR)) - (dram->eCR[(shortCR+1)-((shortCR+1 > shortC)?shortC+1:0)]*0.618033988749894848204586f);
			float earlyGR = (earlyHR - (earlyDR + earlyLR + earlyPR)) - (dram->eGR[(shortGR+1)-((shortGR+1 > shortG)?shortG+1:0)]*0.618033988749894848204586f);
			float earlyKR = (earlyLR - (earlyDR + earlyHR + earlyPR)) - (dram->eKR[(shortKR+1)-((shortKR+1 > shortK)?shortK+1:0)]*0.618033988749894848204586f);
			float earlyOR = (earlyPR - (earlyDR + earlyHR + earlyLR)) - (dram->eOR[(shortOR+1)-((shortOR+1 > shortO)?shortO+1:0)]*0.618033988749894848204586f);
			
			dram->eEL[shortEL] = earlyEL; earlyEL *= 0.618033988749894848204586f;
			dram->eFL[shortFL] = earlyFL; earlyFL *= 0.618033988749894848204586f;
			dram->eGL[shortGL] = earlyGL; earlyGL *= 0.618033988749894848204586f;
			dram->eHL[shortHL] = earlyHL; earlyHL *= 0.618033988749894848204586f;
			dram->eCR[shortCR] = earlyCR; earlyCR *= 0.618033988749894848204586f;
			dram->eGR[shortGR] = earlyGR; earlyGR *= 0.618033988749894848204586f;
			dram->eKR[shortKR] = earlyKR; earlyKR *= 0.618033988749894848204586f;
			dram->eOR[shortOR] = earlyOR; earlyOR *= 0.618033988749894848204586f;
			
			shortEL++; if (shortEL < 0 || shortEL > shortE) shortEL = 0;
			shortFL++; if (shortFL < 0 || shortFL > shortF) shortFL = 0;
			shortGL++; if (shortGL < 0 || shortGL > shortG) shortGL = 0;
			shortHL++; if (shortHL < 0 || shortHL > shortH) shortHL = 0;
			shortCR++; if (shortCR < 0 || shortCR > shortC) shortCR = 0;
			shortGR++; if (shortGR < 0 || shortGR > shortG) shortGR = 0;
			shortKR++; if (shortKR < 0 || shortKR > shortK) shortKR = 0;
			shortOR++; if (shortOR < 0 || shortOR > shortO) shortOR = 0;
			
			earlyEL += dram->eEL[shortEL-((shortEL > shortE)?shortE+1:0)];
			earlyFL += dram->eFL[shortFL-((shortFL > shortF)?shortF+1:0)];
			earlyGL += dram->eGL[shortGL-((shortGL > shortG)?shortG+1:0)];
			earlyHL += dram->eHL[shortHL-((shortHL > shortH)?shortH+1:0)];
			earlyCR += dram->eCR[shortCR-((shortCR > shortC)?shortC+1:0)];
			earlyGR += dram->eGR[shortGR-((shortGR > shortG)?shortG+1:0)];
			earlyKR += dram->eKR[shortKR-((shortKR > shortK)?shortK+1:0)];
			earlyOR += dram->eOR[shortOR-((shortOR > shortO)?shortO+1:0)];
			
			float earlyIL = (earlyEL - (earlyFL + earlyGL + earlyHL)) - (dram->eIL[(shortIL+1)-((shortIL+1 > shortI)?shortI+1:0)]*0.618033988749894848204586f);
			float earlyJL = (earlyFL - (earlyEL + earlyGL + earlyHL)) - (dram->eJL[(shortJL+1)-((shortJL+1 > shortJ)?shortJ+1:0)]*0.618033988749894848204586f);
			float earlyKL = (earlyGL - (earlyEL + earlyFL + earlyHL)) - (dram->eKL[(shortKL+1)-((shortKL+1 > shortK)?shortK+1:0)]*0.618033988749894848204586f);
			float earlyLL = (earlyHL - (earlyEL + earlyFL + earlyGL)) - (dram->eLL[(shortLL+1)-((shortLL+1 > shortL)?shortL+1:0)]*0.618033988749894848204586f);
			float earlyBR = (earlyCR - (earlyGR + earlyKR + earlyOR)) - (dram->eBR[(shortBR+1)-((shortBR+1 > shortB)?shortB+1:0)]*0.618033988749894848204586f);
			float earlyFR = (earlyGR - (earlyCR + earlyKR + earlyOR)) - (dram->eFR[(shortFR+1)-((shortFR+1 > shortF)?shortF+1:0)]*0.618033988749894848204586f);
			float earlyJR = (earlyKR - (earlyCR + earlyGR + earlyOR)) - (dram->eJR[(shortJR+1)-((shortJR+1 > shortJ)?shortJ+1:0)]*0.618033988749894848204586f);
			float earlyNR = (earlyOR - (earlyCR + earlyGR + earlyKR)) - (dram->eNR[(shortNR+1)-((shortNR+1 > shortN)?shortN+1:0)]*0.618033988749894848204586f);
			
			dram->eIL[shortIL] = earlyIL; earlyIL *= 0.618033988749894848204586f;
			dram->eJL[shortJL] = earlyJL; earlyJL *= 0.618033988749894848204586f;
			dram->eKL[shortKL] = earlyKL; earlyKL *= 0.618033988749894848204586f;
			dram->eLL[shortLL] = earlyLL; earlyLL *= 0.618033988749894848204586f;
			dram->eBR[shortBR] = earlyBR; earlyBR *= 0.618033988749894848204586f;
			dram->eFR[shortFR] = earlyFR; earlyFR *= 0.618033988749894848204586f;
			dram->eJR[shortJR] = earlyJR; earlyJR *= 0.618033988749894848204586f;
			dram->eNR[shortNR] = earlyNR; earlyNR *= 0.618033988749894848204586f;
			
			shortIL++; if (shortIL < 0 || shortIL > shortI) shortIL = 0;
			shortJL++; if (shortJL < 0 || shortJL > shortJ) shortJL = 0;
			shortKL++; if (shortKL < 0 || shortKL > shortK) shortKL = 0;
			shortLL++; if (shortLL < 0 || shortLL > shortL) shortLL = 0;
			shortBR++; if (shortBR < 0 || shortBR > shortB) shortBR = 0;
			shortFR++; if (shortFR < 0 || shortFR > shortF) shortFR = 0;
			shortJR++; if (shortJR < 0 || shortJR > shortJ) shortJR = 0;
			shortNR++; if (shortNR < 0 || shortNR > shortN) shortNR = 0;
			
			earlyIL += dram->eIL[shortIL-((shortIL > shortI)?shortI+1:0)];
			earlyJL += dram->eJL[shortJL-((shortJL > shortJ)?shortJ+1:0)];
			earlyKL += dram->eKL[shortKL-((shortKL > shortK)?shortK+1:0)];
			earlyLL += dram->eLL[shortLL-((shortLL > shortL)?shortL+1:0)];
			earlyBR += dram->eBR[shortBR-((shortBR > shortB)?shortB+1:0)];
			earlyFR += dram->eFR[shortFR-((shortFR > shortF)?shortF+1:0)];
			earlyJR += dram->eJR[shortJR-((shortJR > shortJ)?shortJ+1:0)];
			earlyNR += dram->eNR[shortNR-((shortNR > shortN)?shortN+1:0)];
			
			float earlyML = (earlyIL - (earlyJL + earlyKL + earlyLL)) - (dram->eML[(shortML+1)-((shortML+1 > shortM)?shortM+1:0)]*0.618033988749894848204586f);
			float earlyNL = (earlyJL - (earlyIL + earlyKL + earlyLL)) - (dram->eNL[(shortNL+1)-((shortNL+1 > shortN)?shortN+1:0)]*0.618033988749894848204586f);
			float earlyOL = (earlyKL - (earlyIL + earlyJL + earlyLL)) - (dram->eOL[(shortOL+1)-((shortOL+1 > shortO)?shortO+1:0)]*0.618033988749894848204586f);
			float earlyPL = (earlyLL - (earlyIL + earlyJL + earlyKL)) - (dram->ePL[(shortPL+1)-((shortPL+1 > shortP)?shortP+1:0)]*0.618033988749894848204586f);
			float earlyAR = (earlyBR - (earlyFR + earlyJR + earlyNR)) - (dram->eAR[(shortAR+1)-((shortAR+1 > shortA)?shortA+1:0)]*0.618033988749894848204586f);
			float earlyER = (earlyFR - (earlyBR + earlyJR + earlyNR)) - (dram->eER[(shortER+1)-((shortER+1 > shortE)?shortE+1:0)]*0.618033988749894848204586f);
			float earlyIR = (earlyJR - (earlyBR + earlyFR + earlyNR)) - (dram->eIR[(shortIR+1)-((shortIR+1 > shortI)?shortI+1:0)]*0.618033988749894848204586f);
			float earlyMR = (earlyNR - (earlyBR + earlyFR + earlyJR)) - (dram->eMR[(shortMR+1)-((shortMR+1 > shortM)?shortM+1:0)]*0.618033988749894848204586f);
			
			dram->eML[shortML] = earlyML; earlyML *= 0.618033988749894848204586f;
			dram->eNL[shortNL] = earlyNL; earlyNL *= 0.618033988749894848204586f;
			dram->eOL[shortOL] = earlyOL; earlyOL *= 0.618033988749894848204586f;
			dram->ePL[shortPL] = earlyPL; earlyPL *= 0.618033988749894848204586f;
			dram->eAR[shortAR] = earlyAR; earlyAR *= 0.618033988749894848204586f;
			dram->eER[shortER] = earlyER; earlyER *= 0.618033988749894848204586f;
			dram->eIR[shortIR] = earlyIR; earlyIR *= 0.618033988749894848204586f;
			dram->eMR[shortMR] = earlyMR; earlyMR *= 0.618033988749894848204586f;
			
			shortML++; if (shortML < 0 || shortML > shortM) shortML = 0;
			shortNL++; if (shortNL < 0 || shortNL > shortN) shortNL = 0;
			shortOL++; if (shortOL < 0 || shortOL > shortO) shortOL = 0;
			shortPL++; if (shortPL < 0 || shortPL > shortP) shortPL = 0;
			shortAR++; if (shortAR < 0 || shortAR > shortA) shortAR = 0;
			shortER++; if (shortER < 0 || shortER > shortE) shortER = 0;
			shortIR++; if (shortIR < 0 || shortIR > shortI) shortIR = 0;
			shortMR++; if (shortMR < 0 || shortMR > shortM) shortMR = 0;
			
			earlyML += dram->eML[shortML-((shortML > shortM)?shortM+1:0)];
			earlyNL += dram->eNL[shortNL-((shortNL > shortN)?shortN+1:0)];
			earlyOL += dram->eOL[shortOL-((shortOL > shortO)?shortO+1:0)];
			earlyPL += dram->ePL[shortPL-((shortPL > shortP)?shortP+1:0)];
			earlyAR += dram->eAR[shortAR-((shortAR > shortA)?shortA+1:0)];
			earlyER += dram->eER[shortER-((shortER > shortE)?shortE+1:0)];
			earlyIR += dram->eIR[shortIR-((shortIR > shortI)?shortI+1:0)];
			earlyMR += dram->eMR[shortMR-((shortMR > shortM)?shortM+1:0)];
			
			float earlyReflectionsL = -(earlyML + earlyNL + earlyOL + earlyPL)/8.0f;
			float earlyReflectionsR = -(earlyAR + earlyER + earlyIR + earlyMR)/8.0f;
			//and take the final combined sum of outputs, corrected for Householder gain
						
			//VLF predelay
			dram->aVLFL[countVLF] = outSampleL;
			dram->aVLFR[countVLF] = outSampleR;
			countVLF++; if (countVLF < 0 || countVLF > adjSubDelay) countVLF = 0;
			outSampleL = dram->aVLFL[countVLF-((countVLF > adjSubDelay)?adjSubDelay+1:0)] * 2.0f;
			outSampleR = dram->aVLFR[countVLF-((countVLF > adjSubDelay)?adjSubDelay+1:0)] * 2.0f;
			//end VLF predelay
						
			inputSampleL += outSampleL;
			inputSampleR += outSampleR;
			//having re-added our VLF delayed channel we can now re-use outSample
			
			dram->aAL[countAL] = inputSampleL + (feedbackAL * 0.000285f);
			dram->aBL[countBL] = inputSampleL + (feedbackBL * 0.000285f);
			dram->aCL[countCL] = inputSampleL + (feedbackCL * 0.000285f);
			dram->aDL[countDL] = inputSampleL + (feedbackDL * 0.000285f);
			dram->aEL[countEL] = inputSampleL + (feedbackEL * 0.000285f);
			
			dram->aER[countER] = inputSampleR + (feedbackER * 0.000285f);
			dram->aJR[countJR] = inputSampleR + (feedbackJR * 0.000285f);
			dram->aOR[countOR] = inputSampleR + (feedbackOR * 0.000285f);
			dram->aTR[countTR] = inputSampleR + (feedbackTR * 0.000285f);
			dram->aYR[countYR] = inputSampleR + (feedbackYR * 0.000285f);
			
			countAL++; if (countAL < 0 || countAL > delayA) countAL = 0;
			countBL++; if (countBL < 0 || countBL > delayB) countBL = 0;
			countCL++; if (countCL < 0 || countCL > delayC) countCL = 0;
			countDL++; if (countDL < 0 || countDL > delayD) countDL = 0;
			countEL++; if (countEL < 0 || countEL > delayE) countEL = 0;
			
			countER++; if (countER < 0 || countER > delayE) countER = 0;
			countJR++; if (countJR < 0 || countJR > delayJ) countJR = 0;
			countOR++; if (countOR < 0 || countOR > delayO) countOR = 0;
			countTR++; if (countTR < 0 || countTR > delayT) countTR = 0;
			countYR++; if (countYR < 0 || countYR > delayY) countYR = 0;
			
			float outAL = dram->aAL[countAL-((countAL > delayA)?delayA+1:0)];
			float outBL = dram->aBL[countBL-((countBL > delayB)?delayB+1:0)];
			float outCL = dram->aCL[countCL-((countCL > delayC)?delayC+1:0)];
			float outDL = dram->aDL[countDL-((countDL > delayD)?delayD+1:0)];
			float outEL = dram->aEL[countEL-((countEL > delayE)?delayE+1:0)];
			
			float outER = dram->aER[countER-((countER > delayE)?delayE+1:0)];
			float outJR = dram->aJR[countJR-((countJR > delayJ)?delayJ+1:0)];
			float outOR = dram->aOR[countOR-((countOR > delayO)?delayO+1:0)];
			float outTR = dram->aTR[countTR-((countTR > delayT)?delayT+1:0)];
			float outYR = dram->aYR[countYR-((countYR > delayY)?delayY+1:0)];
			
			//-------- one
						
			dram->aFL[countFL] = ((outAL*3.0f) - ((outBL + outCL + outDL + outEL)*2.0f));
			dram->aGL[countGL] = ((outBL*3.0f) - ((outAL + outCL + outDL + outEL)*2.0f));
			dram->aHL[countHL] = ((outCL*3.0f) - ((outAL + outBL + outDL + outEL)*2.0f));
			dram->aIL[countIL] = ((outDL*3.0f) - ((outAL + outBL + outCL + outEL)*2.0f));
			dram->aJL[countJL] = ((outEL*3.0f) - ((outAL + outBL + outCL + outDL)*2.0f));
			
			dram->aDR[countDR] = ((outER*3.0f) - ((outJR + outOR + outTR + outYR)*2.0f));
			dram->aIR[countIR] = ((outJR*3.0f) - ((outER + outOR + outTR + outYR)*2.0f));
			dram->aNR[countNR] = ((outOR*3.0f) - ((outER + outJR + outTR + outYR)*2.0f));
			dram->aSR[countSR] = ((outTR*3.0f) - ((outER + outJR + outOR + outYR)*2.0f));
			dram->aXR[countXR] = ((outYR*3.0f) - ((outER + outJR + outOR + outTR)*2.0f));
			
			countFL++; if (countFL < 0 || countFL > delayF) countFL = 0;
			countGL++; if (countGL < 0 || countGL > delayG) countGL = 0;
			countHL++; if (countHL < 0 || countHL > delayH) countHL = 0;
			countIL++; if (countIL < 0 || countIL > delayI) countIL = 0;
			countJL++; if (countJL < 0 || countJL > delayJ) countJL = 0;
			
			countDR++; if (countDR < 0 || countDR > delayD) countDR = 0;
			countIR++; if (countIR < 0 || countIR > delayI) countIR = 0;
			countNR++; if (countNR < 0 || countNR > delayN) countNR = 0;
			countSR++; if (countSR < 0 || countSR > delayS) countSR = 0;
			countXR++; if (countXR < 0 || countXR > delayX) countXR = 0;
			
			float outFL = dram->aFL[countFL-((countFL > delayF)?delayF+1:0)];
			float outGL = dram->aGL[countGL-((countGL > delayG)?delayG+1:0)];
			float outHL = dram->aHL[countHL-((countHL > delayH)?delayH+1:0)];
			float outIL = dram->aIL[countIL-((countIL > delayI)?delayI+1:0)];
			float outJL = dram->aJL[countJL-((countJL > delayJ)?delayJ+1:0)];
			
			float outDR = dram->aDR[countDR-((countDR > delayD)?delayD+1:0)];
			float outIR = dram->aIR[countIR-((countIR > delayI)?delayI+1:0)];
			float outNR = dram->aNR[countNR-((countNR > delayN)?delayN+1:0)];
			float outSR = dram->aSR[countSR-((countSR > delayS)?delayS+1:0)];
			float outXR = dram->aXR[countXR-((countXR > delayX)?delayX+1:0)];
			
			//-------- two
			
			dram->aKL[countKL] = ((outFL*3.0f) - ((outGL + outHL + outIL + outJL)*2.0f));
			dram->aLL[countLL] = ((outGL*3.0f) - ((outFL + outHL + outIL + outJL)*2.0f));
			dram->aML[countML] = ((outHL*3.0f) - ((outFL + outGL + outIL + outJL)*2.0f));
			dram->aNL[countNL] = ((outIL*3.0f) - ((outFL + outGL + outHL + outJL)*2.0f));
			dram->aOL[countOL] = ((outJL*3.0f) - ((outFL + outGL + outHL + outIL)*2.0f));
			
			dram->aCR[countCR] = ((outDR*3.0f) - ((outIR + outNR + outSR + outXR)*2.0f));
			dram->aHR[countHR] = ((outIR*3.0f) - ((outDR + outNR + outSR + outXR)*2.0f));
			dram->aMR[countMR] = ((outNR*3.0f) - ((outDR + outIR + outSR + outXR)*2.0f));
			dram->aRR[countRR] = ((outSR*3.0f) - ((outDR + outIR + outNR + outXR)*2.0f));
			dram->aWR[countWR] = ((outXR*3.0f) - ((outDR + outIR + outNR + outSR)*2.0f));
			
			countKL++; if (countKL < 0 || countKL > delayK) countKL = 0;
			countLL++; if (countLL < 0 || countLL > delayL) countLL = 0;
			countML++; if (countML < 0 || countML > delayM) countML = 0;
			countNL++; if (countNL < 0 || countNL > delayN) countNL = 0;
			countOL++; if (countOL < 0 || countOL > delayO) countOL = 0;
			
			countCR++; if (countCR < 0 || countCR > delayC) countCR = 0;
			countHR++; if (countHR < 0 || countHR > delayH) countHR = 0;
			countMR++; if (countMR < 0 || countMR > delayM) countMR = 0;
			countRR++; if (countRR < 0 || countRR > delayR) countRR = 0;
			countWR++; if (countWR < 0 || countWR > delayW) countWR = 0;
			
			float outKL = dram->aKL[countKL-((countKL > delayK)?delayK+1:0)];
			float outLL = dram->aLL[countLL-((countLL > delayL)?delayL+1:0)];
			float outML = dram->aML[countML-((countML > delayM)?delayM+1:0)];
			float outNL = dram->aNL[countNL-((countNL > delayN)?delayN+1:0)];
			float outOL = dram->aOL[countOL-((countOL > delayO)?delayO+1:0)];
			
			float outCR = dram->aCR[countCR-((countCR > delayC)?delayC+1:0)];
			float outHR = dram->aHR[countHR-((countHR > delayH)?delayH+1:0)];
			float outMR = dram->aMR[countMR-((countMR > delayM)?delayM+1:0)];
			float outRR = dram->aRR[countRR-((countRR > delayR)?delayR+1:0)];
			float outWR = dram->aWR[countWR-((countWR > delayW)?delayW+1:0)];
			
			//-------- three
			
			dram->aPL[countPL] = ((outKL*3.0f) - ((outLL + outML + outNL + outOL)*2.0f));
			dram->aQL[countQL] = ((outLL*3.0f) - ((outKL + outML + outNL + outOL)*2.0f));
			dram->aRL[countRL] = ((outML*3.0f) - ((outKL + outLL + outNL + outOL)*2.0f));
			dram->aSL[countSL] = ((outNL*3.0f) - ((outKL + outLL + outML + outOL)*2.0f));
			dram->aTL[countTL] = ((outOL*3.0f) - ((outKL + outLL + outML + outNL)*2.0f));
			
			dram->aBR[countBR] = ((outCR*3.0f) - ((outHR + outMR + outRR + outWR)*2.0f));
			dram->aGR[countGR] = ((outHR*3.0f) - ((outCR + outMR + outRR + outWR)*2.0f));
			dram->aLR[countLR] = ((outMR*3.0f) - ((outCR + outHR + outRR + outWR)*2.0f));
			dram->aQR[countQR] = ((outRR*3.0f) - ((outCR + outHR + outMR + outWR)*2.0f));
			dram->aVR[countVR] = ((outWR*3.0f) - ((outCR + outHR + outMR + outRR)*2.0f));
			
			countPL++; if (countPL < 0 || countPL > delayP) countPL = 0;
			countQL++; if (countQL < 0 || countQL > delayQ) countQL = 0;
			countRL++; if (countRL < 0 || countRL > delayR) countRL = 0;
			countSL++; if (countSL < 0 || countSL > delayS) countSL = 0;
			countTL++; if (countTL < 0 || countTL > delayT) countTL = 0;
			
			countBR++; if (countBR < 0 || countBR > delayB) countBR = 0;
			countGR++; if (countGR < 0 || countGR > delayG) countGR = 0;
			countLR++; if (countLR < 0 || countLR > delayL) countLR = 0;
			countQR++; if (countQR < 0 || countQR > delayQ) countQR = 0;
			countVR++; if (countVR < 0 || countVR > delayV) countVR = 0;
			
			float outPL = dram->aPL[countPL-((countPL > delayP)?delayP+1:0)];
			float outQL = dram->aQL[countQL-((countQL > delayQ)?delayQ+1:0)];
			float outRL = dram->aRL[countRL-((countRL > delayR)?delayR+1:0)];
			float outSL = dram->aSL[countSL-((countSL > delayS)?delayS+1:0)];
			float outTL = dram->aTL[countTL-((countTL > delayT)?delayT+1:0)];
			
			float outBR = dram->aBR[countBR-((countBR > delayB)?delayB+1:0)];
			float outGR = dram->aGR[countGR-((countGR > delayG)?delayG+1:0)];
			float outLR = dram->aLR[countLR-((countLR > delayL)?delayL+1:0)];
			float outQR = dram->aQR[countQR-((countQR > delayQ)?delayQ+1:0)];
			float outVR = dram->aVR[countVR-((countVR > delayV)?delayV+1:0)];
						
			//-------- four
			
			dram->aVL[countVL] = ((outQL*3.0f) - ((outPL + outRL + outSL + outTL)*2.0f));
			dram->aWL[countWL] = ((outRL*3.0f) - ((outPL + outQL + outSL + outTL)*2.0f));
			dram->aXL[countXL] = ((outSL*3.0f) - ((outPL + outQL + outRL + outTL)*2.0f));
			dram->aYL[countYL] = ((outTL*3.0f) - ((outPL + outQL + outRL + outSL)*2.0f));
			
			dram->aAR[countAR] = ((outBR*3.0f) - ((outGR + outLR + outQR + outVR)*2.0f));
			dram->aFR[countFR] = ((outGR*3.0f) - ((outBR + outLR + outQR + outVR)*2.0f));
			dram->aKR[countKR] = ((outLR*3.0f) - ((outBR + outGR + outQR + outVR)*2.0f));
			dram->aPR[countPR] = ((outQR*3.0f) - ((outBR + outGR + outLR + outVR)*2.0f));
			
			float outUL = ((outPL*3.0f) - ((outQL + outRL + outSL + outTL)*2.0f)) - (dram->aUL[(countUL+1)-((countUL+1 > delayU)?delayU+1:0)]*0.618033988749894848204586f);
			float outUR = ((outVR*3.0f) - ((outBR + outGR + outLR + outQR)*2.0f)) - (dram->aUR[(countUR+1)-((countUR+1 > delayU)?delayU+1:0)]*0.618033988749894848204586f);
			dram->aUL[countUL] = outUL; outUL *= 0.618033988749894848204586f;
			dram->aUR[countUR] = outUR; outUR *= 0.618033988749894848204586f;
			countUL++; if (countUL < 0 || countUL > delayU) countUL = 0;
			countUR++; if (countUR < 0 || countUR > delayU) countUR = 0;
			outUL += dram->aUL[countUL-((countUL > delayU)?delayU+1:0)];
			outUR += dram->aUR[countUR-((countUR > delayU)?delayU+1:0)];
			//the 11-length delay slot becomes a sole allpass
			
			vibBL = vibAL; vibAL = outUL;
			vibBR = vibAR; vibAR = outUR; //tiny two sample delay chains
			vibratoL += fpdL * 0.5e-13f; if (vibratoL > M_PI*2.0f) vibratoL -= M_PI*2.0f;
			vibratoR += fpdR * 0.5e-13f; if (vibratoR > M_PI*2.0f) vibratoR -= M_PI*2.0f;
			float quadL = sin(vibratoL)+1.0f;
			float quadR = sin(vibratoR)+1.0f;
			//quadrature delay points play back from a position in delay chains
			if (quadL < 1.0f) outUL = (outUL*(1.0f-quadL))+(vibAL*quadL);
			else outUL = (vibAL*(1.0f-(quadL-1.0f)))+(vibBL*(quadL-1.0f));
			if (quadR < 1.0f) outUR = (outUR*(1.0f-quadR))+(vibAR*quadR);
			else outUR = (vibAR*(1.0f-(quadR-1.0f)))+(vibBR*(quadR-1.0f));
			//also, pitch drift this allpass slot for very subtle motion
			
			countVL++; if (countVL < 0 || countVL > delayV) countVL = 0;
			countWL++; if (countWL < 0 || countWL > delayW) countWL = 0;
			countXL++; if (countXL < 0 || countXL > delayX) countXL = 0;
			countYL++; if (countYL < 0 || countYL > delayY) countYL = 0;
			
			countAR++; if (countAR < 0 || countAR > delayA) countAR = 0;
			countFR++; if (countFR < 0 || countFR > delayF) countFR = 0;
			countKR++; if (countKR < 0 || countKR > delayK) countKR = 0;
			countPR++; if (countPR < 0 || countPR > delayP) countPR = 0;
			
			float outVL = dram->aVL[countVL-((countVL > delayV)?delayV+1:0)];
			float outWL = dram->aWL[countWL-((countWL > delayW)?delayW+1:0)];
			float outXL = dram->aXL[countXL-((countXL > delayX)?delayX+1:0)];
			float outYL = dram->aYL[countYL-((countYL > delayY)?delayY+1:0)];
			
			float outAR = dram->aAR[countAR-((countAR > delayA)?delayA+1:0)];
			float outFR = dram->aFR[countFR-((countFR > delayF)?delayF+1:0)];
			float outKR = dram->aKR[countKR-((countKR > delayK)?delayK+1:0)];
			float outPR = dram->aPR[countPR-((countPR > delayP)?delayP+1:0)];
			
			//-------- five
			
			feedbackER = ((outUL*3.0f) - ((outVL + outWL + outXL + outYL)*2.0f));
			feedbackAL = ((outAR*3.0f) - ((outFR + outKR + outPR + outUR)*2.0f));
			for (int x = 0; x < 1; x += 4) {
				float slew = ((feedbackAL - dram->pearA[x]) + dram->pearA[x+1])*pear*0.5f;
				dram->pearA[x] = feedbackAL = (pear * feedbackAL) + ((1.0f-pear) * (dram->pearA[x] + dram->pearA[x+1]));
				dram->pearA[x+1] = slew;
				slew = ((feedbackER - dram->pearA[x+2]) + dram->pearA[x+3])*pear*0.5f;
				dram->pearA[x+2] = feedbackER = (pear * feedbackER) + ((1.0f-pear) * (dram->pearA[x+2] + dram->pearA[x+3]));
				dram->pearA[x+3] = slew;
			}			
			
			feedbackBL = ((outVL*3.0f) - ((outUL + outWL + outXL + outYL)*2.0f));
			feedbackJR = ((outFR*3.0f) - ((outAR + outKR + outPR + outUR)*2.0f));
			for (int x = 0; x < pearStages; x += 4) {
				float slew = ((feedbackBL - dram->pearB[x]) + dram->pearB[x+1])*pear*0.5f;
				dram->pearB[x] = feedbackBL = (pear * feedbackBL) + ((1.0f-pear) * (dram->pearB[x] + dram->pearB[x+1]));
				dram->pearB[x+1] = slew;
				slew = ((feedbackJR - dram->pearB[x+2]) + dram->pearB[x+3])*pear*0.5f;
				dram->pearB[x+2] = feedbackJR = (pear * feedbackJR) + ((1.0f-pear) * (dram->pearB[x+2] + dram->pearB[x+3]));
				dram->pearB[x+3] = slew;
			}			
			
			feedbackCL = ((outWL*3.0f) - ((outUL + outVL + outXL + outYL)*2.0f));
			feedbackOR = ((outKR*3.0f) - ((outAR + outFR + outPR + outUR)*2.0f));
			for (int x = 0; x < pearStages; x += 4) {
				float slew = ((feedbackCL - dram->pearC[x]) + dram->pearC[x+1])*pear*0.5f;
				dram->pearC[x] = feedbackCL = (pear * feedbackCL) + ((1.0f-pear) * (dram->pearC[x] + dram->pearC[x+1]));
				dram->pearC[x+1] = slew;
				slew = ((feedbackOR - dram->pearC[x+2]) + dram->pearC[x+3])*pear*0.5f;
				dram->pearC[x+2] = feedbackOR = (pear * feedbackOR) + ((1.0f-pear) * (dram->pearC[x+2] + dram->pearC[x+3]));
				dram->pearC[x+3] = slew;
			}			
			
			feedbackDL = ((outXL*3.0f) - ((outUL + outVL + outWL + outYL)*2.0f));
			feedbackTR = ((outPR*3.0f) - ((outAR + outFR + outKR + outUR)*2.0f));
			for (int x = 0; x < pearStages; x += 4) {
				float slew = ((feedbackDL - dram->pearD[x]) + dram->pearD[x+1])*pear*0.5f;
				dram->pearD[x] = feedbackDL = (pear * feedbackDL) + ((1.0f-pear) * (dram->pearD[x] + dram->pearD[x+1]));
				dram->pearD[x+1] = slew;
				slew = ((feedbackTR - dram->pearD[x+2]) + dram->pearD[x+3])*pear*0.5f;
				dram->pearD[x+2] = feedbackTR = (pear * feedbackTR) + ((1.0f-pear) * (dram->pearD[x+2] + dram->pearD[x+3]));
				dram->pearD[x+3] = slew;
			}			
			
			feedbackEL = ((outYL*3.0f) - ((outUL + outVL + outWL + outXL)*2.0f));
			feedbackYR = ((outUR*3.0f) - ((outAR + outFR + outKR + outPR)*2.0f));
			for (int x = 0; x < pearStages; x += 4) {
				float slew = ((feedbackEL - dram->pearE[x]) + dram->pearE[x+1])*pear*0.5f;
				dram->pearE[x] = feedbackEL = (pear * feedbackEL) + ((1.0f-pear) * (dram->pearE[x] + dram->pearE[x+1]));
				dram->pearE[x+1] = slew;
				slew = ((feedbackYR - dram->pearE[x+2]) + dram->pearE[x+3])*pear*0.5f;
				dram->pearE[x+2] = feedbackYR = (pear * feedbackYR) + ((1.0f-pear) * (dram->pearE[x+2] + dram->pearE[x+3]));
				dram->pearE[x+3] = slew;
			}			
			//which we need to feed back into the input again, a bit
						 
			inputSampleL = (outUL + outVL + outWL + outXL + outYL)*0.0004f;
			inputSampleR = (outAR + outFR + outKR + outPR + outUR)*0.0004f;
			//and take the final combined sum of outputs, corrected for Householder gain

			//begin SubBoost section
			outSampleL = inputSampleL * 0.00186f;
			outSampleR = inputSampleR * 0.00186f;
			scale = 0.5f+fabs(outSampleL*0.5f);
			outSampleL = (sbAL+(sin(sbAL-outSampleL)*scale));
			sbAL = outSampleL*scale;
			scale = 0.5f+fabs(outSampleR*0.5f);
			outSampleR = (sbAR+(sin(sbAR-outSampleR)*scale));
			sbAR = outSampleR*scale;
			scale = 0.5f+fabs(outSampleL*0.5f);
			outSampleL = (sbBL+(sin(sbBL-outSampleL)*scale));
			sbBL = outSampleL*scale;
			scale = 0.5f+fabs(outSampleR*0.5f);
			outSampleR = (sbBR+(sin(sbBR-outSampleR)*scale));
			sbBR = outSampleR*scale;
			scale = 0.5f+fabs(outSampleL*0.5f);
			outSampleL = (sbCL+(sin(sbCL-outSampleL)*scale));
			sbCL = outSampleL*scale;
			scale = 0.5f+fabs(outSampleR*0.5f);
			outSampleR = (sbCR+(sin(sbCR-outSampleR)*scale));
			sbCR = outSampleR*scale;
			outSampleL = -outSampleL; outSampleR = -outSampleR;
			if (outSampleL > 0.25f) outSampleL = 0.25f; if (outSampleL < -0.25f) outSampleL = -0.25f;
			if (outSampleR > 0.25f) outSampleR = 0.25f; if (outSampleR < -0.25f) outSampleR = -0.25f;
			outSampleL *= 32.0f;
			outSampleR *= 32.0f;
			inputSampleL += outSampleL;
			inputSampleR += outSampleR;
			//end SubBoost section
			
			inputSampleL += (earlyReflectionsL*0.25f);
			inputSampleR += (earlyReflectionsR*0.25f);			
			
			if (cycleEnd == 4) {
				lastRefL[0] = lastRefL[4]; //start from previous last
				lastRefL[2] = (lastRefL[0] + inputSampleL)/2; //half
				lastRefL[1] = (lastRefL[0] + lastRefL[2])/2; //one quarter
				lastRefL[3] = (lastRefL[2] + inputSampleL)/2; //three quarters
				lastRefL[4] = inputSampleL; //full
				lastRefR[0] = lastRefR[4]; //start from previous last
				lastRefR[2] = (lastRefR[0] + inputSampleR)/2; //half
				lastRefR[1] = (lastRefR[0] + lastRefR[2])/2; //one quarter
				lastRefR[3] = (lastRefR[2] + inputSampleR)/2; //three quarters
				lastRefR[4] = inputSampleR; //full
			}
			if (cycleEnd == 3) {
				lastRefL[0] = lastRefL[3]; //start from previous last
				lastRefL[2] = (lastRefL[0]+lastRefL[0]+inputSampleL)/3; //third
				lastRefL[1] = (lastRefL[0]+inputSampleL+inputSampleL)/3; //two thirds
				lastRefL[3] = inputSampleL; //full
				lastRefR[0] = lastRefR[3]; //start from previous last
				lastRefR[2] = (lastRefR[0]+lastRefR[0]+inputSampleR)/3; //third
				lastRefR[1] = (lastRefR[0]+inputSampleR+inputSampleR)/3; //two thirds
				lastRefR[3] = inputSampleR; //full
			}
			if (cycleEnd == 2) {
				lastRefL[0] = lastRefL[2]; //start from previous last
				lastRefL[1] = (lastRefL[0] + inputSampleL)/2; //half
				lastRefL[2] = inputSampleL; //full
				lastRefR[0] = lastRefR[2]; //start from previous last
				lastRefR[1] = (lastRefR[0] + inputSampleR)/2; //half
				lastRefR[2] = inputSampleR; //full
			}
			if (cycleEnd == 1) {
				lastRefL[0] = inputSampleL;
				lastRefR[0] = inputSampleR;
			}
			cycle = 0; //reset
			inputSampleL = lastRefL[cycle];
			inputSampleR = lastRefR[cycle];
		} else {
			inputSampleL = lastRefL[cycle];
			inputSampleR = lastRefR[cycle];
			//we are going through our references now
		}
		
		for (int x = 0; x < 1; x += 4) {
			float slew = ((inputSampleL - dram->pearF[x]) + dram->pearF[x+1])*pearScaled*0.5f;
			dram->pearF[x] = inputSampleL = (pearScaled * inputSampleL) + ((1.0f-pearScaled) * (dram->pearF[x] + dram->pearF[x+1]));
			dram->pearF[x+1] = slew;
			slew = ((inputSampleR - dram->pearF[x+2]) + dram->pearF[x+3])*pearScaled*0.5f;
			dram->pearF[x+2] = inputSampleR = (pearScaled * inputSampleR) + ((1.0f-pearScaled) * (dram->pearF[x+2] + dram->pearF[x+3]));
			dram->pearF[x+3] = slew;
		}
				
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
	gainOutL = gainOutR = 1.0;
	
	for(int count = 0; count < shortA+2; count++) {dram->eAL[count] = 0.0; dram->eAR[count] = 0.0;}
	for(int count = 0; count < shortB+2; count++) {dram->eBL[count] = 0.0; dram->eBR[count] = 0.0;}
	for(int count = 0; count < shortC+2; count++) {dram->eCL[count] = 0.0; dram->eCR[count] = 0.0;}
	for(int count = 0; count < shortD+2; count++) {dram->eDL[count] = 0.0; dram->eDR[count] = 0.0;}
	for(int count = 0; count < shortE+2; count++) {dram->eEL[count] = 0.0; dram->eER[count] = 0.0;}
	for(int count = 0; count < shortF+2; count++) {dram->eFL[count] = 0.0; dram->eFR[count] = 0.0;}
	for(int count = 0; count < shortG+2; count++) {dram->eGL[count] = 0.0; dram->eGR[count] = 0.0;}
	for(int count = 0; count < shortH+2; count++) {dram->eHL[count] = 0.0; dram->eHR[count] = 0.0;}
	for(int count = 0; count < shortI+2; count++) {dram->eIL[count] = 0.0; dram->eIR[count] = 0.0;}
	for(int count = 0; count < shortJ+2; count++) {dram->eJL[count] = 0.0; dram->eJR[count] = 0.0;}
	for(int count = 0; count < shortK+2; count++) {dram->eKL[count] = 0.0; dram->eKR[count] = 0.0;}
	for(int count = 0; count < shortL+2; count++) {dram->eLL[count] = 0.0; dram->eLR[count] = 0.0;}
	for(int count = 0; count < shortM+2; count++) {dram->eML[count] = 0.0; dram->eMR[count] = 0.0;}
	for(int count = 0; count < shortN+2; count++) {dram->eNL[count] = 0.0; dram->eNR[count] = 0.0;}
	for(int count = 0; count < shortO+2; count++) {dram->eOL[count] = 0.0; dram->eOR[count] = 0.0;}
	for(int count = 0; count < shortP+2; count++) {dram->ePL[count] = 0.0; dram->ePR[count] = 0.0;}		
	
	
	shortAL = 1;
	shortBL = 1;
	shortCL = 1;
	shortDL = 1;	
	shortEL = 1;
	shortFL = 1;
	shortGL = 1;
	shortHL = 1;
	shortIL = 1;
	shortJL = 1;
	shortKL = 1;
	shortLL = 1;
	shortML = 1;
	shortNL = 1;
	shortOL = 1;
	shortPL = 1;
	
	shortAR = 1;
	shortBR = 1;
	shortCR = 1;
	shortDR = 1;	
	shortER = 1;
	shortFR = 1;
	shortGR = 1;
	shortHR = 1;
	shortIR = 1;
	shortJR = 1;
	shortKR = 1;
	shortLR = 1;
	shortMR = 1;
	shortNR = 1;
	shortOR = 1;
	shortPR = 1;
	
	
	for(int count = 0; count < delayA+2; count++) {dram->aAL[count] = 0.0; dram->aAR[count] = 0.0;}
	for(int count = 0; count < delayB+2; count++) {dram->aBL[count] = 0.0; dram->aBR[count] = 0.0;}
	for(int count = 0; count < delayC+2; count++) {dram->aCL[count] = 0.0; dram->aCR[count] = 0.0;}
	for(int count = 0; count < delayD+2; count++) {dram->aDL[count] = 0.0; dram->aDR[count] = 0.0;}
	for(int count = 0; count < delayE+2; count++) {dram->aEL[count] = 0.0; dram->aER[count] = 0.0;}
	for(int count = 0; count < delayF+2; count++) {dram->aFL[count] = 0.0; dram->aFR[count] = 0.0;}
	for(int count = 0; count < delayG+2; count++) {dram->aGL[count] = 0.0; dram->aGR[count] = 0.0;}
	for(int count = 0; count < delayH+2; count++) {dram->aHL[count] = 0.0; dram->aHR[count] = 0.0;}
	for(int count = 0; count < delayI+2; count++) {dram->aIL[count] = 0.0; dram->aIR[count] = 0.0;}
	for(int count = 0; count < delayJ+2; count++) {dram->aJL[count] = 0.0; dram->aJR[count] = 0.0;}
	for(int count = 0; count < delayK+2; count++) {dram->aKL[count] = 0.0; dram->aKR[count] = 0.0;}
	for(int count = 0; count < delayL+2; count++) {dram->aLL[count] = 0.0; dram->aLR[count] = 0.0;}
	for(int count = 0; count < delayM+2; count++) {dram->aML[count] = 0.0; dram->aMR[count] = 0.0;}
	for(int count = 0; count < delayN+2; count++) {dram->aNL[count] = 0.0; dram->aNR[count] = 0.0;}
	for(int count = 0; count < delayO+2; count++) {dram->aOL[count] = 0.0; dram->aOR[count] = 0.0;}
	for(int count = 0; count < delayP+2; count++) {dram->aPL[count] = 0.0; dram->aPR[count] = 0.0;}
	for(int count = 0; count < delayQ+2; count++) {dram->aQL[count] = 0.0; dram->aQR[count] = 0.0;}
	for(int count = 0; count < delayR+2; count++) {dram->aRL[count] = 0.0; dram->aRR[count] = 0.0;}
	for(int count = 0; count < delayS+2; count++) {dram->aSL[count] = 0.0; dram->aSR[count] = 0.0;}
	for(int count = 0; count < delayT+2; count++) {dram->aTL[count] = 0.0; dram->aTR[count] = 0.0;}
	for(int count = 0; count < delayU+2; count++) {dram->aUL[count] = 0.0; dram->aUR[count] = 0.0;}
	for(int count = 0; count < delayV+2; count++) {dram->aVL[count] = 0.0; dram->aVR[count] = 0.0;}
	for(int count = 0; count < delayW+2; count++) {dram->aWL[count] = 0.0; dram->aWR[count] = 0.0;}
	for(int count = 0; count < delayX+2; count++) {dram->aXL[count] = 0.0; dram->aXR[count] = 0.0;}
	for(int count = 0; count < delayY+2; count++) {dram->aYL[count] = 0.0; dram->aYR[count] = 0.0;}
	
	for(int count = 0; count < predelay+2; count++) {dram->aZL[count] = 0.0; dram->aZR[count] = 0.0;}
	for(int count = 0; count < vlfpredelay+2; count++) {dram->aVLFL[count] = 0.0; dram->aVLFR[count] = 0.0;}
	
	feedbackAL = 0.0;
	feedbackBL = 0.0;
	feedbackCL = 0.0;
	feedbackDL = 0.0;
	feedbackEL = 0.0;
	
	feedbackER = 0.0;
	feedbackJR = 0.0;
	feedbackOR = 0.0;
	feedbackTR = 0.0;
	feedbackYR = 0.0;
		
	for(int count = 0; count < 6; count++) {lastRefL[count] = 0.0; lastRefR[count] = 0.0;}
		
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
	countQL = 1;
	countRL = 1;
	countSL = 1;
	countTL = 1;
	countUL = 1;
	countVL = 1;
	countWL = 1;
	countXL = 1;
	countYL = 1;
	
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
	countQR = 1;
	countRR = 1;
	countSR = 1;
	countTR = 1;
	countUR = 1;
	countVR = 1;
	countWR = 1;
	countXR = 1;
	countYR = 1;
	
	countZ = 1;
	countVLF = 1;
	
	cycle = 0;
	
	for (int x = 0; x < pear_total; x++) {dram->pearA[x] = 0.0; dram->pearB[x] = 0.0; dram->pearC[x] = 0.0; dram->pearD[x] = 0.0; dram->pearE[x] = 0.0; dram->pearF[x] = 0.0;}
	//from PearEQ
	
	vibratoL = vibAL = vibAR = vibBL = vibBR = 0.0;
	vibratoR = M_PI_4;
		
	subAL = subAR = subBL = subBR = subCL = subCR = 0.0;
	sbAL = sbAR = sbBL = sbBR = sbCL = sbCR = 0.0;
	//from SubTight
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
