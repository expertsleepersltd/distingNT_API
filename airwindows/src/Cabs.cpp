#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Cabs"
#define AIRWINDOWS_DESCRIPTION "An Airwindows guitar speaker cabinet simulator."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','a','s' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
static const int kHighPowerStack = 1;
static const int kVintageStack = 2;
static const int kBoutiqueStack = 3;
static const int kLargeCombo = 4;
static const int kSmallCombo = 5;
static const int kBass = 6;
static const int kDefaultValue_ParamOne = kHighPowerStack;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static char const * const enumStrings0[] = { "", "High Power Stack", "Vintage Stack", "Boutique Stack", "Large Combo", "Small Combo", "Bass Amp", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Cab Type", .min = 1, .max = 6, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "Intensity", .min = 0, .max = 1000, .def = 660, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Rm Loud", .min = 0, .max = 1000, .def = 330, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Cab Size", .min = 0, .max = 1000, .def = 660, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Off Axis", .min = 0, .max = 1000, .def = 330, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 ataLast3Sample;
		Float32 ataLast2Sample;
		Float32 ataLast1Sample;
		Float32 ataHalfwaySample;
		Float32 ataHalfDrySample;
		Float32 ataHalfDiffSample;
		Float32 ataA;
		Float32 ataB;
		Float32 ataC;
		Float32 ataDrySample;
		Float32 ataDiffSample;
		Float32 ataPrevDiffSample;
		Float32 lastSample;
		Float32 lastHalfSample;
		Float32 lastPostSample;
		Float32 lastPostHalfSample;
		Float32 postPostSample;
		Float32 control;
		Float32 iirHeadBumpA;
		Float32 iirHeadBumpB;
		Float32 iirHalfHeadBumpA;
		Float32 iirHalfHeadBumpB;
		float lastRef[7];
		
		bool flip;
		bool ataFlip;
		int gcount;
		int cycle;
		
		uint32_t fpd;
	
	struct _dram {
			Float32 b[90];
		Float32 d[21];
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
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check
	
	int speaker = (int) GetParameter( kParam_One );
	float colorIntensity = powf(GetParameter( kParam_Two ),4);
	float correctboost = 1.0f + (colorIntensity*4);
	float correctdrygain = 1.0f - colorIntensity;
	float threshold = powf((1.0f-GetParameter( kParam_Three )),5)+0.021f; //room loud is slew
	float rarefaction = cbrt(threshold);
	float postThreshold = sqrt(rarefaction);
	float postRarefaction = cbrt(postThreshold);
	float postTrim = sqrt(postRarefaction);
	float HeadBumpFreq = 0.0298f+((1.0f-GetParameter( kParam_Four ))/8.0f);
	float LowsPad = 0.12f + (HeadBumpFreq*12.0f);
	float dcblock = powf(HeadBumpFreq,2) / 8.0f;
	float heavy = powf(GetParameter( kParam_Five ),3); //wet of head bump
	float output = powf(GetParameter( kParam_Six ),2);
	float dynamicconv = 5.0f;
	dynamicconv *= powf(GetParameter( kParam_Two ),2);
	dynamicconv *= powf(GetParameter( kParam_Three ),2);
	//set constants for sag speed
	int offsetA = 4+((int)(GetParameter( kParam_Four )*5.0f));

	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a chorus sample
			//everything in here is undersampled, including the dry/wet		
			
			float ataDrySample = inputSample;		
			float ataHalfwaySample = (inputSample + ataLast1Sample + ((-ataLast2Sample + ataLast3Sample) * 0.05f)) / 2.0f;
			float ataHalfDrySample = ataHalfwaySample;
			ataLast3Sample = ataLast2Sample; ataLast2Sample = ataLast1Sample; ataLast1Sample = inputSample;
			//setting up oversampled special antialiasing
			//pre-center code on inputSample and halfwaySample in parallel
			//begin raw sample- inputSample and ataDrySample handled separately here
			float clamp = inputSample - lastSample;
			if (clamp > threshold) inputSample = lastSample + threshold;
			if (-clamp > rarefaction) inputSample = lastSample - rarefaction;
			lastSample = inputSample;
			//end raw sample
			
			//begin interpolated sample- change inputSample -> ataHalfwaySample, ataDrySample -> ataHalfDrySample
			clamp = ataHalfwaySample - lastHalfSample;
			if (clamp > threshold) ataHalfwaySample = lastHalfSample + threshold;
			if (-clamp > rarefaction) ataHalfwaySample = lastHalfSample - rarefaction;
			lastHalfSample = ataHalfwaySample;
			//end interpolated sample
			
			//begin center code handling conv stuff tied to 44.1K, or stuff in time domain like delays
			ataHalfwaySample -= inputSample;
			//retain only difference with raw signal
			
			if (gcount < 0 || gcount > 10) {gcount = 10;}
			dram->d[gcount+10] = dram->d[gcount] = fabs(inputSample);
			control += (dram->d[gcount] / offsetA);
			control -= (dram->d[gcount+offsetA] / offsetA);
			control -= 0.0001f;				
			if (control < 0) {control = 0;}
			if (control > 13) {control = 13;}
			gcount--;
			float applyconv = (control / offsetA) * dynamicconv;
			//now we have a 'sag' style average to apply to the conv
			dram->b[82] = dram->b[81]; dram->b[81] = dram->b[80]; dram->b[80] = dram->b[79]; 
			dram->b[79] = dram->b[78]; dram->b[78] = dram->b[77]; dram->b[77] = dram->b[76]; dram->b[76] = dram->b[75]; dram->b[75] = dram->b[74]; dram->b[74] = dram->b[73]; dram->b[73] = dram->b[72]; dram->b[72] = dram->b[71]; 
			dram->b[71] = dram->b[70]; dram->b[70] = dram->b[69]; dram->b[69] = dram->b[68]; dram->b[68] = dram->b[67]; dram->b[67] = dram->b[66]; dram->b[66] = dram->b[65]; dram->b[65] = dram->b[64]; dram->b[64] = dram->b[63]; 
			dram->b[63] = dram->b[62]; dram->b[62] = dram->b[61]; dram->b[61] = dram->b[60]; dram->b[60] = dram->b[59]; dram->b[59] = dram->b[58]; dram->b[58] = dram->b[57]; dram->b[57] = dram->b[56]; dram->b[56] = dram->b[55]; 
			dram->b[55] = dram->b[54]; dram->b[54] = dram->b[53]; dram->b[53] = dram->b[52]; dram->b[52] = dram->b[51]; dram->b[51] = dram->b[50]; dram->b[50] = dram->b[49]; dram->b[49] = dram->b[48]; dram->b[48] = dram->b[47]; 
			dram->b[47] = dram->b[46]; dram->b[46] = dram->b[45]; dram->b[45] = dram->b[44]; dram->b[44] = dram->b[43]; dram->b[43] = dram->b[42]; dram->b[42] = dram->b[41]; dram->b[41] = dram->b[40]; dram->b[40] = dram->b[39]; 
			dram->b[39] = dram->b[38]; dram->b[38] = dram->b[37]; dram->b[37] = dram->b[36]; dram->b[36] = dram->b[35]; dram->b[35] = dram->b[34]; dram->b[34] = dram->b[33]; dram->b[33] = dram->b[32]; dram->b[32] = dram->b[31]; 
			dram->b[31] = dram->b[30]; dram->b[30] = dram->b[29]; dram->b[29] = dram->b[28]; dram->b[28] = dram->b[27]; dram->b[27] = dram->b[26]; dram->b[26] = dram->b[25]; dram->b[25] = dram->b[24]; dram->b[24] = dram->b[23]; 
			dram->b[23] = dram->b[22]; dram->b[22] = dram->b[21]; dram->b[21] = dram->b[20]; dram->b[20] = dram->b[19]; dram->b[19] = dram->b[18]; dram->b[18] = dram->b[17]; dram->b[17] = dram->b[16]; dram->b[16] = dram->b[15]; 
			dram->b[15] = dram->b[14]; dram->b[14] = dram->b[13]; dram->b[13] = dram->b[12]; dram->b[12] = dram->b[11]; dram->b[11] = dram->b[10]; dram->b[10] = dram->b[9]; dram->b[9] = dram->b[8]; dram->b[8] = dram->b[7]; 
			dram->b[7] = dram->b[6]; dram->b[6] = dram->b[5]; dram->b[5] = dram->b[4]; dram->b[4] = dram->b[3]; dram->b[3] = dram->b[2]; dram->b[2] = dram->b[1]; dram->b[1] = dram->b[0]; dram->b[0] = inputSample;
			//load conv
			
			float tempSample = 0.0f; //set up for applying the cab sound
			switch (speaker)
			{
				case 1:
					//begin HighPowerStack conv
					tempSample += (dram->b[1] * (1.29550481610475132f  + (0.19713872057074355f*applyconv)));
					tempSample += (dram->b[2] * (1.42302569895462616f  + (0.30599505521284787f*applyconv)));
					tempSample += (dram->b[3] * (1.28728195804197565f  + (0.23168333460446133f*applyconv)));
					tempSample += (dram->b[4] * (0.88553784290822690f  + (0.14263256172918892f*applyconv)));
					tempSample += (dram->b[5] * (0.37129054918432319f  + (0.00150040944205920f*applyconv)));
					tempSample -= (dram->b[6] * (0.12150959412556320f  + (0.32776273620569107f*applyconv)));
					tempSample -= (dram->b[7] * (0.44900065463203775f  + (0.74101214925298819f*applyconv)));
					tempSample -= (dram->b[8] * (0.54058781908186482f  + (1.07821707459008387f*applyconv)));
					tempSample -= (dram->b[9] * (0.49361966401791391f  + (1.23540109014850508f*applyconv)));
					tempSample -= (dram->b[10] * (0.39819495093078133f  + (1.11247213730917749f*applyconv)));
					tempSample -= (dram->b[11] * (0.31379279985435521f  + (0.80330360359638298f*applyconv)));
					tempSample -= (dram->b[12] * (0.30744359242808555f  + (0.42132528876858205f*applyconv)));
					tempSample -= (dram->b[13] * (0.33943170284673974f  + (0.09183418349389982f*applyconv)));
					tempSample -= (dram->b[14] * (0.33838775119286391f  - (0.06453051658561271f*applyconv)));
					tempSample -= (dram->b[15] * (0.30682305697961665f  - (0.09549380253249232f*applyconv)));
					tempSample -= (dram->b[16] * (0.23408741339295336f  - (0.08083404732361277f*applyconv)));
					tempSample -= (dram->b[17] * (0.10411746814025019f  + (0.00253651281245780f*applyconv)));
					tempSample += (dram->b[18] * (0.00133623776084696f  - (0.04447267870865820f*applyconv)));
					tempSample += (dram->b[19] * (0.02461903992114161f  + (0.07530671732655550f*applyconv)));
					tempSample += (dram->b[20] * (0.02086715842475373f  + (0.22795860236804899f*applyconv)));
					tempSample += (dram->b[21] * (0.02761433637100917f  + (0.26108320417844094f*applyconv)));
					tempSample += (dram->b[22] * (0.04475285369162533f  + (0.19160705011061663f*applyconv)));
					tempSample += (dram->b[23] * (0.09447338372862381f  + (0.03681550508743799f*applyconv)));
					tempSample += (dram->b[24] * (0.13445890343722280f  - (0.13713036462146147f*applyconv)));
					tempSample += (dram->b[25] * (0.13872868945088121f  - (0.22401242373298191f*applyconv)));
					tempSample += (dram->b[26] * (0.14915650097434549f  - (0.26718804981526367f*applyconv)));
					tempSample += (dram->b[27] * (0.12766643217091783f  - (0.27745664795660430f*applyconv)));
					tempSample += (dram->b[28] * (0.03675849788393101f  - (0.18338278173550679f*applyconv)));
					tempSample -= (dram->b[29] * (0.06307306864232835f  + (0.06089480869040766f*applyconv)));
					tempSample -= (dram->b[30] * (0.14947389348962944f  + (0.04642103054798480f*applyconv)));
					tempSample -= (dram->b[31] * (0.25235266566401526f  + (0.08423062596460507f*applyconv)));
					tempSample -= (dram->b[32] * (0.33496344048679683f  + (0.09808328256677995f*applyconv)));
					tempSample -= (dram->b[33] * (0.36590030482175445f  + (0.10622650888958179f*applyconv)));
					tempSample -= (dram->b[34] * (0.35015197011464372f  + (0.08982043516016047f*applyconv)));
					tempSample -= (dram->b[35] * (0.26808437585665090f  + (0.00735561860229533f*applyconv)));
					tempSample -= (dram->b[36] * (0.11624318543291220f  - (0.07142484314510467f*applyconv)));
					tempSample += (dram->b[37] * (0.05617084165377551f  + (0.11785854050350089f*applyconv)));
					tempSample += (dram->b[38] * (0.20540028692589385f  + (0.20479174663329586f*applyconv)));
					tempSample += (dram->b[39] * (0.30455415003043818f  + (0.29074864580096849f*applyconv)));
					tempSample += (dram->b[40] * (0.33810750937829476f  + (0.29182307921316802f*applyconv)));
					tempSample += (dram->b[41] * (0.31936133365277430f  + (0.26535537727394987f*applyconv)));
					tempSample += (dram->b[42] * (0.27388548321981876f  + (0.19735049990538350f*applyconv)));
					tempSample += (dram->b[43] * (0.21454597517994098f  + (0.06415909270247236f*applyconv)));
					tempSample += (dram->b[44] * (0.15001045817707717f  - (0.03831118543404573f*applyconv)));
					tempSample += (dram->b[45] * (0.07283437284653138f  - (0.09281952429543777f*applyconv)));
					tempSample -= (dram->b[46] * (0.03917872184241358f  + (0.14306291461398810f*applyconv)));
					tempSample -= (dram->b[47] * (0.16695932032148642f  + (0.19138995946950504f*applyconv)));
					tempSample -= (dram->b[48] * (0.27055854466909462f  + (0.22531296466343192f*applyconv)));
					tempSample -= (dram->b[49] * (0.33256357307578271f  + (0.23305840475692102f*applyconv)));
					tempSample -= (dram->b[50] * (0.33459770116834442f  + (0.24091822618917569f*applyconv)));
					tempSample -= (dram->b[51] * (0.27156687236338090f  + (0.24062938573512443f*applyconv)));
					tempSample -= (dram->b[52] * (0.17197093288412094f  + (0.19083085091993421f*applyconv)));
					tempSample -= (dram->b[53] * (0.06738628195910543f  + (0.10268609751019808f*applyconv)));
					tempSample += (dram->b[54] * (0.00222429218204290f  + (0.01439664435720548f*applyconv)));
					tempSample += (dram->b[55] * (0.01346992803494091f  + (0.15947137113534526f*applyconv)));
					tempSample -= (dram->b[56] * (0.02038911881377448f  - (0.26763170752416160f*applyconv)));
					tempSample -= (dram->b[57] * (0.08233579178189687f  - (0.29415931086406055f*applyconv)));
					tempSample -= (dram->b[58] * (0.15447855089824883f  - (0.26489186990840807f*applyconv)));
					tempSample -= (dram->b[59] * (0.20518281113362655f  - (0.16135382257522859f*applyconv)));
					tempSample -= (dram->b[60] * (0.22244686050232007f  + (0.00847180390247432f*applyconv)));
					tempSample -= (dram->b[61] * (0.21849243134998034f  + (0.14460595245753741f*applyconv)));
					tempSample -= (dram->b[62] * (0.20256105734574054f  + (0.18932793221831667f*applyconv)));
					tempSample -= (dram->b[63] * (0.18604070054295399f  + (0.17250665610927965f*applyconv)));
					tempSample -= (dram->b[64] * (0.17222844322058231f  + (0.12992472027850357f*applyconv)));
					tempSample -= (dram->b[65] * (0.14447856616566443f  + (0.09089219002147308f*applyconv)));
					tempSample -= (dram->b[66] * (0.10385520794251019f  + (0.08600465834570559f*applyconv)));
					tempSample -= (dram->b[67] * (0.07124435678265063f  + (0.09071532210549428f*applyconv)));
					tempSample -= (dram->b[68] * (0.05216857461197572f  + (0.06794061706070262f*applyconv)));
					tempSample -= (dram->b[69] * (0.05235381920184123f  + (0.02818101717909346f*applyconv)));
					tempSample -= (dram->b[70] * (0.07569701245553526f  - (0.00634228544764946f*applyconv)));
					tempSample -= (dram->b[71] * (0.10320125382718826f  - (0.02751486906644141f*applyconv)));
					tempSample -= (dram->b[72] * (0.12122120969079088f  - (0.05434007312178933f*applyconv)));
					tempSample -= (dram->b[73] * (0.13438969117200902f  - (0.09135218559713874f*applyconv)));
					tempSample -= (dram->b[74] * (0.13534390437529981f  - (0.10437672041458675f*applyconv)));
					tempSample -= (dram->b[75] * (0.11424128854188388f  - (0.08693450726462598f*applyconv)));
					tempSample -= (dram->b[76] * (0.08166894518596159f  - (0.06949989431475120f*applyconv)));
					tempSample -= (dram->b[77] * (0.04293976378555305f  - (0.05718625137421843f*applyconv)));
					tempSample += (dram->b[78] * (0.00933076320644409f  + (0.01728285211520138f*applyconv)));
					tempSample += (dram->b[79] * (0.06450430362918153f  - (0.02492994833691022f*applyconv)));
					tempSample += (dram->b[80] * (0.10187400687649277f  - (0.03578455940532403f*applyconv)));
					tempSample += (dram->b[81] * (0.11039763294094571f  - (0.03995523517573508f*applyconv)));
					tempSample += (dram->b[82] * (0.08557960776024547f  - (0.03482514309492527f*applyconv)));
					tempSample += (dram->b[83] * (0.02730881850805332f  - (0.00514750108411127f*applyconv)));
					//end HighPowerStack conv
					break;
				case 2:
					//begin VintageStack conv
					tempSample += (dram->b[1] * (1.31698250313308396f  - (0.08140616497621633f*applyconv)));
					tempSample += (dram->b[2] * (1.47229016949915326f  - (0.27680278993637253f*applyconv)));
					tempSample += (dram->b[3] * (1.30410109086044956f  - (0.35629113432046489f*applyconv)));
					tempSample += (dram->b[4] * (0.81766210474551260f  - (0.26808782337659753f*applyconv)));
					tempSample += (dram->b[5] * (0.19868872545506663f  - (0.11105517193919669f*applyconv)));
					tempSample -= (dram->b[6] * (0.39115909132567039f  - (0.12630622002682679f*applyconv)));
					tempSample -= (dram->b[7] * (0.76881891559343574f  - (0.40879849500403143f*applyconv)));
					tempSample -= (dram->b[8] * (0.87146861782680340f  - (0.59529560488000599f*applyconv)));
					tempSample -= (dram->b[9] * (0.79504575932563670f  - (0.60877047551611796f*applyconv)));
					tempSample -= (dram->b[10] * (0.61653017622406314f  - (0.47662851438557335f*applyconv)));
					tempSample -= (dram->b[11] * (0.40718195794382067f  - (0.24955839378539713f*applyconv)));
					tempSample -= (dram->b[12] * (0.31794900040616203f  - (0.04169792259600613f*applyconv)));
					tempSample -= (dram->b[13] * (0.41075032540217843f  + (0.00368483996076280f*applyconv)));
					tempSample -= (dram->b[14] * (0.56901352922170667f  - (0.11027360805893105f*applyconv)));
					tempSample -= (dram->b[15] * (0.62443222391889264f  - (0.22198075154245228f*applyconv)));
					tempSample -= (dram->b[16] * (0.53462856723129204f  - (0.22933544545324852f*applyconv)));
					tempSample -= (dram->b[17] * (0.34441703361995046f  - (0.12956809502269492f*applyconv)));
					tempSample -= (dram->b[18] * (0.13947052337867882f  + (0.00339775055962799f*applyconv)));
					tempSample += (dram->b[19] * (0.03771252648928484f  - (0.10863931549251718f*applyconv)));
					tempSample += (dram->b[20] * (0.18280210770271693f  - (0.17413646599296417f*applyconv)));
					tempSample += (dram->b[21] * (0.24621986701761467f  - (0.14547053270435095f*applyconv)));
					tempSample += (dram->b[22] * (0.22347075142737360f  - (0.02493869490104031f*applyconv)));
					tempSample += (dram->b[23] * (0.14346348482123716f  + (0.11284054747963246f*applyconv)));
					tempSample += (dram->b[24] * (0.00834364862916028f  + (0.24284684053733926f*applyconv)));
					tempSample -= (dram->b[25] * (0.11559740296078347f  - (0.32623054435304538f*applyconv)));
					tempSample -= (dram->b[26] * (0.18067604561283060f  - (0.32311481551122478f*applyconv)));
					tempSample -= (dram->b[27] * (0.22927997789035612f  - (0.26991539052832925f*applyconv)));
					tempSample -= (dram->b[28] * (0.28487666578669446f  - (0.22437227250279349f*applyconv)));
					tempSample -= (dram->b[29] * (0.31992973037153838f  - (0.15289876100963865f*applyconv)));
					tempSample -= (dram->b[30] * (0.35174606303520733f  - (0.05656293023086628f*applyconv)));
					tempSample -= (dram->b[31] * (0.36894898011375254f  + (0.04333925421463558f*applyconv)));
					tempSample -= (dram->b[32] * (0.32567576055307507f  + (0.14594589410921388f*applyconv)));
					tempSample -= (dram->b[33] * (0.27440135050585784f  + (0.15529667398122521f*applyconv)));
					tempSample -= (dram->b[34] * (0.21998973785078091f  + (0.05083553737157104f*applyconv)));
					tempSample -= (dram->b[35] * (0.10323624876862457f  - (0.04651829594199963f*applyconv)));
					tempSample += (dram->b[36] * (0.02091603687851074f  + (0.12000046818439322f*applyconv)));
					tempSample += (dram->b[37] * (0.11344930914138468f  + (0.17697142512225839f*applyconv)));
					tempSample += (dram->b[38] * (0.22766779627643968f  + (0.13645102964003858f*applyconv)));
					tempSample += (dram->b[39] * (0.38378309953638229f  - (0.01997653307333791f*applyconv)));
					tempSample += (dram->b[40] * (0.52789400804568076f  - (0.21409137428422448f*applyconv)));
					tempSample += (dram->b[41] * (0.55444630296938280f  - (0.32331980931576626f*applyconv)));
					tempSample += (dram->b[42] * (0.42333237669264601f  - (0.26855847463044280f*applyconv)));
					tempSample += (dram->b[43] * (0.21942831522035078f  - (0.12051365248820624f*applyconv)));
					tempSample -= (dram->b[44] * (0.00584169427830633f  - (0.03706970171280329f*applyconv)));
					tempSample -= (dram->b[45] * (0.24279799124660351f  - (0.17296440491477982f*applyconv)));
					tempSample -= (dram->b[46] * (0.40173760787507085f  - (0.21717989835163351f*applyconv)));
					tempSample -= (dram->b[47] * (0.43930035724188155f  - (0.16425928481378199f*applyconv)));
					tempSample -= (dram->b[48] * (0.41067765934041811f  - (0.10390115786636855f*applyconv)));
					tempSample -= (dram->b[49] * (0.34409235547165967f  - (0.07268159377411920f*applyconv)));
					tempSample -= (dram->b[50] * (0.26542883122568151f  - (0.05483457497365785f*applyconv)));
					tempSample -= (dram->b[51] * (0.22024754776138800f  - (0.06484897950087598f*applyconv)));
					tempSample -= (dram->b[52] * (0.20394367993632415f  - (0.08746309731952180f*applyconv)));
					tempSample -= (dram->b[53] * (0.17565242431124092f  - (0.07611309538078760f*applyconv)));
					tempSample -= (dram->b[54] * (0.10116623231246825f  - (0.00642818706295112f*applyconv)));
					tempSample -= (dram->b[55] * (0.00782648272053632f  + (0.08004141267685004f*applyconv)));
					tempSample += (dram->b[56] * (0.05059046006747323f  - (0.12436676387548490f*applyconv)));
					tempSample += (dram->b[57] * (0.06241531553254467f  - (0.11530779547021434f*applyconv)));
					tempSample += (dram->b[58] * (0.04952694587101836f  - (0.08340945324333944f*applyconv)));
					tempSample += (dram->b[59] * (0.00843873294401687f  - (0.03279659052562903f*applyconv)));
					tempSample -= (dram->b[60] * (0.05161338949440241f  - (0.03428181149163798f*applyconv)));
					tempSample -= (dram->b[61] * (0.08165520146902012f  - (0.08196746092283110f*applyconv)));
					tempSample -= (dram->b[62] * (0.06639532849935320f  - (0.09797462781896329f*applyconv)));
					tempSample -= (dram->b[63] * (0.02953430910661621f  - (0.09175612938515763f*applyconv)));
					tempSample += (dram->b[64] * (0.00741058547442938f  + (0.05442091048731967f*applyconv)));
					tempSample += (dram->b[65] * (0.01832866125391727f  + (0.00306243693643687f*applyconv)));
					tempSample += (dram->b[66] * (0.00526964230373573f  - (0.04364102661136410f*applyconv)));
					tempSample -= (dram->b[67] * (0.00300984373848200f  + (0.09742737841278880f*applyconv)));
					tempSample -= (dram->b[68] * (0.00413616769576694f  + (0.14380661694523073f*applyconv)));
					tempSample -= (dram->b[69] * (0.00588769034931419f  + (0.16012843578892538f*applyconv)));
					tempSample -= (dram->b[70] * (0.00688588239450581f  + (0.14074464279305798f*applyconv)));
					tempSample -= (dram->b[71] * (0.02277307992926315f  + (0.07914752191801366f*applyconv)));
					tempSample -= (dram->b[72] * (0.04627166091180877f  - (0.00192787268067208f*applyconv)));
					tempSample -= (dram->b[73] * (0.05562045897455786f  - (0.05932868727665747f*applyconv)));
					tempSample -= (dram->b[74] * (0.05134243784922165f  - (0.08245334798868090f*applyconv)));
					tempSample -= (dram->b[75] * (0.04719409472239919f  - (0.07498680629253825f*applyconv)));
					tempSample -= (dram->b[76] * (0.05889738914266415f  - (0.06116127018043697f*applyconv)));
					tempSample -= (dram->b[77] * (0.09428363535111127f  - (0.06535868867863834f*applyconv)));
					tempSample -= (dram->b[78] * (0.15181756953225126f  - (0.08982979655234427f*applyconv)));
					tempSample -= (dram->b[79] * (0.20878969456036670f  - (0.10761070891499538f*applyconv)));
					tempSample -= (dram->b[80] * (0.22647885581813790f  - (0.08462542510349125f*applyconv)));
					tempSample -= (dram->b[81] * (0.19723482443646323f  - (0.02665160920736287f*applyconv)));
					tempSample -= (dram->b[82] * (0.16441643451155163f  + (0.02314691954338197f*applyconv)));
					tempSample -= (dram->b[83] * (0.15201914054931515f  + (0.04424903493886839f*applyconv)));
					tempSample -= (dram->b[84] * (0.15454370641307855f  + (0.04223203797913008f*applyconv)));
					//end VintageStack conv
					break;
				case 3:
					//begin BoutiqueStack conv
					tempSample += (dram->b[1] * (1.30406584776167445f  - (0.01410622186823351f*applyconv)));
					tempSample += (dram->b[2] * (1.09350974154373559f  + (0.34478044709202327f*applyconv)));
					tempSample += (dram->b[3] * (0.52285510059938256f  + (0.84225842837363574f*applyconv)));
					tempSample -= (dram->b[4] * (0.00018126260714707f  - (1.02446537989058117f*applyconv)));
					tempSample -= (dram->b[5] * (0.34943699771860115f  - (0.84094709567790016f*applyconv)));
					tempSample -= (dram->b[6] * (0.53068048407937285f  - (0.49231169327705593f*applyconv)));
					tempSample -= (dram->b[7] * (0.48631669406792399f  - (0.08965111766223610f*applyconv)));
					tempSample -= (dram->b[8] * (0.28099201947014130f  + (0.23921137841068607f*applyconv)));
					tempSample -= (dram->b[9] * (0.10333290012666248f  + (0.35058962687321482f*applyconv)));
					tempSample -= (dram->b[10] * (0.06605032198166226f  + (0.23447405567823365f*applyconv)));
					tempSample -= (dram->b[11] * (0.10485808661261729f  + (0.05025985449763527f*applyconv)));
					tempSample -= (dram->b[12] * (0.13231190973014911f  - (0.05484648240248013f*applyconv)));
					tempSample -= (dram->b[13] * (0.12926184767180304f  - (0.04054223744746116f*applyconv)));
					tempSample -= (dram->b[14] * (0.13802696739839460f  + (0.01876754906568237f*applyconv)));
					tempSample -= (dram->b[15] * (0.16548980700926913f  + (0.06772130758771169f*applyconv)));
					tempSample -= (dram->b[16] * (0.14469310965751475f  + (0.10590928840978781f*applyconv)));
					tempSample -= (dram->b[17] * (0.07838457396093310f  + (0.13120101199677947f*applyconv)));
					tempSample -= (dram->b[18] * (0.05123031606187391f  + (0.13883400806512292f*applyconv)));
					tempSample -= (dram->b[19] * (0.08906103481939850f  + (0.07840461228402337f*applyconv)));
					tempSample -= (dram->b[20] * (0.13939265522625241f  + (0.01194366471800457f*applyconv)));
					tempSample -= (dram->b[21] * (0.14957600717294034f  + (0.07687598594361914f*applyconv)));
					tempSample -= (dram->b[22] * (0.14112708654047090f  + (0.20118461131186977f*applyconv)));
					tempSample -= (dram->b[23] * (0.14961020766492997f  + (0.30005716443826147f*applyconv)));
					tempSample -= (dram->b[24] * (0.16130382224652270f  + (0.40459872030013055f*applyconv)));
					tempSample -= (dram->b[25] * (0.15679868471080052f  + (0.47292767226083465f*applyconv)));
					tempSample -= (dram->b[26] * (0.16456530552807727f  + (0.45182121471666481f*applyconv)));
					tempSample -= (dram->b[27] * (0.16852385701909278f  + (0.38272684270752266f*applyconv)));
					tempSample -= (dram->b[28] * (0.13317562760966850f  + (0.28829580273670768f*applyconv)));
					tempSample -= (dram->b[29] * (0.09396196532150952f  + (0.18886898332071317f*applyconv)));
					tempSample -= (dram->b[30] * (0.10133496956734221f  + (0.11158788414137354f*applyconv)));
					tempSample -= (dram->b[31] * (0.16097596389376778f  + (0.02621299102374547f*applyconv)));
					tempSample -= (dram->b[32] * (0.21419006394821866f  - (0.03585678078834797f*applyconv)));
					tempSample -= (dram->b[33] * (0.21273234570555244f  - (0.02574469802924526f*applyconv)));
					tempSample -= (dram->b[34] * (0.16934948798707830f  + (0.01354331184333835f*applyconv)));
					tempSample -= (dram->b[35] * (0.11970436472852493f  + (0.04242183865883427f*applyconv)));
					tempSample -= (dram->b[36] * (0.09329023656747724f  + (0.06890873292358397f*applyconv)));
					tempSample -= (dram->b[37] * (0.10255328436608116f  + (0.11482972519137427f*applyconv)));
					tempSample -= (dram->b[38] * (0.13883223352796811f  + (0.18016014431438840f*applyconv)));
					tempSample -= (dram->b[39] * (0.16532844286979087f  + (0.24521957638633446f*applyconv)));
					tempSample -= (dram->b[40] * (0.16254607738965438f  + (0.25669472097572482f*applyconv)));
					tempSample -= (dram->b[41] * (0.15353207135544752f  + (0.15048064682912729f*applyconv)));
					tempSample -= (dram->b[42] * (0.13039046390746015f  - (0.00200335414623601f*applyconv)));
					tempSample -= (dram->b[43] * (0.06707245032180627f  - (0.06498125592578702f*applyconv)));
					tempSample += (dram->b[44] * (0.01427326441869788f  + (0.01940451360783622f*applyconv)));
					tempSample += (dram->b[45] * (0.06151238306578224f  - (0.07335755969763329f*applyconv)));
					tempSample += (dram->b[46] * (0.04685840498892526f  - (0.14258849371688248f*applyconv)));
					tempSample -= (dram->b[47] * (0.00950136304466093f  + (0.14379354707665129f*applyconv)));
					tempSample -= (dram->b[48] * (0.06245771575493557f  + (0.07639718586346110f*applyconv)));
					tempSample -= (dram->b[49] * (0.07159593175777741f  - (0.00595536565276915f*applyconv)));
					tempSample -= (dram->b[50] * (0.03167929390245019f  - (0.03856769526301793f*applyconv)));
					tempSample += (dram->b[51] * (0.01890898565110766f  + (0.00760539424271147f*applyconv)));
					tempSample += (dram->b[52] * (0.04926161137832240f  - (0.06411014430053390f*applyconv)));
					tempSample += (dram->b[53] * (0.05768814623421683f  - (0.15068618173358578f*applyconv)));
					tempSample += (dram->b[54] * (0.06144258297076708f  - (0.21200636329120301f*applyconv)));
					tempSample += (dram->b[55] * (0.06348341960185613f  - (0.19620269813094307f*applyconv)));
					tempSample += (dram->b[56] * (0.04877736350310589f  - (0.11864999881200111f*applyconv)));
					tempSample += (dram->b[57] * (0.01010950997574472f  - (0.02630070679113791f*applyconv)));
					tempSample -= (dram->b[58] * (0.02929178864801191f  - (0.04439260202207482f*applyconv)));
					tempSample -= (dram->b[59] * (0.03484517126321562f  - (0.04508635396034735f*applyconv)));
					tempSample -= (dram->b[60] * (0.00547176780437610f  - (0.00205637806941426f*applyconv)));
					tempSample += (dram->b[61] * (0.02278296865283977f  - (0.00063732526427685f*applyconv)));
					tempSample += (dram->b[62] * (0.02688982591366477f  + (0.05333738901586284f*applyconv)));
					tempSample += (dram->b[63] * (0.01942012754957055f  + (0.10942832669749143f*applyconv)));
					tempSample += (dram->b[64] * (0.01572585258756565f  + (0.11189204189054594f*applyconv)));
					tempSample += (dram->b[65] * (0.01490550715016034f  + (0.04449822818925343f*applyconv)));
					tempSample += (dram->b[66] * (0.01715683226376727f  - (0.06944648050933899f*applyconv)));
					tempSample += (dram->b[67] * (0.02822659878011318f  - (0.17843652160132820f*applyconv)));
					tempSample += (dram->b[68] * (0.03758307610456144f  - (0.21986013433664692f*applyconv)));
					tempSample += (dram->b[69] * (0.03275008021608433f  - (0.15869878676112170f*applyconv)));
					tempSample += (dram->b[70] * (0.01855749786752354f  - (0.02337224995718105f*applyconv)));
					tempSample += (dram->b[71] * (0.00217095395782931f  + (0.10971764224593601f*applyconv)));
					tempSample -= (dram->b[72] * (0.01851381451105007f  - (0.17214910008793413f*applyconv)));
					tempSample -= (dram->b[73] * (0.04722574936345419f  - (0.14341588977845254f*applyconv)));
					tempSample -= (dram->b[74] * (0.07151540514482006f  - (0.04684695724814321f*applyconv)));
					tempSample -= (dram->b[75] * (0.06827195484995092f  + (0.07022207121861397f*applyconv)));
					tempSample -= (dram->b[76] * (0.03290227240464227f  + (0.16328400808152735f*applyconv)));
					tempSample += (dram->b[77] * (0.01043861198275382f  - (0.20184486126076279f*applyconv)));
					tempSample += (dram->b[78] * (0.03236563559476477f  - (0.17125821306380920f*applyconv)));
					tempSample += (dram->b[79] * (0.02040121529932702f  - (0.09103660189829657f*applyconv)));
					tempSample -= (dram->b[80] * (0.00509649513318102f  + (0.01170360991547489f*applyconv)));
					tempSample -= (dram->b[81] * (0.01388353426600228f  - (0.03588955538451771f*applyconv)));
					tempSample -= (dram->b[82] * (0.00523671715033842f  - (0.07068798057534148f*applyconv)));
					tempSample += (dram->b[83] * (0.00665852487721137f  + (0.11666210640054926f*applyconv)));
					tempSample += (dram->b[84] * (0.01593540832939290f  + (0.15844892856402149f*applyconv)));
					tempSample += (dram->b[85] * (0.02080509201836796f  + (0.17186274420065850f*applyconv)));
					//end BoutiqueStack conv
					break;
				case 4:
					//begin LargeCombo conv
					tempSample += (dram->b[1] * (1.31819680801404560f  + (0.00362521700518292f*applyconv)));
					tempSample += (dram->b[2] * (1.37738284126127919f  + (0.14134596126256205f*applyconv)));
					tempSample += (dram->b[3] * (1.09957637225311622f  + (0.33199581815501555f*applyconv)));
					tempSample += (dram->b[4] * (0.62025358899656258f  + (0.37476042042088142f*applyconv)));
					tempSample += (dram->b[5] * (0.12926194402137478f  + (0.24702655568406759f*applyconv)));
					tempSample -= (dram->b[6] * (0.28927985011367602f  - (0.13289168298307708f*applyconv)));
					tempSample -= (dram->b[7] * (0.56518146339033448f  - (0.11026641793526121f*applyconv)));
					tempSample -= (dram->b[8] * (0.59843200696815069f  - (0.10139909232154271f*applyconv)));
					tempSample -= (dram->b[9] * (0.45219971861789204f  - (0.13313355255903159f*applyconv)));
					tempSample -= (dram->b[10] * (0.32520490032331351f  - (0.29009061730364216f*applyconv)));
					tempSample -= (dram->b[11] * (0.29773131872442909f  - (0.45307495356996669f*applyconv)));
					tempSample -= (dram->b[12] * (0.31738895975218867f  - (0.43198591958928922f*applyconv)));
					tempSample -= (dram->b[13] * (0.33336150604703757f  - (0.24240412850274029f*applyconv)));
					tempSample -= (dram->b[14] * (0.32461638442042151f  - (0.02779297492397464f*applyconv)));
					tempSample -= (dram->b[15] * (0.27812829473787770f  + (0.15565718905032455f*applyconv)));
					tempSample -= (dram->b[16] * (0.19413454458668097f  + (0.32087693535188599f*applyconv)));
					tempSample -= (dram->b[17] * (0.12378036344480114f  + (0.37736575956794161f*applyconv)));
					tempSample -= (dram->b[18] * (0.12550494837257106f  + (0.25593811142722300f*applyconv)));
					tempSample -= (dram->b[19] * (0.17725736033713696f  + (0.07708896413593636f*applyconv)));
					tempSample -= (dram->b[20] * (0.22023699647700670f  - (0.01600371273599124f*applyconv)));
					tempSample -= (dram->b[21] * (0.21987645486953747f  + (0.00973336938352798f*applyconv)));
					tempSample -= (dram->b[22] * (0.15014276479707978f  + (0.11602269600138954f*applyconv)));
					tempSample -= (dram->b[23] * (0.05176520203073560f  + (0.20383164255692698f*applyconv)));
					tempSample -= (dram->b[24] * (0.04276687165294867f  + (0.17785002166834518f*applyconv)));
					tempSample -= (dram->b[25] * (0.15951546388137597f  + (0.06748854885822464f*applyconv)));
					tempSample -= (dram->b[26] * (0.30211952144352616f  - (0.03440494237025149f*applyconv)));
					tempSample -= (dram->b[27] * (0.36462803375134506f  - (0.05874284362202409f*applyconv)));
					tempSample -= (dram->b[28] * (0.32283960219377539f  + (0.01189623197958362f*applyconv)));
					tempSample -= (dram->b[29] * (0.19245178663350720f  + (0.11088858383712991f*applyconv)));
					tempSample += (dram->b[30] * (0.00681589563349590f  - (0.16314250963457660f*applyconv)));
					tempSample += (dram->b[31] * (0.20927798345622584f  - (0.16952981620487462f*applyconv)));
					tempSample += (dram->b[32] * (0.25638846543430976f  - (0.11462562122281153f*applyconv)));
					tempSample += (dram->b[33] * (0.04584495673888605f  + (0.04669671229804190f*applyconv)));
					tempSample -= (dram->b[34] * (0.25221561978187662f  - (0.19250758741703761f*applyconv)));
					tempSample -= (dram->b[35] * (0.35662801992424953f  - (0.12244680002787561f*applyconv)));
					tempSample -= (dram->b[36] * (0.21498114329314663f  + (0.12152120956991189f*applyconv)));
					tempSample += (dram->b[37] * (0.00968605571673376f  - (0.30597812512858558f*applyconv)));
					tempSample += (dram->b[38] * (0.18029119870614621f  - (0.31569386468576782f*applyconv)));
					tempSample += (dram->b[39] * (0.22744437185251629f  - (0.18028438460422197f*applyconv)));
					tempSample += (dram->b[40] * (0.09725687638959078f  + (0.05479918522830433f*applyconv)));
					tempSample -= (dram->b[41] * (0.17970389267353537f  - (0.29222750363124067f*applyconv)));
					tempSample -= (dram->b[42] * (0.42371969704763018f  - (0.34924957781842314f*applyconv)));
					tempSample -= (dram->b[43] * (0.43313266755788055f  - (0.11503731970288061f*applyconv)));
					tempSample -= (dram->b[44] * (0.22178165627851801f  + (0.25002925550036226f*applyconv)));
					tempSample -= (dram->b[45] * (0.00410198176852576f  + (0.43283281457037676f*applyconv)));
					tempSample += (dram->b[46] * (0.09072426344812032f  - (0.35318250460706446f*applyconv)));
					tempSample += (dram->b[47] * (0.08405839183965140f  - (0.16936391987143717f*applyconv)));
					tempSample -= (dram->b[48] * (0.01110419756114383f  - (0.01247164991313877f*applyconv)));
					tempSample -= (dram->b[49] * (0.18593334646855278f  - (0.14513260199423966f*applyconv)));
					tempSample -= (dram->b[50] * (0.33665010871497486f  - (0.14456206192169668f*applyconv)));
					tempSample -= (dram->b[51] * (0.32644968491439380f  + (0.01594380759082303f*applyconv)));
					tempSample -= (dram->b[52] * (0.14855437679485431f  + (0.23555511219002742f*applyconv)));
					tempSample += (dram->b[53] * (0.05113019250820622f  - (0.35556617126595202f*applyconv)));
					tempSample += (dram->b[54] * (0.12915754942362243f  - (0.28571671825750300f*applyconv)));
					tempSample += (dram->b[55] * (0.07406865846069306f  - (0.10543886479975995f*applyconv)));
					tempSample -= (dram->b[56] * (0.03669573814193980f  - (0.03194267657582078f*applyconv)));
					tempSample -= (dram->b[57] * (0.13429103278009327f  - (0.06145796486786051f*applyconv)));
					tempSample -= (dram->b[58] * (0.17884021749974641f  - (0.00156626902982124f*applyconv)));
					tempSample -= (dram->b[59] * (0.16138212225178239f  + (0.09402070836837134f*applyconv)));
					tempSample -= (dram->b[60] * (0.10867028245257521f  + (0.15407963447815898f*applyconv)));
					tempSample -= (dram->b[61] * (0.06312416389213464f  + (0.11241095544179526f*applyconv)));
					tempSample -= (dram->b[62] * (0.05826376574081994f  - (0.03635253545701986f*applyconv)));
					tempSample -= (dram->b[63] * (0.07991631148258237f  - (0.18041947557579863f*applyconv)));
					tempSample -= (dram->b[64] * (0.07777397532506500f  - (0.20817156738202205f*applyconv)));
					tempSample -= (dram->b[65] * (0.03812528734394271f  - (0.13679963125162486f*applyconv)));
					tempSample += (dram->b[66] * (0.00204900323943951f  + (0.04009000730101046f*applyconv)));
					tempSample += (dram->b[67] * (0.01779599498119764f  - (0.04218637577942354f*applyconv)));
					tempSample += (dram->b[68] * (0.00950301949319113f  - (0.07908911305044238f*applyconv)));
					tempSample -= (dram->b[69] * (0.04283600714814891f  + (0.02716262334097985f*applyconv)));
					tempSample -= (dram->b[70] * (0.14478320837041933f  - (0.08823515277628832f*applyconv)));
					tempSample -= (dram->b[71] * (0.23250267035795688f  - (0.15334197814956568f*applyconv)));
					tempSample -= (dram->b[72] * (0.22369031446225857f  - (0.08550989980799503f*applyconv)));
					tempSample -= (dram->b[73] * (0.11142757883989868f  + (0.08321482928259660f*applyconv)));
					tempSample += (dram->b[74] * (0.02752318631713307f  - (0.25252906099212968f*applyconv)));
					tempSample += (dram->b[75] * (0.11940028414727490f  - (0.34358127205009553f*applyconv)));
					tempSample += (dram->b[76] * (0.12702057126698307f  - (0.31808560130583663f*applyconv)));
					tempSample += (dram->b[77] * (0.03639067777025356f  - (0.17970282734717846f*applyconv)));
					tempSample -= (dram->b[78] * (0.11389848143835518f  + (0.00470616711331971f*applyconv)));
					tempSample -= (dram->b[79] * (0.23024072979374310f  - (0.09772245468884058f*applyconv)));
					tempSample -= (dram->b[80] * (0.24389015061112601f  - (0.09600959885615798f*applyconv)));
					tempSample -= (dram->b[81] * (0.16680269075295703f  - (0.05194978963662898f*applyconv)));
					tempSample -= (dram->b[82] * (0.05108041495077725f  - (0.01796071525570735f*applyconv)));
					tempSample += (dram->b[83] * (0.06489835353859555f  - (0.00808013770331126f*applyconv)));
					tempSample += (dram->b[84] * (0.15481511440233464f  - (0.02674063848284838f*applyconv)));
					tempSample += (dram->b[85] * (0.18620867857907253f  - (0.01786423699465214f*applyconv)));
					tempSample += (dram->b[86] * (0.13879832139055756f  + (0.01584446839973597f*applyconv)));
					tempSample += (dram->b[87] * (0.04878235109120615f  + (0.02962866516075816f*applyconv)));
					//end LargeCombo conv
					break;
				case 5:
					//begin SmallCombo conv
					tempSample += (dram->b[1] * (1.42133070619855229f  - (0.18270903813104500f*applyconv)));
					tempSample += (dram->b[2] * (1.47209686171873821f  - (0.27954009590498585f*applyconv)));
					tempSample += (dram->b[3] * (1.34648011331265294f  - (0.47178343556301960f*applyconv)));
					tempSample += (dram->b[4] * (0.82133804036124580f  - (0.41060189990353935f*applyconv)));
					tempSample += (dram->b[5] * (0.21628057120466901f  - (0.26062442734317454f*applyconv)));
					tempSample -= (dram->b[6] * (0.30306716082877883f  + (0.10067648425439185f*applyconv)));
					tempSample -= (dram->b[7] * (0.69484313178531876f  - (0.09655574841702286f*applyconv)));
					tempSample -= (dram->b[8] * (0.88320822356980833f  - (0.26501644327144314f*applyconv)));
					tempSample -= (dram->b[9] * (0.81326147029423723f  - (0.31115926837054075f*applyconv)));
					tempSample -= (dram->b[10] * (0.56728759049069222f  - (0.23304233545561287f*applyconv)));
					tempSample -= (dram->b[11] * (0.33340326645198737f  - (0.12361361388240180f*applyconv)));
					tempSample -= (dram->b[12] * (0.20280263733605616f  - (0.03531960962500105f*applyconv)));
					tempSample -= (dram->b[13] * (0.15864533788751345f  + (0.00355160825317868f*applyconv)));
					tempSample -= (dram->b[14] * (0.12544767480555119f  + (0.01979010423176500f*applyconv)));
					tempSample -= (dram->b[15] * (0.06666788902658917f  + (0.00188830739903378f*applyconv)));
					tempSample += (dram->b[16] * (0.02977793355081072f  + (0.02304216615605394f*applyconv)));
					tempSample += (dram->b[17] * (0.12821526330916558f  + (0.02636238376777800f*applyconv)));
					tempSample += (dram->b[18] * (0.19933812710210136f  - (0.02932657234709721f*applyconv)));
					tempSample += (dram->b[19] * (0.18346460191225772f  - (0.12859581955080629f*applyconv)));
					tempSample -= (dram->b[20] * (0.00088697526755385f  + (0.15855257539277415f*applyconv)));
					tempSample -= (dram->b[21] * (0.28904286712096761f  + (0.06226286786982616f*applyconv)));
					tempSample -= (dram->b[22] * (0.49133546282552537f  - (0.06512851581813534f*applyconv)));
					tempSample -= (dram->b[23] * (0.52908013030763046f  - (0.13606992188523465f*applyconv)));
					tempSample -= (dram->b[24] * (0.45897241332311706f  - (0.15527194946346906f*applyconv)));
					tempSample -= (dram->b[25] * (0.35535938629924352f  - (0.13634771941703441f*applyconv)));
					tempSample -= (dram->b[26] * (0.26185269405237693f  - (0.08736651482771546f*applyconv)));
					tempSample -= (dram->b[27] * (0.19997351944186473f  - (0.01714565029656306f*applyconv)));
					tempSample -= (dram->b[28] * (0.18894054145105646f  + (0.04557612705740050f*applyconv)));
					tempSample -= (dram->b[29] * (0.24043993691153928f  + (0.05267500387081067f*applyconv)));
					tempSample -= (dram->b[30] * (0.29191852873822671f  + (0.01922151122971644f*applyconv)));
					tempSample -= (dram->b[31] * (0.29399783430587761f  - (0.02238952856106585f*applyconv)));
					tempSample -= (dram->b[32] * (0.26662219155294159f  - (0.07760819463416335f*applyconv)));
					tempSample -= (dram->b[33] * (0.20881206667122221f  - (0.11930017354479640f*applyconv)));
					tempSample -= (dram->b[34] * (0.12916658879944876f  - (0.11798638949823513f*applyconv)));
					tempSample -= (dram->b[35] * (0.07678815166012012f  - (0.06826864734598684f*applyconv)));
					tempSample -= (dram->b[36] * (0.08568505484529348f  - (0.00510459741104792f*applyconv)));
					tempSample -= (dram->b[37] * (0.13613615872486634f  + (0.02288223583971244f*applyconv)));
					tempSample -= (dram->b[38] * (0.17426657494209266f  + (0.02723737220296440f*applyconv)));
					tempSample -= (dram->b[39] * (0.17343619261009030f  + (0.01412920547179825f*applyconv)));
					tempSample -= (dram->b[40] * (0.14548368977428555f  - (0.02640418940455951f*applyconv)));
					tempSample -= (dram->b[41] * (0.10485295885802372f  - (0.06334665781931498f*applyconv)));
					tempSample -= (dram->b[42] * (0.06632268974717079f  - (0.05960343688612868f*applyconv)));
					tempSample -= (dram->b[43] * (0.06915692039882040f  - (0.03541337869596061f*applyconv)));
					tempSample -= (dram->b[44] * (0.11889611687783583f  - (0.02250608307287119f*applyconv)));
					tempSample -= (dram->b[45] * (0.14598456370320673f  + (0.00280345943128246f*applyconv)));
					tempSample -= (dram->b[46] * (0.12312084125613143f  + (0.04947283933434576f*applyconv)));
					tempSample -= (dram->b[47] * (0.11379940289994711f  + (0.06590080966570636f*applyconv)));
					tempSample -= (dram->b[48] * (0.12963290754003182f  + (0.02597647654256477f*applyconv)));
					tempSample -= (dram->b[49] * (0.12723837402978638f  - (0.04942071966927938f*applyconv)));
					tempSample -= (dram->b[50] * (0.09185015882996231f  - (0.10420810015956679f*applyconv)));
					tempSample -= (dram->b[51] * (0.04011592913036545f  - (0.10234174227772008f*applyconv)));
					tempSample += (dram->b[52] * (0.00992597785057113f  + (0.05674042373836896f*applyconv)));
					tempSample += (dram->b[53] * (0.04921452178306781f  - (0.00222698867111080f*applyconv)));
					tempSample += (dram->b[54] * (0.06096504883783566f  - (0.04040426549982253f*applyconv)));
					tempSample += (dram->b[55] * (0.04113530718724200f  - (0.04190143593049960f*applyconv)));
					tempSample += (dram->b[56] * (0.01292699017654650f  - (0.01121994018532499f*applyconv)));
					tempSample -= (dram->b[57] * (0.00437123132431870f  - (0.02482497612289103f*applyconv)));
					tempSample -= (dram->b[58] * (0.02090571264211918f  - (0.03732746039260295f*applyconv)));
					tempSample -= (dram->b[59] * (0.04749751678612051f  - (0.02960060937328099f*applyconv)));
					tempSample -= (dram->b[60] * (0.07675095194206227f  - (0.02241927084099648f*applyconv)));
					tempSample -= (dram->b[61] * (0.08879414028581609f  - (0.01144281133042115f*applyconv)));
					tempSample -= (dram->b[62] * (0.07378854974999530f  + (0.02518742701599147f*applyconv)));
					tempSample -= (dram->b[63] * (0.04677309194488959f  + (0.08984657372223502f*applyconv)));
					tempSample -= (dram->b[64] * (0.02911874044176449f  + (0.14202665940555093f*applyconv)));
					tempSample -= (dram->b[65] * (0.02103564720234969f  + (0.14640411976171003f*applyconv)));
					tempSample -= (dram->b[66] * (0.01940626429101940f  + (0.10867274382865903f*applyconv)));
					tempSample -= (dram->b[67] * (0.03965401793931531f  + (0.04775225375522835f*applyconv)));
					tempSample -= (dram->b[68] * (0.08102486457314527f  - (0.03204447425666343f*applyconv)));
					tempSample -= (dram->b[69] * (0.11794849372825778f  - (0.12755667382696789f*applyconv)));
					tempSample -= (dram->b[70] * (0.11946469076758266f  - (0.20151394599125422f*applyconv)));
					tempSample -= (dram->b[71] * (0.07404630324668053f  - (0.21300634351769704f*applyconv)));
					tempSample -= (dram->b[72] * (0.00477584437144086f  - (0.16864707684978708f*applyconv)));
					tempSample += (dram->b[73] * (0.05924822014377220f  + (0.09394651445109450f*applyconv)));
					tempSample += (dram->b[74] * (0.10060989907457370f  + (0.00419196431884887f*applyconv)));
					tempSample += (dram->b[75] * (0.10817907203844988f  - (0.07459664480796091f*applyconv)));
					tempSample += (dram->b[76] * (0.08701102204768002f  - (0.11129477437630560f*applyconv)));
					tempSample += (dram->b[77] * (0.05673785623180162f  - (0.10638640242375266f*applyconv)));
					tempSample += (dram->b[78] * (0.02944190197442081f  - (0.08499792583420167f*applyconv)));
					tempSample += (dram->b[79] * (0.01570145445652971f  - (0.06190456843465320f*applyconv)));
					tempSample += (dram->b[80] * (0.02770233032476748f  - (0.04573713136865480f*applyconv)));
					tempSample += (dram->b[81] * (0.05417160459175360f  - (0.03965651064634598f*applyconv)));
					tempSample += (dram->b[82] * (0.06080831637644498f  - (0.02909500789113911f*applyconv)));
					//end SmallCombo conv
					break;
				case 6:
					//begin Bass conv
					tempSample += (dram->b[1] * (1.35472031405494242f  + (0.00220914099195157f*applyconv)));
					tempSample += (dram->b[2] * (1.63534207755253003f  - (0.11406232654509685f*applyconv)));
					tempSample += (dram->b[3] * (1.82334575691525869f  - (0.42647194712964054f*applyconv)));
					tempSample += (dram->b[4] * (1.86156386235405868f  - (0.76744187887586590f*applyconv)));
					tempSample += (dram->b[5] * (1.67332739338852599f  - (0.95161997324293013f*applyconv)));
					tempSample += (dram->b[6] * (1.25054130794899021f  - (0.98410433514572859f*applyconv)));
					tempSample += (dram->b[7] * (0.70049121047281737f  - (0.87375612110718992f*applyconv)));
					tempSample += (dram->b[8] * (0.15291791448081560f  - (0.61195266024519046f*applyconv)));
					tempSample -= (dram->b[9] * (0.37301992914152693f  + (0.16755422915252094f*applyconv)));
					tempSample -= (dram->b[10] * (0.76568539228498433f  - (0.28554435228965386f*applyconv)));
					tempSample -= (dram->b[11] * (0.95726568749937369f  - (0.61659719162806048f*applyconv)));
					tempSample -= (dram->b[12] * (1.01273552193911032f  - (0.81827288407943954f*applyconv)));
					tempSample -= (dram->b[13] * (0.93920108117234447f  - (0.80077111864205874f*applyconv)));
					tempSample -= (dram->b[14] * (0.79831898832953974f  - (0.65814750339694406f*applyconv)));
					tempSample -= (dram->b[15] * (0.64200088100452313f  - (0.46135833001232618f*applyconv)));
					tempSample -= (dram->b[16] * (0.48807302802822128f  - (0.15506178974799034f*applyconv)));
					tempSample -= (dram->b[17] * (0.36545171501947982f  + (0.16126103769376721f*applyconv)));
					tempSample -= (dram->b[18] * (0.31469581455759105f  + (0.32250870039053953f*applyconv)));
					tempSample -= (dram->b[19] * (0.36893534817945800f  + (0.25409418897237473f*applyconv)));
					tempSample -= (dram->b[20] * (0.41092557722975687f  + (0.13114730488878301f*applyconv)));
					tempSample -= (dram->b[21] * (0.38584044480710594f  + (0.06825323739722661f*applyconv)));
					tempSample -= (dram->b[22] * (0.33378434007178670f  + (0.04144255489164217f*applyconv)));
					tempSample -= (dram->b[23] * (0.26144203061699706f  + (0.06031313105098152f*applyconv)));
					tempSample -= (dram->b[24] * (0.25818342000920502f  + (0.03642289242586355f*applyconv)));
					tempSample -= (dram->b[25] * (0.28096018498822661f  + (0.00976973667327174f*applyconv)));
					tempSample -= (dram->b[26] * (0.25845682019095384f  + (0.02749015358080831f*applyconv)));
					tempSample -= (dram->b[27] * (0.26655607865953096f  - (0.00329839675455690f*applyconv)));
					tempSample -= (dram->b[28] * (0.30590085026938518f  - (0.07375043215328811f*applyconv)));
					tempSample -= (dram->b[29] * (0.32875683916470899f  - (0.12454134857516502f*applyconv)));
					tempSample -= (dram->b[30] * (0.38166643180506560f  - (0.19973911428609989f*applyconv)));
					tempSample -= (dram->b[31] * (0.49068186937289598f  - (0.34785166842136384f*applyconv)));
					tempSample -= (dram->b[32] * (0.60274753867622777f  - (0.48685038872711034f*applyconv)));
					tempSample -= (dram->b[33] * (0.65944678627090636f  - (0.49844657885975518f*applyconv)));
					tempSample -= (dram->b[34] * (0.64488955808717285f  - (0.40514406499806987f*applyconv)));
					tempSample -= (dram->b[35] * (0.55818730353434354f  - (0.28029870614987346f*applyconv)));
					tempSample -= (dram->b[36] * (0.43110859113387556f  - (0.15373504582939335f*applyconv)));
					tempSample -= (dram->b[37] * (0.37726894966096269f  - (0.11570983506028532f*applyconv)));
					tempSample -= (dram->b[38] * (0.39953242355200935f  - (0.17879231130484088f*applyconv)));
					tempSample -= (dram->b[39] * (0.36726676379100875f  - (0.22013553023983223f*applyconv)));
					tempSample -= (dram->b[40] * (0.27187029469227386f  - (0.18461171768478427f*applyconv)));
					tempSample -= (dram->b[41] * (0.21109334552321635f  - (0.14497481318083569f*applyconv)));
					tempSample -= (dram->b[42] * (0.19808797405293213f  - (0.14916579928186940f*applyconv)));
					tempSample -= (dram->b[43] * (0.16287926785495671f  - (0.15146098461120627f*applyconv)));
					tempSample -= (dram->b[44] * (0.11086621477163359f  - (0.13182973443924018f*applyconv)));
					tempSample -= (dram->b[45] * (0.07531043236890560f  - (0.08062172796472888f*applyconv)));
					tempSample -= (dram->b[46] * (0.01747364473230771f  + (0.02201865873632456f*applyconv)));
					tempSample += (dram->b[47] * (0.03080279125662693f  - (0.08721756240972101f*applyconv)));
					tempSample += (dram->b[48] * (0.02354148659185142f  - (0.06376361763053796f*applyconv)));
					tempSample -= (dram->b[49] * (0.02835772372098715f  + (0.00589978513642627f*applyconv)));
					tempSample -= (dram->b[50] * (0.08983370744565244f  - (0.02350960427706536f*applyconv)));
					tempSample -= (dram->b[51] * (0.14148947620055380f  - (0.03329826628693369f*applyconv)));
					tempSample -= (dram->b[52] * (0.17576502674572581f  - (0.06507546651241880f*applyconv)));
					tempSample -= (dram->b[53] * (0.17168865666573860f  - (0.07734801128437317f*applyconv)));
					tempSample -= (dram->b[54] * (0.14107027738292105f  - (0.03136459344220402f*applyconv)));
					tempSample -= (dram->b[55] * (0.12287163395380074f  + (0.01933408169185258f*applyconv)));
					tempSample -= (dram->b[56] * (0.12276622398112971f  + (0.01983508766241737f*applyconv)));
					tempSample -= (dram->b[57] * (0.12349721440213673f  - (0.01111031415304768f*applyconv)));
					tempSample -= (dram->b[58] * (0.08649454142716655f  + (0.02252815645513927f*applyconv)));
					tempSample -= (dram->b[59] * (0.00953083685474757f  + (0.13778878548343007f*applyconv)));
					tempSample += (dram->b[60] * (0.06045983158868478f  - (0.23966318224935096f*applyconv)));
					tempSample += (dram->b[61] * (0.09053229817093242f  - (0.27190119941572544f*applyconv)));
					tempSample += (dram->b[62] * (0.08112662178843048f  - (0.22456862606452327f*applyconv)));
					tempSample += (dram->b[63] * (0.07503525686243730f  - (0.14330154410548213f*applyconv)));
					tempSample += (dram->b[64] * (0.07372595404399729f  - (0.06185193766408734f*applyconv)));
					tempSample += (dram->b[65] * (0.06073789200080433f  + (0.01261857435786178f*applyconv)));
					tempSample += (dram->b[66] * (0.04616712695742254f  + (0.05851771967084609f*applyconv)));
					tempSample += (dram->b[67] * (0.01036235510345900f  + (0.08286534414423796f*applyconv)));
					tempSample -= (dram->b[68] * (0.03708389413229191f  - (0.06695282381039531f*applyconv)));
					tempSample -= (dram->b[69] * (0.07092204876981217f  - (0.01915829199112784f*applyconv)));
					tempSample -= (dram->b[70] * (0.09443579589460312f  + (0.01210082455316246f*applyconv)));
					tempSample -= (dram->b[71] * (0.07824038577769601f  + (0.06121988546065113f*applyconv)));
					tempSample -= (dram->b[72] * (0.00854730633079399f  + (0.14468518752295506f*applyconv)));
					tempSample += (dram->b[73] * (0.06845589924191028f  - (0.18902431382592944f*applyconv)));
					tempSample += (dram->b[74] * (0.10351569998375465f  - (0.13204443060279647f*applyconv)));
					tempSample += (dram->b[75] * (0.10513368758532179f  - (0.02993199294485649f*applyconv)));
					tempSample += (dram->b[76] * (0.08896978950235003f  + (0.04074499273825906f*applyconv)));
					tempSample += (dram->b[77] * (0.03697537734050980f  + (0.09217751130846838f*applyconv)));
					tempSample -= (dram->b[78] * (0.04014322441280276f  - (0.14062297149365666f*applyconv)));
					tempSample -= (dram->b[79] * (0.10505934581398618f  - (0.16988861157275814f*applyconv)));
					tempSample -= (dram->b[80] * (0.13937661651676272f  - (0.15083294570551492f*applyconv)));
					tempSample -= (dram->b[81] * (0.13183458845108439f  - (0.06657454442471208f*applyconv)));
					//end Bass conv
					break;
			}
			inputSample *= correctdrygain;
			inputSample += (tempSample*colorIntensity);
			inputSample /= correctboost;
			ataHalfwaySample += inputSample;
			//restore interpolated signal including time domain stuff now
			//end center code for handling timedomain/conv stuff
			
			//second wave of Cabs style slew clamping
			clamp = inputSample - lastPostSample;
			if (clamp > threshold) inputSample = lastPostSample + threshold;
			if (-clamp > rarefaction) inputSample = lastPostSample - rarefaction;
			lastPostSample = inputSample;
			//end raw sample
			
			//begin interpolated sample- change inputSample -> ataHalfwaySample, ataDrySample -> ataHalfDrySample
			clamp = ataHalfwaySample - lastPostHalfSample;
			if (clamp > threshold) ataHalfwaySample = lastPostHalfSample + threshold;
			if (-clamp > rarefaction) ataHalfwaySample = lastPostHalfSample - rarefaction;
			lastPostHalfSample = ataHalfwaySample;
			//end interpolated sample
			
			//post-center code on inputSample and halfwaySample in parallel
			//begin raw sample- inputSample and ataDrySample handled separately here
			float HeadBump;
			if (flip)
			{
				iirHeadBumpA += (inputSample * HeadBumpFreq);
				iirHeadBumpA -= (iirHeadBumpA * iirHeadBumpA * iirHeadBumpA * HeadBumpFreq);
				if (iirHeadBumpA > 0) iirHeadBumpA -= dcblock;
				if (iirHeadBumpA < 0) iirHeadBumpA += dcblock;
				if (fabs(iirHeadBumpA) > 100.0f)
				{iirHeadBumpA = 0.0f; iirHeadBumpB = 0.0f; iirHalfHeadBumpA = 0.0f; iirHalfHeadBumpB = 0.0f;}
				HeadBump = iirHeadBumpA;
			}
			else
			{
				iirHeadBumpB += (inputSample * HeadBumpFreq);
				iirHeadBumpB -= (iirHeadBumpB * iirHeadBumpB * iirHeadBumpB * HeadBumpFreq);
				if (iirHeadBumpB > 0) iirHeadBumpB -= dcblock;
				if (iirHeadBumpB < 0) iirHeadBumpB += dcblock;
				if (fabs(iirHeadBumpB) > 100.0f)
				{iirHeadBumpA = 0.0f; iirHeadBumpB = 0.0f; iirHalfHeadBumpA = 0.0f; iirHalfHeadBumpB = 0.0f;}
				HeadBump = iirHeadBumpB;
			}
			HeadBump /= LowsPad;
			inputSample = (inputSample * (1.0f-heavy)) + (HeadBump * heavy);
			//end raw sample
			
			//begin interpolated sample- change inputSample -> ataHalfwaySample, ataDrySample -> ataHalfDrySample
			if (flip)
			{
				iirHalfHeadBumpA += (ataHalfwaySample * HeadBumpFreq);
				iirHalfHeadBumpA -= (iirHalfHeadBumpA * iirHalfHeadBumpA * iirHalfHeadBumpA * HeadBumpFreq);
				if (iirHalfHeadBumpA > 0) iirHalfHeadBumpA -= dcblock;
				if (iirHalfHeadBumpA < 0) iirHalfHeadBumpA += dcblock;
				if (fabs(iirHalfHeadBumpA) > 100.0f)
				{iirHeadBumpA = 0.0f; iirHeadBumpB = 0.0f; iirHalfHeadBumpA = 0.0f; iirHalfHeadBumpB = 0.0f;}
				HeadBump = iirHalfHeadBumpA;
			}
			else
			{
				iirHalfHeadBumpB += (ataHalfwaySample * HeadBumpFreq);
				iirHalfHeadBumpB -= (iirHalfHeadBumpB * iirHalfHeadBumpB * iirHalfHeadBumpB * HeadBumpFreq);
				if (iirHalfHeadBumpB > 0) iirHalfHeadBumpB -= dcblock;
				if (iirHalfHeadBumpB < 0) iirHalfHeadBumpB += dcblock;
				if (fabs(iirHalfHeadBumpB) > 100.0f)
				{iirHeadBumpA = 0.0f; iirHeadBumpB = 0.0f; iirHalfHeadBumpA = 0.0f; iirHalfHeadBumpB = 0.0f;}
				HeadBump = iirHalfHeadBumpB;
			}
			HeadBump /= LowsPad;
			ataHalfwaySample = (ataHalfwaySample * (1.0f-heavy)) + (HeadBump * heavy);
			//end interpolated sample
						
			//begin antialiasing section for halfway sample
			ataC = ataHalfwaySample - ataHalfDrySample;
			if (flip) {ataA *= 0.94f; ataB *= 0.94f; ataA += ataC; ataB -= ataC; ataC = ataA;}
			else {ataB *= 0.94f; ataA *= 0.94f; ataB += ataC; ataA -= ataC; ataC = ataB;}
			ataHalfDiffSample = (ataC * 0.94f);
			//end antialiasing section for halfway sample
			
			//begin antialiasing section for raw sample
			ataC = inputSample - ataDrySample;
			if (flip) {ataA *= 0.94f; ataB *= 0.94f; ataA += ataC; ataB -= ataC; ataC = ataA;}
			else {ataB *= 0.94f; ataA *= 0.94f; ataB += ataC; ataA -= ataC; ataC = ataB;}
			ataDiffSample = (ataC * 0.94f);
			//end antialiasing section for input sample
			flip = !flip;
			
			inputSample = ataDrySample; inputSample += ((ataDiffSample + ataHalfDiffSample + ataPrevDiffSample) / 1.0f);
			ataPrevDiffSample = ataDiffSample / 2.0f;
			//apply processing as difference to non-oversampled raw input
			
			clamp = inputSample - postPostSample;
			if (clamp > postThreshold) inputSample = postPostSample + postThreshold;
			if (-clamp > postRarefaction) inputSample = postPostSample - postRarefaction;
			postPostSample = inputSample;
			inputSample /= postTrim;
			
			inputSample *= output;
			
			if (cycleEnd == 4) {
				lastRef[0] = lastRef[4]; //start from previous last
				lastRef[2] = (lastRef[0] + inputSample)/2; //half
				lastRef[1] = (lastRef[0] + lastRef[2])/2; //one quarter
				lastRef[3] = (lastRef[2] + inputSample)/2; //three quarters
				lastRef[4] = inputSample; //full
			}
			if (cycleEnd == 3) {
				lastRef[0] = lastRef[3]; //start from previous last
				lastRef[2] = (lastRef[0]+lastRef[0]+inputSample)/3; //third
				lastRef[1] = (lastRef[0]+inputSample+inputSample)/3; //two thirds
				lastRef[3] = inputSample; //full
			}
			if (cycleEnd == 2) {
				lastRef[0] = lastRef[2]; //start from previous last
				lastRef[1] = (lastRef[0] + inputSample)/2; //half
				lastRef[2] = inputSample; //full
			}
			if (cycleEnd == 1) {
				lastRef[0] = inputSample;
			}
			cycle = 0; //reset
			inputSample = lastRef[cycle];
		} else {
			inputSample = lastRef[cycle];
			//we are going through our references now
		}
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	ataLast3Sample = ataLast2Sample = ataLast1Sample = 0.0;
	ataHalfwaySample = ataHalfDrySample = ataHalfDiffSample = 0.0;
	ataA = ataB = ataC = ataDrySample = ataDiffSample = ataPrevDiffSample = 0.0;
	
	for(int count = 0; count < 90; count++) {dram->b[count] = 0;}
	lastSample = 0.0;
	lastHalfSample = 0.0;
	lastPostSample = 0.0;
	lastPostHalfSample = 0.0;
	postPostSample = 0.0;
	for(int count = 0; count < 20; count++) {dram->d[count] = 0;}
	control = 0;
	iirHeadBumpA = 0.0;
	iirHeadBumpB = 0.0;
	iirHalfHeadBumpA = 0.0;
	iirHalfHeadBumpB = 0.0;
	for(int count = 0; count < 6; count++) lastRef[count] = 0.0;

	flip = false;
	ataFlip = false;
	gcount = 0;
	cycle = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
