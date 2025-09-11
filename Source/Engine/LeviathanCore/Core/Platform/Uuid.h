#pragma once

namespace Core
{
	class Uuid
	{
	private:
		uint32_t Data1 = 0;
		uint16_t Data2 = 0;
		uint16_t Data3 = 0;
		std::array<uint8_t, 8> Data4 = {};

	public:
		void Generate();

		bool operator==(const Core::Uuid& Rhs);
	};
}