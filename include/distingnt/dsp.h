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

#ifndef _DISTINGNT_DSP_H
#define _DISTINGNT_DSP_H

#include <stdint.h>

/*
 * FFT methods.
 *
 * Currently a thin wrapper around an FFTReal implementation.
 *
 * Bin layout:

               |                | Positive FFT    | Negative FFT
   Bin         | Real part      | imaginary part  | imaginary part
   ------------+----------------+-----------------+---------------
   0           | f [0]          | 0               | 0
   1           | f [1]          | f [length/2+1]  | -f [length/2+1]
   ...         | f [...],       | f [...]         | -f [...]
   length/2-1  | f [length/2-1] | f [length-1]    | -f [length-1]
   length/2    | f [length/2]   | 0               | 0
   length/2+1  | f [length/2-1] | -f [length-1]   | f [length-1]
   ...         | f [...]        | -f [...]        | f [...]
   length-1    | f [1]          | -f [length/2+1] | f [length/2+1]

 * NT_FFT() and NT_IFFT() take a pointer to a _NT_FFT object, which holds
 * precomputed data and a work buffer. Plug-ins are responsible for providing
 * storage for this object, and for initializing it with NT_constructFFT().
 */

typedef uint8_t const * _NT_FFT;

/*
 * Return the size of memory allocation required by an _NT_FFT object
 * capable of supporting FFTs of the given size.
 */
inline constexpr size_t	NT_FFT_objectSize( uint32_t fftSizeLog2 )
{
	return 24 + 8 * ( 1 << fftSizeLog2 ) + 4 * ( ( 1 << ( fftSizeLog2 - 1 ) ) - 4 );
}

extern "C" {

// Construct an _NT_FFT object for the given FFT size.
void	NT_constructFFT( _NT_FFT fft, uint32_t fftSizeLog2 );

// Perform an FFT.
// f is the output buffer, laid out as above.
// t is the real-valued input.
void	NT_FFT( _NT_FFT fft, float* f, const float* t );

// Perform an Inverse FFT.
// f is the input buffer, laid out as above.
// t is the real-valued output.
void	NT_IFFT( _NT_FFT fft, const float* f, float* t );

}

#endif // _DISTINGNT_DSP_H
