#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "TapeDelay"
#define AIRWINDOWS_DESCRIPTION "An old school tape echo with pitch swerve effects."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','a','y' )
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
{ .name = "Dry", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wet", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Delay", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Feedback", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lean/Fat", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Depth", .min = 3000, .max = 32000, .def = 8000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		SInt32 p[258];
		int gcount;
		int delay;
		int maxdelay;
		int chase;		
		uint32_t fpd;
	
	struct _dram {
			Float32 d[44100];
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
	Float32 dry = powf(GetParameter( kParam_One ),2);
	Float32 wet = powf(GetParameter( kParam_Two ),2);
	int targetdelay = (int)(44000*GetParameter( kParam_Three ));
	Float32 feedback = (GetParameter( kParam_Four )*1.3f);
	Float32 leanfat = GetParameter( kParam_Five );
	Float32 fatwet = fabs(leanfat);
	Float32 storedelay;
	int fatness = (int)GetParameter( kParam_Six );
	Float32 sum = 0.0f;
	Float32 floattotal = 0.0f;
	SInt32 sumtotal = 0;
	SInt32 count;
	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		if (gcount < 0 || gcount > 128) {gcount = 128;}
		count = gcount;
		if (delay < 0 || delay > maxdelay) {delay = maxdelay;}
		sum = inputSample + (dram->d[delay]*feedback);
		p[count+128] = p[count] = sumtotal = (SInt32)(sum*8388608.0f);
		switch (fatness)
		{
			case 32: sumtotal += p[count+127]; //note NO break statement.
			case 31: sumtotal += p[count+113]; //This jumps to the relevant tap
			case 30: sumtotal += p[count+109]; //and then includes all smaller taps.
			case 29: sumtotal += p[count+107];
			case 28: sumtotal += p[count+103];
			case 27: sumtotal += p[count+101];
			case 26: sumtotal += p[count+97];
			case 25: sumtotal += p[count+89];
			case 24: sumtotal += p[count+83];
			case 23: sumtotal += p[count+79];
			case 22: sumtotal += p[count+73];
			case 21: sumtotal += p[count+71];
			case 20: sumtotal += p[count+67];
			case 19: sumtotal += p[count+61];
			case 18: sumtotal += p[count+59];
			case 17: sumtotal += p[count+53];
			case 16: sumtotal += p[count+47];
			case 15: sumtotal += p[count+43];
			case 14: sumtotal += p[count+41];
			case 13: sumtotal += p[count+37];
			case 12: sumtotal += p[count+31];
			case 11: sumtotal += p[count+29];
			case 10: sumtotal += p[count+23];
			case 9: sumtotal += p[count+19];
			case 8: sumtotal += p[count+17];
			case 7: sumtotal += p[count+13];
			case 6: sumtotal += p[count+11];
			case 5: sumtotal += p[count+7];
			case 4: sumtotal += p[count+5];
			case 3: sumtotal += p[count+3];
			case 2: sumtotal += p[count+2];
			case 1: sumtotal += p[count+1];
		}
		
		floattotal = (Float32)(sumtotal/fatness+1);
		floattotal /= 8388608.0f;
		floattotal *= fatwet;
		if (leanfat < 0) {storedelay = sum-floattotal;}
		else {storedelay = (sum * (1-fatwet))+floattotal;}
				
		chase += abs(maxdelay - targetdelay);
		if (chase > 9000)
		{
			if (maxdelay > targetdelay) {dram->d[delay] = storedelay; maxdelay -= 1; delay -= 1; if (delay < 0) {delay = maxdelay;} dram->d[delay] = storedelay;}
			if (maxdelay < targetdelay) {maxdelay += 1; delay += 1; if (delay > maxdelay) {delay = 0;} dram->d[delay] = storedelay;}
			chase = 0;
		}
		else
		{
			dram->d[delay] = storedelay;
		}
		
		gcount--;
		delay--;
		if (delay < 0 || delay > maxdelay) {delay = maxdelay;}
		//yes this is a second bounds check. it's cheap, check EVERY time
		inputSample = (inputSample * dry) + (dram->d[delay] * wet);		

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 257; count++) {p[count] = 0;}
	for(delay = 0; delay < 44100; delay++) {dram->d[delay] = 0.0;}
	maxdelay = 0;
	delay = 0;
	gcount = 0;
	chase = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
