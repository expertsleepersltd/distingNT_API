#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Luxor"
#define AIRWINDOWS_DESCRIPTION "A re-release of another old Character plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','L','u','r' )
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
		
		inputSample *= indrive;
		//calibrated to match gain through convolution and -0.3f correction
		if (sqdrive > 0.0f){
			dram->b[33] = dram->b[32]; dram->b[32] = dram->b[31]; 
			dram->b[31] = dram->b[30]; dram->b[30] = dram->b[29]; dram->b[29] = dram->b[28]; dram->b[28] = dram->b[27]; dram->b[27] = dram->b[26]; dram->b[26] = dram->b[25]; dram->b[25] = dram->b[24]; dram->b[24] = dram->b[23]; 
			dram->b[23] = dram->b[22]; dram->b[22] = dram->b[21]; dram->b[21] = dram->b[20]; dram->b[20] = dram->b[19]; dram->b[19] = dram->b[18]; dram->b[18] = dram->b[17]; dram->b[17] = dram->b[16]; dram->b[16] = dram->b[15]; 
			dram->b[15] = dram->b[14]; dram->b[14] = dram->b[13]; dram->b[13] = dram->b[12]; dram->b[12] = dram->b[11]; dram->b[11] = dram->b[10]; dram->b[10] = dram->b[9]; dram->b[9] = dram->b[8]; dram->b[8] = dram->b[7]; 
			dram->b[7] = dram->b[6]; dram->b[6] = dram->b[5]; dram->b[5] = dram->b[4]; dram->b[4] = dram->b[3]; dram->b[3] = dram->b[2]; dram->b[2] = dram->b[1]; dram->b[1] = dram->b[0]; dram->b[0] = inputSample * sqdrive;
			inputSample -= (dram->b[1] * (0.20641602693167951f  - (0.00078952185394898f*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (0.07601816702459827f  + (0.00022786334179951f*fabs(dram->b[2]))));
			inputSample -= (dram->b[3] * (0.03929765560019285f  - (0.00054517993246352f*fabs(dram->b[3]))));
			inputSample -= (dram->b[4] * (0.00298333157711103f  - (0.00033083756545638f*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (0.00724006282304610f  + (0.00045483683460812f*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.03073108963506036f  - (0.00038190060537423f*fabs(dram->b[6]))));
			inputSample += (dram->b[7] * (0.02332434692533051f  + (0.00040347288688932f*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.03792606869061214f  - (0.00039673687335892f*fabs(dram->b[8]))));
			inputSample += (dram->b[9] * (0.02437059376675688f  + (0.00037221210539535f*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.03416764311979521f  - (0.00040314850796953f*fabs(dram->b[10]))));
			inputSample += (dram->b[11] * (0.01761669868102127f  + (0.00035989484330131f*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.02538237753523052f  - (0.00040149119125394f*fabs(dram->b[12]))));
			inputSample += (dram->b[13] * (0.00770737340728377f  + (0.00035462118723555f*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.01580706228482803f  - (0.00037563141307594f*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.00055119240005586f  - (0.00035409299268971f*fabs(dram->b[15]))));
			inputSample -= (dram->b[16] * (0.00818552143438768f  - (0.00036507661042180f*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.00661842703548304f  - (0.00034550528559056f*fabs(dram->b[17]))));
			inputSample -= (dram->b[18] * (0.00362447476272098f  - (0.00035553012761240f*fabs(dram->b[18]))));
			inputSample -= (dram->b[19] * (0.00957098027225745f  - (0.00034091691045338f*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.00193621774016660f  - (0.00034554529131668f*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.01005433027357935f  - (0.00033878223153845f*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.00221712428802004f  - (0.00033481410137711f*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.00911255639207995f  - (0.00033263425232666f*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.00339667169034909f  - (0.00032634428038430f*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.00774096948249924f  - (0.00032599868802996f*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.00463907626773794f  - (0.00032131993173361f*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.00658222997260378f  - (0.00032014977430211f*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.00550347079924993f  - (0.00031557153256653f*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.00588754981375325f  - (0.00032041307242303f*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.00590293898419892f  - (0.00030457857428714f*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.00558952010441800f  - (0.00030448053548086f*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.00598183557634295f  - (0.00030715064323181f*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.00555223929714115f  - (0.00030319367948553f*fabs(dram->b[33]))));}
		//voxbox 40 db 
		
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
		randy = ((float(fpd)/UINT32_MAX)*0.031f);
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
