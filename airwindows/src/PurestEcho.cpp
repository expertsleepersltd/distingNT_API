#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestEcho"
#define AIRWINDOWS_DESCRIPTION "Optimized Airwindows echo with exactly four evenly spaced taps on tap."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','!' )
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
{ .name = "Time", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tap 1", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tap 2", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tap 3", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tap 4", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		const static int totalsamples = 65535;
		int gcount;
		uint32_t fpd;
	
	struct _dram {
			Float32 d[totalsamples];
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
	
	int loopLimit = (int)(totalsamples * 0.499f);
	//this is a float buffer so we will be splitting it in two
	
	Float32 time = powf(GetParameter( kParam_One ),2) * 0.999f;
	Float32 tap1 = GetParameter( kParam_Two );
	Float32 tap2 = GetParameter( kParam_Three );
	Float32 tap3 = GetParameter( kParam_Four );
	Float32 tap4 = GetParameter( kParam_Five );
	
	Float32 gainTrim = 1.0f / (1.0f + tap1 + tap2 + tap3 + tap4);
	//this becomes our equal-loudness mechanic. 0.2f to 1.0f gain on all things.
	Float32 tapsTrim = gainTrim * 0.5f;
	//the taps interpolate and require half that gain: 0.1f to 0.5f on all taps.
	
	int position1 = (int)(loopLimit * time * 0.25f);
	int position2 = (int)(loopLimit * time * 0.5f);
	int position3 = (int)(loopLimit * time * 0.75f);
	int position4 = (int)(loopLimit * time);
	//basic echo information: we're taking four equally spaced echoes and setting their levels as desired.
	//position4 is what you'd have for 'just set a delay time'

	Float32 volAfter1 = (loopLimit * time * 0.25f) - position1;
	Float32 volAfter2 = (loopLimit * time * 0.5f) - position2;
	Float32 volAfter3 = (loopLimit * time * 0.75f) - position3;
	Float32 volAfter4 = (loopLimit * time) - position4;
	//these are 0-1: casting to an (int) truncates fractional numbers towards zero (and is faster than floor() )
	//so, when we take the integer number (all above zero) and subtract it from the real value, we get 0-1
	Float32 volBefore1 = (1.0f - volAfter1) * tap1;
	Float32 volBefore2 = (1.0f - volAfter2) * tap2;
	Float32 volBefore3 = (1.0f - volAfter3) * tap3;
	Float32 volBefore4 = (1.0f - volAfter4) * tap4;
	//and if we are including a bit of the previous/next sample to interpolate, then if the sample position is 1.0001f
	//we'll be leaning most heavily on the 'before' sample which is nearer to us, and the 'after' sample is almost not used.
	//if the sample position is 1.9999f, the 'after' sample is strong and 'before' is almost not used.
	
	volAfter1 *= tap1;
	volAfter2 *= tap2;
	volAfter3 *= tap3;
	volAfter4 *= tap4;
	//and like with volBefore, we also want to scale this 'interpolate' to the loudness of this tap.
	//We do it here because we can do it only once per audio buffer, not on every sample. This assumes we're
	//not moving the tap every sample: if so we'd have to do this every sample as well.	
	
	int oneBefore1 = position1 - 1;
	int oneBefore2 = position2 - 1;
	int oneBefore3 = position3 - 1;
	int oneBefore4 = position4 - 1;
	if (oneBefore1 < 0) oneBefore1 = 0;
	if (oneBefore2 < 0) oneBefore2 = 0;
	if (oneBefore3 < 0) oneBefore3 = 0;
	if (oneBefore4 < 0) oneBefore4 = 0;
	int oneAfter1 = position1 + 1;
	int oneAfter2 = position2 + 1;
	int oneAfter3 = position3 + 1;
	int oneAfter4 = position4 + 1;
	//this is setting up the way we interpolate samples: we're doing an echo-darkening thing
	//to make it sound better. Pretty much no acoustic delay in human-breathable air will give
	//you zero attenuation at 22 kilohertz: forget this at your peril ;)
	
	Float32 delaysBuffer;
		
	float inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (gcount < 0 || gcount > loopLimit) gcount = loopLimit;
		dram->d[gcount+loopLimit] = dram->d[gcount] = inputSample * tapsTrim; //this is how the float buffer works:
		//we can look for delay taps without ever having to 'wrap around' within our calculation.
		//As long as the delay tap is less than our loop limit we can always just add it to where we're
		//at, and get a valid sample back right away, no matter where we are in the buffer.
		//The 0.5f is taking into account the interpolation, by padding down the whole buffer.
		
		delaysBuffer = (dram->d[gcount+oneBefore4]*volBefore4);
		delaysBuffer += (dram->d[gcount+oneAfter4]*volAfter4);
		delaysBuffer += (dram->d[gcount+oneBefore3]*volBefore3);
		delaysBuffer += (dram->d[gcount+oneAfter3]*volAfter3);
		delaysBuffer += (dram->d[gcount+oneBefore2]*volBefore2);
		delaysBuffer += (dram->d[gcount+oneAfter2]*volAfter2);
		delaysBuffer += (dram->d[gcount+oneBefore1]*volBefore1);
		delaysBuffer += (dram->d[gcount+oneAfter1]*volAfter1);
		//These are the interpolated samples. We're adding them first, because we know they're smaller
		//and while the value of delaysBuffer is small we'll add similarly small values to it. Note the order.
		
		delaysBuffer += (dram->d[gcount+position4]*tap4);
		delaysBuffer += (dram->d[gcount+position3]*tap3);
		delaysBuffer += (dram->d[gcount+position2]*tap2);
		delaysBuffer += (dram->d[gcount+position1]*tap1);
		//These are the primary samples for the echo, and we're adding them last. As before we're starting with the
		//most delayed echoes, and ending with what we think might be the loudest echo. We're building this delaybuffer
		//from the faintest noises to the loudest, to avoid adding a bunch of teeny values at the end.
		//You can of course put the last echo as loudest, but with diminishing echo volumes this is optimal.
		//This technique is also present in other plugins such as Iron Oxide.
		
		inputSample = (inputSample * gainTrim) + delaysBuffer;
		//this could be just inputSample += dram->d[gcount+position1];
		//for literally a single, full volume echo combined with dry.
		//What I'm doing is making the echoes more interesting.
		
		gcount--;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	//totalsamples comes from the .h file: it's a const static number that defines
	//the whole delay buffer. We still have a hardcoded delay buffer, but some might like
	//to use this to define the buffer in terms of seconds: samples as a factor of GetSampleRate()
	//The danger there, of course, is having a user start up the plugin at 384K and smashing their memory
	
	for(int count = 0; count < totalsamples-1; count++) {dram->d[count] = 0;}
	gcount = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
