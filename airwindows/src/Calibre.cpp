#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Calibre"
#define AIRWINDOWS_DESCRIPTION "A re-release of another old Character plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','a','e' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Hardness", .min = 0, .max = 1000, .def = 700, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Personality", .min = 0, .max = 3000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 3000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Level", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 lastSample;
		uint32_t fpd;
	
	struct _dram {
			Float32 b[35];
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
	
	Float32 threshold = GetParameter( kParam_One );
	Float32 hardness;
	Float32 breakup = (1.0f-(threshold/2.0f))*3.14159265358979f;
	Float32 bridgerectifier;
	Float32 sqdrive = GetParameter( kParam_Two );
	if (sqdrive > 1.0f) sqdrive *= sqdrive;
	sqdrive = sqrt(sqdrive);
	Float32 indrive = GetParameter( kParam_Three );
	if (indrive > 1.0f) indrive *= indrive;
	indrive *= (1.0f+(0.1935f*sqdrive));
	//no gain loss of convolution for APIcolypse
	//calibrate this to match noise level with character at 1.0f
	//you get for instance 0.819f and 1.0f-0.819f is 0.181f
	Float32 randy;
	Float32 outlevel = GetParameter( kParam_Four );
	
	if (threshold < 1) hardness = 1.0f / (1.0f-threshold);
	else hardness = 999999999999999999999.0f;
	//set up hardness to exactly fill gap between threshold and 0db
	//if threshold is literally 1 then hardness is infinite, so we make it very big
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		
		inputSample = *sourceP * indrive;
		//calibrated to match gain through convolution and -0.3f correction
		if (sqdrive > 0.0f){
			dram->b[33] = dram->b[32]; dram->b[32] = dram->b[31]; 
			dram->b[31] = dram->b[30]; dram->b[30] = dram->b[29]; dram->b[29] = dram->b[28]; dram->b[28] = dram->b[27]; dram->b[27] = dram->b[26]; dram->b[26] = dram->b[25]; dram->b[25] = dram->b[24]; dram->b[24] = dram->b[23]; 
			dram->b[23] = dram->b[22]; dram->b[22] = dram->b[21]; dram->b[21] = dram->b[20]; dram->b[20] = dram->b[19]; dram->b[19] = dram->b[18]; dram->b[18] = dram->b[17]; dram->b[17] = dram->b[16]; dram->b[16] = dram->b[15]; 
			dram->b[15] = dram->b[14]; dram->b[14] = dram->b[13]; dram->b[13] = dram->b[12]; dram->b[12] = dram->b[11]; dram->b[11] = dram->b[10]; dram->b[10] = dram->b[9]; dram->b[9] = dram->b[8]; dram->b[8] = dram->b[7]; 
			dram->b[7] = dram->b[6]; dram->b[6] = dram->b[5]; dram->b[5] = dram->b[4]; dram->b[4] = dram->b[3]; dram->b[3] = dram->b[2]; dram->b[2] = dram->b[1]; dram->b[1] = dram->b[0]; dram->b[0] = inputSample * sqdrive;
			inputSample -= (dram->b[1] * (0.23505923670562212f  - (0.00028312859289245f*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (0.08188436704577637f  - (0.00008817721351341f*fabs(dram->b[2]))));
			inputSample -= (dram->b[3] * (0.05075798481700617f  - (0.00018817166632483f*fabs(dram->b[3]))));
			inputSample -= (dram->b[4] * (0.00455811821873093f  + (0.00001922902995296f*fabs(dram->b[4]))));
			inputSample -= (dram->b[5] * (0.00027610521433660f  - (0.00013252525469291f*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.03529246280346626f  - (0.00002772989223299f*fabs(dram->b[6]))));
			inputSample += (dram->b[7] * (0.01784111585586136f  + (0.00010230276997291f*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.04394950700298298f  - (0.00005910607126944f*fabs(dram->b[8]))));
			inputSample += (dram->b[9] * (0.01990770780547606f  + (0.00007640328340556f*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.04073629569741782f  - (0.00007712327117090f*fabs(dram->b[10]))));
			inputSample += (dram->b[11] * (0.01349648572795252f  + (0.00005959130575917f*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.03191590248003717f  - (0.00008418000575151f*fabs(dram->b[12]))));
			inputSample += (dram->b[13] * (0.00348795527924766f  + (0.00005489156318238f*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.02198496281481767f  - (0.00008471601187581f*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.00504771152505089f  - (0.00005525060587917f*fabs(dram->b[15]))));
			inputSample -= (dram->b[16] * (0.01391075698598491f  - (0.00007929630732607f*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.01142762504081717f  - (0.00005967036737742f*fabs(dram->b[17]))));
			inputSample -= (dram->b[18] * (0.00893541815021255f  - (0.00007535697758141f*fabs(dram->b[18]))));
			inputSample -= (dram->b[19] * (0.01459704973464936f  - (0.00005969199602841f*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.00694755135226282f  - (0.00006930127097865f*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.01516695630808575f  - (0.00006365800069826f*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.00705917318113651f  - (0.00006497209096539f*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.01420501209177591f  - (0.00006555654576113f*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.00815905656808701f  - (0.00006105622534761f*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.01274326525552961f  - (0.00006542652857017f*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.00937146927845488f  - (0.00006051267868722f*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.01146573981165209f  - (0.00006381511607749f*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.01021294359409007f  - (0.00005930397856398f*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.01065217095323532f  - (0.00006371505438319f*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.01058751196699751f  - (0.00006042857480233f*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.01026557827762401f  - (0.00006007776163871f*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.01060929183604604f  - (0.00006114703012726f*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.01014533525058528f  - (0.00005963567932887f*fabs(dram->b[33]))));}
		
		if (fabs(inputSample) > threshold)
		{
			bridgerectifier = (fabs(inputSample)-threshold)*hardness;
			//skip flat area if any, scale to distortion limit
			if (bridgerectifier > breakup) bridgerectifier = breakup;
			//max value for sine function, 'breakup' modeling for trashed console tone
			//more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
			bridgerectifier = sin(bridgerectifier)/hardness;
			//do the sine factor, scale back to proper amount
			if (inputSample > 0) inputSample = bridgerectifier+threshold;
			else inputSample = -(bridgerectifier+threshold);
		}
		//otherwise we leave it untouched by the overdrive stuff
		randy = ((float(fpd)/UINT32_MAX)*0.042f);
		inputSample = ((inputSample*(1-randy))+(lastSample*randy)) * outlevel;
		lastSample = inputSample;
		
		
		
		

		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 34; count++) {dram->b[count] = 0;}
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
