#include<Gear/Configuration.h>

Configuration::Configuration(const unsigned int width, const unsigned int height, const std::wstring title, const EngineUsage usage) :
	width(width), height(height), title(title), usage(usage)
{
}
