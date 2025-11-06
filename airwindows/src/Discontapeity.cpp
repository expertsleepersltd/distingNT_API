#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Discontapeity"
#define AIRWINDOWS_DESCRIPTION "Combines air modeling with tape-style overdrive."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i','s' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
const int dscBuf = 256;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "More", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		
		uint32_t fpd;
	
	struct _dram {
			float dBaL[dscBuf+5];
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
	
	float moreDiscontinuity = fmax(powf(GetParameter( kParam_A )*0.42f,3.0f)*overallscale,0.00001f);
	float moreTapeHack = (GetParameter( kParam_A )*1.4152481f)+1.2f;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		//begin Discontinuity section
		inputSample *= moreDiscontinuity;
		dram->dBaL[dBaXL] = inputSample; dBaPosL *= 0.5f; dBaPosL += fabs((inputSample*((inputSample*0.25f)-0.5f))*0.5f);
		dBaPosL = fmin(dBaPosL,1.0f);
		int dBdly = floor(dBaPosL*dscBuf);
		float dBi = (dBaPosL*dscBuf)-dBdly;
		inputSample = dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
		dBdly++; inputSample += dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBaXL++; if (dBaXL < 0 || dBaXL >= dscBuf) dBaXL = 0;
		inputSample /= moreDiscontinuity;
		//end Discontinuity section, begin TapeHack section
		inputSample = fmax(fmin(inputSample*moreTapeHack,2.305929007734908f),-2.305929007734908f);
		float addtwo = inputSample * inputSample;
		float empower = inputSample * addtwo; // inputSample to the third power
		inputSample -= (empower / 6.0f);
		empower *= addtwo; // to the fifth power
		inputSample += (empower / 69.0f);
		empower *= addtwo; //seventh
		inputSample -= (empower / 2530.08f);
		empower *= addtwo; //ninth
		inputSample += (empower / 224985.6f);
		empower *= addtwo; //eleventh
		inputSample -= (empower / 9979200.0f);
		//this is a degenerate form of a Taylor Series to approximate sin()
		inputSample *= 0.9239f;
		//end TapeHack section
				
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < dscBuf+2; count++) {
		dram->dBaL[count] = 0.0;
	}
	dBaPosL = 0.0;
	dBaXL = 1;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
