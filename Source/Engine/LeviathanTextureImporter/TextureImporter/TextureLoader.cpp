#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureImporter::TextureLoadData TextureImporter::TextureLoader::LoadTexture(const std::string& Path)
{
	TextureLoadData Temp;
	Temp.PixelData = stbi_load(Path.c_str(), &Temp.Width, &Temp.Height, &Temp.NumColorChannels, 0);
	return Temp;
}

void TextureImporter::TextureLoader::FreeTextureLoadData(TextureLoadData& Data)
{
	stbi_image_free(Data.PixelData);
	Data = {};
}
