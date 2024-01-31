#pragma once

#ifndef _GLOBALROOTSIGNATURE_H_
#define _GLOBALROOTSIGNATURE_H_

#include<Gear/Core/GraphicsDevice.h>

class GlobalRootSignature
{
public:

	GlobalRootSignature(const GlobalRootSignature&) = delete;

	void operator=(const GlobalRootSignature&) = delete;

private:

	friend class Gear;

	static GlobalRootSignature* instance;

	GlobalRootSignature();

	ComPtr<ID3D12RootSignature> rootSignature;

};

#endif // !_GLOBALROOTSIGNATURE_H_
