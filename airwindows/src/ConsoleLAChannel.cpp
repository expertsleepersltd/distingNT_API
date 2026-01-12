#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleLAChannel"
#define AIRWINDOWS_DESCRIPTION "The Airwindows take on the Quad Eight console."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','6' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
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
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float subAL;
	float subAR;
	float subBL;
	float subBR;
	float subCL;
	float subCR;
	int hullp;
	float midA;
	float midB;
	float bassA;
	float bassB;
	float gainA;
	float gainB; //smoothed master fader for channel, from Z2 series filter code
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float hullL[225];	
	float hullR[225];	
	float pearB[22];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate(); //will be over 1.0848f when over 48k
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	
	int limit = 4*cycleEnd;
	float divisor = 2.0f/limit;

	float treble = (GetParameter( kParam_One )*6.0f)-3.0f;
	midA = midB;
	midB = (GetParameter( kParam_Two )*6.0f)-3.0f;
	bassA = bassB;
	bassB = (GetParameter( kParam_Three )*6.0f)-3.0f;
	//these should stack to go up to -3.0f to 3.0f
	if (treble < 0.0f) treble /= 3.0f;
	if (midB < 0.0f) midB /= 3.0f;
	if (bassB < 0.0f) bassB /= 3.0f;
	//and then become -1.0f to 3.0f;
	//there will be successive sin/cos stages w. dry/wet in these
	float freqMid = 0.024f;
	switch (cycleEnd)
	{
		case 1: //base sample rate, no change
			break;
		case 2: //96k tier
			freqMid = 0.012f;
			break;
		case 3: //192k tier
			freqMid = 0.006f;
			break;
	}
	
	
	int bitshiftL = 0;
	int bitshiftR = 0;
	float panControl = (GetParameter( kParam_Four )*2.0f)-1.0f; //-1.0f to 1.0f
	float panAttenuation = (1.0f-fabs(panControl));
	int panBits = 20; //start centered
	if (panAttenuation > 0.0f) panBits = floor(1.0f / panAttenuation);
	if (panControl > 0.25f) bitshiftL += panBits;
	if (panControl < -0.25f) bitshiftR += panBits;
	if (bitshiftL < 0) bitshiftL = 0; if (bitshiftL > 17) bitshiftL = 17;
	if (bitshiftR < 0) bitshiftR = 0; if (bitshiftR > 17) bitshiftR = 17;
	float gainL = 1.0f;
	float gainR = 1.0f;
	switch (bitshiftL)
	{
		case 17: gainL = 0.0f; break;
		case 16: gainL = 0.0000152587890625f; break;
		case 15: gainL = 0.000030517578125f; break;
		case 14: gainL = 0.00006103515625f; break;
		case 13: gainL = 0.0001220703125f; break;
		case 12: gainL = 0.000244140625f; break;
		case 11: gainL = 0.00048828125f; break;
		case 10: gainL = 0.0009765625f; break;
		case 9: gainL = 0.001953125f; break;
		case 8: gainL = 0.00390625f; break;
		case 7: gainL = 0.0078125f; break;
		case 6: gainL = 0.015625f; break;
		case 5: gainL = 0.03125f; break;
		case 4: gainL = 0.0625f; break;
		case 3: gainL = 0.125f; break;
		case 2: gainL = 0.25f; break;
		case 1: gainL = 0.5f; break;
		case 0: break;
	}
	switch (bitshiftR)
	{
		case 17: gainR = 0.0f; break;
		case 16: gainR = 0.0000152587890625f; break;
		case 15: gainR = 0.000030517578125f; break;
		case 14: gainR = 0.00006103515625f; break;
		case 13: gainR = 0.0001220703125f; break;
		case 12: gainR = 0.000244140625f; break;
		case 11: gainR = 0.00048828125f; break;
		case 10: gainR = 0.0009765625f; break;
		case 9: gainR = 0.001953125f; break;
		case 8: gainR = 0.00390625f; break;
		case 7: gainR = 0.0078125f; break;
		case 6: gainR = 0.015625f; break;
		case 5: gainR = 0.03125f; break;
		case 4: gainR = 0.0625f; break;
		case 3: gainR = 0.125f; break;
		case 2: gainR = 0.25f; break;
		case 1: gainR = 0.5f; break;
		case 0: break;
	}
	
	gainA = gainB;
	gainB = GetParameter( kParam_Five )*2.0f; //smoothed master fader from Z2 filters
	//BitShiftGain pre gain trim goes here
	
	float subTrim = 0.0011f / overallscale;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float gain = (gainA*temp)+(gainB*(1.0f-temp));
		float mid = (midA*temp)+(midB*(1.0f-temp));
		float bass = (bassA*temp)+(bassB*(1.0f-temp));
		
		//begin Hull2 Treble
		hullp--; if (hullp < 0) hullp += 60;
		dram->hullL[hullp] = dram->hullL[hullp+60] = inputSampleL;
		dram->hullR[hullp] = dram->hullR[hullp+60] = inputSampleR;
		
		int x = hullp;
		float bassL = 0.0f;
		float bassR = 0.0f;
		while (x < hullp+(limit/2)) {
			bassL += dram->hullL[x] * divisor;
			bassR += dram->hullR[x] * divisor;
			x++;
		}
		bassL += bassL * 0.125f;
		bassR += bassR * 0.125f;
		while (x < hullp+limit) {
			bassL -= dram->hullL[x] * 0.125f * divisor;
			bassR -= dram->hullR[x] * 0.125f * divisor;
			x++;
		}
		dram->hullL[hullp+20] = dram->hullL[hullp+80] = bassL;
		dram->hullR[hullp+20] = dram->hullR[hullp+80] = bassR;
		x = hullp+20;
		bassL = bassR = 0.0f;
		while (x < hullp+20+(limit/2)) {
			bassL += dram->hullL[x] * divisor;
			bassR += dram->hullR[x] * divisor;
			x++;
		}
		bassL += bassL * 0.125f;
		bassR += bassR * 0.125f;
		while (x < hullp+20+limit) {
			bassL -= dram->hullL[x] * 0.125f * divisor;
			bassR -= dram->hullR[x] * 0.125f * divisor;
			x++;
		}
		dram->hullL[hullp+40] = dram->hullL[hullp+100] = bassL;
		dram->hullR[hullp+40] = dram->hullR[hullp+100] = bassR;
		x = hullp+40;
		bassL = bassR = 0.0f;
		while (x < hullp+40+(limit/2)) {
			bassL += dram->hullL[x] * divisor;
			bassR += dram->hullR[x] * divisor;
			x++;
		}
		bassL += bassL * 0.125f;
		bassR += bassR * 0.125f;
		while (x < hullp+40+limit) {
			bassL -= dram->hullL[x] * 0.125f * divisor;
			bassR -= dram->hullR[x] * 0.125f * divisor;
			x++;
		}
		float trebleL = inputSampleL - bassL; inputSampleL = bassL;
		float trebleR = inputSampleR - bassR; inputSampleR = bassR;
		//end Hull2 treble
		
		//begin Pear filter stages
		//at this point 'bass' is actually still mid and bass
		float slew = ((bassL - dram->pearB[0]) + dram->pearB[1])*freqMid*0.5f;
		dram->pearB[0] = bassL = (freqMid * bassL) + ((1.0f-freqMid) * (dram->pearB[0] + dram->pearB[1]));
		dram->pearB[1] = slew; slew = ((bassR - dram->pearB[2]) + dram->pearB[3])*freqMid*0.5f;
		dram->pearB[2] = bassR = (freqMid * bassR) + ((1.0f-freqMid) * (dram->pearB[2] + dram->pearB[3]));
		dram->pearB[3] = slew; slew = ((bassL - dram->pearB[4]) + dram->pearB[5])*freqMid*0.5f;
		dram->pearB[4] = bassL = (freqMid * bassL) + ((1.0f-freqMid) * (dram->pearB[4] + dram->pearB[5]));
		dram->pearB[5] = slew; slew = ((bassR - dram->pearB[6]) + dram->pearB[7])*freqMid*0.5f;
		dram->pearB[6] = bassR = (freqMid * bassR) + ((1.0f-freqMid) * (dram->pearB[6] + dram->pearB[7]));
		dram->pearB[7] = slew; slew = ((bassL - dram->pearB[8]) + dram->pearB[9])*freqMid*0.5f;
		dram->pearB[8] = bassL = (freqMid * bassL) + ((1.0f-freqMid) * (dram->pearB[8] + dram->pearB[9]));
		dram->pearB[9] = slew; slew = ((bassR - dram->pearB[10]) + dram->pearB[11])*freqMid*0.5f;
		dram->pearB[10] = bassR = (freqMid * bassR) + ((1.0f-freqMid) * (dram->pearB[10] + dram->pearB[11]));
		dram->pearB[11] = slew; slew = ((bassL - dram->pearB[12]) + dram->pearB[13])*freqMid*0.5f;
		dram->pearB[12] = bassL = (freqMid * bassL) + ((1.0f-freqMid) * (dram->pearB[12] + dram->pearB[13]));
		dram->pearB[13] = slew; slew = ((bassR - dram->pearB[14]) + dram->pearB[15])*freqMid*0.5f;
		dram->pearB[14] = bassR = (freqMid * bassR) + ((1.0f-freqMid) * (dram->pearB[14] + dram->pearB[15]));
		dram->pearB[15] = slew; slew = ((bassL - dram->pearB[16]) + dram->pearB[17])*freqMid*0.5f;
		dram->pearB[16] = bassL = (freqMid * bassL) + ((1.0f-freqMid) * (dram->pearB[16] + dram->pearB[17]));
		dram->pearB[17] = slew; slew = ((bassR - dram->pearB[18]) + dram->pearB[19])*freqMid*0.5f;
		dram->pearB[18] = bassR = (freqMid * bassR) + ((1.0f-freqMid) * (dram->pearB[18] + dram->pearB[19]));
		dram->pearB[19] = slew;
		float midL = inputSampleL - bassL;
		float midR = inputSampleR - bassR;
		//we now have three bands out of hull and pear filters
		
		float w = 0.0f; //filter into bands, apply the sin/cos to each band
		if (treble > 0.0f) {
			w = treble; if (w > 1.0f) w = 1.0f;
			trebleL = (trebleL*(1.0f-w)) + (sin(trebleL*M_PI_2)*treble);
			trebleR = (trebleR*(1.0f-w)) + (sin(trebleR*M_PI_2)*treble);
		}
		if (treble < 0.0f) {
			if (trebleL > 1.0f) trebleL = 1.0f; if (trebleL < -1.0f) trebleL = -1.0f;
			if (trebleR > 1.0f) trebleR = 1.0f; if (trebleR < -1.0f) trebleR = -1.0f;
			w = -treble; if (w > 1.0f) w = 1.0f;
			if (trebleL > 0) trebleL = (trebleL*(1.0f-w))+((1.0f-cos(trebleL*w))*(1.0f-w));
			else trebleL = (trebleL*(1.0f-w))+((-1.0f+cos(-trebleL*w))*(1.0f-w));
			if (trebleR > 0) trebleR = (trebleR*(1.0f-w))+((1.0f-cos(trebleR*w))*(1.0f-w));
			else trebleR = (trebleR*(1.0f-w))+((-1.0f+cos(-trebleR*w))*(1.0f-w));
		} //cosine stages for EQ or expansion
		
		if (midL > 1.0f) midL = 1.0f; if (midL < -1.0f) midL = -1.0f;
		if (midR > 1.0f) midR = 1.0f; if (midR < -1.0f) midR = -1.0f;
		if (mid > 0.0f) {
			w = mid; if (w > 1.0f) w = 1.0f;
			midL = (midL*(1.0f-w)) + (sin(midL*M_PI_2)*mid);
			midR = (midR*(1.0f-w)) + (sin(midR*M_PI_2)*mid);
		}
		if (mid < 0.0f) {
			w = -mid; if (w > 1.0f) w = 1.0f;	
			if (midL > 0) midL = (midL*(1.0f-w))+((1.0f-cos(midL*w))*(1.0f-w));
			else midL = (midL*(1.0f-w))+((-1.0f+cos(-midL*w))*(1.0f-w));
			if (midR > 0) midR = (midR*(1.0f-w))+((1.0f-cos(midR*w))*(1.0f-w));
			else midR = (midR*(1.0f-w))+((-1.0f+cos(-midR*w))*(1.0f-w));
		} //cosine stages for EQ or expansion
		
		if (bassL > 1.0f) bassL = 1.0f; if (bassL < -1.0f) bassL = -1.0f;
		if (bassR > 1.0f) bassR = 1.0f; if (bassR < -1.0f) bassR = -1.0f;
		if (bass > 0.0f) {
			w = bass; if (w > 1.0f) w = 1.0f;
			bassL = (bassL*(1.0f-w)) + (sin(bassL*M_PI_2)*bass);
			bassR = (bassR*(1.0f-w)) + (sin(bassR*M_PI_2)*bass);
		}
		if (bass < 0.0f) {
			w = -bass; if (w > 1.0f) w = 1.0f;
			if (bassL > 0) bassL = (bassL*(1.0f-w))+((1.0f-cos(bassL*w))*(1.0f-w));
			else bassL = (bassL*(1.0f-w))+((-1.0f+cos(-bassL*w))*(1.0f-w));
			if (bassR > 0) bassR = (bassR*(1.0f-w))+((1.0f-cos(bassR*w))*(1.0f-w));
			else bassR = (bassR*(1.0f-w))+((-1.0f+cos(-bassR*w))*(1.0f-w));
		} //cosine stages for EQ or expansion
		
		inputSampleL = (bassL + midL + trebleL)*gainL*gain;
		inputSampleR = (bassR + midR + trebleR)*gainR*gain;
		//applies BitShiftPan pan section, and smoothed fader gain
		
		//begin SubTight section
		float subSampleL = inputSampleL * subTrim;
		float subSampleR = inputSampleR * subTrim;
		
		float scale = 0.5f+fabs(subSampleL*0.5f);
		subSampleL = (subAL+(sin(subAL-subSampleL)*scale));
		subAL = subSampleL*scale;
		scale = 0.5f+fabs(subSampleR*0.5f);
		subSampleR = (subAR+(sin(subAR-subSampleR)*scale));
		subAR = subSampleR*scale;
		scale = 0.5f+fabs(subSampleL*0.5f);
		subSampleL = (subBL+(sin(subBL-subSampleL)*scale));
		subBL = subSampleL*scale;
		scale = 0.5f+fabs(subSampleR*0.5f);
		subSampleR = (subBR+(sin(subBR-subSampleR)*scale));
		subBR = subSampleR*scale;
		scale = 0.5f+fabs(subSampleL*0.5f);
		subSampleL = (subCL+(sin(subCL-subSampleL)*scale));
		subCL = subSampleL*scale;
		scale = 0.5f+fabs(subSampleR*0.5f);
		subSampleR = (subCR+(sin(subCR-subSampleR)*scale));
		subCR = subSampleR*scale;
		if (subSampleL > 0.25f) subSampleL = 0.25f;
		if (subSampleL < -0.25f) subSampleL = -0.25f;
		if (subSampleR > 0.25f) subSampleR = 0.25f;
		if (subSampleR < -0.25f) subSampleR = -0.25f;
		inputSampleL += (subSampleL*16.0f);
		inputSampleR += (subSampleR*16.0f);
		//end SubTight section
		
		//begin Console7 Channel processing
		if (inputSampleL > 1.097f) inputSampleL = 1.097f;
		if (inputSampleL < -1.097f) inputSampleL = -1.097f;
		if (inputSampleR > 1.097f) inputSampleR = 1.097f;
		if (inputSampleR < -1.097f) inputSampleR = -1.097f;
		inputSampleL = ((sin(inputSampleL*fabs(inputSampleL))/((fabs(inputSampleL) == 0.0f) ?1:fabs(inputSampleL)))*0.8f)+(sin(inputSampleL)*0.2f);
		inputSampleR = ((sin(inputSampleR*fabs(inputSampleR))/((fabs(inputSampleR) == 0.0f) ?1:fabs(inputSampleR)))*0.8f)+(sin(inputSampleR)*0.2f);
		//this is a version of Spiral blended 80/20 with regular Density.
		//It's blending between two different harmonics in the overtones of the algorithm
		
		
		
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
	for(int count = 0; count < 222; count++) {dram->hullL[count] = 0.0; dram->hullR[count] = 0.0;}
	hullp = 1;
	for (int x = 0; x < 21; x++) dram->pearB[x] = 0.0;
	subAL = subAR = subBL = subBR = subCL = subCR = 0.0;
	midA = midB = 0.0;
	bassA = bassB = 0.0;
	gainA = gainB = 1.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
