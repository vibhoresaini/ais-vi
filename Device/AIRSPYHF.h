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

#include "Device.h"

#ifdef HASAIRSPYHF
#include <airspyhf.h>
#endif

namespace Device{

	class AIRSPYHF : public Device
	{
#ifdef HASAIRSPYHF

		struct airspyhf_device* dev = NULL;
		std::vector<uint32_t> rates;
		bool lost = false;

		bool preamp = false;
		bool treshold_high = false;

		static int callback_static(airspyhf_transfer_t* tf);
		void callback(CFLOAT32 *,int);

		void setTreshold(int);
		void setLNA(int);
		void setAGC(void);

		void applySettings();

	public:

		// Control
		void Open(uint64_t h);
		void Play();
		void Stop();
		void Close();

		bool isStreaming();
		bool isCallback() { return true; }

		void getDeviceList(std::vector<Description>& DeviceList);

		// Settings
		void Print();
		void Set(std::string option, std::string arg);
#endif
	};
}
