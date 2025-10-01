#pragma once

namespace TextureImporter
{
	enum class LoadFormat : uint8_t
	{
		GREY = 0,
		GREY_ALPHA,
		RGB,
		RGB_ALPHA,
		MAX
	};

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
		TextureLoadData LoadTexture(const std::string& Path, const TextureImporter::LoadFormat LoadFormat);
		void FreeTextureLoadData(TextureLoadData& Data);
	};
}