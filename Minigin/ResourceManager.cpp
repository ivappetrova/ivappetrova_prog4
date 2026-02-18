#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "ResourceManager.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "Font.h"

namespace fs = std::filesystem;

void dae::ResourceManager::Init(const std::filesystem::path& dataPath)
{
	m_DataPath = dataPath;

	if (!TTF_Init())
	{
		throw std::runtime_error(std::string("Failed to load support for fonts: ") + SDL_GetError());
	}
}

std::shared_ptr<dae::Texture2D> dae::ResourceManager::LoadTexture(const std::string& file)
{
	const auto FULL_PATH = m_DataPath/file;
	const auto FILENAME = fs::path(FULL_PATH).filename().string();
	if (m_LoadedTextures.find(FILENAME) == m_LoadedTextures.end())
	{
		m_LoadedTextures.insert(std::pair(FILENAME, std::make_shared<Texture2D>(FULL_PATH.string())));
	}
	return m_LoadedTextures.at(FILENAME);
}

std::shared_ptr<dae::Font> dae::ResourceManager::LoadFont(const std::string& file, uint8_t size)
{
	const auto FULL_PATH = m_DataPath/file;
	const auto FILENAME = fs::path(FULL_PATH).filename().string();
	const auto KEY = std::pair<std::string, uint8_t>(FILENAME, size);
	if (m_LoadedFonts.find(KEY) == m_LoadedFonts.end())
	{
		m_LoadedFonts.insert(std::pair(KEY, std::make_shared<Font>(FULL_PATH.string(), size)));
	}
	return m_LoadedFonts.at(KEY);
}

void dae::ResourceManager::UnloadUnusedResources()
{
	for (auto it = m_LoadedTextures.begin(); it != m_LoadedTextures.end();)
	{
		if (it->second.use_count() == 1)
			it = m_LoadedTextures.erase(it);
		else
			++it;
	}

	for (auto it = m_LoadedFonts.begin(); it != m_LoadedFonts.end();)
	{
		if (it->second.use_count() == 1)
			it = m_LoadedFonts.erase(it);
		else
			++it;
	}
}
