#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "TakeCare"
#define AIRWINDOWS_DESCRIPTION "A lush chorus ensemble universe."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','a','k' )
#define AIRWINDOWS_TAGS kNT_tagEffect
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	kParam_H =7,
	//Add your parameters here...
	kNumberOfParameters=8
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 150, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Rando", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Depth", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Regen", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Derez", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Buffer", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	int c3AL,c3AR,c3BL,c3BR,c3CL,c3CR,c3DL,c3DR,c3EL,c3ER;
	int c3FL,c3FR,c3GL,c3GR,c3HL,c3HR,c3IL,c3IR;
	float f3AL,f3BL,f3CL,f3CR,f3FR,f3IR;	
	float avg3L,avg3R;
	
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
		bez_AvgInSampL,
		bez_AvgInSampR,
		bez_AvgOutSampL,
		bez_AvgOutSampR,
		bez_cycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	
	float rotate;
	float oldfpd;
	
	int buf;
	float vibDepth;
	float derezA;
	float derezB;
	float outA;
	float outB;
	float wetA;
	float wetB;
	
	float lastSampleL;
	bool wasPosClipL;
	bool wasNegClipL;
	float lastSampleR;
	bool wasPosClipR;
	bool wasNegClipR; //Stereo ClipOnly
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float a3AL[32767+5];
	float a3BL[32767+5];
	float a3CL[32767+5];
	float a3DL[32767+5];
	float a3EL[32767+5];
	float a3FL[32767+5];
	float a3GL[32767+5];
	float a3HL[32767+5];
	float a3IL[32767+5];
	float a3AR[32767+5];
	float a3BR[32767+5];
	float a3CR[32767+5];
	float a3DR[32767+5];
	float a3ER[32767+5];
	float a3FR[32767+5];
	float a3GR[32767+5];
	float a3HR[32767+5];
	float a3IR[32767+5];
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
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;
	float vibSpeed = powf(GetParameter( kParam_A ),5.0f) * 0.1f;
	float vibRandom = powf(GetParameter( kParam_B ),3.0f) * 0.99f;
	float targetDepth = powf(GetParameter( kParam_C ),2.0f) * 0.5f;
	float reg3n = GetParameter( kParam_D )*0.0625f;
	
	derezA = derezB; derezB = GetParameter( kParam_E )*2.0f;
	bool stepped = true; // Revised Bezier Undersampling
	if (derezB > 1.0f) {  // has full rez at center, stepped
		stepped = false; // to left, continuous to right
		derezB = 1.0f-(derezB-1.0f);
	} //if it's set up like that it's the revised algorithm
	derezB = fmin(fmax(derezB/overallscale,0.0005f),1.0f);
	int bezFraction = (int)(1.0f/derezB);
	float bezTrim = (float)bezFraction/(bezFraction+1.0f);
	if (stepped) { //this hard-locks derez to exact subdivisions of 1.0f
		derezB = 1.0f / bezFraction;
		bezTrim = 1.0f-(derezB*bezTrim);
	} else { //this makes it match the 1.0f case using stepped
		derezB /= (2.0f/powf(overallscale,0.5f-((overallscale-1.0f)*0.0375f)));
		bezTrim = 1.0f-powf(derezB*0.5f,1.0f/(derezB*0.5f));
	} //the revision more accurately connects the bezier curves
	
	int targetBuf = (powf(GetParameter( kParam_F ),3.0f)*32510.0f)+256;
	outA = outB; outB = GetParameter( kParam_G );
	wetA = wetB; wetB = 1.0f-powf(1.0f-GetParameter( kParam_H ),2.0f);
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		float temp = (float)nSampleFrames/inFramesToProcess;
		float derez = (derezA*temp)+(derezB*(1.0f-temp));
		float out = (outA*temp)+(outB*(1.0f-temp));
		float wet = (wetA*temp)+(wetB*(1.0f-temp));
				
		dram->bez[bez_cycle] += derez;
		dram->bez[bez_SampL] += ((inputSampleL+dram->bez[bez_InL]) * derez);
		dram->bez[bez_SampR] += ((inputSampleR+dram->bez[bez_InR]) * derez);
		dram->bez[bez_InL] = inputSampleL; dram->bez[bez_InR] = inputSampleR;
		if (dram->bez[bez_cycle] > 1.0f) { //hit the end point and we do a reverb sample
			if (stepped) dram->bez[bez_cycle] = 0.0f;
			else dram->bez[bez_cycle] -= 1.0f;
			
			inputSampleL = (dram->bez[bez_SampL]+dram->bez[bez_AvgInSampL])*0.5f;
			dram->bez[bez_AvgInSampL] = dram->bez[bez_SampL];
			inputSampleR = (dram->bez[bez_SampR]+dram->bez[bez_AvgInSampR])*0.5f;
			dram->bez[bez_AvgInSampR] = dram->bez[bez_SampR];
			
			rotate += (oldfpd*vibSpeed);
			if (rotate > (M_PI*2.0f)) {
				rotate -= (M_PI*2.0f);
				oldfpd = (1.0f-vibRandom) + ((fpdL*0.000000000618f)*vibRandom);
			}
			vibDepth = (vibDepth*0.99f)+(targetDepth*0.01f);
			if (buf < targetBuf) buf++;
			if (buf > targetBuf) buf--;
			
			float mA = (sin(rotate)+1.0f)*vibDepth*buf;
			float mB = (sin(rotate+(M_PI/9.0f))+1.0f)*vibDepth*buf;
			float mC = (sin(rotate+((M_PI/9.0f)*2.0f))+1.0f)*vibDepth*buf;
			float mD = (sin(rotate+((M_PI/9.0f)*3.0f))+1.0f)*vibDepth*buf;
			float mE = (sin(rotate+((M_PI/9.0f)*4.0f))+1.0f)*vibDepth*buf;
			float mF = (sin(rotate+((M_PI/9.0f)*5.0f))+1.0f)*vibDepth*buf;
			float mG = (sin(rotate+((M_PI/9.0f)*6.0f))+1.0f)*vibDepth*buf;
			float mH = (sin(rotate+((M_PI/9.0f)*7.0f))+1.0f)*vibDepth*buf;
			float mI = (sin(rotate+((M_PI/9.0f)*8.0f))+1.0f)*vibDepth*buf;		

			inputSampleL = sin(fmin(fmax(inputSampleL*0.5f,-M_PI_2),M_PI_2));
			inputSampleR = sin(fmin(fmax(inputSampleR*0.5f,-M_PI_2),M_PI_2));
			
			dram->a3AL[c3AL] = inputSampleL + fmin(fmax(f3AL*reg3n,-M_PI),M_PI);
			dram->a3BL[c3BL] = inputSampleL + fmin(fmax(f3BL*reg3n,-M_PI),M_PI);
			dram->a3CL[c3CL] = inputSampleL + fmin(fmax(f3CL*reg3n,-M_PI),M_PI);
			
			dram->a3CR[c3CR] = inputSampleR + fmin(fmax(f3CR*reg3n,-M_PI),M_PI);
			dram->a3FR[c3FR] = inputSampleR + fmin(fmax(f3FR*reg3n,-M_PI),M_PI);
			dram->a3IR[c3IR] = inputSampleR + fmin(fmax(f3IR*reg3n,-M_PI),M_PI);
			
			c3AL++; if (c3AL < 0 || c3AL > buf) c3AL = 0;
			c3BL++; if (c3BL < 0 || c3BL > buf) c3BL = 0;
			c3CL++; if (c3CL < 0 || c3CL > buf) c3CL = 0;
			c3CR++; if (c3CR < 0 || c3CR > buf) c3CR = 0;
			c3FR++; if (c3FR < 0 || c3FR > buf) c3FR = 0;
			c3IR++; if (c3IR < 0 || c3IR > buf) c3IR = 0;
			
			float o3AL = dram->a3AL[(int)(c3AL+mA-((c3AL+mA>buf)?buf+1:0))] *(1.0f-(mA-floor(mA)));
			o3AL += (dram->a3AL[(int)(c3AL+mA+1-((c3AL+mA+1>buf)?buf+1:0))] *((mA-floor(mA))));
			float o3BL = dram->a3BL[(int)(c3BL+mB-((c3BL+mB>buf)?buf+1:0))] *(1.0f-(mB-floor(mB)));
			o3BL += (dram->a3BL[(int)(c3BL+mB+1-((c3BL+mB+1>buf)?buf+1:0))] *((mB-floor(mB))));
			float o3CL = dram->a3CL[(int)(c3CL+mC-((c3CL+mC>buf)?buf+1:0))] *(1.0f-(mC-floor(mC)));
			o3CL += (dram->a3CL[(int)(c3CL+mC+1-((c3CL+mC+1>buf)?buf+1:0))] *((mC-floor(mC))));
			float o3CR = dram->a3CR[(int)(c3CR+mC-((c3CR+mC>buf)?buf+1:0))] *(1.0f-(mC-floor(mC)));
			o3CR += (dram->a3CR[(int)(c3CR+mC+1-((c3CR+mC+1>buf)?buf+1:0))] *((mC-floor(mC))));
			float o3FR = dram->a3FR[(int)(c3FR+mF-((c3FR+mF>buf)?buf+1:0))] *(1.0f-(mF-floor(mF)));
			o3FR += (dram->a3FR[(int)(c3FR+mF+1-((c3FR+mF+1>buf)?buf+1:0))] *((mF-floor(mF))));
			float o3IR = dram->a3IR[(int)(c3IR+mI-((c3IR+mI>buf)?buf+1:0))] *(1.0f-(mI-floor(mI)));
			o3IR += (dram->a3IR[(int)(c3IR+mI+1-((c3IR+mI+1>buf)?buf+1:0))] *((mI-floor(mI))));
			
			dram->a3DL[c3DL] = (((o3BL + o3CL) * -2.0f) + o3AL);
			dram->a3EL[c3EL] = (((o3AL + o3CL) * -2.0f) + o3BL);
			dram->a3FL[c3FL] = (((o3AL + o3BL) * -2.0f) + o3CL);
			dram->a3BR[c3BR] = (((o3FR + o3IR) * -2.0f) + o3CR);
			dram->a3ER[c3ER] = (((o3CR + o3IR) * -2.0f) + o3FR);
			dram->a3HR[c3HR] = (((o3CR + o3FR) * -2.0f) + o3IR);
			
			c3DL++; if (c3DL < 0 || c3DL > buf) c3DL = 0;
			c3EL++; if (c3EL < 0 || c3EL > buf) c3EL = 0;
			c3FL++; if (c3FL < 0 || c3FL > buf) c3FL = 0;
			c3BR++; if (c3BR < 0 || c3BR > buf) c3BR = 0;
			c3ER++; if (c3ER < 0 || c3ER > buf) c3ER = 0;
			c3HR++; if (c3HR < 0 || c3HR > buf) c3HR = 0;
						
			float o3DL = dram->a3DL[(int)(c3DL+mD-((c3DL+mD>buf)?buf+1:0))] *(1.0f-(mD-floor(mD)));
			o3DL += (dram->a3DL[(int)(c3DL+mD+1-((c3DL+mD+1>buf)?buf+1:0))] *((mD-floor(mD))));
			float o3EL = dram->a3EL[(int)(c3EL+mE-((c3EL+mE>buf)?buf+1:0))] *(1.0f-(mE-floor(mE)));
			o3EL += (dram->a3EL[(int)(c3EL+mE+1-((c3EL+mE+1>buf)?buf+1:0))] *((mE-floor(mE))));
			float o3FL = dram->a3FL[(int)(c3FL+mF-((c3FL+mF>buf)?buf+1:0))] *(1.0f-(mF-floor(mF)));
			o3FL += (dram->a3FL[(int)(c3FL+mF+1-((c3FL+mF+1>buf)?buf+1:0))] *((mF-floor(mF))));
			float o3BR = dram->a3BR[(int)(c3BR+mB-((c3BR+mB>buf)?buf+1:0))] *(1.0f-(mB-floor(mB)));
			o3BR += (dram->a3BR[(int)(c3BR+mB+1-((c3BR+mB+1>buf)?buf+1:0))] *((mB-floor(mB))));
			float o3ER = dram->a3ER[(int)(c3ER+mE-((c3ER+mE>buf)?buf+1:0))] *(1.0f-(mE-floor(mE)));
			o3ER += (dram->a3ER[(int)(c3ER+mE+1-((c3ER+mE+1>buf)?buf+1:0))] *((mE-floor(mE))));
			float o3HR = dram->a3HR[(int)(c3HR+mH-((c3HR+mH>buf)?buf+1:0))] *(1.0f-(mH-floor(mH)));
			o3HR += (dram->a3HR[(int)(c3HR+mH+1-((c3HR+mH+1>buf)?buf+1:0))] *((mH-floor(mH))));
			
			dram->a3GL[c3GL] = (((o3EL + o3FL) * -2.0f) + o3DL);
			dram->a3HL[c3HL] = (((o3DL + o3FL) * -2.0f) + o3EL);
			dram->a3IL[c3IL] = (((o3DL + o3EL) * -2.0f) + o3FL);
			dram->a3AR[c3AR] = (((o3ER + o3HR) * -2.0f) + o3BR);
			dram->a3DR[c3DR] = (((o3BR + o3HR) * -2.0f) + o3ER);
			dram->a3GR[c3GR] = (((o3BR + o3ER) * -2.0f) + o3HR);
			
			
			c3GL++; if (c3GL < 0 || c3GL > buf) c3GL = 0;
			c3HL++; if (c3HL < 0 || c3HL > buf) c3HL = 0;
			c3IL++; if (c3IL < 0 || c3IL > buf) c3IL = 0;
			c3AR++; if (c3AR < 0 || c3AR > buf) c3AR = 0;
			c3DR++; if (c3DR < 0 || c3DR > buf) c3DR = 0;
			c3GR++; if (c3GR < 0 || c3GR > buf) c3GR = 0;
			
			float o3GL = dram->a3GL[(int)(c3GL+mG-((c3GL+mG>buf)?buf+1:0))] *(1.0f-(mG-floor(mG)));
			o3GL += (dram->a3GL[(int)(c3GL+mG+1-((c3GL+mG+1>buf)?buf+1:0))] *((mG-floor(mG))));
			float o3HL = dram->a3HL[(int)(c3HL+mH-((c3HL+mH>buf)?buf+1:0))] *(1.0f-(mH-floor(mH)));
			o3HL += (dram->a3HL[(int)(c3HL+mH+1-((c3HL+mH+1>buf)?buf+1:0))] *((mH-floor(mH))));
			float o3IL = dram->a3IL[(int)(c3IL+mI-((c3IL+mI>buf)?buf+1:0))] *(1.0f-(mI-floor(mI)));
			o3IL += (dram->a3IL[(int)(c3IL+mI+1-((c3IL+mI+1>buf)?buf+1:0))] *((mI-floor(mI))));
			float o3AR = dram->a3AR[(int)(c3AR+mA-((c3AR+mA>buf)?buf+1:0))] *(1.0f-(mA-floor(mA)));
			o3AR += (dram->a3AR[(int)(c3AR+mA+1-((c3AR+mA+1>buf)?buf+1:0))] *((mA-floor(mA))));
			float o3DR = dram->a3DR[(int)(c3DR+mD-((c3DR+mD>buf)?buf+1:0))] *(1.0f-(mD-floor(mD)));
			o3DR += (dram->a3DR[(int)(c3DR+mD+1-((c3DR+mD+1>buf)?buf+1:0))] *((mD-floor(mD))));
			float o3GR = dram->a3GR[(int)(c3GR+mG-((c3GR+mG>buf)?buf+1:0))] *(1.0f-(mG-floor(mG)));
			o3GR += (dram->a3GR[(int)(c3GR+mG+1-((c3GR+mG+1>buf)?buf+1:0))] *((mG-floor(mG))));
			
			f3AL = (((o3GR + o3HR) * -2.0f) + o3IR);				
			f3BL = (((o3GR + o3HR) * -2.0f) + o3IR);
			f3CL = (((o3GR + o3HR) * -2.0f) + o3IR);
			
			f3CR = (((o3AL + o3DL) * -2.0f) + o3GL);
			f3FR = (((o3AL + o3DL) * -2.0f) + o3GL);
			f3IR = (((o3AL + o3DL) * -2.0f) + o3GL);
			
			float inputSampleL = (o3GL + o3HL + o3IL)*0.03125f;
			float inputSampleR = (o3AR + o3DR + o3GR)*0.03125f;
			
			f3AL = (f3AL+f3AL+f3AL+fabs(avg3L))*0.25f; avg3L = f3AL;
			f3CR = (f3CR+f3CR+f3CR+fabs(avg3R))*0.25f; avg3R = f3CR;
			//manipulating deep reverb tail for realism	
			
			//begin ClipOnly stereo as a little, compressed chunk that can be dropped into undersampled code
			inputSampleL = fmin(fmax(inputSampleL,-4.0f),4.0f);
			if (wasPosClipL == true) { //current will be over
				if (inputSampleL<lastSampleL) lastSampleL=0.79f+(inputSampleL*0.2f);
				else lastSampleL = 0.2f+(lastSampleL*0.79f);
			} wasPosClipL = false;
			if (inputSampleL>0.99f) {wasPosClipL=true;inputSampleL=0.79f+(lastSampleL*0.2f);}
			if (wasNegClipL == true) { //current will be -over
				if (inputSampleL > lastSampleL) lastSampleL=-0.79f+(inputSampleL*0.2f);
				else lastSampleL=-0.2f+(lastSampleL*0.79f);
			} wasNegClipL = false;
			if (inputSampleL<-0.99f) {wasNegClipL=true;inputSampleL=-0.79f+(lastSampleL*0.2f);}
			lastSampleL = inputSampleL;
			inputSampleR = fmin(fmax(inputSampleR,-4.0f),4.0f);
			if (wasPosClipR == true) { //current will be over
				if (inputSampleR<lastSampleR) lastSampleR=0.79f+(inputSampleR*0.2f);
				else lastSampleR = 0.2f+(lastSampleR*0.79f);
			} wasPosClipR = false;
			if (inputSampleR>0.99f) {wasPosClipR=true;inputSampleR=0.79f+(lastSampleR*0.2f);}
			if (wasNegClipR == true) { //current will be -over
				if (inputSampleR > lastSampleR) lastSampleR=-0.79f+(inputSampleR*0.2f);
				else lastSampleR=-0.2f+(lastSampleR*0.79f);
			} wasNegClipR = false;
			if (inputSampleR<-0.99f) {wasNegClipR=true;inputSampleR=-0.79f+(lastSampleR*0.2f);}
			lastSampleR = inputSampleR;
			//end ClipOnly stereo as a little, compressed chunk that can be dropped into undersampled code							
			
			inputSampleL = asin(inputSampleL*0.7f);
			inputSampleR = asin(inputSampleR*0.7f);
			
			dram->bez[bez_CL] = dram->bez[bez_BL];
			dram->bez[bez_BL] = dram->bez[bez_AL];
			dram->bez[bez_AL] = inputSampleL;
			dram->bez[bez_SampL] = 0.0f;
			
			dram->bez[bez_CR] = dram->bez[bez_BR];
			dram->bez[bez_BR] = dram->bez[bez_AR];
			dram->bez[bez_AR] = inputSampleR;
			dram->bez[bez_SampR] = 0.0f;
		}
		
		float X = dram->bez[bez_cycle] * bezTrim;
		float CBL = (dram->bez[bez_CL]*(1.0f-X))+(dram->bez[bez_BL]*X);
		float CBR = (dram->bez[bez_CR]*(1.0f-X))+(dram->bez[bez_BR]*X);
		float BAL = (dram->bez[bez_BL]*(1.0f-X))+(dram->bez[bez_AL]*X);
		float BAR = (dram->bez[bez_BR]*(1.0f-X))+(dram->bez[bez_AR]*X);
		float CBAL = (dram->bez[bez_BL]+(CBL*(1.0f-X))+(BAL*X))*-0.25f;
		float CBAR = (dram->bez[bez_BR]+(CBR*(1.0f-X))+(BAR*X))*-0.25f;
		inputSampleL = CBAL+dram->bez[bez_AvgOutSampL]; dram->bez[bez_AvgOutSampL] = CBAL;
		inputSampleR = CBAR+dram->bez[bez_AvgOutSampR]; dram->bez[bez_AvgOutSampR] = CBAR;
		
		if (out != 1.0f) {
			inputSampleL *= out;
			inputSampleR *= out;
		}
				
		if (wet != 1.0f) {
		 inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
		 inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		
		
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
	for(int x = 0; x < 32767+2; x++) {dram->a3AL[x] = 0.0; dram->a3AR[x] = 0.0;}
	for(int x = 0; x < 32767+2; x++) {dram->a3BL[x] = 0.0; dram->a3BR[x] = 0.0;}
	for(int x = 0; x < 32767+2; x++) {dram->a3CL[x] = 0.0; dram->a3CR[x] = 0.0;}
	for(int x = 0; x < 32767+2; x++) {dram->a3DL[x] = 0.0; dram->a3DR[x] = 0.0;}
	for(int x = 0; x < 32767+2; x++) {dram->a3EL[x] = 0.0; dram->a3ER[x] = 0.0;}
	for(int x = 0; x < 32767+2; x++) {dram->a3FL[x] = 0.0; dram->a3FR[x] = 0.0;}
	for(int x = 0; x < 32767+2; x++) {dram->a3GL[x] = 0.0; dram->a3GR[x] = 0.0;}
	for(int x = 0; x < 32767+2; x++) {dram->a3HL[x] = 0.0; dram->a3HR[x] = 0.0;}
	for(int x = 0; x < 32767+2; x++) {dram->a3IL[x] = 0.0; dram->a3IR[x] = 0.0;}
	c3AL = c3BL = c3CL = c3DL = c3EL = c3FL = c3GL = c3HL = c3IL = 1;
	c3AR = c3BR = c3CR = c3DR = c3ER = c3FR = c3GR = c3HR = c3IR = 1;
	f3AL = f3BL = f3CL = 0.0;
	f3CR = f3FR = f3IR = 0.0;
	avg3L = avg3R = 0.0;
	
	for (int x = 0; x < bez_total; x++) dram->bez[x] = 0.0;
	dram->bez[bez_cycle] = 1.0;
	
	rotate = 0.0;
	oldfpd = 0.4294967295;
	
	buf = 8192;
	vibDepth = 0.0;
	outA = 1.0;
	outB = 1.0;
	wetA = 1.0;
	wetB = 1.0;
	derezA = 0.5;
	derezB = 0.5;

	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	lastSampleR = 0.0;
	wasPosClipR = false;
	wasNegClipR = false;

	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
