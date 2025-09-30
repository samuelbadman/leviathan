#pragma once

namespace TextureImporter
{
	struct TextureLoadData
	{
		int32_t Width = 0;
		int32_t Height = 0;
		int32_t NumColorChannels = 0;
		uint8_t* PixelData = nullptr;
	};

	class TextureLoader
	{
	public:
		TextureLoadData LoadTexture(const std::string& Path);
		void FreeTextureLoadData(TextureLoadData& Data);
	};
}