#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Monitoring"
#define AIRWINDOWS_DESCRIPTION "Your one-stop shop for final 2-buss work!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','o','g' )
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
static const int kNJAD = 0;
static const int kNJCD = 1;
static const int kPEAK = 2;
static const int kSLEW = 3;
static const int kSUBS = 4;
static const int kMONO = 5;
static const int kSIDE = 6;
static const int kVINYL = 7;
static const int kAURAT = 8;
static const int kMONORAT = 9;
static const int kMONOLAT = 10;
static const int kPHONE = 11;
static const int kCANSA = 12;
static const int kCANSB = 13;
static const int kCANSC = 14;
static const int kCANSD = 15;
static const int kTRICK = 16;
static const int kDefaultValue_ParamOne = kNJAD;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, };
static char const * const enumStrings0[] = { "Out24", "Out16", "Peaks", "Slew", "Subs", "Mono", "Side", "Vinyl", "Aurat", "MonoRat", "MonoLat", "Phone", "Cans A", "Cans B", "Cans C", "Cans D", "Voice Trick", };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Monitor", .min = 0, .max = 16, .def = 0, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float bynL[13], bynR[13];
	float noiseShapingL, noiseShapingR;
	//NJAD
	Float32 aL[1503], bL[1503], cL[1503], dL[1503];
	Float32 aR[1503], bR[1503], cR[1503], dR[1503];
	int ax, bx, cx, dx;
	//PeaksOnly
	Float32 lastSampleL, lastSampleR;
	//SlewOnly
	Float32 iirSampleAL, iirSampleBL, iirSampleCL, iirSampleDL, iirSampleEL, iirSampleFL, iirSampleGL;
	Float32 iirSampleHL, iirSampleIL, iirSampleJL, iirSampleKL, iirSampleLL, iirSampleML, iirSampleNL, iirSampleOL, iirSamplePL;
	Float32 iirSampleQL, iirSampleRL, iirSampleSL;
	Float32 iirSampleTL, iirSampleUL, iirSampleVL;
	Float32 iirSampleWL, iirSampleXL, iirSampleYL, iirSampleZL;
	
	Float32 iirSampleAR, iirSampleBR, iirSampleCR, iirSampleDR, iirSampleER, iirSampleFR, iirSampleGR;
	Float32 iirSampleHR, iirSampleIR, iirSampleJR, iirSampleKR, iirSampleLR, iirSampleMR, iirSampleNR, iirSampleOR, iirSamplePR;
	Float32 iirSampleQR, iirSampleRR, iirSampleSR;
	Float32 iirSampleTR, iirSampleUR, iirSampleVR;
	Float32 iirSampleWR, iirSampleXR, iirSampleYR, iirSampleZR; // o/`
	//SubsOnly
	float biquad[11];
	//Bandpasses

	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();	
	int processing = (int) GetParameter( kParam_One );
	int am = (int)149.0f * overallscale;
	int bm = (int)179.0f * overallscale;
	int cm = (int)191.0f * overallscale;
	int dm = (int)223.0f * overallscale; //these are 'good' primes, spacing out the allpasses
	int allpasstemp;
	//for PeaksOnly
	biquad[0] = 0.0375f/overallscale; biquad[1] = 0.1575f; //define as AURAT, MONORAT, MONOLAT unless overridden
	if (processing == kVINYL) {biquad[0] = 0.0385f/overallscale; biquad[1] = 0.0825f;}
	if (processing == kPHONE) {biquad[0] = 0.1245f/overallscale; biquad[1] = 0.46f;}	
	float K = tan(M_PI * biquad[0]);
	float norm = 1.0f / (1.0f + K / biquad[1] + K * K);
	biquad[2] = K / biquad[1] * norm;
	biquad[4] = -biquad[2]; //for bandpass, ignore [3] = 0.0f
	biquad[5] = 2.0f * (K * K - 1.0f) * norm;
	biquad[6] = (1.0f - K / biquad[1] + K * K) * norm;
	//for Bandpasses
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		switch (processing)
		{
			case 0:
			case 1:
				break;
			case 2:				
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f; inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f; inputSampleR = asin(inputSampleR);
				//amplitude aspect
				allpasstemp = ax - 1; if (allpasstemp < 0 || allpasstemp > am) allpasstemp = am;
				inputSampleL -= aL[allpasstemp]*0.5f; aL[ax] = inputSampleL; inputSampleL *= 0.5f;
				inputSampleR -= aR[allpasstemp]*0.5f; aR[ax] = inputSampleR; inputSampleR *= 0.5f;
				ax--; if (ax < 0 || ax > am) {ax = am;}
				inputSampleL += (aL[ax]);
				inputSampleR += (aR[ax]);
				//a single Midiverb-style allpass
				
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f; inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f; inputSampleR = asin(inputSampleR);
				//amplitude aspect
				
				allpasstemp = bx - 1; if (allpasstemp < 0 || allpasstemp > bm) allpasstemp = bm;
				inputSampleL -= bL[allpasstemp]*0.5f; bL[bx] = inputSampleL; inputSampleL *= 0.5f;
				inputSampleR -= bR[allpasstemp]*0.5f; bR[bx] = inputSampleR; inputSampleR *= 0.5f;
				bx--; if (bx < 0 || bx > bm) {bx = bm;}
				inputSampleL += (bL[bx]);
				inputSampleR += (bR[bx]);
				//a single Midiverb-style allpass
				
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f; inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f; inputSampleR = asin(inputSampleR);
				//amplitude aspect
				
				allpasstemp = cx - 1; if (allpasstemp < 0 || allpasstemp > cm) allpasstemp = cm;
				inputSampleL -= cL[allpasstemp]*0.5f; cL[cx] = inputSampleL; inputSampleL *= 0.5f;
				inputSampleR -= cR[allpasstemp]*0.5f; cR[cx] = inputSampleR; inputSampleR *= 0.5f;
				cx--; if (cx < 0 || cx > cm) {cx = cm;}
				inputSampleL += (cL[cx]);
				inputSampleR += (cR[cx]);
				//a single Midiverb-style allpass
				
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f; inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f; inputSampleR = asin(inputSampleR);
				//amplitude aspect
				
				allpasstemp = dx - 1; if (allpasstemp < 0 || allpasstemp > dm) allpasstemp = dm;
				inputSampleL -= dL[allpasstemp]*0.5f; dL[dx] = inputSampleL; inputSampleL *= 0.5f;
				inputSampleR -= dR[allpasstemp]*0.5f; dR[dx] = inputSampleR; inputSampleR *= 0.5f;
				dx--; if (dx < 0 || dx > dm) {dx = dm;}
				inputSampleL += (dL[dx]);
				inputSampleR += (dR[dx]);
				//a single Midiverb-style allpass
				
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f; inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f; inputSampleR = asin(inputSampleR);
				//amplitude aspect
				
				inputSampleL *= 0.63679f; inputSampleR *= 0.63679f; //scale it to 0dB output at full blast
				//PeaksOnly
				break;
			case 3:
				Float32 trim;
				trim = 2.302585092994045684017991f; //natural logarithm of 10
				float slewSample; slewSample = (inputSampleL - lastSampleL)*trim;
				lastSampleL = inputSampleL;
				if (slewSample > 1.0f) slewSample = 1.0f; if (slewSample < -1.0f) slewSample = -1.0f;
				inputSampleL = slewSample;
				slewSample = (inputSampleR - lastSampleR)*trim;
				lastSampleR = inputSampleR;
				if (slewSample > 1.0f) slewSample = 1.0f; if (slewSample < -1.0f) slewSample = -1.0f;
				inputSampleR = slewSample;
				//SlewOnly
				break;
			case 4:
				Float32 iirAmount; iirAmount = (2250/44100.0f) / overallscale;
				Float32 gain; gain = 1.42f;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				
				iirSampleAL = (iirSampleAL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleAL;
				iirSampleAR = (iirSampleAR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleAR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleBL = (iirSampleBL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleBL;
				iirSampleBR = (iirSampleBR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleBR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleCL = (iirSampleCL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleCL;
				iirSampleCR = (iirSampleCR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleCR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleDL = (iirSampleDL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleDL;
				iirSampleDR = (iirSampleDR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleDR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleEL = (iirSampleEL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleEL;
				iirSampleER = (iirSampleER * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleER;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleFL = (iirSampleFL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleFL;
				iirSampleFR = (iirSampleFR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleFR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleGL = (iirSampleGL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleGL;
				iirSampleGR = (iirSampleGR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleGR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleHL = (iirSampleHL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleHL;
				iirSampleHR = (iirSampleHR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleHR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleIL = (iirSampleIL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleIL;
				iirSampleIR = (iirSampleIR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleIR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleJL = (iirSampleJL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleJL;
				iirSampleJR = (iirSampleJR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleJR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleKL = (iirSampleKL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleKL;
				iirSampleKR = (iirSampleKR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleKR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleLL = (iirSampleLL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleLL;
				iirSampleLR = (iirSampleLR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleLR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleML = (iirSampleML * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleML;
				iirSampleMR = (iirSampleMR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleMR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleNL = (iirSampleNL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleNL;
				iirSampleNR = (iirSampleNR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleNR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleOL = (iirSampleOL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleOL;
				iirSampleOR = (iirSampleOR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleOR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSamplePL = (iirSamplePL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSamplePL;
				iirSamplePR = (iirSamplePR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSamplePR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleQL = (iirSampleQL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleQL;
				iirSampleQR = (iirSampleQR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleQR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleRL = (iirSampleRL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleRL;
				iirSampleRR = (iirSampleRR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleRR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleSL = (iirSampleSL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleSL;
				iirSampleSR = (iirSampleSR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleSR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleTL = (iirSampleTL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleTL;
				iirSampleTR = (iirSampleTR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleTR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleUL = (iirSampleUL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleUL;
				iirSampleUR = (iirSampleUR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleUR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleVL = (iirSampleVL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleVL;
				iirSampleVR = (iirSampleVR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleVR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleWL = (iirSampleWL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleWL;
				iirSampleWR = (iirSampleWR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleWR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleXL = (iirSampleXL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleXL;
				iirSampleXR = (iirSampleXR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleXR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleYL = (iirSampleYL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleYL;
				iirSampleYR = (iirSampleYR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleYR;
				inputSampleL *= gain; inputSampleR *= gain; gain = ((gain-1)*0.75f)+1;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				
				iirSampleZL = (iirSampleZL * (1.0f-iirAmount)) + (inputSampleL * iirAmount); inputSampleL = iirSampleZL;
				iirSampleZR = (iirSampleZR * (1.0f-iirAmount)) + (inputSampleR * iirAmount); inputSampleR = iirSampleZR;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;				
				//SubsOnly
				break;
			case 5:
			case 6:
				float mid; mid = inputSampleL + inputSampleR;
				float side; side = inputSampleL - inputSampleR;
				if (processing < 6) side = 0.0f;
				else mid = 0.0f; //mono monitoring, or side-only monitoring
				inputSampleL = (mid+side)/2.0f;
				inputSampleR = (mid-side)/2.0f; 
				break;
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				//Bandpass: changes in EQ are up in the variable defining, not here
				//7 Vinyl, 8 9 10 Aurat, 11 Phone
				
				if (processing == 9) {inputSampleR = (inputSampleL + inputSampleR)*0.5f;inputSampleL = 0.0f;}
				if (processing == 10) {inputSampleL = (inputSampleL + inputSampleR)*0.5f;inputSampleR = 0.0f;}
				if (processing == 11) {float M; M = (inputSampleL + inputSampleR)*0.5f; inputSampleL = M;inputSampleR = M;}
				
				inputSampleL = sin(inputSampleL); inputSampleR = sin(inputSampleR);
				//encode Console5: good cleanness
				
				float tempSampleL; tempSampleL = (inputSampleL * biquad[2]) + biquad[7];
				biquad[7] = (-tempSampleL * biquad[5]) + biquad[8];
				biquad[8] = (inputSampleL * biquad[4]) - (tempSampleL * biquad[6]);
				inputSampleL = tempSampleL; //like mono AU, 7 and 8 store L channel
				
				float tempSampleR; tempSampleR = (inputSampleR * biquad[2]) + biquad[9];
				biquad[9] = (-tempSampleR * biquad[5]) + biquad[10];
				biquad[10] = (inputSampleR * biquad[4]) - (tempSampleR * biquad[6]);
				inputSampleR = tempSampleR; //note: 9 and 10 store the R channel
				
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				//without this, you can get a NaN condition where it spits out DC offset at full blast!
				inputSampleL = asin(inputSampleL); inputSampleR = asin(inputSampleR);
				//amplitude aspect
				break;
			case 12:
			case 13:
			case 14:
			case 15:
				if (processing == 12) {inputSampleL *= 0.855f; inputSampleR *= 0.855f;}
				if (processing == 13) {inputSampleL *= 0.748f; inputSampleR *= 0.748f;}
				if (processing == 14) {inputSampleL *= 0.713f; inputSampleR *= 0.713f;}
				if (processing == 15) {inputSampleL *= 0.680f; inputSampleR *= 0.680f;}
				//we do a volume compensation immediately to gain stage stuff cleanly
				inputSampleL = sin(inputSampleL);
				inputSampleR = sin(inputSampleR);
				float drySampleL; drySampleL = inputSampleL;
				float drySampleR; drySampleR = inputSampleR; //everything runs 'inside' Console
				float bass; bass = (processing * processing * 0.00001f) / overallscale;
				//we are using the iir filters from out of SubsOnly
				
				mid = inputSampleL + inputSampleR; side = inputSampleL - inputSampleR;
				iirSampleAL = (iirSampleAL * (1.0f - (bass*0.618f))) + (side * bass * 0.618f); side = side - iirSampleAL;
				inputSampleL = (mid+side)/2.0f; inputSampleR = (mid-side)/2.0f;
				//bass narrowing filter				
								
				allpasstemp = ax - 1; if (allpasstemp < 0 || allpasstemp > am) allpasstemp = am;
				inputSampleL -= aL[allpasstemp]*0.5f; aL[ax] = inputSampleL; inputSampleL *= 0.5f;
				inputSampleR -= aR[allpasstemp]*0.5f; aR[ax] = inputSampleR; inputSampleR *= 0.5f;
				
				ax--; if (ax < 0 || ax > am) {ax = am;}
				inputSampleL += (aL[ax])*0.5f; inputSampleR += (aR[ax])*0.5f;
				if (ax == am) {inputSampleL += (aL[0])*0.5f; inputSampleR += (aR[0])*0.5f;}
				else {inputSampleL += (aL[ax+1])*0.5f; inputSampleR += (aR[ax+1])*0.5f;}
				//a darkened Midiverb-style allpass
				
				if (processing == 12) {inputSampleL *= 0.125f; inputSampleR *= 0.125f;}
				if (processing == 13) {inputSampleL *= 0.25f; inputSampleR *= 0.25f;}
				if (processing == 14) {inputSampleL *= 0.30f; inputSampleR *= 0.30f;}
				if (processing == 15) {inputSampleL *= 0.35f; inputSampleR *= 0.35f;}
				//Cans A suppresses the crossfeed more, Cans B makes it louder
								
				drySampleL += inputSampleR;
				drySampleR += inputSampleL; //the crossfeed
				
				allpasstemp = dx - 1; if (allpasstemp < 0 || allpasstemp > dm) allpasstemp = dm;
				inputSampleL -= dL[allpasstemp]*0.5f; dL[dx] = inputSampleL; inputSampleL *= 0.5f;
				inputSampleR -= dR[allpasstemp]*0.5f; dR[dx] = inputSampleR; inputSampleR *= 0.5f;
				
				dx--; if (dx < 0 || dx > dm) {dx = dm;}
				inputSampleL += (dL[dx])*0.5f; inputSampleR += (dR[dx])*0.5f;
				if (dx == dm) {inputSampleL += (dL[0])*0.5f; inputSampleR += (dR[0])*0.5f;}
				else {inputSampleL += (dL[dx+1])*0.5f; inputSampleR += (dR[dx+1])*0.5f;}
				//a darkened Midiverb-style allpass, which is stretching the previous one even more
				
				inputSampleL *= 0.25f; inputSampleR *= 0.25f;
				//for all versions of Cans the second level of bloom is this far down
				//and, remains on the opposite speaker rather than crossing again to the original side
				
				drySampleL += inputSampleR;
				drySampleR += inputSampleL; //add the crossfeed and very faint extra verbyness
				
				inputSampleL = drySampleL;
				inputSampleR = drySampleR; //and output our can-opened headphone feed
				
				mid = inputSampleL + inputSampleR; side = inputSampleL - inputSampleR;
				iirSampleAR = (iirSampleAR * (1.0f - bass)) + (side * bass); side = side - iirSampleAR;
				inputSampleL = (mid+side)/2.0f; inputSampleR = (mid-side)/2.0f;
				//bass narrowing filter
				
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f; inputSampleL = asin(inputSampleL);
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f; inputSampleR = asin(inputSampleR);
				//ConsoleBuss processing
				break;
			case 16:
				float inputSample = (inputSampleL + inputSampleR) * 0.5f;
				inputSampleL = -inputSample;
				inputSampleR = inputSample;
				break;
		}
		
		
		//begin Not Just Another Dither
		if (processing == 1) {
			inputSampleL = inputSampleL * 32768.0f; //or 16 bit option
			inputSampleR = inputSampleR * 32768.0f; //or 16 bit option
		} else {
			inputSampleL = inputSampleL * 8388608.0f; //for literally everything else
			inputSampleR = inputSampleR * 8388608.0f; //we will apply the 24 bit NJAD
		} //on the not unreasonable assumption that we are very likely playing back on 24 bit DAC
		//if we're not, then all we did was apply a Benford Realness function at 24 bits down.
		
		bool cutbinsL; cutbinsL = false;
		bool cutbinsR; cutbinsR = false;
		float drySampleL; drySampleL = inputSampleL;
		float drySampleR; drySampleR = inputSampleR;
		inputSampleL -= noiseShapingL;
		inputSampleR -= noiseShapingR;
		//NJAD L
		float benfordize; benfordize = floor(inputSampleL);
		while (benfordize >= 1.0f) benfordize /= 10;
		while (benfordize < 1.0f && benfordize > 0.0000001f) benfordize *= 10;
		int hotbinA; hotbinA = floor(benfordize);
		//hotbin becomes the Benford bin value for this number floored
		float totalA; totalA = 0;
		if ((hotbinA > 0) && (hotbinA < 10))
		{
			bynL[hotbinA] += 1; if (bynL[hotbinA] > 982) cutbinsL = true;
			totalA += (301-bynL[1]); totalA += (176-bynL[2]); totalA += (125-bynL[3]);
			totalA += (97-bynL[4]); totalA += (79-bynL[5]); totalA += (67-bynL[6]);
			totalA += (58-bynL[7]); totalA += (51-bynL[8]); totalA += (46-bynL[9]); bynL[hotbinA] -= 1;
		} else hotbinA = 10;
		//produce total number- smaller is closer to Benford real
		benfordize = ceil(inputSampleL);
		while (benfordize >= 1.0f) benfordize /= 10;
		while (benfordize < 1.0f && benfordize > 0.0000001f) benfordize *= 10;
		int hotbinB; hotbinB = floor(benfordize);
		//hotbin becomes the Benford bin value for this number ceiled
		float totalB; totalB = 0;
		if ((hotbinB > 0) && (hotbinB < 10))
		{
			bynL[hotbinB] += 1; if (bynL[hotbinB] > 982) cutbinsL = true;
			totalB += (301-bynL[1]); totalB += (176-bynL[2]); totalB += (125-bynL[3]);
			totalB += (97-bynL[4]); totalB += (79-bynL[5]); totalB += (67-bynL[6]);
			totalB += (58-bynL[7]); totalB += (51-bynL[8]); totalB += (46-bynL[9]); bynL[hotbinB] -= 1;
		} else hotbinB = 10;
		//produce total number- smaller is closer to Benford real
		float outputSample;
		if (totalA < totalB) {bynL[hotbinA] += 1; outputSample = floor(inputSampleL);}
		else {bynL[hotbinB] += 1; outputSample = floor(inputSampleL+1);}
		//assign the relevant one to the delay line
		//and floor/ceil signal accordingly
		if (cutbinsL) {
			bynL[1] *= 0.99f; bynL[2] *= 0.99f; bynL[3] *= 0.99f; bynL[4] *= 0.99f; bynL[5] *= 0.99f; 
			bynL[6] *= 0.99f; bynL[7] *= 0.99f; bynL[8] *= 0.99f; bynL[9] *= 0.99f; bynL[10] *= 0.99f; 
		}
		noiseShapingL += outputSample - drySampleL;			
		if (noiseShapingL > fabs(inputSampleL)) noiseShapingL = fabs(inputSampleL);
		if (noiseShapingL < -fabs(inputSampleL)) noiseShapingL = -fabs(inputSampleL);
		if (processing == 1) inputSampleL = outputSample / 32768.0f;
		else inputSampleL = outputSample / 8388608.0f;
		if (inputSampleL > 1.0f) inputSampleL = 1.0f;
		if (inputSampleL < -1.0f) inputSampleL = -1.0f;
		//finished NJAD L
		
		//NJAD R
		benfordize = floor(inputSampleR);
		while (benfordize >= 1.0f) benfordize /= 10;
		while (benfordize < 1.0f && benfordize > 0.0000001f) benfordize *= 10;		
		hotbinA = floor(benfordize);
		//hotbin becomes the Benford bin value for this number floored
		totalA = 0;
		if ((hotbinA > 0) && (hotbinA < 10))
		{
			bynR[hotbinA] += 1; if (bynR[hotbinA] > 982) cutbinsR = true;
			totalA += (301-bynR[1]); totalA += (176-bynR[2]); totalA += (125-bynR[3]);
			totalA += (97-bynR[4]); totalA += (79-bynR[5]); totalA += (67-bynR[6]);
			totalA += (58-bynR[7]); totalA += (51-bynR[8]); totalA += (46-bynR[9]); bynR[hotbinA] -= 1;
		} else hotbinA = 10;
		//produce total number- smaller is closer to Benford real
		benfordize = ceil(inputSampleR);
		while (benfordize >= 1.0f) benfordize /= 10;
		while (benfordize < 1.0f && benfordize > 0.0000001f) benfordize *= 10;		
		hotbinB = floor(benfordize);
		//hotbin becomes the Benford bin value for this number ceiled
		totalB = 0;
		if ((hotbinB > 0) && (hotbinB < 10))
		{
			bynR[hotbinB] += 1; if (bynR[hotbinB] > 982) cutbinsR = true;
			totalB += (301-bynR[1]); totalB += (176-bynR[2]); totalB += (125-bynR[3]);
			totalB += (97-bynR[4]); totalB += (79-bynR[5]); totalB += (67-bynR[6]);
			totalB += (58-bynR[7]); totalB += (51-bynR[8]); totalB += (46-bynR[9]); bynR[hotbinB] -= 1;
		} else hotbinB = 10;
		//produce total number- smaller is closer to Benford real
		if (totalA < totalB) {bynR[hotbinA] += 1; outputSample = floor(inputSampleR);}
		else {bynR[hotbinB] += 1; outputSample = floor(inputSampleR+1);}
		//assign the relevant one to the delay line
		//and floor/ceil signal accordingly
		if (cutbinsR) {
			bynR[1] *= 0.99f; bynR[2] *= 0.99f; bynR[3] *= 0.99f; bynR[4] *= 0.99f; bynR[5] *= 0.99f; 
			bynR[6] *= 0.99f; bynR[7] *= 0.99f; bynR[8] *= 0.99f; bynR[9] *= 0.99f; bynR[10] *= 0.99f; 
		}
		noiseShapingR += outputSample - drySampleR;			
		if (noiseShapingR > fabs(inputSampleR)) noiseShapingR = fabs(inputSampleR);
		if (noiseShapingR < -fabs(inputSampleR)) noiseShapingR = -fabs(inputSampleR);
		if (processing == 1) inputSampleR = outputSample / 32768.0f;
		else inputSampleR = outputSample / 8388608.0f;
		if (inputSampleR > 1.0f) inputSampleR = 1.0f;
		if (inputSampleR < -1.0f) inputSampleR = -1.0f;
		//finished NJAD R		
		
		//does not use 32 bit stereo floating point dither
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//direct stereo out
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	bynL[0] = 1000.0;
	bynL[1] = 301.0;
	bynL[2] = 176.0;
	bynL[3] = 125.0;
	bynL[4] = 97.0;
	bynL[5] = 79.0;
	bynL[6] = 67.0;
	bynL[7] = 58.0;
	bynL[8] = 51.0;
	bynL[9] = 46.0;
	bynL[10] = 1000.0;
	noiseShapingL = 0.0;
	bynR[0] = 1000.0;
	bynR[1] = 301.0;
	bynR[2] = 176.0;
	bynR[3] = 125.0;
	bynR[4] = 97.0;
	bynR[5] = 79.0;
	bynR[6] = 67.0;
	bynR[7] = 58.0;
	bynR[8] = 51.0;
	bynR[9] = 46.0;
	bynR[10] = 1000.0;
	noiseShapingR = 0.0;
	//end NJAD
	for(int count = 0; count < 1502; count++) {
		aL[count] = 0.0; bL[count] = 0.0; cL[count] = 0.0; dL[count] = 0.0;
		aR[count] = 0.0; bR[count] = 0.0; cR[count] = 0.0; dR[count] = 0.0;
	}
	ax = 1; bx = 1; cx = 1; dx = 1;
	//PeaksOnly
	lastSampleL = 0.0; lastSampleR = 0.0;
	//SlewOnly
	iirSampleAL = 0.0; iirSampleBL = 0.0; iirSampleCL = 0.0; iirSampleDL = 0.0; iirSampleEL = 0.0; iirSampleFL = 0.0; iirSampleGL = 0.0;
	iirSampleHL = 0.0; iirSampleIL = 0.0; iirSampleJL = 0.0; iirSampleKL = 0.0; iirSampleLL = 0.0; iirSampleML = 0.0; iirSampleNL = 0.0; iirSampleOL = 0.0; iirSamplePL = 0.0;
	iirSampleQL = 0.0; iirSampleRL = 0.0; iirSampleSL = 0.0;
	iirSampleTL = 0.0; iirSampleUL = 0.0; iirSampleVL = 0.0;
	iirSampleWL = 0.0; iirSampleXL = 0.0; iirSampleYL = 0.0; iirSampleZL = 0.0;

	iirSampleAR = 0.0; iirSampleBR = 0.0; iirSampleCR = 0.0; iirSampleDR = 0.0; iirSampleER = 0.0; iirSampleFR = 0.0; iirSampleGR = 0.0;
	iirSampleHR = 0.0; iirSampleIR = 0.0; iirSampleJR = 0.0; iirSampleKR = 0.0; iirSampleLR = 0.0; iirSampleMR = 0.0; iirSampleNR = 0.0; iirSampleOR = 0.0; iirSamplePR = 0.0;
	iirSampleQR = 0.0; iirSampleRR = 0.0; iirSampleSR = 0.0;
	iirSampleTR = 0.0; iirSampleUR = 0.0; iirSampleVR = 0.0;
	iirSampleWR = 0.0; iirSampleXR = 0.0; iirSampleYR = 0.0; iirSampleZR = 0.0; // o/`	
	//SubsOnly
	for (int x = 0; x < 11; x++) {biquad[x] = 0.0;}
	//Bandpasses
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
