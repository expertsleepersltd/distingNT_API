#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ChannelX"
#define AIRWINDOWS_DESCRIPTION "Translates Channel9 into a profusion of wild experiments."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','X' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
static const int kNeve = 1;
static const int kAPI = 2;
static const int kSSL = 3;
static const int kTeac = 4;
static const int kMackie = 5;
static const int kDefaultValue_ParamOne = kNeve;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static char const * const enumStrings0[] = { "", "Neve", "API", "SSL", "Teac", "Mackie", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Console Type", .min = 1, .max = 5, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "Drive", .min = 0, .max = 20000, .def = 0, .unit = kNT_unitPercent, .scaling = kNT_scaling100, .enumStrings = NULL },
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
 
		float lastSampleA, lastSampleB, lastSampleC;
		float rbSampleLA;
		float rbSampleLB;
		bool flip;
		enum {
			bip_dvA, bip_dvB, bip_dvC, bip_dvD, bip_pvA, bip_pvB, bip_pvC, bip_pvD,
			bip_drbLA, bip_drbLB, bip_drbLC, bip_drbLD,
			bip_praLA, bip_praLB, bip_praLC, bip_praLD,
			bip_prbLA, bip_prbLB, bip_prbLC, bip_prbLD,
			bip_total //each distortion section can have one of these, it stacks well
		}; //not remotely elliptic BLEP antialiasing, instead it is derivative BIP :D
		uint32_t fpd;
	
	struct _dram {
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
	int console = (int) GetParameter( kParam_One );
	float density = GetParameter( kParam_Two )/100.0f; //0-2
	float phattity = density - 1.0f;
	if (density > 1.0f) density = 1.0f; //max out at full wet for Spiral aspect
	if (phattity < 0.0f) phattity = 0.0f; //
	float nonLin = 5.0f-density; //number is smaller for more intense, larger for more subtle
	float output = GetParameter( kParam_Three );
	float rbAmount = 0.005832f;
	float threshold = 0.33362176f;
	switch (console)
	{
		case 1: rbAmount = 0.005832f; threshold = 0.33362176f; break; //Neve
		case 2: rbAmount = 0.004096f; threshold = 0.59969536f; break; //API
		case 3: rbAmount = 0.004913f; threshold = 0.84934656f; break; //SSL
		case 4: rbAmount = 0.009216f; threshold = 0.149f; break; //Teac
		case 5: rbAmount = 0.011449f; threshold = 0.092f; break; //Mackie
	}
	rbAmount /= overallscale; //we've learned not to try and adjust threshold for sample rate
	
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		float dielectricScale = fabs(2.0f-((inputSampleL+nonLin)/nonLin));
		if (flip) {
			rbSampleLA = (rbSampleLA * (1.0f-(rbAmount*dielectricScale)))+(inputSampleL*rbAmount*dielectricScale);
			if (fabs(rbSampleLA)>1.18e-37f) {
				float bip_delta = inputSampleL; //delta can be just local and re-used
				inputSampleL *= rbSampleLA*0.96f;
				inputSampleL = fma(((inputSampleL*inputSampleL) * -0.166666666666666f),(inputSampleL*inputSampleL),inputSampleL);
				inputSampleL /= rbSampleLA*0.96f;
				dram->bip[bip_drbLA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
				dram->bip[bip_drbLB] = dram->bip[bip_praLA]-dram->bip[bip_drbLA]; dram->bip[bip_praLA] = dram->bip[bip_drbLA];//velocity
				dram->bip[bip_drbLC] = dram->bip[bip_praLB]-dram->bip[bip_drbLB]; dram->bip[bip_praLB] = dram->bip[bip_drbLB];//acceleration
				dram->bip[bip_drbLD] = dram->bip[bip_praLC]-dram->bip[bip_drbLC]; dram->bip[bip_praLC] = dram->bip[bip_drbLC];//jerk
				float bip_drbE = dram->bip[bip_praLD]-dram->bip[bip_drbLD]; dram->bip[bip_praLD] = dram->bip[bip_drbLD];//snap
				inputSampleL *= (1.0f+(fabs(dram->bip[bip_drbLC])*0.0618f)+(fabs(dram->bip[bip_drbLD])*-0.05982f)+(fabs(bip_drbE)*0.0206f));
			}
			inputSampleL -= rbSampleLA*0.92f;
		} else {
			rbSampleLB = (rbSampleLB * (1.0f-(rbAmount*dielectricScale)))+(inputSampleL*rbAmount*dielectricScale);
			if (fabs(rbSampleLB)>1.18e-37f) {
				float bip_delta = inputSampleL; //delta can be just local and re-used
				inputSampleL *= rbSampleLB*0.96f;
				inputSampleL = fma(((inputSampleL*inputSampleL) * -0.166666666666666f),(inputSampleL*inputSampleL),inputSampleL);
				inputSampleL /= rbSampleLB*0.96f;
				dram->bip[bip_drbLA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
				dram->bip[bip_drbLB] = dram->bip[bip_prbLA]-dram->bip[bip_drbLA]; dram->bip[bip_prbLA] = dram->bip[bip_drbLA];//velocity
				dram->bip[bip_drbLC] = dram->bip[bip_prbLB]-dram->bip[bip_drbLB]; dram->bip[bip_prbLB] = dram->bip[bip_drbLB];//acceleration
				dram->bip[bip_drbLD] = dram->bip[bip_prbLC]-dram->bip[bip_drbLC]; dram->bip[bip_prbLC] = dram->bip[bip_drbLC];//jerk
				float bip_drbE = dram->bip[bip_prbLD]-dram->bip[bip_drbLD]; dram->bip[bip_prbLD] = dram->bip[bip_drbLD];//snap
				inputSampleL *= (1.0f+(fabs(dram->bip[bip_drbLC])*0.0618f)+(fabs(dram->bip[bip_drbLD])*-0.05982f)+(fabs(bip_drbE)*0.0206f));
			}
			inputSampleL -= rbSampleLB*0.92f;
		}
		//highpass section including capacitor modeling nonlinearity
		
		float bip_delta = inputSampleL; //delta can be just local and re-used
		float drySampleL = inputSampleL;
		inputSampleL = fmax(fmin(inputSampleL,1.0f),-1.0f);
		float phatSample = inputSampleL * 1.57079633f; //phatSample is sin()
		float X = phatSample; X *= X; //float for even
		float temp = phatSample * X; //the initial multiplies
		phatSample -= temp*0.16666666666666666666666666666666666f; temp *= X;
		phatSample += temp*0.00833333333333333333333333333333333f; temp *= X;
		phatSample -= temp*0.00019841269841269841269841269841269f; temp *= X;
		phatSample += temp*0.00000275573192239858906525573192239f; temp *= X;
		phatSample -= temp*0.00000002505210838544171877521083854f; temp *= X;
		phatSample += temp*0.00000000016059043836821614599392377f; temp *= X;
		phatSample -= temp*0.00000000000076471637318198164759011f; temp *= X;
		phatSample += temp*0.00000000000000281145725434552076319f; temp *= X;
		phatSample -= temp*0.00000000000000000822063524662432971f; temp *= X;
		phatSample += temp*0.00000000000000000001957294106339126f;
		//retain mantissa of a float increasing power function
		//float probably doesn't handle more than 36 digits or so
		
		inputSampleL *= 1.2533141373155f;
		//clip to 1.2533141373155f to reach maximum output, or 1.57079633f for pure sine 'phat' version
		float distSample = inputSampleL * fabs(inputSampleL); //distSample is Spiral
		X = distSample; X *= X; //float for even
		temp = distSample * X; //the initial multiplies
		distSample -= temp*0.16666666666666666666666666666666666f; temp *= X;
		distSample += temp*0.00833333333333333333333333333333333f; temp *= X;
		distSample -= temp*0.00019841269841269841269841269841269f; temp *= X;
		distSample += temp*0.00000275573192239858906525573192239f; temp *= X;
		distSample -= temp*0.00000002505210838544171877521083854f; temp *= X;
		distSample += temp*0.00000000016059043836821614599392377f; temp *= X;
		distSample -= temp*0.00000000000076471637318198164759011f; temp *= X;
		distSample += temp*0.00000000000000281145725434552076319f; temp *= X;
		distSample -= temp*0.00000000000000000822063524662432971f; temp *= X;
		distSample += temp*0.00000000000000000001957294106339126f;
		//retain mantissa of a float increasing power function
		//float probably doesn't handle more than 36 digits or so
		distSample /= ((fabs(inputSampleL) == 0.0f) ?1.0f:fabs(inputSampleL));
		inputSampleL = distSample; //purest form is full Spiral
		
		if (density < 1.0f) inputSampleL = (drySampleL*(1-density))+(distSample*density); //fade Spiral aspect
		if (phattity > 0.0f) inputSampleL = (inputSampleL*(1-phattity))+(phatSample*phattity); //apply original Density on top
		
		dram->bip[bip_dvA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
		dram->bip[bip_dvB] = dram->bip[bip_pvA]-dram->bip[bip_dvA]; dram->bip[bip_pvA] = dram->bip[bip_dvA];//velocity
		dram->bip[bip_dvC] = dram->bip[bip_pvB]-dram->bip[bip_dvB]; dram->bip[bip_pvB] = dram->bip[bip_dvB];//acceleration
		dram->bip[bip_dvD] = dram->bip[bip_pvC]-dram->bip[bip_dvC]; dram->bip[bip_pvC] = dram->bip[bip_dvC];//jerk
		float bip_dvE = dram->bip[bip_pvD]-dram->bip[bip_dvD]; dram->bip[bip_pvD] = dram->bip[bip_dvD];//snap
		inputSampleL *= (1.0f+(fabs(dram->bip[bip_dvC])*0.0618f)+(fabs(dram->bip[bip_dvD])*-0.05982f)+(fabs(bip_dvE)*0.0206f));
		
		float clamp = (lastSampleB - lastSampleC) * 0.381966011250105f;
		clamp -= (lastSampleA - lastSampleB) * 0.6180339887498948482045f;
		clamp += inputSampleL - lastSampleA; //regular slew clamping added
		
		lastSampleC = lastSampleB;
		lastSampleB = lastSampleA;
		lastSampleA = inputSampleL; //now our output relates off lastSampleB
		
		if (clamp > threshold)
			inputSampleL = lastSampleB + threshold;
		if (-clamp > threshold)
			inputSampleL = lastSampleB - threshold;
		//slew section		
		lastSampleA = (lastSampleA*0.381966011250105f)+(inputSampleL*0.6180339887498948482045f);
		//split the difference between raw and smoothed for buffer
		
		flip = !flip;
		
		if (output < 1.0f)
		{
			inputSampleL *= output;
		}
		
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSampleA = lastSampleB = lastSampleC = 0.0;
	rbSampleLA = rbSampleLB = 0.0;
	flip = false;
	for (int x = 0; x < bip_total; x++) dram->bip[x] = 0.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
