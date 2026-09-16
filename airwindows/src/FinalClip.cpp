#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "FinalClip"
#define AIRWINDOWS_DESCRIPTION "One stage of ADClip8 set up to clip +6dB for Final Cut Pro."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','i','p' )
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
 
		Float32 lastSample;  //Float32 as this runs in pure bypass most of the time
		Float32 intermediate[16];
		bool wasPosClip;
		bool wasNegClip;
		//uint32_t fpd; //leave off
	
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
	
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;
	//float hardness = 0.618033988749894f; // golden ratio
	//float softness = 0.381966011250105f; // 1.0f - hardness
	//float refclip = 1.618033988749894f; // -0.2dB we're making all this pure raw code
	//refclip*hardness = 1.0f  to use ClipOnly as a prefab code-chunk.
	//refclip*softness = 0.618033988749894f	Seven decimal places is plenty as it's
	//not related to the original sound much: it's an arbitrary position in softening.

	while (nSampleFrames-- > 0) {
		Float32 inputSample = *sourceP;
		
		if (inputSample > 4.0f) inputSample = 4.0f; if (inputSample < -4.0f) inputSample = -4.0f;
		if (inputSample - lastSample > 0.618033988749894f) inputSample = lastSample + 0.618033988749894f;
		if (inputSample - lastSample < -0.618033988749894f) inputSample = lastSample - 0.618033988749894f;
		//same as slew clippage
		
		//begin ClipOnly2 as a little, compressed chunk that can be dropped into code
		if (wasPosClip == true) { //current will be over
			if (inputSample<lastSample) lastSample=1.0f+(inputSample*0.381966011250105f);
			else lastSample = 0.618033988749894f+(lastSample*0.618033988749894f);
		} wasPosClip = false;
		if (inputSample>1.618033988749894f) {wasPosClip=true;inputSample=1.0f+(lastSample*0.381966011250105f);}
		if (wasNegClip == true) { //current will be -over
			if (inputSample > lastSample) lastSample=-1.0f+(inputSample*0.381966011250105f);
			else lastSample=-0.618033988749894f+(lastSample*0.618033988749894f);
		} wasNegClip = false;
		if (inputSample<-1.618033988749894f) {wasNegClip=true;inputSample=-1.0f+(lastSample*0.381966011250105f);}
		intermediate[spacing] = inputSample;
        inputSample = lastSample; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediate[x-1] = intermediate[x];
		lastSample = intermediate[0]; //run a little buffer to handle this
		//end ClipOnly2 as a little, compressed chunk that can be dropped into code
		
        *destP = inputSample; //this one doesn't get dithering as it is almost always pure bypass
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	wasPosClip = false;
	wasNegClip = false;
	for (int x = 0; x < 16; x++) intermediate[x] = 0.0;
	//fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX; //leave off for ClipOnly
}
};
