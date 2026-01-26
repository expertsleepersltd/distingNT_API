import os, sys, re

def process( text ):
	# find the small arrays
	ss = re.findall( r'//\s+(f[VR]ec\d+)\n\s+fManager->info\(8,', text )

	# replace the declarations
	for s in ss:
		text = text.replace( f'\tfloat* {s};', f'\tfloat {s}[2];' )
		
	# remove the memoryInfo() calls
	for s in ss:
		bits = text.split( f'// {s}\n' )
		text = bits[0] + bits[1].split( ';', maxsplit=1 )[1]
		
	# remove the memoryCreate() calls
	for s in ss:
		text = text.replace( f'{s} = static_cast<float*>(fManager->allocate(8));', '' )
		
	# remove the memoryDestroy() calls
	for s in ss:
		text = text.replace( f'fManager->destroy({s});', '' )

	# delete lines containing C++ destructors
	lines = text.splitlines()
	filtered_lines = [line for line in lines if not re.search(r'~\w+\s*\(', line)]
	text = '\n'.join(filtered_lines)

	# delete lines containing deletemydspSIG calls
	lines = text.splitlines()
	filtered_lines = [line for line in lines if not re.search(r'deletemydspSIG\d+\(sig\d+, fManager\);', line)]
	text = '\n'.join(filtered_lines)

	return text
	
def main():
	if len(sys.argv) != 2:
		print("Usage: python remove_small_arrays.py <.cpp file>")
		sys.exit(1)
	
	filename = sys.argv[1]
	if not os.path.exists(filename):
		print( f"Error: File {filename} not found" )
		sys.exit(1)
	
	with open( filename, 'r', encoding='utf-8' ) as f:
		text = f.read()
	
	text = process( text )
	
	with open( filename, 'w', encoding='utf-8' ) as f:
		f.write( text )

if __name__ == "__main__":
	main()
