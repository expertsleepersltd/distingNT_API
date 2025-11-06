#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "FatEQ"
#define AIRWINDOWS_DESCRIPTION "FatEQ"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','a','t' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
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
{ .name = "High", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Out", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		enum {
			prevSampL1,
			prevSlewL1,
			prevSampL2,
			prevSlewL2,
			prevSampL3,
			prevSlewL3,
			prevSampL4,
			prevSlewL4,
			prevSampL5,
			prevSlewL5,
			prevSampL6,
			prevSlewL6,
			pear_max,
			figL,
			gndL,
			slew,
			freq,
			levl,
			pear_total
		}; //new pear filter
		
		
		float lastSample;
		float intermediate[16];
		bool wasPosClip;
		bool wasNegClip;
		
		uint32_t fpd;
	
	struct _dram {
			float pearA[pear_total];
		float pearB[pear_total];
		float pearC[pear_total];
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
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;
	
	float topLevl = GetParameter( kParam_A )*2.0f; if (topLevl < 1.0f) topLevl = sqrt(topLevl);
	dram->pearA[levl] = GetParameter( kParam_B )*2.0f; if (dram->pearA[levl] < 1.0f) dram->pearA[levl] = sqrt(dram->pearA[levl]);
	dram->pearB[levl] = GetParameter( kParam_C )*2.0f; if (dram->pearB[levl] < 1.0f) dram->pearB[levl] = sqrt(dram->pearB[levl]);
	dram->pearC[levl] = GetParameter( kParam_D )*2.0f; if (dram->pearC[levl] < 1.0f) dram->pearC[levl] = sqrt(dram->pearC[levl]);
	float out = GetParameter( kParam_E );
	
	float freqFactor = sqrt(overallscale) + (overallscale*0.5f);
	dram->pearA[freq] = powf(0.564f,freqFactor+0.85f);
	dram->pearB[freq] = powf(0.564f,freqFactor+4.1f);
	dram->pearC[freq] = powf(0.564f,freqFactor+7.1f);
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		for (int x = 0; x < pear_max; x += 2) {
			//begin Pear filter stages
			dram->pearA[figL] = inputSampleL;
			dram->pearA[slew] = ((dram->pearA[figL]-dram->pearA[x])+dram->pearA[x+1])*dram->pearA[freq]*0.5f;
			dram->pearA[x] = dram->pearA[figL] = (dram->pearA[freq]*dram->pearA[figL])+((1.0f-dram->pearA[freq])*(dram->pearA[x]+dram->pearA[x+1]));
			dram->pearA[x+1] = dram->pearA[slew];
			inputSampleL -= dram->pearA[figL];
			
			dram->pearB[figL] = dram->pearA[figL];
			dram->pearB[slew] = ((dram->pearB[figL]-dram->pearB[x])+dram->pearB[x+1])*dram->pearB[freq]*0.5f;
			dram->pearB[x] = dram->pearB[figL] = (dram->pearB[freq]*dram->pearA[figL])+((1.0f-dram->pearB[freq])*(dram->pearB[x]+dram->pearB[x+1]));
			dram->pearB[x+1] = dram->pearB[slew];
			dram->pearA[figL] -= dram->pearB[figL];
			
			dram->pearC[figL] = dram->pearB[figL];
			dram->pearC[slew] = ((dram->pearC[figL]-dram->pearC[x])+dram->pearC[x+1])*dram->pearC[freq]*0.5f;
			dram->pearC[x] = dram->pearC[figL] = (dram->pearC[freq]*dram->pearB[figL])+((1.0f-dram->pearC[freq])*(dram->pearC[x]+dram->pearC[x+1]));
			dram->pearC[x+1] = dram->pearC[slew];
			dram->pearB[figL] -= dram->pearC[figL];
			
			float altered = inputSampleL;
			if (topLevl > 1.0f) {
				altered = fmax(fmin(inputSampleL*M_PI_2,M_PI_2),-M_PI_2);
				float X = altered*altered;
				float temp = altered*X; altered -= (temp/6.0f); temp *= X;
				altered += (temp / 120.0f); temp *= X; altered -= (temp / 5040.0f); temp *= X;
				altered += (temp / 362880.0f); temp *= X; altered -= (temp / 39916800.0f);
			}
			if (topLevl < 1.0f) {
				altered = fmax(fmin(inputSampleL,1.0f),-1.0f);
				float polarity = altered;
				float X = inputSampleL * altered;
				float temp = X; altered = (temp / 2.0f); temp *= X;
				altered -= (temp / 24.0f); temp *= X; altered += (temp / 720.0f); temp *= X;
				altered -= (temp / 40320.0f); temp *= X; altered += (temp / 3628800.0f);
				altered *= ((polarity<0.0f)?-1.0f:1.0f);
			}
			inputSampleL = (inputSampleL*(1.0f-fabs(topLevl-1.0f)))+(altered*fabs(topLevl-1.0f));
			
			altered = dram->pearA[figL];
			if (dram->pearA[levl] > 1.0f) {
				altered = fmax(fmin(dram->pearA[figL]*M_PI_2,M_PI_2),-M_PI_2);
				float X = altered*altered;
				float temp = altered*X; altered -= (temp/6.0f); temp *= X;
				altered += (temp / 120.0f); temp *= X; altered -= (temp / 5040.0f); temp *= X;
				altered += (temp / 362880.0f); temp *= X; altered -= (temp / 39916800.0f);
			}
			if (dram->pearA[levl] < 1.0f) {
				altered = fmax(fmin(dram->pearA[figL],1.0f),-1.0f);
				float polarity = altered;
				float X = dram->pearA[figL] * altered;
				float temp = X; altered = (temp / 2.0f); temp *= X;
				altered -= (temp / 24.0f); temp *= X; altered += (temp / 720.0f); temp *= X;
				altered -= (temp / 40320.0f); temp *= X; altered += (temp / 3628800.0f);
				altered *= ((polarity<0.0f)?-1.0f:1.0f);
			}
			inputSampleL += (dram->pearA[figL]*(1.0f-fabs(dram->pearA[levl]-1.0f)))+(altered*fabs(dram->pearA[levl]-1.0f));
			
			altered = dram->pearB[figL];
			if (dram->pearB[levl] > 1.0f) {
				altered = fmax(fmin(dram->pearB[figL]*M_PI_2,M_PI_2),-M_PI_2);
				float X = altered*altered;
				float temp = altered*X; altered -= (temp/6.0f); temp *= X;
				altered += (temp / 120.0f); temp *= X; altered -= (temp / 5040.0f); temp *= X;
				altered += (temp / 362880.0f); temp *= X; altered -= (temp / 39916800.0f);
			}
			if (dram->pearB[levl] < 1.0f) {
				altered = fmax(fmin(dram->pearB[figL],1.0f),-1.0f);
				float polarity = altered;
				float X = dram->pearB[figL] * altered;
				float temp = X; altered = (temp / 2.0f); temp *= X;
				altered -= (temp / 24.0f); temp *= X; altered += (temp / 720.0f); temp *= X;
				altered -= (temp / 40320.0f); temp *= X; altered += (temp / 3628800.0f);
				altered *= ((polarity<0.0f)?-1.0f:1.0f);
			}
			inputSampleL += (dram->pearB[figL]*(1.0f-fabs(dram->pearB[levl]-1.0f)))+(altered*fabs(dram->pearB[levl]-1.0f));
			
			altered = dram->pearC[figL];
			if (dram->pearC[levl] > 1.0f) {
				altered = fmax(fmin(dram->pearC[figL]*M_PI_2,M_PI_2),-M_PI_2);
				float X = altered*altered;
				float temp = altered*X; altered -= (temp/6.0f); temp *= X;
				altered += (temp / 120.0f); temp *= X; altered -= (temp / 5040.0f); temp *= X;
				altered += (temp / 362880.0f); temp *= X; altered -= (temp / 39916800.0f);
			}
			if (dram->pearC[levl] < 1.0f) {
				altered = fmax(fmin(dram->pearC[figL],1.0f),-1.0f);
				float polarity = altered;
				float X = dram->pearC[figL] * altered;
				float temp = X; altered = (temp / 2.0f); temp *= X;
				altered -= (temp / 24.0f); temp *= X; altered += (temp / 720.0f); temp *= X;
				altered -= (temp / 40320.0f); temp *= X; altered += (temp / 3628800.0f);
				altered *= ((polarity<0.0f)?-1.0f:1.0f);
			}
			inputSampleL += (dram->pearC[figL]*(1.0f-fabs(dram->pearC[levl]-1.0f)))+(altered*fabs(dram->pearC[levl]-1.0f));
		}
		
		inputSampleL *= out;
		
		//begin ClipOnly2 as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0f) inputSampleL = 4.0f; if (inputSampleL < -4.0f) inputSampleL = -4.0f;
		if (wasPosClip == true) { //current will be over
			if (inputSampleL<lastSample) lastSample=0.7058208f+(inputSampleL*0.2609148f);
			else lastSample = 0.2491717f+(lastSample*0.7390851f);
		} wasPosClip = false;
		if (inputSampleL>0.9549925859f) {wasPosClip=true;inputSampleL=0.7058208f+(lastSample*0.2609148f);}
		if (wasNegClip == true) { //current will be -over
			if (inputSampleL > lastSample) lastSample=-0.7058208f+(inputSampleL*0.2609148f);
			else lastSample=-0.2491717f+(lastSample*0.7390851f);
		} wasNegClip = false;
		if (inputSampleL<-0.9549925859f) {wasNegClip=true;inputSampleL=-0.7058208f+(lastSample*0.2609148f);}
		intermediate[spacing] = inputSampleL;
        inputSampleL = lastSample; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediate[x-1] = intermediate[x];
		lastSample = intermediate[0]; //run a little buffer to handle this
		//end ClipOnly2 as a little, compressed chunk that can be dropped into code		
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < pear_total; x++) {
		dram->pearA[x] = 0.0;
		dram->pearB[x] = 0.0;
		dram->pearC[x] = 0.0;
	}
	lastSample = 0.0;
	wasPosClip = false;
	wasNegClip = false;
	for (int x = 0; x < 16; x++) intermediate[x] = 0.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
