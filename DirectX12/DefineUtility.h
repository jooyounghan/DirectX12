#pragma once
#include <windows.h>
#include <stdio.h>

#define AutoZeroMemory(Object) ZeroMemory(&Object, sizeof(decltype(Object)));

#define ExitIfFailed(ErrorCode, IsFailed)								\
	if(FAILED(IsFailed))												\
		{ printf_s("Error Code : %s", #ErrorCode); exit(ErrorCode); }