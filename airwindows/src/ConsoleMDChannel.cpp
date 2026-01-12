#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleMDChannel"
#define AIRWINDOWS_DESCRIPTION "Goes for the tone shaping of the classic MCI console!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o',':' )
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
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MidFreq", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MidPeak", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float prevfreqMPeak;
	float prevamountMPeak;
	int mpc;
	float bassA;
	float bassB;
	float gainA;
	float gainB; //smoothed master fader for channel, from Z2 series filter code
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float pearA[18];
	float pearB[22];
	float mpkL[2005];
	float mpkR[2005];
	float f[66];
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
	if (cycleEnd > 3) cycleEnd = 3;
	
	float fatTreble = (GetParameter( kParam_One )*6.0f)-3.0f;
	bassA = bassB;
	bassB = (GetParameter( kParam_Four )*6.0f)-3.0f;
	//these should stack to go up to -3.0f to 3.0f
	if (fatTreble < 0.0f) fatTreble /= 3.0f;
	if (bassB < 0.0f) bassB /= 3.0f;
	//and then become -1.0f to 3.0f;
	//there will be successive sin/cos stages w. dry/wet in these
	float freqTreble = 0.853f;
	float freqMid = 0.026912f;
	switch (cycleEnd)
	{
		case 1: //base sample rate, no change
			break;
		case 2: //96k tier
			freqTreble = 0.4265f;
			freqMid = 0.013456f;
			break;
		case 3: //192k tier
			freqTreble = 0.21325f;
			freqMid = 0.006728f;
			break;
	}
	
	//begin ResEQ2 Mid Boost
	float freqMPeak = powf(GetParameter( kParam_Two )+0.16f,3);
	float amountMPeak = powf(GetParameter( kParam_Three ),2);
	int maxMPeak = (amountMPeak*63.0f)+1;
	if ((freqMPeak != prevfreqMPeak)||(amountMPeak != prevamountMPeak)) {
		for (int x = 0; x < maxMPeak; x++) {
			if (((float)x*freqMPeak) < M_PI_4) dram->f[x] = sin(((float)x*freqMPeak)*4.0f)*freqMPeak*sin(((float)(maxMPeak-x)/(float)maxMPeak)*M_PI_2);
			else dram->f[x] = cos((float)x*freqMPeak)*freqMPeak*sin(((float)(maxMPeak-x)/(float)maxMPeak)*M_PI_2);
		}
		prevfreqMPeak = freqMPeak; prevamountMPeak = amountMPeak;
	}//end ResEQ2 Mid Boost
	
	int bitshiftL = 0;
	int bitshiftR = 0;
	float panControl = (GetParameter( kParam_Five )*2.0f)-1.0f; //-1.0f to 1.0f
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
	gainB = GetParameter( kParam_Six )*2.0f; //smoothed master fader from Z2 filters
	//BitShiftGain pre gain trim goes here
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float gain = (gainA*temp)+(gainB*(1.0f-temp));
		float bass = (bassA*temp)+(bassB*(1.0f-temp));
		inputSampleL *= gain;
		inputSampleR *= gain;
		//for MCI consoles, the fader is before the EQ, which overdrives easily.
		//so we put the main fader here.
		
		//begin Pear filter stages
		float bassL = inputSampleL;
		float bassR = inputSampleR;
		float slew = ((bassL - dram->pearA[0]) + dram->pearA[1])*freqTreble*0.5f;
		dram->pearA[0] = bassL = (freqTreble * bassL) + ((1.0f-freqTreble) * (dram->pearA[0] + dram->pearA[1]));
		dram->pearA[1] = slew; slew = ((bassR - dram->pearA[2]) + dram->pearA[3])*freqTreble*0.5f;
		dram->pearA[2] = bassR = (freqTreble * bassR) + ((1.0f-freqTreble) * (dram->pearA[2] + dram->pearA[3]));
		dram->pearA[3] = slew; slew = ((bassL - dram->pearA[4]) + dram->pearA[5])*freqTreble*0.5f;
		dram->pearA[4] = bassL = (freqTreble * bassL) + ((1.0f-freqTreble) * (dram->pearA[4] + dram->pearA[5]));
		dram->pearA[5] = slew; slew = ((bassR - dram->pearA[6]) + dram->pearA[7])*freqTreble*0.5f;
		dram->pearA[6] = bassR = (freqTreble * bassR) + ((1.0f-freqTreble) * (dram->pearA[6] + dram->pearA[7]));
		dram->pearA[7] = slew; slew = ((bassL - dram->pearA[8]) + dram->pearA[9])*freqTreble*0.5f;
		dram->pearA[8] = bassL = (freqTreble * bassL) + ((1.0f-freqTreble) * (dram->pearA[8] + dram->pearA[9]));
		dram->pearA[9] = slew; slew = ((bassR - dram->pearA[10]) + dram->pearA[11])*freqTreble*0.5f;
		dram->pearA[10] = bassR = (freqTreble * bassR) + ((1.0f-freqTreble) * (dram->pearA[10] + dram->pearA[11]));
		dram->pearA[11] = slew; slew = ((bassL - dram->pearA[12]) + dram->pearA[13])*freqTreble*0.5f;
		dram->pearA[12] = bassL = (freqTreble * bassL) + ((1.0f-freqTreble) * (dram->pearA[12] + dram->pearA[13]));
		dram->pearA[13] = slew; slew = ((bassR - dram->pearA[14]) + dram->pearA[15])*freqTreble*0.5f;
		dram->pearA[14] = bassR = (freqTreble * bassR) + ((1.0f-freqTreble) * (dram->pearA[14] + dram->pearA[15]));
		dram->pearA[15] = slew;
		//unrolled mid/treble crossover (called bass to use fewer variables)		
		float trebleL = inputSampleL - bassL; inputSampleL = bassL;
		float trebleR = inputSampleR - bassR; inputSampleR = bassR;
		//at this point 'bass' is actually still mid and bass
		slew = ((bassL - dram->pearB[0]) + dram->pearB[1])*freqMid*0.5f;
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
		//we now have three bands out of two pear filters
		
		float w = 0.0f; //filter into bands, apply the sin/cos to each band
		if (fatTreble > 0.0f) {
			w = fatTreble; if (w > 1.0f) w = 1.0f;
			trebleL = (trebleL*(1.0f-w)) + (sin(trebleL*M_PI_2)*w);
			trebleR = (trebleR*(1.0f-w)) + (sin(trebleR*M_PI_2)*w);
			if (fatTreble > 1.0f) {
				w = fatTreble-1.0f; if (w > 1.0f) w = 1.0f;
				trebleL = (trebleL*(1.0f-w)) + (sin(trebleL*M_PI_2)*w);
				trebleR = (trebleR*(1.0f-w)) + (sin(trebleR*M_PI_2)*w);
				if (fatTreble > 2.0f) {
					w = fatTreble-2.0f;
					trebleL = (trebleL*(1.0f-w)) + (sin(trebleL*M_PI_2)*w);
					trebleR = (trebleR*(1.0f-w)) + (sin(trebleR*M_PI_2)*w);
				} //sine stages for EQ or compression
			}
		}
		if (fatTreble < 0.0f) {
			if (trebleL > 1.0f) trebleL = 1.0f; if (trebleL < -1.0f) trebleL = -1.0f;
			if (trebleR > 1.0f) trebleR = 1.0f; if (trebleR < -1.0f) trebleR = -1.0f;
			w = -fatTreble; if (w > 1.0f) w = 1.0f;
			if (trebleL > 0) trebleL = (trebleL*(1.0f-w))+((1.0f-cos(trebleL))*sin(w));
			else trebleL = (trebleL*(1.0f-w))+((-1.0f+cos(-trebleL))*sin(w));
			if (trebleR > 0) trebleR = (trebleR*(1.0f-w))+((1.0f-cos(trebleR))*sin(w));
			else trebleR = (trebleR*(1.0f-w))+((-1.0f+cos(-trebleR))*sin(w));
		} //cosine stages for EQ or expansion
		
		//begin ResEQ2 Mid Boost
		mpc++; if (mpc < 1 || mpc > 2001) mpc = 1;
		dram->mpkL[mpc] = midL;
		dram->mpkR[mpc] = midR;
		float midMPeakL = 0.0f;
		float midMPeakR = 0.0f;
		for (int x = 0; x < maxMPeak; x++) {
			int y = x*cycleEnd;
			switch (cycleEnd)
			{
				case 1: 
					midMPeakL += (dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x]);
					midMPeakR += (dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x]); break;
				case 2: 
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.5f);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.5f); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.5f);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.5f); break;
				case 3: 
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333f);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333f); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333f);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333f); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333f);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333f); break;
				case 4: 
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25f); //break
			}
		}
		midL = (midMPeakL*amountMPeak)+((1.5f-amountMPeak>1.0f)?midL:midL*(1.5f-amountMPeak));
		midR = (midMPeakR*amountMPeak)+((1.5f-amountMPeak>1.0f)?midR:midR*(1.5f-amountMPeak));
		//end ResEQ2 Mid Boost
		
		if (bassL > 1.0f) bassL = 1.0f; if (bassL < -1.0f) bassL = -1.0f;
		if (bassR > 1.0f) bassR = 1.0f; if (bassR < -1.0f) bassR = -1.0f;
		if (bass > 0.0f) {
			w = bass; if (w > 1.0f) w = 1.0f;
			bassL = (bassL*(1.0f-w)) + (sin(bassL*M_PI_2)*w*1.6f);
			bassR = (bassR*(1.0f-w)) + (sin(bassR*M_PI_2)*w*1.6f);
			if (bass > 1.0f) {
				w = bass-1.0f; if (w > 1.0f) w = 1.0f;
				bassL = (bassL*(1.0f-w)) + (sin(bassL*M_PI_2)*w*1.4f);
				bassR = (bassR*(1.0f-w)) + (sin(bassR*M_PI_2)*w*1.4f);
				if (bass > 2.0f) {
					w = bass-2.0f;
					bassL = (bassL*(1.0f-w)) + (sin(bassL*M_PI_2)*w*1.2f);
					bassR = (bassR*(1.0f-w)) + (sin(bassR*M_PI_2)*w*1.2f);
				} //sine stages for EQ or compression
			}
		}
		if (bass < 0.0f) {
			w = -bass; if (w > 1.0f) w = 1.0f;	
			if (bassL > 0) bassL = (bassL*(1.0f-w))+((1.0f-cos(bassL))*sin(w));
			else bassL = (bassL*(1.0f-w))+((-1.0f+cos(-bassL))*sin(w));
			if (bassR > 0) bassR = (bassR*(1.0f-w))+((1.0f-cos(bassR))*sin(w));
			else bassR = (bassR*(1.0f-w))+((-1.0f+cos(-bassR))*sin(w));
		} //cosine stages for EQ or expansion
		//the sin() is further restricting output when fully attenuated
				
		inputSampleL = (bassL + midL + trebleL)*gainL;
		inputSampleR = (bassR + midR + trebleR)*gainR;
		//applies BitShiftPan pan section
		
		//begin sin() style Channel processing
		if (inputSampleL > 1.57079633f) inputSampleL = 1.57079633f;
		if (inputSampleL < -1.57079633f) inputSampleL = -1.57079633f;
		if (inputSampleR > 1.57079633f) inputSampleR = 1.57079633f;
		if (inputSampleR < -1.57079633f) inputSampleR = -1.57079633f;
		inputSampleL = sin(inputSampleL);
		inputSampleR = sin(inputSampleR);
		
		
		
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
	for (int x = 0; x < 17; x++) dram->pearA[x] = 0.0;
	for (int x = 0; x < 21; x++) dram->pearB[x] = 0.0;
	for(int count = 0; count < 2004; count++) {dram->mpkL[count] = 0.0; dram->mpkR[count] = 0.0;}
	for(int count = 0; count < 65; count++) {dram->f[count] = 0.0;}
	prevfreqMPeak = -1;
	prevamountMPeak = -1;
	mpc = 1;	
	bassA = bassB = 0.0;
	gainA = gainB = 1.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
