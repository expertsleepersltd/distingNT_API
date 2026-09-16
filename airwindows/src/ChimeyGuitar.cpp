#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ChimeyGuitar"
#define AIRWINDOWS_DESCRIPTION "A supremely compressible instrument amp."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','s' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	kParam_H =7,
	kParam_I =8,
	kParam_J =9,
	//Add your parameters here...
	kNumberOfParameters=10
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Compres", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Presnce", .min = 0, .max = 1000, .def = 800, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "High", .min = 0, .max = 1000, .def = 700, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low", .min = 0, .max = 1000, .def = 700, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sub", .min = 0, .max = 1000, .def = 400, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HSpeakr", .min = 0, .max = 1000, .def = 800, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LSpeakr", .min = 0, .max = 1000, .def = 400, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 300, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float angS[22][12];
		float angA[22][12];
		float angG[12];
		
		enum {
			bez_AL,
			bez_BL,
			bez_CL,
			bez_InL,
			bez_UnInL,
			bez_SampL,
			bez_cycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		float bezComp[bez_total][12];
		float bezRezA, bezRezB;
		uint32_t fpd;
	
	struct _dram {
			float iirHPosition[37];
		float iirHAngle[37];
		float iirBPosition[37];
		float iirBAngle[37];
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
	
	int poles = (int)(powf(GetParameter( kParam_A ),2)*20.0f);
	angG[0] = sqrt(GetParameter( kParam_B )*2.0f);
	angG[2] = sqrt(GetParameter( kParam_C )*2.0f);
	angG[4] = sqrt(GetParameter( kParam_D )*2.0f);
	angG[6] = sqrt(GetParameter( kParam_E )*2.0f);
	angG[8] = sqrt(GetParameter( kParam_F )*2.0f);
	angG[1] = (angG[0]+angG[2])*0.5f;
	angG[3] = (angG[2]+angG[4])*0.5f;
	angG[5] = (angG[4]+angG[6])*0.5f;
	angG[7] = (angG[6]+angG[8])*0.5f;
	angG[9] = angG[8];
	float hFreq = powf(GetParameter( kParam_G ),overallscale);
	float lFreq = powf(GetParameter( kParam_H ),overallscale+3.0f);
	bezRezA = bezRezB; bezRezB = ((powf(GetParameter( kParam_I ),4.0f)*0.5f)+0.0002f)/overallscale;
	if (bezRezB > 1.0f) bezRezB = 1.0f;
	float output = GetParameter( kParam_J );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float bezRez = (bezRezA*temp)+(bezRezB*(1.0f-temp));
		
		float CBAL = 0.0f;
		for (int x = 0; x < poles; x++) {
			float fr = 0.9f/overallscale;
			float band = inputSample; inputSample = 0.0f;
			for (int y = 0; y < 9; y++) {
				angA[x][y] = (angA[x][y]*(1.0f-fr)) + ((band-angS[x][y])*fr);
				float temp = band; band = ((angS[x][y]+(angA[x][y]*fr)) * (1.0f-fr))+(band*fr);
				angS[x][y] = ((angS[x][y]+(angA[x][y]*fr)) * (1.0f-fr))+(band*fr);
				inputSample += ((temp-band)*angG[y]);
				fr *= 0.618033988749894f;
				bezComp[bez_cycle][y] += bezRez;
				bezComp[bez_SampL][y] += (fabs(inputSample) * bezRez);
				if (bezComp[bez_cycle][y] > 1.0f) {
					bezComp[bez_cycle][y] -= 1.0f;
					bezComp[bez_CL][y] = bezComp[bez_BL][y];
					bezComp[bez_BL][y] = bezComp[bez_AL][y];
					bezComp[bez_AL][y] = bezComp[bez_SampL][y];
					bezComp[bez_SampL][y] = 0.0f;
				}
				float CBL = (bezComp[bez_CL][y]*(1.0f-bezComp[bez_cycle][y]))+(bezComp[bez_BL][y]*bezComp[bez_cycle][y]);
				float BAL = (bezComp[bez_BL][y]*(1.0f-bezComp[bez_cycle][y]))+(bezComp[bez_AL][y]*bezComp[bez_cycle][y]);
				CBAL += (bezComp[bez_BL][y]+(CBL*(1.0f-bezComp[bez_cycle][y]))+(BAL*bezComp[bez_cycle][y]))*0.5f;
				inputSample *= 1.0f-(fmin(CBAL*0.01f,1.0f));
			}
			inputSample += (band*angG[9]);
		}
		inputSample = sin(inputSample);
				
		float lowSample = inputSample;
		for(int count = 0; count < (3.0f+(lFreq*32.0f)); count++) {
			dram->iirBAngle[count] = (dram->iirBAngle[count]*(1.0f-lFreq))+((lowSample-dram->iirBPosition[count])*lFreq);
			lowSample = ((dram->iirBPosition[count]+(dram->iirBAngle[count]*lFreq))*(1.0f-lFreq))+(lowSample*lFreq);
			dram->iirBPosition[count] = ((dram->iirBPosition[count]+(dram->iirBAngle[count]*lFreq))*(1.0f-lFreq))+(lowSample*lFreq);
			inputSample -= (lowSample * (1.0f/(3.0f+(lFreq*32.0f))) );
		}
		
		for(int count = 0; count < (3.0f+(hFreq*32.0f)); count++) {
			dram->iirHAngle[count] = (dram->iirHAngle[count]*(1.0f-hFreq))+((inputSample-dram->iirHPosition[count])*hFreq);
			inputSample = ((dram->iirHPosition[count]+(dram->iirHAngle[count]*hFreq))*(1.0f-hFreq))+(inputSample*hFreq);
			dram->iirHPosition[count] = ((dram->iirHPosition[count]+(dram->iirHAngle[count]*hFreq))*(1.0f-hFreq))+(inputSample*hFreq);
		} //the lowpass	
		inputSample *= output;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int x=0; x<21; x++) {
		for(int y=0; y<11; y++) {
			angS[x][y] = 0.0;angA[x][y] = 0.0;
		}
	}
	for(int y=0; y<11; y++) {
		angG[y] = 0.0;
		for (int x = 0; x < bez_total; x++) {bezComp[x][y] = 0.0;}
		bezComp[bez_cycle][y] = 1.0;
	}
	bezRezA = bezRezB = 0.0002;
	
	for(int count = 0; count < 36; count++) {
		dram->iirHPosition[count] = 0.0;
		dram->iirHAngle[count] = 0.0;
		dram->iirBPosition[count] = 0.0;
		dram->iirBAngle[count] = 0.0;
	}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
