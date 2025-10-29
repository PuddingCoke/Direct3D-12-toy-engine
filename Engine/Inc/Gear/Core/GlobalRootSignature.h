#pragma once

#ifndef _GEAR_CORE_GLOBALROOTSIGNATURE_H_
#define _GEAR_CORE_GLOBALROOTSIGNATURE_H_

#include<Gear/Core/D3D12Core/RootSignature.h>

namespace Gear
{
	namespace Core
	{
		namespace GlobalRootSignature
		{

			D3D12Core::RootSignature* getGraphicsRootSignature();

			D3D12Core::RootSignature* getComputeRootSignature();

		}
	}
}

#endif // !_GEAR_CORE_GLOBALROOTSIGNATURE_H_