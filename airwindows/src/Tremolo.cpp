#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Tremolo"
#define AIRWINDOWS_DESCRIPTION "Fluctuating saturation curves for a tubey tremolo."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','r','o' )
#define AIRWINDOWS_TAGS kNT_tagEffect
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Depth", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 speedChase;
		Float32 depthChase;
		Float32 speedAmount;
		Float32 depthAmount;
		Float32 lastSpeed;
		Float32 lastDepth;
		Float32 sweep;
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
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	speedChase = powf(GetParameter( kParam_One ),4);
	depthChase = GetParameter( kParam_Two );
	Float32 speedSpeed = 300 / (fabs( lastSpeed - speedChase)+1.0f);
	Float32 depthSpeed = 300 / (fabs( lastDepth - depthChase)+1.0f);
	lastSpeed = speedChase;
	lastDepth = depthChase;

	Float32 speed;
	Float32 depth;
	Float32 skew;
	Float32 density;
	
	Float32 tupi = 3.141592653589793238f;
	float inputSample;
	float drySample;
	Float32 control;
	Float32 tempcontrol;
	Float32 thickness;
	Float32 out;
	Float32 bridgerectifier;
	Float32 offset;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		speedAmount = (((speedAmount*speedSpeed)+speedChase)/(speedSpeed + 1.0f));
		depthAmount = (((depthAmount*depthSpeed)+depthChase)/(depthSpeed + 1.0f));
		speed = 0.0001f+(speedAmount/1000.0f);
		speed /= overallscale;
		depth = 1.0f - powf(1.0f-depthAmount,5);
		skew = 1.0f+powf(depthAmount,9);
		density = ((1.0f-depthAmount)*2.0f) - 1.0f;		
		
		offset = sin(sweep);
		sweep += speed;
		if (sweep > tupi){sweep -= tupi;}
		control = fabs(offset);
		if (density > 0)
			{
			tempcontrol = sin(control);
			control = (control * (1.0f-density))+(tempcontrol * density);
			}
		else
			{
			tempcontrol = 1-cos(control);
			control = (control * (1.0f+density))+(tempcontrol * -density);
			}
		//produce either boosted or starved version of control signal
		//will go from 0 to 1
		
		thickness = ((control * 2.0f) - 1.0f)*skew;
		out = fabs(thickness);		
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSample > 0) inputSample = (inputSample*(1-out))+(bridgerectifier*out);
		else inputSample = (inputSample*(1-out))-(bridgerectifier*out);
		//blend according to density control
		inputSample *= (1.0f - control);
		inputSample *= 2.0f;
		//apply tremolo, apply gain boost to compensate for volume loss
		inputSample = (drySample * (1-depth)) + (inputSample*depth);

		
		
		*destP = inputSample;
		destP += inNumChannels;
		sourceP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	sweep = 3.141592653589793238 / 2.0;
	speedChase = 0.0;
	depthChase = 0.0;
	speedAmount = 1.0;
	depthAmount = 0.0;
	lastSpeed = 1000.0;
	lastDepth = 1000.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
