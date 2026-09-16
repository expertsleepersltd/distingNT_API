#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "StudioTan"
#define AIRWINDOWS_DESCRIPTION "All the 'non-dither' dithers, up to date and convenient."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','t','n' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One = 0,
	//Add your parameters here...
	kNumberOfParameters=1
};
static const int kST = 0;
static const int kDM = 1;
static const int kNJ = 2;
static const int kSTCD = 3;
static const int kDMCD = 4;
static const int kNJCD = 5;
static const int kDefaultValue_ParamOne = kST;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static char const * const enumStrings0[] = { "Studio Tan 24", "Dither Me Timbers 24", "Not Just Another Dither 24", "Studio Tan 16", "Dither Me Timbers 16", "Not Just Another Dither 16", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Quantizer", .min = 0, .max = 5, .def = 0, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float byn[13];
		float noiseShaping;
		float lastSample;
		float lastSample2;
	
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
	int processing = (int) GetParameter( kParam_One );
	bool highres = true; //for 24 bit: false for 16 bit
	bool brightfloor = true; //for Studio Tan: false for Dither Me Timbers
	bool benford = true; //for Not Just Another Dither: false for newer two
	bool cutbins = false; //for NJAD: only attenuate bins if one gets very full
	switch (processing)
	{
		case 0: benford = false; break; //Studio Tan 24
		case 1: benford = false; brightfloor = false; break; //Dither Me Timbers 24
		case 2: break; //Not Just Another Dither 24
		case 3: benford = false; highres = false; break; //Studio Tan 16
		case 4: benford = false; brightfloor = false; highres = false; break; //Dither Me Timbers 16
		case 5: highres = false; break; //Not Just Another Dither 16
	}
	
	while (nSampleFrames-- > 0) {
		float inputSample;
		float outputSample;
		float drySample;
		
		if (highres) inputSample = *sourceP * 8388608.0f;
		else inputSample = *sourceP * 32768.0f;
		//shared input stage
		
		if (benford) {
			//begin Not Just Another Dither
			cutbins = false;
			drySample = inputSample;
			inputSample -= noiseShaping;
			float benfordize = floor(inputSample);
			while (benfordize >= 1.0f) {benfordize /= 10;}
			if (benfordize < 1.0f) {benfordize *= 10;}
			if (benfordize < 1.0f) {benfordize *= 10;}
			if (benfordize < 1.0f) {benfordize *= 10;}
			if (benfordize < 1.0f) {benfordize *= 10;}
			if (benfordize < 1.0f) {benfordize *= 10;}
			int hotbinA = floor(benfordize);
			//hotbin becomes the Benford bin value for this number floored
			float totalA = 0;
			if ((hotbinA > 0) && (hotbinA < 10))
			{
				byn[hotbinA] += 1;
				if (byn[hotbinA] > 982) cutbins = true;
				totalA += (301-byn[1]);
				totalA += (176-byn[2]);
				totalA += (125-byn[3]);
				totalA += (97-byn[4]);
				totalA += (79-byn[5]);
				totalA += (67-byn[6]);
				totalA += (58-byn[7]);
				totalA += (51-byn[8]);
				totalA += (46-byn[9]);
				byn[hotbinA] -= 1;
			} else {hotbinA = 10;}
			//produce total number- smaller is closer to Benford real
			
			benfordize = ceil(inputSample);
			while (benfordize >= 1.0f) {benfordize /= 10;}
			if (benfordize < 1.0f) {benfordize *= 10;}
			if (benfordize < 1.0f) {benfordize *= 10;}
			if (benfordize < 1.0f) {benfordize *= 10;}
			if (benfordize < 1.0f) {benfordize *= 10;}
			if (benfordize < 1.0f) {benfordize *= 10;}
			int hotbinB = floor(benfordize);
			//hotbin becomes the Benford bin value for this number ceiled
			float totalB = 0;
			if ((hotbinB > 0) && (hotbinB < 10))
			{
				byn[hotbinB] += 1;
				if (byn[hotbinB] > 982) cutbins = true;
				totalB += (301-byn[1]);
				totalB += (176-byn[2]);
				totalB += (125-byn[3]);
				totalB += (97-byn[4]);
				totalB += (79-byn[5]);
				totalB += (67-byn[6]);
				totalB += (58-byn[7]);
				totalB += (51-byn[8]);
				totalB += (46-byn[9]);
				byn[hotbinB] -= 1;
			} else {hotbinB = 10;}
			//produce total number- smaller is closer to Benford real
			
			if (totalA < totalB)
			{
				byn[hotbinA] += 1;
				outputSample = floor(inputSample);
			}
			else
			{
				byn[hotbinB] += 1;
				outputSample = floor(inputSample+1);
			}
			//assign the relevant one to the delay line
			//and floor/ceil signal accordingly
			if (cutbins) {
				byn[1] *= 0.99f;
				byn[2] *= 0.99f;
				byn[3] *= 0.99f;
				byn[4] *= 0.99f;
				byn[5] *= 0.99f;
				byn[6] *= 0.99f;
				byn[7] *= 0.99f;
				byn[8] *= 0.99f;
				byn[9] *= 0.99f;
				byn[10] *= 0.99f; //catchall for garbage data
			}
			noiseShaping += outputSample - drySample;
			
			//end Not Just Another Dither
		} else {		
			//begin StudioTan or Dither Me Timbers
			if (brightfloor) {
				lastSample -= (noiseShaping*0.8f);
				if ((lastSample+lastSample) <= (inputSample+lastSample2)) outputSample = floor(lastSample); //StudioTan
				else outputSample = floor(lastSample+1.0f); //round down or up based on whether it softens treble angles
			} else {
				lastSample -= (noiseShaping*0.11f);
				if ((lastSample+lastSample) >= (inputSample+lastSample2)) outputSample = floor(lastSample); //DitherMeTimbers
				else outputSample = floor(lastSample+1.0f); //round down or up based on whether it softens treble angles
			}
			noiseShaping += outputSample;
			noiseShaping -= lastSample; //apply noise shaping
			lastSample2 = lastSample;
			lastSample = inputSample; //we retain three samples in a row
			//end StudioTan or Dither Me Timbers
		}
		
		//shared output stage
		float noiseSuppress = fabs(inputSample);
		if (noiseShaping > noiseSuppress) noiseShaping = noiseSuppress;
		if (noiseShaping < -noiseSuppress) noiseShaping = -noiseSuppress;
		
		Float32 ironBar;
		if (highres) ironBar = outputSample / 8388608.0f;
		else ironBar = outputSample / 32768.0f;
		
		if (ironBar > 1.0f) ironBar = 1.0f;
		if (ironBar < -1.0f) ironBar = -1.0f;
		*destP = ironBar;
		
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	byn[0] = 1000.0;
	byn[1] = 301.0;
	byn[2] = 176.0;
	byn[3] = 125.0;
	byn[4] = 97.0;
	byn[5] = 79.0;
	byn[6] = 67.0;
	byn[7] = 58.0;
	byn[8] = 51.0;
	byn[9] = 46.0;
	byn[10] = 1000.0;
	noiseShaping = 0.0;
	lastSample = 0.0;
	lastSample2 = 0.0;
}
};
