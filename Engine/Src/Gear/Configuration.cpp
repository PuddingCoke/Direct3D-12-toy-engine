#include<Gear/Configuration.h>

Configuration::Configuration(const uint32_t width, const uint32_t height, const std::wstring& title, const EngineUsage usage, const bool enableImGuiSurface) :
	width(width), height(height), title(title), usage(usage), enableImGuiSurface(enableImGuiSurface)
{
}
