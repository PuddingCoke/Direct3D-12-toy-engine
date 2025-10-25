#pragma once

#ifndef _INPUT_KEYBOARD_INTERNAL_H_
#define _INPUT_KEYBOARD_INTERNAL_H_

namespace Input
{
	namespace Keyboard
	{
		namespace Internal
		{

			void resetDeltaValue();

			void pressKey(const Key key);

			void releaseKey(const Key key);

		}
	}
}

#endif // !_INPUT_KEYBOARD_INTERNAL_H_
