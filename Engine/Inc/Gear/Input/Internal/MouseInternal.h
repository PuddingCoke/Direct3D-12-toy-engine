#pragma once

#ifndef _INPUT_MOUSE_INTERNAL_H_
#define _INPUT_MOUSE_INTERNAL_H_

namespace Input
{
	namespace Mouse
	{
		namespace Internal
		{

			void resetDeltaValue();

			void pressLeft();

			void pressRight();

			void releaseLeft();

			void releaseRight();

			void scroll(const float delta);

			void move(const float curX, const float curY);

		}
	}
}

#endif // !_INPUT_MOUSE_INTERNAL_H_
