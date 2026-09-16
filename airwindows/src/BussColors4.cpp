#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BussColors4"
#define AIRWINDOWS_DESCRIPTION "The Airwindows console emulations, now working up to 192K correctly."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','u','4' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
static const int kDark = 1;
static const int kRock = 2;
static const int kLush = 3;
static const int kVibe = 4;
static const int kHolo = 5;
static const int kPunch = 6;
static const int kSteel = 7;
static const int kTube = 8;
static const int kDefaultValue_ParamOne = kDark;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static char const * const enumStrings0[] = { "", "Dark", "Rock", "Lush", "Vibe", "Holo", "Punch", "Steel", "Tube", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Console Type", .min = 1, .max = 8, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "Input Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		int c[35]; //just the number of taps we use, doesn't have to scale
		Float32 g[9]; //console model
		Float32 outg[9]; //console model
		Float32 control;
		int gcount;
		Float32 slowdyn;
		uint32_t fpd;
	
	struct _dram {
			Float32 b[175]; //full buffer for high sample rates. Scales to 192K
		Float32 d[100]; //buffer for calculating sag as it relates to the dynamic impulse synthesis. To 192K.
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
	if (overallscale < 1.0f) overallscale = 1.0f;
	if (overallscale > 4.5f) overallscale = 4.5f;
	
	const int maxConvolutionBufferSize = (int)(34.0f * overallscale); //we won't use more of the buffer than we have to
	for (int count = 0; count < 34; count++) c[count] = (int)(count * overallscale); //assign conv taps

	float inputSample;
	Float32 drySample;
	Float32 applyconv;
	
	int offsetA = 3;
	Float32 dynamicconv = 3.0f;
	Float32 gain = 0.436f;
	Float32 outgain = 1.0f;
	Float32 bridgerectifier;
	
	int console = (int) GetParameter( kParam_One );
	switch (console)
	{
		case 1: offsetA = 4; gain = g[1]; outgain = outg[1]; break; //Dark (Focusrite)
		case 2: offsetA = 3; gain = g[2]; outgain = outg[2]; break; //Rock (SSL)
		case 3: offsetA = 5; gain = g[3]; outgain = outg[3]; break; //Lush (Neve)
		case 4: offsetA = 8; gain = g[4]; outgain = outg[4]; break; //Vibe (Elation)
		case 5: offsetA = 5; gain = g[5]; outgain = outg[5]; break; //Holo (Precision 8)
		case 6: offsetA = 7; gain = g[6]; outgain = outg[6]; break; //Punch (API)
		case 7: offsetA = 7; gain = g[7]; outgain = outg[7]; break; //Steel (Calibre)
		case 8: offsetA = 6; gain = g[8]; outgain = outg[8]; break; //Tube (Manley)
	}
	offsetA = (int)(offsetA * overallscale); //we extend the sag buffer too, at high sample rates
	if (offsetA < 3) offsetA = 3; //are we getting divides by zero?
	gain *= powf(10.0f,GetParameter( kParam_Two )/14.0f); //add adjustment factor
	outgain *= powf(10.0f,(GetParameter( kParam_Three )+3.3f)/14.0f); //add adjustment factor
	Float32 wet = GetParameter( kParam_Four );
	//removed unnecessary dry variable
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		if (gain != 1.0f) {
			inputSample *= gain;
		}
		
		bridgerectifier = fabs(inputSample);
		
		slowdyn *= 0.999f;
		slowdyn += bridgerectifier;
		if (slowdyn > 1.5f) {slowdyn = 1.5f;}
		//before the iron bar- fry console for crazy behavior
		dynamicconv = 2.5f + slowdyn;
		
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.0f;
		else bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		
		
		if (gcount < 0 || gcount > 44) gcount = 44;
		dram->d[gcount+44] = dram->d[gcount] = fabs(inputSample);
		control += (dram->d[gcount] / offsetA);
		control -= (dram->d[gcount+offsetA] / offsetA);
		control -= 0.000001f;				
		if (control < 0) control = 0;
		if (control > 100) control = 100;
		gcount--;
		applyconv = (control / offsetA) * dynamicconv;
		//now we have a 'sag' style average to apply to the conv
		
		//now the convolution
		for (int count = maxConvolutionBufferSize; count > 0; --count) {dram->b[count] = dram->b[count-1];} //was 173
		//we're only doing assigns, and it saves us an add inside the convolution calculation
		//therefore, we'll just assign everything one step along and have our buffer that way.
		dram->b[0] = inputSample;

		switch (console)
		{
			case 1:
				//begin Cider (Focusrite) MCI
				inputSample += (dram->b[c[1]] * (0.61283288942201319f  + (0.00024011410669522f*applyconv)));
				inputSample -= (dram->b[c[2]] * (0.24036380659761222f  - (0.00020789518206241f*applyconv)));
				inputSample += (dram->b[c[3]] * (0.09104669761717916f  + (0.00012829642741548f*applyconv)));
				inputSample -= (dram->b[c[4]] * (0.02378290768554025f  - (0.00017673646470440f*applyconv)));
				inputSample -= (dram->b[c[5]] * (0.02832818490275965f  - (0.00013536187747384f*applyconv)));
				inputSample += (dram->b[c[6]] * (0.03268797679215937f  + (0.00015035126653359f*applyconv)));
				inputSample -= (dram->b[c[7]] * (0.04024464202655586f  - (0.00015034923056735f*applyconv)));
				inputSample += (dram->b[c[8]] * (0.01864890074318696f  + (0.00014513281680642f*applyconv)));
				inputSample -= (dram->b[c[9]] * (0.01632731954100322f  - (0.00015509089075614f*applyconv)));
				inputSample -= (dram->b[c[10]] * (0.00318907090555589f  - (0.00014784812076550f*applyconv)));
				inputSample -= (dram->b[c[11]] * (0.00208573465221869f  - (0.00015350520779465f*applyconv)));
				inputSample -= (dram->b[c[12]] * (0.00907033901519614f  - (0.00015442964157250f*applyconv)));
				inputSample -= (dram->b[c[13]] * (0.00199458794148013f  - (0.00015595640046297f*applyconv)));
				inputSample -= (dram->b[c[14]] * (0.00705979153201755f  - (0.00015730069418051f*applyconv)));
				inputSample -= (dram->b[c[15]] * (0.00429488975412722f  - (0.00015743697943505f*applyconv)));
				inputSample -= (dram->b[c[16]] * (0.00497724878704936f  - (0.00016014760011861f*applyconv)));
				inputSample -= (dram->b[c[17]] * (0.00506059305562353f  - (0.00016194824072466f*applyconv)));
				inputSample -= (dram->b[c[18]] * (0.00483432223285621f  - (0.00016329050124225f*applyconv)));
				inputSample -= (dram->b[c[19]] * (0.00495100420886005f  - (0.00016297509798749f*applyconv)));
				inputSample -= (dram->b[c[20]] * (0.00489319520555115f  - (0.00016472839684661f*applyconv)));
				inputSample -= (dram->b[c[21]] * (0.00489177657970308f  - (0.00016791875866630f*applyconv)));
				inputSample -= (dram->b[c[22]] * (0.00487900894707044f  - (0.00016755993898534f*applyconv)));
				inputSample -= (dram->b[c[23]] * (0.00486234009335561f  - (0.00016968157345446f*applyconv)));
				inputSample -= (dram->b[c[24]] * (0.00485737490288736f  - (0.00017180713324431f*applyconv)));
				inputSample -= (dram->b[c[25]] * (0.00484106070563455f  - (0.00017251073661092f*applyconv)));
				inputSample -= (dram->b[c[26]] * (0.00483219429408410f  - (0.00017321683790891f*applyconv)));
				inputSample -= (dram->b[c[27]] * (0.00482013597437550f  - (0.00017392186866488f*applyconv)));
				inputSample -= (dram->b[c[28]] * (0.00480949628051497f  - (0.00017569098775602f*applyconv)));
				inputSample -= (dram->b[c[29]] * (0.00479992055604049f  - (0.00017746046369449f*applyconv)));
				inputSample -= (dram->b[c[30]] * (0.00478750757986987f  - (0.00017745630047554f*applyconv)));
				inputSample -= (dram->b[c[31]] * (0.00477828651185740f  - (0.00017958043287604f*applyconv)));
				inputSample -= (dram->b[c[32]] * (0.00476906544384494f  - (0.00018170456527653f*applyconv)));
				inputSample -= (dram->b[c[33]] * (0.00475700712413634f  - (0.00018099144598088f*applyconv)));
				//end Cider (Focusrite) MCI
				break;
			case 2:
				//begin Rock (SSL) conv
				inputSample += (dram->b[c[1]] * (0.67887916185274055f  + (0.00068787552301086f*applyconv)));
				inputSample -= (dram->b[c[2]] * (0.25671050678827934f  + (0.00017691749454490f*applyconv)));
				inputSample += (dram->b[c[3]] * (0.15135839896615280f  + (0.00007481480365043f*applyconv)));
				inputSample -= (dram->b[c[4]] * (0.11813512969090802f  + (0.00005191138121359f*applyconv)));
				inputSample += (dram->b[c[5]] * (0.08329104347166429f  + (0.00001871054659794f*applyconv)));
				inputSample -= (dram->b[c[6]] * (0.07663817456103936f  + (0.00002751359071705f*applyconv)));
				inputSample += (dram->b[c[7]] * (0.05477586152148759f  + (0.00000744843212679f*applyconv)));
				inputSample -= (dram->b[c[8]] * (0.05547314737187786f  + (0.00001025289931145f*applyconv)));
				inputSample += (dram->b[c[9]] * (0.03822948356540711f  - (0.00000249791561457f*applyconv)));
				inputSample -= (dram->b[c[10]] * (0.04199383340841713f  - (0.00000067328840674f*applyconv)));
				inputSample += (dram->b[c[11]] * (0.02695796542339694f  - (0.00000796704606548f*applyconv)));
				inputSample -= (dram->b[c[12]] * (0.03228715059431878f  - (0.00000579711816722f*applyconv)));
				inputSample += (dram->b[c[13]] * (0.01846929689819187f  - (0.00000984017804950f*applyconv)));
				inputSample -= (dram->b[c[14]] * (0.02528050435045951f  - (0.00000701189792484f*applyconv)));
				inputSample += (dram->b[c[15]] * (0.01207844846859765f  - (0.00001522630289356f*applyconv)));
				inputSample -= (dram->b[c[16]] * (0.01894464378378515f  - (0.00001205456372080f*applyconv)));
				inputSample += (dram->b[c[17]] * (0.00667804407593324f  - (0.00001343604283817f*applyconv)));
				inputSample -= (dram->b[c[18]] * (0.01408418045473130f  - (0.00001246443581504f*applyconv)));
				inputSample += (dram->b[c[19]] * (0.00228696509481569f  - (0.00001506764046927f*applyconv)));
				inputSample -= (dram->b[c[20]] * (0.01006277891348454f  - (0.00000970723079112f*applyconv)));
				inputSample -= (dram->b[c[21]] * (0.00132368373546377f  + (0.00001188847238761f*applyconv)));
				inputSample -= (dram->b[c[22]] * (0.00676615715578373f  - (0.00001209129844861f*applyconv)));
				inputSample -= (dram->b[c[23]] * (0.00426288438418556f  + (0.00001286836943559f*applyconv)));
				inputSample -= (dram->b[c[24]] * (0.00408897698639688f  - (0.00001102542567911f*applyconv)));
				inputSample -= (dram->b[c[25]] * (0.00662040619382751f  + (0.00001206328529063f*applyconv)));
				inputSample -= (dram->b[c[26]] * (0.00196101294183599f  - (0.00000950703614981f*applyconv)));
				inputSample -= (dram->b[c[27]] * (0.00845620581010342f  + (0.00001279970295678f*applyconv)));
				inputSample -= (dram->b[c[28]] * (0.00032595215043616f  - (0.00000920518241371f*applyconv)));
				inputSample -= (dram->b[c[29]] * (0.00982957737435458f  + (0.00001177745362317f*applyconv)));
				inputSample += (dram->b[c[30]] * (0.00086920573760513f  + (0.00000913758382404f*applyconv)));
				inputSample -= (dram->b[c[31]] * (0.01079020871452061f  + (0.00000900750153697f*applyconv)));
				inputSample += (dram->b[c[32]] * (0.00167613606334460f  + (0.00000732769151038f*applyconv)));
				inputSample -= (dram->b[c[33]] * (0.01138050011044332f  + (0.00000946908207442f*applyconv)));
				//end Rock (SSL) conv
				break;
			case 3:
				//begin Lush (Neve) conv
				inputSample += (dram->b[c[1]] * (0.20641602693167951f  - (0.00078952185394898f*applyconv)));
				inputSample -= (dram->b[c[2]] * (0.07601816702459827f  + (0.00022786334179951f*applyconv)));
				inputSample += (dram->b[c[3]] * (0.03929765560019285f  - (0.00054517993246352f*applyconv)));
				inputSample += (dram->b[c[4]] * (0.00298333157711103f  - (0.00033083756545638f*applyconv)));
				inputSample -= (dram->b[c[5]] * (0.00724006282304610f  + (0.00045483683460812f*applyconv)));
				inputSample += (dram->b[c[6]] * (0.03073108963506036f  - (0.00038190060537423f*applyconv)));
				inputSample -= (dram->b[c[7]] * (0.02332434692533051f  + (0.00040347288688932f*applyconv)));
				inputSample += (dram->b[c[8]] * (0.03792606869061214f  - (0.00039673687335892f*applyconv)));
				inputSample -= (dram->b[c[9]] * (0.02437059376675688f  + (0.00037221210539535f*applyconv)));
				inputSample += (dram->b[c[10]] * (0.03416764311979521f  - (0.00040314850796953f*applyconv)));
				inputSample -= (dram->b[c[11]] * (0.01761669868102127f  + (0.00035989484330131f*applyconv)));
				inputSample += (dram->b[c[12]] * (0.02538237753523052f  - (0.00040149119125394f*applyconv)));
				inputSample -= (dram->b[c[13]] * (0.00770737340728377f  + (0.00035462118723555f*applyconv)));
				inputSample += (dram->b[c[14]] * (0.01580706228482803f  - (0.00037563141307594f*applyconv)));
				inputSample += (dram->b[c[15]] * (0.00055119240005586f  - (0.00035409299268971f*applyconv)));
				inputSample += (dram->b[c[16]] * (0.00818552143438768f  - (0.00036507661042180f*applyconv)));
				inputSample += (dram->b[c[17]] * (0.00661842703548304f  - (0.00034550528559056f*applyconv)));
				inputSample += (dram->b[c[18]] * (0.00362447476272098f  - (0.00035553012761240f*applyconv)));
				inputSample += (dram->b[c[19]] * (0.00957098027225745f  - (0.00034091691045338f*applyconv)));
				inputSample += (dram->b[c[20]] * (0.00193621774016660f  - (0.00034554529131668f*applyconv)));
				inputSample += (dram->b[c[21]] * (0.01005433027357935f  - (0.00033878223153845f*applyconv)));
				inputSample += (dram->b[c[22]] * (0.00221712428802004f  - (0.00033481410137711f*applyconv)));
				inputSample += (dram->b[c[23]] * (0.00911255639207995f  - (0.00033263425232666f*applyconv)));
				inputSample += (dram->b[c[24]] * (0.00339667169034909f  - (0.00032634428038430f*applyconv)));
				inputSample += (dram->b[c[25]] * (0.00774096948249924f  - (0.00032599868802996f*applyconv)));
				inputSample += (dram->b[c[26]] * (0.00463907626773794f  - (0.00032131993173361f*applyconv)));
				inputSample += (dram->b[c[27]] * (0.00658222997260378f  - (0.00032014977430211f*applyconv)));
				inputSample += (dram->b[c[28]] * (0.00550347079924993f  - (0.00031557153256653f*applyconv)));
				inputSample += (dram->b[c[29]] * (0.00588754981375325f  - (0.00032041307242303f*applyconv)));
				inputSample += (dram->b[c[30]] * (0.00590293898419892f  - (0.00030457857428714f*applyconv)));
				inputSample += (dram->b[c[31]] * (0.00558952010441800f  - (0.00030448053548086f*applyconv)));
				inputSample += (dram->b[c[32]] * (0.00598183557634295f  - (0.00030715064323181f*applyconv)));
				inputSample += (dram->b[c[33]] * (0.00555223929714115f  - (0.00030319367948553f*applyconv)));
				//end Lush (Neve) conv
				break;
			case 4:
				//begin Elation (LA2A) vibe
				inputSample -= (dram->b[c[1]] * (0.25867935358656502f  - (0.00045755657070112f*applyconv)));
				inputSample += (dram->b[c[2]] * (0.11509367290253694f  - (0.00017494270657228f*applyconv)));
				inputSample -= (dram->b[c[3]] * (0.06709853575891785f  - (0.00058913102597723f*applyconv)));
				inputSample += (dram->b[c[4]] * (0.01871006356851681f  - (0.00003387358004645f*applyconv)));
				inputSample -= (dram->b[c[5]] * (0.00794797957360465f  - (0.00044224784691203f*applyconv)));
				inputSample -= (dram->b[c[6]] * (0.01956921817394220f  - (0.00006718936750076f*applyconv)));
				inputSample += (dram->b[c[7]] * (0.01682120257195205f  + (0.00032857446292230f*applyconv)));
				inputSample -= (dram->b[c[8]] * (0.03401069039824205f  - (0.00013634182872897f*applyconv)));
				inputSample += (dram->b[c[9]] * (0.02369950268232634f  + (0.00023112685751657f*applyconv)));
				inputSample -= (dram->b[c[10]] * (0.03477071178117132f  - (0.00018029792231600f*applyconv)));
				inputSample += (dram->b[c[11]] * (0.02024369717958201f  + (0.00017337813374202f*applyconv)));
				inputSample -= (dram->b[c[12]] * (0.02819087729102172f  - (0.00021438538665420f*applyconv)));
				inputSample += (dram->b[c[13]] * (0.01147946743141303f  + (0.00014424066034649f*applyconv)));
				inputSample -= (dram->b[c[14]] * (0.01894777011468867f  - (0.00021549146262408f*applyconv)));
				inputSample += (dram->b[c[15]] * (0.00301370330346873f  + (0.00013527460148394f*applyconv)));
				inputSample -= (dram->b[c[16]] * (0.01067147835815486f  - (0.00020960689910868f*applyconv)));
				inputSample -= (dram->b[c[17]] * (0.00402715397506384f  - (0.00014421582712470f*applyconv)));
				inputSample -= (dram->b[c[18]] * (0.00502221703392005f  - (0.00019805767015024f*applyconv)));
				inputSample -= (dram->b[c[19]] * (0.00808788533308497f  - (0.00016095444141931f*applyconv)));
				inputSample -= (dram->b[c[20]] * (0.00232696588842683f  - (0.00018384470981829f*applyconv)));
				inputSample -= (dram->b[c[21]] * (0.00943950821324531f  - (0.00017098987347593f*applyconv)));
				inputSample -= (dram->b[c[22]] * (0.00193709517200834f  - (0.00018151995939591f*applyconv)));
				inputSample -= (dram->b[c[23]] * (0.00899713952612659f  - (0.00017385835059948f*applyconv)));
				inputSample -= (dram->b[c[24]] * (0.00280584331659089f  - (0.00017742164162470f*applyconv)));
				inputSample -= (dram->b[c[25]] * (0.00780381001954970f  - (0.00018002500755708f*applyconv)));
				inputSample -= (dram->b[c[26]] * (0.00400370310490333f  - (0.00017471691087957f*applyconv)));
				inputSample -= (dram->b[c[27]] * (0.00661527728186928f  - (0.00018137323370347f*applyconv)));
				inputSample -= (dram->b[c[28]] * (0.00496545526864518f  - (0.00017681872601767f*applyconv)));
				inputSample -= (dram->b[c[29]] * (0.00580728820997532f  - (0.00018186220389790f*applyconv)));
				inputSample -= (dram->b[c[30]] * (0.00549309984725666f  - (0.00017722985399075f*applyconv)));
				inputSample -= (dram->b[c[31]] * (0.00542194777529239f  - (0.00018486900185338f*applyconv)));
				inputSample -= (dram->b[c[32]] * (0.00565992080998939f  - (0.00018005824393118f*applyconv)));
				inputSample -= (dram->b[c[33]] * (0.00532121562846656f  - (0.00018643189636216f*applyconv)));
				//end Elation (LA2A)
				break;
			case 5:
				//begin Precious (Precision 8) Holo
				inputSample += (dram->b[c[1]] * (0.59188440274551890f  - (0.00008361469668405f*applyconv)));
				inputSample -= (dram->b[c[2]] * (0.24439750948076133f  + (0.00002651678396848f*applyconv)));
				inputSample += (dram->b[c[3]] * (0.14109876103205621f  - (0.00000840487181372f*applyconv)));
				inputSample -= (dram->b[c[4]] * (0.10053507128157971f  + (0.00001768100964598f*applyconv)));
				inputSample += (dram->b[c[5]] * (0.05859287880626238f  - (0.00000361398065989f*applyconv)));
				inputSample -= (dram->b[c[6]] * (0.04337406889823660f  + (0.00000735941182117f*applyconv)));
				inputSample += (dram->b[c[7]] * (0.01589900680531097f  + (0.00000207347387987f*applyconv)));
				inputSample -= (dram->b[c[8]] * (0.01087234854973281f  + (0.00000732123412029f*applyconv)));
				inputSample -= (dram->b[c[9]] * (0.00845782429679176f  - (0.00000133058605071f*applyconv)));
				inputSample += (dram->b[c[10]] * (0.00662278586618295f  - (0.00000424594730611f*applyconv)));
				inputSample -= (dram->b[c[11]] * (0.02000592193760155f  + (0.00000632896879068f*applyconv)));
				inputSample += (dram->b[c[12]] * (0.01321157777167565f  - (0.00001421171592570f*applyconv)));
				inputSample -= (dram->b[c[13]] * (0.02249955362988238f  + (0.00000163937127317f*applyconv)));
				inputSample += (dram->b[c[14]] * (0.01196492077581504f  - (0.00000535385220676f*applyconv)));
				inputSample -= (dram->b[c[15]] * (0.01905917427000097f  + (0.00000121672882030f*applyconv)));
				inputSample += (dram->b[c[16]] * (0.00761909482108073f  - (0.00000326242895115f*applyconv)));
				inputSample -= (dram->b[c[17]] * (0.01362744780256239f  + (0.00000359274216003f*applyconv)));
				inputSample += (dram->b[c[18]] * (0.00200183122683721f  - (0.00000089207452791f*applyconv)));
				inputSample -= (dram->b[c[19]] * (0.00833042637239315f  + (0.00000946767677294f*applyconv)));
				inputSample -= (dram->b[c[20]] * (0.00258481175207224f  - (0.00000087429351464f*applyconv)));
				inputSample -= (dram->b[c[21]] * (0.00459744479712244f  - (0.00000049519758701f*applyconv)));
				inputSample -= (dram->b[c[22]] * (0.00534277030993820f  + (0.00000397547847155f*applyconv)));
				inputSample -= (dram->b[c[23]] * (0.00272332919605675f  + (0.00000040077229097f*applyconv)));
				inputSample -= (dram->b[c[24]] * (0.00637243782359372f  - (0.00000139419072176f*applyconv)));
				inputSample -= (dram->b[c[25]] * (0.00233001590327504f  + (0.00000420129915747f*applyconv)));
				inputSample -= (dram->b[c[26]] * (0.00623296727793041f  + (0.00000019010664856f*applyconv)));
				inputSample -= (dram->b[c[27]] * (0.00276177096376805f  + (0.00000580301901385f*applyconv)));
				inputSample -= (dram->b[c[28]] * (0.00559184754866264f  + (0.00000080597287792f*applyconv)));
				inputSample -= (dram->b[c[29]] * (0.00343180144395919f  - (0.00000243701142085f*applyconv)));
				inputSample -= (dram->b[c[30]] * (0.00493325428861701f  + (0.00000300985740900f*applyconv)));
				inputSample -= (dram->b[c[31]] * (0.00396140827680823f  - (0.00000051459681789f*applyconv)));
				inputSample -= (dram->b[c[32]] * (0.00448497879902493f  + (0.00000744412841743f*applyconv)));
				inputSample -= (dram->b[c[33]] * (0.00425146888772076f  - (0.00000082346016542f*applyconv)));
				//end Precious (Precision 8) Holo
				break;
			case 6:
				//begin Punch (API) conv
				inputSample += (dram->b[c[1]] * (0.09299870608542582f  - (0.00009582362368873f*applyconv)));
				inputSample -= (dram->b[c[2]] * (0.11947847710741009f  - (0.00004500891602770f*applyconv)));
				inputSample += (dram->b[c[3]] * (0.09071606264761795f  + (0.00005639498984741f*applyconv)));
				inputSample -= (dram->b[c[4]] * (0.08561982770836980f  - (0.00004964855606916f*applyconv)));
				inputSample += (dram->b[c[5]] * (0.06440549220820363f  + (0.00002428052139507f*applyconv)));
				inputSample -= (dram->b[c[6]] * (0.05987991812840746f  + (0.00000101867082290f*applyconv)));
				inputSample += (dram->b[c[7]] * (0.03980233135839382f  + (0.00003312430049041f*applyconv)));
				inputSample -= (dram->b[c[8]] * (0.03648402630896925f  - (0.00002116186381142f*applyconv)));
				inputSample += (dram->b[c[9]] * (0.01826860869525248f  + (0.00003115110025396f*applyconv)));
				inputSample -= (dram->b[c[10]] * (0.01723968622495364f  - (0.00002450634121718f*applyconv)));
				inputSample += (dram->b[c[11]] * (0.00187588812316724f  + (0.00002838206198968f*applyconv)));
				inputSample -= (dram->b[c[12]] * (0.00381796423957237f  - (0.00003155815499462f*applyconv)));
				inputSample -= (dram->b[c[13]] * (0.00852092214496733f  - (0.00001702651162392f*applyconv)));
				inputSample += (dram->b[c[14]] * (0.00315560292270588f  + (0.00002547861676047f*applyconv)));
				inputSample -= (dram->b[c[15]] * (0.01258630914496868f  - (0.00004555319243213f*applyconv)));
				inputSample += (dram->b[c[16]] * (0.00536435648963575f  + (0.00001812393657101f*applyconv)));
				inputSample -= (dram->b[c[17]] * (0.01272975658159178f  - (0.00004103775306121f*applyconv)));
				inputSample += (dram->b[c[18]] * (0.00403818975172755f  + (0.00003764615492871f*applyconv)));
				inputSample -= (dram->b[c[19]] * (0.01042617366897483f  - (0.00003605210426041f*applyconv)));
				inputSample += (dram->b[c[20]] * (0.00126599583390057f  + (0.00004305458668852f*applyconv)));
				inputSample -= (dram->b[c[21]] * (0.00747876207688339f  - (0.00003731207018977f*applyconv)));
				inputSample -= (dram->b[c[22]] * (0.00149873689175324f  - (0.00005086601800791f*applyconv)));
				inputSample -= (dram->b[c[23]] * (0.00503221309488033f  - (0.00003636086782783f*applyconv)));
				inputSample -= (dram->b[c[24]] * (0.00342998224655821f  - (0.00004103091180506f*applyconv)));
				inputSample -= (dram->b[c[25]] * (0.00355585977903117f  - (0.00003698982145400f*applyconv)));
				inputSample -= (dram->b[c[26]] * (0.00437201792934817f  - (0.00002720235666939f*applyconv)));
				inputSample -= (dram->b[c[27]] * (0.00299217874451556f  - (0.00004446954727956f*applyconv)));
				inputSample -= (dram->b[c[28]] * (0.00457924652487249f  - (0.00003859065778860f*applyconv)));
				inputSample -= (dram->b[c[29]] * (0.00298182934892027f  - (0.00002064710931733f*applyconv)));
				inputSample -= (dram->b[c[30]] * (0.00438838441540584f  - (0.00005223008424866f*applyconv)));
				inputSample -= (dram->b[c[31]] * (0.00323984218794705f  - (0.00003397987535887f*applyconv)));
				inputSample -= (dram->b[c[32]] * (0.00407693981307314f  - (0.00003935772436894f*applyconv)));
				inputSample -= (dram->b[c[33]] * (0.00350435348467321f  - (0.00005525463935338f*applyconv)));
				//end Punch (API) conv
				break;
			case 7:
				//begin Calibre (?) steel
				inputSample -= (dram->b[c[1]] * (0.23505923670562212f  - (0.00028312859289245f*applyconv)));
				inputSample += (dram->b[c[2]] * (0.08188436704577637f  - (0.00008817721351341f*applyconv)));
				inputSample -= (dram->b[c[3]] * (0.05075798481700617f  - (0.00018817166632483f*applyconv)));
				inputSample -= (dram->b[c[4]] * (0.00455811821873093f  + (0.00001922902995296f*applyconv)));
				inputSample -= (dram->b[c[5]] * (0.00027610521433660f  - (0.00013252525469291f*applyconv)));
				inputSample -= (dram->b[c[6]] * (0.03529246280346626f  - (0.00002772989223299f*applyconv)));
				inputSample += (dram->b[c[7]] * (0.01784111585586136f  + (0.00010230276997291f*applyconv)));
				inputSample -= (dram->b[c[8]] * (0.04394950700298298f  - (0.00005910607126944f*applyconv)));
				inputSample += (dram->b[c[9]] * (0.01990770780547606f  + (0.00007640328340556f*applyconv)));
				inputSample -= (dram->b[c[10]] * (0.04073629569741782f  - (0.00007712327117090f*applyconv)));
				inputSample += (dram->b[c[11]] * (0.01349648572795252f  + (0.00005959130575917f*applyconv)));
				inputSample -= (dram->b[c[12]] * (0.03191590248003717f  - (0.00008418000575151f*applyconv)));
				inputSample += (dram->b[c[13]] * (0.00348795527924766f  + (0.00005489156318238f*applyconv)));
				inputSample -= (dram->b[c[14]] * (0.02198496281481767f  - (0.00008471601187581f*applyconv)));
				inputSample -= (dram->b[c[15]] * (0.00504771152505089f  - (0.00005525060587917f*applyconv)));
				inputSample -= (dram->b[c[16]] * (0.01391075698598491f  - (0.00007929630732607f*applyconv)));
				inputSample -= (dram->b[c[17]] * (0.01142762504081717f  - (0.00005967036737742f*applyconv)));
				inputSample -= (dram->b[c[18]] * (0.00893541815021255f  - (0.00007535697758141f*applyconv)));
				inputSample -= (dram->b[c[19]] * (0.01459704973464936f  - (0.00005969199602841f*applyconv)));
				inputSample -= (dram->b[c[20]] * (0.00694755135226282f  - (0.00006930127097865f*applyconv)));
				inputSample -= (dram->b[c[21]] * (0.01516695630808575f  - (0.00006365800069826f*applyconv)));
				inputSample -= (dram->b[c[22]] * (0.00705917318113651f  - (0.00006497209096539f*applyconv)));
				inputSample -= (dram->b[c[23]] * (0.01420501209177591f  - (0.00006555654576113f*applyconv)));
				inputSample -= (dram->b[c[24]] * (0.00815905656808701f  - (0.00006105622534761f*applyconv)));
				inputSample -= (dram->b[c[25]] * (0.01274326525552961f  - (0.00006542652857017f*applyconv)));
				inputSample -= (dram->b[c[26]] * (0.00937146927845488f  - (0.00006051267868722f*applyconv)));
				inputSample -= (dram->b[c[27]] * (0.01146573981165209f  - (0.00006381511607749f*applyconv)));
				inputSample -= (dram->b[c[28]] * (0.01021294359409007f  - (0.00005930397856398f*applyconv)));
				inputSample -= (dram->b[c[29]] * (0.01065217095323532f  - (0.00006371505438319f*applyconv)));
				inputSample -= (dram->b[c[30]] * (0.01058751196699751f  - (0.00006042857480233f*applyconv)));
				inputSample -= (dram->b[c[31]] * (0.01026557827762401f  - (0.00006007776163871f*applyconv)));
				inputSample -= (dram->b[c[32]] * (0.01060929183604604f  - (0.00006114703012726f*applyconv)));
				inputSample -= (dram->b[c[33]] * (0.01014533525058528f  - (0.00005963567932887f*applyconv)));
				//end Calibre (?)
				break;
			case 8:
				//begin Tube (Manley) conv
				inputSample -= (dram->b[c[1]] * (0.20641602693167951f  - (0.00078952185394898f*applyconv)));
				inputSample += (dram->b[c[2]] * (0.07601816702459827f  + (0.00022786334179951f*applyconv)));
				inputSample -= (dram->b[c[3]] * (0.03929765560019285f  - (0.00054517993246352f*applyconv)));
				inputSample -= (dram->b[c[4]] * (0.00298333157711103f  - (0.00033083756545638f*applyconv)));
				inputSample += (dram->b[c[5]] * (0.00724006282304610f  + (0.00045483683460812f*applyconv)));
				inputSample -= (dram->b[c[6]] * (0.03073108963506036f  - (0.00038190060537423f*applyconv)));
				inputSample += (dram->b[c[7]] * (0.02332434692533051f  + (0.00040347288688932f*applyconv)));
				inputSample -= (dram->b[c[8]] * (0.03792606869061214f  - (0.00039673687335892f*applyconv)));
				inputSample += (dram->b[c[9]] * (0.02437059376675688f  + (0.00037221210539535f*applyconv)));
				inputSample -= (dram->b[c[10]] * (0.03416764311979521f  - (0.00040314850796953f*applyconv)));
				inputSample += (dram->b[c[11]] * (0.01761669868102127f  + (0.00035989484330131f*applyconv)));
				inputSample -= (dram->b[c[12]] * (0.02538237753523052f  - (0.00040149119125394f*applyconv)));
				inputSample += (dram->b[c[13]] * (0.00770737340728377f  + (0.00035462118723555f*applyconv)));
				inputSample -= (dram->b[c[14]] * (0.01580706228482803f  - (0.00037563141307594f*applyconv)));
				inputSample -= (dram->b[c[15]] * (0.00055119240005586f  - (0.00035409299268971f*applyconv)));
				inputSample -= (dram->b[c[16]] * (0.00818552143438768f  - (0.00036507661042180f*applyconv)));
				inputSample -= (dram->b[c[17]] * (0.00661842703548304f  - (0.00034550528559056f*applyconv)));
				inputSample -= (dram->b[c[18]] * (0.00362447476272098f  - (0.00035553012761240f*applyconv)));
				inputSample -= (dram->b[c[19]] * (0.00957098027225745f  - (0.00034091691045338f*applyconv)));
				inputSample -= (dram->b[c[20]] * (0.00193621774016660f  - (0.00034554529131668f*applyconv)));
				inputSample -= (dram->b[c[21]] * (0.01005433027357935f  - (0.00033878223153845f*applyconv)));
				inputSample -= (dram->b[c[22]] * (0.00221712428802004f  - (0.00033481410137711f*applyconv)));
				inputSample -= (dram->b[c[23]] * (0.00911255639207995f  - (0.00033263425232666f*applyconv)));
				inputSample -= (dram->b[c[24]] * (0.00339667169034909f  - (0.00032634428038430f*applyconv)));
				inputSample -= (dram->b[c[25]] * (0.00774096948249924f  - (0.00032599868802996f*applyconv)));
				inputSample -= (dram->b[c[26]] * (0.00463907626773794f  - (0.00032131993173361f*applyconv)));
				inputSample -= (dram->b[c[27]] * (0.00658222997260378f  - (0.00032014977430211f*applyconv)));
				inputSample -= (dram->b[c[28]] * (0.00550347079924993f  - (0.00031557153256653f*applyconv)));
				inputSample -= (dram->b[c[29]] * (0.00588754981375325f  - (0.00032041307242303f*applyconv)));
				inputSample -= (dram->b[c[30]] * (0.00590293898419892f  - (0.00030457857428714f*applyconv)));
				inputSample -= (dram->b[c[31]] * (0.00558952010441800f  - (0.00030448053548086f*applyconv)));
				inputSample -= (dram->b[c[32]] * (0.00598183557634295f  - (0.00030715064323181f*applyconv)));
				inputSample -= (dram->b[c[33]] * (0.00555223929714115f  - (0.00030319367948553f*applyconv)));
				//end Tube (Manley) conv
				break;
		}
				
		bridgerectifier = fabs(inputSample);
		bridgerectifier = 1.0f-cos(bridgerectifier);
		if (inputSample > 0) inputSample -= bridgerectifier;
		else inputSample += bridgerectifier;
		
		if (outgain != 1.0f) {
			inputSample *= outgain;
		}
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int count = 0; count < 174; count++) dram->b[count] = 0;
	for (int count = 0; count < 99; count++) dram->d[count] = 0;
	for (int count = 0; count < 34; count++) c[count] = count; //initial setup for 44.1K
	g[1] = pow(10.0, -5.2 / 14.0); //dark
	g[2] = pow(10.0, -6.2 / 14.0); //rock
	g[3] = pow(10.0, -2.9 / 14.0); //lush
	g[4] = pow(10.0, -1.1 / 14.0); //vibe
	g[5] = pow(10.0, -5.1 / 14.0); //holo
	g[6] = pow(10.0, -3.6 / 14.0); //punch
	g[7] = pow(10.0, -2.3 / 14.0); //steel
	g[8] = pow(10.0, -2.9 / 14.0); //tube
	//preset gains for models
	outg[1] = pow(10.0, -0.3 / 14.0); //dark
	outg[2] = pow(10.0, 0.5 / 14.0); //rock
	outg[3] = pow(10.0, -0.7 / 14.0); //lush
	outg[4] = pow(10.0, -0.6 / 14.0); //vibe
	outg[5] = pow(10.0, -0.2 / 14.0); //holo
	outg[6] = pow(10.0, 0.3 / 14.0); //punch
	outg[7] = pow(10.0, 0.1 / 14.0); //steel
	outg[8] = pow(10.0, 0.9 / 14.0); //tube
	//preset gains for models
	control = 0;
	gcount = 0;
	slowdyn = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
