#include "TextureImporterModule.h"
#include "TextureLoader.h"

TextureImporter::TextureImporterModule::TextureImporterModule()
	: TextureLoaderInstance(std::make_unique<TextureImporter::TextureLoader>())
{
}

TextureImporter::TextureLoader& TextureImporter::TextureImporterModule::GetTextureLoader()
{
	return *TextureLoaderInstance;
}
