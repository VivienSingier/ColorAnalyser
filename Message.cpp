#include "Message.h"
#include <cwchar>

Message::Message(const wchar_t* message)
{
	mMessage = message;
	mLength = std::wcslen(message);
}

int Message::GetLength()
{
	return mLength;
}

int Message::GetCharacterCode(int index)
{
	return (int)(mMessage[index]);
}