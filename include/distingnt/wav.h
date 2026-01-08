/*
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
*/

#ifndef _DISTINGNT_WAV_H
#define _DISTINGNT_WAV_H

#include <stdint.h>
#include <distingnt/api.h>

enum _NT_wavProgress
{
	kNT_WavNoProgress,
	kNT_WavProgress,
};

// The number of channels in a WAV file.
enum _NT_wavChannels
{
	kNT_WavMono,
	kNT_WavStereo,
};

// The number of bits per sample in a WAV file.
enum _NT_wavBits
{
	kNT_WavBits8,
	kNT_WavBits16,
	kNT_WavBits24,
	kNT_WavBits32,		// float
};

// Round robin mode for samples, used in _NT_streamOpenData.
enum _NT_roundRobinMode
{
    kNT_RRModeSequential,
    kNT_RRModeRandom,
    kNT_RRModeRandom2,
    kNT_RRModeRandom3,

    kNT_numRRModes,
};

/*
 * Used to return info from NT_getSampleFolderInfo().
 */
struct _NT_wavFolderInfo
{
	const char* 	name;
	uint32_t		numSampleFiles;
};

/*
 * Used to return info from NT_getSampleFileInfo().
 */
struct _NT_wavInfo
{
	const char* 	name;
	uint32_t    	numFrames;
	uint32_t		sampleRate;
	_NT_wavChannels	channels;
	_NT_wavBits		bits;
};

/*
 * Used with NT_readSampleFrames().
 */
struct _NT_wavRequest
{
	uint32_t 		folder;
	uint32_t 		sample;
	void* 			dst;
	uint32_t 		numFrames;
	uint32_t 		startOffset;
	_NT_wavChannels	channels;
	_NT_wavBits 	bits;
	_NT_wavProgress	progress;
	void 			(*callback)( void*, bool );
	void* 			callbackData;
};

/*
 * Used to return info from NT_getWavetableInfo().
 */
struct _NT_wavetableInfo
{
	const char* 	name;
};

/*
 * Used with NT_readWavetable().
 */
struct _NT_wavetableRequest
{
	// Request inputs

	uint32_t 		index;			// Index of the wavetable to load.

    int16_t*		table;			// Pointer to memory to hold the wavetable.
    uint32_t		tableSize;      // Maximum number of frames that table can hold.

    void 			(*callback)( void* );	// The function to call when the read is complete.
	void* 			callbackData;			// Data to be passed to the callback function.

	// Request outputs

    bool    		error;			// An error occurred during load.
    bool    		usingMipMaps;	// If true, the table uses mipmaps. See below.

    uint32_t		numWaves;		// The number of waves in the table.
    uint32_t		waveLength;		// The number of frames per wave in the table.
};

/*
 * Used with NT_evaluateWavetable().
 */
struct _NT_wavetableEvaluation
{
	float		phase;			// The oscillator phase, 0.0 <= phase < 1.0
	float		offset;			// The position in the wavetable, 0 <= offset < (request.numWaves-1)
	float		frequency;		// The frequency of the oscillator using the wavetable in Hz
	uint32_t	mipmapSize;		// (Output) The chosen mipmap size
};

/*
 * Wavetable organisation
 * ======================
 *
 * If not using mipmaps:
 * the waves are simply contiguous in memory.
 * waveLength frames of wave 0; waveLength frames of wave 1; etc.
 *
 * If using mipmaps:
 * power-of-two downsampled versions of the table are available, from 1 x 1 up to waveLength x waveLength
 * The offset of a table of size s is ( s * numWaves )
 * So the offsets of the downsampled tables are
 * 1 x 1 : 	numWaves
 * 2 x 2 :  2 * numWaves
 * 4 * 4 : 	4 * numWaves
 * etc.
 * and the full-sized table is at ( waveLength * numWaves )
 */

/*
 * Opaque pointer to a memory block holding a stream structure.
 * Should be allocated using NT_globals.streamSizeBytes
 */
typedef void* _NT_stream;

/*
 * Structure used by NT_streamOpen().
 */
struct _NT_streamOpenData
{
	void*				streamBuffer;		// A buffer for use by the streaming system, of size NT_globals.streamBufferSizeBytes
	uint32_t			folder;				// The sample folder
	uint32_t			sample;				// The sample index within the folder
	float				velocity;			// Velocity in range 0.0-1.0, used for velocity switching
	uint32_t 			startOffset;		// The position in the sample to start playback, in sample frames
	bool 				reverse;			// Whether to start playback in the reverse direction
	_NT_roundRobinMode	rrMode;				// The round robin mode
};

extern "C" {

/*
 * Query whether the MicroSD card is mounted.
 * Plug-ins should defer any kind of card-based activity until the card is mounted,
 * which might be some considerable time after the plug-in is constructed.
 * All built-in algorithms watch for card (un)mount in step().
 */
bool		NT_isSdCardMounted(void);

/*
 * Get the number of sample folders.
 */
uint32_t	NT_getNumSampleFolders(void);

/*
 * Get info about the given folder.
 */
void		NT_getSampleFolderInfo( uint32_t folder, _NT_wavFolderInfo& info );

/*
 * Get info about the given sample file.
 */
void		NT_getSampleFileInfo( uint32_t folder, uint32_t sample, _NT_wavInfo& info );

/*
 * Read sample frames from a sample file.
 *
 * _NT_wavRequest contains data as follows:
 *
 * folder 		The folder index, 0 to NT_getNumSampleFolders()-1
 * sample 		The sample file index in the folder, 0 to _NT_wavFolderInfo::numSampleFiles-1
 * dst 			Pointer to memory buffer to receive the sample frames.
 * numFrames 	The number of frames to read.
 * startOffset	The number of frames at the start of the file to skip over.
 * channels		The desired channels (mono/stereo).
 * bits			The desired bit depth.
 * progress		Whether to display a progress indicator.
 * callback		The function to call when the read is complete.
 * 					It is passed callbackData and boolean success/failure.
 * callbackData	Data to be passed to the callback.
 *
 * If channels and bits don't match the actual file data,
 * the file will be converted to match the requested format.
 *
 * The request object should persist until the callback has been called.
 * You cannot for example declare the request on the stack.
 *
 * Returns true if the read was successfully initiated.
 * The callback will not be called if the function returns false.
 */
bool		NT_readSampleFrames( const _NT_wavRequest& request );

/*
 * Get the number of wavetables.
 */
uint32_t	NT_getNumWavetables(void);

/*
 * Get info about the given wavetable.
 */
void		NT_getWavetableInfo( uint32_t index, _NT_wavetableInfo& info );

/*
 * Read a wavetable into memory.
 *
 * Returns true if the read was successfully initiated.
 * The callback will not be called if the function returns false.
 */
bool		NT_readWavetable( _NT_wavetableRequest& request );

/*
 * A convenience function to evaluate a wavetable.
 *
 * Returns a value in the range ±1.0.
 */
float		NT_evaluateWavetable( const _NT_wavetableRequest& request, _NT_wavetableEvaluation& eval );

/*
 * Open a stream for playback.
 *
 * Returns boolean success.
 */
bool		NT_streamOpen( _NT_stream stream, const _NT_streamOpenData& data );

/*
 * Render frames from the stream,
 * placing the results in renderBuffer
 *
 * numFrames	The maximum number of frames to render.
 * speed		The playback speed, in sample frames per render frame.
 *
 * Returns the number of frames rendered.
 */
uint32_t 	NT_streamRender( _NT_stream stream, _NT_frame* renderBuffer, uint32_t numFrames, float speed );

}

#endif // _DISTINGNT_WAV_H
