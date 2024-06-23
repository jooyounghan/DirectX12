#pragma once
#include <utility>

#define AutoZeroMemory(Object) ZeroMemory(&Object, sizeof(decltype(Object)));

#define ExitIfFailed(ErrorCode, IsFailed) if(FAILED(IsFailed)) { printf_s("Error Code : %s", #ErrorCode); exit(ErrorCode); }

#define NotPublicVariable(Variable)																	\
public:																								\
	inline void Set##Variable(const decltype(Variable)& ValueIn) { Variable = ValueIn; };			\
	inline void Set##Variable(decltype(Variable) && ValueIn) { Variable = std::move(ValueIn); };	\
	inline const decltype(Variable)& Get##Variable() { return Variable; };							\
