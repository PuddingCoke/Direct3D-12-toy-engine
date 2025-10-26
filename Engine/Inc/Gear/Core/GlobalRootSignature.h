#pragma once

#ifndef _CORE_GLOBALROOTSIGNATURE_H_
#define _CORE_GLOBALROOTSIGNATURE_H_

#include<Gear/Core/RootSignature.h>

namespace Core
{
	namespace GlobalRootSignature
	{

		RootSignature* getGraphicsRootSignature();

		RootSignature* getComputeRootSignature();

	}
}

#endif // !_CORE_GLOBALROOTSIGNATURE_H_