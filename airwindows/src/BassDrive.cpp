#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BassDrive"
#define AIRWINDOWS_DESCRIPTION "An old secret weapon, like a kind of bass amp."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','a','e' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One = 0,
	kParam_Two = 1,
	kParam_Three = 2,
	kParam_Four = 3,
	kParam_Five = 4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Presence", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "High", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 presenceInA[7];
		Float32 presenceOutA[7];
		Float32 highInA[7];
		Float32 highOutA[7];
		Float32 midInA[7];
		Float32 midOutA[7];
		Float32 lowInA[7];
		Float32 lowOutA[7];
		Float32 presenceInB[7];
		Float32 presenceOutB[7];
		Float32 highInB[7];
		Float32 highOutB[7];
		Float32 midInB[7];
		Float32 midOutB[7];
		Float32 lowInB[7];
		Float32 lowOutB[7];
		bool flip;
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
	Float32 sum;
	Float32 presence = powf(GetParameter( kParam_One ),5) * 8.0f;
	Float32 high = powf(GetParameter( kParam_Two ),3) * 4.0f;
	Float32 mid = powf(GetParameter( kParam_Three ),2);
	Float32 low = GetParameter( kParam_Four ) / 4.0f;
	Float32 drive = GetParameter( kParam_Five ) * 2.0f;
	Float32 bridgerectifier;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		sum = 0.0f;
		
		
		if (flip)
		{
			presenceInA[0] = presenceInA[1]; presenceInA[1] = presenceInA[2]; presenceInA[2] = presenceInA[3];
			presenceInA[3] = presenceInA[4]; presenceInA[4] = presenceInA[5]; presenceInA[5] = presenceInA[6]; 
			presenceInA[6] = inputSample * presence; presenceOutA[2] = presenceOutA[3];
			presenceOutA[3] = presenceOutA[4]; presenceOutA[4] = presenceOutA[5]; presenceOutA[5] = presenceOutA[6]; 
			presenceOutA[6] = (presenceInA[0] + presenceInA[6]) + 1.9152966321f * (presenceInA[1] + presenceInA[5]) 
			- (presenceInA[2] + presenceInA[4]) - 3.8305932641f * presenceInA[3]
			+ ( -0.2828214615f * presenceOutA[2]) + (  0.2613069963f * presenceOutA[3])
			+ ( -0.8628193852f * presenceOutA[4]) + (  0.5387164389f * presenceOutA[5]);
			bridgerectifier = fabs(presenceOutA[6]);
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			bridgerectifier = sin(bridgerectifier);
			if (presenceOutA[6] > 0.0f){sum += bridgerectifier;}
			else {sum -= bridgerectifier;}			
			//presence section
			
			highInA[0] = highInA[1]; highInA[1] = highInA[2]; highInA[2] = highInA[3];
			highInA[3] = highInA[4]; highInA[4] = highInA[5]; highInA[5] = highInA[6]; 
			bridgerectifier = fabs(inputSample) * high;
			if (bridgerectifier > 1.57079633f) {bridgerectifier = 1.57079633f;}
			bridgerectifier = sin(bridgerectifier);
			if (inputSample > 0.0f) {highInA[6] = bridgerectifier;}
			else {highInA[6] = -bridgerectifier;}
			highInA[6] *= high; highOutA[2] = highOutA[3];
			highOutA[3] = highOutA[4]; highOutA[4] = highOutA[5]; highOutA[5] = highOutA[6]; 
			highOutA[6] = (highInA[0] + highInA[6]) -   0.5141967433f * (highInA[1] + highInA[5]) 
			- (highInA[2] + highInA[4]) +   1.0283934866f * highInA[3]
			+ ( -0.2828214615f * highOutA[2]) + (  1.0195930909f * highOutA[3])
			+ ( -1.9633013869f * highOutA[4]) + (  2.1020162751f * highOutA[5]);
			bridgerectifier = fabs(highOutA[6]);
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			bridgerectifier = sin(bridgerectifier);
			if (highOutA[6] > 0.0f){sum += bridgerectifier;}
			else {sum -= bridgerectifier;}			
			//high section
			
			midInA[0] = midInA[1]; midInA[1] = midInA[2]; midInA[2] = midInA[3];
			midInA[3] = midInA[4]; midInA[4] = midInA[5]; midInA[5] = midInA[6]; 
			bridgerectifier = fabs(inputSample) * mid;
			if (bridgerectifier > 1.57079633f) {bridgerectifier = 1.57079633f;}
			bridgerectifier = sin(bridgerectifier);
			if (inputSample > 0.0f) {midInA[6] = bridgerectifier;}
			else {midInA[6] = -bridgerectifier;}
			midInA[6] *= mid; midOutA[2] = midOutA[3];
			midOutA[3] = midOutA[4]; midOutA[4] = midOutA[5]; midOutA[5] = midOutA[6]; 
			midOutA[6] = (midInA[0] + midInA[6]) - 1.1790257790f * (midInA[1] + midInA[5]) 
			- (midInA[2] + midInA[4]) + 2.3580515580f * midInA[3]
			+ ( -0.6292082828f * midOutA[2]) + (  2.7785843605f * midOutA[3])
			+ ( -4.6638295236f * midOutA[4]) + (  3.5142515802f * midOutA[5]);
			sum += midOutA[6];
			//mid section
			
			lowInA[0] = lowInA[1]; lowInA[1] = lowInA[2]; lowInA[2] = lowInA[3];
			lowInA[3] = lowInA[4]; lowInA[4] = lowInA[5]; lowInA[5] = lowInA[6]; 
			bridgerectifier = fabs(inputSample) * low;
			if (bridgerectifier > 1.57079633f) {bridgerectifier = 1.57079633f;}
			bridgerectifier = sin(bridgerectifier);
			if (inputSample > 0.0f) {lowInA[6] = bridgerectifier;}
			else {lowInA[6] = -bridgerectifier;}
			lowInA[6] *= low; lowOutA[2] = lowOutA[3];
			lowOutA[3] = lowOutA[4]; lowOutA[4] = lowOutA[5]; lowOutA[5] = lowOutA[6]; 
			lowOutA[6] = (lowInA[0] + lowInA[6]) - 1.9193504547f * (lowInA[1] + lowInA[5]) 
			- (lowInA[2] + lowInA[4]) + 3.8387009093f * lowInA[3]
			+ ( -0.9195964462f * lowOutA[2]) + (  3.7538173833f * lowOutA[3])
			+ ( -5.7487775603f * lowOutA[4]) + (  3.9145559258f * lowOutA[5]);
			sum += lowOutA[6];
			//low section
		}
		else
		{
			presenceInB[0] = presenceInB[1]; presenceInB[1] = presenceInB[2]; presenceInB[2] = presenceInB[3];
			presenceInB[3] = presenceInB[4]; presenceInB[4] = presenceInB[5]; presenceInB[5] = presenceInB[6]; 
			presenceInB[6] = inputSample * presence; presenceOutB[2] = presenceOutB[3];
			presenceOutB[3] = presenceOutB[4]; presenceOutB[4] = presenceOutB[5]; presenceOutB[5] = presenceOutB[6]; 
			presenceOutB[6] = (presenceInB[0] + presenceInB[6]) + 1.9152966321f * (presenceInB[1] + presenceInB[5]) 
			- (presenceInB[2] + presenceInB[4]) - 3.8305932641f * presenceInB[3]
			+ ( -0.2828214615f * presenceOutB[2]) + (  0.2613069963f * presenceOutB[3])
			+ ( -0.8628193852f * presenceOutB[4]) + (  0.5387164389f * presenceOutB[5]);
			bridgerectifier = fabs(presenceOutB[6]);
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			bridgerectifier = sin(bridgerectifier);
			if (presenceOutB[6] > 0.0f){sum += bridgerectifier;}
			else {sum -= bridgerectifier;}			
			//presence section
			
			highInB[0] = highInB[1]; highInB[1] = highInB[2]; highInB[2] = highInB[3];
			highInB[3] = highInB[4]; highInB[4] = highInB[5]; highInB[5] = highInB[6]; 
			bridgerectifier = fabs(inputSample) * high;
			if (bridgerectifier > 1.57079633f) {bridgerectifier = 1.57079633f;}
			bridgerectifier = sin(bridgerectifier);
			if (inputSample > 0.0f) {highInB[6] = bridgerectifier;}
			else {highInB[6] = -bridgerectifier;}
			highInB[6] *= high; highOutB[2] = highOutB[3];
			highOutB[3] = highOutB[4]; highOutB[4] = highOutB[5]; highOutB[5] = highOutB[6]; 
			highOutB[6] = (highInB[0] + highInB[6]) -   0.5141967433f * (highInB[1] + highInB[5]) 
			- (highInB[2] + highInB[4]) +   1.0283934866f * highInB[3]
			+ ( -0.2828214615f * highOutB[2]) + (  1.0195930909f * highOutB[3])
			+ ( -1.9633013869f * highOutB[4]) + (  2.1020162751f * highOutB[5]);
			bridgerectifier = fabs(highOutB[6]);
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			bridgerectifier = sin(bridgerectifier);
			if (highOutB[6] > 0.0f){sum += bridgerectifier;}
			else {sum -= bridgerectifier;}			
			//high section
			
			midInB[0] = midInB[1]; midInB[1] = midInB[2]; midInB[2] = midInB[3];
			midInB[3] = midInB[4]; midInB[4] = midInB[5]; midInB[5] = midInB[6]; 
			bridgerectifier = fabs(inputSample) * mid;
			if (bridgerectifier > 1.57079633f) {bridgerectifier = 1.57079633f;}
			bridgerectifier = sin(bridgerectifier);
			if (inputSample > 0.0f) {midInB[6] = bridgerectifier;}
			else {midInB[6] = -bridgerectifier;}
			midInB[6] *= mid; midOutB[2] = midOutB[3];
			midOutB[3] = midOutB[4]; midOutB[4] = midOutB[5]; midOutB[5] = midOutB[6]; 
			midOutB[6] = (midInB[0] + midInB[6]) - 1.1790257790f * (midInB[1] + midInB[5]) 
			- (midInB[2] + midInB[4]) + 2.3580515580f * midInB[3]
			+ ( -0.6292082828f * midOutB[2]) + (  2.7785843605f * midOutB[3])
			+ ( -4.6638295236f * midOutB[4]) + (  3.5142515802f * midOutB[5]);
			sum += midOutB[6];
			//mid section
			
			lowInB[0] = lowInB[1]; lowInB[1] = lowInB[2]; lowInB[2] = lowInB[3];
			lowInB[3] = lowInB[4]; lowInB[4] = lowInB[5]; lowInB[5] = lowInB[6]; 
			bridgerectifier = fabs(inputSample) * low;
			if (bridgerectifier > 1.57079633f) {bridgerectifier = 1.57079633f;}
			bridgerectifier = sin(bridgerectifier);
			if (inputSample > 0.0f) {lowInB[6] = bridgerectifier;}
			else {lowInB[6] = -bridgerectifier;}
			lowInB[6] *= low; lowOutB[2] = lowOutB[3];
			lowOutB[3] = lowOutB[4]; lowOutB[4] = lowOutB[5]; lowOutB[5] = lowOutB[6]; 
			lowOutB[6] = (lowInB[0] + lowInB[6]) - 1.9193504547f * (lowInB[1] + lowInB[5]) 
			- (lowInB[2] + lowInB[4]) + 3.8387009093f * lowInB[3]
			+ ( -0.9195964462f * lowOutB[2]) + (  3.7538173833f * lowOutB[3])
			+ ( -5.7487775603f * lowOutB[4]) + (  3.9145559258f * lowOutB[5]);
			sum += lowOutB[6];
			//low section
		}
		
		inputSample = fabs(sum) * drive;
		if (inputSample > 1.57079633f) {inputSample = 1.57079633f;}
		inputSample = sin(inputSample);
		if (sum < 0) inputSample = -inputSample;
		
		flip = !flip;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int fcount = 0; fcount < 7; fcount++)
	{
		presenceInA[fcount] = 0.0;
		presenceOutA[fcount] = 0.0;
		highInA[fcount] = 0.0;
		highOutA[fcount] = 0.0;
		midInA[fcount] = 0.0;
		midOutA[fcount] = 0.0;
		lowInA[fcount] = 0.0;
		lowOutA[fcount] = 0.0;
		presenceInB[fcount] = 0.0;
		presenceOutB[fcount] = 0.0;
		highInB[fcount] = 0.0;
		highOutB[fcount] = 0.0;
		midInB[fcount] = 0.0;
		midOutB[fcount] = 0.0;
		lowInB[fcount] = 0.0;
		lowOutB[fcount] = 0.0;
	}
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
