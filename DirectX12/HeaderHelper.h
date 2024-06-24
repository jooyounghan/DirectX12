#pragma once

#define MakeGetter(Variable)																		\
public:																								\
	inline const decltype(Variable)& Get##Variable() { return Variable; };							

#define MakeAddressGetter(Variable)																	\
public:																								\
	inline void Set##Variable(const decltype(Variable)& ValueIn) { Variable = ValueIn; };			

#define MakeSetter(Variable)																		\
public:																								\
	inline void Set##Variable(const decltype(Variable)& ValueIn) { Variable = ValueIn; };			\
	inline void Set##Variable(decltype(Variable) && ValueIn) { Variable = std::move(ValueIn); };	


#define MakeSetterGetter(Variable)																	\
	MakeGetter(Variable)																			\
	MakeSetter(Variable)																			