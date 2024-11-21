#pragma once



class Message
{
	const wchar_t* mMessage;
	int mLength;

public:


	Message(const wchar_t* message);
	int GetLength();
	int GetCharacterCode(int index);
};

