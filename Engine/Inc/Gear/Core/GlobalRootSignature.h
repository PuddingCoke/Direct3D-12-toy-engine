#pragma once

#ifndef _CORE_GLOBALROOTSIGNATURE_H_
#define _CORE_GLOBALROOTSIGNATURE_H_

#include<Gear/Core/D3D12Core/RootSignature.h>

namespace Core
{
	namespace GlobalRootSignature
	{

		D3D12Core::RootSignature* getGraphicsRootSignature();

		D3D12Core::RootSignature* getComputeRootSignature();

	}
}

#endif // !_CORE_GLOBALROOTSIGNATURE_H_