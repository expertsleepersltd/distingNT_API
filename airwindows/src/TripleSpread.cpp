#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "TripleSpread"
#define AIRWINDOWS_DESCRIPTION "A stereo tripler with extra wideness and GlitchShifter processing."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','r','d' )
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
{ .name = "Spread", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tightness", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	int pL[131076];
	int offsetL[258];
	int pastzeroL[258];
	int previousL[258];
	int thirdL[258];
	int fourthL[258];
	int tempL;
	int lasttempL;
	int thirdtempL;
	int fourthtempL;
	int sincezerocrossL;
	int crossesL;
	int realzeroesL;
	float positionL;
	bool splicingL;
	
	float airPrevL;
	float airEvenL;
	float airOddL;
	float airFactorL;
	
	int pR[131076];
	int offsetR[258];
	int pastzeroR[258];
	int previousR[258];
	int thirdR[258];
	int fourthR[258];
	int tempR;
	int lasttempR;
	int thirdtempR;
	int fourthtempR;
	int sincezerocrossR;
	int crossesR;
	int realzeroesR;
	float positionR;
	bool splicingR;
	
	float airPrevR;
	float airEvenR;
	float airOddR;
	float airFactorR;
	
	int gcount;
	int lastwidth;
	bool flip;
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	
	Float32 speed = GetParameter( kParam_One )*0.041666666666667f;
	SInt32 width = (SInt32)(65536-((1-powf(1-GetParameter( kParam_Two ),2))*65530.0f));
	Float32 bias = powf(GetParameter( kParam_Two ),3);
	Float32 wet = GetParameter( kParam_Three );
	
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		airFactorL = airPrevL - inputSampleL;
		if (flip) {airEvenL += airFactorL; airOddL -= airFactorL; airFactorL = airEvenL;}
		else {airOddL += airFactorL; airEvenL -= airFactorL; airFactorL = airOddL;}
		airOddL = (airOddL - ((airOddL - airEvenL)/256.0f)) / 1.0001f;
		airEvenL = (airEvenL - ((airEvenL - airOddL)/256.0f)) / 1.0001f;
		airPrevL = inputSampleL;
		inputSampleL += airFactorL;
		
		airFactorR = airPrevR - inputSampleR;
		if (flip) {airEvenR += airFactorR; airOddR -= airFactorR; airFactorR = airEvenR;}
		else {airOddR += airFactorR; airEvenR -= airFactorR; airFactorR = airOddR;}
		airOddR = (airOddR - ((airOddR - airEvenR)/256.0f)) / 1.0001f;
		airEvenR = (airEvenR - ((airEvenR - airOddR)/256.0f)) / 1.0001f;
		airPrevR = inputSampleR;
		inputSampleR += airFactorR;
		
		flip = !flip;
		//air, compensates for loss of highs of interpolation
		
		if (lastwidth != width){crossesL = 0; realzeroesL = 0; crossesR = 0; realzeroesR = 0; lastwidth = width;}
		//global: changing this resets both channels
		
		gcount++;
		if (gcount < 0 || gcount > width) {gcount = 0;}
		int count = gcount;
		int countone = count-1;
		int counttwo = count-2;
		while (count < 0){count += width;}
		while (countone < 0){countone += width;}
		while (counttwo < 0){counttwo += width;}
		while (count > width){count -= width;} //this can only happen with very insane variables
		while (countone > width){countone -= width;}
		while (counttwo > width){counttwo -= width;}
		//yay sanity checks
		//now we have counts zero, one, two, none of which have sanity checked values
		//we are tracking most recent samples and must SUBTRACT.
		//this is a wrap on the overall buffers, so count, one and two are also common to both channels
		
		pL[count+width] = pL[count] = (int)((inputSampleL*8388352.0f));
		pR[count+width] = pR[count] = (int)((inputSampleR*8388352.0f));
		//float buffer -8388352 to 8388352 is equal to 24 bit linear space
		
		if ((pL[countone] > 0 && pL[count] < 0)||(pL[countone] < 0 && pL[count] > 0)) //source crossed zero
		{
			crossesL++;
			realzeroesL++;
			if (crossesL > 256) {crossesL = 0;} //wrap crosses to keep adding new crosses
			if (realzeroesL > 256) {realzeroesL = 256;} //don't wrap realzeroes, full buffer, use all
			offsetL[crossesL] = count;
			pastzeroL[crossesL] = pL[count];
			previousL[crossesL] = pL[countone];
			thirdL[crossesL] = pL[counttwo];
			//we load the zero crosses register with crosses to examine
		} //we just put in a source zero cross in the registry
		
		if ((pR[countone] > 0 && pR[count] < 0)||(pR[countone] < 0 && pR[count] > 0)) //source crossed zero
		{
			crossesR++;
			realzeroesR++;
			if (crossesR > 256) {crossesR = 0;} //wrap crosses to keep adding new crosses
			if (realzeroesR > 256) {realzeroesR = 256;} //don't wrap realzeroes, full buffer, use all
			offsetR[crossesR] = count;
			pastzeroR[crossesR] = pR[count];
			previousR[crossesR] = pR[countone];
			thirdR[crossesR] = pR[counttwo];
			//we load the zero crosses register with crosses to examine
		} //we just put in a source zero cross in the registry
		//in this we don't update count at all, so we can run them one after another because this is
		//feeding the system, not tracking the output of two parallel but non-matching output taps
		
		positionL -= speed; //this is individual to each channel!
		
		if (positionL > width) { //we just caught up to the buffer end
			if (realzeroesL > 0) { //we just caught up to the buffer end with zero crosses in the bin
				positionL = 0;
				float diff = 99999999.0f; 
				int best = 0; //these can be local, I think
				int scan;
				for(scan = (realzeroesL-1); scan >= 0; scan--) {
					int scanone = scan + crossesL;
					if (scanone > 256){scanone -= 256;}
					//try to track the real most recent ones more closely
					float howdiff = (float)((tempL - pastzeroL[scanone]) + (lasttempL - previousL[scanone]) + (thirdtempL - thirdL[scanone]) + (fourthtempL - fourthL[scanone]));
					//got difference factor between things
					howdiff -= (float)(scan*bias);
					//try to bias in favor of more recent crosses
					if (howdiff < diff) {diff = howdiff; best = scanone;}
				} //now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
				positionL = offsetL[best]-sincezerocrossL;
				crossesL = 0;
				realzeroesL = 0;
				splicingL = true; //we just kicked the delay tap back, changing positionL
			} else { //we just caught up to the buffer end with no crosses- glitch speeds.
				positionL -= width;
				crossesL = 0;
				realzeroesL = 0;
				splicingL = true; //so, hard splice it.
			}
		}
		
		if (positionL < 0) { //we just caught up to the dry tap.
			if (realzeroesL > 0) { //we just caught up to the dry tap with zero crosses in the bin
				positionL = 0;
				float diff = 99999999.0f; 
				int best = 0; //these can be local, I think
				int scan;
				for(scan = (realzeroesL-1); scan >= 0; scan--) {
					int scanone = scan + crossesL;
					if (scanone > 256){scanone -= 256;}
					//try to track the real most recent ones more closely
					float howdiff = (float)((tempL - pastzeroL[scanone]) + (lasttempL - previousL[scanone]) + (thirdtempL - thirdL[scanone]) + (fourthtempL - fourthL[scanone]));
					//got difference factor between things
					howdiff -= (float)(scan*bias);
					//try to bias in favor of more recent crosses
					if (howdiff < diff) {diff = howdiff; best = scanone;}
				} //now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
				positionL = offsetL[best]-sincezerocrossL;
				crossesL = 0;
				realzeroesL = 0;
				splicingL = true; //we just kicked the delay tap back, changing positionL
			} else { //we just caught up to the dry tap with no crosses- glitch speeds.
				positionL += width;
				crossesL = 0;
				realzeroesL = 0;
				splicingL = true; //so, hard splice it.
			}
		}
		
		positionR += speed; //this is individual to each channel and inverted for R!
		
		if (positionR > width) { //we just caught up to the buffer end
			if (realzeroesR > 0) { //we just caught up to the buffer end with zero crosses in the bin
				positionR = 0;
				float diff = 99999999.0f; 
				int best = 0; //these can be local, I think
				int scan;
				for(scan = (realzeroesR-1); scan >= 0; scan--) {
					int scanone = scan + crossesR;
					if (scanone > 256){scanone -= 256;}
					//try to track the real most recent ones more closely
					float howdiff = (float)((tempR - pastzeroR[scanone]) + (lasttempR - previousR[scanone]) + (thirdtempR - thirdR[scanone]) + (fourthtempR - fourthR[scanone]));
					//got difference factor between things
					howdiff -= (float)(scan*bias);
					//try to bias in favor of more recent crosses
					if (howdiff < diff) {diff = howdiff; best = scanone;}
				} //now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
				positionR = offsetR[best]-sincezerocrossR;
				crossesR = 0;
				realzeroesR = 0;
				splicingR = true; //we just kicked the delay tap back, changing positionL
			} else { //we just caught up to the buffer end with no crosses- glitch speeds.
				positionR -= width;
				crossesR = 0;
				realzeroesR = 0;
				splicingR = true; //so, hard splice it.
			}
		}
		
		if (positionR < 0) { //we just caught up to the dry tap.
			if (realzeroesR > 0) { //we just caught up to the dry tap with zero crosses in the bin
				positionR = 0;
				float diff = 99999999.0f; 
				int best = 0; //these can be local, I think
				int scan;
				for(scan = (realzeroesR-1); scan >= 0; scan--) {
					int scanone = scan + crossesR;
					if (scanone > 256){scanone -= 256;}
					//try to track the real most recent ones more closely
					float howdiff = (float)((tempR - pastzeroR[scanone]) + (lasttempR - previousR[scanone]) + (thirdtempR - thirdR[scanone]) + (fourthtempR - fourthR[scanone]));
					//got difference factor between things
					howdiff -= (float)(scan*bias);
					//try to bias in favor of more recent crosses
					if (howdiff < diff) {diff = howdiff; best = scanone;}
				} //now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
				positionR = offsetR[best]-sincezerocrossR;
				crossesR = 0;
				realzeroesR = 0;
				splicingR = true; //we just kicked the delay tap back, changing positionL
			} else { //we just caught up to the dry tap with no crosses- glitch speeds.
				positionR += width;
				crossesR = 0;
				realzeroesR = 0;
				splicingR = true; //so, hard splice it.
			}
		}
		
		
		count = gcount - (int)floor(positionL);
		//we go back because the buffer goes forward this time
		countone = count+1; 
		counttwo = count+2;
		//now we have counts zero, one, two, none of which have sanity checked values
		//we are interpolating, we ADD
		while (count < 0){count += width;}
		while (countone < 0){countone += width;}
		while (counttwo < 0){counttwo += width;}
		while (count > width){count -= width;} //this can only happen with very insane variables
		while (countone > width){countone -= width;}
		while (counttwo > width){counttwo -= width;}
		
		//here's where we do our shift against the rotating buffer
		tempL = 0;
		tempL += (int)(pL[count] * (1-(positionL-floor(positionL)))); //less as value moves away from .0
		tempL += pL[count+1]; //we can assume always using this in one way or another?
		tempL += (int)(pL[count+2] * (positionL-floor(positionL))); //greater as value moves away from .0
		tempL -= (int)(((pL[count]-pL[count+1])-(pL[count+1]-pL[count+2]))/50); //interpolation hacks 'r us		
		tempL /= 2; //gotta make temp be the same level scale as buffer
		//now we have our delay tap, which is going to do our pitch shifting
		if (abs(tempL) > 8388352.0f){tempL = (lasttempL + (lasttempL - thirdtempL));}
		//kill ticks of bad buffer mojo by sticking with the trajectory. Ugly hack *shrug*
		
		sincezerocrossL++;
		if (sincezerocrossL < 0 || sincezerocrossL > width){sincezerocrossL = 0;} //just a sanity check
		if (splicingL){tempL = (tempL + (lasttempL + (lasttempL - thirdtempL)))/2; splicingL = false;}
		//do a smoother transition by taking the sample of transition and going half with it
		
		if ((lasttempL > 0 && tempL < 0)||(lasttempL < 0 && tempL > 0)) //delay tap crossed zero
		{
			sincezerocrossL = 0;
		} //we just restarted counting from the delay tap zero cross
		
		count = gcount - (int)floor(positionR);
		//we go back because the buffer goes forward this time
		countone = count+1; 
		counttwo = count+2;
		//now we have counts zero, one, two, none of which have sanity checked values
		//we are interpolating, we ADD
		while (count < 0){count += width;}
		while (countone < 0){countone += width;}
		while (counttwo < 0){counttwo += width;}
		while (count > width){count -= width;} //this can only happen with very insane variables
		while (countone > width){countone -= width;}
		while (counttwo > width){counttwo -= width;}
		
		
		tempR = 0;
		tempR += (int)(pR[count] * (1-(positionR-floor(positionR)))); //less as value moves away from .0
		tempR += pR[count+1]; //we can assume always using this in one way or another?
		tempR += (int)(pR[count+2] * (positionR-floor(positionR))); //greater as value moves away from .0
		tempR -= (int)(((pR[count]-pR[count+1])-(pR[count+1]-pR[count+2]))/50); //interpolation hacks 'r us		
		tempR /= 2; //gotta make temp be the same level scale as buffer
		//now we have our delay tap, which is going to do our pitch shifting
		if (abs(tempR) > 8388352.0f){tempR = (lasttempR + (lasttempR - thirdtempR));}
		//kill ticks of bad buffer mojo by sticking with the trajectory. Ugly hack *shrug*
		
		sincezerocrossR++;
		if (sincezerocrossR < 0 || sincezerocrossR > width){sincezerocrossR = 0;} //just a sanity check
		if (splicingR){tempR = (tempR + (lasttempR + (lasttempR - thirdtempR)))/2; splicingR = false;}
		//do a smoother transition by taking the sample of transition and going half with it
		
		if ((lasttempR > 0 && tempR < 0)||(lasttempR < 0 && tempR > 0)) //delay tap crossed zero
		{
			sincezerocrossR = 0;
		} //we just restarted counting from the delay tap zero cross
		
		fourthtempL = thirdtempL;
		thirdtempL = lasttempL;
		lasttempL = tempL;
		
		fourthtempR = thirdtempR;
		thirdtempR = lasttempR;
		lasttempR = tempR;
		
		float mid = (inputSampleL + inputSampleR)*(1-wet);
		float side = inputSampleL - inputSampleR;
		//assign mid and side.Between these sections, you can do mid/side processing
		inputSampleL = (mid+side)/2.0f;
		inputSampleR = (mid-side)/2.0f;
		//unassign mid and side		
		
		inputSampleL = ( drySampleL * (1-wet))+((float)(tempL/(8388352.0f))*wet);
		if (inputSampleL > 4.0f) inputSampleL = 4.0f;
		if (inputSampleL < -4.0f) inputSampleL = -4.0f;
		
		inputSampleR = ( drySampleR * (1-wet))+((float)(tempR/(8388352.0f))*wet);
		if (inputSampleR > 4.0f) inputSampleR = 4.0f;
		if (inputSampleR < -4.0f) inputSampleR = -4.0f;
		//this plugin can throw insane outputs so we'll put in a hard clip
		
		
		
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
	for (int count = 0; count < 131074; count++) {pL[count] = 0; pR[count] = 0;}
	for (int count = 0; count < 257; count++) {
		offsetL[count] = 0; pastzeroL[count] = 0; previousL[count] = 0; thirdL[count] = 0; fourthL[count] = 0;
		offsetR[count] = 0; pastzeroR[count] = 0; previousR[count] = 0; thirdR[count] = 0; fourthR[count] = 0;
	}
	crossesL = 0;
	realzeroesL = 0;
	tempL = 0;
	lasttempL = 0;
	thirdtempL = 0;
	fourthtempL = 0;
	sincezerocrossL = 0;
	airPrevL = 0.0;
	airEvenL = 0.0;
	airOddL = 0.0;
	airFactorL = 0.0;
	positionL = 0.0;
	splicingL = false;	
	
	crossesR = 0;
	realzeroesR = 0;
	tempR = 0;
	lasttempR = 0;
	thirdtempR = 0;
	fourthtempR = 0;
	sincezerocrossR = 0;
	airPrevR = 0.0;
	airEvenR = 0.0;
	airOddR = 0.0;
	airFactorR = 0.0;
	positionR = 0.0;
	splicingR = false;	
	
	gcount = 0;
	lastwidth = 16386;
	flip = false;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
