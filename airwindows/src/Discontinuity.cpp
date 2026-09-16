#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Discontinuity"
#define AIRWINDOWS_DESCRIPTION "Models air under intense loudness."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i',' ' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
const int dscBuf = 90;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Top dB", .min = 7000, .max = 14000, .def = 10000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
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
 
		
		float dBaPosL;
		int dBaXL;		
		
		float dBbPosL;
		int dBbXL;		
		
		float dBcPosL;
		int dBcXL;		
				
		uint32_t fpd;
	
	struct _dram {
			float dBaL[dscBuf+5];
		float dBbL[dscBuf+5];
		float dBcL[dscBuf+5];
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
	
	float refdB = GetParameter( kParam_One );
	float topdB = 0.000000075f * powf(10.0f,refdB/20.0f) * overallscale;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		inputSample *= topdB;
		if (inputSample < -0.222f) inputSample = -0.222f; if (inputSample > 0.222f) inputSample = 0.222f;
		//Air Discontinuity A begin
		dram->dBaL[dBaXL] = inputSample; dBaPosL *= 0.5f; dBaPosL += fabs((inputSample*((inputSample*0.25f)-0.5f))*0.5f);
		int dBdly = floor(dBaPosL*dscBuf);
		float dBi = (dBaPosL*dscBuf)-dBdly;
		inputSample = dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
		dBdly++; inputSample += dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBaXL++; if (dBaXL < 0 || dBaXL >= dscBuf) dBaXL = 0;
		//Air Discontinuity A end
		//Air Discontinuity B begin
		dram->dBbL[dBbXL] = inputSample;  dBbPosL *= 0.5f; dBbPosL += fabs((inputSample*((inputSample*0.25f)-0.5f))*0.5f);
		dBdly = floor(dBbPosL*dscBuf); dBi = (dBbPosL*dscBuf)-dBdly;
		inputSample = dram->dBbL[dBbXL-dBdly +((dBbXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
		dBdly++; inputSample += dram->dBbL[dBbXL-dBdly +((dBbXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBbXL++; if (dBbXL < 0 || dBbXL >= dscBuf) dBbXL = 0;
		//Air Discontinuity B end
		//Air Discontinuity C begin
		dram->dBcL[dBcXL] = inputSample;  dBcPosL *= 0.5f; dBcPosL += fabs((inputSample*((inputSample*0.25f)-0.5f))*0.5f);
		dBdly = floor(dBcPosL*dscBuf); dBi = (dBcPosL*dscBuf)-dBdly;
		inputSample = dram->dBcL[dBcXL-dBdly +((dBcXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
		dBdly++; inputSample += dram->dBcL[dBcXL-dBdly +((dBcXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBcXL++; if (dBcXL < 0 || dBcXL >= dscBuf) dBcXL = 0;
		//Air Discontinuity C end
		inputSample /= topdB;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < dscBuf+2; count++) {
		dram->dBaL[count] = 0.0;
		dram->dBbL[count] = 0.0;
		dram->dBcL[count] = 0.0;
	}
	dBaPosL = 0.0;
	dBbPosL = 0.0;
	dBcPosL = 0.0;
	dBaXL = 1;
	dBbXL = 1;
	dBcXL = 1;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
