#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Monitoring2"
#define AIRWINDOWS_DESCRIPTION "Monitoring, but with Dark as the wordlength reducer."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','o','2' )
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
static const int kDKAD = 0;
static const int kDKCD = 1;
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
static const int kDefaultValue_ParamOne = kDKAD;
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
	};
	//Bandpasses
	

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
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float biquad[fix_total];
	float darkSampleL[100];
	float darkSampleR[100];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	int depth = (int)(17.0f*overallscale);
	if (depth < 3) depth = 3;
	if (depth > 98) depth = 98; //for Dark
	
	int processing = (int) GetParameter( kParam_One );
	int am = (int)149.0f * overallscale;
	int bm = (int)179.0f * overallscale;
	int cm = (int)191.0f * overallscale;
	int dm = (int)223.0f * overallscale; //these are 'good' primes, spacing out the allpasses
	int allpasstemp;
	//for PeaksOnly
	dram->biquad[fix_freq] = 0.0375f/overallscale; dram->biquad[fix_reso] = 0.1575f; //define as AURAT, MONORAT, MONOLAT unless overridden
	if (processing == kVINYL) {dram->biquad[fix_freq] = 0.0385f/overallscale; dram->biquad[fix_reso] = 0.0825f;}
	if (processing == kPHONE) {dram->biquad[fix_freq] = 0.1245f/overallscale; dram->biquad[fix_reso] = 0.46f;}	
	float K = tan(M_PI * dram->biquad[fix_freq]);
	float norm = 1.0f / (1.0f + K / dram->biquad[fix_reso] + K * K);
	dram->biquad[fix_a0] = K / dram->biquad[fix_reso] * norm;
	dram->biquad[fix_a2] = -dram->biquad[fix_a0]; //for bandpass, ignore [fix_a1] = 0.0f
	dram->biquad[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->biquad[fix_b2] = (1.0f - K / dram->biquad[fix_reso] + K * K) * norm;
	//for Bandpasses
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		//we need to make our dither run up here, there's no spot on the end to do it
		
		switch (processing)
		{
			case kDKAD:
			case kDKCD:
				break;
			case kPEAK:
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
			case kSLEW:
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
			case kSUBS:
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
			case kMONO:
			case kSIDE:
				float mid; mid = inputSampleL + inputSampleR;
				float side; side = inputSampleL - inputSampleR;
				if (processing < kSIDE) side = 0.0f;
				else mid = 0.0f; //mono monitoring, or side-only monitoring
				inputSampleL = (mid+side)/2.0f;
				inputSampleR = (mid-side)/2.0f; 
				break;
			case kVINYL:
			case kAURAT:
			case kMONORAT:
			case kMONOLAT:
			case kPHONE:
				//Bandpass: changes in EQ are up in the variable defining, not here
				//7 Vinyl, 8 9 10 Aurat, 11 Phone
				
				if (processing == kMONORAT) {inputSampleR = (inputSampleL + inputSampleR)*0.5f;inputSampleL = 0.0f;}
				if (processing == kMONOLAT) {inputSampleL = (inputSampleL + inputSampleR)*0.5f;inputSampleR = 0.0f;}
				if (processing == kPHONE) {float M; M = (inputSampleL + inputSampleR)*0.5f; inputSampleL = M;inputSampleR = M;}
				
				inputSampleL = sin(inputSampleL); inputSampleR = sin(inputSampleR);
				//encode Console5: good cleanness
				
				float tempSampleL; tempSampleL = (inputSampleL * dram->biquad[fix_a0]) + dram->biquad[fix_sL1];
				dram->biquad[fix_sL1] = (-tempSampleL * dram->biquad[fix_b1]) + dram->biquad[fix_sL2];
				dram->biquad[fix_sL2] = (inputSampleL * dram->biquad[fix_a2]) - (tempSampleL * dram->biquad[fix_b2]);
				inputSampleL = tempSampleL; //like mono AU, 7 and 8 store L channel
				
				float tempSampleR; tempSampleR = (inputSampleR * dram->biquad[fix_a0]) + dram->biquad[fix_sR1];
				dram->biquad[fix_sR1] = (-tempSampleR * dram->biquad[fix_b1]) + dram->biquad[fix_sR2];
				dram->biquad[fix_sR2] = (inputSampleR * dram->biquad[fix_a2]) - (tempSampleR * dram->biquad[fix_b2]);
				inputSampleR = tempSampleR; //note: 9 and 10 store the R channel
				
				if (inputSampleL > 1.0f) inputSampleL = 1.0f; if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				if (inputSampleR > 1.0f) inputSampleR = 1.0f; if (inputSampleR < -1.0f) inputSampleR = -1.0f;
				//without this, you can get a NaN condition where it spits out DC offset at full blast!
				inputSampleL = asin(inputSampleL); inputSampleR = asin(inputSampleR);
				//amplitude aspect
				break;
			case kCANSA:
			case kCANSB:
			case kCANSC:
			case kCANSD:
				if (processing == kCANSA) {inputSampleL *= 0.855f; inputSampleR *= 0.855f;}
				if (processing == kCANSB) {inputSampleL *= 0.748f; inputSampleR *= 0.748f;}
				if (processing == kCANSC) {inputSampleL *= 0.713f; inputSampleR *= 0.713f;}
				if (processing == kCANSD) {inputSampleL *= 0.680f; inputSampleR *= 0.680f;}
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
				
				if (processing == kCANSA) {inputSampleL *= 0.125f; inputSampleR *= 0.125f;}
				if (processing == kCANSB) {inputSampleL *= 0.25f; inputSampleR *= 0.25f;}
				if (processing == kCANSC) {inputSampleL *= 0.30f; inputSampleR *= 0.30f;}
				if (processing == kCANSD) {inputSampleL *= 0.35f; inputSampleR *= 0.35f;}
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
			case kTRICK:
				float inputSample = (inputSampleL + inputSampleR) * 0.5f;
				inputSampleL = -inputSample;
				inputSampleR = inputSample;
				break;
		}
		
		//begin Dark		
		if (processing == kDKCD) {
			inputSampleL *= 32768.0f; //or 16 bit option
			inputSampleR *= 32768.0f; //or 16 bit option
		} else {
			inputSampleL *= 8388608.0f; //for literally everything else
			inputSampleR *= 8388608.0f; //we will apply the 24 bit Dark
		} //on the not unreasonable assumption that we are very likely playing back on 24 bit DAC
		
		//We are doing it first Left, then Right, because the loops may run faster if
		//they aren't too jammed full of variables. This means re-running code.
		
		//begin left
		int quantA = floor(inputSampleL);
		int quantB = floor(inputSampleL+1.0f);
		//to do this style of dither, we quantize in either direction and then
		//do a reconstruction of what the result will be for each choice.
		//We then evaluate which one we like, and keep a history of what we previously had
		
		float expectedSlew = 0;
		for(int x = 0; x < depth; x++) {
			expectedSlew += (dram->darkSampleL[x+1] - dram->darkSampleL[x]);
		}
		expectedSlew /= depth; //we have an average of all recent slews
		//we are doing that to voice the thing down into the upper mids a bit
		//it mustn't just soften the brightest treble, it must smooth high mids too
		
		float testA = fabs((dram->darkSampleL[0] - quantA) - expectedSlew);
		float testB = fabs((dram->darkSampleL[0] - quantB) - expectedSlew);
		
		if (testA < testB) inputSampleL = quantA;
		else inputSampleL = quantB;
		//select whichever one departs LEAST from the vector of averaged
		//reconstructed previous final samples. This will force a kind of dithering
		//as it'll make the output end up as smooth as possible
		
		for(int x = depth; x >=0; x--) {
			dram->darkSampleL[x+1] = dram->darkSampleL[x];
		}
		dram->darkSampleL[0] = inputSampleL;
		//end Dark left
		
		//begin right
		quantA = floor(inputSampleR);
		quantB = floor(inputSampleR+1.0f);
		//to do this style of dither, we quantize in either direction and then
		//do a reconstruction of what the result will be for each choice.
		//We then evaluate which one we like, and keep a history of what we previously had
		
		expectedSlew = 0;
		for(int x = 0; x < depth; x++) {
			expectedSlew += (dram->darkSampleR[x+1] - dram->darkSampleR[x]);
		}
		expectedSlew /= depth; //we have an average of all recent slews
		//we are doing that to voice the thing down into the upper mids a bit
		//it mustn't just soften the brightest treble, it must smooth high mids too
		
		testA = fabs((dram->darkSampleR[0] - quantA) - expectedSlew);
		testB = fabs((dram->darkSampleR[0] - quantB) - expectedSlew);
		
		if (testA < testB) inputSampleR = quantA;
		else inputSampleR = quantB;
		//select whichever one departs LEAST from the vector of averaged
		//reconstructed previous final samples. This will force a kind of dithering
		//as it'll make the output end up as smooth as possible
		
		for(int x = depth; x >=0; x--) {
			dram->darkSampleR[x+1] = dram->darkSampleR[x];
		}
		dram->darkSampleR[0] = inputSampleR;
		//end Dark right
		
		if (processing == kDKCD) {
			inputSampleL /= 32768.0f;
			inputSampleR /= 32768.0f;
		} else {
			inputSampleL /= 8388608.0f;
			inputSampleR /= 8388608.0f;
		}
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
	for(int count = 0; count < 99; count++) {
		dram->darkSampleL[count] = 0;
		dram->darkSampleR[count] = 0;
	}
	
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
	for (int x = 0; x < fix_total; x++) {dram->biquad[x] = 0.0;}
	//Bandpasses
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
