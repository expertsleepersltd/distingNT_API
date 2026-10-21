/*
MIT License

Copyright (c) 2026 Expert Sleepers Ltd

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
*/

#ifndef _DISTINGNT_FILE_H
#define _DISTINGNT_FILE_H

#include <stdint.h>
#include <distingnt/api.h>

extern "C" {

/*
 * Constants for filesystem call results.
 */
enum _NT_fileResult
{
	kNT_fileOK = 0,                 	/* Succeeded */
    kNT_diskErr = 1,                    /* A hard error occurred in the low level disk I/O layer */
    kNT_intErr = 2,                     /* Assertion failed */
    kNT_notReady = 3,                   /* The physical drive cannot work */
    kNT_noFile = 4,                     /* Could not find the file */
    kNT_noPath = 5,                     /* Could not find the path */
    kNT_invalidName = 6,                /* The path name format is invalid */
    kNT_denied = 7,                     /* Access denied due to prohibited access or directory full */
    kNT_exist = 8,                      /* Access denied due to prohibited access */
    kNT_invalidObject = 9,              /* The file/directory object is invalid */
    kNT_writeProtected = 10,            /* The physical drive is write protected */
    kNT_invalidDrive = 11,              /* The logical drive number is invalid */
    kNT_notEnabled = 12,                /* The volume has no work area */
    kNT_noFilesystem = 13,              /* There is no valid FAT volume */
    kNT_timeout = 14,                   /* Could not get a grant to access the volume within defined period */
    kNT_locked = 15,                    /* The operation is rejected according to the file sharing policy */
    kNT_notEnoughCore = 16,             /* LFN working buffer could not be allocated */
    kNT_invalidParameter = 19,          /* Given parameter is invalid */
	kNT_invalidFileHandle = 100,		/* Invalid value for _NT_fileHandle */
};

/*
 * Constants for the mode argument of NT_fileOpen().
 * Can be combined e.g. kNT_modeWrite | kNT_modeCreateAlways
 *  to open a file for write, and to always create a new file,
 *  even if it exists already.
 */
enum _NT_fileOpenMode
{
	kNT_modeRead			= 0x01,		// open for read
	kNT_modeWrite			= 0x02,		// open for write
	kNT_modeOpenExisting	= 0x00,		// only open existing files - don't create a new one
	kNT_modeCreateNew		= 0x04,		// create a new file if one doesn't already exist
	kNT_modeCreateAlways	= 0x08,		// always create a new file
	kNT_modeOpenAlways		= 0x10,		// always open a file, even if that means creating one
	kNT_modeOpenAppend		= 0x30,		// open for append (seek to end of file on open)
};

/*
 * Constants for use with NT_findFiles().
 */
enum _NT_findFlags
{
	kNT_findFiles			= 0x01,		// only find files, not folders
	kNT_findFolders			= 0x02,		// only find folders, not files
	kNT_findAll				= 0x03,		// find both files and folders
};

/*
 * Bitmask values for attrib field of _NT_fileInfo.
 */
enum _NT_fileAttrib
{
	kNT_readOnly	= 0x01,
	kNT_hidden	 	= 0x02,
	kNT_system	 	= 0x04,
	kNT_directory	= 0x10,
	kNT_archive	 	= 0x20,
};

/*
 * Structure to return data from NT_fileInfo().
 */
struct _NT_fileInfo
{
	uint64_t	size;			// file size
	uint16_t	date;			// file modified date
	uint16_t	time;			// file modified time
	uint8_t		attrib;			// file attributes
	char		name[256];		// file name (null terminated)
};

/*
 * The number of available file handles.
 * Values from 0 and up can be used for _NT_fileHandle.
 */
enum { kNT_numAvailableFileHandles = 8 };

/*
 * A locally unique value used to keep track of an open file.
 */
typedef uint32_t	_NT_fileHandle;

/*
 * All access to the filesystem API must be via this call.
 * Put your filesystem code in a callback function, and then invoke it with NT_callFileSystemCode().
 * The value you provide for callbackData is passed to your callback function.
 *
 * No filesystem data can be assumed to persist outside of your callback.
 * E.g. if you need to keep working on a file, store its path, and open
 * the file every time you make a callback.
 *
 * Returns true if the callback was successfully registered.
 * The callback will not be called if the function returns false.
 */
bool			NT_callFileSystemCode( void (*callback)( void* ), void* callbackData );

/*
 * Open a file.
 */
_NT_fileResult	NT_fileOpen( _NT_fileHandle F, const char* filename, int mode );

/*
 * Close an open file.
 * Note that you don't need to close a file open only for read - you can
 *  simply reuse the file handle if required.
 * You must close a file open for write, to ensure all data is actually written
 *  back to the card, and that the directory information is updated.
 */
_NT_fileResult	NT_fileClose( _NT_fileHandle F );

/*
 * Read from an open file.
 */
_NT_fileResult	NT_fileRead( _NT_fileHandle F, void* dst, uint32_t bytes, uint32_t& bytesRead );

/*
 * Write to an open file.
 */
_NT_fileResult	NT_fileWrite( _NT_fileHandle F, const void* src, uint32_t bytes, uint32_t& bytesWritten );

/*
 * Move the read/write location in an open file.
 */
_NT_fileResult	NT_fileSeek( _NT_fileHandle F, uint32_t pos );

/*
 * Query the read/write location in an open file.
 */
uint32_t		NT_ftell( _NT_fileHandle F );

/*
 * For all functions that take a path as a parameter, the path can be
 * relative to the current directory, or it can be absolute (i.e. beginning
 * with '/').
 */

/*
 * Query information about a file or folder.
 */
_NT_fileResult	NT_fileInfo( const char* path, _NT_fileInfo& info );

/*
 * Delete a file or folder.
 */
_NT_fileResult	NT_fileDelete( const char* path );

/*
 * Change the current directory.
 */
_NT_fileResult	NT_chdir( const char* path );

/*
 * Create a new directory.
 */
_NT_fileResult	NT_mkdir( const char* path );

/*
 * Find files or folders, within the current directory.
 * The pattern is a standard wildcard syntax e.g. "*" or "*.wav".
 *
 * Returns the number of files/folders found.
 */
uint32_t		NT_findFiles( const char* pattern, _NT_findFlags findFlags );

/*
 * Get the name of the file found with NT_findFiles().
 * 'size' is the size of the buffer pointed to by 'buffer'.
 */
void 			NT_getFoundFile( uint32_t index, char* buffer, size_t size );

/*
 * Get a pointer to a scratch buffer which can be used by fileystem operations
 * (or by any code within a NT_callFileSystemCode() callback).
 */
void*			NT_getFileScratchBuffer( size_t& size );

};

#endif // _DISTINGNT_FILE_H
