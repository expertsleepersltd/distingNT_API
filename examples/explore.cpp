
#include <math.h>
#include <new>
#include <distingnt/api.h>
#include <distingnt/slot.h>

struct _exploreAlgorithm : public _NT_algorithm
{
	_exploreAlgorithm() {}
	~_exploreAlgorithm() {}
};

enum
{
	kParamSlot,
	kParamParameter,
};

static const _NT_parameter	parameters[] = {
	{ .name = "Slot", .min = 1, .max = 32, .def = 1, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "Parameter", .min = 1, .max = 256, .def = 1, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamSlot, kParamParameter };

static const _NT_parameterPage pages[] = {
	{ .name = "Explore", .numParams = ARRAY_SIZE(page1), .params = page1 },
};

static const _NT_parameterPages parameterPages = {
	.numPages = ARRAY_SIZE(pages),
	.pages = pages,
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_exploreAlgorithm);
	req.dram = 0;
	req.dtc = 0;
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	_exploreAlgorithm* alg = new (ptrs.sram) _exploreAlgorithm();
	alg->parameters = parameters;
	alg->parameterPages = &parameterPages;
	return alg;
}

bool	draw( _NT_algorithm* self )
{
	_exploreAlgorithm* pThis = (_exploreAlgorithm*)self;
	
	uint32_t s = pThis->v[ kParamSlot ] - 1;
	uint32_t p = pThis->v[ kParamParameter ] - 1;
	
	int y = 30;
	if ( s >= NT_algorithmCount() )
	{
		NT_drawText( 10, y, "Slot out of range" );
	}
	else
	{
		_NT_slot slot;
		NT_getSlot( slot, s );
		NT_drawText( 10, y, slot.name() );
		y += 8;
		uint32_t guid[2];
		guid[0] = slot.guid();
		guid[1] = 0;
		NT_drawText( 10, y, (const char*)guid );
		
		_NT_algorithm* plug = slot.plugin();
		if ( plug )
			NT_drawText( 60, y, "Plug-in" );
		y += 8;
		
		if ( p >= slot.numParameters() )
		{
			NT_drawText( 10, y, "Parameter out of range" );
		}
		else
		{
			_NT_parameter info;
			slot.parameterInfo( info, p );
			int pv = slot.parameterPresetValue( p );
			int v = slot.parameterValue( p );
			char valueString[kNT_parameterStringSize];
			int len = slot.parameterString( p, valueString );
			
			NT_drawText( 10, y, info.name );
			y += 8;
			char buff[16];
			NT_intToString( buff, pv );
			NT_drawText( 10, y, buff );
			NT_drawText( 10 + 6*8, y, ":" );
			NT_intToString( buff, v );
			NT_drawText( 10 + 8*8, y, buff );
			y += 8;
			if ( len > 0 )
				NT_drawText( 10, y, valueString );
		}
	}
			
	return false;
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 'e', 'x' ),
	.name = "Explore",
	.description = "Demo of inspecting other slots",
	.numSpecifications = 0,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.draw = draw,
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
