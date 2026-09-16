#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BrightAmbience"
#define AIRWINDOWS_DESCRIPTION "A plugin for very bright gated reverbs."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','r','e' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sustain", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Decay", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		int p[25361];
		int gcount;
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
	Float32 wet = GetParameter( kParam_One );
	int cpu = (int)(GetParameter( kParam_Two ) * 32);
	Float32 decay = 1.0f+(powf(GetParameter( kParam_Three ),2)/2.0f);
	Float32 scale = 4;
	int count;
	int temp;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		temp = 0;
		if (gcount < 0 || gcount > 12680) {gcount = 12680;}
		count = gcount;
		p[count+12680] = p[count] = (SInt32)(inputSample*8388352.0f);
		//float buffer
		//-8388352 to 8388352 is equal to 24 bit linear space
		switch (cpu)
		{
			case 32:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+12679]; temp += p[count+12671]; temp += p[count+12661]; temp += p[count+12613]; temp += p[count+12587]; temp += p[count+12577]; temp += p[count+12563]; temp += p[count+12557]; temp += p[count+12521]; temp += p[count+12499]; 
				temp += p[count+12493]; temp += p[count+12457]; temp += p[count+12427]; temp += p[count+12401]; temp += p[count+12373]; temp += p[count+12367]; temp += p[count+12349]; temp += p[count+12337]; temp += p[count+12323]; temp += p[count+12301]; 
			case 31:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+12281]; temp += p[count+12247]; temp += p[count+12239]; temp += p[count+12203]; temp += p[count+12197]; temp += p[count+12191]; temp += p[count+12163]; temp += p[count+12143]; temp += p[count+12127]; temp += p[count+12109]; 
				temp += p[count+12083]; temp += p[count+12077]; temp += p[count+12059]; temp += p[count+12037]; temp += p[count+11989]; temp += p[count+11969]; temp += p[count+11951]; temp += p[count+11933]; temp += p[count+11927]; temp += p[count+11911]; 
			case 30:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+11897]; temp += p[count+11873]; temp += p[count+11843]; temp += p[count+11819]; temp += p[count+11791]; temp += p[count+11773]; temp += p[count+11731]; temp += p[count+11723]; temp += p[count+11701]; temp += p[count+11687]; 
				temp += p[count+11659]; temp += p[count+11623]; temp += p[count+11611]; temp += p[count+11587]; temp += p[count+11573]; temp += p[count+11549]; temp += p[count+11537]; temp += p[count+11527]; temp += p[count+11513]; temp += p[count+11503]; 
			case 29:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+11467]; temp += p[count+11441]; temp += p[count+11413]; temp += p[count+11399]; temp += p[count+11363]; temp += p[count+11353]; temp += p[count+11329]; temp += p[count+11323]; temp += p[count+11303]; temp += p[count+11293]; 
				temp += p[count+11281]; temp += p[count+11251]; temp += p[count+11231]; temp += p[count+11209]; temp += p[count+11201]; temp += p[count+11177]; temp += p[count+11171]; temp += p[count+11153]; temp += p[count+11141]; temp += p[count+11117]; 
			case 28:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+11099]; temp += p[count+11071]; temp += p[count+11047]; temp += p[count+11033]; temp += p[count+11021]; temp += p[count+10993]; temp += p[count+10973]; temp += p[count+10943]; temp += p[count+10933]; temp += p[count+10919]; 
				temp += p[count+10897]; temp += p[count+10879]; temp += p[count+10867]; temp += p[count+10841]; temp += p[count+10783]; temp += p[count+10763]; temp += p[count+10751]; temp += p[count+10733]; temp += p[count+10721]; temp += p[count+10687]; 
			case 27:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+10669]; temp += p[count+10657]; temp += p[count+10649]; temp += p[count+10637]; temp += p[count+10621]; temp += p[count+10609]; temp += p[count+10603]; temp += p[count+10597]; temp += p[count+10571]; temp += p[count+10561]; 
				temp += p[count+10553]; temp += p[count+10499]; temp += p[count+10481]; temp += p[count+10453]; temp += p[count+10439]; temp += p[count+10417]; temp += p[count+10399]; temp += p[count+10391]; temp += p[count+10363]; temp += p[count+10343]; 
			case 26:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+10321]; temp += p[count+10301]; temp += p[count+10261]; temp += p[count+10253]; temp += p[count+10231]; temp += p[count+10211]; temp += p[count+10189]; temp += p[count+10159]; temp += p[count+10139]; temp += p[count+10099]; 
				temp += p[count+10093]; temp += p[count+10079]; temp += p[count+10039]; temp += p[count+9983]; temp += p[count+9973]; temp += p[count+9953]; temp += p[count+9929]; temp += p[count+9911]; temp += p[count+9893]; temp += p[count+9871]; 
			case 25:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+9851]; temp += p[count+9817]; temp += p[count+9773]; temp += p[count+9763]; temp += p[count+9721]; temp += p[count+9703]; temp += p[count+9697]; temp += p[count+9673]; temp += p[count+9641]; temp += p[count+9631]; 
				temp += p[count+9619]; temp += p[count+9607]; temp += p[count+9581]; temp += p[count+9559]; temp += p[count+9547]; temp += p[count+9521]; temp += p[count+9493]; temp += p[count+9487]; temp += p[count+9469]; temp += p[count+9449]; 
			case 24:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+9433]; temp += p[count+9413]; temp += p[count+9403]; temp += p[count+9391]; temp += p[count+9371]; temp += p[count+9349]; temp += p[count+9323]; temp += p[count+9311]; temp += p[count+9301]; temp += p[count+9277]; 
				temp += p[count+9257]; temp += p[count+9239]; temp += p[count+9227]; temp += p[count+9221]; temp += p[count+9169]; temp += p[count+9161]; temp += p[count+9151]; temp += p[count+9137]; temp += p[count+9109]; temp += p[count+9101]; 
			case 23:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+9089]; temp += p[count+9071]; temp += p[count+9043]; temp += p[count+9019]; temp += p[count+9007]; temp += p[count+8971]; temp += p[count+8963]; temp += p[count+8929]; temp += p[count+8917]; temp += p[count+8893]; 
				temp += p[count+8863]; temp += p[count+8849]; temp += p[count+8837]; temp += p[count+8803]; temp += p[count+8779]; temp += p[count+8761]; temp += p[count+8753]; temp += p[count+8731]; temp += p[count+8699]; temp += p[count+8677]; 
			case 22:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+8669]; temp += p[count+8657]; temp += p[count+8647]; temp += p[count+8623]; temp += p[count+8599]; temp += p[count+8581]; temp += p[count+8569]; temp += p[count+8563]; temp += p[count+8539]; temp += p[count+8521]; 
				temp += p[count+8507]; temp += p[count+8497]; temp += p[count+8473]; temp += p[count+8437]; temp += p[count+8411]; temp += p[count+8387]; temp += p[count+8371]; temp += p[count+8363]; temp += p[count+8339]; temp += p[count+8303]; 
			case 21:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+8287]; temp += p[count+8273]; temp += p[count+8233]; temp += p[count+8221]; temp += p[count+8209]; temp += p[count+8201]; temp += p[count+8173]; temp += p[count+8161]; temp += p[count+8143]; temp += p[count+8117]; 
				temp += p[count+8107]; temp += p[count+8083]; temp += p[count+8069]; temp += p[count+8059]; temp += p[count+8023]; temp += p[count+8009]; temp += p[count+7991]; temp += p[count+7943]; temp += p[count+7927]; temp += p[count+7909]; 
			case 20:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+7879]; temp += p[count+7871]; temp += p[count+7841]; temp += p[count+7831]; temp += p[count+7813]; temp += p[count+7799]; temp += p[count+7783]; temp += p[count+7747]; temp += p[count+7733]; temp += p[count+7709]; 
				temp += p[count+7673]; temp += p[count+7661]; temp += p[count+7633]; temp += p[count+7627]; temp += p[count+7607]; temp += p[count+7597]; temp += p[count+7561]; temp += p[count+7543]; temp += p[count+7529]; temp += p[count+7513]; 
			case 19:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+7481]; temp += p[count+7447]; temp += p[count+7429]; temp += p[count+7409]; temp += p[count+7397]; temp += p[count+7369]; temp += p[count+7363]; temp += p[count+7319]; temp += p[count+7291]; temp += p[count+7277]; 
				temp += p[count+7267]; temp += p[count+7249]; temp += p[count+7243]; temp += p[count+7237]; temp += p[count+7187]; temp += p[count+7171]; temp += p[count+7151]; temp += p[count+7117]; temp += p[count+7099]; temp += p[count+7087]; 
			case 18:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+7069]; temp += p[count+7061]; temp += p[count+7039]; temp += p[count+6997]; temp += p[count+6983]; temp += p[count+6971]; temp += p[count+6953]; temp += p[count+6941]; temp += p[count+6919]; temp += p[count+6913]; 
				temp += p[count+6907]; temp += p[count+6859]; temp += p[count+6847]; temp += p[count+6829]; temp += p[count+6823]; temp += p[count+6809]; temp += p[count+6787]; temp += p[count+6763]; temp += p[count+6749]; temp += p[count+6731]; 
			case 17:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+6707]; temp += p[count+6677]; temp += p[count+6653]; temp += p[count+6641]; temp += p[count+6623]; temp += p[count+6607]; temp += p[count+6581]; temp += p[count+6563]; temp += p[count+6553]; temp += p[count+6539]; 
				temp += p[count+6511]; temp += p[count+6499]; temp += p[count+6487]; temp += p[count+6469]; temp += p[count+6457]; temp += p[count+6449]; temp += p[count+6439]; temp += p[count+6413]; temp += p[count+6397]; temp += p[count+6373]; 
			case 16:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+6353]; temp += p[count+6329]; temp += p[count+6317]; temp += p[count+6301]; temp += p[count+6281]; temp += p[count+6263]; temp += p[count+6253]; temp += p[count+6233]; temp += p[count+6197]; temp += p[count+6163]; 
				temp += p[count+6137]; temp += p[count+6127]; temp += p[count+6109]; temp += p[count+6077]; temp += p[count+6067]; temp += p[count+6049]; temp += p[count+6031]; temp += p[count+6011]; temp += p[count+6001]; temp += p[count+5977]; 
			case 15:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+5959]; temp += p[count+5947]; temp += p[count+5933]; temp += p[count+5921]; temp += p[count+5911]; temp += p[count+5897]; temp += p[count+5879]; temp += p[count+5849]; temp += p[count+5833]; temp += p[count+5813]; 
				temp += p[count+5807]; temp += p[count+5773]; temp += p[count+5743]; temp += p[count+5737]; temp += p[count+5729]; temp += p[count+5717]; temp += p[count+5699]; temp += p[count+5669]; temp += p[count+5641]; temp += p[count+5627]; 
			case 14:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+5611]; temp += p[count+5599]; temp += p[count+5587]; temp += p[count+5569]; temp += p[count+5549]; temp += p[count+5521]; temp += p[count+5503]; temp += p[count+5483]; temp += p[count+5461]; temp += p[count+5429]; 
				temp += p[count+5401]; temp += p[count+5377]; temp += p[count+5353]; temp += p[count+5333]; temp += p[count+5323]; temp += p[count+5267]; temp += p[count+5261]; temp += p[count+5237]; temp += p[count+5213]; temp += p[count+5191]; 
			case 13:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+5179]; temp += p[count+5147]; temp += p[count+5113]; temp += p[count+5101]; temp += p[count+5087]; temp += p[count+5081]; temp += p[count+5059]; temp += p[count+5053]; temp += p[count+5021]; temp += p[count+5009]; 
				temp += p[count+4979]; temp += p[count+4961]; temp += p[count+4939]; temp += p[count+4931]; temp += p[count+4903]; temp += p[count+4891]; temp += p[count+4877]; temp += p[count+4871]; temp += p[count+4861]; temp += p[count+4849]; 
			case 12:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+4813]; temp += p[count+4799]; temp += p[count+4777]; temp += p[count+4757]; temp += p[count+4733]; temp += p[count+4703]; temp += p[count+4679]; temp += p[count+4657]; temp += p[count+4643]; temp += p[count+4631]; 
				temp += p[count+4601]; temp += p[count+4591]; temp += p[count+4577]; temp += p[count+4547]; temp += p[count+4517]; temp += p[count+4507]; temp += p[count+4489]; temp += p[count+4471]; temp += p[count+4439]; temp += p[count+4421]; 
			case 11:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+4399]; temp += p[count+4367]; temp += p[count+4357]; temp += p[count+4343]; temp += p[count+4331]; temp += p[count+4307]; temp += p[count+4289]; temp += p[count+4279]; temp += p[count+4243]; temp += p[count+4223]; 
				temp += p[count+4213]; temp += p[count+4187]; temp += p[count+4181]; temp += p[count+4159]; temp += p[count+4147]; temp += p[count+4133]; temp += p[count+4103]; temp += p[count+4097]; temp += p[count+4087]; temp += p[count+4073]; 
			case 10:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+4033]; temp += p[count+4009]; temp += p[count+4001]; temp += p[count+3979]; temp += p[count+3961]; temp += p[count+3937]; temp += p[count+3919]; temp += p[count+3893]; temp += p[count+3877]; temp += p[count+3851]; 
				temp += p[count+3833]; temp += p[count+3821]; temp += p[count+3799]; temp += p[count+3791]; temp += p[count+3769]; temp += p[count+3757]; temp += p[count+3733]; temp += p[count+3721]; temp += p[count+3707]; temp += p[count+3691]; 
			case 9:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+3677]; temp += p[count+3667]; temp += p[count+3653]; temp += p[count+3631]; temp += p[count+3613]; temp += p[count+3593]; temp += p[count+3581]; temp += p[count+3559]; temp += p[count+3553]; temp += p[count+3523]; 
				temp += p[count+3503]; temp += p[count+3469]; temp += p[count+3457]; temp += p[count+3443]; temp += p[count+3431]; temp += p[count+3419]; temp += p[count+3391]; temp += p[count+3377]; temp += p[count+3341]; temp += p[count+3329]; 
			case 8:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+3299]; temp += p[count+3289]; temp += p[count+3281]; temp += p[count+3253]; temp += p[count+3229]; temp += p[count+3221]; temp += p[count+3191]; temp += p[count+3167]; temp += p[count+3151]; temp += p[count+3139]; 
				temp += p[count+3121]; temp += p[count+3113]; temp += p[count+3103]; temp += p[count+3083]; temp += p[count+3067]; temp += p[count+3043]; temp += p[count+3013]; temp += p[count+3007]; temp += p[count+2993]; temp += p[count+2981]; 
			case 7:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+2963]; temp += p[count+2957]; temp += p[count+2911]; temp += p[count+2897]; temp += p[count+2879]; temp += p[count+2861]; temp += p[count+2837]; temp += p[count+2801]; temp += p[count+2777]; temp += p[count+2767]; 
				temp += p[count+2731]; temp += p[count+2707]; temp += p[count+2683]; temp += p[count+2671]; temp += p[count+2663]; temp += p[count+2647]; temp += p[count+2623]; temp += p[count+2603]; temp += p[count+2579]; temp += p[count+2563]; 
			case 6:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+2533]; temp += p[count+2509]; temp += p[count+2497]; temp += p[count+2479]; temp += p[count+2459]; temp += p[count+2441]; temp += p[count+2431]; temp += p[count+2419]; temp += p[count+2377]; temp += p[count+2353]; 
				temp += p[count+2341]; temp += p[count+2329]; temp += p[count+2299]; temp += p[count+2293]; temp += p[count+2267]; temp += p[count+2251]; temp += p[count+2239]; temp += p[count+2227]; temp += p[count+2183]; temp += p[count+2167]; 
			case 5:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+2153]; temp += p[count+2129]; temp += p[count+2117]; temp += p[count+2083]; temp += p[count+2069]; temp += p[count+2047]; temp += p[count+2039]; temp += p[count+2027]; temp += p[count+2011]; temp += p[count+1969]; 
				temp += p[count+1951]; temp += p[count+1919]; temp += p[count+1909]; temp += p[count+1891]; temp += p[count+1871]; temp += p[count+1847]; temp += p[count+1837]; temp += p[count+1819]; temp += p[count+1807]; temp += p[count+1787]; 
			case 4:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+1777]; temp += p[count+1747]; temp += p[count+1711]; temp += p[count+1693]; temp += p[count+1679]; temp += p[count+1657]; temp += p[count+1639]; temp += p[count+1633]; temp += p[count+1609]; temp += p[count+1579]; 
				temp += p[count+1549]; temp += p[count+1523]; temp += p[count+1513]; temp += p[count+1501]; temp += p[count+1483]; temp += p[count+1469]; temp += p[count+1451]; temp += p[count+1439]; temp += p[count+1427]; temp += p[count+1403]; 
			case 3:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+1373]; temp += p[count+1357]; temp += p[count+1343]; temp += p[count+1331]; temp += p[count+1319]; temp += p[count+1291]; temp += p[count+1279]; temp += p[count+1271]; temp += p[count+1249]; temp += p[count+1241]; 
				temp += p[count+1217]; temp += p[count+1187]; temp += p[count+1147]; temp += p[count+1121]; temp += p[count+1109]; temp += p[count+1087]; temp += p[count+1067]; temp += p[count+1051]; temp += p[count+1037]; temp += p[count+1019]; 
			case 2:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+1009]; temp += p[count+991]; temp += p[count+971]; temp += p[count+961]; temp += p[count+947]; temp += p[count+923]; temp += p[count+913]; temp += p[count+899]; temp += p[count+869]; temp += p[count+857]; 
				temp += p[count+829]; temp += p[count+811]; temp += p[count+803]; temp += p[count+793]; temp += p[count+767]; temp += p[count+727]; temp += p[count+713]; temp += p[count+691]; temp += p[count+671]; temp += p[count+653]; 
			case 1:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+643]; temp += p[count+619]; temp += p[count+613]; temp += p[count+601]; temp += p[count+593]; temp += p[count+577]; temp += p[count+557]; temp += p[count+529]; temp += p[count+521]; temp += p[count+503]; 
				temp += p[count+491]; temp += p[count+463]; temp += p[count+457]; temp += p[count+421]; temp += p[count+407]; temp += p[count+389]; temp += p[count+379]; temp += p[count+361]; temp += p[count+341]; temp += p[count+331]; 
			case 0:
				temp = (SInt32)((Float32)temp/decay);
				temp += p[count+313]; temp += p[count+289]; temp += p[count+271]; temp += p[count+239]; temp += p[count+223]; temp += p[count+209]; temp += p[count+191]; temp += p[count+181]; temp += p[count+151]; temp += p[count+143]; 
				temp += p[count+121]; temp += p[count+109]; temp += p[count+73]; temp += p[count+47]; temp += p[count+41]; temp += p[count+19]; temp += p[count+11]; 
				//Highest Offset = 12679: gcount limit is 12680, Reset() goes to 25360, definition in .h must be p[25361]
		}
		gcount--;
		inputSample = (inputSample*(1-wet))+((Float32)(temp/(8388352.0f*scale))*wet);

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 25360; count++) {p[count] = 0;}
	gcount = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
