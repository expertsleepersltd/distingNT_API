
#include <algorithm>
#include <distingnt/api.h>
#include <distingnt/file.h>

enum
{
	kParamDoIt,
	
	kNumParams,
};

static const _NT_parameter	parameters[] = {
	{ .name = "Do it", .min = 0, .max = 1, .def = 0, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL },
};

static const uint8_t page1[] = { kParamDoIt };

static const _NT_parameterPage pages[] = {
	{ .name = "Demo", .numParams = ARRAY_SIZE(page1), .params = page1 },
};

static const _NT_parameterPages parameterPages = {
	.numPages = ARRAY_SIZE(pages),
	.pages = pages,
};

struct _fileApiDemo : public _NT_algorithm
{
	_fileApiDemo() {}
	~_fileApiDemo() {}

	_NT_parameter	params[ kNumParams ];

	enum
	{
		kIdle,
		kRunning,
		kComplete,
		kFailed,
	};
	
	int		status;
	
	static void		callback( void* callbackData );
	bool			fileOperation(void);
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_fileApiDemo);
	req.dram = 0;
	req.dtc = 0;
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	static_assert( kNumParams == ARRAY_SIZE(parameters) );

	_fileApiDemo* alg = new (ptrs.sram) _fileApiDemo();
	alg->parameters = parameters;
	alg->parameterPages = &parameterPages;
	alg->status = _fileApiDemo::kIdle;

	return alg;
}

void	parameterChanged( _NT_algorithm* self, int p )
{
	_fileApiDemo* pThis = (_fileApiDemo*)self;

	switch ( p )
	{
	case kParamDoIt:
		// if the parameter changed from 0 to 1,
		// and the operation is idle,
		// perform the operation
		if ( pThis->v[kParamDoIt] && ( pThis->status == _fileApiDemo::kIdle ) )
		{
			if ( NT_callFileSystemCode( _fileApiDemo::callback, pThis ) )
				pThis->status = _fileApiDemo::kRunning;
		}
		break;
	}
}

void		_fileApiDemo::callback( void* callbackData )
{
	_fileApiDemo* pThis = (_fileApiDemo*)callbackData;
	bool success = pThis->fileOperation();
	pThis->status = success ? kComplete : kFailed;
}

bool		_fileApiDemo::fileOperation(void)
{
	// all filesystem API calls must be within this function,
	// invoked via NT_callFileSystemCode()
	
	// just by way of a demo, this performs the fairly pointless task
	// of concatenating some Scala files into a single file
	
	// change directory
	if ( kNT_fileOK != NT_chdir( "/scl" ) )
		return false;
	
	// declare file handles
	_NT_fileHandle Fout = 0;
	_NT_fileHandle Fin = 1;
	
	// open a file to write to
	if ( kNT_fileOK != NT_fileOpen( Fout, "fileApiDemo.txt", kNT_modeWrite | kNT_modeCreateAlways ) )
		return false;
	
	// find files
	uint32_t count = NT_findFiles( "*.scl", kNT_findFiles );

	// if we fail from here on, we must still close the output file
	bool success = true;
	
	// get the scratch buffer
	size_t scratchSize;
	void* scratch =	NT_getFileScratchBuffer( scratchSize );

	// loop over first few files and concatenate them
	count = std::min( count, 3UL );
	for ( uint32_t i=0; i<count; ++i )
	{
		// open a file to read from
		char buffer[256];
		NT_getFoundFile( i, buffer, sizeof buffer );
		if ( kNT_fileOK != NT_fileOpen( Fin, buffer, kNT_modeRead ) )
		{
			success = false;
			break;
		}
		
		// read from the input file and write the contents to the output
		for (;;)
		{
			uint32_t bytesRead;
			if ( kNT_fileOK != NT_fileRead( Fin, scratch, scratchSize, bytesRead ) )
			{
				success = false;
				break;
			}
			if ( bytesRead == 0 )
				break;
			uint32_t bytesWritten;
			if ( kNT_fileOK != NT_fileWrite( Fout, scratch, bytesRead, bytesWritten ) )
			{
				success = false;
				break;
			}
		}
		
		// note that there's no need to close a file that is only open for read
	}

	// close the output file, to flush the writes and update the directory info	
	if ( kNT_fileOK != NT_fileClose( Fout ) )
		success = false;
	
	return success;
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
}

bool	draw( _NT_algorithm* self )
{
	_fileApiDemo* pThis = (_fileApiDemo*)self;
	
	static char const * const status[] = {
		"Idle",
		"Running",
		"Complete",
		"Failed",
	};
	NT_drawText( 10, 40, status[ pThis->status ] );
		
	return false;
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 'f', 'a' ),
	.name = "File API demo",
	.description = "Demo of filesystem API",
	.numSpecifications = 0,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.parameterChanged = parameterChanged,
	.step = step,
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
