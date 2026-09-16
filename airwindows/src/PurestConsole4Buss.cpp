#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestConsole4Buss"
#define AIRWINDOWS_DESCRIPTION "PurestConsole4Buss"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','v' )
enum {

	kParam_SMO =0,
	kParam_PAN =1,
	kParam_FAD =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Smooth", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"

	enum {
		bezierGainL_A,bezierGainL_B,bezierGainL_C,bezierGainL_Out,
		bezierGainR_A,bezierGainR_B,bezierGainR_C,bezierGainR_Out,
		bezier_cycle,
		bezier_total
	}; //control smoothing as basic use of bezier curves
		
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float bezier[bezier_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	float bezierRez = fmax(powf((1.0f-GetParameter( kParam_SMO ))*0.25f,3.0f)/overallscale,0.00001f); 
	int stepped = 999999; if (bezierRez > 0.000001f) stepped = (int)(1.0f/bezierRez); bezierRez = 0.99999999f/stepped;
	float bezierTrim = 1.0f-(bezierRez*((float)stepped/(stepped+1.0f)));
	//manages the overall Bezier control smoothing system plugin-wide and feed all controls
	//into dram->bezier[] as just 0-1 values, unprocessed. do it IN the control smoothing engine
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		inputSampleL = fmax(fmin(inputSampleL,1.1f),-1.1f);
		inputSampleR = fmax(fmin(inputSampleR,1.1f),-1.1f);
		float X = inputSampleL; X *= X; //float for even
		float temp = inputSampleL * X; //the initial multiplies
		inputSampleL += temp*0.16666666666666666666666666666666666f; temp *= X;
		inputSampleL += temp*0.075f; temp *= X;
		inputSampleL += temp*0.04464285714285714285714285714285714f; temp *= X;
		inputSampleL += temp*0.03038194444444444444444444444444444f; temp *= X;
		inputSampleL += temp*0.02237215909090909090909090909090909f; temp *= X;
		inputSampleL += temp*0.01735276442307692307692307692307692f; temp *= X;
		inputSampleL += temp*0.01396484375f; temp *= X;
		inputSampleL += temp*0.01155180089613970588235294117647058f; temp *= X;
		inputSampleL += temp*0.00976160952919407894736842105263157f; temp *= X;
		inputSampleL += temp*0.00839033580961681547619047619047619f;
		X = inputSampleR; X *= X; //float for even
		temp = inputSampleR * X; //the initial multiplies
		inputSampleR += temp*0.16666666666666666666666666666666666f; temp *= X;
		inputSampleR += temp*0.075f; temp *= X;
		inputSampleR += temp*0.04464285714285714285714285714285714f; temp *= X;
		inputSampleR += temp*0.03038194444444444444444444444444444f; temp *= X;
		inputSampleR += temp*0.02237215909090909090909090909090909f; temp *= X;
		inputSampleR += temp*0.01735276442307692307692307692307692f; temp *= X;
		inputSampleR += temp*0.01396484375f; temp *= X;
		inputSampleR += temp*0.01155180089613970588235294117647058f; temp *= X;
		inputSampleR += temp*0.00976160952919407894736842105263157f; temp *= X;
		inputSampleR += temp*0.00839033580961681547619047619047619f;
		//retain mantissa of a float increasing power function
		//float probably doesn't handle more than 36 digits or so
		
		//begin Bezier control smoothing engine
		dram->bezier[bezier_cycle] += bezierRez;
		if (dram->bezier[bezier_cycle] > bezierTrim) {dram->bezier[bezier_cycle] = 0.0f;
			dram->bezier[bezierGainL_C] = dram->bezier[bezierGainL_B]; dram->bezier[bezierGainL_B] = dram->bezier[bezierGainL_A];
			dram->bezier[bezierGainR_C] = dram->bezier[bezierGainR_B]; dram->bezier[bezierGainR_B] = dram->bezier[bezierGainR_A];
			//one of these bucket brigade lines for every smoothed control
			//begin expensive control calculations
			float gain = powf(GetParameter( kParam_FAD ),2.0f)*1.414213562373094f; //Pan will pad this
			dram->bezier[bezierGainL_A] = gain*sin(M_PI_2-(GetParameter( kParam_PAN )*M_PI_2));
			dram->bezier[bezierGainR_A] = gain*sin(GetParameter( kParam_PAN )*M_PI_2);
			//end expensive control calculations
		} float lerp = dram->bezier[bezier_cycle];
		dram->bezier[bezierGainL_Out] = (dram->bezier[bezierGainL_B]+(dram->bezier[bezierGainL_C]*(1.0f-lerp)*(1.0f-lerp))+(dram->bezier[bezierGainL_B]*2.0f*(1.0f-lerp)*lerp)+(dram->bezier[bezierGainL_A]*lerp*lerp))*0.5f;
		dram->bezier[bezierGainR_Out] = (dram->bezier[bezierGainR_B]+(dram->bezier[bezierGainR_C]*(1.0f-lerp)*(1.0f-lerp))+(dram->bezier[bezierGainR_B]*2.0f*(1.0f-lerp)*lerp)+(dram->bezier[bezierGainR_A]*lerp*lerp))*0.5f;
		//end Bezier control smoothing engine
		
		inputSampleL *= dram->bezier[bezierGainL_Out];
		inputSampleR *= dram->bezier[bezierGainR_Out];
				
		
		
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
	for (int x = 0; x < bezier_total; x++) dram->bezier[x] = 0.0;

	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
