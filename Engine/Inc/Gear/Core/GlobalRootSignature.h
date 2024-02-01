#pragma once

#ifndef _GLOBALROOTSIGNATURE_H_
#define _GLOBALROOTSIGNATURE_H_

#include<Gear/Core/RootSignature.h>

class GlobalRootSignature
{
public:

	GlobalRootSignature(const GlobalRootSignature&) = delete;

	void operator=(const GlobalRootSignature&) = delete;

private:

	friend class Gear;

	static GlobalRootSignature* instance;

	GlobalRootSignature();

	RootSignature* graphicsRootSignature;

	RootSignature* computeRootSignature;

};

#endif // !_GLOBALROOTSIGNATURE_H_
