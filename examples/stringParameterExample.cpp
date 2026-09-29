
#include <math.h>
#include <string.h>
#include <new>
#include <distingnt/api.h>
#include <distingnt/serialisation.h>

struct _stringParameterExample : public _NT_algorithm
{
	_stringParameterExample() {}
	~_stringParameterExample() {}
	
	// storage for the string parameter kParamBar
	char	bar[32];
};

enum
{
	kParamFoo,
	kParamBar,
};

static const _NT_parameter	parameters[] = {
	{ .name = "Foo", .min = 0, .max = 100, .def = 50, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "Bar", .min = 0, .max = 0, .def = 0, .unit = kNT_unitString, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamFoo, kParamBar };

static const _NT_parameterPage pages[] = {
	{ .name = "Params", .numParams = ARRAY_SIZE(page1), .params = page1 },
};

static const _NT_parameterPages parameterPages = {
	.numPages = ARRAY_SIZE(pages),
	.pages = pages,
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_stringParameterExample);
	req.dram = 0;
	req.dtc = 0;
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	_stringParameterExample* alg = new (ptrs.sram) _stringParameterExample();
	alg->parameters = parameters;
	alg->parameterPages = &parameterPages;
	strcpy( alg->bar, "default" );
	return alg;
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
}

void	serialise( _NT_algorithm* self, _NT_jsonStream& stream )
{
	_stringParameterExample* pThis = (_stringParameterExample*)self;
	stream.addMemberName( "bar" );
   	stream.addString( pThis->bar );
}

bool	deserialise( _NT_algorithm* self, _NT_jsonParse& parse )
{
	_stringParameterExample* pThis = (_stringParameterExample*)self;

	int num;
	if ( !parse.numberOfObjectMembers( num ) )
		return false;
	
	for ( int i=0; i<num; ++i )
	{
		if ( parse.matchName( "bar" ) )
		{
			const char* str = NULL;
			if ( !parse.string( str ) || ( str == NULL ) )
				return false;
			strncpy( pThis->bar, str, (sizeof pThis->bar)-1 );
		}
		else
		{
			if ( !parse.skipMember() )
				return false;
		}
	}

	return true;
}

int		stringParameterBuffer( _NT_algorithm* self, int p, char*& buff )
{
	_stringParameterExample* pThis = (_stringParameterExample*)self;
	int size = 0;
	switch ( p )
	{
	case kParamBar:
		buff = pThis->bar;
		size = sizeof pThis->bar;
		break;
	}
	return size;
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 's', 'p' ),
	.name = "String parameter demo",
	.description = "Example of a string-valued parameter",
	.numSpecifications = 0,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.step = step,
	.tags = kNT_tagUtility,
	.serialise = serialise,
	.deserialise = deserialise,
	.stringParameterBuffer = stringParameterBuffer,
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
