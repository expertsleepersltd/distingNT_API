#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "StarChild2"
#define AIRWINDOWS_DESCRIPTION "A weird digital ambience/echo plugin adapted to high sample rates."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','t','2' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
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
{ .name = "Sustain Duration", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Grain Coarse/Fine", .min = 0, .max = 1000, .def = 700, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	UInt32 dCount;
	
	UInt32 p[171];
	UInt32 t[171];
	UInt32 pitchCounter;
	UInt32 increment;
	UInt32 dutyCycle;
	
	Float32 wearL[11];
	Float32 wearR[11];
	Float32 factor[11];		
	Float32 wearLPrev;
	Float32 wearRPrev;
	
	float lastRefL[7];
	float lastRefR[7];
	int cycle;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		Float32 d[45102];
	Float32 outL[171];
	Float32 outR[171];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	
	UInt32 nSampleFrames = inFramesToProcess;
	
	float cyclescale = 1.0f;
	cyclescale /= 44100.0f;
	cyclescale *= GetSampleRate();
	int cycleEnd = floor(cyclescale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check
	
	
	Float32 drySampleL;
	Float32 drySampleR;
	Float32 inputSampleL;
	Float32 inputSampleR;
	
	int bufferL = 0;
	int bufferR = 0;
	//these are to build up the reverb tank outs
	
	UInt32 rangeDirect = (powf(GetParameter( kParam_Two ),2) * 156.0f) + 7.0f;
	//maximum safe delay is 259 * the prime tap, not including room for the pitch shift offset
	
	Float32 scaleDirect = (powf(GetParameter( kParam_One ),2) * (3280.0f/rangeDirect)) + 2.0f;
	//let's try making it always be the max delay: smaller range forces scale to be longer
	
	Float32 outputPad = 4 * rangeDirect * sqrt(rangeDirect);
	Float32 overallscale = ((1.0f-GetParameter( kParam_Two ))*9.0f)+1.0f;
	//apply the singlestage groove wear strongest when bits are heavily crushed
	Float32 gain = overallscale;
	if (gain > 1.0f) {factor[0] = 1.0f; gain -= 1.0f;} else {factor[0] = gain; gain = 0.0f;}
	if (gain > 1.0f) {factor[1] = 1.0f; gain -= 1.0f;} else {factor[1] = gain; gain = 0.0f;}
	if (gain > 1.0f) {factor[2] = 1.0f; gain -= 1.0f;} else {factor[2] = gain; gain = 0.0f;}
	if (gain > 1.0f) {factor[3] = 1.0f; gain -= 1.0f;} else {factor[3] = gain; gain = 0.0f;}
	if (gain > 1.0f) {factor[4] = 1.0f; gain -= 1.0f;} else {factor[4] = gain; gain = 0.0f;}
	if (gain > 1.0f) {factor[5] = 1.0f; gain -= 1.0f;} else {factor[5] = gain; gain = 0.0f;}
	if (gain > 1.0f) {factor[6] = 1.0f; gain -= 1.0f;} else {factor[6] = gain; gain = 0.0f;}
	if (gain > 1.0f) {factor[7] = 1.0f; gain -= 1.0f;} else {factor[7] = gain; gain = 0.0f;}
	if (gain > 1.0f) {factor[8] = 1.0f; gain -= 1.0f;} else {factor[8] = gain; gain = 0.0f;}
	if (gain > 1.0f) {factor[9] = 1.0f; gain -= 1.0f;} else {factor[9] = gain; gain = 0.0f;}
	//there, now we have a neat little moving average with remainders
	
	if (overallscale < 1.0f) overallscale = 1.0f;
	factor[0] /= overallscale;
	factor[1] /= overallscale;
	factor[2] /= overallscale;
	factor[3] /= overallscale;
	factor[4] /= overallscale;
	factor[5] /= overallscale;
	factor[6] /= overallscale;
	factor[7] /= overallscale;
	factor[8] /= overallscale;
	factor[9] /= overallscale;
	//and now it's neatly scaled, too
	Float32 accumulatorSample;
	Float32 correction;
	Float32 wetness = GetParameter( kParam_Three );
	Float32 dryness = 1.0f - wetness;	//reverb setup
	
	int count;
	for(count = 1; count < 165; count++)
	{
		t[count] = p[count] * scaleDirect;
		//this is the scaled tap for direct out, in number of samples delay
	}
	
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		//assign working variables like the dry
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;
		
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a reverb sample
			
			
			
			if (dCount < 0 || dCount > 22050) {dCount = 22050;}
			dram->d[dCount + 22050] = dram->d[dCount] = inputSampleL + inputSampleR;
			dCount--; 
			//feed the delay line with summed channels. The stuff we're reading back
			//will always be plus dCount, because we're counting back to 0.
			
			//now we're going to start pitch shifting.
			dutyCycle += 1;
			if (dutyCycle > scaleDirect) {
				dutyCycle = 1;
				//this whole routine doesn't run every sample, it's making a wacky hypervibrato
				t[pitchCounter] += increment; pitchCounter += 1;
				//pitchCounter always goes up, t[] goes up and down
				//possibly do that not every sample? Let's see what we get
				if (pitchCounter > rangeDirect) {
					if (increment == 1) {
						pitchCounter = 1;
						if (t[1] > ((11 * scaleDirect) + 1000)) increment = -1;
						//let's try hardcoding a big 1000 sample buffer
					}
					else {
						//increment is -1 so we have been counting down!
						pitchCounter = 1;
						
						if (t[1] < (11 * scaleDirect)) {
							//we've scaled everything back so we're going up again
							increment = 1;
							//and we're gonna reset the lot in case of screw-ups (control manipulations)
							for(count = 1; count < 165; count++)
							{
								t[count] = p[count] * scaleDirect;
							}
							//which means we're back to normal and counting up again.
						}
					}
					//wrap around to begin again, and if our first tap is greater than
					//its base value plus scaleDirect, start going down.
				}
			}
			//always wrap around to the first tap		
			
			//Now we do a select case where we jump into the middle of some repetitive math, unrolled.
			bufferL = 0; bufferR = 0;
			//clear before making our delay sound
			switch (rangeDirect) {
				case 164: bufferL += (int)(dram->d[dCount+t[164]]*dram->outL[164]); bufferR += (int)(dram->d[dCount+t[164]]*dram->outR[164]); 
				case 163: bufferL += (int)(dram->d[dCount+t[163]]*dram->outL[163]); bufferR += (int)(dram->d[dCount+t[163]]*dram->outR[163]); 
				case 162: bufferL += (int)(dram->d[dCount+t[162]]*dram->outL[162]); bufferR += (int)(dram->d[dCount+t[162]]*dram->outR[162]); 
				case 161: bufferL += (int)(dram->d[dCount+t[161]]*dram->outL[161]); bufferR += (int)(dram->d[dCount+t[161]]*dram->outR[161]); 
				case 160: bufferL += (int)(dram->d[dCount+t[160]]*dram->outL[160]); bufferR += (int)(dram->d[dCount+t[160]]*dram->outR[160]); 
				case 159: bufferL += (int)(dram->d[dCount+t[159]]*dram->outL[159]); bufferR += (int)(dram->d[dCount+t[159]]*dram->outR[159]); 
				case 158: bufferL += (int)(dram->d[dCount+t[158]]*dram->outL[158]); bufferR += (int)(dram->d[dCount+t[158]]*dram->outR[158]); 
				case 157: bufferL += (int)(dram->d[dCount+t[157]]*dram->outL[157]); bufferR += (int)(dram->d[dCount+t[157]]*dram->outR[157]); 
				case 156: bufferL += (int)(dram->d[dCount+t[156]]*dram->outL[156]); bufferR += (int)(dram->d[dCount+t[156]]*dram->outR[156]); 
				case 155: bufferL += (int)(dram->d[dCount+t[155]]*dram->outL[155]); bufferR += (int)(dram->d[dCount+t[155]]*dram->outR[155]); 
				case 154: bufferL += (int)(dram->d[dCount+t[154]]*dram->outL[154]); bufferR += (int)(dram->d[dCount+t[154]]*dram->outR[154]); 
				case 153: bufferL += (int)(dram->d[dCount+t[153]]*dram->outL[153]); bufferR += (int)(dram->d[dCount+t[153]]*dram->outR[153]); 
				case 152: bufferL += (int)(dram->d[dCount+t[152]]*dram->outL[152]); bufferR += (int)(dram->d[dCount+t[152]]*dram->outR[152]); 
				case 151: bufferL += (int)(dram->d[dCount+t[151]]*dram->outL[151]); bufferR += (int)(dram->d[dCount+t[151]]*dram->outR[151]); 
				case 150: bufferL += (int)(dram->d[dCount+t[150]]*dram->outL[150]); bufferR += (int)(dram->d[dCount+t[150]]*dram->outR[150]);
				case 149: bufferL += (int)(dram->d[dCount+t[149]]*dram->outL[149]); bufferR += (int)(dram->d[dCount+t[149]]*dram->outR[149]); 
				case 148: bufferL += (int)(dram->d[dCount+t[148]]*dram->outL[148]); bufferR += (int)(dram->d[dCount+t[148]]*dram->outR[148]); 
				case 147: bufferL += (int)(dram->d[dCount+t[147]]*dram->outL[147]); bufferR += (int)(dram->d[dCount+t[147]]*dram->outR[147]); 
				case 146: bufferL += (int)(dram->d[dCount+t[146]]*dram->outL[146]); bufferR += (int)(dram->d[dCount+t[146]]*dram->outR[146]); 
				case 145: bufferL += (int)(dram->d[dCount+t[145]]*dram->outL[145]); bufferR += (int)(dram->d[dCount+t[145]]*dram->outR[145]); 
				case 144: bufferL += (int)(dram->d[dCount+t[144]]*dram->outL[144]); bufferR += (int)(dram->d[dCount+t[144]]*dram->outR[144]); 
				case 143: bufferL += (int)(dram->d[dCount+t[143]]*dram->outL[143]); bufferR += (int)(dram->d[dCount+t[143]]*dram->outR[143]); 
				case 142: bufferL += (int)(dram->d[dCount+t[142]]*dram->outL[142]); bufferR += (int)(dram->d[dCount+t[142]]*dram->outR[142]); 
				case 141: bufferL += (int)(dram->d[dCount+t[141]]*dram->outL[141]); bufferR += (int)(dram->d[dCount+t[141]]*dram->outR[141]); 
				case 140: bufferL += (int)(dram->d[dCount+t[140]]*dram->outL[140]); bufferR += (int)(dram->d[dCount+t[140]]*dram->outR[140]); 
				case 139: bufferL += (int)(dram->d[dCount+t[139]]*dram->outL[139]); bufferR += (int)(dram->d[dCount+t[139]]*dram->outR[139]); 
				case 138: bufferL += (int)(dram->d[dCount+t[138]]*dram->outL[138]); bufferR += (int)(dram->d[dCount+t[138]]*dram->outR[138]); 
				case 137: bufferL += (int)(dram->d[dCount+t[137]]*dram->outL[137]); bufferR += (int)(dram->d[dCount+t[137]]*dram->outR[137]); 
				case 136: bufferL += (int)(dram->d[dCount+t[136]]*dram->outL[136]); bufferR += (int)(dram->d[dCount+t[136]]*dram->outR[136]); 
				case 135: bufferL += (int)(dram->d[dCount+t[135]]*dram->outL[135]); bufferR += (int)(dram->d[dCount+t[135]]*dram->outR[135]); 
				case 134: bufferL += (int)(dram->d[dCount+t[134]]*dram->outL[134]); bufferR += (int)(dram->d[dCount+t[134]]*dram->outR[134]); 
				case 133: bufferL += (int)(dram->d[dCount+t[133]]*dram->outL[133]); bufferR += (int)(dram->d[dCount+t[133]]*dram->outR[133]); 
				case 132: bufferL += (int)(dram->d[dCount+t[132]]*dram->outL[132]); bufferR += (int)(dram->d[dCount+t[132]]*dram->outR[132]); 
				case 131: bufferL += (int)(dram->d[dCount+t[131]]*dram->outL[131]); bufferR += (int)(dram->d[dCount+t[131]]*dram->outR[131]); 
				case 130: bufferL += (int)(dram->d[dCount+t[130]]*dram->outL[130]); bufferR += (int)(dram->d[dCount+t[130]]*dram->outR[130]); 
				case 129: bufferL += (int)(dram->d[dCount+t[129]]*dram->outL[129]); bufferR += (int)(dram->d[dCount+t[129]]*dram->outR[129]); 
				case 128: bufferL += (int)(dram->d[dCount+t[128]]*dram->outL[128]); bufferR += (int)(dram->d[dCount+t[128]]*dram->outR[128]); 
				case 127: bufferL += (int)(dram->d[dCount+t[127]]*dram->outL[127]); bufferR += (int)(dram->d[dCount+t[127]]*dram->outR[127]); 
				case 126: bufferL += (int)(dram->d[dCount+t[126]]*dram->outL[126]); bufferR += (int)(dram->d[dCount+t[126]]*dram->outR[126]); 
				case 125: bufferL += (int)(dram->d[dCount+t[125]]*dram->outL[125]); bufferR += (int)(dram->d[dCount+t[125]]*dram->outR[125]); 
				case 124: bufferL += (int)(dram->d[dCount+t[124]]*dram->outL[124]); bufferR += (int)(dram->d[dCount+t[124]]*dram->outR[124]); 
				case 123: bufferL += (int)(dram->d[dCount+t[123]]*dram->outL[123]); bufferR += (int)(dram->d[dCount+t[123]]*dram->outR[123]); 
				case 122: bufferL += (int)(dram->d[dCount+t[122]]*dram->outL[122]); bufferR += (int)(dram->d[dCount+t[122]]*dram->outR[122]); 
				case 121: bufferL += (int)(dram->d[dCount+t[121]]*dram->outL[121]); bufferR += (int)(dram->d[dCount+t[121]]*dram->outR[121]); 
				case 120: bufferL += (int)(dram->d[dCount+t[120]]*dram->outL[120]); bufferR += (int)(dram->d[dCount+t[120]]*dram->outR[120]); 
				case 119: bufferL += (int)(dram->d[dCount+t[119]]*dram->outL[119]); bufferR += (int)(dram->d[dCount+t[119]]*dram->outR[119]); 
				case 118: bufferL += (int)(dram->d[dCount+t[118]]*dram->outL[118]); bufferR += (int)(dram->d[dCount+t[118]]*dram->outR[118]); 
				case 117: bufferL += (int)(dram->d[dCount+t[117]]*dram->outL[117]); bufferR += (int)(dram->d[dCount+t[117]]*dram->outR[117]); 
				case 116: bufferL += (int)(dram->d[dCount+t[116]]*dram->outL[116]); bufferR += (int)(dram->d[dCount+t[116]]*dram->outR[116]); 
				case 115: bufferL += (int)(dram->d[dCount+t[115]]*dram->outL[115]); bufferR += (int)(dram->d[dCount+t[115]]*dram->outR[115]); 
				case 114: bufferL += (int)(dram->d[dCount+t[114]]*dram->outL[114]); bufferR += (int)(dram->d[dCount+t[114]]*dram->outR[114]); 
				case 113: bufferL += (int)(dram->d[dCount+t[113]]*dram->outL[113]); bufferR += (int)(dram->d[dCount+t[113]]*dram->outR[113]); 
				case 112: bufferL += (int)(dram->d[dCount+t[112]]*dram->outL[112]); bufferR += (int)(dram->d[dCount+t[112]]*dram->outR[112]); 
				case 111: bufferL += (int)(dram->d[dCount+t[111]]*dram->outL[111]); bufferR += (int)(dram->d[dCount+t[111]]*dram->outR[111]); 
				case 110: bufferL += (int)(dram->d[dCount+t[110]]*dram->outL[110]); bufferR += (int)(dram->d[dCount+t[110]]*dram->outR[110]); 
				case 109: bufferL += (int)(dram->d[dCount+t[109]]*dram->outL[109]); bufferR += (int)(dram->d[dCount+t[109]]*dram->outR[109]); 
				case 108: bufferL += (int)(dram->d[dCount+t[108]]*dram->outL[108]); bufferR += (int)(dram->d[dCount+t[108]]*dram->outR[108]); 
				case 107: bufferL += (int)(dram->d[dCount+t[107]]*dram->outL[107]); bufferR += (int)(dram->d[dCount+t[107]]*dram->outR[107]); 
				case 106: bufferL += (int)(dram->d[dCount+t[106]]*dram->outL[106]); bufferR += (int)(dram->d[dCount+t[106]]*dram->outR[106]); 
				case 105: bufferL += (int)(dram->d[dCount+t[105]]*dram->outL[105]); bufferR += (int)(dram->d[dCount+t[105]]*dram->outR[105]); 
				case 104: bufferL += (int)(dram->d[dCount+t[104]]*dram->outL[104]); bufferR += (int)(dram->d[dCount+t[104]]*dram->outR[104]); 
				case 103: bufferL += (int)(dram->d[dCount+t[103]]*dram->outL[103]); bufferR += (int)(dram->d[dCount+t[103]]*dram->outR[103]); 
				case 102: bufferL += (int)(dram->d[dCount+t[102]]*dram->outL[102]); bufferR += (int)(dram->d[dCount+t[102]]*dram->outR[102]); 
				case 101: bufferL += (int)(dram->d[dCount+t[101]]*dram->outL[101]); bufferR += (int)(dram->d[dCount+t[101]]*dram->outR[101]); 
				case 100: bufferL += (int)(dram->d[dCount+t[100]]*dram->outL[100]); bufferR += (int)(dram->d[dCount+t[100]]*dram->outR[100]); 
				case  99: bufferL += (int)(dram->d[dCount+t[ 99]]*dram->outL[ 99]); bufferR += (int)(dram->d[dCount+t[ 99]]*dram->outR[ 99]); 
				case  98: bufferL += (int)(dram->d[dCount+t[ 98]]*dram->outL[ 98]); bufferR += (int)(dram->d[dCount+t[ 98]]*dram->outR[ 98]); 
				case  97: bufferL += (int)(dram->d[dCount+t[ 97]]*dram->outL[ 97]); bufferR += (int)(dram->d[dCount+t[ 97]]*dram->outR[ 97]); 
				case  96: bufferL += (int)(dram->d[dCount+t[ 96]]*dram->outL[ 96]); bufferR += (int)(dram->d[dCount+t[ 96]]*dram->outR[ 96]); 
				case  95: bufferL += (int)(dram->d[dCount+t[ 95]]*dram->outL[ 95]); bufferR += (int)(dram->d[dCount+t[ 95]]*dram->outR[ 95]); 
				case  94: bufferL += (int)(dram->d[dCount+t[ 94]]*dram->outL[ 94]); bufferR += (int)(dram->d[dCount+t[ 94]]*dram->outR[ 94]); 
				case  93: bufferL += (int)(dram->d[dCount+t[ 93]]*dram->outL[ 93]); bufferR += (int)(dram->d[dCount+t[ 93]]*dram->outR[ 93]); 
				case  92: bufferL += (int)(dram->d[dCount+t[ 92]]*dram->outL[ 92]); bufferR += (int)(dram->d[dCount+t[ 92]]*dram->outR[ 92]); 
				case  91: bufferL += (int)(dram->d[dCount+t[ 91]]*dram->outL[ 91]); bufferR += (int)(dram->d[dCount+t[ 91]]*dram->outR[ 91]); 
				case  90: bufferL += (int)(dram->d[dCount+t[ 90]]*dram->outL[ 90]); bufferR += (int)(dram->d[dCount+t[ 90]]*dram->outR[ 90]); 
				case  89: bufferL += (int)(dram->d[dCount+t[ 89]]*dram->outL[ 89]); bufferR += (int)(dram->d[dCount+t[ 89]]*dram->outR[ 89]); 
				case  88: bufferL += (int)(dram->d[dCount+t[ 88]]*dram->outL[ 88]); bufferR += (int)(dram->d[dCount+t[ 88]]*dram->outR[ 88]); 
				case  87: bufferL += (int)(dram->d[dCount+t[ 87]]*dram->outL[ 87]); bufferR += (int)(dram->d[dCount+t[ 87]]*dram->outR[ 87]); 
				case  86: bufferL += (int)(dram->d[dCount+t[ 86]]*dram->outL[ 86]); bufferR += (int)(dram->d[dCount+t[ 86]]*dram->outR[ 86]); 
				case  85: bufferL += (int)(dram->d[dCount+t[ 85]]*dram->outL[ 85]); bufferR += (int)(dram->d[dCount+t[ 85]]*dram->outR[ 85]); 
				case  84: bufferL += (int)(dram->d[dCount+t[ 84]]*dram->outL[ 84]); bufferR += (int)(dram->d[dCount+t[ 84]]*dram->outR[ 84]); 
				case  83: bufferL += (int)(dram->d[dCount+t[ 83]]*dram->outL[ 83]); bufferR += (int)(dram->d[dCount+t[ 83]]*dram->outR[ 83]); 
				case  82: bufferL += (int)(dram->d[dCount+t[ 82]]*dram->outL[ 82]); bufferR += (int)(dram->d[dCount+t[ 82]]*dram->outR[ 82]); 
				case  81: bufferL += (int)(dram->d[dCount+t[ 81]]*dram->outL[ 81]); bufferR += (int)(dram->d[dCount+t[ 81]]*dram->outR[ 81]); 
				case  80: bufferL += (int)(dram->d[dCount+t[ 80]]*dram->outL[ 80]); bufferR += (int)(dram->d[dCount+t[ 80]]*dram->outR[ 80]); 
				case  79: bufferL += (int)(dram->d[dCount+t[ 79]]*dram->outL[ 79]); bufferR += (int)(dram->d[dCount+t[ 79]]*dram->outR[ 79]); 
				case  78: bufferL += (int)(dram->d[dCount+t[ 78]]*dram->outL[ 78]); bufferR += (int)(dram->d[dCount+t[ 78]]*dram->outR[ 78]); 
				case  77: bufferL += (int)(dram->d[dCount+t[ 77]]*dram->outL[ 77]); bufferR += (int)(dram->d[dCount+t[ 77]]*dram->outR[ 77]); 
				case  76: bufferL += (int)(dram->d[dCount+t[ 76]]*dram->outL[ 76]); bufferR += (int)(dram->d[dCount+t[ 76]]*dram->outR[ 76]); 
				case  75: bufferL += (int)(dram->d[dCount+t[ 75]]*dram->outL[ 75]); bufferR += (int)(dram->d[dCount+t[ 75]]*dram->outR[ 75]); 
				case  74: bufferL += (int)(dram->d[dCount+t[ 74]]*dram->outL[ 74]); bufferR += (int)(dram->d[dCount+t[ 74]]*dram->outR[ 74]); 
				case  73: bufferL += (int)(dram->d[dCount+t[ 73]]*dram->outL[ 73]); bufferR += (int)(dram->d[dCount+t[ 73]]*dram->outR[ 73]); 
				case  72: bufferL += (int)(dram->d[dCount+t[ 72]]*dram->outL[ 72]); bufferR += (int)(dram->d[dCount+t[ 72]]*dram->outR[ 72]); 
				case  71: bufferL += (int)(dram->d[dCount+t[ 71]]*dram->outL[ 71]); bufferR += (int)(dram->d[dCount+t[ 71]]*dram->outR[ 71]); 
				case  70: bufferL += (int)(dram->d[dCount+t[ 70]]*dram->outL[ 70]); bufferR += (int)(dram->d[dCount+t[ 70]]*dram->outR[ 70]); 
				case  69: bufferL += (int)(dram->d[dCount+t[ 69]]*dram->outL[ 69]); bufferR += (int)(dram->d[dCount+t[ 69]]*dram->outR[ 69]); 
				case  68: bufferL += (int)(dram->d[dCount+t[ 68]]*dram->outL[ 68]); bufferR += (int)(dram->d[dCount+t[ 68]]*dram->outR[ 68]); 
				case  67: bufferL += (int)(dram->d[dCount+t[ 67]]*dram->outL[ 67]); bufferR += (int)(dram->d[dCount+t[ 67]]*dram->outR[ 67]); 
				case  66: bufferL += (int)(dram->d[dCount+t[ 66]]*dram->outL[ 66]); bufferR += (int)(dram->d[dCount+t[ 66]]*dram->outR[ 66]); 
				case  65: bufferL += (int)(dram->d[dCount+t[ 65]]*dram->outL[ 65]); bufferR += (int)(dram->d[dCount+t[ 65]]*dram->outR[ 65]); 
				case  64: bufferL += (int)(dram->d[dCount+t[ 64]]*dram->outL[ 64]); bufferR += (int)(dram->d[dCount+t[ 64]]*dram->outR[ 64]); 
				case  63: bufferL += (int)(dram->d[dCount+t[ 63]]*dram->outL[ 63]); bufferR += (int)(dram->d[dCount+t[ 63]]*dram->outR[ 63]); 
				case  62: bufferL += (int)(dram->d[dCount+t[ 62]]*dram->outL[ 62]); bufferR += (int)(dram->d[dCount+t[ 62]]*dram->outR[ 62]); 
				case  61: bufferL += (int)(dram->d[dCount+t[ 61]]*dram->outL[ 61]); bufferR += (int)(dram->d[dCount+t[ 61]]*dram->outR[ 61]); 
				case  60: bufferL += (int)(dram->d[dCount+t[ 60]]*dram->outL[ 60]); bufferR += (int)(dram->d[dCount+t[ 60]]*dram->outR[ 60]); 
				case  59: bufferL += (int)(dram->d[dCount+t[ 59]]*dram->outL[ 59]); bufferR += (int)(dram->d[dCount+t[ 59]]*dram->outR[ 59]); 
				case  58: bufferL += (int)(dram->d[dCount+t[ 58]]*dram->outL[ 58]); bufferR += (int)(dram->d[dCount+t[ 58]]*dram->outR[ 58]); 
				case  57: bufferL += (int)(dram->d[dCount+t[ 57]]*dram->outL[ 57]); bufferR += (int)(dram->d[dCount+t[ 57]]*dram->outR[ 57]); 
				case  56: bufferL += (int)(dram->d[dCount+t[ 56]]*dram->outL[ 56]); bufferR += (int)(dram->d[dCount+t[ 56]]*dram->outR[ 56]); 
				case  55: bufferL += (int)(dram->d[dCount+t[ 55]]*dram->outL[ 55]); bufferR += (int)(dram->d[dCount+t[ 55]]*dram->outR[ 55]); 
				case  54: bufferL += (int)(dram->d[dCount+t[ 54]]*dram->outL[ 54]); bufferR += (int)(dram->d[dCount+t[ 54]]*dram->outR[ 54]); 
				case  53: bufferL += (int)(dram->d[dCount+t[ 53]]*dram->outL[ 53]); bufferR += (int)(dram->d[dCount+t[ 53]]*dram->outR[ 53]); 
				case  52: bufferL += (int)(dram->d[dCount+t[ 52]]*dram->outL[ 52]); bufferR += (int)(dram->d[dCount+t[ 52]]*dram->outR[ 52]); 
				case  51: bufferL += (int)(dram->d[dCount+t[ 51]]*dram->outL[ 51]); bufferR += (int)(dram->d[dCount+t[ 51]]*dram->outR[ 51]); 
				case  50: bufferL += (int)(dram->d[dCount+t[ 50]]*dram->outL[ 50]); bufferR += (int)(dram->d[dCount+t[ 50]]*dram->outR[ 50]); 
				case  49: bufferL += (int)(dram->d[dCount+t[ 49]]*dram->outL[ 49]); bufferR += (int)(dram->d[dCount+t[ 49]]*dram->outR[ 49]); 
				case  48: bufferL += (int)(dram->d[dCount+t[ 48]]*dram->outL[ 48]); bufferR += (int)(dram->d[dCount+t[ 48]]*dram->outR[ 48]); 
				case  47: bufferL += (int)(dram->d[dCount+t[ 47]]*dram->outL[ 47]); bufferR += (int)(dram->d[dCount+t[ 47]]*dram->outR[ 47]); 
				case  46: bufferL += (int)(dram->d[dCount+t[ 46]]*dram->outL[ 46]); bufferR += (int)(dram->d[dCount+t[ 46]]*dram->outR[ 46]); 
				case  45: bufferL += (int)(dram->d[dCount+t[ 45]]*dram->outL[ 45]); bufferR += (int)(dram->d[dCount+t[ 45]]*dram->outR[ 45]); 
				case  44: bufferL += (int)(dram->d[dCount+t[ 44]]*dram->outL[ 44]); bufferR += (int)(dram->d[dCount+t[ 44]]*dram->outR[ 44]); 
				case  43: bufferL += (int)(dram->d[dCount+t[ 43]]*dram->outL[ 43]); bufferR += (int)(dram->d[dCount+t[ 43]]*dram->outR[ 43]); 
				case  42: bufferL += (int)(dram->d[dCount+t[ 42]]*dram->outL[ 42]); bufferR += (int)(dram->d[dCount+t[ 42]]*dram->outR[ 42]); 
				case  41: bufferL += (int)(dram->d[dCount+t[ 41]]*dram->outL[ 41]); bufferR += (int)(dram->d[dCount+t[ 41]]*dram->outR[ 41]); 
				case  40: bufferL += (int)(dram->d[dCount+t[ 40]]*dram->outL[ 40]); bufferR += (int)(dram->d[dCount+t[ 40]]*dram->outR[ 40]); 
				case  39: bufferL += (int)(dram->d[dCount+t[ 39]]*dram->outL[ 39]); bufferR += (int)(dram->d[dCount+t[ 39]]*dram->outR[ 39]); 
				case  38: bufferL += (int)(dram->d[dCount+t[ 38]]*dram->outL[ 38]); bufferR += (int)(dram->d[dCount+t[ 38]]*dram->outR[ 38]); 
				case  37: bufferL += (int)(dram->d[dCount+t[ 37]]*dram->outL[ 37]); bufferR += (int)(dram->d[dCount+t[ 37]]*dram->outR[ 37]); 
				case  36: bufferL += (int)(dram->d[dCount+t[ 36]]*dram->outL[ 36]); bufferR += (int)(dram->d[dCount+t[ 36]]*dram->outR[ 36]); 
				case  35: bufferL += (int)(dram->d[dCount+t[ 35]]*dram->outL[ 35]); bufferR += (int)(dram->d[dCount+t[ 35]]*dram->outR[ 35]); 
				case  34: bufferL += (int)(dram->d[dCount+t[ 34]]*dram->outL[ 34]); bufferR += (int)(dram->d[dCount+t[ 34]]*dram->outR[ 34]); 
				case  33: bufferL += (int)(dram->d[dCount+t[ 33]]*dram->outL[ 33]); bufferR += (int)(dram->d[dCount+t[ 33]]*dram->outR[ 33]); 
				case  32: bufferL += (int)(dram->d[dCount+t[ 32]]*dram->outL[ 32]); bufferR += (int)(dram->d[dCount+t[ 32]]*dram->outR[ 32]); 
				case  31: bufferL += (int)(dram->d[dCount+t[ 31]]*dram->outL[ 31]); bufferR += (int)(dram->d[dCount+t[ 31]]*dram->outR[ 31]); 
				case  30: bufferL += (int)(dram->d[dCount+t[ 30]]*dram->outL[ 30]); bufferR += (int)(dram->d[dCount+t[ 30]]*dram->outR[ 30]);
				case  29: bufferL += (int)(dram->d[dCount+t[ 29]]*dram->outL[ 29]); bufferR += (int)(dram->d[dCount+t[ 29]]*dram->outR[ 29]); 
				case  28: bufferL += (int)(dram->d[dCount+t[ 28]]*dram->outL[ 28]); bufferR += (int)(dram->d[dCount+t[ 28]]*dram->outR[ 28]); 
				case  27: bufferL += (int)(dram->d[dCount+t[ 27]]*dram->outL[ 27]); bufferR += (int)(dram->d[dCount+t[ 27]]*dram->outR[ 27]); 
				case  26: bufferL += (int)(dram->d[dCount+t[ 26]]*dram->outL[ 26]); bufferR += (int)(dram->d[dCount+t[ 26]]*dram->outR[ 26]); 
				case  25: bufferL += (int)(dram->d[dCount+t[ 25]]*dram->outL[ 25]); bufferR += (int)(dram->d[dCount+t[ 25]]*dram->outR[ 25]); 
				case  24: bufferL += (int)(dram->d[dCount+t[ 24]]*dram->outL[ 24]); bufferR += (int)(dram->d[dCount+t[ 24]]*dram->outR[ 24]); 
				case  23: bufferL += (int)(dram->d[dCount+t[ 23]]*dram->outL[ 23]); bufferR += (int)(dram->d[dCount+t[ 23]]*dram->outR[ 23]); 
				case  22: bufferL += (int)(dram->d[dCount+t[ 22]]*dram->outL[ 22]); bufferR += (int)(dram->d[dCount+t[ 22]]*dram->outR[ 22]); 
				case  21: bufferL += (int)(dram->d[dCount+t[ 21]]*dram->outL[ 21]); bufferR += (int)(dram->d[dCount+t[ 21]]*dram->outR[ 21]); 
				case  20: bufferL += (int)(dram->d[dCount+t[ 20]]*dram->outL[ 20]); bufferR += (int)(dram->d[dCount+t[ 20]]*dram->outR[ 20]); 
				case  19: bufferL += (int)(dram->d[dCount+t[ 19]]*dram->outL[ 19]); bufferR += (int)(dram->d[dCount+t[ 19]]*dram->outR[ 19]); 
				case  18: bufferL += (int)(dram->d[dCount+t[ 18]]*dram->outL[ 18]); bufferR += (int)(dram->d[dCount+t[ 18]]*dram->outR[ 18]); 
				case  17: bufferL += (int)(dram->d[dCount+t[ 17]]*dram->outL[ 17]); bufferR += (int)(dram->d[dCount+t[ 17]]*dram->outR[ 17]); 
				case  16: bufferL += (int)(dram->d[dCount+t[ 16]]*dram->outL[ 16]); bufferR += (int)(dram->d[dCount+t[ 16]]*dram->outR[ 16]); 
				case  15: bufferL += (int)(dram->d[dCount+t[ 15]]*dram->outL[ 15]); bufferR += (int)(dram->d[dCount+t[ 15]]*dram->outR[ 15]); 
				case  14: bufferL += (int)(dram->d[dCount+t[ 14]]*dram->outL[ 14]); bufferR += (int)(dram->d[dCount+t[ 14]]*dram->outR[ 14]); 
				case  13: bufferL += (int)(dram->d[dCount+t[ 13]]*dram->outL[ 13]); bufferR += (int)(dram->d[dCount+t[ 13]]*dram->outR[ 13]); 
				case  12: bufferL += (int)(dram->d[dCount+t[ 12]]*dram->outL[ 12]); bufferR += (int)(dram->d[dCount+t[ 12]]*dram->outR[ 12]); 
				case  11: bufferL += (int)(dram->d[dCount+t[ 11]]*dram->outL[ 11]); bufferR += (int)(dram->d[dCount+t[ 11]]*dram->outR[ 11]); 
				case  10: bufferL += (int)(dram->d[dCount+t[ 10]]*dram->outL[ 10]); bufferR += (int)(dram->d[dCount+t[ 10]]*dram->outR[ 10]); 
				case   9: bufferL += (int)(dram->d[dCount+t[  9]]*dram->outL[  9]); bufferR += (int)(dram->d[dCount+t[  9]]*dram->outR[  9]); 
				case   8: bufferL += (int)(dram->d[dCount+t[  8]]*dram->outL[  8]); bufferR += (int)(dram->d[dCount+t[  8]]*dram->outR[  8]); 
				case   7: bufferL += (int)(dram->d[dCount+t[  7]]*dram->outL[  7]); bufferR += (int)(dram->d[dCount+t[  7]]*dram->outR[  7]); 
				case   6: bufferL += (int)(dram->d[dCount+t[  6]]*dram->outL[  6]); bufferR += (int)(dram->d[dCount+t[  6]]*dram->outR[  6]); 
				case   5: bufferL += (int)(dram->d[dCount+t[  5]]*dram->outL[  5]); bufferR += (int)(dram->d[dCount+t[  5]]*dram->outR[  5]); 
				case   4: bufferL += (int)(dram->d[dCount+t[  4]]*dram->outL[  4]); bufferR += (int)(dram->d[dCount+t[  4]]*dram->outR[  4]);
				case   3: bufferL += (int)(dram->d[dCount+t[  3]]*dram->outL[  3]); bufferR += (int)(dram->d[dCount+t[  3]]*dram->outR[  3]);
				case   2: bufferL += (int)(dram->d[dCount+t[  2]]*dram->outL[  2]); bufferR += (int)(dram->d[dCount+t[  2]]*dram->outR[  2]);
				case   1: bufferL += (int)(dram->d[dCount+t[  1]]*dram->outL[  1]); bufferR += (int)(dram->d[dCount+t[  1]]*dram->outR[  1]);
			}
			//test to see that delay is working at all: it will be a big stack of case with no break
			
			inputSampleL = bufferL;
			inputSampleR = bufferR;
			//scale back the reverb buffers based on how big of a range we used
			
			
			wearR[9] = wearR[8]; wearR[8] = wearR[7]; wearR[7] = wearR[6]; wearR[6] = wearR[5];
			wearR[5] = wearR[4]; wearR[4] = wearR[3]; wearR[3] = wearR[2]; wearR[2] = wearR[1];
			wearR[1] = wearR[0]; wearR[0] = accumulatorSample = (inputSampleR-wearRPrev);
			
			accumulatorSample *= factor[0];
			accumulatorSample += (wearR[1] * factor[1]);
			accumulatorSample += (wearR[2] * factor[2]);
			accumulatorSample += (wearR[3] * factor[3]);
			accumulatorSample += (wearR[4] * factor[4]);
			accumulatorSample += (wearR[5] * factor[5]);
			accumulatorSample += (wearR[6] * factor[6]);
			accumulatorSample += (wearR[7] * factor[7]);
			accumulatorSample += (wearR[8] * factor[8]);
			accumulatorSample += (wearR[9] * factor[9]);
			//we are doing our repetitive calculations on a separate value
			correction = (inputSampleR-wearRPrev) + accumulatorSample;
			wearRPrev = inputSampleR;		
			inputSampleR += correction;
			
			wearL[9] = wearL[8]; wearL[8] = wearL[7]; wearL[7] = wearL[6]; wearL[6] = wearL[5];
			wearL[5] = wearL[4]; wearL[4] = wearL[3]; wearL[3] = wearL[2]; wearL[2] = wearL[1];
			wearL[1] = wearL[0]; wearL[0] = accumulatorSample = (inputSampleL-wearLPrev);
			
			accumulatorSample *= factor[0];
			accumulatorSample += (wearL[1] * factor[1]);
			accumulatorSample += (wearL[2] * factor[2]);
			accumulatorSample += (wearL[3] * factor[3]);
			accumulatorSample += (wearL[4] * factor[4]);
			accumulatorSample += (wearL[5] * factor[5]);
			accumulatorSample += (wearL[6] * factor[6]);
			accumulatorSample += (wearL[7] * factor[7]);
			accumulatorSample += (wearL[8] * factor[8]);
			accumulatorSample += (wearL[9] * factor[9]);
			//we are doing our repetitive calculations on a separate value		
			correction = (inputSampleL-wearLPrev) + accumulatorSample;
			wearLPrev = inputSampleL;		
			inputSampleL += correction;
			//completed Groove Wear section
			
			inputSampleL /= outputPad;
			inputSampleR /= outputPad;
			
			
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
		
		
		//back to previous plugin
		drySampleL *= dryness;
		drySampleR *= dryness;
		
		inputSampleL *= wetness;
		inputSampleR *= wetness;
		
		inputSampleL += drySampleL;
		inputSampleR += drySampleR;
		//here we combine the tanks with the dry signal
		
		
		
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//here we mix the reverb output with the dry input
		
		inputL += 1; inputR += 1; outputL += 1; outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	int count;
	
	for(count = 0; count < 44101; count++) {dram->d[count] = 0.0;}
	dCount = 0;
	
	for(count = 0; count < 11; count++) {wearL[count] = 0.0; wearR[count] = 0.0; factor[count] = 0.0;}
	
	wearLPrev = 0.0; wearRPrev = 0.0;
	
	for(int count = 0; count < 6; count++) {lastRefL[count] = 0.0; lastRefR[count] = 0.0;}
	cycle = 0;
	
	p[0] = 1;
	p[1] = 11; p[2] = 13; p[3] = 17; p[4] = 19; p[5] = 23; p[6] = 29; p[7] = 31; p[8] = 37; p[9] = 41;
	p[10] = 43; p[11] = 47; p[12] = 53; p[13] = 59; p[14] = 61; p[15] = 67; p[16] = 71; p[17] = 73; p[18] = 79; p[19] = 83; p[20] = 89;
	p[21] = 97; p[22] = 101; p[23] = 103; p[24] = 107; p[25] = 109; p[26] = 113; p[27] = 127; p[28] = 131; p[29] = 137; p[30] = 139;
	p[31] = 149; p[32] = 151; p[33] = 157; p[34] = 163; p[35] = 167; p[36] = 173; p[37] = 179; p[38] = 181; p[39] = 191; p[40] = 193;
	p[41] = 197; p[42] = 199; p[43] = 211; p[44] = 223; p[45] = 227; p[46] = 229; p[47] = 233; p[48] = 239; p[49] = 241; p[50] = 251;
	p[51] = 257; p[52] = 263; p[53] = 269; p[54] = 271; p[55] = 277; p[56] = 281; p[57] = 283; p[58] = 293; p[59] = 307; p[60] = 311;
	p[61] = 313; p[62] = 317; p[63] = 331; p[64] = 337; p[65] = 347; p[66] = 349; p[67] = 353; p[68] = 359; p[69] = 367; p[70] = 373;
	p[71] = 379; p[72] = 383; p[73] = 389; p[74] = 397; p[75] = 401; p[76] = 409; p[77] = 419; p[78] = 421; p[79] = 431; p[80] = 433;
	p[81] = 439; p[82] = 443; p[83] = 449; p[84] = 457; p[85] = 461; p[86] = 463; p[87] = 467; p[88] = 479; p[89] = 487; p[90] = 491;
	p[91] = 499; p[92] = 503; p[93] = 509; p[94] = 521; p[95] = 523; p[96] = 541; p[97] = 547; p[98] = 557; p[99] = 563; p[100] = 569;
	p[101] = 571; p[102] = 577; p[103] = 587; p[104] = 593; p[105] = 599; p[106] = 601; p[107] = 607; p[108] = 613; p[109] = 617; p[110] = 619;
	p[111] = 631; p[112] = 641; p[113] = 643; p[114] = 647; p[115] = 653; p[116] = 659; p[117] = 661; p[118] = 673; p[119] = 677; p[120] = 683;
	p[121] = 691; p[122] = 701; p[123] = 709; p[124] = 719; p[125] = 727; p[126] = 733; p[127] = 739; p[128] = 743; p[129] = 751; p[130] = 757;
	p[131] = 761; p[132] = 769; p[133] = 773; p[134] = 787; p[135] = 797; p[136] = 809; p[137] = 811; p[138] = 821; p[139] = 823; p[140] = 827;
	p[141] = 829; p[142] = 839; p[143] = 853; p[144] = 857; p[145] = 859; p[146] = 863; p[147] = 877; p[148] = 881; p[149] = 883; p[150] = 887;
	p[151] = 907; p[152] = 911; p[153] = 919; p[154] = 929; p[155] = 937; p[156] = 941; p[157] = 947; p[158] = 953; p[159] = 967; p[160] = 971;
	p[161] = 977; p[162] = 983; p[163] = 991; p[164] = 997; p[165] = 998; p[166] = 999;
	
	int assign;
	for(count = 0; count < 165; count++)
	{
		t[count] = p[count];
		//these get assigned again but we'll give them real values in case of trouble. They are 32 bit unsigned ints
		assign = p[count] % 10;
		//give us the 1, 3, 7 or 9 on the end
		
		switch (assign){
			case 1: dram->outL[count] = 0.0; dram->outR[count] = p[count]; break;
			case 3: dram->outL[count] = p[count] * 0.25; dram->outR[count] = p[count] * 0.75; break;
			case 7: dram->outL[count] = p[count] * 0.75; dram->outR[count] = p[count] * 0.25; break;
			case 9: dram->outL[count] = p[count]; dram->outR[count] = 0.0; break;
		}
		//this gives us a set of stereo offsets that are always the same. When building our delay outputs,
		//we multiply our -1 to 1 float values by this and add the result to a simple signed int.
		//The result gives us a coarser and coarser output the fewer taps we have,
		//and we divide the result by count*count to drop the volume down again.
	}
	pitchCounter = 2;
	increment = 1;
	dutyCycle = 1;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
