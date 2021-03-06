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

#ifdef HASHACKRF
#include <hackrf.h>
#endif

namespace Device {

	class HACKRF : public Device
	{
#ifdef HASHACKRF

		hackrf_device* device = NULL;
		hackrf_device_list_t* list = NULL;

		int LNA_Gain = 8;
		int VGA_Gain = 20;
		bool preamp = false;

		static int callback_static(hackrf_transfer* tf);
		void callback(uint8_t*, int);

		void applySettings();

	public:

		HACKRF() {if (hackrf_init() != HACKRF_SUCCESS) throw "HACKRF: Cannot open hackrf library"; }
		~HACKRF() { if(list) hackrf_device_list_free(list); }

		// Control
		void Open(uint64_t h);
		void Close();
		void Play();
		void Stop();

		bool isStreaming() { return Device::isStreaming() && hackrf_is_streaming(device) == HACKRF_TRUE; }
		virtual bool isCallback() { return true; }

		void getDeviceList(std::vector<Description>& DeviceList);

		// Settings
		void Print();
		void Set(std::string option, std::string arg);

#endif
	};
}
