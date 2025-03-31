#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ZRegion2"
#define AIRWINDOWS_DESCRIPTION "An Emu e6400 style Airwindows Region filter, with coefficient smoothing."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','Z','R','f' )
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
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Input", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "First", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Last", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Poles", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		
		double iirSampleA;
		enum {
			biq_freq,
			biq_reso,
			biq_a0,
			biq_a1,
			biq_a2,
			biq_b1,
			biq_b2,
			biq_aA0,
			biq_aA1,
			biq_aA2,
			biq_bA1,
			biq_bA2,
			biq_aB0,
			biq_aB1,
			biq_aB2,
			biq_bB1,
			biq_bB2,
			biq_sL1,
			biq_sL2,
			biq_sR1,
			biq_sR2,
			biq_total
		}; //coefficient interpolating biquad filter, stereo
		double biquad[biq_total];
		double biquadA[biq_total];
		double biquadB[biq_total];
		double biquadC[biq_total];
		double biquadD[biq_total];
		double inTrimA;
		double inTrimB;
		double wetA;
		double wetB;
		double overallWetA;
		double overallWetB;
		
		enum {
			fix_freq,
			fix_reso,
			fix_a0,
			fix_a1,
			fix_a2,
			fix_b1,
			fix_b2,
			fix_sL1,
			fix_sL2,
			fix_sR1,
			fix_sR2,
			fix_total
		}; //fixed frequency biquad filter for ultrasonics, stereo
		double fixA[fix_total];
		double fixB[fix_total];
		
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	//begin from XRegion
	Float64 high = GetParameter( kParam_Two );
	Float64 low = GetParameter( kParam_Three );
	Float64 mid = (high+low)*0.5;
	Float64 spread = 1.001-fabs(high-low);
	
	biquad[biq_freq] = (pow(high,3)*20000.0)/GetSampleRate();
	if (biquad[biq_freq] < 0.00009) biquad[biq_freq] = 0.00009;
	Float64 compensation = sqrt(biquad[biq_freq])*6.4*spread;
	Float64 clipFactor = 0.75+(biquad[biq_freq]*GetParameter( kParam_Four )*37.0);
	
	biquadA[biq_freq] = (pow((high+mid)*0.5,3)*20000.0)/GetSampleRate();
	if (biquadA[biq_freq] < 0.00009) biquadA[biq_freq] = 0.00009;
	Float64 compensationA = sqrt(biquadA[biq_freq])*6.4*spread;
	Float64 clipFactorA = 0.75+(biquadA[biq_freq]*GetParameter( kParam_Four )*37.0);
	
	biquadB[biq_freq] = (pow(mid,3)*20000.0)/GetSampleRate();
	if (biquadB[biq_freq] < 0.00009) biquadB[biq_freq] = 0.00009;
	Float64 compensationB = sqrt(biquadB[biq_freq])*6.4*spread;
	Float64 clipFactorB = 0.75+(biquadB[biq_freq]*GetParameter( kParam_Four )*37.0);
	
	biquadC[biq_freq] = (pow((mid+low)*0.5,3)*20000.0)/GetSampleRate();
	if (biquadC[biq_freq] < 0.00009) biquadC[biq_freq] = 0.00009;
	Float64 compensationC = sqrt(biquadC[biq_freq])*6.4*spread;
	Float64 clipFactorC = 0.75+(biquadC[biq_freq]*GetParameter( kParam_Four )*37.0);
	
	biquadD[biq_freq] = (pow(low,3)*20000.0)/GetSampleRate();
	if (biquadD[biq_freq] < 0.00009) biquadD[biq_freq] = 0.00009;
	Float64 compensationD = sqrt(biquadD[biq_freq])*6.4*spread;
	Float64 clipFactorD = 0.75+(biquadD[biq_freq]*GetParameter( kParam_Four )*37.0);
	
	//set up all the interpolations
	biquad[biq_aA0] = biquad[biq_aB0];
	biquad[biq_aA1] = biquad[biq_aB1];
	biquad[biq_aA2] = biquad[biq_aB2];
	biquad[biq_bA1] = biquad[biq_bB1];
	biquad[biq_bA2] = biquad[biq_bB2];

	biquadA[biq_aA0] = biquadA[biq_aB0];
	biquadA[biq_aA1] = biquadA[biq_aB1];
	biquadA[biq_aA2] = biquadA[biq_aB2];
	biquadA[biq_bA1] = biquadA[biq_bB1];
	biquadA[biq_bA2] = biquadA[biq_bB2];

	biquadB[biq_aA0] = biquadB[biq_aB0];
	biquadB[biq_aA1] = biquadB[biq_aB1];
	biquadB[biq_aA2] = biquadB[biq_aB2];
	biquadB[biq_bA1] = biquadB[biq_bB1];
	biquadB[biq_bA2] = biquadB[biq_bB2];

	biquadC[biq_aA0] = biquadC[biq_aB0];
	biquadC[biq_aA1] = biquadC[biq_aB1];
	biquadC[biq_aA2] = biquadC[biq_aB2];
	biquadC[biq_bA1] = biquadC[biq_bB1];
	biquadC[biq_bA2] = biquadC[biq_bB2];

	biquadD[biq_aA0] = biquadD[biq_aB0];
	biquadD[biq_aA1] = biquadD[biq_aB1];
	biquadD[biq_aA2] = biquadD[biq_aB2];
	biquadD[biq_bA1] = biquadD[biq_bB1];
	biquadD[biq_bA2] = biquadD[biq_bB2];
	//since this is Region, they are all different
	
	double K = tan(M_PI * biquad[biq_freq]);
	double norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	biquad[biq_aB0] = K / 0.7071 * norm;
	biquad[biq_aB2] = -biquad[biq_aB0];
	biquad[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquad[biq_bB2] = (1.0 - K / 0.7071 + K * K) * norm;
	
	K = tan(M_PI * biquadA[biq_freq]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	biquadA[biq_aB0] = K / 0.7071 * norm;
	biquadA[biq_aB2] = -biquadA[biq_aB0];
	biquadA[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquadA[biq_bB2] = (1.0 - K / 0.7071 + K * K) * norm;
	
	K = tan(M_PI * biquadB[biq_freq]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	biquadB[biq_aB0] = K / 0.7071 * norm;
	biquadB[biq_aB2] = -biquadB[biq_aB0];
	biquadB[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquadB[biq_bB2] = (1.0 - K / 0.7071 + K * K) * norm;
	
	K = tan(M_PI * biquadC[biq_freq]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	biquadC[biq_aB0] = K / 0.7071 * norm;
	biquadC[biq_aB2] = -biquadC[biq_aB0];
	biquadC[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquadC[biq_bB2] = (1.0 - K / 0.7071 + K * K) * norm;
	
	K = tan(M_PI * biquadD[biq_freq]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	biquadD[biq_aB0] = K / 0.7071 * norm;
	biquadD[biq_aB2] = -biquadD[biq_aB0];
	biquadD[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquadD[biq_bB2] = (1.0 - K / 0.7071 + K * K) * norm;	
	//end from XRegion	
		
	//opamp stuff
	inTrimA = inTrimB;
	inTrimB = GetParameter( kParam_One )*10.0;
	inTrimB *= inTrimB; inTrimB *= inTrimB;
	wetA = wetB;
	wetB = GetParameter( kParam_Four );
	overallWetA = overallWetB;
	overallWetB = GetParameter( kParam_Five );

	double iirAmountA = 0.00069/overallscale;
	fixA[fix_freq] = fixB[fix_freq] = 15500.0 / GetSampleRate();
    fixA[fix_reso] = fixB[fix_reso] = 0.935;
	K = tan(M_PI * fixB[fix_freq]); //lowpass
	norm = 1.0 / (1.0 + K / fixB[fix_reso] + K * K);
	fixA[fix_a0] = fixB[fix_a0] = K * K * norm;
	fixA[fix_a1] = fixB[fix_a1] = 2.0 * fixB[fix_a0];
	fixA[fix_a2] = fixB[fix_a2] = fixB[fix_a0];
	fixA[fix_b1] = fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixA[fix_b2] = fixB[fix_b2] = (1.0 - K / fixB[fix_reso] + K * K) * norm;
	//end opamp stuff	
	
	double outSample = 0.0;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double overallDrySample = *sourceP;
		double nukeLevel = inputSample;

		double temp = (double)nSampleFrames/inFramesToProcess;
		biquad[biq_a0] = (biquad[biq_aA0]*temp)+(biquad[biq_aB0]*(1.0-temp));
		biquad[biq_a1] = (biquad[biq_aA1]*temp)+(biquad[biq_aB1]*(1.0-temp));
		biquad[biq_a2] = (biquad[biq_aA2]*temp)+(biquad[biq_aB2]*(1.0-temp));
		biquad[biq_b1] = (biquad[biq_bA1]*temp)+(biquad[biq_bB1]*(1.0-temp));
		biquad[biq_b2] = (biquad[biq_bA2]*temp)+(biquad[biq_bB2]*(1.0-temp));
		
		biquadA[biq_a0] = (biquadA[biq_aA0]*temp)+(biquadA[biq_aB0]*(1.0-temp));
		biquadA[biq_a1] = (biquadA[biq_aA1]*temp)+(biquadA[biq_aB1]*(1.0-temp));
		biquadA[biq_a2] = (biquadA[biq_aA2]*temp)+(biquadA[biq_aB2]*(1.0-temp));
		biquadA[biq_b1] = (biquadA[biq_bA1]*temp)+(biquadA[biq_bB1]*(1.0-temp));
		biquadA[biq_b2] = (biquadA[biq_bA2]*temp)+(biquadA[biq_bB2]*(1.0-temp));
		
		biquadB[biq_a0] = (biquadB[biq_aA0]*temp)+(biquadB[biq_aB0]*(1.0-temp));
		biquadB[biq_a1] = (biquadB[biq_aA1]*temp)+(biquadB[biq_aB1]*(1.0-temp));
		biquadB[biq_a2] = (biquadB[biq_aA2]*temp)+(biquadB[biq_aB2]*(1.0-temp));
		biquadB[biq_b1] = (biquadB[biq_bA1]*temp)+(biquadB[biq_bB1]*(1.0-temp));
		biquadB[biq_b2] = (biquadB[biq_bA2]*temp)+(biquadB[biq_bB2]*(1.0-temp));
		
		biquadC[biq_a0] = (biquadC[biq_aA0]*temp)+(biquadC[biq_aB0]*(1.0-temp));
		biquadC[biq_a1] = (biquadC[biq_aA1]*temp)+(biquadC[biq_aB1]*(1.0-temp));
		biquadC[biq_a2] = (biquadC[biq_aA2]*temp)+(biquadC[biq_aB2]*(1.0-temp));
		biquadC[biq_b1] = (biquadC[biq_bA1]*temp)+(biquadC[biq_bB1]*(1.0-temp));
		biquadC[biq_b2] = (biquadC[biq_bA2]*temp)+(biquadC[biq_bB2]*(1.0-temp));
		
		biquadD[biq_a0] = (biquadD[biq_aA0]*temp)+(biquadD[biq_aB0]*(1.0-temp));
		biquadD[biq_a1] = (biquadD[biq_aA1]*temp)+(biquadD[biq_aB1]*(1.0-temp));
		biquadD[biq_a2] = (biquadD[biq_aA2]*temp)+(biquadD[biq_aB2]*(1.0-temp));
		biquadD[biq_b1] = (biquadD[biq_bA1]*temp)+(biquadD[biq_bB1]*(1.0-temp));
		biquadD[biq_b2] = (biquadD[biq_bA2]*temp)+(biquadD[biq_bB2]*(1.0-temp));
		//this is the interpolation code for all the biquads
		
		double inTrim = (inTrimA*temp)+(inTrimB*(1.0-temp));
		double wet = (wetA*temp)+(wetB*(1.0-temp));
		double aWet = 1.0;
		double bWet = 1.0;
		double cWet = 1.0;
		double dWet = wet*4.0;
		//four-stage wet/dry control using progressive stages that bypass when not engaged
		if (dWet < 1.0) {aWet = dWet; bWet = 0.0; cWet = 0.0; dWet = 0.0;}
		else if (dWet < 2.0) {bWet = dWet - 1.0; cWet = 0.0; dWet = 0.0;}
		else if (dWet < 3.0) {cWet = dWet - 2.0; dWet = 0.0;}
		else {dWet -= 3.0;}
		//this is one way to make a little set of dry/wet stages that are successively added to the
		//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
		//beyond that point: this is a way to progressively add a 'black box' sound processing
		//which lets you fall through to simpler processing at lower settings.
		double overallWet = (overallWetA*temp)+(overallWetB*(1.0-temp));
		
		if (inTrim != 1.0) inputSample *= inTrim;
		
		inputSample *= clipFactor;
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);		
		outSample = (inputSample * biquad[biq_a0]) + biquad[biq_sL1];
		biquad[biq_sL1] = (inputSample * biquad[biq_a1]) - (outSample * biquad[biq_b1]) + biquad[biq_sL2];
		biquad[biq_sL2] = (inputSample * biquad[biq_a2]) - (outSample * biquad[biq_b2]);
		inputSample = outSample / compensation; nukeLevel = inputSample;
		
		if (aWet > 0.0) {
			inputSample *= clipFactorA;
			if (inputSample > 1.57079633) inputSample = 1.57079633;
			if (inputSample < -1.57079633) inputSample = -1.57079633;
			inputSample = sin(inputSample);
			outSample = (inputSample * biquadA[biq_a0]) + biquadA[biq_sL1];
			biquadA[biq_sL1] = (inputSample * biquadA[biq_a1]) - (outSample * biquadA[biq_b1]) + biquadA[biq_sL2];
			biquadA[biq_sL2] = (inputSample * biquadA[biq_a2]) - (outSample * biquadA[biq_b2]);
			inputSample = outSample / compensationA; inputSample = (inputSample * aWet) + (nukeLevel * (1.0-aWet));
			nukeLevel = inputSample;
		}
		if (bWet > 0.0) {
			inputSample *= clipFactorB;
			if (inputSample > 1.57079633) inputSample = 1.57079633;
			if (inputSample < -1.57079633) inputSample = -1.57079633;
			inputSample = sin(inputSample);
			outSample = (inputSample * biquadB[biq_a0]) + biquadB[biq_sL1];
			biquadB[biq_sL1] = (inputSample * biquadB[biq_a1]) - (outSample * biquadB[biq_b1]) + biquadB[biq_sL2];
			biquadB[biq_sL2] = (inputSample * biquadB[biq_a2]) - (outSample * biquadB[biq_b2]);			
			inputSample = outSample / compensationB; inputSample = (inputSample * bWet) + (nukeLevel * (1.0-bWet));
			nukeLevel = inputSample;
		}
		if (cWet > 0.0) {
			inputSample *= clipFactorC;
			if (inputSample > 1.57079633) inputSample = 1.57079633;
			if (inputSample < -1.57079633) inputSample = -1.57079633;
			inputSample = sin(inputSample);
			outSample = (inputSample * biquadC[biq_a0]) + biquadC[biq_sL1];
			biquadC[biq_sL1] = (inputSample * biquadC[biq_a1]) - (outSample * biquadC[biq_b1]) + biquadC[biq_sL2];
			biquadC[biq_sL2] = (inputSample * biquadC[biq_a2]) - (outSample * biquadC[biq_b2]);
			inputSample = outSample / compensationC; inputSample = (inputSample * cWet) + (nukeLevel * (1.0-cWet));
			nukeLevel = inputSample;
		}
		if (dWet > 0.0) {
			inputSample *= clipFactorD;
			if (inputSample > 1.57079633) inputSample = 1.57079633;
			if (inputSample < -1.57079633) inputSample = -1.57079633;
			inputSample = sin(inputSample);
			outSample = (inputSample * biquadD[biq_a0]) + biquadD[biq_sL1];
			biquadD[biq_sL1] = (inputSample * biquadD[biq_a1]) - (outSample * biquadD[biq_b1]) + biquadD[biq_sL2];
			biquadD[biq_sL2] = (inputSample * biquadD[biq_a2]) - (outSample * biquadD[biq_b2]);
			inputSample = outSample / compensationD; inputSample = (inputSample * dWet) + (nukeLevel * (1.0-dWet));
		}
				
		//opamp stage
		if (fabs(iirSampleA)<1.18e-37) iirSampleA = 0.0;
		iirSampleA = (iirSampleA * (1.0 - iirAmountA)) + (inputSample * iirAmountA);
		inputSample -= iirSampleA;
		
		outSample = (inputSample * fixA[fix_a0]) + fixA[fix_sL1];
		fixA[fix_sL1] = (inputSample * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sL2];
		fixA[fix_sL2] = (inputSample * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		if (inputSample > 1.0) inputSample = 1.0; if (inputSample < -1.0) inputSample = -1.0;
		inputSample -= (inputSample*inputSample*inputSample*inputSample*inputSample*0.1768);
		
		outSample = (inputSample * fixB[fix_a0]) + fixB[fix_sL1];
		fixB[fix_sL1] = (inputSample * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sL2];
		fixB[fix_sL2] = (inputSample * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		//end opamp stage
		
		if (overallWet !=1.0) {
			inputSample = (inputSample * overallWet) + (overallDrySample * (1.0-overallWet));
		}	
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirSampleA = 0.0;
	for (int x = 0; x < biq_total; x++) {biquad[x] = 0.0; biquadA[x] = 0.0; biquadB[x] = 0.0; biquadC[x] = 0.0; biquadD[x] = 0.0;}
	inTrimA = 0.1; inTrimB = 0.1;
	//outTrimA = 1.0; outTrimB = 1.0;
	wetA = 0.5; wetB = 0.5;
	overallWetA = 1.0; overallWetB = 1.0;
	for (int x = 0; x < fix_total; x++) {fixA[x] = 0.0; fixB[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
