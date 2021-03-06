/*
Copyright(c) 2021-2022 jvde.github@gmail.com

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

#pragma once

#include <assert.h>

#include "Filters.h"

#include "Stream.h"
#include "Signals.h"

namespace DSP
{
	class SimplePLL : public SimpleStreamInOut<FLOAT32, FLOAT32>, public MessageIn<DecoderMessages>
	{
		std::vector<BIT> output;
		BIT prev = 0;

		float PLL = 0.0f;
		bool FastPLL = true;

	public:
		// StreamIn
		virtual void Receive(const FLOAT32* data, int len);

		// MessageIn
		virtual void Message(const DecoderMessages& in);
	};

	template <typename T>
	class Deinterleave : public StreamIn<T>
	{
		std::vector <T> output;
		int lastSymbol = 0;

	public:
		void setConnections(int n) { out.resize(n); }

		// Streams out
		std::vector<Connection<T>> out;

		// Streams in
		void Receive(const T* data, int len)
		{
			for (int i = 0; i < len; i++)
			{
				out[lastSymbol].Send(&data[i], 1);
				lastSymbol = (lastSymbol + 1) % out.size();
			}
		}
	};

	class Downsample2CIC5 : public SimpleStreamInOut<CFLOAT32, CFLOAT32>
	{
		CFLOAT32 h0 = 0, h1 = 0, h2 = 0, h3 = 0, h4 = 0;
		std::vector <CFLOAT32> output;

	public:
		void Receive(const CFLOAT32* data, int len);
	};

	class Decimate2 : public SimpleStreamInOut<CFLOAT32, CFLOAT32>
	{
		std::vector <CFLOAT32> output;

	public:
		void Receive(const CFLOAT32* data, int len);
	};

	class Upsample : public SimpleStreamInOut<CFLOAT32, CFLOAT32>
	{
		std::vector <CFLOAT32> output;

		FLOAT32 alpha = 0, increment = 1.0f;
		CFLOAT32 a = 0.0f;

		int idx_out = 0;

	public:

		void setParams(int n, int m) { assert(n < m); increment = (FLOAT32) n / (FLOAT32) m; }
		// StreamIn
		void Receive(const CFLOAT32* data, int len);
	};

	class DownsampleKFilter : public SimpleStreamInOut<CFLOAT32, CFLOAT32>
	{
		std::vector <CFLOAT32> output;

		std::vector <CFLOAT32> buffer;
		std::vector <FLOAT32> taps;

		int idx_in = 0;
		int idx_out = 0;

		int K = 1;

		static const int outputSize = 16384/2;

		inline CFLOAT32 dot(const CFLOAT32* data)
		{
				CFLOAT32 x = 0.0f;
				for (int i = 0; i < taps.size(); i++) x += taps[i] * *data++;
				return x;
		}

	public:

		void setParams(const std::vector<FLOAT32>& t, int k) { taps = t; K = k; }
		void setTaps(const std::vector<FLOAT32>& t) { taps = t; }
		void setK(int k) { K = k; }

		// StreamIn
		void Receive(const CFLOAT32* data, int len);
	};

	class FilterComplex : public SimpleStreamInOut<CFLOAT32, CFLOAT32>
	{
		std::vector <CFLOAT32> output;

		std::vector <CFLOAT32> buffer;
		std::vector <FLOAT32> taps;

		inline CFLOAT32 dot(const CFLOAT32* data)
		{
			CFLOAT32 x = 0.0f;
			for (int i = 0; i < taps.size(); i++) x += taps[i] * *data++;
			return x;
		}

	public:
		FilterComplex() { }

		void setTaps(const std::vector<FLOAT32>& t)
		{
			taps = t;
			buffer.resize(taps.size() * 2, 0.0f);
		}

		// StreamIn
		void Receive(const CFLOAT32* data, int len);
	};

	class Filter : public SimpleStreamInOut<FLOAT32, FLOAT32>
	{
		std::vector <FLOAT32> output;
		std::vector <FLOAT32> buffer;
		std::vector <FLOAT32> taps;

		inline FLOAT32 dot(const FLOAT32* data)
		{
			FLOAT32 x = 0.0f;
			for (int i = 0; i < taps.size(); i++) x += taps[i] * *data++;

			return x;
		}

	public:
		void setTaps(const std::vector<FLOAT32>& t)
		{
			taps = t;
			buffer.resize(taps.size() * 2, 0.0f);
		}

		// StreamIn
		void Receive(const FLOAT32* data, int len);
	};

	class FilterCIC5 : public SimpleStreamInOut<CFLOAT32, CFLOAT32>
	{
		CFLOAT32 h0 = 0, h1 = 0, h2 = 0, h3 = 0, h4 = 0;
		std::vector <CFLOAT32> output;

	public:
		void Receive(const CFLOAT32* data, int len);
	};

	class Rotate : public StreamIn<CFLOAT32>
	{
		std::vector <CFLOAT32> output_up, output_down;
		CFLOAT32 rot = 1.0f, mult = 1.0f;

	public:

		void setRotation(float angle) { mult = std::polar(1.0f, angle); }

		// Streams out
		Connection<CFLOAT32> up;
		Connection<CFLOAT32> down;

		// Streams in
		void Receive(const CFLOAT32* data, int len);
	};

	class SquareFreqOffsetCorrection : public SimpleStreamInOut<CFLOAT32, CFLOAT32>
	{
		std::vector <CFLOAT32> output;
		std::vector <CFLOAT32> fft_data;

		CFLOAT32 rot = 1.0f;
		int N = 2048;
		int logN = 11;
		int count = 0;
		int window = 750;

		void correctFrequency();

	public:
		void setParams(int,int);
		void Receive(const CFLOAT32* data, int len);
	};

	class DS_UINT16
	{
		uint32_t h0 = 0, h1 = 0, h2 = 0, h3 = 0, h4 = 0;

	public:

		int Run(uint32_t*, int, int);
		int Run(uint8_t*, uint32_t*, int, int);
		int Run(int8_t*, uint32_t*, int, int);
		int Run(uint32_t*, CFLOAT32*, int, int);

	};

	class Downsample32_CU8 : public SimpleStreamInOut<CU8, CFLOAT32>
	{
		std::vector <CFLOAT32> output;
		std::vector <uint32_t> buffer;

		DS_UINT16 DS1, DS2, DS3, DS4, DS5;

	public:
		void Receive(const CU8* data, int len);
	};

	class Downsample32_CS8 : public SimpleStreamInOut<CS8, CFLOAT32>
	{
		std::vector <CFLOAT32> output;
		std::vector <uint32_t> buffer;

		DS_UINT16 DS1, DS2, DS3, DS4, DS5;

	public:
		void Receive(const CS8* data, int len);
	};

	class Downsample16_CU8 : public SimpleStreamInOut<CU8, CFLOAT32>
	{
		std::vector <CFLOAT32> output;
		std::vector <uint32_t> buffer;

		DS_UINT16 DS1, DS2, DS3, DS4;

	public:
		void Receive(const CU8* data, int len);
	};

	class Downsample16_CS8 : public SimpleStreamInOut<CS8, CFLOAT32>
	{
		std::vector <CFLOAT32> output;
		std::vector <uint32_t> buffer;

		DS_UINT16 DS1, DS2, DS3, DS4;

	public:
		void Receive(const CS8* data, int len);
	};

	class Downsample8_CU8 : public SimpleStreamInOut<CU8, CFLOAT32>
	{
		std::vector <CFLOAT32> output;
		std::vector <uint32_t> buffer;

		DS_UINT16 DS1, DS2, DS3;

	public:
		void Receive(const CU8* data, int len);
	};

	class Downsample8_CS8 : public SimpleStreamInOut<CS8, CFLOAT32>
	{
		std::vector <CFLOAT32> output;
		std::vector <uint32_t> buffer;

		DS_UINT16 DS1, DS2, DS3;

	public:
		void Receive(const CS8* data, int len);
	};
}
