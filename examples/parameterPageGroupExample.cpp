
#include <math.h>
#include <string.h>
#include <new>
#include <distingnt/api.h>

struct _parameterPageGroupExample : public _NT_algorithm
{
	_parameterPageGroupExample() {}
	~_parameterPageGroupExample() {}
};

enum
{
	kParamInput,
	kParamOutput,
	kParamOutputMode,
	kParamPage1A,
	kParamPage1B,
	kParamPage1C,
	kParamPage2A,
	kParamPage2B,
	kParamPage2C,
	kParamPage3A,
	kParamPage3B,
	kParamPage3C,
};

static const _NT_parameter	parameters[] = {
	NT_PARAMETER_AUDIO_INPUT( "Input", 1, 1 )
	NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output", 1, 13 )
	{ .name = "P 1 A", .min = 0, .max = 100, .def = 50, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "P 1 B", .min = 0, .max = 100, .def = 50, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "P 1 C", .min = 0, .max = 100, .def = 50, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "P 2 A", .min = 0, .max = 100, .def = 50, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "P 2 B", .min = 0, .max = 100, .def = 50, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "P 2 C", .min = 0, .max = 100, .def = 50, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "P 3 A", .min = 0, .max = 100, .def = 50, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "P 3 B", .min = 0, .max = 100, .def = 50, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "P 3 C", .min = 0, .max = 100, .def = 50, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamPage1A, kParamPage1B, kParamPage1C };
static const uint8_t page2[] = { kParamPage2A, kParamPage2B, kParamPage2C };
static const uint8_t page3[] = { kParamPage3A, kParamPage3B, kParamPage3C };
static const uint8_t page4[] = { kParamInput, kParamOutput, kParamOutputMode };

static const _NT_parameterPage pages[] = {
	{ .name = "Page 1 (group 1)", .numParams = ARRAY_SIZE(page1), .group = 1, .params = page1 },
	{ .name = "Page 2 (group 1)", .numParams = ARRAY_SIZE(page2), .group = 1, .params = page2 },
	{ .name = "Page 3 (group 2)", .numParams = ARRAY_SIZE(page3), .group = 2, .params = page3 },
	{ .name = "Routing", .numParams = ARRAY_SIZE(page4), .group = 3, .params = page4 },
};

static const _NT_parameterPages parameterPages = {
	.numPages = ARRAY_SIZE(pages),
	.pages = pages,
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_parameterPageGroupExample);
	req.dram = 0;
	req.dtc = 0;
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	_parameterPageGroupExample* alg = new (ptrs.sram) _parameterPageGroupExample();
	alg->parameters = parameters;
	alg->parameterPages = &parameterPages;
	return alg;
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_parameterPageGroupExample* pThis = (_parameterPageGroupExample*)self;
	int numFrames = numFramesBy4 * 4;
//	const float* in = busFrames + ( pThis->v[kParamInput] - 1 ) * numFrames;
	float* out = busFrames + ( pThis->v[kParamOutput] - 1 ) * numFrames;
	bool replace = pThis->v[kParamOutputMode];
	if ( replace )
		memset( out, 0, numFrames * sizeof(float) );
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 'p', 'p' ),
	.name = "Page group demo",
	.description = "Example of using parameter page groups",
	.numSpecifications = 0,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.step = step,
	.tags = kNT_tagUtility,
};

uintptr_t pluginEntry( _NT_selector selector, uint32_t data )
{
	switch ( selector )
	{
	case kNT_selector_version:
		return kNT_apiVersionCurrent;
	case kNT_selector_numFactories:
		return 1;
	case kNT_selector_factoryInfo:
		return (uintptr_t)( ( data == 0 ) ? &factory : NULL );
	}
	return 0;
}
