#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "IronOxideClassic2"
#define AIRWINDOWS_DESCRIPTION "My bandpassy tape sim, updated for high sample rate and aliasing control."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','r','2' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Input Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tape Speed", .min = 150, .max = 15000, .def = 1500, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Output Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float biquadA[11];
		float biquadB[11];
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 fastIIRA;
		Float32 fastIIRB;
		Float32 slowIIRA;
		Float32 slowIIRB;
		Float32 lastRef[7];
		int cycle;
		int gcount;
		bool flip;
		uint32_t fpd;
	
	struct _dram {
			Float32 d[264];
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
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check
	
	Float32 inputgain = powf(10.0f,GetParameter( kParam_One )/20.0f);
	Float32 outputgain = powf(10.0f,GetParameter( kParam_Three )/20.0f);
	Float32 ips = GetParameter( kParam_Two ) * 1.1f;
	//slight correction to dial in convincing ips settings
	if (ips < 1 || ips > 200) ips=33.0f;
	//sanity checks are always key
	Float32 iirAmount = ips/430.0f; //for low leaning
	Float32 fastTaper = ips/15.0f;
	Float32 slowTaper = 2.0f/(ips*ips);
	
	iirAmount /= overallscale;
	fastTaper /= overallscale;
	slowTaper /= overallscale;
	//now that we have this, we must multiply it back up
	fastTaper *= cycleEnd;
	slowTaper *= cycleEnd;
	//because we're only running that part one sample in two, or three, or four
	fastTaper += 1.0f;
	slowTaper += 1.0f;
	
	biquadA[0] = 24000.0f / GetSampleRate();
    biquadA[1] = 1.618033988749894848204586f;
	biquadB[0] = 24000.0f / GetSampleRate();
    biquadB[1] = 0.618033988749894848204586f;
	
	float K = tan(M_PI * biquadA[0]); //lowpass
	float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
	biquadA[2] = K * K * norm;
	biquadA[3] = 2.0f * biquadA[2];
	biquadA[4] = biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	
	K = tan(M_PI * biquadB[0]); //lowpass
	norm = 1.0f / (1.0f + K / biquadB[1] + K * K);
	biquadB[2] = K * K * norm;
	biquadB[3] = 2.0f * biquadB[2];
	biquadB[4] = biquadB[2];
	biquadB[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadB[6] = (1.0f - K / biquadB[1] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (flip)
		{
			if (fabs(iirSampleA)<1.18e-37f) iirSampleA = 0.0f;
			iirSampleA = (iirSampleA * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirSampleA;
		}
		else
		{
			if (fabs(iirSampleB)<1.18e-37f) iirSampleB = 0.0f;
			iirSampleB = (iirSampleB * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirSampleB;
		}
		//do IIR highpass for leaning out
		
		if (biquadA[0] < 0.49999f) {
			float tempSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
			biquadA[8] = biquadA[7]; biquadA[7] = inputSample; inputSample = tempSample; 
			biquadA[10] = biquadA[9]; biquadA[9] = inputSample; //DF1
		}		
		
		if (inputgain != 1.0f) inputSample *= inputgain;
		
		float bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0.0f) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;		
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a tape sample
		
		if (gcount < 0 || gcount > 131) gcount = 131;
		int count = gcount;
		//increment the counter
		
		float temp;
		dram->d[count+131] = dram->d[count] = inputSample;
			
		if (flip)
		{
			if (fabs(fastIIRA)<1.18e-37f) fastIIRA = 0.0f;
			if (fabs(slowIIRA)<1.18e-37f) slowIIRA = 0.0f;
			fastIIRA = fastIIRA/fastTaper;
			slowIIRA = slowIIRA/slowTaper;
			//scale stuff down
			fastIIRA += dram->d[count];
			count += 3;
			temp = dram->d[count+127];
			temp += dram->d[count+113];
			temp += dram->d[count+109];
			temp += dram->d[count+107];
			temp += dram->d[count+103];
			temp += dram->d[count+101];
			temp += dram->d[count+97];
			temp += dram->d[count+89];
			temp += dram->d[count+83];
			temp /= 2;
			temp += dram->d[count+79];
			temp += dram->d[count+73];
			temp += dram->d[count+71];
			temp += dram->d[count+67];
			temp += dram->d[count+61];
			temp += dram->d[count+59];
			temp += dram->d[count+53];
			temp += dram->d[count+47];
			temp += dram->d[count+43];
			temp += dram->d[count+41];
			temp += dram->d[count+37];
			temp += dram->d[count+31];
			temp += dram->d[count+29];
			temp /= 2;
			temp += dram->d[count+23];
			temp += dram->d[count+19];
			temp += dram->d[count+17];
			temp += dram->d[count+13];
			temp += dram->d[count+11];
			temp /= 2;
			temp += dram->d[count+7];
			temp += dram->d[count+5];
			temp += dram->d[count+3];
			temp /= 2;
			temp += dram->d[count+2];
			temp += dram->d[count+1];
			slowIIRA += (temp/128);
			inputSample = fastIIRA - (slowIIRA / slowTaper);
		}
		else
		{
			if (fabs(fastIIRB)<1.18e-37f) fastIIRB = 0.0f;
			if (fabs(slowIIRB)<1.18e-37f) slowIIRB = 0.0f;
			fastIIRB = fastIIRB/fastTaper;
			slowIIRB = slowIIRB/slowTaper;
			//scale stuff down
			fastIIRB += dram->d[count];
			count += 3;
			temp = dram->d[count+127];
			temp += dram->d[count+113];
			temp += dram->d[count+109];
			temp += dram->d[count+107];
			temp += dram->d[count+103];
			temp += dram->d[count+101];
			temp += dram->d[count+97];
			temp += dram->d[count+89];
			temp += dram->d[count+83];
			temp /= 2;
			temp += dram->d[count+79];
			temp += dram->d[count+73];
			temp += dram->d[count+71];
			temp += dram->d[count+67];
			temp += dram->d[count+61];
			temp += dram->d[count+59];
			temp += dram->d[count+53];
			temp += dram->d[count+47];
			temp += dram->d[count+43];
			temp += dram->d[count+41];
			temp += dram->d[count+37];
			temp += dram->d[count+31];
			temp += dram->d[count+29];
			temp /= 2;
			temp += dram->d[count+23];
			temp += dram->d[count+19];
			temp += dram->d[count+17];
			temp += dram->d[count+13];
			temp += dram->d[count+11];
			temp /= 2;
			temp += dram->d[count+7];
			temp += dram->d[count+5];
			temp += dram->d[count+3];
			temp /= 2;
			temp += dram->d[count+2];
			temp += dram->d[count+1];
			slowIIRB += (temp/128);
			inputSample = fastIIRB - (slowIIRB / slowTaper);
		}
			
			if (cycleEnd == 4) {
				lastRef[0] = lastRef[4]; //start from previous last
				lastRef[2] = (lastRef[0] + inputSample)/2; //half
				lastRef[1] = (lastRef[0] + lastRef[2])/2; //one quarter
				lastRef[3] = (lastRef[2] + inputSample)/2; //three quarters
				lastRef[4] = inputSample; //full
			}
			if (cycleEnd == 3) {
				lastRef[0] = lastRef[3]; //start from previous last
				lastRef[2] = (lastRef[0]+lastRef[0]+inputSample)/3; //third
				lastRef[1] = (lastRef[0]+inputSample+inputSample)/3; //two thirds
				lastRef[3] = inputSample; //full
			}
			if (cycleEnd == 2) {
				lastRef[0] = lastRef[2]; //start from previous last
				lastRef[1] = (lastRef[0] + inputSample)/2; //half
				lastRef[2] = inputSample; //full
			}
			if (cycleEnd == 1) lastRef[0] = inputSample;
			cycle = 0; //reset
			inputSample = lastRef[cycle];
		} else {
			inputSample = lastRef[cycle];
			//we are going through our references now
		}
		
		
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		//can use as an output limiter
		if (inputSample > 0.0f) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//second stage of overdrive to prevent overs and allow bloody loud extremeness
		
		if (biquadB[0] < 0.49999f) {
			float tempSample = biquadB[2]*inputSample+biquadB[3]*biquadB[7]+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
			biquadB[8] = biquadB[7]; biquadB[7] = inputSample; inputSample = tempSample; 
			biquadB[10] = biquadB[9]; biquadB[9] = inputSample; //DF1
		}
		
		if (outputgain != 1.0f) inputSample *= outputgain;
		flip = !flip;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 11; x++) {biquadA[x] = 0.0;biquadB[x] = 0.0;}
	for (int temp = 0; temp < 263; temp++) {dram->d[temp] = 0.0;}
	for(int count = 0; count < 6; count++) {lastRef[count] = 0.0;}
	cycle = 0;
	gcount = 0;
	fastIIRA = fastIIRB = slowIIRA = slowIIRB = 0.0;
	iirSampleA = iirSampleB = 0.0;
	flip = true;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
