#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SubsOnly"
#define AIRWINDOWS_DESCRIPTION "A mix check plugin that shows you only the extreme lows."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','u','y' )
#define AIRWINDOWS_KERNELS
enum {

	//Add your parameters here...
	kNumberOfParameters=0
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
};
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 iirSampleC;
		Float32 iirSampleD;
		Float32 iirSampleE;
		Float32 iirSampleF;
		Float32 iirSampleG;
		Float32 iirSampleH;
		Float32 iirSampleI;
		Float32 iirSampleJ;
		Float32 iirSampleK;
		Float32 iirSampleL;
		Float32 iirSampleM;
		Float32 iirSampleN;
		Float32 iirSampleO;
		Float32 iirSampleP;
		Float32 iirSampleQ;
		Float32 iirSampleR;
		Float32 iirSampleS;
		Float32 iirSampleT;
		Float32 iirSampleU;
		Float32 iirSampleV;
		Float32 iirSampleW;
		Float32 iirSampleX;
		Float32 iirSampleY;
		Float32 iirSampleZ;
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
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 iirAmount = 2250/44100.0f;
	Float32 gaintarget = 1.42f;
	Float32 gain;
	iirAmount /= overallscale;
	Float32 altAmount = 1.0f - iirAmount;
	Float32 inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		gain = gaintarget;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		iirSampleA = (iirSampleA * altAmount) + (inputSample * iirAmount); inputSample = iirSampleA;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleB = (iirSampleB * altAmount) + (inputSample * iirAmount); inputSample = iirSampleB;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleC = (iirSampleC * altAmount) + (inputSample * iirAmount); inputSample = iirSampleC;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleD = (iirSampleD * altAmount) + (inputSample * iirAmount); inputSample = iirSampleD;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleE = (iirSampleE * altAmount) + (inputSample * iirAmount); inputSample = iirSampleE;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleF = (iirSampleF * altAmount) + (inputSample * iirAmount); inputSample = iirSampleF;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleG = (iirSampleG * altAmount) + (inputSample * iirAmount); inputSample = iirSampleG;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleH = (iirSampleH * altAmount) + (inputSample * iirAmount); inputSample = iirSampleH;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleI = (iirSampleI * altAmount) + (inputSample * iirAmount); inputSample = iirSampleI;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleJ = (iirSampleJ * altAmount) + (inputSample * iirAmount); inputSample = iirSampleJ;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleK = (iirSampleK * altAmount) + (inputSample * iirAmount); inputSample = iirSampleK;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleL = (iirSampleL * altAmount) + (inputSample * iirAmount); inputSample = iirSampleL;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleM = (iirSampleM * altAmount) + (inputSample * iirAmount); inputSample = iirSampleM;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleN = (iirSampleN * altAmount) + (inputSample * iirAmount); inputSample = iirSampleN;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleO = (iirSampleO * altAmount) + (inputSample * iirAmount); inputSample = iirSampleO;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleP = (iirSampleP * altAmount) + (inputSample * iirAmount); inputSample = iirSampleP;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleQ = (iirSampleQ * altAmount) + (inputSample * iirAmount); inputSample = iirSampleQ;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleR = (iirSampleR * altAmount) + (inputSample * iirAmount); inputSample = iirSampleR;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleS = (iirSampleS * altAmount) + (inputSample * iirAmount); inputSample = iirSampleS;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleT = (iirSampleT * altAmount) + (inputSample * iirAmount); inputSample = iirSampleT;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleU = (iirSampleU * altAmount) + (inputSample * iirAmount); inputSample = iirSampleU;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleV = (iirSampleV * altAmount) + (inputSample * iirAmount); inputSample = iirSampleV;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleW = (iirSampleW * altAmount) + (inputSample * iirAmount); inputSample = iirSampleW;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleX = (iirSampleX * altAmount) + (inputSample * iirAmount); inputSample = iirSampleX;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleY = (iirSampleY * altAmount) + (inputSample * iirAmount); inputSample = iirSampleY;
		inputSample *= gain; gain = ((gain-1)*0.75f)+1;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		iirSampleZ = (iirSampleZ * altAmount) + (inputSample * iirAmount); inputSample = iirSampleZ;
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		
		
		
		
		*destP = inputSample;		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	iirSampleG = 0.0;
	iirSampleH = 0.0;
	iirSampleI = 0.0;
	iirSampleJ = 0.0;
	iirSampleK = 0.0;
	iirSampleL = 0.0;
	iirSampleM = 0.0;
	iirSampleN = 0.0;
	iirSampleO = 0.0;
	iirSampleP = 0.0;
	iirSampleQ = 0.0;
	iirSampleR = 0.0;
	iirSampleS = 0.0;
	iirSampleT = 0.0;
	iirSampleU = 0.0;
	iirSampleV = 0.0;
	iirSampleW = 0.0;
	iirSampleX = 0.0;
	iirSampleY = 0.0;
	iirSampleZ = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
