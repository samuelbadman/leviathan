#pragma once

namespace Core
{
	class Delegate
	{
	private:
		using FunctionStubType = void(*)();
		using MethodStubType = void(*)(void* object_ptr);
		using LambdaStubType = void(*)();

		FunctionStubType FunctionStubPtr = nullptr;

		void* ObjPtr = nullptr;
		MethodStubType MethodStubPtr = nullptr;

		LambdaStubType LambdaStubPtr = nullptr;

		template <class T, void (T::* Method)()>
		static void MethodStub(void* ObjectPtr)
		{
			T* pObj = static_cast<T*>(ObjectPtr);
			return (pObj->*Method)();
		}

	public:
		void BindFunction(void(*Function)());

		template <class T, void (T::* Method)()>
		void BindMethod(T* ObjectPtr)
		{
			ObjPtr = static_cast<void*>(ObjectPtr);
			MethodStubPtr = &MethodStub<T, Method>;
		}

		void BindLambda(void(*Lambda)());
		void Execute();
	};
}