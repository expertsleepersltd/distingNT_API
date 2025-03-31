#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Recurve"
#define AIRWINDOWS_DESCRIPTION "A special buss compressor with no threshold point."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','R','e','c' )
enum {

	//Add your parameters here...
	kNumberOfParameters=0
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
};
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"

	double gain;
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
struct _dram {
};
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
		
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;

		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		inputSampleL *= 0.5;
		inputSampleR *= 0.5;
		
		if (gain < 0.0078125) gain = 0.0078125; if (gain > 2.0) gain = 2.0;
		//gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
		//smaller number is maximum clamping, if too small it'll take a while to bounce back
		inputSampleL *= gain; inputSampleR *= gain;
		
		gain += sin((fabs(inputSampleL*4)>1)?4:fabs(inputSampleL*4))*pow(inputSampleL,4);
		gain += sin((fabs(inputSampleR*4)>1)?4:fabs(inputSampleR*4))*pow(inputSampleR,4);
		//4.71239 radians sined will turn to -1 which is the maximum gain reduction speed
		
		inputSampleL *= 2.0;
		inputSampleR *= 2.0;
		
		if (inputSampleL > 0.9999) inputSampleL = 0.9999;
		if (inputSampleR > 0.9999) inputSampleR = 0.9999;
		if (inputSampleL < -0.9999) inputSampleL = -0.9999;
		if (inputSampleR < -0.9999) inputSampleR = -0.9999;
		//iron bar so people can play with this as a loudenator
		
		//begin 32 bit stereo floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		frexpf((float)inputSampleR, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit stereo floating point dither
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//direct stereo out
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	gain = 2.0; //default to gain push
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
