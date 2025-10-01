#pragma once

#include "Module.h"

namespace TextureImporter
{
	class TextureLoader;

	class TextureImporterModule : public Core::Module
	{
	private:
		std::unique_ptr<TextureImporter::TextureLoader> TextureLoaderInstance;

	public:
		TextureImporterModule();

		TextureImporter::TextureLoader& GetTextureLoader();
	};
}