
#include <algorithm>
#include <distingnt/api.h>
#include <distingnt/file.h>

enum
{
	kParamDoIt1,
	kParamDoIt2,
	
	kNumParams,
};

static const _NT_parameter	parameters[] = {
	{ .name = "One way", .min = 0, .max = 256, .def = 0, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
	{ .name = "Another way", .min = 0, .max = 64, .def = 0, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamDoIt1, kParamDoIt2 };

static const _NT_parameterPage pages[] = {
	{ .name = "Do it", .numParams = ARRAY_SIZE(page1), .params = page1 },
};

static const _NT_parameterPages parameterPages = {
	.numPages = ARRAY_SIZE(pages),
	.pages = pages,
};

struct _largeSysExDemo : public _NT_algorithm
{
	_largeSysExDemo() {}
	~_largeSysExDemo() {}

	_NT_parameter	params[ kNumParams ];
	
	_NT_sendLargeSysExRequest	request;
	int							state;
	bool						busy;
	
	uint8_t						screen[64][256];
	uint8_t						line[256];

	void			parameterChanged( int p );

	static void		fetchCallback( void* callbackData, const uint8_t*& data, uint32_t& count, bool& end );
	static void		fetchCallback2( void* callbackData, const uint8_t*& data, uint32_t& count, bool& end );
	static void		completeCallback( void* callbackData );
	void			fetch( const uint8_t*& data, uint32_t& count, bool& end );
	void			fetch2( const uint8_t*& data, uint32_t& count, bool& end );
	void			complete(void);
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_largeSysExDemo);
	req.dram = 0;
	req.dtc = 0;
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	static_assert( kNumParams == ARRAY_SIZE(parameters) );

	_largeSysExDemo* alg = new (ptrs.sram) _largeSysExDemo();
	alg->parameters = parameters;
	alg->parameterPages = &parameterPages;

	return alg;
}

void	parameterChanged( _NT_algorithm* self, int p )
{
	((_largeSysExDemo*)self)->parameterChanged( p );
}

void	_largeSysExDemo::parameterChanged( int p )
{
	switch ( p )
	{
	case kParamDoIt1:
		if ( v[kParamDoIt1] && !busy )
		{
			busy = true;
			state = 0;
			request.destination = kNT_destinationUSB;
			request.callbackData = this;
			request.fetchCallback = fetchCallback;
			request.completeCallback = completeCallback;
			NT_sendLargeMidiSysEx( request );
		}
		break;
	case kParamDoIt2:
		if ( v[kParamDoIt2] && !busy )
		{
			busy = true;
			state = 0;
			request.destination = kNT_destinationUSB;
			request.callbackData = this;
			request.fetchCallback = fetchCallback2;
			request.completeCallback = completeCallback;
			NT_sendLargeMidiSysEx( request );
		}
		break;
	}
}

void	_largeSysExDemo::fetchCallback( void* callbackData, const uint8_t*& data, uint32_t& count, bool& end )
{
	((_largeSysExDemo*)callbackData)->fetch( data, count, end );
}

void	_largeSysExDemo::fetchCallback2( void* callbackData, const uint8_t*& data, uint32_t& count, bool& end )
{
	((_largeSysExDemo*)callbackData)->fetch2( data, count, end );
}

void	_largeSysExDemo::completeCallback( void* callbackData )
{
	((_largeSysExDemo*)callbackData)->complete();
}

static const uint8_t header[] = { 0xF0, 0x00, 0x21, 0x27, 0x6D, 0, 0x33, 0 };

void	_largeSysExDemo::fetch( const uint8_t*& data, uint32_t& count, bool& end )
{
	// method 1
	// prepare the whole screen at once and send it in one go
	switch ( state )
	{
	case 0:
		data = header;
		count = ARRAY_SIZE(header);
		end = false;
		state = 1;
		break;
	case 1:
		for ( int x=0; x<256; ++x )
		{
			int b = ( x+1 == v[kParamDoIt1] ) ? 0xf : 0x0;
			for ( int y=0; y<64; ++y )
				screen[y][x] = b;
		}
		data = &screen[0][0];
		count = sizeof screen;
		end = true;
		state = 2;
		break;
	default:
		count = 0;
		end = true;
		break;
	}
}

void	_largeSysExDemo::fetch2( const uint8_t*& data, uint32_t& count, bool& end )
{
	// method 2
	// prepare and send a line at a time
	if ( state >= 1 && state <= 64 )
	{
		int b = ( state == v[kParamDoIt2] ) ? 0xf : 0x0;
		for ( int x=0; x<256; ++x )
		{
			line[x] = b;
		}
		data = &line[0];
		count = sizeof line;
		end = ( state == 64 );
		state += 1;
	}
	else
	switch ( state )
	{
	case 0:
		data = header;
		count = ARRAY_SIZE(header);
		end = false;
		state = 1;
		break;
	default:
		count = 0;
		end = true;
		break;
	}
}

void	_largeSysExDemo::complete(void)
{
	busy = false;
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 'l', 'x' ),
	.name = "Large SysEx Demo",
	.description = "Demo of API for sending large SysEx",
	.numSpecifications = 0,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.parameterChanged = parameterChanged,
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
