#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Spiral3"
#define AIRWINDOWS_DESCRIPTION "Spiral3"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','p','3' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Input", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Smooth", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		enum {
			bezierInput_A,bezierInput_B,bezierInput_C,bezierInput_Out,
			bezierOutput_A,bezierOutput_B,bezierOutput_C,bezierOutput_Out,
			bezier_cycle,
			bezier_total
		}; //control smoothing as basic use of bezier curves
		
		enum {
			bip_dvA, bip_dvB, bip_dvC, bip_dvD, bip_pvA, bip_pvB, bip_pvC, bip_pvD,
			bip_total //each distortion section can have one of these, it stacks well
		}; //not remotely elliptic BLEP antialiasing, instead it is derivative BIP :D
		
		uint32_t fpd;
	
	struct _dram {
			float bezier[bezier_total];
		float bip[bip_total];
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

	float input = GetParameter( kParam_A );
	float bezierRez = fmax(powf((1.0f-GetParameter( kParam_B ))*0.25f,3.0f)/overallscale,0.00001f); 
	int stepped = 999999; if (bezierRez > 0.000001f) stepped = (int)(1.0f/bezierRez); bezierRez = 0.99999999f/stepped;
	float bezierTrim = 1.0f-(bezierRez*((float)stepped/(stepped+1.0f)));
	//manages the overall Bezier control smoothing system plugin-wide and feed all controls
	//into dram->bezier[] as just 0-1 values, unprocessed. do it IN the control smoothing engine
	float output = GetParameter( kParam_C );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		//begin Bezier control smoothing engine
		dram->bezier[bezier_cycle] += bezierRez;
		if (dram->bezier[bezier_cycle] > bezierTrim) {dram->bezier[bezier_cycle] = 0.0f;
			dram->bezier[bezierInput_C] = dram->bezier[bezierInput_B]; dram->bezier[bezierInput_B] = dram->bezier[bezierInput_A];
			dram->bezier[bezierOutput_C] = dram->bezier[bezierOutput_B]; dram->bezier[bezierOutput_B] = dram->bezier[bezierOutput_A];
			//one of these bucket brigade lines for every smoothed control
			//begin expensive control calculations
			dram->bezier[bezierInput_A] = input*10.0f;
			dram->bezier[bezierOutput_A] = output;
			//end expensive control calculations
		} float lerp = dram->bezier[bezier_cycle];
		dram->bezier[bezierInput_Out] = (dram->bezier[bezierInput_B]+(dram->bezier[bezierInput_C]*(1.0f-lerp)*(1.0f-lerp))+(dram->bezier[bezierInput_B]*2.0f*(1.0f-lerp)*lerp)+(dram->bezier[bezierInput_A]*lerp*lerp))*0.5f;
		dram->bezier[bezierOutput_Out] = (dram->bezier[bezierOutput_B]+(dram->bezier[bezierOutput_C]*(1.0f-lerp)*(1.0f-lerp))+(dram->bezier[bezierOutput_B]*2.0f*(1.0f-lerp)*lerp)+(dram->bezier[bezierOutput_A]*lerp*lerp))*0.5f;
		//end Bezier control smoothing engine
		
		inputSample *= dram->bezier[bezierInput_Out];
		
		float bip_delta = inputSample; //delta can be just local and re-used
		if (inputSample != 0.0f) {
			inputSample = fmax(fmin(inputSample,1.0f),-1.0f);
			inputSample *= 1.0795f;
			float spiral = inputSample * fabs(inputSample);
			float X = spiral; X *= X;
			float temp = spiral * X;
			spiral -= temp*0.16666666666666666666666666666666666f; temp *= X;//-8 dB
			spiral += temp*0.00833333333333333333333333333333333f; temp *= X;//-20 dB
			spiral -= temp*0.00019841269841269841269841269841269f; temp *= X;//-37 dB
			spiral += temp*0.00000275573192239858906525573192239f; temp *= X;//-55 dB
			spiral -= temp*0.00000002505210838544171877521083854f; temp *= X;//-76 dB
			spiral += temp*0.00000000016059043836821614599392377f; temp *= X;//-108 dB
			spiral -= temp*0.00000000000076471637318198164759011f; temp *= X;//-121 dB
			spiral += temp*0.00000000000000281145725434552076319f; temp *= X;//-146 dB
			spiral -= temp*0.00000000000000000822063524662432971f; temp *= X;//-171 dB
			spiral += temp*0.00000000000000000001957294106339126f;           //-197 dB
			inputSample = spiral / fabs(inputSample); //purest form is full Spiral
		}
		dram->bip[bip_dvA] = bip_delta - inputSample; // these are derivatives: raw clip is position
		dram->bip[bip_dvB] = dram->bip[bip_pvA]-dram->bip[bip_dvA]; dram->bip[bip_pvA] = dram->bip[bip_dvA];//velocity
		dram->bip[bip_dvC] = dram->bip[bip_pvB]-dram->bip[bip_dvB]; dram->bip[bip_pvB] = dram->bip[bip_dvB];//acceleration
		dram->bip[bip_dvD] = dram->bip[bip_pvC]-dram->bip[bip_dvC]; dram->bip[bip_pvC] = dram->bip[bip_dvC];//jerk
		float bip_dvE = dram->bip[bip_pvD]-dram->bip[bip_dvD]; dram->bip[bip_pvD] = dram->bip[bip_dvD];//snap
		inputSample *= (1.0f+(fabs(dram->bip[bip_dvC])*0.0618f)+(fabs(dram->bip[bip_dvD])*-0.05982f)+(fabs(bip_dvE)*0.0206f));

		inputSample *= dram->bezier[bezierOutput_Out];
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bezier_total; x++) dram->bezier[x] = 0.0;
	for (int x = 0; x < bip_total; x++) dram->bip[x] = 0.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
