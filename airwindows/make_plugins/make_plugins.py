'''
MIT License

Copyright (c) 2025 Expert Sleepers Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
'''

import os
import re

path = 'reports'
files = os.listdir( path )
files = [ f for f in files if os.path.isfile( os.path.join( path, f ) ) and f.endswith( '.txt' ) ]
files.sort()

descriptions = {}
tag_effects = []
tag_reverb = []
tag_filter = []
with open ( '../airwindows/Airwindopedia.txt', 'r' ) as F:
	lines = F.readlines()
	for line in lines:
		if line.startswith( '############ ' ):
			bits = line.split( ' ', maxsplit=2 )
			if len(bits) == 3:
				d = bits[2].strip()
				if d.startswith( 'is ' ):
					d = d[3:]
				d = d[:1].upper() + d[1:]
				descriptions[ bits[1] ] = d
		elif line.startswith( 'Effects:' ):
			bits = line[8:].split( ',' )
			for b in bits:
				tag_effects.append( b.strip() )
		elif line.startswith( 'Filter:' ):
			bits = line[7:].split( ',' )
			for b in bits:
				tag_filter.append( b.strip() )
		elif line.startswith( 'Reverb:' ):
			bits = line[7:].split( ',' )
			for b in bits:
				tag_reverb.append( b.strip() )

srcPath = '../airwindows/plugins/MacAU/'

skipList = [ 'VerbSixes' ]

def matchCurly( t ):
	s = ''
	b = 0
	for c in t:
		s += c
		if c == '{':
			b += 1
		elif c == '}':
			b -= 1
			if b == 0:
				return s
	return ''

def repl_fconst( m ):
	return m.group(1) + 'f' + m.group(2)

def adjustFloatTypes( text ):
	text = text.replace( 'long double ', 'double ' )
	if True:
		text = text.replace( 'double', 'float' )
		text = text.replace( 'Float64', 'Float32' )
		text = re.sub( r'(\d+\.\d+)(\W)', repl_fconst, text )
		text = re.sub( r'(\d+\.\d+e-?\d+)(\W)', repl_fconst, text )
		text = text.replace( 'pow(', 'powf(' )
		# remove dither
		while True:
			bits = text.split( '//begin 32 bit floating point dither', maxsplit=1 )
			if len(bits) == 2:
				text = bits[0] + bits[1].split( '//end 32 bit floating point dither', maxsplit=1 )[1]
			else:
				break
		while True:
			bits = text.split( '//begin 32 bit stereo floating point dither', maxsplit=1 )
			if len(bits) == 2:
				text = bits[0] + bits[1].split( '//end 32 bit stereo floating point dither', maxsplit=1 )[1]
			else:
				break
	return text

def repl( m ):
	return m.group(1) + 'dram->' + m.group(2) + '['
	
def processDramSymbols( text, dramSymbols ):
	for s in dramSymbols:
		pat = r'([^\w\d])(' + s + r')\['
		text = re.sub( pat, repl, text )
	return text

guids = []

for f in files:
	name = f.replace( '.txt', '' )
	if name in skipList:
		continue
	with open( os.path.join( path, f ), 'r' ) as F:
		lines = F.readlines()
		numEnums = 0
		firstParamLine = 2
		if ( lines[1] == "ENUMS\n" ):
			numEnums = int( lines[2].strip() )
			firstParamLine = 4 + numEnums
		numParameters = int( lines[firstParamLine-1].strip() )

		fname = name
		if fname == 'Point':
			fname = 'Poynt'
		src = srcPath + name + '/' + fname + '.cpp'

		enums = ''
		with open( src.replace( '.cpp', '.h' ), 'r', encoding='mac_roman' ) as G:
			content = G.read()
			while True:
				bits = content.split( 'enum {' )
				if len(bits) < 2:
					break;
				bits = bits[1].split( '}' )
				enums += 'enum {\n' + bits[0] + '};\n'
				content = bits[1]

		privates = ''
		static_data = ''
		with open( src.replace( '.cpp', '.h' ), 'r', encoding='mac_roman' ) as G:
			content = G.read()
			bits = content.split( 'private:' )
			if len(bits) != 2:
				bits = content.split( 'Reset();' )
			if len(bits) == 2:
				bits = '{' + bits[1]
				privates = matchCurly( bits )[1:-1]
			m = re.findall( r'static int (\w+)\[\] = ({[\d,\s]+});', content )
			for n in m:
				static_data += 'static const int ' + n[0] + '[] = ' + n[1] + ';\n'
		privates = adjustFloatTypes( privates )
		
		# look for large arrays and move them to dram
		dram = ''
		dramSymbols = []
		for p in privates.split( '\n' ):
			for t in [ 'double', 'float', 'Float64', 'Float32' ]:
				pat = r'\s*' + t + r'\s+([\d\w]+)\[(\d+)\];'
				m = re.match( pat, p )
				if m:
					size = int( m.group(2) )
					# arbitrary size limit
					if size > 16:
						dram += p + '\n'
						dramSymbols.append( m.group(1) )
						privates = privates.replace( p + '\n', '' )
				else:
					# look for arrays with symbols as sizes
					# assume these are big
					# though this is defintely not the case for all examples
					pat = r'\s*' + t + r'\s+([\d\w]+)\[([\w\d\+]+)\];'
					m = re.match( pat, p )
					if m:
						dram += p + '\n'
						dramSymbols.append( m.group(1) )
						privates = privates.replace( p + '\n', '' )

		consts = ''
		with open( src.replace( '.cpp', '.h' ), 'r', encoding='mac_roman' ) as G:
			content = G.readlines()
			content = [ c for c in content if c.startswith( 'const int' ) or c.startswith( 'static const int' ) ]
			consts = ''.join( content )
			
		description = name
		if name in descriptions:
			description = descriptions[ name ]

		while len( name ) < 3:
			name += ' '
		guid = [ 'A', name[0], name[1], name[-1] ]
		while guid in guids:
			i = ord( guid[3] ) + 1
			while i in [ ord("'"), ord('"'), ord('\\') ]:
				i += 1
			i = ord(' ') + ( ( i - ord(' ') ) % ( ord('z') - ord(' ') ) )
			guid[3] = chr( i )
		guids.append( guid )
		
		tags = ''
		if name in tag_reverb:
			tags = 'kNT_tagEffect | kNT_tagReverb'
		elif name in tag_filter:
			tags = 'kNT_tagFilterEQ'
		elif name in tag_effects:
			tags = 'kNT_tagEffect'

		if ( lines[0] == 'STEREO\n' ):
			with open( src, 'r', encoding='mac_roman' ) as G:
				content = G.read()
				bits = content.split( '(Float32*)(outBuffer.mBuffers[1].mData);' )
				bits = bits[1].split( 'return noErr;' )
				renderCall = bits[0]
				renderCall = adjustFloatTypes( renderCall )
				renderCall = processDramSymbols( renderCall, dramSymbols )
				bits = content.split( '::Reset(AudioUnitScope inScope, AudioUnitElement inElement)' )
				resetCall = bits[1].split( '//~~~~~~~~~~~' )[0]
				resetCall = processDramSymbols( resetCall, dramSymbols )

			with open( os.path.join( '../src', f.replace( '.txt', '.cpp' ) ), 'w' ) as G:
				G.write( '#include <math.h>\n#include <new>\n#include <distingnt/api.h>\n' )
				G.write( '#define AIRWINDOWS_NAME "' + name + '"\n' )
				G.write( '#define AIRWINDOWS_DESCRIPTION "' + description + '"\n' )
				G.write( "#define AIRWINDOWS_GUID NT_MULTICHAR( '" + guid[0] + "','" + guid[1] + "','" + guid[2] + "','" + guid[3] + "' )\n" )
				if len(tags) > 0:
					G.write( f'#define AIRWINDOWS_TAGS {tags}\n' )
				G.write( enums )
				G.write( consts )
				G.write( static_data )
				G.write( 'enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,\n' )
				G.write( 'kParamPrePostGain,\n' )
				for i in range( numParameters ):
					G.write( f'kParam{i}, ' )
				G.write( '};\n' )
				if numEnums > 0:
					G.write( ''.join( lines[3:3+numEnums] ) )
				G.write( 'static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };\n' )
				G.write( 'static const uint8_t page3[] = { kParamPrePostGain };\n' )
				G.write( 'static const _NT_parameter	parameters[] = {\n' )
				G.write( 'NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )\n' )
				G.write( 'NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )\n' )
				G.write( 'NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )\n' )
				G.write( 'NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )\n' )
				G.write( '{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },\n' )
				G.write( ''.join( lines[firstParamLine:] ) )
				G.write( '};\n' )
				G.write( 'static const uint8_t page1[] = {\n' )
				for i in range( numParameters ):
					G.write( f'kParam{i}, ' )
				G.write( '};\n' )
				G.write( 'enum { kNumTemplateParameters = 7 };\n' )
				G.write( '#include "../include/template1.h"\n' )
				G.write( privates )
				G.write( '\n\tstruct _dram {\n\t' )
				G.write( dram )
				G.write( '\t};\n' )
				G.write( '\t_dram* dram;\n' )
				G.write( '#include "../include/template2.h"\n' )
				G.write( '#include "../include/templateStereo.h"\n' )
				G.write( 'void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {\n')
				G.write( renderCall )
				G.write( '};\n' )
				G.write( 'int _airwindowsAlgorithm::reset(void) {\n')
				G.write( resetCall )
				G.write( '};\n' )
		
		elif ( lines[0] == 'KERNELS\n' ):
			with open( src, 'r', encoding='mac_roman' ) as G:
				content = G.read()
				bits = content.split( 'Kernel::Process(' )
				bits = '{' + bits[1].split( '{', maxsplit=1 )[1]
				renderCall = matchCurly( bits )
				renderCall = adjustFloatTypes( renderCall )
				renderCall = processDramSymbols( renderCall, dramSymbols )
				bits = content.split( 'Kernel::Reset()', maxsplit=1 )
				bits = '{' + bits[1].split( '{', maxsplit=1 )[1]
				resetCall = matchCurly( bits )
				resetCall = processDramSymbols( resetCall, dramSymbols )

			with open( os.path.join( '../src', f.replace( '.txt', '.cpp' ) ), 'w' ) as G:
				G.write( '#include <math.h>\n#include <new>\n#include <distingnt/api.h>\n' )
				G.write( '#define AIRWINDOWS_NAME "' + name + '"\n' )
				G.write( '#define AIRWINDOWS_DESCRIPTION "' + description + '"\n' )
				G.write( "#define AIRWINDOWS_GUID NT_MULTICHAR( '" + guid[0] + "','" + guid[1] + "','" + guid[2] + "','" + guid[3] + "' )\n" )
				if len(tags) > 0:
					G.write( f'#define AIRWINDOWS_TAGS {tags}\n' )
				G.write( '#define AIRWINDOWS_KERNELS\n' )
				G.write( enums )
				G.write( consts )
				G.write( static_data )
				G.write( 'enum { kParamInput1, kParamOutput1, kParamOutput1mode,\n' )
				G.write( 'kParamPrePostGain,\n' )
				for i in range( numParameters ):
					G.write( f'kParam{i}, ' )
				G.write( '};\n' )
				if numEnums > 0:
					G.write( ''.join( lines[3:3+numEnums] ) )
				G.write( 'static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };\n' )
				G.write( 'static const uint8_t page3[] = { kParamPrePostGain };\n' )
				G.write( 'static const _NT_parameter	parameters[] = {\n' )
				G.write( 'NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )\n' )
				G.write( 'NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )\n' )
				G.write( '{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },\n' )
				G.write( ''.join( lines[firstParamLine:] ) )
				G.write( '};\n' )
				G.write( 'static const uint8_t page1[] = {\n' )
				for i in range( numParameters ):
					G.write( f'kParam{i}, ' )
				G.write( '};\n' )
				G.write( 'enum { kNumTemplateParameters = 4 };\n' )
				G.write( '#include "../include/template1.h"\n' )
				G.write( 'struct _kernel {\n' )
				G.write( '\tvoid render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );\n' )
				G.write( '\tvoid reset(void);\n' )
				G.write( '\tfloat GetParameter( int index ) { return owner->GetParameter( index ); }\n' )
				G.write( '\t_airwindowsAlgorithm* owner;\n' )
				G.write( privates )
				G.write( '\n\tstruct _dram {\n\t' )
				G.write( dram )
				G.write( '\t};\n' )
				G.write( '\t_dram* dram;\n' )
				G.write( '};\n' )
				G.write( '_kernel kernels[1];\n' )
				G.write( '\n#include "../include/template2.h"\n' )
				G.write( '#include "../include/templateKernels.h"\n' )
				G.write( 'void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {\n')
				G.write( '#define inNumChannels (1)\n')
				G.write( renderCall )
				G.write( '\n}\n' )
				G.write( 'void _airwindowsAlgorithm::_kernel::reset(void) {\n')
				G.write( resetCall )
				G.write( '\n};\n' )

		else:
			pass
