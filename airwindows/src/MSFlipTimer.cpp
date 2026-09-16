#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "MSFlipTimer"
#define AIRWINDOWS_DESCRIPTION "A utility that swaps stereo with mono every few (1-10) minutes."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','S','r' )
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Minutes", .min = 1000, .max = 10000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	uint32_t tick;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	
	uint32_t time = (uint32_t)(GetParameter( kParam_One ) * 60.0f * GetSampleRate());
	uint32_t transition = (uint32_t)(GetSampleRate()*0.1f);
	//number of minutes * 60 seconds * samples per second
	//transition: tenth of a second crossfade
	
	while (nSampleFrames-- > 0) {
		Float32 inputSampleL = *inputL;
		Float32 inputSampleR = *inputR;
		Float32 outputSampleL = inputSampleL;
		Float32 outputSampleR = inputSampleR;
		
		tick++;
		if (tick > time*2) tick = 0;
		
		if (tick < time) {
			if (tick < transition) {
				float crossfade;
				crossfade = tick;
				crossfade /= transition;
				//0 to 1 from beginning mono to end as direct stereo out
				float mono = (inputSampleL+inputSampleR)*0.5f;
				if (crossfade < 0.5f) {
					outputSampleL = (inputSampleL*crossfade)+(mono*(1.0f-crossfade));
					outputSampleR = (inputSampleR*crossfade)+(mono*(1.0f-crossfade));
				} else {
					crossfade = fabs(crossfade-1.0f);
					outputSampleL = (mono*crossfade)+(inputSampleL*(1.0f-crossfade));
					outputSampleR = (mono*crossfade)+(inputSampleR*(1.0f-crossfade));
				}
			} else {
				outputSampleL = inputSampleL;
				outputSampleR = inputSampleR;
				//direct stereo out
			}
		} else {
			if ((tick - time) < transition) {
				float crossfade;
				crossfade = tick-time;
				crossfade /= transition;
				//0 to 1 from beginning direct to end as mono
				float mono = (inputSampleL+inputSampleR)*0.5f;
				if (crossfade < 0.5f) {
					outputSampleL = (mono*crossfade)+(inputSampleL*(1.0f-crossfade));
					outputSampleR = (mono*crossfade)+(inputSampleR*(1.0f-crossfade));
				} else {
					crossfade = fabs(crossfade-1.0f);
					outputSampleL = (inputSampleL*crossfade)+(mono*(1.0f-crossfade));
					outputSampleR = (inputSampleR*crossfade)+(mono*(1.0f-crossfade));
				}
			} else {
				float mono = (inputSampleL+inputSampleR)*0.5f;
				outputSampleL = outputSampleR = mono;
				//mono out
			}
		}
		
		*outputL = outputSampleL;
		*outputR = outputSampleR;
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	tick = 0;
	return noErr;
}

};
