#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureImporter::TextureLoadData TextureImporter::TextureLoader::LoadTexture(const std::string& Path, const TextureImporter::LoadFormat LoadFormat)
{
	TextureLoadData Temp;

	static auto GetLoadFormatSTBIType = [](const TextureImporter::LoadFormat Format)
		{
			switch (Format)
			{
			case TextureImporter::LoadFormat::GREY: return STBI_grey;
			case TextureImporter::LoadFormat::GREY_ALPHA: return STBI_grey_alpha;
			case TextureImporter::LoadFormat::RGB: return STBI_rgb;
			case TextureImporter::LoadFormat::RGB_ALPHA: return STBI_rgb_alpha;
			default: return STBI_default;
			}
		};

	Temp.PixelData = stbi_load(Path.c_str(), &Temp.Width, &Temp.Height, &Temp.NumColorChannels, GetLoadFormatSTBIType(LoadFormat));

	return Temp;
}

void TextureImporter::TextureLoader::FreeTextureLoadData(TextureLoadData& Data)
{
	stbi_image_free(Data.PixelData);
	Data = {};
}
