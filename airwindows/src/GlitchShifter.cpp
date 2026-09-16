#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "GlitchShifter"
#define AIRWINDOWS_DESCRIPTION "A really gnarly, raw-sounding pitch shifter with a dose of insanity!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','l','r' )
#define AIRWINDOWS_TAGS kNT_tagEffect
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Note", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Trim", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tightness", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Feedback", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		SInt32 p[131076];
		SInt32 offset[258];
		SInt32 pastzero[258];
		SInt32 previous[258];
		SInt32 third[258];
		SInt32 fourth[258];
		SInt32 lastwidth;
		SInt32 temp;
		SInt32 lasttemp;
		SInt32 thirdtemp;
		SInt32 fourthtemp;
		SInt32 sincezerocross;
		int gcount;
		int crosses;
		int realzeroes;
		Float32 airPrev;
		Float32 airEven;
		Float32 airOdd;
		Float32 airFactor;
		Float32 position;
		bool flip;
		bool splicing;
		uint32_t fpd;
	
	struct _dram {
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
	Float32 note = GetParameter( kParam_One );
	Float32 trim = GetParameter( kParam_Two );
	Float32 speed = ((1.0f/12.0f)*note)+trim;	
	if (speed < 0) speed *= 0.5f;
	//include sanity check- maximum pitch lowering cannot be to 0 hz.
	SInt32 width = (SInt32)(65536-((1-powf(1-GetParameter( kParam_Three ),2))*65530.0f));
	Float32 bias = powf(GetParameter( kParam_Three ),3);
	Float32 feedback = GetParameter( kParam_Four )/1.5f;
	Float32 wet = GetParameter( kParam_Five );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		airFactor = airPrev - inputSample;
		if (flip) {airEven += airFactor; airOdd -= airFactor; airFactor = airEven;}
		else {airOdd += airFactor; airEven -= airFactor; airFactor = airOdd;}
		airOdd = (airOdd - ((airOdd - airEven)/256.0f)) / 1.0001f;
		airEven = (airEven - ((airEven - airOdd)/256.0f)) / 1.0001f;
		airPrev = inputSample;
		inputSample += airFactor;
		flip = !flip;
		//air, compensates for loss of highs of interpolation
		
		if (lastwidth != width){crosses = 0; realzeroes = 0; lastwidth = width;}
		
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
		
		p[count+width] = p[count] = (SInt32)((inputSample*8388352.0f)+(Float32)(lasttemp*feedback));
		//float buffer -8388352 to 8388352 is equal to 24 bit linear space

		if ((p[countone] > 0 && p[count] < 0)||(p[countone] < 0 && p[count] > 0)) //source crossed zero
			{
				crosses++;
				realzeroes++;
				if (crosses > 256) {crosses = 0;} //wrap crosses to keep adding new crosses
				if (realzeroes > 256) {realzeroes = 256;} //don't wrap realzeroes, full buffer, use all
				offset[crosses] = count;
				pastzero[crosses] = p[count];
				previous[crosses] = p[countone];
				third[crosses] = p[counttwo];
				//we load the zero crosses register with crosses to examine
				//the purpose is amassing situations to compare potential zero splice points
				//and switch at the moment the wave angles are most similar
			} //we just put in a source zero cross in the registry

		position -= speed;
		
		if (position > width) {
			if (realzeroes > 0) { //we just caught up to the buffer end with zero crosses in the bin
				position = 0;
				Float32 diff = 99999999.0f;
				int best = 0;
				int scan;
				for(scan = (realzeroes-1); scan >= 0; scan--) {
					int scanone = scan + crosses;
					if (scanone > 256){scanone -= 256;}
					//try to track the real most recent ones more closely
					Float32 howdiff = (Float32)((temp - pastzero[scanone]) + (lasttemp - previous[scanone]) + (thirdtemp - third[scanone]) + (fourthtemp - fourth[scanone]));
					//got difference factor between things
					howdiff -= (Float32)(scan*bias);
					//try to bias in favor of more recent crosses
					if (howdiff < diff) {diff = howdiff; best = scanone;}
				} //now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
				position = offset[best]-sincezerocross;
				crosses = 0;
				realzeroes = 0;
				splicing = true; //we just kicked the delay tap back
			} else {
				position -= width; //with no zero crosses in the bin
				crosses = 0;
				realzeroes = 0;
				splicing = true; //so, hard splice it.
			}
		}
		
		if (position < 0) { //we just caught up to the dry tap
			if (realzeroes > 0) { //we just caught up to the dry tap with zero crosses in the bin
				position = 0;
				Float32 diff = 99999999.0f;
				int best = 0;
				int scan;
				for(scan = (realzeroes-1); scan >= 0; scan--) {
					int scanone = scan + crosses;
					if (scanone > 256){scanone -= 256;}
					//try to track the real most recent ones more closely
					Float32 howdiff = (Float32)((temp - pastzero[scanone]) + (lasttemp - previous[scanone]) + (thirdtemp - third[scanone]) + (fourthtemp - fourth[scanone]));
					//got difference factor between things
					howdiff -= (Float32)(scan*bias);
					//try to bias in favor of more recent crosses
					if (howdiff < diff) {diff = howdiff; best = scanone;}
				} //now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
				position = offset[best]-sincezerocross;
				crosses = 0;
				realzeroes = 0;
				splicing = true; //we just kicked the delay tap back
			} else {
				position += width; //with no zero crosses in the bin
				crosses = 0;
				realzeroes = 0;
				splicing = true; //so, hard splice it.
			}
		}

		count = gcount - (int)floor(position);
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
		temp = 0;
		temp += (SInt32)(p[count] * (1-(position-floor(position)))); //less as value moves away from .0
		temp += p[count+1]; //we can assume always using this in one way or another?
		temp += (SInt32)(p[count+2] * (position-floor(position))); //greater as value moves away from .0
		temp -= (SInt32)(((p[count]-p[count+1])-(p[count+1]-p[count+2]))/50); //interpolation hacks 'r us		
		temp /= 2; //gotta make temp be the same level scale as buffer
		//now we have our delay tap, which is going to do our pitch shifting
		
		if (abs(temp) > 8388352.0f){temp = (lasttemp + (lasttemp - thirdtemp));}
		//kill ticks of bad buffer mojo by sticking with the trajectory. Ugly hack *shrug*
		
		sincezerocross++;
		if (sincezerocross < 0 || sincezerocross > width){sincezerocross = 0;} //just a sanity check
		if (splicing){temp = (temp + (lasttemp + (lasttemp - thirdtemp)))/2; splicing = false;}
		//do a smoother transition by taking the sample of transition and going half with it

		if ((lasttemp > 0 && temp < 0)||(lasttemp < 0 && temp > 0)) //delay tap crossed zero
		{
			sincezerocross = 0;
		} //we just restarted counting from the delay tap zero cross
		
		fourthtemp = thirdtemp;
		thirdtemp = lasttemp;
		lasttemp = temp;
		
		inputSample = ( *sourceP * (1-wet))+((Float32)(temp/(8388352.0f))*wet);
		
		if (inputSample > 4.0f) inputSample = 4.0f;
		if (inputSample < -4.0f) inputSample = -4.0f;
		//this plugin can throw insane outputs so we'll put in a hard clip
		
		
		
		*destP = inputSample;
		destP += inNumChannels;
		sourceP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int count = 0; count < 131074; count++) {p[count] = 0;}
	for (int count = 0; count < 257; count++) {offset[count] = 0; pastzero[count] = 0; previous[count] = 0; third[count] = 0; fourth[count] = 0;}
	gcount = 0;
	crosses = 0;
	realzeroes = 0;
	temp = 0;
	lasttemp = 0;
	thirdtemp = 0;
	fourthtemp = 0;
	lastwidth = 16386;
	sincezerocross = 0;
	airPrev = 0.0;
	airEven = 0.0;
	airOdd = 0.0;
	airFactor = 0.0;
	position = 0.0;
	flip = false;
	splicing = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
