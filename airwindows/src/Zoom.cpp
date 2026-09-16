#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Zoom"
#define AIRWINDOWS_DESCRIPTION "A bank of Density-like waveshapers for subtly modulating peak energy."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','Z','o','m' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Zoom", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
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
	
	float zoom = (GetParameter( kParam_A )*2.0f)-1.0f;
	float zoomStages = (fabs(zoom)*4.0f)+1.0f;
	for (int count = 0; count < sqrt(zoomStages); count++) zoom *= fabs(zoom);
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		for (int count = 0; count < zoomStages; count++) {
			if (zoom > 0.0f) {
				float closer = inputSample * 1.57079633f;
				if (closer > 1.57079633f) closer = 1.57079633f;
				if (closer < -1.57079633f) closer = -1.57079633f;
				inputSample = (inputSample*(1.0f-zoom))+(sin(closer)*zoom);
			} //zooming in will make the body of the sound louder: it's just Density
			if (zoom < 0.0f) {
				float farther = fabs(inputSample) * 1.57079633f;
				if (farther > 1.57079633f) farther = 1.0f;
				else farther = 1.0f-cos(farther);
				if (inputSample > 0.0f) inputSample = (inputSample*(1.0f+zoom))-(farther*zoom*1.57079633f);
				if (inputSample < 0.0f) inputSample = (inputSample*(1.0f+zoom))+(farther*zoom*1.57079633f);			
			} //zooming out boosts the hottest peaks but cuts back softer stuff
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
