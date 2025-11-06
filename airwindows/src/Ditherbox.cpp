#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Ditherbox"
#define AIRWINDOWS_DESCRIPTION "A switchable selection of dithers in 16 and 24 bit, plus monitoring tools."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i','$' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One = 0,
	//Add your parameters here...
	kNumberOfParameters=1
};
static const int kTruncate = 1;
static const int kFlat = 2;
static const int kTPDF = 3;
static const int kPaul = 4;
static const int kDoublePaul = 5;
static const int kTape = 6;
static const int kQuadratic = 7;
static const int kTenNines = 8;
static const int kContingent = 9;
static const int kNaturalize = 10;
static const int kNJAD = 11;
static const int kTruncateHR = 12;
static const int kFlatHR = 13;
static const int kTPDFHR = 14;
static const int kPaulHR = 15;
static const int kDoublePaulHR = 16;
static const int kTapeHR = 17;
static const int kQuadraticHR = 18;
static const int kTenNinesHR = 19;
static const int kContingentHR = 20;
static const int kNaturalizeHR = 21;
static const int kNJADHR = 22;
static const int kSlewOnly = 23;
static const int kSubsOnly = 24;
static const int kSilhouette = 25;
static const int kDefaultValue_ParamOne = 22;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static char const * const enumStrings0[] = { "", "16 Bit Truncation", "16 bit Flat Dither", "16 bit TPDF Dither", "16 bit Paul Dither", "16 bit DoublePaul Dither", "16 bit Tape Dither", "16 bit High Gloss Dither", "16 bit Vinyl Dither", "16 bit Spatialize Dither", "16 bit Naturalize Dither", "16 bit Not Just Another Dither", "24 bit Truncation", "24 bit Flat Dither", "24 bit TPDF Dither", "24 bit Paul Dither", "24 bit DoublePaul Dither", "24 bit Tape Dither", "24 bit High Gloss Dither", "24 bit Vinyl Dither", "24 bit Spatialize Dither", "24 bit Naturalize Dither", "24 bit Not Just Another Dither", "Slew Only Monitoring", "Subs Only Monitoring", "Noise Silhouette Monitoring", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Dither Type", .min = 1, .max = 25, .def = 22, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
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
		float contingentErr;
		float currentDither;
		float NSOdd;
		float NSEven;
		float prev;
		float ns[16];
		int Position;
		bool flip;
		float lastSample;
		float outSample;
		float iirSampleA;
		float iirSampleB;
		float iirSampleC;
		float iirSampleD;
		float iirSampleE;
		float iirSampleF;
		float iirSampleG;
		float iirSampleH;
		float iirSampleI;
		float iirSampleJ;
		float iirSampleK;
		float iirSampleL;
		float iirSampleM;
		float iirSampleN;
		float iirSampleO;
		float iirSampleP;
		float iirSampleQ;
		float iirSampleR;
		float iirSampleS;
		float iirSampleT;
		float iirSampleU;
		float iirSampleV;
		float iirSampleW;
		float iirSampleX;
		float iirSampleY;
		float iirSampleZ;
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
	float contingentRnd;
	float absSample;
	float contingent;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	float iirAmount = 2250/44100.0f;
	float gaintarget = 1.42f;
	float gain;
	iirAmount /= overallscale;
	float altAmount = 1.0f - iirAmount;
	float inputSample;
	float outputSample;
	float silhouette;
	float smoother;
	float bridgerectifier;
	float benfordize;
	int hotbinA;
	int hotbinB;
	float totalA;
	float totalB;
	float randyConstant = 1.61803398874989484820458683436563811772030917980576f;
	float omegaConstant = 0.56714329040978387299996866221035554975381578718651f;
	float expConstant = 0.06598803584531253707679018759684642493857704825279f;
	float trim = 2.302585092994045684017991f; //natural logarithm of 10
	int dtype = (int) GetParameter( kParam_One ); // +1 for Reaper bug workaround
	bool highRes = false;
	bool dithering = true;
	
	Float32 drySample; //should be the same as what the native DAW buss is

	if (dtype > 11){highRes = true; dtype -= 11;}
	
	if (dtype > 11){dithering = false; highRes = false;}
	//follow up by switching high res back off for the monitoring
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		sourceP += inNumChannels;
		
		if (dtype == 8) inputSample -= noiseShaping;
		
		if (dithering) inputSample *= 32768.0f;
		//denormalizing as way of controlling insane detail boosting
		if (highRes) inputSample *= 256.0f; //256 for 16/24 version
		
		switch (dtype)
		{
		case 1: 
		inputSample = floor(inputSample);
		//truncate
		break;
		
		case 2: 
		inputSample += (float(fpd)/UINT32_MAX);
		inputSample -= 0.5f;
		inputSample = floor(inputSample);
		//flat dither
		break;
		
			case 3:
				inputSample += (float(fpd)/UINT32_MAX);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
				inputSample += (float(fpd)/UINT32_MAX);
				inputSample -= 1.0f;
				inputSample = floor(inputSample);
				//TPDF dither
				break;
				
			case 4:
				currentDither = (float(fpd)/UINT32_MAX);
				inputSample += currentDither;
				inputSample -= lastSample;
				inputSample = floor(inputSample);
				lastSample = currentDither;
				//Paul dither
				break;
				
			case 5:
				ns[9] = ns[8]; ns[8] = ns[7]; ns[7] = ns[6]; ns[6] = ns[5];
				ns[5] = ns[4]; ns[4] = ns[3]; ns[3] = ns[2]; ns[2] = ns[1];
				ns[1] = ns[0]; ns[0] = (float(fpd)/UINT32_MAX);
				
				currentDither  = (ns[0] * 0.061f);
				currentDither -= (ns[1] * 0.11f);
				currentDither += (ns[8] * 0.126f);
				currentDither -= (ns[7] * 0.23f);
				currentDither += (ns[2] * 0.25f);
				currentDither -= (ns[3] * 0.43f);
				currentDither += (ns[6] * 0.5f);
				currentDither -= ns[5];
				currentDither += ns[4];
				//this sounds different from doing it in order of sample position
				//cumulative tiny errors seem to build up even at this buss depth
				//considerably more pronounced at 32 bit float.
				//Therefore we add the most significant components LAST.
				//trying to keep values on like exponents of the floating point value.
				inputSample += currentDither;
				
				inputSample = floor(inputSample);
				//DoublePaul dither
				break;
				
			case 6:
				currentDither = (float(fpd)/UINT32_MAX);
				inputSample += currentDither;
				inputSample -= ns[4];
				inputSample = floor(inputSample);
				ns[4] = ns[3];
				ns[3] = ns[2];
				ns[2] = ns[1];
				ns[1] = currentDither;
				//Tape dither
				break;
				
				
		case 7: 
		Position += 1;
		//Note- uses integer overflow as a 'mod' operator
		hotbinA = Position * Position;
		hotbinA = hotbinA % 170003; //% is C++ mod operator
		hotbinA *= hotbinA;
		hotbinA = hotbinA % 17011; //% is C++ mod operator
		hotbinA *= hotbinA;
		hotbinA = hotbinA % 1709; //% is C++ mod operator
		hotbinA *= hotbinA;
		hotbinA = hotbinA % 173; //% is C++ mod operator
		hotbinA *= hotbinA;
		hotbinA = hotbinA % 17;
		hotbinA *= 0.0635f;
		if (flip) hotbinA = -hotbinA;
		inputSample += hotbinA;
		inputSample = floor(inputSample);
		//Quadratic dither
		break;

		case 8:
		absSample = ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[0] += absSample; ns[0] /= 2; absSample -= ns[0];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[1] += absSample; ns[1] /= 2; absSample -= ns[1];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[2] += absSample; ns[2] /= 2; absSample -= ns[2];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[3] += absSample; ns[3] /= 2; absSample -= ns[3];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[4] += absSample; ns[4] /= 2; absSample -= ns[4];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[5] += absSample; ns[5] /= 2; absSample -= ns[5];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[6] += absSample; ns[6] /= 2; absSample -= ns[6];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[7] += absSample; ns[7] /= 2; absSample -= ns[7];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[8] += absSample; ns[8] /= 2; absSample -= ns[8];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[9] += absSample; ns[9] /= 2; absSample -= ns[9];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[10] += absSample; ns[10] /= 2; absSample -= ns[10];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[11] += absSample; ns[11] /= 2; absSample -= ns[11];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[12] += absSample; ns[12] /= 2; absSample -= ns[12];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[13] += absSample; ns[13] /= 2; absSample -= ns[13];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[14] += absSample; ns[14] /= 2; absSample -= ns[14];
		absSample += ((float(fpd)/UINT32_MAX) - 0.5f);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		ns[15] += absSample; ns[15] /= 2; absSample -= ns[15];
		//install noise and then shape it
		absSample += inputSample;
		
		//NSOdd /= 1.0001f; //NSDensity
		
		if (NSOdd > 0) NSOdd -= 0.97f;
		if (NSOdd < 0) NSOdd += 0.97f;
		
		NSOdd -= (NSOdd * NSOdd * NSOdd * 0.475f);

		NSOdd += prev;
		absSample += (NSOdd*0.475f);
		prev = floor(absSample) - inputSample;
		inputSample = floor(absSample);
		//TenNines dither
		break;
		
			case 9: 
				if (inputSample > 0) inputSample += 0.383f;
				if (inputSample < 0) inputSample -= 0.383f;
				//adjusting to permit more information drug outta the noisefloor
				contingentRnd = (float(fpd)/UINT32_MAX);
				fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
				contingentRnd += ((float(fpd)/UINT32_MAX)-1.0f); 
				contingentRnd *= randyConstant; //produce TPDF dist, scale
				contingentRnd -= contingentErr*omegaConstant; //include err
				absSample = fabs(inputSample);
				contingentErr = absSample - floor(absSample); //get next err
				contingent = contingentErr * 2.0f; //scale of quantization levels
				if (contingent > 1.0f) contingent = ((-contingent+2.0f)*omegaConstant) + expConstant;
				else contingent = (contingent * omegaConstant) + expConstant;
				//zero is next to a quantization level, one is exactly between them
				if (flip) contingentRnd = (contingentRnd * (1.0f-contingent)) + contingent + 0.5f;
				else contingentRnd = (contingentRnd * (1.0f-contingent)) - contingent + 0.5f;
				inputSample += (contingentRnd * contingent);
				//Contingent Dither
				inputSample = floor(inputSample);
				//note: this does not dither for values exactly the same as 16 bit values-
				//which forces the dither to gate at 0.0f. It goes to digital black,
				//and does a teeny parallel-compression thing when almost at digital black.
				break;
				
			case 10:
				if (inputSample > 0) inputSample += (0.3333333333f);
				if (inputSample < 0) inputSample -= (0.3333333333f);
				
				inputSample += (float(fpd)/UINT32_MAX)*0.6666666666f;
				
				benfordize = floor(inputSample);
				while (benfordize >= 1.0f) {benfordize /= 10;}
				if (benfordize < 1.0f) {benfordize *= 10;}
				if (benfordize < 1.0f) {benfordize *= 10;}
				hotbinA = floor(benfordize);
				//hotbin becomes the Benford bin value for this number floored
				totalA = 0;
				if ((hotbinA > 0) && (hotbinA < 10))
				{
					byn[hotbinA] += 1;
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
				hotbinB = floor(benfordize);
				//hotbin becomes the Benford bin value for this number ceiled
				totalB = 0;
				if ((hotbinB > 0) && (hotbinB < 10))
				{
					byn[hotbinB] += 1;
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
					inputSample = floor(inputSample);
				}
				else
				{
					byn[hotbinB] += 1;
					inputSample = ceil(inputSample);
				}
				//assign the relevant one to the delay line
				//and floor/ceil signal accordingly
				
				totalA = byn[1] + byn[2] + byn[3] + byn[4] + byn[5] + byn[6] + byn[7] + byn[8] + byn[9];
				totalA /= 1000;
				totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
				byn[1] /= totalA;
				byn[2] /= totalA;
				byn[3] /= totalA;
				byn[4] /= totalA;
				byn[5] /= totalA;
				byn[6] /= totalA;
				byn[7] /= totalA;
				byn[8] /= totalA;
				byn[9] /= totalA;
				byn[10] /= 2; //catchall for garbage data
				break;
				
			case 11: //this one is the Not Just Another Dither
				
				benfordize = floor(inputSample);
				while (benfordize >= 1.0f) {benfordize /= 10;}
				if (benfordize < 1.0f) {benfordize *= 10;}
				if (benfordize < 1.0f) {benfordize *= 10;}
				if (benfordize < 1.0f) {benfordize *= 10;}
				if (benfordize < 1.0f) {benfordize *= 10;}
				if (benfordize < 1.0f) {benfordize *= 10;}
				hotbinA = floor(benfordize);
				//hotbin becomes the Benford bin value for this number floored
				totalA = 0;
				if ((hotbinA > 0) && (hotbinA < 10))
				{
					byn[hotbinA] += 1;
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
				hotbinB = floor(benfordize);
				//hotbin becomes the Benford bin value for this number ceiled
				totalB = 0;
				if ((hotbinB > 0) && (hotbinB < 10))
				{
					byn[hotbinB] += 1;
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
					inputSample = floor(inputSample);
				}
				else
				{
					byn[hotbinB] += 1;
					inputSample = ceil(inputSample);
				}
				//assign the relevant one to the delay line
				//and floor/ceil signal accordingly
				
				totalA = byn[1] + byn[2] + byn[3] + byn[4] + byn[5] + byn[6] + byn[7] + byn[8] + byn[9];
				totalA /= 1000;
				totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
				byn[1] /= totalA;
				byn[2] /= totalA;
				byn[3] /= totalA;
				byn[4] /= totalA;
				byn[5] /= totalA;
				byn[6] /= totalA;
				byn[7] /= totalA;
				byn[8] /= totalA;
				byn[9] /= totalA;
				byn[10] /= 2; //catchall for garbage data
				break;
				
			case 12: 
			//slew only
			outputSample = (inputSample - lastSample)*trim;
			lastSample = inputSample;
			if (outputSample > 1.0f) outputSample = 1.0f;
			if (outputSample < -1.0f) outputSample = -1.0f;
			inputSample = outputSample;
		break;
				
		case 13: 
			//subs only
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
		break;
				
		case 14: 
			//silhouette
			bridgerectifier = fabs(inputSample)*1.57079633f;
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			bridgerectifier = 1.0f-cos(bridgerectifier);
			if (inputSample > 0.0f) inputSample = bridgerectifier;
			else inputSample = -bridgerectifier;
			
			silhouette = (float(fpd)/UINT32_MAX);
			fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
			silhouette -= 0.5f;
			silhouette *= 2.0f;
			silhouette *= fabs(inputSample);
			
			smoother = (float(fpd)/UINT32_MAX);
			smoother -= 0.5f;
			smoother *= 2.0f;
			smoother *= fabs(lastSample);
			lastSample = inputSample;
			
			silhouette += smoother;
			
			bridgerectifier = fabs(silhouette)*1.57079633f;
			if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
			bridgerectifier = sin(bridgerectifier);
			if (silhouette > 0.0f) silhouette = bridgerectifier;
			else silhouette = -bridgerectifier;
			
			inputSample = (silhouette + outSample) / 2.0f;
			outSample = silhouette;
		break;
		}
		
		flip = !flip;
		//several dithers use this
		if (highRes) inputSample /= 256.0f; //256 for 16/24 version
		if (dithering) inputSample /= 32768.0f;
		
		if (dtype == 8) noiseShaping += inputSample - drySample;

		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		//pseudorandom number updater
		
		*destP = inputSample;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	Position = 99999999;
	contingentErr = 0.0;
	byn[0] = 1000;
	byn[1] = 301;
	byn[2] = 176;
	byn[3] = 125;
	byn[4] = 97;
	byn[5] = 79;
	byn[6] = 67;
	byn[7] = 58;
	byn[8] = 51;
	byn[9] = 46;
	byn[10] = 1000;
	noiseShaping = 0.0;
	NSOdd = 0.0;
	NSEven = 0.0;
	prev = 0.0;
	ns[0] = 0;
	ns[1] = 0;
	ns[2] = 0;
	ns[3] = 0;
	ns[4] = 0;
	ns[5] = 0;
	ns[6] = 0;
	ns[7] = 0;
	ns[8] = 0;
	ns[9] = 0;
	ns[10] = 0;
	ns[11] = 0;
	ns[12] = 0;
	ns[13] = 0;
	ns[14] = 0;
	ns[15] = 0;
	lastSample = 0.0;
	outSample = 0.0;
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
