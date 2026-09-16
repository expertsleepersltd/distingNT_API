#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "GuitarConditioner"
#define AIRWINDOWS_DESCRIPTION "Like a Tube Screamer voicing without the squishiness and indistinctness."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','u','r' )
#define AIRWINDOWS_TAGS kNT_tagEffect
#define AIRWINDOWS_KERNELS
enum {

	//Add your parameters here...
	kNumberOfParameters=0
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
};
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		uint32_t fpd;
		bool fpFlip;
		Float32 lastSampleT;
		Float32 lastSampleB; //for Slews
		Float32 iirSampleTA;
		Float32 iirSampleTB;
		Float32 iirSampleBA;
		Float32 iirSampleBB; //for Highpasses

		
	
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
	float inputSample;
	float treble;
	float bass;
	Float32 iirTreble = 0.287496f/overallscale; //tight is -1
	Float32 iirBass = 0.085184f/overallscale; //tight is 1
	iirTreble += iirTreble;
	iirBass += iirBass; //simple float when tight is -1 or 1
	Float32 tightBass = 0.6666666666f;
	Float32 tightTreble = -0.3333333333f;
	Float32 offset;
	Float32 clamp;
	Float32 threshTreble = 0.0081f/overallscale;
	Float32 threshBass = 0.0256f/overallscale;

	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		
		treble = bass = inputSample;
		treble += treble; //+3dB on treble as the highpass is higher
		
		offset = (1 + tightTreble) + ((1-fabs(treble))*tightTreble); //treble HP
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1; //made offset for HP
		if (fpFlip) {
			iirSampleTA = (iirSampleTA * (1 - (offset * iirTreble))) + (treble * (offset * iirTreble));
			treble = treble - iirSampleTA;
		} else {
			iirSampleTB = (iirSampleTB * (1 - (offset * iirTreble))) + (treble * (offset * iirTreble));
			treble = treble - iirSampleTB;
		} //done treble HP
		
		offset = (1 - tightBass) + (fabs(bass)*tightBass); //bass HP
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		if (fpFlip) {
			iirSampleBA = (iirSampleBA * (1 - (offset * iirBass))) + (bass * (offset * iirBass));
			bass = bass - iirSampleBA;
		} else {
			iirSampleBB = (iirSampleBB * (1 - (offset * iirBass))) + (bass * (offset * iirBass));
			bass = bass - iirSampleBB;
		} //done bass HP
		
		inputSample = treble;
		clamp = inputSample - lastSampleT;
		if (clamp > threshTreble)
			treble = lastSampleT + threshTreble;
		if (-clamp > threshTreble)
			treble = lastSampleT - threshTreble;
		lastSampleT = treble; //treble slew
		
		inputSample = bass;
		clamp = inputSample - lastSampleB;
		if (clamp > threshBass)
			bass = lastSampleB + threshBass;
		if (-clamp > threshBass)
			bass = lastSampleB - threshBass;
		lastSampleB = bass; //bass slew
		
		inputSample = treble + bass; //final merge
		fpFlip = !fpFlip;
		
		
		
		*destP = inputSample;
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = true;
	lastSampleT = 0.0;
	lastSampleB = 0.0; //for Slews. T for treble, B for bass
	iirSampleTA = 0.0;
	iirSampleTB = 0.0;
	iirSampleBA = 0.0;
	iirSampleBB = 0.0; //for Highpasses
}
};
