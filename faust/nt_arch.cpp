
#include <algorithm>
#include <new>
#include <distingnt/api.h>

<<includeIntrinsic>>

#define FAUSTFLOAT float

class dsp {};

struct Meta {
    virtual void declare(const char* key, const char* value) {};
};

struct Soundfile {};

struct UI
{
	UI( _NT_parameter* defs, float** activePtr ) : ptr( defs ), actives( activePtr ) {}
	~UI() {}
	
	_NT_parameter* 	ptr;
	float**			actives;

    // -- widget's layouts
    void openTabBox(const char* label) {}
    void openHorizontalBox(const char* label) {}
    void openVerticalBox(const char* label) {}
    void closeBox() {}

    void add(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    {
    	NT_setParameterRange( ptr, init, min, max, step );
    	ptr->unit = 0;
    	ptr->name = label;
    	ptr->enumStrings = 0;
    	ptr++;
    	*actives = zone;
    	actives++;
    }

    // -- active widgets
    void addButton(const char* label, FAUSTFLOAT* zone)
    {
    	add( label, zone, 0, 0, 1, 1 );
    }
    void addCheckButton(const char* label, FAUSTFLOAT* zone)
    {
    	add( label, zone, 0, 0, 1, 1 );
    }
    void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    {
    	add( label, zone, init, min, max, step );
    }
    void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    {
    	add( label, zone, init, min, max, step );
    }
    void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    {
    	add( label, zone, init, min, max, step );
    }

    // -- passive widgets
    void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
    void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}

    // -- soundfiles
    void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}

    // -- metadata declarations
    void declare(FAUSTFLOAT* zone, const char* key, const char* val) {}
};

#define FAUST_UIMACROS
#define FAUST_ADDBUTTON( n, v )
#define FAUST_ADDCHECKBOX( n, v )
#define FAUST_ADDVERTICALSLIDER( n, v, def, min, max, step )
#define FAUST_ADDHORIZONTALSLIDER( n, v, def, min, max, step )
#define FAUST_ADDNUMENTRY( n, v, def, min, max, step )
#define FAUST_ADDHORIZONTALBARGRAPH( n, v, min, max )
#define FAUST_ADDVERTICALBARGRAPH( n, v, min, max )

struct dsp_memory_manager
{
	enum _mem { kMemClass, kMemInstance };
	
    enum MemType { kInt32, kInt32_ptr, kFloat, kFloat_ptr, kDouble, kDouble_ptr, kQuad, 
        kQuad_ptr, kFixedPoint, kFixedPoint_ptr, kObj, kObj_ptr, kSound, kSound_ptr };

	size_t	total;
	bool 	first;
	_mem	mode;

	dsp_memory_manager( _mem mode_ ) : mode( mode_ ) {}
	~dsp_memory_manager() {}
	void begin(size_t count)
	{
		total = 0;
		first = true;
	}
	void end() {}
	void info(const char* name, MemType type, size_t count, size_t size, size_t reads, size_t writes)
	{
		if ( mode == kMemClass )
		{
			if ( writes == 0 )
			{
				total += size;
			}
			return;
		}
		if ( writes == 0 )		// TODO check this logic with some more examples
			return;				// class static table
		if ( first )
		{
			first = false;
		}
		else
		{
			total += size;
		}
	}
	void* allocate(size_t size)
	{
		size_t m = total;
		total += size;
		return (void*)m; 
	}
	void destroy(void* ptr) {}
};

<<includeclass>>

enum
{
	kParamFirstInput = FAUST_ACTIVES,
	kParamFirstOutput = FAUST_ACTIVES + FAUST_INPUTS,
};

#define DECLARE_NAME( r, n )	#r " " #n

static const char* inputParamNames[FAUST_INPUTS] = {
#if FAUST_INPUTS >= 1
DECLARE_NAME( Input, 1 ),
#endif
#if FAUST_INPUTS >= 2
DECLARE_NAME( Input, 2 ),
#endif
};
static const char* outputParamNames[FAUST_OUTPUTS] = {
#if FAUST_OUTPUTS >= 1
DECLARE_NAME( Output, 1 ),
#endif
#if FAUST_OUTPUTS >= 2
DECLARE_NAME( Output, 2 ),
#endif
};
static const char* outputModeParamNames[FAUST_OUTPUTS] = {
#if FAUST_OUTPUTS >= 1
DECLARE_NAME( Output mode, 1 ),
#endif
#if FAUST_OUTPUTS >= 2
DECLARE_NAME( Output mode, 2 ),
#endif
};

struct _fauseAlgorithm : public _NT_algorithm
{
	_fauseAlgorithm() {}
	~_fauseAlgorithm() {}
	
	mydsp*	dsp;
	
	_NT_parameter	parameterDefs[ FAUST_ACTIVES + FAUST_INPUTS + 2 * FAUST_OUTPUTS ];
	float*			actives[FAUST_ACTIVES];
	
	uint8_t 			page1[ FAUST_ACTIVES ];
	uint8_t 			page2[ FAUST_INPUTS + 2 * FAUST_OUTPUTS ];
	_NT_parameterPage 	pages[ 2 ];
	_NT_parameterPages	ppages;
};

void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	dsp_memory_manager mem( dsp_memory_manager::kMemInstance );
	mydsp::fManager = &mem;
	mydsp::memoryInfo(); // instance
	mydsp::fManager = 0;

	req.numParameters = ARRAY_SIZE(_fauseAlgorithm::parameterDefs);
	req.sram = sizeof(_fauseAlgorithm);
	req.dram = mem.total;
	req.dtc = sizeof(mydsp);
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	if ( FAUST_OUTPUTS * NT_globals.maxFramesPerStep * sizeof(float) > NT_globals.workBufferSizeBytes )
		return NULL;

	_fauseAlgorithm* alg = new (ptrs.sram) _fauseAlgorithm();
	
	alg->dsp = new (ptrs.dtc) mydsp();

	dsp_memory_manager mem( dsp_memory_manager::kMemInstance );
	mem.total = (size_t)ptrs.dram;
	mydsp::fManager = &mem;
	alg->dsp->memoryCreate();
	mydsp::fManager = 0;

	UI ui( alg->parameterDefs, alg->actives );
	alg->dsp->buildUserInterface( &ui );
	
	for ( int i=0; i<FAUST_ACTIVES; ++i )
	{
		alg->page1[ i ] = i;
	}
	for ( int i=0; i<FAUST_INPUTS; ++i )
	{
		_NT_parameter& p = alg->parameterDefs[ kParamFirstInput + i ];
		p.name = inputParamNames[i];
		p.min = 1;
		p.max = 28;
		p.def = 1 + i;
		p.unit = kNT_unitAudioInput;
		p.scaling = 0;
		p.enumStrings = NULL;
		alg->page2[ i ] = FAUST_ACTIVES + i;
	}
	for ( int i=0; i<FAUST_OUTPUTS; ++i )
	{
		_NT_parameter& p = alg->parameterDefs[ kParamFirstOutput + 2*i ];
		p.name = outputParamNames[i];
		p.min = 1;
		p.max = 28;
		p.def = 13 + i;
		p.unit = kNT_unitAudioOutput;
		p.scaling = 0;
		p.enumStrings = NULL;
		_NT_parameter& pm = alg->parameterDefs[ kParamFirstOutput + 2*i + 1 ];
		pm.name = outputModeParamNames[i];;
		pm.min = 0;
		pm.max = 1;
		pm.def = 0;
		pm.unit = kNT_unitOutputMode;
		pm.scaling = 0;
		pm.enumStrings = NULL;
		alg->page2[ FAUST_INPUTS + 2*i + 0 ] = FAUST_ACTIVES + FAUST_INPUTS + 2*i + 0;
		alg->page2[ FAUST_INPUTS + 2*i + 1 ] = FAUST_ACTIVES + FAUST_INPUTS + 2*i + 1;
	}
	
	alg->parameters = alg->parameterDefs;

	alg->pages[0].name = "Program";
	alg->pages[0].numParams = ARRAY_SIZE(alg->page1);
	alg->pages[0].params = alg->page1;
	alg->pages[1].name = "Routing";
	alg->pages[1].numParams = ARRAY_SIZE(alg->page2);
	alg->pages[1].params = alg->page2;
	alg->ppages.numPages = ARRAY_SIZE(alg->pages);
	alg->ppages.pages = alg->pages;
	alg->parameterPages = &alg->ppages;
	
	alg->dsp->instanceInit( NT_globals.sampleRate );
	
	return alg;
}

void	parameterChanged( _NT_algorithm* self, int p )
{
	_fauseAlgorithm* pThis = (_fauseAlgorithm*)self;
	
	if ( (uint32_t)p >= kParamFirstInput )
		return;
	
	float v = pThis->v[ p ];
	switch ( pThis->parameterDefs[ p ].scaling )
	{
	case kNT_scaling10:
		v *= 0.1f;
		break;
	case kNT_scaling100:
		v *= 0.01f;
		break;
	case kNT_scaling1000:
		v *= 0.001f;
		break;
	}
	*( pThis->actives[ p ] ) = v;
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_fauseAlgorithm* pThis = (_fauseAlgorithm*)self;

	int numFrames = numFramesBy4 * 4;
	
	int numInputs = mydsp::getStaticNumInputs();
	int numOutputs = mydsp::getStaticNumOutputs();
	
	float* work = NT_globals.workBuffer;
	
	FAUSTFLOAT* inputs[numInputs];
	for ( int i=0; i<numInputs; ++i )
		inputs[i] = busFrames + ( self->v[ kParamFirstInput + i ] - 1 ) * numFrames;
	FAUSTFLOAT* outputs[numOutputs];
	for ( int i=0; i<numOutputs; ++i )
	{
		if ( self->v[ kParamFirstOutput + 2*i + 1 ] )
			outputs[i] = busFrames + ( self->v[ kParamFirstOutput + 2*i ] - 1 ) * numFrames;
		else
			outputs[i] = work + i * numFrames;
	}
	pThis->dsp->compute( numFrames, inputs, outputs );
	for ( int i=0; i<numOutputs; ++i )
	{
		if ( !self->v[ kParamFirstOutput + 2*i + 1 ] )
		{
			const float* src = work + i * numFrames;
			float* dst = busFrames + ( self->v[ kParamFirstOutput + 2*i ] - 1 ) * numFrames;
			for ( int j=0; j<numFrames; ++j )
				dst[j] += src[j];
		}
	}
}

void	calculateStaticRequirements( _NT_staticRequirements& req )
{
	dsp_memory_manager mem( dsp_memory_manager::kMemClass );
	mydsp::fManager = &mem;
	mydsp::memoryInfo(); // class
	mydsp::fManager = 0;
	
	req.dram = mem.total;
}

void	initialise( _NT_staticMemoryPtrs& ptrs, const _NT_staticRequirements& req )
{
	dsp_memory_manager mem( dsp_memory_manager::kMemClass );
	mem.total = (size_t)ptrs.dram;
	mydsp::fManager = &mem;
	mydsp::classInit( NT_globals.sampleRate );
	mydsp::fManager = 0;
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'F', 'a', 'u', 's' ),
	.name = "faust",
	.description = "faust",
	.numSpecifications = 0,
	.calculateStaticRequirements = calculateStaticRequirements,
	.initialise = initialise,
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.parameterChanged = parameterChanged,
	.step = step,
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
