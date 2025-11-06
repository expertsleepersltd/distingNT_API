#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DynamicsMono"
#define AIRWINDOWS_DESCRIPTION "DynamicsMono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','y','q' )
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
{ .name = "Comp", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gate", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		//begin Gate
		bool WasNegative;
		int ZeroCross;
		float gateroller;
		float gate;
		//end Gate
		
		//begin ButterComp
		float controlApos;
		float controlAneg;
		float controlBpos;
		float controlBneg;
		float targetpos;
		float targetneg;
		float avgA;
		float avgB;
		float nvgA;
		float nvgB;
		
		bool flip;
		//end ButterComp
		
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
	
	//begin ButterComp
	float inputgain = (powf(GetParameter( kParam_One ),5)*35)+1.0f;
	float divisor = (powf(GetParameter( kParam_Two ),4) * 0.01f)+0.0005f;
	divisor /= overallscale;
	float remainder = divisor;
	divisor = 1.0f - divisor;
	//end ButterComp
	
	//begin Gate
	float onthreshold = (powf(GetParameter( kParam_Three ),3)/3)+0.00018f;
	float offthreshold = onthreshold * 1.1f;	
	float release = 0.028331119964586f;
	float absmax = 220.9f;
	//speed to be compensated w.r.t sample rate
	//end Gate
	
	float wet = GetParameter(kParam_Four );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		//begin compressor
		inputSample *= inputgain;
		float inputpos = inputSample + 1.0f;		
		if (inputpos < 0.0f) inputpos = 0.0f;
		float outputpos = inputpos / 2.0f;
		if (outputpos > 1.0f) outputpos = 1.0f;		
		inputpos *= inputpos;
		targetpos *= divisor;
		targetpos += (inputpos * remainder);
		float calcpos = 1.0f/targetpos;
		
		float inputneg = -inputSample + 1.0f;		
		if (inputneg < 0.0f) inputneg = 0.0f;
		float outputneg = inputneg / 2.0f;
		if (outputneg > 1.0f) outputneg = 1.0f;		
		inputneg *= inputneg;
		targetneg *= divisor;
		targetneg += (inputneg * remainder);
		float calcneg = 1.0f/targetneg;
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1
		
		if (inputSample > 0)
		{ //working on pos
			if (true == flip)
			{
				controlApos *= divisor;
				controlApos += (calcpos*remainder);
			} else {
				controlBpos *= divisor;
				controlBpos += (calcpos*remainder);
			}	
		} else { //working on neg
			if (true == flip)
			{
				controlAneg *= divisor;
				controlAneg += (calcneg*remainder);
			} else {
				controlBneg *= divisor;
				controlBneg += (calcneg*remainder);
			}
		}
		//this causes each of the four to update only when active and in the correct 'flip'
		
		float totalmultiplier;
		if (true == flip) totalmultiplier = (controlApos * outputpos) + (controlAneg * outputneg);
		else totalmultiplier = (controlBpos * outputpos) + (controlBneg * outputneg);
		//this combines the sides according to flip, blending relative to the input value
		inputSample *= totalmultiplier;
		inputSample /= inputgain;
		
		flip = !flip;
		//end compressor
		
		//begin Gate
		if (drySample > 0.0f)
		{
			if (WasNegative == true) ZeroCross = absmax * 0.3f;
			WasNegative = false;
		} else {
			ZeroCross += 1; WasNegative = true;
		}
				
		if (ZeroCross > absmax) ZeroCross = absmax;
		
		if (gate == 0.0f)
		{
			//if gate is totally silent
			if (fabs(drySample) > onthreshold)
			{
				if (gateroller == 0.0f) gateroller = ZeroCross;
				else gateroller -= release;
				// trigger from total silence only- if we're active then signal must clear offthreshold
			}
			else gateroller -= release;
		} else {
			//gate is not silent but closing
			if (fabs(drySample) > offthreshold)
			{
				if (gateroller < ZeroCross) gateroller = ZeroCross;
				else gateroller -= release;
				//always trigger if gate is over offthreshold, otherwise close anyway
			}
			else gateroller -= release;
		}
				
		if (gateroller < 0.0f) gateroller = 0.0f;
		
		if (gateroller < 1.0f)
		{
			gate = gateroller;
			float bridgerectifier = 1-cos(fabs(inputSample));			
			if (inputSample > 0) inputSample = (inputSample*gate)+(bridgerectifier*(1.0f-gate));
			else inputSample = (inputSample*gate)-(bridgerectifier*(1.0f-gate));
			if (gate == 0.0f) inputSample = 0.0f;			
		} else gate = 1.0f;
		//end Gate
		
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
	//begin Gate
	WasNegative = false;
	ZeroCross = 0;
	gateroller = 0.0;
	gate = 0.0;
	//end Gate
	
	//begin ButterComp
	controlApos = 1.0;
	controlAneg = 1.0;
	controlBpos = 1.0;
	controlBneg = 1.0;
	targetpos = 1.0;
	targetneg = 1.0;	
	avgA = avgB = 0.0;
	nvgA = nvgB = 0.0;
	//end ButterComp
	flip = false;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
