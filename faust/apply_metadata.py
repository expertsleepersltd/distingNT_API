import os, sys, re

def process( text ):
	# make a metadata dictionary
	d = {}
	for t in text:
		s = re.split( r'.*\"(\w+)\".+\"(.+)\"', t )
		if len(s) == 4:
			d[ s[1] ] = s[2]

	# parse the memoryInfo() function
	state = 0
	which = 0
	mem = [ 0, 0, 0 ]
	for t in text:
		if state == 0:
			if 'void memoryInfo()' in t:
				state = 1
		elif state == 1:
			if '}' in t:
				break;
			# before the mydsp allocation is class static data
			if t.strip().endswith( '// mydsp' ):
				which = 1
			s = re.split( r'\s*fManager->info\((\d+),\s(\d+),\s(\d+)\)', t )
			if len(s) == 5:
				mem[ which ] += int( s[1] )
				# after we have the mydsp alloction, the rest is per-instance data
				if which == 1:
					which = 2
			else:
				# new format for Faust 2.83.1
				s = re.split( r'\s*fManager->info\(\".+\",\sdsp_memory_manager::.+,\s\d+,\s(\d+),\s(\d+),\s(\d+)\)', t )
				if len(s) == 5:
					mem[ which ] += int( s[1] )
					# after we have the mydsp alloction, the rest is per-instance data
					if which == 1:
						which = 2

	# stich the lines back into a single block of text
	text = "".join( text )
	
	# replace metadata text with text from dictionary
	if 'name' in d:
		text = text.replace( '.name = "faust",', '.name = "%s",' % d[ 'name' ] )
	if 'description' in d:
		text = text.replace( '.description = "faust",', '.description = "%s",' % d[ 'description' ] )
	if 'guid' in d:
		guid = d[ 'guid' ]
		if len(guid) == 4:
			text = text.replace( ".guid = NT_MULTICHAR( 'F', 'a', 'u', 's' ),", ".guid = NT_MULTICHAR( '%s', '%s', '%s', '%s' )," % ( guid[0], guid[1], guid[2], guid[3] ) )

	# replace memoryInfo() calls with data found above
	text = text.replace( 'mydsp::memoryInfo(); // class', 'mem.total = %d;' % mem[0] )
	text = text.replace( 'mydsp::memoryInfo(); // instance', 'mem.total = %d;' % mem[2] )

	return text

def main():
	if len(sys.argv) != 2:
		print("Usage: python apply_metadata.py <.cpp file>")
		sys.exit(1)
	
	filename = sys.argv[1]
	if not os.path.exists(filename):
		print( f"Error: File {filename} not found" )
		sys.exit(1)
	
	with open( filename, 'r', encoding='utf-8' ) as f:
		text = f.readlines()
	
	text = process( text )
	
	with open( filename, 'w', encoding='utf-8' ) as f:
		f.write( text )

if __name__ == "__main__":
	main()
