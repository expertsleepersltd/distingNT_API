#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Distance3"
#define AIRWINDOWS_DESCRIPTION "Combines the best parts of Distance and Discontinuity."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i','x' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
const int dscBuf = 90;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Distance", .min = 0, .max = 10000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Top dB", .min = 7000, .max = 14000, .def = 10000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		
		float lastclampAL;
		float clampAL;
		float changeAL;
		float prevresultAL;
		float lastAL;
		
		float lastclampBL;
		float clampBL;
		float changeBL;
		float prevresultBL;
		float lastBL;
		
		float lastclampCL;
		float clampCL;
		float changeCL;
		float prevresultCL;
		float lastCL;
		
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

	float softslew = (GetParameter( kParam_One )*10.0f)+0.5f;
	softslew *= overallscale;
	float outslew = softslew * (1.0f-(GetParameter( kParam_One )*0.0333f));
	float refdB = GetParameter( kParam_Two );
	float topdB = 0.000000075f * powf(10.0f,refdB/20.0f) * overallscale;
	float wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		float drySampleL = inputSampleL;
		
		inputSampleL *= softslew;
		lastclampAL = clampAL; clampAL = inputSampleL - lastAL;
		float postfilter = changeAL = fabs(clampAL - lastclampAL);
		postfilter += (softslew / 2.0f);
		inputSampleL /= outslew;
		inputSampleL += (prevresultAL * postfilter);
		inputSampleL /= (postfilter + 1.0f);
		prevresultAL = inputSampleL;
		//do an IIR like thing to further squish superdistant stuff
		
		inputSampleL *= topdB;
		if (inputSampleL < -0.222f) inputSampleL = -0.222f; if (inputSampleL > 0.222f) inputSampleL = 0.222f;
		//Air Discontinuity A begin
		dram->dBaL[dBaXL] = inputSampleL; dBaPosL *= 0.5f; dBaPosL += fabs((inputSampleL*((inputSampleL*0.25f)-0.5f))*0.5f);
		int dBdly = floor(dBaPosL*dscBuf);
		float dBi = (dBaPosL*dscBuf)-dBdly;
		inputSampleL = dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
		dBdly++; inputSampleL += dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBaXL++; if (dBaXL < 0 || dBaXL >= dscBuf) dBaXL = 0;
		//Air Discontinuity A end
		inputSampleL /= topdB;
		
		inputSampleL *= softslew;
		lastclampBL = clampBL; clampBL = inputSampleL - lastBL;
		postfilter = changeBL = fabs(clampBL - lastclampBL);
		postfilter += (softslew / 2.0f);
		lastBL = inputSampleL;
		inputSampleL /= outslew;
		inputSampleL += (prevresultBL * postfilter);
		inputSampleL /= (postfilter + 1.0f);
		prevresultBL = inputSampleL;
		//do an IIR like thing to further squish superdistant stuff
		
		inputSampleL *= topdB;
		if (inputSampleL < -0.222f) inputSampleL = -0.222f; if (inputSampleL > 0.222f) inputSampleL = 0.222f;
		//Air Discontinuity B begin
		dram->dBbL[dBbXL] = inputSampleL;  dBbPosL *= 0.5f; dBbPosL += fabs((inputSampleL*((inputSampleL*0.25f)-0.5f))*0.5f);
		dBdly = floor(dBbPosL*dscBuf); dBi = (dBbPosL*dscBuf)-dBdly;
		inputSampleL = dram->dBbL[dBbXL-dBdly +((dBbXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
		dBdly++; inputSampleL += dram->dBbL[dBbXL-dBdly +((dBbXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBbXL++; if (dBbXL < 0 || dBbXL >= dscBuf) dBbXL = 0;
		//Air Discontinuity B end
		inputSampleL /= topdB;
		
		inputSampleL *= softslew;
		lastclampCL = clampCL; clampCL = inputSampleL - lastCL;
		postfilter = changeCL = fabs(clampCL - lastclampCL);
		postfilter += (softslew / 2.0f);
		lastCL = inputSampleL;
		inputSampleL /= softslew; //don't boost the final time!
		inputSampleL += (prevresultCL * postfilter);
		inputSampleL /= (postfilter + 1.0f);
		prevresultCL = inputSampleL;
		//do an IIR like thing to further squish superdistant stuff
		
		inputSampleL *= topdB;
		if (inputSampleL < -0.222f) inputSampleL = -0.222f; if (inputSampleL > 0.222f) inputSampleL = 0.222f;		
		//Air Discontinuity C begin
		dram->dBcL[dBcXL] = inputSampleL;  dBcPosL *= 0.5f; dBcPosL += fabs((inputSampleL*((inputSampleL*0.25f)-0.5f))*0.5f);
		dBdly = floor(dBcPosL*dscBuf); dBi = (dBcPosL*dscBuf)-dBdly;
		inputSampleL = dram->dBcL[dBcXL-dBdly +((dBcXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
		dBdly++; inputSampleL += dram->dBcL[dBcXL-dBdly +((dBcXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBcXL++; if (dBcXL < 0 || dBcXL >= dscBuf) dBcXL = 0;
		//Air Discontinuity C end
		inputSampleL /= topdB;
		
		if (wet < 1.0f) inputSampleL = (drySampleL * (1.0f-wet))+(inputSampleL*wet);
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	prevresultAL = lastclampAL = clampAL = changeAL = lastAL = 0.0;
	prevresultBL = lastclampBL = clampBL = changeBL = lastBL = 0.0;
	prevresultCL = lastclampCL = clampCL = changeCL = lastCL = 0.0;

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
