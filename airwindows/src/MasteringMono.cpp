#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "MasteringMono"
#define AIRWINDOWS_DESCRIPTION "MasteringMono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','a','u' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
static const int kDark = 1;
static const int kTenNines = 2;
static const int kTPDFWide = 3;
static const int kPaulWide = 4;
static const int kNJAD = 5;
static const int kBypass = 6;
static const int kDefaultValue_ParamF = kBypass;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static char const * const enumStrings5[] = { "", "Dark", "Ten Nines", "TPDF", "PaulDither", "NJAD", "Bypass", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Glue", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Scope", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Skronk", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Girth", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dither", .min = 1, .max = 6, .def = 6, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings5 },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		enum {
			pvAL1,
			pvSL1,
			accSL1,
			acc2SL1,
			pvAL2,
			pvSL2,
			accSL2,
			acc2SL2,
			pvAL3,
			pvSL3,
			accSL3,
			pvAL4,
			pvSL4,
			gndavgL,
			outAL,
			gainAL,
			air_total
		};
		
		enum {
			prevSampL1,
			prevSlewL1,
			accSlewL1,
			prevSampL2,
			prevSlewL2,
			accSlewL2,
			prevSampL3,
			prevSlewL3,
			accSlewL3,
			kalGainL,
			kalOutL,
			kalAvgL,
			kal_total
		};
		
		float lastSinewL;
		//this is overkill, used to run both Zoom and Sinew stages as they are after
		//the summing in StoneFire, which sums three floats to a long float.
		
		float lastSampleL;
		float intermediateL[16];
		bool wasPosClipL;
		bool wasNegClipL;
		
		int quantA;
		int quantB;
		float expectedSlew;
		float testA;
		float testB;
		float correction;
		float shapedSampleL;
		float currentDither;
		float ditherL;
		bool cutbinsL;
		int hotbinA;
		int hotbinB;
		float benfordize;
		float totalA;
		float totalB;
		float outputSample;
		int expon; //internal dither variables
		
		float NSOddL; //dither section!
		float NSEvenL;
		float prevShapeL;
		bool flip; //VinylDither
		float previousDitherL;
		float bynL[13];
		float noiseShapingL; //NJAD
		
		uint32_t fpdL;
	
	struct _dram {
			float air[air_total];
		float kalM[kal_total];
		float kalS[kal_total];
		float darkSampleL[100];
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
	
	float threshSinew = (0.25f+((1.0f-GetParameter( kParam_A ))*0.333f))/overallscale;
	float depthSinew = 1.0f-powf(1.0f-GetParameter( kParam_A ),2.0f);
	
	float trebleZoom = GetParameter( kParam_B )-0.5f;
	float trebleGain = (trebleZoom*fabs(trebleZoom))+1.0f;
	if (trebleGain > 1.0f) trebleGain = powf(trebleGain,3.0f+sqrt(overallscale));
	//this boost is necessary to adapt to higher sample rates
	
	float midZoom = GetParameter( kParam_C )-0.5f;
	float midGain = (midZoom*fabs(midZoom))+1.0f;
	float kalMid = 0.35f-(GetParameter( kParam_C )*0.25f); //crossover frequency between mid/bass
	float kalSub = 0.45f+(GetParameter( kParam_C )*0.25f); //crossover frequency between bass/sub
	
	float bassZoom = (GetParameter( kParam_D )*0.5f)-0.25f;
	float bassGain = (-bassZoom*fabs(bassZoom))+1.0f; //control inverted
	float subGain = ((GetParameter( kParam_D )*0.25f)-0.125f)+1.0f;
	if (subGain < 1.0f) subGain = 1.0f; //very small sub shift, only pos.
	
	float driveIn = (GetParameter( kParam_E )-0.5f)+1.0f;
	float driveOut = (-(GetParameter( kParam_E )-0.5f)*fabs(GetParameter( kParam_E )-0.5f))+1.0f;
	
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;
	int dither = (int) GetParameter( kParam_F );
	int depth = (int)(17.0f*overallscale);
	if (depth < 3) depth = 3; if (depth > 98) depth = 98; //for Dark
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		inputSampleL *= driveIn;
		float drySampleL = inputSampleL;
		
		//begin Air3L
		dram->air[pvSL4] = dram->air[pvAL4] - dram->air[pvAL3]; dram->air[pvSL3] = dram->air[pvAL3] - dram->air[pvAL2];
		dram->air[pvSL2] = dram->air[pvAL2] - dram->air[pvAL1]; dram->air[pvSL1] = dram->air[pvAL1] - inputSampleL;
		dram->air[accSL3] = dram->air[pvSL4] - dram->air[pvSL3]; dram->air[accSL2] = dram->air[pvSL3] - dram->air[pvSL2];
		dram->air[accSL1] = dram->air[pvSL2] - dram->air[pvSL1];
		dram->air[acc2SL2] = dram->air[accSL3] - dram->air[accSL2]; dram->air[acc2SL1] = dram->air[accSL2] - dram->air[accSL1];		
		dram->air[outAL] = -(dram->air[pvAL1] + dram->air[pvSL3] + dram->air[acc2SL2] - ((dram->air[acc2SL2] + dram->air[acc2SL1])*0.5f));
		dram->air[gainAL] *= 0.5f; dram->air[gainAL] += fabs(drySampleL-dram->air[outAL])*0.5f;
		if (dram->air[gainAL] > 0.3f*sqrt(overallscale)) dram->air[gainAL] = 0.3f*sqrt(overallscale);
		dram->air[pvAL4] = dram->air[pvAL3]; dram->air[pvAL3] = dram->air[pvAL2];
		dram->air[pvAL2] = dram->air[pvAL1]; dram->air[pvAL1] = (dram->air[gainAL] * dram->air[outAL]) + drySampleL;
		float midL = drySampleL - ((dram->air[outAL]*0.5f)+(drySampleL*(0.457f-(0.017f*overallscale))));
		float temp = (midL + dram->air[gndavgL])*0.5f; dram->air[gndavgL] = midL; midL = temp;
		float trebleL = drySampleL-midL;
		//end Air3L
		
		//begin KalmanML
		temp = midL;
		dram->kalM[prevSlewL3] += dram->kalM[prevSampL3] - dram->kalM[prevSampL2]; dram->kalM[prevSlewL3] *= 0.5f;
		dram->kalM[prevSlewL2] += dram->kalM[prevSampL2] - dram->kalM[prevSampL1]; dram->kalM[prevSlewL2] *= 0.5f;
		dram->kalM[prevSlewL1] += dram->kalM[prevSampL1] - midL; dram->kalM[prevSlewL1] *= 0.5f;
		//make slews from each set of samples used
		dram->kalM[accSlewL2] += dram->kalM[prevSlewL3] - dram->kalM[prevSlewL2]; dram->kalM[accSlewL2] *= 0.5f;
		dram->kalM[accSlewL1] += dram->kalM[prevSlewL2] - dram->kalM[prevSlewL1]; dram->kalM[accSlewL1] *= 0.5f;
		//differences between slews: rate of change of rate of change
		dram->kalM[accSlewL3] += (dram->kalM[accSlewL2] - dram->kalM[accSlewL1]); dram->kalM[accSlewL3] *= 0.5f;
		//entering the abyss, what even is this
		dram->kalM[kalOutL] += dram->kalM[prevSampL1] + dram->kalM[prevSlewL2] + dram->kalM[accSlewL3]; dram->kalM[kalOutL] *= 0.5f;
		
		//resynthesizing predicted result (all iir smoothed)
		dram->kalM[kalGainL] += fabs(temp-dram->kalM[kalOutL])*kalMid*8.0f; dram->kalM[kalGainL] *= 0.5f;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kalM[kalGainL] > kalMid*0.5f) dram->kalM[kalGainL] = kalMid*0.5f;
		//attempts to avoid explosions
		dram->kalM[kalOutL] += (temp*(1.0f-(0.68f+(kalMid*0.157f))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kalM[prevSampL3] = dram->kalM[prevSampL2]; dram->kalM[prevSampL2] = dram->kalM[prevSampL1];
		dram->kalM[prevSampL1] = (dram->kalM[kalGainL] * dram->kalM[kalOutL]) + ((1.0f-dram->kalM[kalGainL])*temp);
		//feed the chain of previous samples
		float bassL = (dram->kalM[kalOutL]+dram->kalM[kalAvgL])*0.5f;
		dram->kalM[kalAvgL] = dram->kalM[kalOutL];
		midL -= bassL;
		//end KalmanML
				
		//begin KalmanSL
		temp = bassL;
		dram->kalS[prevSlewL3] += dram->kalS[prevSampL3] - dram->kalS[prevSampL2]; dram->kalS[prevSlewL3] *= 0.5f;
		dram->kalS[prevSlewL2] += dram->kalS[prevSampL2] - dram->kalS[prevSampL1]; dram->kalS[prevSlewL2] *= 0.5f;
		dram->kalS[prevSlewL1] += dram->kalS[prevSampL1] - bassL; dram->kalS[prevSlewL1] *= 0.5f;
		//make slews from each set of samples used
		dram->kalS[accSlewL2] += dram->kalS[prevSlewL3] - dram->kalS[prevSlewL2]; dram->kalS[accSlewL2] *= 0.5f;
		dram->kalS[accSlewL1] += dram->kalS[prevSlewL2] - dram->kalS[prevSlewL1]; dram->kalS[accSlewL1] *= 0.5f;
		//differences between slews: rate of change of rate of change
		dram->kalS[accSlewL3] += (dram->kalS[accSlewL2] - dram->kalS[accSlewL1]); dram->kalS[accSlewL3] *= 0.5f;
		//entering the abyss, what even is this
		dram->kalS[kalOutL] += dram->kalS[prevSampL1] + dram->kalS[prevSlewL2] + dram->kalS[accSlewL3]; dram->kalS[kalOutL] *= 0.5f;
		//resynthesizing predicted result (all iir smoothed)
		dram->kalS[kalGainL] += fabs(temp-dram->kalS[kalOutL])*kalSub*8.0f; dram->kalS[kalGainL] *= 0.5f;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kalS[kalGainL] > kalSub*0.5f) dram->kalS[kalGainL] = kalSub*0.5f;
		//attempts to avoid explosions
		dram->kalS[kalOutL] += (temp*(1.0f-(0.68f+(kalSub*0.157f))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kalS[prevSampL3] = dram->kalS[prevSampL2]; dram->kalS[prevSampL2] = dram->kalS[prevSampL1];
		dram->kalS[prevSampL1] = (dram->kalS[kalGainL] * dram->kalS[kalOutL]) + ((1.0f-dram->kalS[kalGainL])*temp);
		//feed the chain of previous samples
		float subL = (dram->kalS[kalOutL]+dram->kalS[kalAvgL])*0.5f;
		dram->kalS[kalAvgL] = dram->kalS[kalOutL];
		bassL -= subL;
		//end KalmanSL
		
		inputSampleL = (subL*subGain);
		
		if (bassZoom > 0.0f) {
			float closer = bassL * 1.57079633f;
			if (closer > 1.57079633f) closer = 1.57079633f;
			if (closer < -1.57079633f) closer = -1.57079633f;
			bassL = (bassL*(1.0f-bassZoom))+(sin(closer)*bassZoom);
		} //zooming in will make the body of the sound louder: it's just Density
		if (bassZoom < 0.0f) {
			float farther = fabs(bassL) * 1.57079633f;
			if (farther > 1.57079633f) farther = 1.0f;
			else farther = 1.0f-cos(farther);
			if (bassL > 0.0f) bassL = (bassL*(1.0f+bassZoom))-(farther*bassZoom*1.57079633f);
			if (bassL < 0.0f) bassL = (bassL*(1.0f+bassZoom))+(farther*bassZoom*1.57079633f);			
		} //zooming out boosts the hottest peaks but cuts back softer stuff
		inputSampleL += (bassL*bassGain);
		
		if (midZoom > 0.0f) {
			float closer = midL * 1.57079633f;
			if (closer > 1.57079633f) closer = 1.57079633f;
			if (closer < -1.57079633f) closer = -1.57079633f;
			midL = (midL*(1.0f-midZoom))+(sin(closer)*midZoom);
		} //zooming in will make the body of the sound louder: it's just Density
		if (midZoom < 0.0f) {
			float farther = fabs(midL) * 1.57079633f;
			if (farther > 1.57079633f) farther = 1.0f;
			else farther = 1.0f-cos(farther);
			if (midL > 0.0f) midL = (midL*(1.0f+midZoom))-(farther*midZoom*1.57079633f);
			if (midL < 0.0f) midL = (midL*(1.0f+midZoom))+(farther*midZoom*1.57079633f);			
		} //zooming out boosts the hottest peaks but cuts back softer stuff
		inputSampleL += (midL*midGain);
		
		if (trebleZoom > 0.0f) {
			float closer = trebleL * 1.57079633f;
			if (closer > 1.57079633f) closer = 1.57079633f;
			if (closer < -1.57079633f) closer = -1.57079633f;
			trebleL = (trebleL*(1.0f-trebleZoom))+(sin(closer)*trebleZoom);
		} //zooming in will make the body of the sound louder: it's just Density
		if (trebleZoom < 0.0f) {
			float farther = fabs(trebleL) * 1.57079633f;
			if (farther > 1.57079633f) farther = 1.0f;
			else farther = 1.0f-cos(farther);
			if (trebleL > 0.0f) trebleL = (trebleL*(1.0f+trebleZoom))-(farther*trebleZoom*1.57079633f);
			if (trebleL < 0.0f) trebleL = (trebleL*(1.0f+trebleZoom))+(farther*trebleZoom*1.57079633f);			
		} //zooming out boosts the hottest peaks but cuts back softer stuff
		inputSampleL += (trebleL*trebleGain);
		
		inputSampleL *= driveOut;
		
		//begin ClipOnly2 as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0f) inputSampleL = 4.0f; if (inputSampleL < -4.0f) inputSampleL = -4.0f;
		if (wasPosClipL == true) { //current will be over
			if (inputSampleL<lastSampleL) lastSampleL=0.7058208f+(inputSampleL*0.2609148f);
			else lastSampleL = 0.2491717f+(lastSampleL*0.7390851f);
		} wasPosClipL = false;
		if (inputSampleL>0.9549925859f) {wasPosClipL=true;inputSampleL=0.7058208f+(lastSampleL*0.2609148f);}
		if (wasNegClipL == true) { //current will be -over
			if (inputSampleL > lastSampleL) lastSampleL=-0.7058208f+(inputSampleL*0.2609148f);
			else lastSampleL=-0.2491717f+(lastSampleL*0.7390851f);
		} wasNegClipL = false;
		if (inputSampleL<-0.9549925859f) {wasNegClipL=true;inputSampleL=-0.7058208f+(lastSampleL*0.2609148f);}
		intermediateL[spacing] = inputSampleL;
        inputSampleL = lastSampleL; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediateL[x-1] = intermediateL[x];
		lastSampleL = intermediateL[0]; //run a little buffer to handle this
		//end ClipOnly2 as a little, compressed chunk that can be dropped into code
		
		temp = inputSampleL;
		float sinew = threshSinew * cos(lastSinewL*lastSinewL);
		if (inputSampleL - lastSinewL > sinew) temp = lastSinewL + sinew;
		if (-(inputSampleL - lastSinewL) > sinew) temp = lastSinewL - sinew;
		lastSinewL = temp;
		inputSampleL = (inputSampleL * (1.0f-depthSinew))+(lastSinewL*depthSinew);
		//run Sinew to stop excess slews, but run a dry/wet to allow a range of brights
		
		switch (dither) {
			case 1:
				//begin Dark		
				inputSampleL *= 8388608.0f; //we will apply the 24 bit Dark
				//We are doing it first Left, then Right, because the loops may run faster if
				//they aren't too jammed full of variables. This means re-running code.
				
				//begin left
				quantA = floor(inputSampleL);
				quantB = floor(inputSampleL+1.0f);
				//to do this style of dither, we quantize in either direction and then
				//do a reconstruction of what the result will be for each choice.
				//We then evaluate which one we like, and keep a history of what we previously had
				
				expectedSlew = 0;
				for(int x = 0; x < depth; x++) {
					expectedSlew += (dram->darkSampleL[x+1] - dram->darkSampleL[x]);
				}
				expectedSlew /= depth; //we have an average of all recent slews
				//we are doing that to voice the thing down into the upper mids a bit
				//it mustn't just soften the brightest treble, it must smooth high mids too
				
				testA = fabs((dram->darkSampleL[0] - quantA) - expectedSlew);
				testB = fabs((dram->darkSampleL[0] - quantB) - expectedSlew);
				
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
								
				inputSampleL /= 8388608.0f;
				break; //Dark (Monitoring2)
			case 2:
				//begin Dark	 for Ten Nines
				inputSampleL *= 8388608.0f; //we will apply the 24 bit Dark
				//We are doing it first Left, then Right, because the loops may run faster if
				//they aren't too jammed full of variables. This means re-running code.
				
				//begin L
				correction = 0;
				if (flip) {
					NSOddL = (NSOddL * 0.9999999999f) + prevShapeL;
					NSEvenL = (NSEvenL * 0.9999999999f) - prevShapeL;
					correction = NSOddL;
				} else {
					NSOddL = (NSOddL * 0.9999999999f) - prevShapeL;
					NSEvenL = (NSEvenL * 0.9999999999f) + prevShapeL;
					correction = NSEvenL;
				}
				shapedSampleL = inputSampleL+correction;
				//end Ten Nines L
				
				//begin Dark L
				quantA = floor(shapedSampleL);
				quantB = floor(shapedSampleL+1.0f);
				//to do this style of dither, we quantize in either direction and then
				//do a reconstruction of what the result will be for each choice.
				//We then evaluate which one we like, and keep a history of what we previously had
				
				expectedSlew = 0;
				for(int x = 0; x < depth; x++) {
					expectedSlew += (dram->darkSampleL[x+1] - dram->darkSampleL[x]);
				}
				expectedSlew /= depth; //we have an average of all recent slews
				//we are doing that to voice the thing down into the upper mids a bit
				//it mustn't just soften the brightest treble, it must smooth high mids too
				
				testA = fabs((dram->darkSampleL[0] - quantA) - expectedSlew);
				testB = fabs((dram->darkSampleL[0] - quantB) - expectedSlew);
				
				if (testA < testB) inputSampleL = quantA;
				else inputSampleL = quantB;
				//select whichever one departs LEAST from the vector of averaged
				//reconstructed previous final samples. This will force a kind of dithering
				//as it'll make the output end up as smooth as possible
				
				for(int x = depth; x >=0; x--) {
					dram->darkSampleL[x+1] = dram->darkSampleL[x];
				}
				dram->darkSampleL[0] = inputSampleL;
				//end Dark L
				
				prevShapeL = (floor(shapedSampleL) - inputSampleL)*0.9999999999f;
				//end Ten Nines L

				flip = !flip;
				
				inputSampleL /= 8388608.0f;
				break; //Ten Nines (which goes into Dark in Monitoring3)
			case 3:
				inputSampleL *= 8388608.0f;
				
				ditherL = -1.0f;
				ditherL += (float(fpdL)/UINT32_MAX);
				fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
				ditherL += (float(fpdL)/UINT32_MAX);
				//TPDF: two 0-1 random noises
												
				inputSampleL = floor(inputSampleL+ditherL);
				
				inputSampleL /= 8388608.0f;
				break; //TPDF
			case 4:
				inputSampleL *= 8388608.0f;
				//Paul Frindle: It's true that the dither itself can sound different 
				//if it's given a different freq response and you get to hear it. 
				//The one we use most is triangular single pole high pass dither. 
				//It's not freq bent enough to sound odd, but is slightly less audible than 
				//flat dither. It can also be easily made by taking one sample of dither 
				//away from the previous one - this gives you the triangular PDF and the 
				//filtering in one go :-)
				
				currentDither = (float(fpdL)/UINT32_MAX);
				ditherL = currentDither;
				ditherL -= previousDitherL;
				previousDitherL = currentDither;
				//TPDF: 0-1 random noise
				
				inputSampleL = floor(inputSampleL+ditherL);
				
				inputSampleL /= 8388608.0f;
				break; //PaulDither
			case 5:
				inputSampleL *= 8388608.0f;
				cutbinsL = false;
				drySampleL = inputSampleL;//re-using in NJAD
				inputSampleL -= noiseShapingL;
				//NJAD L
				benfordize = floor(inputSampleL);
				while (benfordize >= 1.0f) benfordize /= 10;
				while (benfordize < 1.0f && benfordize > 0.0000001f) benfordize *= 10;
				hotbinA = floor(benfordize);
				//hotbin becomes the Benford bin value for this number floored
				totalA = 0.0f;
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
				hotbinB = floor(benfordize);
				//hotbin becomes the Benford bin value for this number ceiled
				totalB = 0.0f;
				if ((hotbinB > 0) && (hotbinB < 10))
				{
					bynL[hotbinB] += 1; if (bynL[hotbinB] > 982) cutbinsL = true;
					totalB += (301-bynL[1]); totalB += (176-bynL[2]); totalB += (125-bynL[3]);
					totalB += (97-bynL[4]); totalB += (79-bynL[5]); totalB += (67-bynL[6]);
					totalB += (58-bynL[7]); totalB += (51-bynL[8]); totalB += (46-bynL[9]); bynL[hotbinB] -= 1;
				} else hotbinB = 10;
				//produce total number- smaller is closer to Benford real
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
				inputSampleL /= 8388608.0f;
				if (inputSampleL > 1.0f) inputSampleL = 1.0f;
				if (inputSampleL < -1.0f) inputSampleL = -1.0f;
				//finished NJAD L
				break; //NJAD (Monitoring. Brightest)
			case 6:
				
				break; //Bypass for saving floating point files directly
		}
				
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < air_total; x++) dram->air[x] = 0.0;
	for (int x = 0; x < kal_total; x++) {dram->kalM[x] = 0.0;dram->kalS[x] = 0.0;}
	
	lastSinewL = 0.0;
	
	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	for (int x = 0; x < 16; x++) {intermediateL[x] = 0.0;}
	
	quantA = 0;
	quantB = 1;
	expectedSlew = 0.0;
	testA = 0.0;
	testB = 0.0;
	correction = 0.0;
	shapedSampleL = 0.0;
	currentDither = 0.0;
	ditherL = 0.0;
	cutbinsL = false;
	hotbinA = 0;
	hotbinB = 0;
	benfordize = 0.0;
	totalA = 0.0;
	totalB = 0.0;
	outputSample = 0.0;
	expon = 0; //internal dither variables
	//these didn't like to be defined inside a case statement
	
	NSOddL = 0.0; NSEvenL = 0.0; prevShapeL = 0.0;
	flip = true; //Ten Nines
	for(int count = 0; count < 99; count++) {
		dram->darkSampleL[count] = 0;
	} //Dark
	previousDitherL = 0.0; //PaulWide
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
	noiseShapingL = 0.0; //NJAD
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
}
};
