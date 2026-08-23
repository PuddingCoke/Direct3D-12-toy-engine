#pragma once

#ifndef _GEAR_INPUT_MOUSE_INTERNAL_H_
#define _GEAR_INPUT_MOUSE_INTERNAL_H_

namespace Gear::Input::Mouse::Internal
{

	void resetDeltaValue();

	void pressLeft();

	void pressRight();

	void releaseLeft();

	void releaseRight();

	void scroll(const float delta);

	void setPosition(const float x, const float y);

	void move(const float deltaX, const float deltaY);

}

#endif // !_GEAR_INPUT_MOUSE_INTERNAL_H_
