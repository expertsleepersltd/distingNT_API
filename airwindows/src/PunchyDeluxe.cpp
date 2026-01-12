#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PunchyDeluxe"
#define AIRWINDOWS_DESCRIPTION "PunchyDeluxe"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','n' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	kParam_H =7,
	kParam_I =8,
	kParam_J =9,
	//Add your parameters here...
	kNumberOfParameters=10
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Hell", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fizz", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pick", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Satan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Danger", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Crtical", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "H Meat", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "L Meat", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Swing", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Rarely", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float angS[18][16];
		float angA[18][16];
		float angG[16];
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
	
	float drive = 1.0f;
	float pad = 1.0f;
	angG[1] = sqrt(GetParameter( kParam_A )*2.0f);
	if (pad > angG[1]) pad = angG[1];
	if (drive < angG[1]) drive = angG[1];
	angG[0] = (1.0f+angG[1])*0.5f; //if boost, boost half
	if (angG[0] > angG[1]) angG[0] = angG[1];
	angG[2] = sqrt(GetParameter( kParam_B )*2.0f);
	if (pad > angG[2]) pad = angG[2];
	if (drive < angG[2]) drive = angG[2];
	angG[3] = sqrt(GetParameter( kParam_C )*2.0f);
	if (pad > angG[3]) pad = angG[3];
	if (drive < angG[3]) drive = angG[3];
	angG[4] = sqrt(GetParameter( kParam_D )*2.0f);
	if (pad > angG[4]) pad = angG[4];
	if (drive < angG[4]) drive = angG[4];
	angG[5] = sqrt(GetParameter( kParam_E )*2.0f);
	if (pad > angG[5]) pad = angG[5];
	if (drive < angG[5]) drive = angG[5];
	angG[6] = sqrt(GetParameter( kParam_F )*2.0f);
	if (pad > angG[6]) pad = angG[6];
	if (drive < angG[6]) drive = angG[6];
	angG[7] = sqrt(GetParameter( kParam_G )*2.0f);
	if (pad > angG[7]) pad = angG[7];
	if (drive < angG[7]) drive = angG[7];
	angG[8] = sqrt(GetParameter( kParam_H )*2.0f);
	if (pad > angG[8]) pad = angG[8];
	if (drive < angG[8]) drive = angG[8];
	angG[9] = sqrt(GetParameter( kParam_I )*2.0f);
	if (pad > angG[9]) pad = angG[9];
	if (drive < angG[9]) drive = angG[9];
	angG[10] = sqrt(GetParameter( kParam_J )*2.0f);
	if (pad > angG[10]) pad = angG[10];
	if (drive < angG[10]) drive = angG[10];
	angG[11] = 1.0f;
	angG[12] = 1.0f;
	pad = sqrt(pad);
	int poles = (int)(drive*11.0f);
	float tune = 0.618f+(overallscale*0.0055f);
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		for (int x = 0; x < poles; x++) {
			float fr = (0.92f/overallscale)+(overallscale*0.01f);
			float band = inputSample; inputSample = 0.0f;
			for (int y = 0; y < 12; y++) {
				angA[x][y] = (angA[x][y]*(1.0f-fr)) + ((band-angS[x][y])*fr);
				float temp = band; band = ((angS[x][y]+(angA[x][y]*fr)) * (1.0f-fr))+(band*fr);
				angS[x][y] = ((angS[x][y]+(angA[x][y]*fr)) * (1.0f-fr))+(band*fr);
				inputSample += ((temp-band)*angG[y]);
				fr *= tune;
			}
			inputSample += band;
			inputSample *= drive;
			inputSample = fmin(fmax(inputSample,-2.032610446872596f),2.032610446872596f);
			float X = inputSample * inputSample;
			float temp = inputSample * X;
			inputSample -= (temp*0.125f); temp *= X;
			inputSample += (temp*0.0078125f); temp *= X;
			inputSample -= (temp*0.000244140625f); temp *= X;
			inputSample += (temp*0.000003814697265625f); temp *= X;
			inputSample -= (temp*0.0000000298023223876953125f); temp *= X;
			//purestsaturation: sine, except all the corrections
			//retain mantissa of a float increasing power function
		}
		
		if (pad < 1.0f) {
			inputSample *= pad;
		}
		
		inputSample = fmin(fmax(inputSample,-2.032610446872596f),2.032610446872596f);
		float X = inputSample * inputSample;
		float temp = inputSample * X;
		inputSample -= (temp*0.125f); temp *= X;
		inputSample += (temp*0.0078125f); temp *= X;
		inputSample -= (temp*0.000244140625f); temp *= X;
		inputSample += (temp*0.000003814697265625f); temp *= X;
		inputSample -= (temp*0.0000000298023223876953125f); temp *= X;
		//purestsaturation: sine, except all the corrections
		//retain mantissa of a float increasing power function
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int x=0; x<17; x++) {
		for(int y=0; y<14; y++) {
			angS[x][y] = 0.0;angA[x][y] = 0.0;
		}
	}
	for(int y=0; y<14; y++) angG[y] = 0.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
