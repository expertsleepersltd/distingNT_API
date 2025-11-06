#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Slew4"
#define AIRWINDOWS_DESCRIPTION "Slew4"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','l','h' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Compres", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		float slew[24][12];
		int slewCount;
		float sustainSlew;
		float secondSample;
		float thirdSample;
		
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

	float compresity = powf(GetParameter( kParam_A )*0.95f,2.0f)*8.0f*overallscale;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		sustainSlew += fabs((inputSample-secondSample)-(secondSample-thirdSample));
		sustainSlew *= 0.9f; inputSample = thirdSample; //two sample lookahead
		float depthAmt = fmin(sin(fmin(sustainSlew,M_PI_2))*compresity,9.0f);
		int z = floor(depthAmt);
		float depthTrim = depthAmt - (float)z;
		
		for (int y = 0; y < z; y++) {
			switch (y) {
				case 0:
					slew[slewCount%2][y] = inputSample;
					inputSample = (slew[0][y] + slew[1][y]) * 0.5f;
					break;
				case 1:
					slew[slewCount%4][y] = inputSample;
					inputSample = (slew[0][y]+slew[1][y]+slew[2][y]+slew[3][y]) * 0.25f;
					break;
				case 2:
					slew[slewCount%6][y] = inputSample;
					inputSample = slew[0][y]+slew[1][y]+slew[2][y];
					inputSample += slew[3][y]+slew[4][y]+slew[5][y];
					inputSample *= 0.1666666666f;
					break;
				case 3:
					slew[slewCount%8][y] = inputSample;
					inputSample = slew[0][y]+slew[1][y]+slew[2][y]+slew[3][y];
					inputSample += slew[4][y]+slew[5][y]+slew[6][y]+slew[7][y];
					inputSample *= 0.125f;
					break;
				case 4:
					slew[slewCount%10][y] = inputSample;
					inputSample = slew[0][y]+slew[1][y]+slew[2][y]+slew[3][y]+slew[4][y];
					inputSample += slew[5][y]+slew[6][y]+slew[7][y]+slew[8][y]+slew[9][y];
					inputSample *= 0.1f;
					break;
				case 5:
					slew[slewCount%12][y] = inputSample;
					inputSample = slew[0][y]+slew[1][y]+slew[2][y]+slew[3][y]+slew[4][y]+slew[5][y];
					inputSample += slew[6][y]+slew[7][y]+slew[8][y]+slew[9][y]+slew[10][y]+slew[11][y];
					inputSample *= 0.0833333333f;
					break;
				case 6:
					slew[slewCount%14][y] = inputSample;
					inputSample = slew[0][y]+slew[1][y]+slew[2][y]+slew[3][y]+slew[4][y]+slew[5][y]+slew[6][y];
					inputSample += slew[7][y]+slew[8][y]+slew[9][y]+slew[10][y]+slew[11][y]+slew[12][y]+slew[13][y];
					inputSample *= 0.0714285f;
					break;
				case 7:
					slew[slewCount%16][y] = inputSample;
					inputSample = slew[0][y]+slew[1][y]+slew[2][y]+slew[3][y]+slew[4][y]+slew[5][y]+slew[6][y]+slew[7][y];
					inputSample += slew[8][y]+slew[9][y]+slew[10][y]+slew[11][y]+slew[12][y]+slew[13][y]+slew[14][y]+slew[15][y];
					inputSample *= 0.0625f;
					break;
				case 8:
					slew[slewCount%18][y] = inputSample;
					inputSample = slew[0][y]+slew[1][y]+slew[2][y]+slew[3][y]+slew[4][y]+slew[5][y]+slew[6][y]+slew[7][y]+slew[8][y];
					inputSample += slew[9][y]+slew[10][y]+slew[11][y]+slew[12][y]+slew[13][y]+slew[14][y]+slew[15][y]+slew[16][y]+slew[17][y];
					inputSample *= 0.05555555f;
					break;
				case 9:
					slew[slewCount%20][y] = inputSample;
					inputSample = slew[0][y]+slew[1][y]+slew[2][y]+slew[3][y]+slew[4][y]+slew[5][y]+slew[6][y]+slew[7][y]+slew[8][y]+slew[9][y];
					inputSample += slew[10][y]+slew[11][y]+slew[12][y]+slew[13][y]+slew[14][y]+slew[15][y]+slew[16][y]+slew[17][y]+slew[18][y]+slew[19][y];
					inputSample *= 0.05f;
					break;
			} //generate sets of rectangular window averages to layer
		} //all of which have a 'node' (silence) at Nyquist to combine
		
		float remainderSample = 0.0f;
		switch (z) {
			case 0:
				slew[slewCount%2][z] = inputSample;
				remainderSample = (slew[0][z] + slew[1][z]) * 0.5f;
				break;
			case 1:
				slew[slewCount%4][z] = inputSample;
				remainderSample = (slew[0][z]+slew[1][z]+slew[2][z]+slew[3][z]) * 0.25f;
				break;
			case 2:
				slew[slewCount%6][z] = inputSample;
				remainderSample = slew[0][z]+slew[1][z]+slew[2][z];
				remainderSample += slew[3][z]+slew[4][z]+slew[5][z];
				remainderSample *= 0.1666666666f;
				break;
			case 3:
				slew[slewCount%8][z] = inputSample;
				remainderSample = slew[0][z]+slew[1][z]+slew[2][z]+slew[3][z];
				remainderSample += slew[4][z]+slew[5][z]+slew[6][z]+slew[7][z];
				remainderSample *= 0.125f;
				break;
			case 4:
				slew[slewCount%10][z] = inputSample;
				remainderSample = slew[0][z]+slew[1][z]+slew[2][z]+slew[3][z]+slew[4][z];
				remainderSample	+= slew[5][z]+slew[6][z]+slew[7][z]+slew[8][z]+slew[9][z];
				remainderSample *= 0.1f;
				break;
			case 5:
				slew[slewCount%12][z] = inputSample;
				remainderSample = slew[0][z]+slew[1][z]+slew[2][z]+slew[3][z]+slew[4][z]+slew[5][z];
				remainderSample += slew[6][z]+slew[7][z]+slew[8][z]+slew[9][z]+slew[10][z]+slew[11][z];
				remainderSample *= 0.0833333333f;
				break;
			case 6:
				slew[slewCount%14][z] = inputSample;
				remainderSample = slew[0][z]+slew[1][z]+slew[2][z]+slew[3][z]+slew[4][z]+slew[5][z]+slew[6][z];
				remainderSample += slew[7][z]+slew[8][z]+slew[9][z]+slew[10][z]+slew[11][z]+slew[12][z]+slew[13][z];
				remainderSample *= 0.0714285f;
				break;
			case 7:
				slew[slewCount%16][z] = inputSample;
				remainderSample = slew[0][z]+slew[1][z]+slew[2][z]+slew[3][z]+slew[4][z]+slew[5][z]+slew[6][z]+slew[7][z];
				remainderSample += slew[8][z]+slew[9][z]+slew[10][z]+slew[11][z]+slew[12][z]+slew[13][z]+slew[14][z]+slew[15][z];
				remainderSample *= 0.0625f;
				break;
			case 8:
				slew[slewCount%18][z] = inputSample;
				remainderSample = slew[0][z]+slew[1][z]+slew[2][z]+slew[3][z]+slew[4][z]+slew[5][z]+slew[6][z]+slew[7][z]+slew[8][z];
				remainderSample += slew[9][z]+slew[10][z]+slew[11][z]+slew[12][z]+slew[13][z]+slew[14][z]+slew[15][z]+slew[16][z]+slew[17][z];
				remainderSample *= 0.0555555f;
				break;
			case 9:
				slew[slewCount%20][z] = inputSample;
				remainderSample = slew[0][z]+slew[1][z]+slew[2][z]+slew[3][z]+slew[4][z]+slew[5][z]+slew[6][z]+slew[7][z]+slew[8][z]+slew[9][z];
				remainderSample += slew[10][z]+slew[11][z]+slew[12][z]+slew[13][z]+slew[14][z]+slew[15][z]+slew[16][z]+slew[17][z]+slew[18][z]+slew[19][z];
				remainderSample *= 0.05f;
				break;
		} //generate the final fractional amount
		
		for (int y = z+1; y < 9; y++) {
			switch (y) {
				case 0:
					slew[slewCount%2][y] = inputSample; break;
				case 1:
					slew[slewCount%4][y] = inputSample; break;
				case 2:
					slew[slewCount%6][y] = inputSample; break;
				case 3:
					slew[slewCount%8][y] = inputSample; break;
				case 4:
					slew[slewCount%10][y] = inputSample; break;
				case 5:
					slew[slewCount%12][y] = inputSample; break;
				case 6:
					slew[slewCount%14][y] = inputSample; break;
				case 7:
					slew[slewCount%16][y] = inputSample; break;
				case 8:
					slew[slewCount%18][y] = inputSample; break;
				case 9:
					slew[slewCount%20][y] = inputSample; break;
			}
		} //clean up the little buffers when not being actively used
		
		slewCount++; if (slewCount > 619315200) slewCount = 0;
		thirdSample = secondSample; secondSample = drySample;
		
		inputSample = (inputSample*(1.0f-depthTrim))+(remainderSample*depthTrim);
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int x = 0; x < 23; x++) {
		for (int y = 0; y < 11; y++) slew[x][y] = 0.0;
	}
	slewCount = 0;
	sustainSlew = 0.0;
	secondSample = 0.0;
	thirdSample = 0.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
