#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ADClip7"
#define AIRWINDOWS_DESCRIPTION "The ultimate Airwindows loudness maximizer/ peak control."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','D','7' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,	//Add your parameters here...
	kNumberOfParameters=4
};
static const int kNormal = 1;
static const int kGain = 2;
static const int kClip = 3;
static const int kDefaultValue_ParamFour = kNormal;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static char const * const enumStrings3[] = { "", "ADClip Normal", "Gain Match", "Clip Only", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Boost", .min = 0, .max = 18000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Soften", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Enhance", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mode", .min = 1, .max = 3, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings3 },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float lastSample;
		int gcount;
		Float32 lows;
		Float32 iirLowsA;
		Float32 iirLowsB;
		float refclip;
		uint32_t fpd;
	
	struct _dram {
			Float32 b[22200];
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
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 fpOld = 0.618033988749894848204586f; //golden ratio!
	Float32 fpNew = 1.0f - fpOld;
	
	Float32 inputGain = powf(10.0f,(GetParameter( kParam_One ))/20.0f);
	Float32 softness = GetParameter( kParam_Two ) * fpNew;
	Float32 hardness = 1.0f - softness;
	Float32 highslift = 0.307f * GetParameter( kParam_Three );
	Float32 adjust = powf(highslift,3) * 0.416f;
	Float32 subslift = 0.796f * GetParameter( kParam_Three );
	Float32 calibsubs = subslift/53;
	Float32 invcalibsubs = 1.0f - calibsubs;
	Float32 subs = 0.81f + (calibsubs*2);
	float bridgerectifier;
	int mode = (int) GetParameter( kParam_Four );
	Float32 overshoot;
	Float32 offsetH1 = 1.84f;
	offsetH1 *= overallscale;
	Float32 offsetH2 = offsetH1 * 1.9f;
	Float32 offsetH3 = offsetH1 * 2.7f;
	Float32 offsetL1 = 612;
	offsetL1 *= overallscale;
	Float32 offsetL2 = offsetL1 * 2.0f;
	int refH1 = (int)floor(offsetH1);
	int refH2 = (int)floor(offsetH2);
	int refH3 = (int)floor(offsetH3);
	int refL1 = (int)floor(offsetL1);
	int refL2 = (int)floor(offsetL2);
	int temp;
	Float32 fractionH1 = offsetH1 - floor(offsetH1);
	Float32 fractionH2 = offsetH2 - floor(offsetH2);
	Float32 fractionH3 = offsetH3 - floor(offsetH3);
	Float32 minusH1 = 1.0f - fractionH1;
	Float32 minusH2 = 1.0f - fractionH2;
	Float32 minusH3 = 1.0f - fractionH3;
	Float32 highs = 0.0f;
	int count = 0;
	float inputSample;
	
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (inputGain != 1.0f) {
			inputSample *= inputGain;
		}
		
		overshoot = fabs(inputSample) - refclip;
		if (overshoot < 0.0f) overshoot = 0.0f;
				
		if (gcount < 0 || gcount > 11020) {gcount = 11020;}
		count = gcount;
		dram->b[count+11020] = dram->b[count] = overshoot;
		gcount--;
		
		if (highslift > 0.0f)
		{
			//we have a big pile of dram->b[] which is overshoots
			temp = count+refH3;
			highs = -(dram->b[temp] * minusH3); //less as value moves away from .0
			highs -= dram->b[temp+1]; //we can assume always using this in one way or another?
			highs -= (dram->b[temp+2] * fractionH3); //greater as value moves away from .0
			highs += (((dram->b[temp]-dram->b[temp+1])-(dram->b[temp+1]-dram->b[temp+2]))/50); //interpolation hacks 'r us
			highs *= adjust; //add in the kernel elements backwards saves multiplies
			//stage 3 is a negative add
			temp = count+refH2;
			highs += (dram->b[temp] * minusH2); //less as value moves away from .0
			highs += dram->b[temp+1]; //we can assume always using this in one way or another?
			highs += (dram->b[temp+2] * fractionH2); //greater as value moves away from .0
			highs -= (((dram->b[temp]-dram->b[temp+1])-(dram->b[temp+1]-dram->b[temp+2]))/50); //interpolation hacks 'r us
			highs *= adjust; //add in the kernel elements backwards saves multiplies
			//stage 2 is a positive feedback of the overshoot
			temp = count+refH1;
			highs -= (dram->b[temp] * minusH1); //less as value moves away from .0
			highs -= dram->b[temp+1]; //we can assume always using this in one way or another?
			highs -= (dram->b[temp+2] * fractionH1); //greater as value moves away from .0
			highs += (((dram->b[temp]-dram->b[temp+1])-(dram->b[temp+1]-dram->b[temp+2]))/50); //interpolation hacks 'r us
			highs *= adjust; //add in the kernel elements backwards saves multiplies
			//stage 1 is a negative feedback of the overshoot
			//done with interpolated mostly negative feedback of the overshoot
		}
		
		bridgerectifier = sin(fabs(highs) * hardness);
		//this will wrap around and is scaled back by softness
		//wrap around is the same principle as Fracture: no top limit to sin()
		if (highs > 0) highs = bridgerectifier;
		else highs = -bridgerectifier;
		
		if (subslift > 0.0f) 
		{
			lows *= subs;
			//going in we'll reel back some of the swing
			temp = count+refL1;
			lows -= dram->b[temp+127];
			lows -= dram->b[temp+113];
			lows -= dram->b[temp+109];
			lows -= dram->b[temp+107];
			lows -= dram->b[temp+103];
			lows -= dram->b[temp+101];
			lows -= dram->b[temp+97];
			lows -= dram->b[temp+89];
			lows -= dram->b[temp+83];
			lows -= dram->b[temp+79];
			lows -= dram->b[temp+73];
			lows -= dram->b[temp+71];
			lows -= dram->b[temp+67];
			lows -= dram->b[temp+61];
			lows -= dram->b[temp+59];
			lows -= dram->b[temp+53];
			lows -= dram->b[temp+47];
			lows -= dram->b[temp+43];
			lows -= dram->b[temp+41];
			lows -= dram->b[temp+37];
			lows -= dram->b[temp+31];
			lows -= dram->b[temp+29];
			lows -= dram->b[temp+23];
			lows -= dram->b[temp+19];
			lows -= dram->b[temp+17];
			lows -= dram->b[temp+13];
			lows -= dram->b[temp+11];
			lows -= dram->b[temp+7];
			lows -= dram->b[temp+5];
			lows -= dram->b[temp+3];
			lows -= dram->b[temp+2];
			lows -= dram->b[temp+1];
			//initial negative lobe
			lows *= subs;
			lows *= subs;
			//twice, to minimize the suckout in low boost situations
			temp = count+refL2;
			lows += dram->b[temp+127];
			lows += dram->b[temp+113];
			lows += dram->b[temp+109];
			lows += dram->b[temp+107];
			lows += dram->b[temp+103];
			lows += dram->b[temp+101];
			lows += dram->b[temp+97];
			lows += dram->b[temp+89];
			lows += dram->b[temp+83];
			lows += dram->b[temp+79];
			lows += dram->b[temp+73];
			lows += dram->b[temp+71];
			lows += dram->b[temp+67];
			lows += dram->b[temp+61];
			lows += dram->b[temp+59];
			lows += dram->b[temp+53];
			lows += dram->b[temp+47];
			lows += dram->b[temp+43];
			lows += dram->b[temp+41];
			lows += dram->b[temp+37];
			lows += dram->b[temp+31];
			lows += dram->b[temp+29];
			lows += dram->b[temp+23];
			lows += dram->b[temp+19];
			lows += dram->b[temp+17];
			lows += dram->b[temp+13];
			lows += dram->b[temp+11];
			lows += dram->b[temp+7];
			lows += dram->b[temp+5];
			lows += dram->b[temp+3];
			lows += dram->b[temp+2];
			lows += dram->b[temp+1];
			lows *= subs;
			//followup positive lobe
			//now we have the lows content to use
		}
		
		bridgerectifier = sin(fabs(lows) * softness);
		//this will wrap around and is scaled back by hardness: hard = less bass push, more treble
		//wrap around is the same principle as Fracture: no top limit to sin()
		if (lows > 0) lows = bridgerectifier;
		else lows = -bridgerectifier;
		
		iirLowsA = (iirLowsA * invcalibsubs) + (lows * calibsubs);
		lows = iirLowsA;
		bridgerectifier = sin(fabs(lows));
		if (lows > 0) lows = bridgerectifier;
		else lows = -bridgerectifier;
		
		iirLowsB = (iirLowsB * invcalibsubs) + (lows * calibsubs);
		lows = iirLowsB;
		bridgerectifier = sin(fabs(lows)) * 2.0f;
		if (lows > 0) lows = bridgerectifier;
		else lows = -bridgerectifier;
		
		if (highslift > 0.0f) inputSample += (highs * (1.0f-fabs(inputSample*hardness)));
		if (subslift > 0.0f) inputSample += (lows * (1.0f-fabs(inputSample*softness)));
		
		if (inputSample > refclip && refclip > 0.9f) refclip -= 0.01f;
		if (inputSample < -refclip && refclip > 0.9f) refclip -= 0.01f;
		if (refclip < 0.99f) refclip += 0.00001f;
		//adjust clip level on the fly
		
		if (lastSample >= refclip)
		{
			if (inputSample < refclip) lastSample = ((refclip*hardness) + (inputSample * softness));
			else lastSample = refclip;
		}
		
		if (lastSample <= -refclip)
		{
			if (inputSample > -refclip) lastSample = ((-refclip*hardness) + (inputSample * softness));
			else lastSample = -refclip;
		}
		
		if (inputSample > refclip)
		{
			if (lastSample < refclip) inputSample = ((refclip*hardness) + (lastSample * softness));
			else inputSample = refclip;
		}
		
		if (inputSample < -refclip)
		{
			if (lastSample > -refclip) inputSample = ((-refclip*hardness) + (lastSample * softness));
			else inputSample = -refclip;
		}
		lastSample = inputSample;
		
		switch (mode)
		{
			case 1: break; //Normal
			case 2: inputSample /= inputGain; break; //Gain Match
			case 3: inputSample = overshoot + highs + lows; break; //Clip Only
		}
		//this is our output mode switch, showing the effects
		
		if (inputSample > refclip) inputSample = refclip;
		if (inputSample < -refclip) inputSample = -refclip;
		//final iron bar
		
		
				
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	for(int count = 0; count < 22199; count++) {dram->b[count] = 0;}
	gcount = 0;
	lows = 0;
	refclip = 0.99;
	iirLowsA = 0.0;
	iirLowsB = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
