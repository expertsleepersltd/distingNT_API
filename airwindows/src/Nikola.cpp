#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Nikola"
#define AIRWINDOWS_DESCRIPTION "An Audio Tesla Coil plugin! Note: audio tesla coils don't sound nice."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','N','i','a' )
#define AIRWINDOWS_TAGS kNT_tagEffect
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Voltage", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		int framenumber;
		bool WasNegative;
		Float32 outlevel;
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

	Float32 decay = powf(GetParameter( kParam_One ),3)*32767.0f;
	int HighsBoost = 16 - (int)(powf(GetParameter( kParam_One ),2)*16);
	Float32 wet = GetParameter( kParam_Two );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		Float32 bridgerectifier = fabs(inputSample);
		if (bridgerectifier > outlevel) outlevel = bridgerectifier;
		else outlevel = ((outlevel * decay) + bridgerectifier) / (decay+1.0f);
		//set up envelope for handling quieter stuff if desired
		
		framenumber += 1;
		//we're having the pulse wave start with a 0 if it's starting
		
		if (inputSample > 0) {if (WasNegative) framenumber = HighsBoost; WasNegative = false;}
		else WasNegative = true;
		//our crossover trigger. If framenumber is 0, we've just gone into the positive cycle of the wave.
		
		if ((framenumber > 0) && (framenumber < 469))
		{
			switch(framenumber)
			{
				case 0: inputSample = 0.0f; break;
				case 1: inputSample = 0.043397916539811392f; break;
				case 2: inputSample = 0.083011537904293956f; break;
				case 3: inputSample = 0.115117486724753554f; break;
				case 4: inputSample = 0.129278313547471546f; break;
				case 5: inputSample = 0.126867351298867753f; break;
				case 6: inputSample = 0.115941486480605482f; break;
				case 7: inputSample = 0.117162226859645377f; break;
				case 8: inputSample = 0.152472738388012313f; break;
				case 9: inputSample = 0.226817019600817904f; break;
				case 10: inputSample = 0.362687450407727319f; break;
				case 11: inputSample = 0.548425960188909545f; break;
				case 12: inputSample = 0.761967189741508233f; break;
				case 13: inputSample = 0.963667118404492373f; break;
				case 14: inputSample = 0.970747531815851383f; break;
				case 15: inputSample = 0.978133130321970246f; break;
				case 16: inputSample = 0.98234468462965796f; break;
				case 17: inputSample = 0.779637645153965941f; break;
				case 18: inputSample = 0.420765008431043475f; break;
				case 19: inputSample = -0.006744709806817921f; break;
				case 20: inputSample = -0.427998133602099762f; break;
				case 21: inputSample = -0.776677468947111538f; break;
				case 22: inputSample = -0.970656095499740634f; break;
				case 23: inputSample = -0.976790435117343758f; break;
				case 24: inputSample = -0.982924655522019242f; break;
				case 25: inputSample = -0.913249825471968868f; break;
				case 26: inputSample = -0.707521453558153102f; break;
				case 27: inputSample = -0.498252815332194066f; break;
				case 28: inputSample = -0.319930422566606754f; break;
				case 29: inputSample = -0.204965528389538354f; break;
				case 30: inputSample = -0.152533894619281685f; break;
				case 31: inputSample = -0.14597229586901464f; break;
				case 32: inputSample = -0.169685535370647372f; break;
				case 33: inputSample = -0.198434448148747306f; break;
				case 34: inputSample = -0.212503719443037281f; break;
				case 35: inputSample = -0.200692817849971089f; break;
				case 36: inputSample = -0.16089608542863254f; break;
				case 37: inputSample = -0.098973075998413124f; break;
				case 38: inputSample = -0.028993060853602797f; break;
				case 39: inputSample = 0.031831282235480818f; break;
				case 40: inputSample = 0.083896574679097863f; break;
				case 41: inputSample = 0.115025931196325565f; break;
				case 42: inputSample = 0.11221822832453382f; break;
				case 43: inputSample = 0.082187538148442027f; break;
				case 44: inputSample = 0.023194305627918334f; break;
				case 45: inputSample = -0.057436788536942743f; break;
				case 46: inputSample = -0.148383258117618433f; break;
				case 47: inputSample = -0.230754026535538892f; break;
				case 48: inputSample = -0.291364740058290439f; break;
				case 49: inputSample = -0.32316550378399006f; break;
				case 50: inputSample = -0.312972202406079369f; break;
				case 51: inputSample = -0.270215055352641453f; break;
				case 52: inputSample = -0.196267633975951511f; break;
				case 53: inputSample = -0.109166377376323828f; break;
				case 54: inputSample = -0.026582098604999018f; break;
				case 55: inputSample = 0.038179132206488234f; break;
				case 56: inputSample = 0.078281168935514386f; break;
				case 57: inputSample = 0.090061432806176941f; break;
				case 58: inputSample = 0.082523241752678f; break;
				case 59: inputSample = 0.0721773478273873f; break;
				case 60: inputSample = 0.060427483253273108f; break;
				case 61: inputSample = 0.064822267830744337f; break;
				case 62: inputSample = 0.081088871807306132f; break;
				case 63: inputSample = 0.107609814180730609f; break;
				case 64: inputSample = 0.136328208449354532f; break;
				case 65: inputSample = 0.15158770161320842f; break;
				case 66: inputSample = 0.151129923971068447f; break;
				case 67: inputSample = 0.128881572924283566f; break;
				case 68: inputSample = 0.094791920987884151f; break;
				case 69: inputSample = 0.059725557535325172f; break;
				case 70: inputSample = 0.034059133427228612f; break;
				case 71: inputSample = 0.030366274567705312f; break;
				case 72: inputSample = 0.044954360523087246f; break;
				case 73: inputSample = 0.07425260647175512f; break;
				case 74: inputSample = 0.103733963477278965f; break;
				case 75: inputSample = 0.112737042985625785f; break;
				case 76: inputSample = 0.086490767197485274f; break;
				case 77: inputSample = 0.019135343867610705f; break;
				case 78: inputSample = -0.083164249663991302f; break;
				case 79: inputSample = -0.19562674527695556f; break;
				case 80: inputSample = -0.283765511985839491f; break;
				case 81: inputSample = -0.322799281670278104f; break;
				case 82: inputSample = -0.288312769897763055f; break;
				case 83: inputSample = -0.171516645939207207f; break;
				case 84: inputSample = 0.015076262894375438f; break;
				case 85: inputSample = 0.243449845691091654f; break;
				case 86: inputSample = 0.469503902554704478f; break;
				case 87: inputSample = 0.648131361202124112f; break;
				case 88: inputSample = 0.74451036389538261f; break;
				case 89: inputSample = 0.733553980567644315f; break;
				case 90: inputSample = 0.618070271729483922f; break;
				case 91: inputSample = 0.409137337107760901f; break;
				case 92: inputSample = 0.135046431051362659f; break;
				case 93: inputSample = -0.165931758705099741f; break;
				case 94: inputSample = -0.451131521423688564f; break;
				case 95: inputSample = -0.68655487990936015f; break;
				case 96: inputSample = -0.842262699514450652f; break;
				case 97: inputSample = -0.900889709921262383f; break;
				case 98: inputSample = -0.856484563356120554f; break;
				case 99: inputSample = -0.716158310952787924f; break;
				case 100: inputSample = -0.500877526360057446f; break;
				case 101: inputSample = -0.240825253875545597f; break;
				case 102: inputSample = 0.024720350314645832f; break;
				case 103: inputSample = 0.264263707579577012f; break;
				case 104: inputSample = 0.44478343302743617f; break;
				case 105: inputSample = 0.540826732116458708f; break;
				case 106: inputSample = 0.542444213118686513f; break;
				case 107: inputSample = 0.451497624325083169f; break;
				case 108: inputSample = 0.278241423345439054f; break;
				case 109: inputSample = 0.048921885967894527f; break;
				case 110: inputSample = -0.203348047387310493f; break;
				case 111: inputSample = -0.445638189718009059f; break;
				case 112: inputSample = -0.64224140808557395f; break;
				case 113: inputSample = -0.765202270958281305f; break;
				case 114: inputSample = -0.791509702978301477f; break;
				case 115: inputSample = -0.713472682118900248f; break;
				case 116: inputSample = -0.537011918431348656f; break;
				case 117: inputSample = -0.286664770386059253f; break;
				case 118: inputSample = 0.001312295907773064f; break;
				case 119: inputSample = 0.277692090174871065f; break;
				case 120: inputSample = 0.500450148015076146f; break;
				case 121: inputSample = 0.634519986762901778f; break;
				case 122: inputSample = 0.666290231979125402f; break;
				case 123: inputSample = 0.602291963904538097f; break;
				case 124: inputSample = 0.46706230258369702f; break;
				case 125: inputSample = 0.301069626072267837f; break;
				case 126: inputSample = 0.147223435545213166f; break;
				case 127: inputSample = 0.037049947355876335f; break;
				case 128: inputSample = -0.013611493651844931f; break;
				case 129: inputSample = -0.00317404419812625f; break;
				case 130: inputSample = 0.051943218406018246f; break;
				case 131: inputSample = 0.123662788590960415f; break;
				case 132: inputSample = 0.190804582354503011f; break;
				case 133: inputSample = 0.234874025315408796f; break;
				case 134: inputSample = 0.243846586314279606f; break;
				case 135: inputSample = 0.221018383587450795f; break;
				case 136: inputSample = 0.177162570192876984f; break;
				case 137: inputSample = 0.126470610675679801f; break;
				case 138: inputSample = 0.080417464598834185f; break;
				case 139: inputSample = 0.042940138897671433f; break;
				case 140: inputSample = 0.016022455901058993f; break;
				case 141: inputSample = -0.009033717230445352f; break;
				case 142: inputSample = -0.03940011101077312f; break;
				case 143: inputSample = -0.075076725439924399f; break;
				case 144: inputSample = -0.117406613360698353f; break;
				case 145: inputSample = -0.168007017349467547f; break;
				case 146: inputSample = -0.220163984534440221f; break;
				case 147: inputSample = -0.262280123675954546f; break;
				case 148: inputSample = -0.287580325670339143f; break;
				case 149: inputSample = -0.293714665287942212f; break;
				case 150: inputSample = -0.282361660549943605f; break;
				case 151: inputSample = -0.254314673489791643f; break;
				case 152: inputSample = -0.219767243911252269f; break;
				case 153: inputSample = -0.186287842951445126f; break;
				case 154: inputSample = -0.157478012367320869f; break;
				case 155: inputSample = -0.1383120307776117f; break;
				case 156: inputSample = -0.129919321458784848f; break;
				case 157: inputSample = -0.131048506309396739f; break;
				case 158: inputSample = -0.136084179585863912f; break;
				case 159: inputSample = -0.142035408146611003f; break;
				case 160: inputSample = -0.148047554513382446f; break;
				case 161: inputSample = -0.147711850909146486f; break;
				case 162: inputSample = -0.140387289421979505f; break;
				case 163: inputSample = -0.126775914982757115f; break;
				case 164: inputSample = -0.108525488677327892f; break;
				case 165: inputSample = -0.087436960203558539f; break;
				case 166: inputSample = -0.065585349779961638f; break;
				case 167: inputSample = -0.045076553773308596f; break;
				case 168: inputSample = -0.026612617114475014f; break;
				case 169: inputSample = -0.012787493895993006f; break;
				case 170: inputSample = -0.005066191785638076f; break;
				case 171: inputSample = -0.001739674252754382f; break;
				case 172: inputSample = -0.001709155743278385f; break;
				case 173: inputSample = -0.002258488913846334f; break;
				case 174: inputSample = -0.001281896610614424f; break;
				case 175: inputSample = 0.005188146611224708f; break;
				case 176: inputSample = 0.018250307092806787f; break;
				case 177: inputSample = 0.038148613697012237f; break;
				case 178: inputSample = 0.065676786096072262f; break;
				case 179: inputSample = 0.097691179388103885f; break;
				case 180: inputSample = 0.128820535905331574f; break;
				case 181: inputSample = 0.156012885487227998f; break;
				case 182: inputSample = 0.177498273797112943f; break;
				case 183: inputSample = 0.189003990295419172f; break;
				case 184: inputSample = 0.18885139774803919f; break;
				case 185: inputSample = 0.17816968100558489f; break;
				case 186: inputSample = 0.156806485946531571f; break;
				case 187: inputSample = 0.127172536393627744f; break;
				case 188: inputSample = 0.09524969863002411f; break;
				case 189: inputSample = 0.066470267342448194f; break;
				case 190: inputSample = 0.043977768219855336f; break;
				case 191: inputSample = 0.032014393292336796f; break;
				case 192: inputSample = 0.033570837275612657f; break;
				case 193: inputSample = 0.04724336794671468f; break;
				case 194: inputSample = 0.070163006989043847f; break;
				case 195: inputSample = 0.096043179876400028f; break;
				case 196: inputSample = 0.117650642224188973f; break;
				case 197: inputSample = 0.127172536393627744f; break;
				case 198: inputSample = 0.120427826586504708f; break;
				case 199: inputSample = 0.09720288323648793f; break;
				case 200: inputSample = 0.059634002006897183f; break;
				case 201: inputSample = 0.0144048556859035f; break;
				case 202: inputSample = -0.029817060609454722f; break;
				case 203: inputSample = -0.064639275986205724f; break;
				case 204: inputSample = -0.083866175381939231f; break;
				case 205: inputSample = -0.085514174893643075f; break;
				case 206: inputSample = -0.071841763435468692f; break;
				case 207: inputSample = -0.050936226805627698f; break;
				case 208: inputSample = -0.028443727683034846f; break;
				case 209: inputSample = -0.010468087175817219f; break;
				case 210: inputSample = -0.002105896366466348f; break;
				case 211: inputSample = -0.003662340349742205f; break;
				case 212: inputSample = -0.01220764221594906f; break;
				case 213: inputSample = -0.022248351046479778f; break;
				case 214: inputSample = -0.029847579118930719f; break;
				case 215: inputSample = -0.031343105296182221f; break;
				case 216: inputSample = -0.024750988036439187f; break;
				case 217: inputSample = -0.013550456632892937f; break;
				case 218: inputSample = -0.000854637477950463f; break;
				case 219: inputSample = 0.010071227340311899f; break;
				case 220: inputSample = 0.016663344600054933f; break;
				case 221: inputSample = 0.016846455656910918f; break;
				case 222: inputSample = 0.01190233790887173f; break;
				case 223: inputSample = 0.006378368480788599f; break;
				case 224: inputSample = 0.004242072817468795f; break;
				case 225: inputSample = 0.007934931676992095f; break;
				case 226: inputSample = 0.01995934362346263f; break;
				case 227: inputSample = 0.041017472800683609f; break;
				case 228: inputSample = 0.067507896664632097f; break;
				case 229: inputSample = 0.095280217139500106f; break;
				case 230: inputSample = 0.119237604716940823f; break;
				case 231: inputSample = 0.134863319994506653f; break;
				case 232: inputSample = 0.139074874302194285f; break;
				case 233: inputSample = 0.130285424360179453f; break;
				case 234: inputSample = 0.109227295182958456f; break;
				case 235: inputSample = 0.077731716552018798f; break;
				case 236: inputSample = 0.038667428358104189f; break;
				case 237: inputSample = -0.002929896122318273f; break;
				case 238: inputSample = -0.042360406429944847f; break;
				case 239: inputSample = -0.075229317987304381f; break;
				case 240: inputSample = -0.097386113505661273f; break;
				case 241: inputSample = -0.105595711767632161f; break;
				case 242: inputSample = -0.098606853884701154f; break;
				case 243: inputSample = -0.07764028023590816f; break;
				case 244: inputSample = -0.046632997756584457f; break;
				case 245: inputSample = -0.011505716498001124f; break;
				case 246: inputSample = 0.021668380154118472f; break;
				case 247: inputSample = 0.048555663854182557f; break;
				case 248: inputSample = 0.061800816179692977f; break;
				case 249: inputSample = 0.060244372196417123f; break;
				case 250: inputSample = 0.046663397053743089f; break;
				case 251: inputSample = 0.024995016899929806f; break;
				case 252: inputSample = 0.000061037019257179f; break;
				case 253: inputSample = -0.022461980612811756f; break;
				case 254: inputSample = -0.036927992530289708f; break;
				case 255: inputSample = -0.038942214155705521f; break;
				case 256: inputSample = -0.026917802209234988f; break;
				case 257: inputSample = -0.002624711027558301f; break;
				case 258: inputSample = 0.029603311830805383f; break;
				case 259: inputSample = 0.064456045717032381f; break;
				case 260: inputSample = 0.096531476028015983f; break;
				case 261: inputSample = 0.120824567209692674f; break;
				case 262: inputSample = 0.132910135388042833f; break;
				case 263: inputSample = 0.129858165227515476f; break;
				case 264: inputSample = 0.111302673040253916f; break;
				case 265: inputSample = 0.080173316523026214f; break;
				case 266: inputSample = 0.042482361255531474f; break;
				case 267: inputSample = 0.006225775933408613f; break;
				case 268: inputSample = -0.020295166440015958f; break;
				case 269: inputSample = -0.030457949308450662f; break;
				case 270: inputSample = -0.020844499610583905f; break;
				case 271: inputSample = 0.007202368236640523f; break;
				case 272: inputSample = 0.048006330683614609f; break;
				case 273: inputSample = 0.092655506111636698f; break;
				case 274: inputSample = 0.13120097964445937f; break;
				case 275: inputSample = 0.154975256165044095f; break;
				case 276: inputSample = 0.15885122608142338f; break;
				case 277: inputSample = 0.142798251671193588f; break;
				case 278: inputSample = 0.111333191549729912f; break;
				case 279: inputSample = 0.071811125713675344f; break;
				case 280: inputSample = 0.032472170934476755f; break;
				case 281: inputSample = 0.000732444227729118f; break;
				case 282: inputSample = -0.018616648418836111f; break;
				case 283: inputSample = -0.024903580583819172f; break;
				case 284: inputSample = -0.020722425572679917f; break;
				case 285: inputSample = -0.010681716742149201f; break;
				case 286: inputSample = 0.000030518509781182f; break;
				case 287: inputSample = 0.006958220160832545f; break;
				case 288: inputSample = 0.007538071840876492f; break;
				case 289: inputSample = 0.001831110568865017f; break;
				case 290: inputSample = -0.008179198965117431f; break;
				case 291: inputSample = -0.019501685193640029f; break;
				case 292: inputSample = -0.028443727683034846f; break;
				case 293: inputSample = -0.031678808900418187f; break;
				case 294: inputSample = -0.027467135379802936f; break;
				case 295: inputSample = -0.016083612132328343f; break;
				case 296: inputSample = 0.000518814661397137f; break;
				case 297: inputSample = 0.019593121509750663f; break;
				case 298: inputSample = 0.037782391583300268f; break;
				case 299: inputSample = 0.052126329462874231f; break;
				case 300: inputSample = 0.060946297914365059f; break;
				case 301: inputSample = 0.063601527451704457f; break;
				case 302: inputSample = 0.059939187101657153f; break;
				case 303: inputSample = 0.050386774422742392f; break;
				case 304: inputSample = 0.036714243751640369f; break;
				case 305: inputSample = 0.021729417173070469f; break;
				case 306: inputSample = 0.008453746338084048f; break;
				case 307: inputSample = -0.000579970892666489f; break;
				case 308: inputSample = -0.003814932897122191f; break;
				case 309: inputSample = -0.000793600458998469f; break;
				case 310: inputSample = 0.007507553331400495f; break;
				case 311: inputSample = 0.019654158528702659f; break;
				case 312: inputSample = 0.033692911313516642f; break;
				case 313: inputSample = 0.047579071550950647f; break;
				case 314: inputSample = 0.059542446478469187f; break;
				case 315: inputSample = 0.068881229591051973f; break;
				case 316: inputSample = 0.075961643002410956f; break;
				case 317: inputSample = 0.081821316034730057f; break;
				case 318: inputSample = 0.087864100123905151f; break;
				case 319: inputSample = 0.094944513535264133f; break;
				case 320: inputSample = 0.10254374160771508f; break;
				case 321: inputSample = 0.108342258408154549f; break;
				case 322: inputSample = 0.109257813692434452f; break;
				case 323: inputSample = 0.102574260117191077f; break;
				case 324: inputSample = 0.086032989555345316f; break;
				case 325: inputSample = 0.058748965232093261f; break;
				case 326: inputSample = 0.02255341692892239f; break;
				case 327: inputSample = -0.017701093134556194f; break;
				case 328: inputSample = -0.0557277520062869f; break;
				case 329: inputSample = -0.085025878742027119f; break;
				case 330: inputSample = -0.100071742339549019f; break;
				case 331: inputSample = -0.09766078009094524f; break;
				case 332: inputSample = -0.07873906578997171f; break;
				case 333: inputSample = -0.048220079462263948f; break;
				case 334: inputSample = -0.013458901104464944f; break;
				case 335: inputSample = 0.017304233299050873f; break;
				case 336: inputSample = 0.036439577166356395f; break;
				case 337: inputSample = 0.039033650471816152f; break;
				case 338: inputSample = 0.024567757767265847f; break;
				case 339: inputSample = -0.003753895878170197f; break;
				case 340: inputSample = -0.040620851389813008f; break;
				case 341: inputSample = -0.078555954733115718f; break;
				case 342: inputSample = -0.109135858866847832f; break;
				case 343: inputSample = -0.12497520371074565f; break;
				case 344: inputSample = -0.121618167668385957f; break;
				case 345: inputSample = -0.097599743071993247f; break;
				case 346: inputSample = -0.054140670300607402f; break;
				case 347: inputSample = 0.004272591326944792f; break;
				case 348: inputSample = 0.069857821894283884f; break;
				case 349: inputSample = 0.133306876011230813f; break;
				case 350: inputSample = 0.185463723983275847f; break;
				case 351: inputSample = 0.218088487464827424f; break;
				case 352: inputSample = 0.225413048951994377f; break;
				case 353: inputSample = 0.206582890179448836f; break;
				case 354: inputSample = 0.166389417134922335f; break;
				case 355: inputSample = 0.113957783364665666f; break;
				case 356: inputSample = 0.05987815008270516f; break;
				case 357: inputSample = 0.014435374195379498f; break;
				case 358: inputSample = -0.01416082682241288f; break;
				case 359: inputSample = -0.021790573404339819f; break;
				case 360: inputSample = -0.00991875400524927f; break;
				case 361: inputSample = 0.0144048556859035f; break;
				case 362: inputSample = 0.040742806215399635f; break;
				case 363: inputSample = 0.059176224364757224f; break;
				case 364: inputSample = 0.063784638508560434f; break;
				case 365: inputSample = 0.053255514313486123f; break;
				case 366: inputSample = 0.030244200529801323f; break;
				case 367: inputSample = 0.000610370189825129f; break;
				case 368: inputSample = -0.017395908039796223f; break;
				case 369: inputSample = -0.039918925671865071f; break;
				case 370: inputSample = -0.052797855883663529f; break;
				case 371: inputSample = -0.055544640949430915f; break;
				case 372: inputSample = -0.043153887676320772f; break;
				case 373: inputSample = -0.021210721724295872f; break;
				case 374: inputSample = 0.004119998779564807f; break;
				case 375: inputSample = 0.029237089717093417f; break;
				case 376: inputSample = 0.048982922986846519f; break;
				case 377: inputSample = 0.061739779160740985f; break;
				case 378: inputSample = 0.067141674550920127f; break;
				case 379: inputSample = 0.065615749077120269f; break;
				case 380: inputSample = 0.060305409215369116f; break;
				case 381: inputSample = 0.052034773934446235f; break;
				case 382: inputSample = 0.047731664098330635f; break;
				case 383: inputSample = 0.042390805727103485f; break;
				case 384: inputSample = 0.035096762749412515f; break;
				case 385: inputSample = 0.025819016655781731f; break;
				case 386: inputSample = 0.01278737468367565f; break;
				case 387: inputSample = -0.004303229048738146f; break;
				case 388: inputSample = -0.022461980612811756f; break;
				case 389: inputSample = -0.041200703069856952f; break;
				case 390: inputSample = -0.058138714254890679f; break;
				case 391: inputSample = -0.073215096361888568f; break;
				case 392: inputSample = -0.077213021103244198f; break;
				case 393: inputSample = -0.073001466795556594f; break;
				case 394: inputSample = -0.063937350268257781f; break;
				case 395: inputSample = -0.054171188810083405f; break;
				case 396: inputSample = -0.040773443937192996f; break;
				case 397: inputSample = -0.031739845919370187f; break;
				case 398: inputSample = -0.023682840204779284f; break;
				case 399: inputSample = -0.020508796006347935f; break;
				case 400: inputSample = -0.025910691396527082f; break;
				case 401: inputSample = -0.04406944296060069f; break;
				case 402: inputSample = -0.065646386798913631f; break;
				case 403: inputSample = -0.091160218359630202f; break;
				case 404: inputSample = -0.117101309053010735f; break;
				case 405: inputSample = -0.140478844950407522f; break;
				case 406: inputSample = -0.152778042695089655f; break;
				case 407: inputSample = -0.149024266029541996f; break;
				case 408: inputSample = -0.126867470511185132f; break;
				case 409: inputSample = -0.084934323213599131f; break;
				case 410: inputSample = -0.027711283455610913f; break;
				case 411: inputSample = 0.028779312074953458f; break;
				case 412: inputSample = 0.082279093676870016f; break;
				case 413: inputSample = 0.120580419133884703f; break;
				case 414: inputSample = 0.137945689451582393f; break;
				case 415: inputSample = 0.128332239753715632f; break;
				case 416: inputSample = 0.098209994049195837f; break;
				case 417: inputSample = 0.057345113796197389f; break;
				case 418: inputSample = 0.014282781647999511f; break;
				case 419: inputSample = -0.019440648174688036f; break;
				case 420: inputSample = -0.040712406918240997f; break;
				case 421: inputSample = -0.052675781845759537f; break;
				case 422: inputSample = -0.049166153256019862f; break;
				case 423: inputSample = -0.03909492591601315f; break;
				case 424: inputSample = -0.02404906231849125f; break;
				case 425: inputSample = -0.010071346552629257f; break;
				case 426: inputSample = -0.005920710050965997f; break;
				case 427: inputSample = -0.004547377124546123f; break;
				case 428: inputSample = -0.014008234275032894f; break;
				case 429: inputSample = -0.021515906819055845f; break;
				case 430: inputSample = -0.027009357737662981f; break;
				case 431: inputSample = -0.037538362719809648f; break;
				case 432: inputSample = -0.043520109790032742f; break;
				case 433: inputSample = -0.04736556119693603f; break;
				case 434: inputSample = -0.048159042443311956f; break;
				case 435: inputSample = -0.045076553773308596f; break;
				case 436: inputSample = -0.039216999953917135f; break;
				case 437: inputSample = -0.030946245460066617f; break;
				case 438: inputSample = -0.026490543076571026f; break;
				case 439: inputSample = -0.020234129421063961f; break;
				case 440: inputSample = -0.012085568178045071f; break;
				case 441: inputSample = -0.007660265091097837f; break;
				case 442: inputSample = -0.005615524956206025f; break;
				case 443: inputSample = -0.005218784333018062f; break;
				case 444: inputSample = -0.008698013626209382f; break;
				case 445: inputSample = -0.015931019584948358f; break;
				case 446: inputSample = -0.026521061586047025f; break;
				case 447: inputSample = -0.036165029793389779f; break;
				case 448: inputSample = -0.043733739356364723f; break;
				case 449: inputSample = -0.04843370902859593f; break;
				case 450: inputSample = -0.048616820085451914f; break;
				case 451: inputSample = -0.043031813638416787f; break;
				case 452: inputSample = -0.032167105052034142f; break;
				case 453: inputSample = -0.019288055627308048f; break;
				case 454: inputSample = -0.00415063650135816f; break;
				case 455: inputSample = 0.010467967963499863f; break;
				case 456: inputSample = 0.020539195303506577f; break;
				case 457: inputSample = 0.025696942617877742f; break;
				case 458: inputSample = 0.027375460639057589f; break;
				case 459: inputSample = 0.023621683973509933f; break;
				case 460: inputSample = 0.016358159505294963f; break;
				case 461: inputSample = 0.008911523980224005f; break;
				case 462: inputSample = 0.002319406720480971f; break;
				case 463: inputSample = -0.002716266555986292f; break;
				case 464: inputSample = -0.00589019154149f; break;
				case 465: inputSample = -0.006683672787865927f; break;
				case 466: inputSample = -0.005279821351970056f; break;
				case 467: inputSample = -0.003387673764458231f; break;
				case 468: inputSample = -0.001281896610614424f; break;
			}
		}
				
		inputSample *= outlevel;
		//if envelope is in use, reduce by amount of quietness in source
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f - wet));
		}

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	WasNegative = false;
	outlevel = 0.0;
	framenumber = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
