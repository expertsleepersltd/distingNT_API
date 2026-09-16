#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "IronOxide5"
#define AIRWINDOWS_DESCRIPTION "The old school, heavily colored Airwindows tape emulation."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','r','5' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	kParam_Seven =6,
	//Add your parameters here...
	kNumberOfParameters=7
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Input Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Highs Tape Speed", .min = 150, .max = 15000, .def = 1500, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Lows Tape Speed", .min = 150, .max = 15000, .def = 1500, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Flutter", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Noise", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Dry/Wet", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 iirSamplehA;
		Float32 iirSamplehB;
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 fastIIRA;
		Float32 fastIIRB;
		Float32 slowIIRA;
		Float32 slowIIRB;
		Float32 fastIIHA;
		Float32 fastIIHB;
		Float32 slowIIHA;
		Float32 slowIIHB;
		SInt32 gcount;
		Float32 prevInputSample;
		
		SInt32 fstoredcount;
		Float32 rateof;
		Float32 sweep;
		Float32 nextmax;
		uint32_t fpd;
		bool flip;
	
	struct _dram {
			Float32 d[264];
		Float32 fl[100];
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
	
	Float32 inputgain = powf(10.0f,GetParameter( kParam_One )/20.0f);
	Float32 outputgain = powf(10.0f,GetParameter( kParam_Six )/20.0f);
	Float32 ips = GetParameter( kParam_Two ) * 1.1f;
	//slight correction to dial in convincing ips settings
	if (ips < 1 || ips > 200){ips=33.0f;}
	//sanity checks are always key
	Float32 tempRandy = 0.04f+(0.11f/sqrt(ips));
	Float32 randy;
	Float32 lps = GetParameter( kParam_Three ) * 1.1f;
	//slight correction to dial in convincing ips settings
	if (lps < 1 || lps > 200){lps=33.0f;}
	//sanity checks are always key
	Float32 iirAmount = lps/430.0f; //for low leaning
	Float32 bridgerectifier;
	Float32 fastTaper = ips/15.0f;
	Float32 slowTaper = 2.0f/(lps*lps);
	Float32 lowspeedscale = (5.0f/ips);
	float inputSample;
	Float32 drySample;
	SInt32 count;
	SInt32 flutcount;
	Float32 flutterrandy;
	Float32 temp;
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 depth = powf(GetParameter( kParam_Four ),2)*overallscale;
	Float32 fluttertrim = 0.00581f/overallscale;
	Float32 sweeptrim = (0.0005f*depth)/overallscale;
	Float32 offset;	
	Float32 tupi = 3.141592653589793238f * 2.0f;
	Float32 newrate = 0.006f/overallscale;
	Float32 oldrate = 1.0f-newrate;	
	if (overallscale == 0) {fastTaper += 1.0f; slowTaper += 1.0f;}
	else
	{
		iirAmount /= overallscale;
		lowspeedscale *= overallscale;
		fastTaper = 1.0f + (fastTaper / overallscale);
		slowTaper = 1.0f + (slowTaper / overallscale);
	}
	Float32 noise = GetParameter( kParam_Five ) * 0.5f;
	Float32 invdrywet = GetParameter( kParam_Seven );
	Float32 dry = 1.0f;
	if (invdrywet > 0.0f) dry -= invdrywet;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		flutterrandy = (float(fpd)/UINT32_MAX);
		//part of flutter section
		//now we've got a random flutter, so we're messing with the pitch before tape effects go on
		if (fstoredcount < 0 || fstoredcount > 30) {fstoredcount = 30;}
		flutcount = fstoredcount;
		dram->fl[flutcount+31] = dram->fl[flutcount] = inputSample;
		offset = (1.0f + sin(sweep)) * depth;
		flutcount += (int)floor(offset);
		bridgerectifier = (dram->fl[flutcount] * (1-(offset-floor(offset))));
		bridgerectifier += (dram->fl[flutcount+1] * (offset-floor(offset)));
		rateof = (nextmax * newrate) + (rateof * oldrate);
		sweep += rateof * fluttertrim;
		sweep += sweep * sweeptrim;
		if (sweep >= tupi){sweep = 0.0f; nextmax = 0.02f + (flutterrandy*0.98f);}
		fstoredcount--;
		inputSample = bridgerectifier;
		//apply to input signal, interpolate samples
		//all the funky renaming is just trying to fix how I never reassigned the control numbers
		
		if (flip)
		{
			iirSampleA = (iirSampleA * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirSampleA;
		}
		else
		{
			iirSampleB = (iirSampleB * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirSampleB;
		}
		//do IIR highpass for leaning out
		inputSample *= inputgain;
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0.0f) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//preliminary gain stage using antialiasing
		
		//over to the Iron Oxide shaping code using inputsample
		if (gcount < 0 || gcount > 131) {gcount = 131;}
		count = gcount;
		//increment the counter
		
		dram->d[count+131] = dram->d[count] = inputSample;
		if (flip)
		{
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
		inputSample /= fastTaper;
		inputSample /= lowspeedscale;
		//inputsample side
		//post-center code on inputSample and halfwaySample in parallel
		//begin raw sample- inputSample and ataDrySample handled separately here
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = sin(bridgerectifier);
		//can use as an output limiter
		if (inputSample > 0.0f) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//second stage of overdrive to prevent overs and allow bloody loud extremeness		
		
		randy = (0.55f + tempRandy + ((float(fpd)/UINT32_MAX)*tempRandy))*noise; //0 to 2
		inputSample *= (1.0f - randy);
		inputSample += (prevInputSample*randy);
		prevInputSample = drySample;
		
		flip = !flip;
		
		//begin invdrywet block with outputgain
		if (outputgain != 1.0f) inputSample *= outputgain;
		if (invdrywet != 1.0f) inputSample *= invdrywet;
		if (dry != 1.0f) drySample *= dry;
		if (fabs(drySample) > 0.0f) inputSample += drySample;
		//end invdrywet block with outputgain
				
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	int temp;
	for (temp = 0; temp < 263; temp++) {dram->d[temp] = 0.0;}
	gcount = 0;
	fastIIRA = fastIIRB = slowIIRA = slowIIRB = 0.0;
	fastIIHA = fastIIHB = slowIIHA = slowIIHB = 0.0;
	iirSamplehA = iirSamplehB = 0.0;
	iirSampleA = iirSampleB = 0.0;
	prevInputSample = 0.0;
	flip = false;
	for (temp = 0; temp < 99; temp++) {dram->fl[temp] = 0.0;}
	fstoredcount = 0;	
	sweep = 0.0;
	rateof = 0.5;
	nextmax = 0.5;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
