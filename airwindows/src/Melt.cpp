#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Melt"
#define AIRWINDOWS_DESCRIPTION "A wobbly chorusy weird diffuse effect."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','e','t' )
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
{ .name = "Depth", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Range", .min = 0, .max = 1000, .def = 750, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		int minTap[32];
		int maxTap[32];
		int position[32];
		int stepTap[32];
		int stepCount;
		int slowCount;
		Float32 combine;
		Float32 scalefactor;
		int gcount;
		//we're using 32 bit because we want to build some truncation into the tails to noise them.		
		uint32_t fpd;
	
	struct _dram {
			Float32 d[32002];
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
	Float32 rate = 1 / (powf(GetParameter( kParam_One ),2) + 0.001f);
	Float32 depthB = (GetParameter( kParam_Two ) * 139.5f)+2;
	Float32 depthA = depthB * (1.0f - GetParameter( kParam_One ));
	Float32 output = GetParameter( kParam_Three ) * 0.05f;
	Float32 wet = GetParameter( kParam_Four );
	Float32 dry = 1.0f-wet;
	
	minTap[0] = floor(2 * depthA); maxTap[0] = floor(2 * depthB);
	minTap[1] = floor(3 * depthA); maxTap[1] = floor(3 * depthB);
	minTap[2] = floor(5 * depthA); maxTap[2] = floor(5 * depthB);
	minTap[3] = floor(7 * depthA); maxTap[3] = floor(7 * depthB);
	minTap[4] = floor(11 * depthA); maxTap[4] = floor(11 * depthB);
	minTap[5] = floor(13 * depthA); maxTap[5] = floor(13 * depthB);
	minTap[6] = floor(17 * depthA); maxTap[6] = floor(17 * depthB);
	minTap[7] = floor(19 * depthA); maxTap[7] = floor(19 * depthB);
	minTap[8] = floor(23 * depthA); maxTap[8] = floor(23 * depthB);
	minTap[9] = floor(29 * depthA); maxTap[9] = floor(29 * depthB);
	minTap[10] = floor(31 * depthA); maxTap[10] = floor(31 * depthB);
	minTap[11] = floor(37 * depthA); maxTap[11] = floor(37 * depthB);
	minTap[12] = floor(41 * depthA); maxTap[12] = floor(41 * depthB);
	minTap[13] = floor(43 * depthA); maxTap[13] = floor(43 * depthB);
	minTap[14] = floor(47 * depthA); maxTap[14] = floor(47 * depthB);
	minTap[15] = floor(53 * depthA); maxTap[15] = floor(53 * depthB);
	minTap[16] = floor(59 * depthA); maxTap[16] = floor(59 * depthB);
	minTap[17] = floor(61 * depthA); maxTap[17] = floor(61 * depthB);
	minTap[18] = floor(67 * depthA); maxTap[18] = floor(67 * depthB);
	minTap[19] = floor(71 * depthA); maxTap[19] = floor(71 * depthB);
	minTap[20] = floor(73 * depthA); maxTap[20] = floor(73 * depthB);
	minTap[21] = floor(79 * depthA); maxTap[21] = floor(79 * depthB);
	minTap[22] = floor(83 * depthA); maxTap[22] = floor(83 * depthB);
	minTap[23] = floor(89 * depthA); maxTap[23] = floor(89 * depthB);
	minTap[24] = floor(97 * depthA); maxTap[24] = floor(97 * depthB);
	minTap[25] = floor(101 * depthA); maxTap[25] = floor(101 * depthB);
	minTap[26] = floor(103 * depthA); maxTap[26] = floor(103 * depthB);
	minTap[27] = floor(107 * depthA); maxTap[27] = floor(107 * depthB);
	minTap[28] = floor(109 * depthA); maxTap[28] = floor(109 * depthB);
	minTap[29] = floor(113 * depthA); maxTap[29] = floor(113 * depthB);
	minTap[30] = floor(117 * depthA); maxTap[30] = floor(117 * depthB);	
	float drySample;
	float inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		if (gcount < 0 || gcount > 16000) {gcount = 16000;}
		dram->d[gcount+16000] = dram->d[gcount] = inputSample;
		
		if (slowCount > rate || slowCount < 0) {
			slowCount = 0;
			stepCount++;
			if (stepCount > 29 || stepCount < 0) {stepCount = 0;}
			position[stepCount] += stepTap[stepCount];
			if (position[stepCount] < minTap[stepCount]) {
				position[stepCount] = minTap[stepCount];
				stepTap[stepCount] = 1;
			}
			if (position[stepCount] > maxTap[stepCount]) {
				position[stepCount] = maxTap[stepCount];
				stepTap[stepCount] = -1;
			}
		}
		scalefactor *= 0.9999f;
		scalefactor += (100.0f - fabs(combine)) * 0.000001f;
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[29]]);
		combine += (dram->d[gcount+position[28]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[27]]);
		combine += (dram->d[gcount+position[26]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[25]]);
		combine += (dram->d[gcount+position[24]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[23]]);
		combine += (dram->d[gcount+position[22]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[21]]);
		combine += (dram->d[gcount+position[20]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[19]]);
		combine += (dram->d[gcount+position[18]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[17]]);
		combine += (dram->d[gcount+position[16]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[15]]);
		combine += (dram->d[gcount+position[14]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[13]]);
		combine += (dram->d[gcount+position[12]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[11]]);
		combine += (dram->d[gcount+position[10]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[9]]);
		combine += (dram->d[gcount+position[8]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[7]]);
		combine += (dram->d[gcount+position[6]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[5]]);
		combine += (dram->d[gcount+position[4]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[3]]);
		combine += (dram->d[gcount+position[2]]);
		
		combine *= scalefactor;
		combine -= (dram->d[gcount+position[1]]);
		combine += (dram->d[gcount+position[0]]);
		
		gcount--;
		slowCount++;
		
		inputSample = combine;
		
		if (output < 1.0f) inputSample *= output;
		if (wet < 1.0f) inputSample = (drySample * dry)+(inputSample*wet);
		//nice little output stage template: if we have another scale of floating point
		//number, we really don't want to meaninglessly multiply that by 1.0f.
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 32001; count++) {dram->d[count] = 0;}
	for(int count = 0; count < 31; count++) {minTap[count] = 0; maxTap[count] = 0; position[count] = 1; stepTap[count] = 1;}
	combine = 0;
	scalefactor = 0.999;
	stepCount = 0;
	slowCount = 0;
	gcount = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
