#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pop2"
#define AIRWINDOWS_DESCRIPTION "Adds control and punch to Pop."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','3' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
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
{ .name = "Compress", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float muVary;
		float muAttack;
		float muNewSpeed;
		float muSpeedA;
		float muSpeedB;
		float muCoefficientA;
		float muCoefficientB;
		bool flip;
		
		float lastSample;
		float intermediate[16];
		bool wasPosClip;
		bool wasNegClip;
		
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
	
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16; //for ClipOnly2
	
	Float32 threshold = 1.0f - ((1.0f-powf(1.0f-GetParameter( kParam_One ),2))*0.9f);
	Float32 attack = ((powf(GetParameter( kParam_Two ),4)*100000.0f)+10.0f)*overallscale;
	Float32 release = ((powf(GetParameter( kParam_Three ),5)*2000000.0f)+20.0f)*overallscale;
	Float32 maxRelease = release * 4.0f;
	Float32 muPreGain = 1.0f/threshold;
	Float32 muMakeupGain = sqrt(1.0f / threshold)*GetParameter( kParam_Four );
	Float32 wet = GetParameter( kParam_Five );
	//compressor section
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		//begin compressor section
		inputSample *= muPreGain;
		if (flip) {
			if (fabs(inputSample) > threshold) {
				muVary = threshold / fabs(inputSample);
				muAttack = sqrt(fabs(muSpeedA));
				muCoefficientA = muCoefficientA * (muAttack-1.0f);
				if (muVary < threshold) muCoefficientA = muCoefficientA + threshold;
				else  muCoefficientA = muCoefficientA + muVary;
				muCoefficientA = muCoefficientA / muAttack;
				muNewSpeed = muSpeedA * (muSpeedA-1.0f);
				muNewSpeed = muNewSpeed + release;
				muSpeedA = muNewSpeed / muSpeedA;
				if (muSpeedA > maxRelease) muSpeedA = maxRelease;
			} else {
				muCoefficientA = muCoefficientA * ((muSpeedA * muSpeedA)-1.0f);
				muCoefficientA = muCoefficientA + 1.0f;
				muCoefficientA = muCoefficientA / (muSpeedA * muSpeedA);
				muNewSpeed = muSpeedA * (muSpeedA-1.0f);
				muNewSpeed = muNewSpeed + attack;
				muSpeedA = muNewSpeed / muSpeedA;}
		} else {
			if (fabs(inputSample) > threshold) {
				muVary = threshold / fabs(inputSample);
				muAttack = sqrt(fabs(muSpeedB));
				muCoefficientB = muCoefficientB * (muAttack-1.0f);
				if (muVary < threshold) muCoefficientB = muCoefficientB + threshold;
				else muCoefficientB = muCoefficientB + muVary;
				muCoefficientB = muCoefficientB / muAttack;
				muNewSpeed = muSpeedB * (muSpeedB-1.0f);
				muNewSpeed = muNewSpeed + release;
				muSpeedB = muNewSpeed / muSpeedB;
				if (muSpeedB > maxRelease) muSpeedB = maxRelease;
			} else {
				muCoefficientB = muCoefficientB * ((muSpeedB * muSpeedB)-1.0f);
				muCoefficientB = muCoefficientB + 1.0f;
				muCoefficientB = muCoefficientB / (muSpeedB * muSpeedB);
				muNewSpeed = muSpeedB * (muSpeedB-1.0f);
				muNewSpeed = muNewSpeed + attack;
				muSpeedB = muNewSpeed / muSpeedB;}
		}
		//got coefficients, adjusted speeds
		if (flip) inputSample *= powf(muCoefficientA,2);
		else inputSample *= powf(muCoefficientB,2);
		inputSample *= muMakeupGain;
		flip = !flip;
		//end compressor section
		
		//begin ClipOnly2 as a little, compressed chunk that can be dropped into code
		if (inputSample > 4.0f) inputSample = 4.0f; if (inputSample < -4.0f) inputSample = -4.0f;
		if (wasPosClip == true) { //current will be over
			if (inputSample<lastSample) lastSample=0.7058208f+(inputSample*0.2609148f);
			else lastSample = 0.2491717f+(lastSample*0.7390851f);
		} wasPosClip = false;
		if (inputSample>0.9549925859f) {wasPosClip=true;inputSample=0.7058208f+(lastSample*0.2609148f);}
		if (wasNegClip == true) { //current will be -over
			if (inputSample > lastSample) lastSample=-0.7058208f+(inputSample*0.2609148f);
			else lastSample=-0.2491717f+(lastSample*0.7390851f);
		} wasNegClip = false;
		if (inputSample<-0.9549925859f) {wasNegClip=true;inputSample=-0.7058208f+(lastSample*0.2609148f);}
		intermediate[spacing] = inputSample;
        inputSample = lastSample; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediate[x-1] = intermediate[x];
		lastSample = intermediate[0]; //run a little buffer to handle this
		//end ClipOnly2 as a little, compressed chunk that can be dropped into code
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	muVary = 0.0;
	muAttack = 0.0;
	muNewSpeed = 1000.0;
	muSpeedA = 1000.0;
	muSpeedB = 1000.0;
	muCoefficientA = 1.0;
	muCoefficientB = 1.0;
	flip = false;
	
	lastSample = 0.0;
	wasPosClip = false;
	wasNegClip = false;
	for (int x = 0; x < 16; x++) intermediate[x] = 0.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
