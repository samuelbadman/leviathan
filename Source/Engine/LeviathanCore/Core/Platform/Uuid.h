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
		size_t Hash() const;

		bool operator==(const Core::Uuid& Rhs) const;
	};
}

namespace std
{
	template<>
	struct hash<Core::Uuid>
	{
		size_t operator()(const Core::Uuid& Key) const
		{
			return hash<size_t>()(Key.Hash());
		}
	};
}