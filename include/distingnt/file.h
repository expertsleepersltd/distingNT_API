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

enum _NT_fileOpenMode
{
	kNT_modeRead			= 0x01,
	kNT_modeWrite			= 0x02,
	kNT_modeOpenExisting	= 0x00,
	kNT_modeCreateNew		= 0x04,
	kNT_modeCreateAlways	= 0x08,
	kNT_modeOpenAlways		= 0x10,
	kNT_modeOpenAppend		= 0x30,
};

enum _NT_findFlags
{
	kNT_findFiles			= 0x01,
	kNT_findFolders			= 0x02,
	kNT_findAll				= 0x03,
};

struct _NT_fileInfo
{
	uint64_t	size;
	uint16_t	date;
	uint16_t	time;
	uint8_t		attrib;
	char		name[256];
};

enum { kNT_numAvailableFileHandles = 8 };

typedef uint32_t	_NT_fileHandle;

bool			NT_callFileSystemCode( void (*callback)( void* ), void* callbackData );

_NT_fileResult	NT_fileOpen( _NT_fileHandle F, const char* filename, int mode );
_NT_fileResult	NT_fileClose( _NT_fileHandle F );
_NT_fileResult	NT_fileRead( _NT_fileHandle F, void* dst, uint32_t bytes, uint32_t& bytesRead );
_NT_fileResult	NT_fileWrite( _NT_fileHandle F, const void* src, uint32_t bytes, uint32_t& bytesWritten );
_NT_fileResult	NT_fileSeek( _NT_fileHandle F, uint32_t pos );
uint32_t		NT_ftell( _NT_fileHandle F );

_NT_fileResult	NT_fileInfo( const char* path, _NT_fileInfo& info );
_NT_fileResult	NT_fileDelete( const char* path );
_NT_fileResult	NT_chdir( const char* path );
_NT_fileResult	NT_mkdir( const char* path );

uint32_t		NT_findFiles( const char* pattern, _NT_findFlags findFlags );
void 			NT_getFoundFile( uint32_t index, char* buffer, size_t size );

void*			NT_getFileScratchBuffer( size_t& size );

};

#endif // _DISTINGNT_FILE_H
