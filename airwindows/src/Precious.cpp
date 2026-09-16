#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Precious"
#define AIRWINDOWS_DESCRIPTION "A re-release of another old Character plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','r','s' )
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
	indrive *= (1.0f-(0.2095f*sqdrive));
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
		
		inputSample *= indrive;
		//calibrated to match gain through convolution and -0.3f correction
		if (sqdrive > 0.0f){
			dram->b[33] = dram->b[32]; dram->b[32] = dram->b[31]; 
			dram->b[31] = dram->b[30]; dram->b[30] = dram->b[29]; dram->b[29] = dram->b[28]; dram->b[28] = dram->b[27]; dram->b[27] = dram->b[26]; dram->b[26] = dram->b[25]; dram->b[25] = dram->b[24]; dram->b[24] = dram->b[23]; 
			dram->b[23] = dram->b[22]; dram->b[22] = dram->b[21]; dram->b[21] = dram->b[20]; dram->b[20] = dram->b[19]; dram->b[19] = dram->b[18]; dram->b[18] = dram->b[17]; dram->b[17] = dram->b[16]; dram->b[16] = dram->b[15]; 
			dram->b[15] = dram->b[14]; dram->b[14] = dram->b[13]; dram->b[13] = dram->b[12]; dram->b[12] = dram->b[11]; dram->b[11] = dram->b[10]; dram->b[10] = dram->b[9]; dram->b[9] = dram->b[8]; dram->b[8] = dram->b[7]; 
			dram->b[7] = dram->b[6]; dram->b[6] = dram->b[5]; dram->b[5] = dram->b[4]; dram->b[4] = dram->b[3]; dram->b[3] = dram->b[2]; dram->b[2] = dram->b[1]; dram->b[1] = dram->b[0]; dram->b[0] = inputSample * sqdrive;
			inputSample += (dram->b[1] * (0.59188440274551890f  - (0.00008361469668405f*fabs(dram->b[1]))));
			inputSample -= (dram->b[2] * (0.24439750948076133f  + (0.00002651678396848f*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (0.14109876103205621f  - (0.00000840487181372f*fabs(dram->b[3]))));
			inputSample -= (dram->b[4] * (0.10053507128157971f  + (0.00001768100964598f*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (0.05859287880626238f  - (0.00000361398065989f*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.04337406889823660f  + (0.00000735941182117f*fabs(dram->b[6]))));
			inputSample += (dram->b[7] * (0.01589900680531097f  + (0.00000207347387987f*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.01087234854973281f  + (0.00000732123412029f*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.00845782429679176f  - (0.00000133058605071f*fabs(dram->b[9]))));
			inputSample += (dram->b[10] * (0.00662278586618295f  - (0.00000424594730611f*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.02000592193760155f  + (0.00000632896879068f*fabs(dram->b[11]))));
			inputSample += (dram->b[12] * (0.01321157777167565f  - (0.00001421171592570f*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.02249955362988238f  + (0.00000163937127317f*fabs(dram->b[13]))));
			inputSample += (dram->b[14] * (0.01196492077581504f  - (0.00000535385220676f*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.01905917427000097f  + (0.00000121672882030f*fabs(dram->b[15]))));
			inputSample += (dram->b[16] * (0.00761909482108073f  - (0.00000326242895115f*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.01362744780256239f  + (0.00000359274216003f*fabs(dram->b[17]))));
			inputSample += (dram->b[18] * (0.00200183122683721f  - (0.00000089207452791f*fabs(dram->b[18]))));
			inputSample -= (dram->b[19] * (0.00833042637239315f  + (0.00000946767677294f*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.00258481175207224f  - (0.00000087429351464f*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.00459744479712244f  - (0.00000049519758701f*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.00534277030993820f  + (0.00000397547847155f*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.00272332919605675f  + (0.00000040077229097f*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.00637243782359372f  - (0.00000139419072176f*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.00233001590327504f  + (0.00000420129915747f*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.00623296727793041f  + (0.00000019010664856f*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.00276177096376805f  + (0.00000580301901385f*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.00559184754866264f  + (0.00000080597287792f*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.00343180144395919f  - (0.00000243701142085f*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.00493325428861701f  + (0.00000300985740900f*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.00396140827680823f  - (0.00000051459681789f*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.00448497879902493f  + (0.00000744412841743f*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.00425146888772076f  - (0.00000082346016542f*fabs(dram->b[33]))));}
		//Precision 8
		
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
		randy = ((float(fpd)/UINT32_MAX)*0.017f);
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
