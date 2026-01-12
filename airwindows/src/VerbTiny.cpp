#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "VerbTiny"
#define AIRWINDOWS_DESCRIPTION "A classic artificial reverb that expands reverb shape."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','V','e','s' )
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
const int d4A = 136; const int d4B = 52; const int d4C = 53; const int d4D = 1261; 
const int d4E = 209; const int d4F = 473; const int d4G = 549; const int d4H = 29; 
const int d4I = 92; const int d4J = 1137; const int d4K = 1406; const int d4L = 994; 
const int d4M = 1314; const int d4N = 191; const int d4O = 1263; const int d4P = 103; 
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
{ .name = "Replace", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Derez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Filter", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wider", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	int c4AL,c4BL,c4CL,c4DL,c4EL,c4FL,c4GL,c4HL;
	int c4IL,c4JL,c4KL,c4LL,c4ML,c4NL,c4OL,c4PL;		
	int c4AR,c4BR,c4CR,c4DR,c4ER,c4FR,c4GR,c4HR;
	int c4IR,c4JR,c4KR,c4LR,c4MR,c4NR,c4OR,c4PR;
	float f4AL,f4BL,f4CL,f4DL,f4DR,f4HR,f4LR,f4PR;
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
	
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float a4AL[d4A+5];
	float a4BL[d4B+5];
	float a4CL[d4C+5];
	float a4DL[d4D+5];
	float a4EL[d4E+5];
	float a4FL[d4F+5];
	float a4GL[d4G+5];
	float a4HL[d4H+5];
	float a4IL[d4I+5];
	float a4JL[d4J+5];
	float a4KL[d4K+5];
	float a4LL[d4L+5];
	float a4ML[d4M+5];
	float a4NL[d4N+5];
	float a4OL[d4O+5];
	float a4PL[d4P+5];
	float a4AR[d4A+5];
	float a4BR[d4B+5];
	float a4CR[d4C+5];
	float a4DR[d4D+5];
	float a4ER[d4E+5];
	float a4FR[d4F+5];
	float a4GR[d4G+5];
	float a4HR[d4H+5];
	float a4IR[d4I+5];
	float a4JR[d4J+5];
	float a4KR[d4K+5];
	float a4LR[d4L+5];
	float a4MR[d4M+5];
	float a4NR[d4N+5];
	float a4OR[d4O+5];
	float a4PR[d4P+5];
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
	float bezF[bez_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	float reg4n = 0.03125f+((1.0f-powf(1.0f-GetParameter( kParam_A ),2.0f))*0.03125f);
	float attenuate = 1.0f - (1.0f-powf(1.0f-GetParameter( kParam_A ),2.0f));
	float derez = powf(GetParameter( kParam_B ),2.0f);
	derez = fmin(fmax(derez/overallscale,0.0001f),1.0f);
	int bezFraction = (int)(1.0f/derez);
	float bezTrim = (float)bezFraction/(bezFraction+1.0f);
	derez = 1.0f / bezFraction;
	bezTrim = 1.0f-(derez*bezTrim);
	//the revision more accurately connects the bezier curves
	float derezFreq = powf(GetParameter( kParam_C ),2.0f);
	derezFreq = fmin(fmax(derezFreq/overallscale,0.0001f),1.0f);
	int bezFreqFraction = (int)(1.0f/derezFreq);
	float bezFreqTrim = (float)bezFreqFraction/(bezFreqFraction+1.0f);
	derezFreq = 1.0f / bezFreqFraction;
	bezFreqTrim = 1.0f-(derezFreq*bezFreqTrim);
	//the revision more accurately connects the bezier curves
	float wider = GetParameter( kParam_D )*2.0f;
	float wet = GetParameter( kParam_E );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		dram->bez[bez_cycle] += derez;
		dram->bez[bez_SampL] += (inputSampleL*attenuate*derez);
		dram->bez[bez_SampR] += (inputSampleR*attenuate*derez);
		if (dram->bez[bez_cycle] > 1.0f) { //hit the end point and we do a reverb sample
			dram->bez[bez_cycle] = 0.0f;
			float mainSampleL = dram->bez[bez_SampL];
			float dualmonoSampleL =  dram->bez[bez_SampR];
			//workaround involves keeping the cross-matrix system,
			//but for initial layering, each side gets each version
			//making blends never quite line up as exactly the same.
			
			//left verbs
			dram->a4AL[c4AL] = mainSampleL + (f4DR * reg4n);
			dram->a4BL[c4BL] = mainSampleL + (f4HR * reg4n);
			dram->a4CL[c4CL] = mainSampleL + (f4LR * reg4n);
			dram->a4DL[c4DL] = mainSampleL + (f4PR * reg4n);
			dram->b4AL[c4AL] = dualmonoSampleL + (g4AL * reg4n);
			dram->b4BL[c4BL] = dualmonoSampleL + (g4BL * reg4n);
			dram->b4CL[c4CL] = dualmonoSampleL + (g4CL * reg4n);
			dram->b4DL[c4DL] = dualmonoSampleL + (g4DL * reg4n);
			
			c4AL++; if (c4AL < 0 || c4AL > d4A) c4AL = 0;
			c4BL++; if (c4BL < 0 || c4BL > d4B) c4BL = 0;
			c4CL++; if (c4CL < 0 || c4CL > d4C) c4CL = 0;
			c4DL++; if (c4DL < 0 || c4DL > d4D) c4DL = 0;
			
			float hA = dram->a4AL[c4AL-((c4AL > d4A)?d4A+1:0)];
			float hB = dram->a4BL[c4BL-((c4BL > d4B)?d4B+1:0)];
			float hC = dram->a4CL[c4CL-((c4CL > d4C)?d4C+1:0)];
			float hD = dram->a4DL[c4DL-((c4DL > d4D)?d4D+1:0)];
			dram->a4EL[c4EL] = hA - (hB + hC + hD);
			dram->a4FL[c4FL] = hB - (hA + hC + hD);
			dram->a4GL[c4GL] = hC - (hA + hB + hD);
			dram->a4HL[c4HL] = hD - (hA + hB + hC);
			hA = dram->b4AL[c4AL-((c4AL > d4A)?d4A+1:0)];
			hB = dram->b4BL[c4BL-((c4BL > d4B)?d4B+1:0)];
			hC = dram->b4CL[c4CL-((c4CL > d4C)?d4C+1:0)];
			hD = dram->b4DL[c4DL-((c4DL > d4D)?d4D+1:0)];
			dram->b4EL[c4EL] = hA - (hB + hC + hD);
			dram->b4FL[c4FL] = hB - (hA + hC + hD);
			dram->b4GL[c4GL] = hC - (hA + hB + hD);
			dram->b4HL[c4HL] = hD - (hA + hB + hC);
			
			c4EL++; if (c4EL < 0 || c4EL > d4E) c4EL = 0;
			c4FL++; if (c4FL < 0 || c4FL > d4F) c4FL = 0;
			c4GL++; if (c4GL < 0 || c4GL > d4G) c4GL = 0;
			c4HL++; if (c4HL < 0 || c4HL > d4H) c4HL = 0;
			
			hA = dram->a4EL[c4EL-((c4EL > d4E)?d4E+1:0)];
			hB = dram->a4FL[c4FL-((c4FL > d4F)?d4F+1:0)];
			hC = dram->a4GL[c4GL-((c4GL > d4G)?d4G+1:0)];
			hD = dram->a4HL[c4HL-((c4HL > d4H)?d4H+1:0)];
			dram->a4IL[c4IL] = hA - (hB + hC + hD);
			dram->a4JL[c4JL] = hB - (hA + hC + hD);
			dram->a4KL[c4KL] = hC - (hA + hB + hD);
			dram->a4LL[c4LL] = hD - (hA + hB + hC);
			hA = dram->b4EL[c4EL-((c4EL > d4E)?d4E+1:0)];
			hB = dram->b4FL[c4FL-((c4FL > d4F)?d4F+1:0)];
			hC = dram->b4GL[c4GL-((c4GL > d4G)?d4G+1:0)];
			hD = dram->b4HL[c4HL-((c4HL > d4H)?d4H+1:0)];
			dram->b4IL[c4IL] = hA - (hB + hC + hD);
			dram->b4JL[c4JL] = hB - (hA + hC + hD);
			dram->b4KL[c4KL] = hC - (hA + hB + hD);
			dram->b4LL[c4LL] = hD - (hA + hB + hC);
			
			c4IL++; if (c4IL < 0 || c4IL > d4I) c4IL = 0;
			c4JL++; if (c4JL < 0 || c4JL > d4J) c4JL = 0;
			c4KL++; if (c4KL < 0 || c4KL > d4K) c4KL = 0;
			c4LL++; if (c4LL < 0 || c4LL > d4L) c4LL = 0;
			
			hA = dram->a4IL[c4IL-((c4IL > d4I)?d4I+1:0)];
			hB = dram->a4JL[c4JL-((c4JL > d4J)?d4J+1:0)];
			hC = dram->a4KL[c4KL-((c4KL > d4K)?d4K+1:0)];
			hD = dram->a4LL[c4LL-((c4LL > d4L)?d4L+1:0)];
			dram->a4ML[c4ML] = hA - (hB + hC + hD);
			dram->a4NL[c4NL] = hB - (hA + hC + hD);
			dram->a4OL[c4OL] = hC - (hA + hB + hD);
			dram->a4PL[c4PL] = hD - (hA + hB + hC);
			hA = dram->b4IL[c4IL-((c4IL > d4I)?d4I+1:0)];
			hB = dram->b4JL[c4JL-((c4JL > d4J)?d4J+1:0)];
			hC = dram->b4KL[c4KL-((c4KL > d4K)?d4K+1:0)];
			hD = dram->b4LL[c4LL-((c4LL > d4L)?d4L+1:0)];
			dram->b4ML[c4ML] = hA - (hB + hC + hD);
			dram->b4NL[c4NL] = hB - (hA + hC + hD);
			dram->b4OL[c4OL] = hC - (hA + hB + hD);
			dram->b4PL[c4PL] = hD - (hA + hB + hC);
			
			c4ML++; if (c4ML < 0 || c4ML > d4M) c4ML = 0;
			c4NL++; if (c4NL < 0 || c4NL > d4N) c4NL = 0;
			c4OL++; if (c4OL < 0 || c4OL > d4O) c4OL = 0;
			c4PL++; if (c4PL < 0 || c4PL > d4P) c4PL = 0;
			
			hA = dram->a4ML[c4ML-((c4ML > d4M)?d4M+1:0)];
			hB = dram->a4NL[c4NL-((c4NL > d4N)?d4N+1:0)];
			hC = dram->a4OL[c4OL-((c4OL > d4O)?d4O+1:0)];
			hD = dram->a4PL[c4PL-((c4PL > d4P)?d4P+1:0)];
			f4AL = hA - (hB + hC + hD);				
			f4BL = hB - (hA + hC + hD);
			f4CL = hC - (hA + hB + hD);
			f4DL = hD - (hA + hB + hC);//not actually crosschannel yet
			mainSampleL = (hA + hB + hC + hD)*0.125f;
			
			hA = dram->b4ML[c4ML-((c4ML > d4M)?d4M+1:0)];
			hB = dram->b4NL[c4NL-((c4NL > d4N)?d4N+1:0)];
			hC = dram->b4OL[c4OL-((c4OL > d4O)?d4O+1:0)];
			hD = dram->b4PL[c4PL-((c4PL > d4P)?d4P+1:0)];
			g4AL = hA - (hB + hC + hD);
			g4BL = hB - (hA + hC + hD);
			g4CL = hC - (hA + hB + hD);
			g4DL = hD - (hA + hB + hC);			
			dualmonoSampleL = (hA + hB + hC + hD)*0.125f;
			
			float mainSampleR = dram->bez[bez_SampR]; //begin primary reverb
			float dualmonoSampleR =  dram->bez[bez_SampL];
			
			//right verbs
			dram->a4DR[c4DR] = mainSampleR + (f4AL * reg4n);
			dram->a4HR[c4HR] = mainSampleR + (f4BL * reg4n);
			dram->a4LR[c4LR] = mainSampleR + (f4CL * reg4n);
			dram->a4PR[c4PR] = mainSampleR + (f4DL * reg4n);
			dram->b4DR[c4DR] = dualmonoSampleR + (g4DR * reg4n);
			dram->b4HR[c4HR] = dualmonoSampleR + (g4HR * reg4n);
			dram->b4LR[c4LR] = dualmonoSampleR + (g4LR * reg4n);
			dram->b4PR[c4PR] = dualmonoSampleR + (g4PR * reg4n);
			
			c4DR++; if (c4DR < 0 || c4DR > d4D) c4DR = 0;
			c4HR++; if (c4HR < 0 || c4HR > d4H) c4HR = 0;
			c4LR++; if (c4LR < 0 || c4LR > d4L) c4LR = 0;
			c4PR++; if (c4PR < 0 || c4PR > d4P) c4PR = 0;
			
			hA = dram->a4DR[c4DR-((c4DR > d4D)?d4D+1:0)];
			hB = dram->a4HR[c4HR-((c4HR > d4H)?d4H+1:0)];
			hC = dram->a4LR[c4LR-((c4LR > d4L)?d4L+1:0)];
			hD = dram->a4PR[c4PR-((c4PR > d4P)?d4P+1:0)];
			dram->a4CR[c4CR] = hA - (hB + hC + hD);
			dram->a4GR[c4GR] = hB - (hA + hC + hD);
			dram->a4KR[c4KR] = hC - (hA + hB + hD);
			dram->a4OR[c4OR] = hD - (hA + hB + hC);
			hA = dram->b4DR[c4DR-((c4DR > d4D)?d4D+1:0)];
			hB = dram->b4HR[c4HR-((c4HR > d4H)?d4H+1:0)];
			hC = dram->b4LR[c4LR-((c4LR > d4L)?d4L+1:0)];
			hD = dram->b4PR[c4PR-((c4PR > d4P)?d4P+1:0)];
			dram->b4CR[c4CR] = hA - (hB + hC + hD);
			dram->b4GR[c4GR] = hB - (hA + hC + hD);
			dram->b4KR[c4KR] = hC - (hA + hB + hD);
			dram->b4OR[c4OR] = hD - (hA + hB + hC);
			
			c4CR++; if (c4CR < 0 || c4CR > d4C) c4CR = 0;
			c4GR++; if (c4GR < 0 || c4GR > d4G) c4GR = 0;
			c4KR++; if (c4KR < 0 || c4KR > d4K) c4KR = 0;
			c4OR++; if (c4OR < 0 || c4OR > d4O) c4OR = 0;
			
			hA = dram->a4CR[c4CR-((c4CR > d4C)?d4C+1:0)];
			hB = dram->a4GR[c4GR-((c4GR > d4G)?d4G+1:0)];
			hC = dram->a4KR[c4KR-((c4KR > d4K)?d4K+1:0)];
			hD = dram->a4OR[c4OR-((c4OR > d4O)?d4O+1:0)];
			dram->a4BR[c4BR] = hA - (hB + hC + hD);
			dram->a4FR[c4FR] = hB - (hA + hC + hD);
			dram->a4JR[c4JR] = hC - (hA + hB + hD);
			dram->a4NR[c4NR] = hD - (hA + hB + hC);
			hA = dram->b4CR[c4CR-((c4CR > d4C)?d4C+1:0)];
			hB = dram->b4GR[c4GR-((c4GR > d4G)?d4G+1:0)];
			hC = dram->b4KR[c4KR-((c4KR > d4K)?d4K+1:0)];
			hD = dram->b4OR[c4OR-((c4OR > d4O)?d4O+1:0)];
			dram->b4BR[c4BR] = hA - (hB + hC + hD);
			dram->b4FR[c4FR] = hB - (hA + hC + hD);
			dram->b4JR[c4JR] = hC - (hA + hB + hD);
			dram->b4NR[c4NR] = hD - (hA + hB + hC);
			
			c4BR++; if (c4BR < 0 || c4BR > d4B) c4BR = 0;
			c4FR++; if (c4FR < 0 || c4FR > d4F) c4FR = 0;
			c4JR++; if (c4JR < 0 || c4JR > d4J) c4JR = 0;
			c4NR++; if (c4NR < 0 || c4NR > d4N) c4NR = 0;
			
			hA = dram->a4BR[c4BR-((c4BR > d4B)?d4B+1:0)];
			hB = dram->a4FR[c4FR-((c4FR > d4F)?d4F+1:0)];
			hC = dram->a4JR[c4JR-((c4JR > d4J)?d4J+1:0)];
			hD = dram->a4NR[c4NR-((c4NR > d4N)?d4N+1:0)];
			dram->a4AR[c4AR] = hA - (hB + hC + hD);
			dram->a4ER[c4ER] = hB - (hA + hC + hD);
			dram->a4IR[c4IR] = hC - (hA + hB + hD);
			dram->a4MR[c4MR] = hD - (hA + hB + hC);
			hA = dram->b4BR[c4BR-((c4BR > d4B)?d4B+1:0)];
			hB = dram->b4FR[c4FR-((c4FR > d4F)?d4F+1:0)];
			hC = dram->b4JR[c4JR-((c4JR > d4J)?d4J+1:0)];
			hD = dram->b4NR[c4NR-((c4NR > d4N)?d4N+1:0)];
			dram->b4AR[c4AR] = hA - (hB + hC + hD);
			dram->b4ER[c4ER] = hB - (hA + hC + hD);
			dram->b4IR[c4IR] = hC - (hA + hB + hD);
			dram->b4MR[c4MR] = hD - (hA + hB + hC);
			
			c4AR++; if (c4AR < 0 || c4AR > d4A) c4AR = 0;
			c4ER++; if (c4ER < 0 || c4ER > d4E) c4ER = 0;
			c4IR++; if (c4IR < 0 || c4IR > d4I) c4IR = 0;
			c4MR++; if (c4MR < 0 || c4MR > d4M) c4MR = 0;
			
			hA = dram->a4AR[c4AR-((c4AR > d4A)?d4A+1:0)];
			hB = dram->a4ER[c4ER-((c4ER > d4E)?d4E+1:0)];
			hC = dram->a4IR[c4IR-((c4IR > d4I)?d4I+1:0)];
			hD = dram->a4MR[c4MR-((c4MR > d4M)?d4M+1:0)];
			f4DR = hA - (hB + hC + hD);
			f4HR = hB - (hA + hC + hD);
			f4LR = hC - (hA + hB + hD);
			f4PR = hD - (hA + hB + hC);
			mainSampleR = (hA + hB + hC + hD)*0.125f;
			
			hA = dram->b4AR[c4AR-((c4AR > d4A)?d4A+1:0)];
			hB = dram->b4ER[c4ER-((c4ER > d4E)?d4E+1:0)];
			hC = dram->b4IR[c4IR-((c4IR > d4I)?d4I+1:0)];
			hD = dram->b4MR[c4MR-((c4MR > d4M)?d4M+1:0)];
			g4DR = hA - (hB + hC + hD);				
			g4HR = hB - (hA + hC + hD);
			g4LR = hC - (hA + hB + hD);
			g4PR = hD - (hA + hB + hC);
			dualmonoSampleR = (hA + hB + hC + hD)*0.125f;
			//dual mono version is wider = 1.0f at the center
			//with mainsample 0.0f and 2.0f (only at the edges)
			//with mainsample out of phase when over 1.0f
			//couldn't re-do the arrays perfectly, so instead
			//we keep exactly the same cross-matrix,
			//but we flip the sides we're using for initial reverb.
			//then, dualmono remains totally dualmono, and blend a bit in for wideness.
			
			if (wider < 1.0f) {
				inputSampleL = (dualmonoSampleR*wider) + (mainSampleL*(1.0f-wider));
				inputSampleR = (dualmonoSampleL*wider) + (mainSampleR*(1.0f-wider));
			} else {
				inputSampleL = (dualmonoSampleR*(2.0f-wider)) + (mainSampleL*(wider-1.0f));
				inputSampleR = (dualmonoSampleL*(2.0f-wider)) + (-mainSampleR*(wider-1.0f));
			}
			
			dram->bez[bez_CL] = dram->bez[bez_BL];
			dram->bez[bez_BL] = dram->bez[bez_AL];
			dram->bez[bez_AL] = inputSampleL;
			dram->bez[bez_SampL] = 0.0f;
			
			dram->bez[bez_CR] = dram->bez[bez_BR];
			dram->bez[bez_BR] = dram->bez[bez_AR];
			dram->bez[bez_AR] = inputSampleR;
			dram->bez[bez_SampR] = 0.0f;
		}
		float X = dram->bez[bez_cycle]*bezTrim;
		float CBL = (dram->bez[bez_CL]*(1.0f-X))+(dram->bez[bez_BL]*X);
		float CBR = (dram->bez[bez_CR]*(1.0f-X))+(dram->bez[bez_BR]*X);
		float BAL = (dram->bez[bez_BL]*(1.0f-X))+(dram->bez[bez_AL]*X);
		float BAR = (dram->bez[bez_BR]*(1.0f-X))+(dram->bez[bez_AR]*X);
		inputSampleL = (dram->bez[bez_BL]+(CBL*(1.0f-X))+(BAL*X))*-0.25f;
		inputSampleR = (dram->bez[bez_BR]+(CBR*(1.0f-X))+(BAR*X))*-0.25f;
		
		dram->bezF[bez_cycle] += derezFreq;
		dram->bezF[bez_SampL] += (inputSampleL * derezFreq);
		dram->bezF[bez_SampR] += (inputSampleR * derezFreq);
		if (dram->bezF[bez_cycle] > 1.0f) { //hit the end point and we do a filter sample
			dram->bezF[bez_cycle] = 0.0f;
			dram->bezF[bez_CL] = dram->bezF[bez_BL];
			dram->bezF[bez_BL] = dram->bezF[bez_AL];
			dram->bezF[bez_AL] = dram->bezF[bez_SampL];
			dram->bezF[bez_SampL] = 0.0f;
			dram->bezF[bez_CR] = dram->bezF[bez_BR];
			dram->bezF[bez_BR] = dram->bezF[bez_AR];
			dram->bezF[bez_AR] = dram->bezF[bez_SampR];
			dram->bezF[bez_SampR] = 0.0f;
		}
		X = dram->bezF[bez_cycle]*bezFreqTrim;
		float CBLfreq = (dram->bezF[bez_CL]*(1.0f-X))+(dram->bezF[bez_BL]*X);
		float BALfreq = (dram->bezF[bez_BL]*(1.0f-X))+(dram->bezF[bez_AL]*X);
		inputSampleL = (dram->bezF[bez_BL]+(CBLfreq*(1.0f-X))+(BALfreq*X))*0.5f;
		float CBRfreq = (dram->bezF[bez_CR]*(1.0f-X))+(dram->bezF[bez_BR]*X);
		float BARfreq = (dram->bezF[bez_BR]*(1.0f-X))+(dram->bezF[bez_AR]*X);
		inputSampleR = (dram->bezF[bez_BR]+(CBRfreq*(1.0f-X))+(BARfreq*X))*0.5f;
		//filtering the reverb separately, after making it
		
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
	for(int x = 0; x < d4A+2; x++) {dram->a4AL[x] = 0.0; dram->a4AR[x] = 0.0;}
	for(int x = 0; x < d4B+2; x++) {dram->a4BL[x] = 0.0; dram->a4BR[x] = 0.0;}
	for(int x = 0; x < d4C+2; x++) {dram->a4CL[x] = 0.0; dram->a4CR[x] = 0.0;}
	for(int x = 0; x < d4D+2; x++) {dram->a4DL[x] = 0.0; dram->a4DR[x] = 0.0;}
	for(int x = 0; x < d4E+2; x++) {dram->a4EL[x] = 0.0; dram->a4ER[x] = 0.0;}
	for(int x = 0; x < d4F+2; x++) {dram->a4FL[x] = 0.0; dram->a4FR[x] = 0.0;}
	for(int x = 0; x < d4G+2; x++) {dram->a4GL[x] = 0.0; dram->a4GR[x] = 0.0;}
	for(int x = 0; x < d4H+2; x++) {dram->a4HL[x] = 0.0; dram->a4HR[x] = 0.0;}
	for(int x = 0; x < d4I+2; x++) {dram->a4IL[x] = 0.0; dram->a4IR[x] = 0.0;}
	for(int x = 0; x < d4J+2; x++) {dram->a4JL[x] = 0.0; dram->a4JR[x] = 0.0;}
	for(int x = 0; x < d4K+2; x++) {dram->a4KL[x] = 0.0; dram->a4KR[x] = 0.0;}
	for(int x = 0; x < d4L+2; x++) {dram->a4LL[x] = 0.0; dram->a4LR[x] = 0.0;}
	for(int x = 0; x < d4M+2; x++) {dram->a4ML[x] = 0.0; dram->a4MR[x] = 0.0;}
	for(int x = 0; x < d4N+2; x++) {dram->a4NL[x] = 0.0; dram->a4NR[x] = 0.0;}
	for(int x = 0; x < d4O+2; x++) {dram->a4OL[x] = 0.0; dram->a4OR[x] = 0.0;}
	for(int x = 0; x < d4P+2; x++) {dram->a4PL[x] = 0.0; dram->a4PR[x] = 0.0;}
	c4AL = c4BL = c4CL = c4DL = c4EL = c4FL = c4GL = c4HL = 1;
	c4IL = c4JL = c4KL = c4LL = c4ML = c4NL = c4OL = c4PL = 1;
	c4AR = c4BR = c4CR = c4DR = c4ER = c4FR = c4GR = c4HR = 1;
	c4IR = c4JR = c4KR = c4LR = c4MR = c4NR = c4OR = c4PR = 1;
	f4AL = f4BL = f4CL = f4DL = 0.0;
	f4DR = f4HR = f4LR = f4PR = 0.0;
	
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
	
	for (int x = 0; x < bez_total; x++) {
		dram->bez[x] = 0.0;
		dram->bezF[x] = 0.0;
	}
	dram->bez[bez_cycle] = 1.0;
	dram->bezF[bez_cycle] = 1.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
