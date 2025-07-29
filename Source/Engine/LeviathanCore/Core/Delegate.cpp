#include "Delegate.h"

void Core::Delegate::BindFunction(void(*Function)())
{
	FunctionStubPtr = Function;
}

void Core::Delegate::BindLambda(void(*Lambda)())
{
	LambdaStubPtr = Lambda;
}

void Core::Delegate::Execute()
{
	if (FunctionStubPtr)
	{
		FunctionStubPtr();
	}

	if (ObjPtr && MethodStubPtr)
	{
		(*MethodStubPtr)(ObjPtr);
	}

	if (LambdaStubPtr)
	{
		LambdaStubPtr();
	}
}