#pragma once

#ifndef _LOGCOLOR_H_
#define _LOGCOLOR_H_

#include<string>

struct LogColor
{
	const wchar_t* code;

	bool operator!=(const LogColor& color) const;

	static const LogColor black;

	static const LogColor red;

	static const LogColor green;

	static const LogColor yellow;

	static const LogColor blue;

	static const LogColor magenta;

	static const LogColor cyan;

	static const LogColor white;

	static const LogColor brightBlack;

	static const LogColor brightRed;

	static const LogColor brightGreen;

	static const LogColor brightYellow;

	static const LogColor brightBlue;

	static const LogColor brightMagenta;

	static const LogColor brightCyan;

	static const LogColor brightWhite;

	//main text
	static const LogColor defaultColor;

	static const LogColor numericColor;

	static const LogColor timeStampColor;

	static const LogColor threadIdColor;

	static const LogColor classNameColor;

	static const LogColor successColor;

	static const LogColor errorColor;

	static const LogColor engineColor;

	static const LogColor userColor;
};

#endif // !_LOGCOLOR_H_
