#pragma once

namespace Core
{
	class Delegate
	{
	private:
		using FunctionStubType = void(*)();
		using MethodStubType = void(*)(void* object_ptr);

		FunctionStubType FunctionStubPtr = nullptr;

		void* ObjPtr = nullptr;
		MethodStubType MethodStubPtr = nullptr;

		template <class T, void (T::* TMethod)()>
		static void MethodStub(void* ObjectPtr)
		{
			T* p = static_cast<T*>(ObjectPtr);
			return (p->*TMethod)();
		}

	public:
		template<void(*TFunction)()>
		void BindFunction()
		{
			if (ObjPtr && MethodStubPtr)
			{
				return;
			}

			FunctionStubPtr = TFunction;
		}

		template <class T, void (T::* TMethod)()>
		void BindMethod(T* ObjectPtr)
		{
			if (FunctionStubPtr)
			{
				return;
			}

			ObjPtr = static_cast<void*>(ObjectPtr);
			MethodStubPtr = &MethodStub<T, TMethod>;
		}

		void operator()() const
		{
			if (FunctionStubPtr)
			{
				return FunctionStubPtr();
			}
			else if (ObjPtr && MethodStubPtr)
			{
				return (*MethodStubPtr)(ObjPtr);
			}
			else
			{
				return; // TODO: Assert?
			}
		}
	};
}