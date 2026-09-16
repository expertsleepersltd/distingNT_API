#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Stonefire"
#define AIRWINDOWS_DESCRIPTION "The non-EQ EQ designed for ConsoleX."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','t','f' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "Air", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fire", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Stone", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Range", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		enum {
			pvAL1,
			pvSL1,
			accSL1,
			acc2SL1,
			pvAL2,
			pvSL2,
			accSL2,
			acc2SL2,
			pvAL3,
			pvSL3,
			accSL3,
			pvAL4,
			pvSL4,
			gndavgL,
			outAL,
			gainAL,
			air_total
		};
		
		enum {
			prevSampL1,
			prevSlewL1,
			accSlewL1,
			prevSampL2,
			prevSlewL2,
			accSlewL2,
			prevSampL3,
			prevSlewL3,
			accSlewL3,
			kalGainL,
			kalOutL,
			kal_total
		};
				
		float trebleGainA;
		float trebleGainB;
		float midGainA;
		float midGainB;
		float bassGainA;
		float bassGainB;
		
		uint32_t fpd;
	
	struct _dram {
			float air[air_total];
		float kal[kal_total];
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
		
	trebleGainA = trebleGainB; trebleGainB = GetParameter( kParam_One )*2.0f;
	midGainA = midGainB; midGainB = GetParameter( kParam_Two )*2.0f;
	bassGainA = bassGainB; bassGainB = GetParameter( kParam_Three )*2.0f;
	//simple three band to adjust
	float kalman = 1.0f-powf(GetParameter( kParam_Four ),2);
	//crossover frequency between mid/bass
			
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		float drySampleL = inputSampleL;
			
		float temp = (float)nSampleFrames/inFramesToProcess;
		float trebleGain = (trebleGainA*temp)+(trebleGainB*(1.0f-temp));
		if (trebleGain > 1.0f) trebleGain = powf(trebleGain,3.0f+sqrt(overallscale));
		if (trebleGain < 1.0f) trebleGain = 1.0f-powf(1.0f-trebleGain,2);
		
		float midGain = (midGainA*temp)+(midGainB*(1.0f-temp));
		if (midGain > 1.0f) midGain *= midGain;
		if (midGain < 1.0f) midGain = 1.0f-powf(1.0f-midGain,2);
		
		float bassGain = (bassGainA*temp)+(bassGainB*(1.0f-temp));
		if (bassGain > 1.0f) bassGain *= bassGain;
		if (bassGain < 1.0f) bassGain = 1.0f-powf(1.0f-bassGain,2);
				
		//begin Air3L
		dram->air[pvSL4] = dram->air[pvAL4] - dram->air[pvAL3]; dram->air[pvSL3] = dram->air[pvAL3] - dram->air[pvAL2];
		dram->air[pvSL2] = dram->air[pvAL2] - dram->air[pvAL1]; dram->air[pvSL1] = dram->air[pvAL1] - inputSampleL;
		dram->air[accSL3] = dram->air[pvSL4] - dram->air[pvSL3]; dram->air[accSL2] = dram->air[pvSL3] - dram->air[pvSL2];
		dram->air[accSL1] = dram->air[pvSL2] - dram->air[pvSL1];
		dram->air[acc2SL2] = dram->air[accSL3] - dram->air[accSL2]; dram->air[acc2SL1] = dram->air[accSL2] - dram->air[accSL1];		
		dram->air[outAL] = -(dram->air[pvAL1] + dram->air[pvSL3] + dram->air[acc2SL2] - ((dram->air[acc2SL2] + dram->air[acc2SL1])*0.5f));
		dram->air[gainAL] *= 0.5f; dram->air[gainAL] += fabs(drySampleL-dram->air[outAL])*0.5f;
		if (dram->air[gainAL] > 0.3f*sqrt(overallscale)) dram->air[gainAL] = 0.3f*sqrt(overallscale);
		dram->air[pvAL4] = dram->air[pvAL3]; dram->air[pvAL3] = dram->air[pvAL2];
		dram->air[pvAL2] = dram->air[pvAL1]; dram->air[pvAL1] = (dram->air[gainAL] * dram->air[outAL]) + drySampleL;
		float midL = drySampleL - ((dram->air[outAL]*0.5f)+(drySampleL*(0.457f-(0.017f*overallscale))));
		temp = (midL + dram->air[gndavgL])*0.5f; dram->air[gndavgL] = midL; midL = temp;
		float trebleL = drySampleL-midL;
		inputSampleL = midL;
		//end Air3L
				
		//begin KalmanL
		temp = inputSampleL = inputSampleL*(1.0f-kalman)*0.777f;
		inputSampleL *= (1.0f-kalman);
		//set up gain levels to control the beast
		dram->kal[prevSlewL3] += dram->kal[prevSampL3] - dram->kal[prevSampL2]; dram->kal[prevSlewL3] *= 0.5f;
		dram->kal[prevSlewL2] += dram->kal[prevSampL2] - dram->kal[prevSampL1]; dram->kal[prevSlewL2] *= 0.5f;
		dram->kal[prevSlewL1] += dram->kal[prevSampL1] - inputSampleL; dram->kal[prevSlewL1] *= 0.5f;
		//make slews from each set of samples used
		dram->kal[accSlewL2] += dram->kal[prevSlewL3] - dram->kal[prevSlewL2]; dram->kal[accSlewL2] *= 0.5f;
		dram->kal[accSlewL1] += dram->kal[prevSlewL2] - dram->kal[prevSlewL1]; dram->kal[accSlewL1] *= 0.5f;
		//differences between slews: rate of change of rate of change
		dram->kal[accSlewL3] += (dram->kal[accSlewL2] - dram->kal[accSlewL1]); dram->kal[accSlewL3] *= 0.5f;
		//entering the abyss, what even is this
		dram->kal[kalOutL] += dram->kal[prevSampL1] + dram->kal[prevSlewL2] + dram->kal[accSlewL3]; dram->kal[kalOutL] *= 0.5f;
		//resynthesizing predicted result (all iir smoothed)
		dram->kal[kalGainL] += fabs(temp-dram->kal[kalOutL])*kalman*8.0f; dram->kal[kalGainL] *= 0.5f;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kal[kalGainL] > kalman*0.5f) dram->kal[kalGainL] = kalman*0.5f;
		//attempts to avoid explosions
		dram->kal[kalOutL] += (temp*(1.0f-(0.68f+(kalman*0.157f))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kal[prevSampL3] = dram->kal[prevSampL2]; dram->kal[prevSampL2] = dram->kal[prevSampL1];
		dram->kal[prevSampL1] = (dram->kal[kalGainL] * dram->kal[kalOutL]) + ((1.0f-dram->kal[kalGainL])*temp);
		//feed the chain of previous samples
		if (dram->kal[prevSampL1] > 1.0f) dram->kal[prevSampL1] = 1.0f; if (dram->kal[prevSampL1] < -1.0f) dram->kal[prevSampL1] = -1.0f;
		float bassL = dram->kal[kalOutL]*0.777f;
		midL -= bassL;
		//end KalmanL
						
		inputSampleL = (bassL*bassGain) + (midL*midGain) + (trebleL*trebleGain);
				
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < air_total; x++) dram->air[x] = 0.0;
	for (int x = 0; x < kal_total; x++) dram->kal[x] = 0.0;
	trebleGainA = 1.0; trebleGainB = 1.0;
	midGainA = 1.0; midGainB = 1.0;
	bassGainA = 1.0; bassGainB = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
