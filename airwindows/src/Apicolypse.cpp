#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Apicolypse"
#define AIRWINDOWS_DESCRIPTION "A re-release of my old API-style color adder, exacly as it was."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','p','e' )
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
	indrive *= (1.0f-(0.008f*sqdrive));
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
			inputSample += (dram->b[1] * (0.09299870608542582f  - (0.00009582362368873f*fabs(dram->b[1]))));
			inputSample -= (dram->b[2] * (0.11947847710741009f  - (0.00004500891602770f*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (0.09071606264761795f  + (0.00005639498984741f*fabs(dram->b[3]))));
			inputSample -= (dram->b[4] * (0.08561982770836980f  - (0.00004964855606916f*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (0.06440549220820363f  + (0.00002428052139507f*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.05987991812840746f  + (0.00000101867082290f*fabs(dram->b[6]))));
			inputSample += (dram->b[7] * (0.03980233135839382f  + (0.00003312430049041f*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.03648402630896925f  - (0.00002116186381142f*fabs(dram->b[8]))));
			inputSample += (dram->b[9] * (0.01826860869525248f  + (0.00003115110025396f*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.01723968622495364f  - (0.00002450634121718f*fabs(dram->b[10]))));
			inputSample += (dram->b[11] * (0.00187588812316724f  + (0.00002838206198968f*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.00381796423957237f  - (0.00003155815499462f*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.00852092214496733f  - (0.00001702651162392f*fabs(dram->b[13]))));
			inputSample += (dram->b[14] * (0.00315560292270588f  + (0.00002547861676047f*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.01258630914496868f  - (0.00004555319243213f*fabs(dram->b[15]))));
			inputSample += (dram->b[16] * (0.00536435648963575f  + (0.00001812393657101f*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.01272975658159178f  - (0.00004103775306121f*fabs(dram->b[17]))));
			inputSample += (dram->b[18] * (0.00403818975172755f  + (0.00003764615492871f*fabs(dram->b[18]))));
			inputSample -= (dram->b[19] * (0.01042617366897483f  - (0.00003605210426041f*fabs(dram->b[19]))));
			inputSample += (dram->b[20] * (0.00126599583390057f  + (0.00004305458668852f*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.00747876207688339f  - (0.00003731207018977f*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.00149873689175324f  - (0.00005086601800791f*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.00503221309488033f  - (0.00003636086782783f*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.00342998224655821f  - (0.00004103091180506f*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.00355585977903117f  - (0.00003698982145400f*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.00437201792934817f  - (0.00002720235666939f*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.00299217874451556f  - (0.00004446954727956f*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.00457924652487249f  - (0.00003859065778860f*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.00298182934892027f  - (0.00002064710931733f*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.00438838441540584f  - (0.00005223008424866f*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.00323984218794705f  - (0.00003397987535887f*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.00407693981307314f  - (0.00003935772436894f*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.00350435348467321f  - (0.00005525463935338f*fabs(dram->b[33]))));}
		//we apply the first 28 samples of the Neve impulse- dynamically adjusted.
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
		randy = ((float(fpd)/UINT32_MAX)*0.033f);
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
