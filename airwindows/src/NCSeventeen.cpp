#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "NCSeventeen"
#define AIRWINDOWS_DESCRIPTION "Dirty Loud!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','N','C','n' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
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
{ .name = "LOUDER", .min = 0, .max = 24000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 lastSample;
		Float32 iirSampleA;
		Float32 iirSampleB;
		bool flip;
		Float32 basslev;
		Float32 treblev;
		Float32 cheblev;
		uint32_t fpd;
		//default stuff
		
	
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
	Float32 inP2;
	Float32 chebyshev;
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 IIRscaleback = 0.0004716f;
	Float32 bassScaleback = 0.0002364f;
	Float32 trebleScaleback = 0.0005484f;
	Float32 addBassBuss = 0.000243f;
	Float32 addTrebBuss = 0.000407f;
	Float32 addShortBuss = 0.000326f;
	IIRscaleback /= overallscale;
	bassScaleback /= overallscale;
	trebleScaleback /= overallscale;
	addBassBuss /= overallscale;
	addTrebBuss /= overallscale;
	addShortBuss /= overallscale;
	Float32 limitingBass = 0.39f;
	Float32 limitingTreb = 0.6f;
	Float32 limiting = 0.36f;
	Float32 maxfeedBass = 0.972f;
	Float32 maxfeedTreb = 0.972f;
	Float32 maxfeed = 0.975f;
	Float32 bridgerectifier;
	float inputSample;
	Float32 lowSample;
	Float32 highSample;
	Float32 distSample;
	Float32 minusSample;
	Float32 plusSample;
	Float32 gain = powf(10.0f,GetParameter( kParam_One )/20);
	Float32 outgain = GetParameter( kParam_Two );

	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		inputSample *= gain;

		if (flip)
			{
			iirSampleA = (iirSampleA * 0.9f) + (inputSample * 0.1f);
			lowSample = iirSampleA;
			}
		else
			{
			iirSampleB = (iirSampleB * 0.9f) + (inputSample * 0.1f);
			lowSample = iirSampleB;
			}
		highSample = inputSample - lowSample;
		flip = !flip;
		//we now have two bands and the original source

		//inputSample = lowSample;
		inP2 = lowSample * lowSample;
		if (inP2 > 1.0f) inP2 = 1.0f; if (inP2 < -1.0f) inP2 = -1.0f;
		chebyshev = (2 * inP2);
		chebyshev *= basslev;
		//second harmonic max +1
		if (basslev > 0) basslev -= bassScaleback;
		if (basslev < 0) basslev += bassScaleback;
		//this is ShortBuss, IIRscaleback is the decay speed. *2 for second harmonic, and so on
		bridgerectifier = fabs(lowSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (lowSample > 0.0f) distSample = bridgerectifier;
		else distSample = -bridgerectifier;
		minusSample = lowSample - distSample;
		plusSample = lowSample + distSample;
		if (minusSample > maxfeedBass) minusSample = maxfeedBass;
		if (plusSample > maxfeedBass) plusSample = maxfeedBass;
		if (plusSample < -maxfeedBass) plusSample = -maxfeedBass;
		if (minusSample < -maxfeedBass) minusSample = -maxfeedBass;
		if (lowSample > distSample) basslev += (minusSample*addBassBuss);
		if (lowSample < -distSample) basslev -= (plusSample*addBassBuss);
		if (basslev > 1.0f)  basslev = 1.0f;
		if (basslev < -1.0f) basslev = -1.0f;
		bridgerectifier = fabs(lowSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (lowSample > 0.0f) lowSample = bridgerectifier;
		else lowSample = -bridgerectifier;
		//apply the distortion transform for reals
		lowSample /= (1.0f+fabs(basslev*limitingBass));
		lowSample += chebyshev;
		//apply the correction measures
		
		//inputSample = highSample;
		inP2 = highSample * highSample;
		if (inP2 > 1.0f) inP2 = 1.0f; if (inP2 < -1.0f) inP2 = -1.0f;
		chebyshev = (2 * inP2);
		chebyshev *= treblev;
		//second harmonic max +1
		if (treblev > 0) treblev -= trebleScaleback;
		if (treblev < 0) treblev += trebleScaleback;
		//this is ShortBuss, IIRscaleback is the decay speed. *2 for second harmonic, and so on
		bridgerectifier = fabs(highSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (highSample > 0.0f) distSample = bridgerectifier;
		else distSample = -bridgerectifier;
		minusSample = highSample - distSample;
		plusSample = highSample + distSample;
		if (minusSample > maxfeedTreb) minusSample = maxfeedTreb;
		if (plusSample > maxfeedTreb) plusSample = maxfeedTreb;
		if (plusSample < -maxfeedTreb) plusSample = -maxfeedTreb;
		if (minusSample < -maxfeedTreb) minusSample = -maxfeedTreb;
		if (highSample > distSample) treblev += (minusSample*addTrebBuss);
		if (highSample < -distSample) treblev -= (plusSample*addTrebBuss);
		if (treblev > 1.0f)  treblev = 1.0f;
		if (treblev < -1.0f) treblev = -1.0f;
		bridgerectifier = fabs(highSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (highSample > 0.0f) highSample = bridgerectifier;
		else highSample = -bridgerectifier;
		//apply the distortion transform for reals
		highSample /= (1.0f+fabs(treblev*limitingTreb));
		highSample += chebyshev;
		//apply the correction measures
		
		inputSample = lowSample + highSample;

		inP2 = inputSample * inputSample;
		if (inP2 > 1.0f) inP2 = 1.0f; if (inP2 < -1.0f) inP2 = -1.0f;
		chebyshev = (2 * inP2);
		chebyshev *= cheblev;
		//third harmonic max -1
		if (cheblev > 0) cheblev -= IIRscaleback;
		if (cheblev < 0) cheblev += IIRscaleback;
		//this is ShortBuss, IIRscaleback is the decay speed. *2 for second harmonic, and so on
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0.0f) distSample = bridgerectifier;
		else distSample = -bridgerectifier;
		minusSample = inputSample - distSample;
		plusSample = inputSample + distSample;
		if (minusSample > maxfeed) minusSample = maxfeed;
		if (plusSample > maxfeed) plusSample = maxfeed;
		if (plusSample < -maxfeed) plusSample = -maxfeed;
		if (minusSample < -maxfeed) minusSample = -maxfeed;
		if (inputSample > distSample) cheblev += (minusSample*addShortBuss);
		if (inputSample < -distSample) cheblev -= (plusSample*addShortBuss);
		if (cheblev > 1.0f)  cheblev = 1.0f;
		if (cheblev < -1.0f) cheblev = -1.0f;
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0.0f) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//apply the distortion transform for reals
		inputSample /= (1.0f+fabs(cheblev*limiting));
		inputSample += chebyshev;
		//apply the correction measures
		
		if (outgain < 1.0f) {
			inputSample *= outgain;
		}
		
		if (inputSample > 0.95f) inputSample = 0.95f;
		if (inputSample < -0.95f) inputSample = -0.95f;
		//iron bar
		
		
		
		*destP = inputSample;
		//built in output trim and dry/wet by default
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	flip = false;
	basslev = 0.0;
	treblev = 0.0;
	cheblev = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
