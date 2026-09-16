#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ResEQ"
#define AIRWINDOWS_DESCRIPTION "A bank of mostly midrange resonances."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','R','e','Q' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	kParam_Seven =6,
	kParam_Eight =7,
	kParam_Nine =8,
	//Add your parameters here...
	kNumberOfParameters=9
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Reso 1", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso 2", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso 3", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso 4", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso 5", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso 6", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso 7", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso 8", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		int framenumber;
		uint32_t fpd;
	
	struct _dram {
			Float32 b[61];
		Float32 f[61];
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
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 v1 = GetParameter( kParam_One );
	Float32 v2 = GetParameter( kParam_Two );
	Float32 v3 = GetParameter( kParam_Three );
	Float32 v4 = GetParameter( kParam_Four );
	Float32 v5 = GetParameter( kParam_Five );
	Float32 v6 = GetParameter( kParam_Six );
	Float32 v7 = GetParameter( kParam_Seven );
	Float32 v8 = GetParameter( kParam_Eight );
	Float32 f1 = powf(v1,2);
	Float32 f2 = powf(v2,2);
	Float32 f3 = powf(v3,2);
	Float32 f4 = powf(v4,2);
	Float32 f5 = powf(v5,2);
	Float32 f6 = powf(v6,2);
	Float32 f7 = powf(v7,2);
	Float32 f8 = powf(v8,2);
	Float32 wet = GetParameter( kParam_Nine );
	Float32 falloff;
	v1 += 0.2f; v2 += 0.2f; v3 += 0.2f; v4 += 0.2f; v5 += 0.2f; v6 += 0.2f; v7 += 0.2f; v8 += 0.2f;
	v1 /= overallscale;
	v2 /= overallscale;
	v3 /= overallscale;
	v4 /= overallscale;
	v5 /= overallscale;
	v6 /= overallscale;
	v7 /= overallscale;
	v8 /= overallscale;
	//each process frame we'll update some of the kernel frames. That way we don't have to crunch the whole thing at once,
	//and we can load a LOT more resonant peaks into the kernel.
	
	//amount = 0.005f + (amount * 0.99f);
	
	
	//mAudioUnit->SetParameter( kParam_Nine, amount );
	
	//		mAudioUnit->Globals()->SetParameter( kParam_Nine, amount );
	
	//		AudioUnitParameter param;
	//		param.mAudioUnit	= mAudioUnit->GetComponentInstance();	
	//		param.mScope 		= kAudioUnitScope_Global;
	//		param.mParameterID	= kParam_Nine;
	//		AUParameterListenerNotify( NULL, NULL, &param);
	
	
	
	
	
	framenumber += 1; if (framenumber > 59) framenumber = 1;
	falloff = sin(framenumber / 19.098992f);
	dram->f[framenumber] = 0.0f;
	if ((framenumber * f1) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f1)*2.0f) * falloff * v1);
	else dram->f[framenumber]  += (cos(framenumber * f1) * falloff * v1);
	if ((framenumber * f2) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f2)*2.0f) * falloff * v2);
	else dram->f[framenumber]  += (cos(framenumber * f2) * falloff * v2);
	if ((framenumber * f3) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f3)*2.0f) * falloff * v3);
	else dram->f[framenumber]  += (cos(framenumber * f3) * falloff * v3);
	if ((framenumber * f4) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f4)*2.0f) * falloff * v4);
	else dram->f[framenumber]  += (cos(framenumber * f4) * falloff * v4);
	if ((framenumber * f5) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f5)*2.0f) * falloff * v5);
	else dram->f[framenumber]  += (cos(framenumber * f5) * falloff * v5);
	if ((framenumber * f6) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f6)*2.0f) * falloff * v6);
	else dram->f[framenumber]  += (cos(framenumber * f6) * falloff * v6);
	if ((framenumber * f7) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f7)*2.0f) * falloff * v7);
	else dram->f[framenumber]  += (cos(framenumber * f7) * falloff * v7);
	if ((framenumber * f8) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f8)*2.0f) * falloff * v8);
	else dram->f[framenumber]  += (cos(framenumber * f8) * falloff * v8);

	framenumber += 1; if (framenumber > 59) framenumber = 1;
	falloff = sin(framenumber / 19.098992f);
	dram->f[framenumber] = 0.0f;
	if ((framenumber * f1) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f1)*2.0f) * falloff * v1);
	else dram->f[framenumber]  += (cos(framenumber * f1) * falloff * v1);
	if ((framenumber * f2) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f2)*2.0f) * falloff * v2);
	else dram->f[framenumber]  += (cos(framenumber * f2) * falloff * v2);
	if ((framenumber * f3) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f3)*2.0f) * falloff * v3);
	else dram->f[framenumber]  += (cos(framenumber * f3) * falloff * v3);
	if ((framenumber * f4) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f4)*2.0f) * falloff * v4);
	else dram->f[framenumber]  += (cos(framenumber * f4) * falloff * v4);
	if ((framenumber * f5) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f5)*2.0f) * falloff * v5);
	else dram->f[framenumber]  += (cos(framenumber * f5) * falloff * v5);
	if ((framenumber * f6) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f6)*2.0f) * falloff * v6);
	else dram->f[framenumber]  += (cos(framenumber * f6) * falloff * v6);
	if ((framenumber * f7) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f7)*2.0f) * falloff * v7);
	else dram->f[framenumber]  += (cos(framenumber * f7) * falloff * v7);
	if ((framenumber * f8) < 1.57079633f) dram->f[framenumber]  += (sin((framenumber * f8)*2.0f) * falloff * v8);
	else dram->f[framenumber]  += (cos(framenumber * f8) * falloff * v8);

	//done updating the kernel for this go-round
		
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
dram->b[59] = dram->b[58]; dram->b[58] = dram->b[57]; dram->b[57] = dram->b[56]; dram->b[56] = dram->b[55]; dram->b[55] = dram->b[54]; dram->b[54] = dram->b[53]; dram->b[53] = dram->b[52]; dram->b[52] = dram->b[51]; dram->b[51] = dram->b[50]; dram->b[50] = dram->b[49]; dram->b[49] = dram->b[48];
dram->b[48] = dram->b[47]; dram->b[47] = dram->b[46]; dram->b[46] = dram->b[45]; dram->b[45] = dram->b[44]; dram->b[44] = dram->b[43]; dram->b[43] = dram->b[42]; dram->b[42] = dram->b[41]; dram->b[41] = dram->b[40]; dram->b[40] = dram->b[39]; dram->b[39] = dram->b[38];
dram->b[38] = dram->b[37]; dram->b[37] = dram->b[36]; dram->b[36] = dram->b[35]; dram->b[35] = dram->b[34]; dram->b[34] = dram->b[33]; dram->b[33] = dram->b[32]; dram->b[32] = dram->b[31]; dram->b[31] = dram->b[30]; dram->b[30] = dram->b[29]; dram->b[29] = dram->b[28]; 
dram->b[28] = dram->b[27]; dram->b[27] = dram->b[26]; dram->b[26] = dram->b[25]; dram->b[25] = dram->b[24]; dram->b[24] = dram->b[23]; dram->b[23] = dram->b[22]; dram->b[22] = dram->b[21]; dram->b[21] = dram->b[20]; dram->b[20] = dram->b[19]; dram->b[19] = dram->b[18];
dram->b[18] = dram->b[17]; dram->b[17] = dram->b[16]; dram->b[16] = dram->b[15]; dram->b[15] = dram->b[14]; dram->b[14] = dram->b[13]; dram->b[13] = dram->b[12]; dram->b[12] = dram->b[11]; dram->b[11] = dram->b[10]; dram->b[10] = dram->b[9]; dram->b[9] = dram->b[8]; dram->b[8] = dram->b[7]; 
dram->b[7] = dram->b[6]; dram->b[6] = dram->b[5]; dram->b[5] = dram->b[4]; dram->b[4] = dram->b[3]; dram->b[3] = dram->b[2]; dram->b[2] = dram->b[1]; dram->b[1] = dram->b[0]; dram->b[0] = inputSample;
		
		inputSample = (dram->b[1] * dram->f[1]);
		inputSample += (dram->b[2] * dram->f[2]);
		inputSample += (dram->b[3] * dram->f[3]);
		inputSample += (dram->b[4] * dram->f[4]);
		inputSample += (dram->b[5] * dram->f[5]);
		inputSample += (dram->b[6] * dram->f[6]);
		inputSample += (dram->b[7] * dram->f[7]);
		inputSample += (dram->b[8] * dram->f[8]);
		inputSample += (dram->b[9] * dram->f[9]);
		inputSample += (dram->b[10] * dram->f[10]);
		inputSample += (dram->b[11] * dram->f[11]);
		inputSample += (dram->b[12] * dram->f[12]);
		inputSample += (dram->b[13] * dram->f[13]);
		inputSample += (dram->b[14] * dram->f[14]);
		inputSample += (dram->b[15] * dram->f[15]);
		inputSample += (dram->b[16] * dram->f[16]);
		inputSample += (dram->b[17] * dram->f[17]);
		inputSample += (dram->b[18] * dram->f[18]);
		inputSample += (dram->b[19] * dram->f[19]);
		inputSample += (dram->b[20] * dram->f[20]);
		inputSample += (dram->b[21] * dram->f[21]);
		inputSample += (dram->b[22] * dram->f[22]);
		inputSample += (dram->b[23] * dram->f[23]);
		inputSample += (dram->b[24] * dram->f[24]);
		inputSample += (dram->b[25] * dram->f[25]);
		inputSample += (dram->b[26] * dram->f[26]);
		inputSample += (dram->b[27] * dram->f[27]);
		inputSample += (dram->b[28] * dram->f[28]);
		inputSample += (dram->b[29] * dram->f[29]);
		inputSample += (dram->b[30] * dram->f[30]);
		inputSample += (dram->b[31] * dram->f[31]);
		inputSample += (dram->b[32] * dram->f[32]);
		inputSample += (dram->b[33] * dram->f[33]);
		inputSample += (dram->b[34] * dram->f[34]);
		inputSample += (dram->b[35] * dram->f[35]);
		inputSample += (dram->b[36] * dram->f[36]);
		inputSample += (dram->b[37] * dram->f[37]);
		inputSample += (dram->b[38] * dram->f[38]);
		inputSample += (dram->b[39] * dram->f[39]);
		inputSample += (dram->b[40] * dram->f[40]);
		inputSample += (dram->b[41] * dram->f[41]);
		inputSample += (dram->b[42] * dram->f[42]);
		inputSample += (dram->b[43] * dram->f[43]);
		inputSample += (dram->b[44] * dram->f[44]);
		inputSample += (dram->b[45] * dram->f[45]);
		inputSample += (dram->b[46] * dram->f[46]);
		inputSample += (dram->b[47] * dram->f[47]);
		inputSample += (dram->b[48] * dram->f[48]);
		inputSample += (dram->b[49] * dram->f[49]);
		inputSample += (dram->b[50] * dram->f[50]);
		inputSample += (dram->b[51] * dram->f[51]);
		inputSample += (dram->b[52] * dram->f[52]);
		inputSample += (dram->b[53] * dram->f[53]);
		inputSample += (dram->b[54] * dram->f[54]);
		inputSample += (dram->b[55] * dram->f[55]);
		inputSample += (dram->b[56] * dram->f[56]);
		inputSample += (dram->b[57] * dram->f[57]);
		inputSample += (dram->b[58] * dram->f[58]);
		inputSample += (dram->b[59] * dram->f[59]);
		inputSample /= 12.0f;
		//inlined- this is our little EQ kernel. Longer will give better tightness on bass frequencies.
		inputSample = (drySample*(1.0f-wet))+(inputSample*wet);

		
		

		*destP = inputSample;
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	register UInt32 count;
	for(count = 0; count < 60; count++) {dram->b[count] = 0.0; dram->f[count] = 0.0;}
	framenumber = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
