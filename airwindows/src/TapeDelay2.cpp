#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "TapeDelay2"
#define AIRWINDOWS_DESCRIPTION "A new implementation of a flexible tape echo."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','a','2' )
#define AIRWINDOWS_KERNELS
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
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Time", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Regen", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Flutter", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 prevSample;
		Float32 delay;
		Float32 sweep;
		float regenFilter[9];
		float outFilter[9];
		float lastRef[10];
		int cycle;	
		uint32_t fpd;
	
	struct _dram {
			Float32 d[88211];
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
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check	
	
	Float32 baseSpeed = (powf(GetParameter( kParam_One ),4)*25.0f)+1.0f;
	Float32 feedback = powf(GetParameter( kParam_Two ),2);
	
	//[0] is frequency: 0.000001f to 0.499999f is near-zero to near-Nyquist
	//[1] is resonance, 0.7071f is Butterworth. Also can't be zero
	regenFilter[0] = ((powf(GetParameter( kParam_Three ),3)*0.4f)+0.0001f);
	regenFilter[1] = powf(GetParameter( kParam_Four ),2)+0.01f; //resonance
	float K = tan(M_PI * regenFilter[0]);
	float norm = 1.0f / (1.0f + K / regenFilter[1] + K * K);
	regenFilter[2] = K / regenFilter[1] * norm;
	regenFilter[4] = -regenFilter[2];
	regenFilter[5] = 2.0f * (K * K - 1.0f) * norm;
	regenFilter[6] = (1.0f - K / regenFilter[1] + K * K) * norm;

	//[0] is frequency: 0.000001f to 0.499999f is near-zero to near-Nyquist
	//[1] is resonance, 0.7071f is Butterworth. Also can't be zero
	outFilter[0] = regenFilter[0];
	outFilter[1] = regenFilter[1] * 1.618033988749894848204586f; //resonance
	K = tan(M_PI * outFilter[0]);
	norm = 1.0f / (1.0f + K / outFilter[1] + K * K);
	outFilter[2] = K / outFilter[1] * norm;
	outFilter[4] = -outFilter[2];
	outFilter[5] = 2.0f * (K * K - 1.0f) * norm;
	outFilter[6] = (1.0f - K / outFilter[1] + K * K) * norm;
	
	Float32 vibSpeed = powf(GetParameter( kParam_Five ),5) * baseSpeed * ((regenFilter[0]*0.09f)+0.025f);//0.05f
	Float32 wet = GetParameter( kParam_Six )*2.0f;
	Float32 dry = 2.0f - wet;
	if (wet > 1.0f) wet = 1.0f;
	if (wet < 0.0f) wet = 0.0f;
	if (dry > 1.0f) dry = 1.0f;
	if (dry < 0.0f) dry = 0.0f;
	//this echo makes 50% full dry AND full wet, not crossfaded.
	//that's so it can be on submixes without cutting back dry channel when adjusted:
	//unless you go super heavy, you are only adjusting the added echo loudness.
	
	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		cycle++;
		if (cycle == cycleEnd) {
			Float32 speed = baseSpeed + (vibSpeed * (sin(sweep)+1.0f));
			sweep += (0.05f*inputSample*inputSample); if (sweep > 6.283185307179586f) sweep -= 6.283185307179586f;
			
			int pos = floor(delay);
			Float32 newSample = inputSample + dram->d[pos]*feedback;
			float tempSample = (newSample * regenFilter[2]) + regenFilter[7];
			regenFilter[7] = -(tempSample * regenFilter[5]) + regenFilter[8];
			regenFilter[8] = (newSample * regenFilter[4]) - (tempSample * regenFilter[6]);
			newSample = tempSample;
			
			delay -= speed; if (delay < 0) delay += 88200.0f;
			Float32 increment = (newSample - prevSample) / speed;
			dram->d[pos] = prevSample;
			while (pos != floor(delay)) {
				dram->d[pos] = prevSample;
				prevSample += increment;
				pos--; if (pos < 0) pos += 88200;
			}
			prevSample = newSample;
			pos = floor(delay); inputSample = dram->d[pos];
			tempSample = (inputSample * outFilter[2]) + outFilter[7];
			outFilter[7] = -(tempSample * outFilter[5]) + outFilter[8];
			outFilter[8] = (inputSample * outFilter[4]) - (tempSample * outFilter[6]);
			inputSample = tempSample;
			
			
			if (cycleEnd == 4) {
				lastRef[0] = lastRef[4]; //start from previous last
				lastRef[2] = (lastRef[0] + inputSample)/2; //half
				lastRef[1] = (lastRef[0] + lastRef[2])/2; //one quarter
				lastRef[3] = (lastRef[2] + inputSample)/2; //three quarters
				lastRef[4] = inputSample; //full
			}
			if (cycleEnd == 3) {
				lastRef[0] = lastRef[3]; //start from previous last
				lastRef[2] = (lastRef[0]+lastRef[0]+inputSample)/3; //third
				lastRef[1] = (lastRef[0]+inputSample+inputSample)/3; //two thirds
				lastRef[3] = inputSample; //full
			}
			if (cycleEnd == 2) {
				lastRef[0] = lastRef[2]; //start from previous last
				lastRef[1] = (lastRef[0] + inputSample)/2; //half
				lastRef[2] = inputSample; //full
			}
			if (cycleEnd == 1) lastRef[0] = inputSample;
			cycle = 0; //reset
			inputSample = lastRef[cycle];
		} else {
			inputSample = lastRef[cycle];
			//we are going through our references now
		}
		switch (cycleEnd) //multi-pole average using lastRef[] variables
		{
			case 4:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[7])*0.5f;
				lastRef[7] = lastRef[8]; //continue, do not break
			case 3:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[6])*0.5f;
				lastRef[6] = lastRef[8]; //continue, do not break
			case 2:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[5])*0.5f;
				lastRef[5] = lastRef[8]; //continue, do not break
			case 1:
				break; //no further averaging
		}
		
		if (wet < 1.0f) inputSample *= wet;
		if (dry < 1.0f) drySample *= dry;
		inputSample += drySample;
		//this is our submix echo dry/wet: 0.5f is BOTH at FULL VOLUME
		//purpose is that, if you're adding echo, you're not altering other balances
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int x = 0; x < 88210; x++) {dram->d[x] = 0.0;}
	prevSample = 0.0;
	delay = 0.0;
	sweep = 0.0;
	for (int x = 0; x < 9; x++) {regenFilter[x] = 0.0; outFilter[x] = 0.0;}
	for (int x = 0; x < 9; x++) {lastRef[x] = 0.0;}
	cycle = 0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
