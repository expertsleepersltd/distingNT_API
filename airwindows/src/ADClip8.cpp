#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ADClip8"
#define AIRWINDOWS_DESCRIPTION "A stacked-up Airwindows loudenator based on ClipOnly2."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','D','8' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,//Add your parameters here...
	kNumberOfParameters=3
};
static const int kNormal = 1;
static const int kGain = 2;
static const int kClip = 3;
static const int kAfterburner = 4;
static const int kExplode = 5;
static const int kNuke = 6;
static const int kApocalypse = 7;
static const int kApotheosis = 8;
static const int kDefaultValue_ParamThree = kNormal;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static char const * const enumStrings2[] = { "", "ADClip Normal", "Gain Match", "Clip Only", "Afterburner", "Explode", "Nuke", "Apocalypse", "Apotheosis", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Boost", .min = 0, .max = 18000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Ceiling", .min = 0, .max = 1000, .def = 883, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mode", .min = 1, .max = 8, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings2 },
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
 
		float lastSample[8];
		float intermediate[16][8];
		bool wasPosClip[8];
		bool wasNegClip[8];
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
	
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;
	//float hardness = 0.618033988749894f; // golden ratio
	//float softness = 0.381966011250105f; // 1.0f - hardness
	//float refclip = 1.618033988749894f; // -0.2dB we're making all this pure raw code
	//refclip*hardness = 1.0f  to use ClipOnly as a prefab code-chunk.
	//refclip*softness = 0.618033988749894f	Seven decimal places is plenty as it's
	//not related to the original sound much: it's an arbitrary position in softening.
	float inputGain = powf(10.0f,(GetParameter( kParam_One ))/20.0f);

	float ceiling = (1.0f+(GetParameter( kParam_Two )*0.23594733f))*0.5f;
	int mode = (int) GetParameter( kParam_Three );
	int stageSetting = mode-2;
	if (stageSetting < 1) stageSetting = 1;
	inputGain = ((inputGain-1.0f)/stageSetting)+1.0f;

	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float overshoot = 0.0f;
		inputSample *= 1.618033988749894f;
		
		for (int stage = 0; stage < stageSetting; stage++) {
			if (inputGain != 1.0f) {
				inputSample *= inputGain;
			}
			if (stage == 0){
				overshoot = fabs(inputSample) - 1.618033988749894f;
				if (overshoot < 0.0f) overshoot = 0.0f;
			}
			if (inputSample > 4.0f) inputSample = 4.0f; if (inputSample < -4.0f) inputSample = -4.0f;
			if (inputSample - lastSample[stage] > 0.618033988749894f) inputSample = lastSample[stage] + 0.618033988749894f;
			if (inputSample - lastSample[stage] < -0.618033988749894f) inputSample = lastSample[stage] - 0.618033988749894f;
			//same as slew clippage
			
			//begin ClipOnly2 as a little, compressed chunk that can be dropped into code
			if (wasPosClip[stage] == true) { //current will be over
				if (inputSample<lastSample[stage]) lastSample[stage]=1.0f+(inputSample*0.381966011250105f);
				else lastSample[stage] = 0.618033988749894f+(lastSample[stage]*0.618033988749894f);
			} wasPosClip[stage] = false;
			if (inputSample>1.618033988749894f) {wasPosClip[stage]=true;inputSample=1.0f+(lastSample[stage]*0.381966011250105f);}
			if (wasNegClip[stage] == true) { //current will be -over
				if (inputSample > lastSample[stage]) lastSample[stage]=-1.0f+(inputSample*0.381966011250105f);
				else lastSample[stage]=-0.618033988749894f+(lastSample[stage]*0.618033988749894f);
			} wasNegClip[stage] = false;
			if (inputSample<-1.618033988749894f) {wasNegClip[stage]=true;inputSample=-1.0f+(lastSample[stage]*0.381966011250105f);}
			intermediate[spacing][stage] = inputSample;
			inputSample = lastSample[stage]; //Latency is however many samples equals one 44.1k sample
			for (int x = spacing; x > 0; x--) intermediate[x-1][stage] = intermediate[x][stage];
			lastSample[stage] = intermediate[0][stage]; //run a little buffer to handle this
			//end ClipOnly2 as a little, compressed chunk that can be dropped into code
		}
		
		switch (mode)
		{
			case 1: break; //Normal
			case 2: inputSample /= inputGain; break; //Gain Match
			case 3: inputSample = overshoot; break; //Clip Only
			case 4: break; //Afterburner
			case 5: break; //Explode
			case 6: break; //Nuke
			case 7: break; //Apocalypse
			case 8: break; //Apotheosis
		}
		//this is our output mode switch, showing the effects
		inputSample *= ceiling;

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int stage = 0; stage < 8; stage++) {
		lastSample[stage] = 0.0;
		wasPosClip[stage] = false;
		wasNegClip[stage] = false;
		for (int x = 0; x < 16; x++) intermediate[x][stage] = 0.0;
	}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
