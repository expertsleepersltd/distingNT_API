#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dattorro"
#define AIRWINDOWS_DESCRIPTION "The resonant lowpass filter out of Donut."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','a','o' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		float low;
		float band;
		float freqA, freqB;
		float resoA, resoB;
		float outA, outB;		
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
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();

	freqA = freqB; resoA = resoB; outA = outB;
	freqB = powf(GetParameter( kParam_A ),overallscale+1.0f)*1.225f;
	resoB = powf(1.0f-GetParameter( kParam_B ),2.0f);
	if (resoB < 0.001f) resoB = 0.001f; // q of 0.0f is just a tone
	outB = GetParameter( kParam_C )/sqrt(resoB);

	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;

		const float temp = (float)nSampleFrames/inFramesToProcess;
		const float freq = (freqA*temp)+(freqB*(1.0f-temp));
		const float reso = (resoA*temp)+(resoB*(1.0f-temp));
		const float out = (outA*temp)+(outB*(1.0f-temp)); //dezippering
		
		low += freq*band; band += freq*((reso*inputSampleL)-low-(reso*band));
		inputSampleL = (low-sin(band*0.5f))*out; //airwindattorro
		
		//since this is called Dattorro, I'm including a variation on
		//the textbook code for this, so you can have the normal SVF
		//on tap if you want its lowpass, bandpass or highpass.
		//You can steepen it by cascading additional layers of SVF.
		//the Dattorro source does not produce correct frequencies.
		//it uses cutoff = GetParameter( kParam_A )*20000.0f;
		//and then f = 2.0f*sin(M_PI * (cutoff / GetSampleRate()));
		//this causes a crash when f is higher than 0.25f Nyquist
		//and also doesn't return the right frequency
		//-------- here is the controls code, for outside the buffer
		//float f = powf(GetParameter( kParam_A ),overallscale+1.0f)*1.225f;
		//float q = powf(1.0f-GetParameter( kParam_B ),2.0f); //reso
		//if (q < 0.001f) q = 0.001f; // q of 0.0f is just a tone
		//float outL = GetParameter( kParam_C ); //lowpass output
		//float outB = 0.0f; //bandpass output
		//float outH = 0.0f; //highpass output
		//notch output is simply highpass+lowpass
		//-------- here is the audio code, inside the buffer
		//low += f*band;
		//band += f*((q*inputSample)-low-(q*band));
		//const float high = (q*inputSample) - low - (q*band);
		//band += f*high;
		//inputSample = 0.0f; //now let's build from the outputs
		//inputSample += low*outL;
		//inputSample += band*outB;
		//inputSample += high*outH;
		//-------- and we're done, that's a Dattorro SVF
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	low = band = 0.0;
	freqA = freqB = 0.5;
	resoA = resoB = 0.5;
	outA = outB = 1.0;	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
