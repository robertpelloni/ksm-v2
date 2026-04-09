#pragma once
#include <Siv3D.hpp>

namespace Hardware::Lighting
{
	struct ControllerProfile
	{
		String name = U"Generic";
		uint16_t vid = 0x1973;
		uint16_t pid = 0x2001;

		uint8_t reportId = 0x00;
		int32 reportLength = 4;

		int32 buttonsByte = 1;
		int32 btABit = 0;
		int32 btBBit = 1;
		int32 btCBit = 2;
		int32 btDBit = 3;
		int32 fxLBit = 4;
		int32 fxRBit = 5;

		int32 laserLByte = 2;
		int32 laserRByte = 3;
	};
}
