#pragma once

#define DECLARE_SINGLE_DELEGATE(Name)\
	class Name\
	{\
	private:\
		using FunctionStubType = void(*)();\
		using MethodStubType = void(*)(void* object_ptr);\
		using LambdaStubType = void(*)();\
\
		FunctionStubType FunctionStubPtr = nullptr;\
\
		void* ObjPtr = nullptr;\
		MethodStubType MethodStubPtr = nullptr;\
\
		LambdaStubType LambdaStubPtr = nullptr;\
\
		template <class T, void (T::* Method)()>\
		static void MethodStub(void* ObjectPtr)\
		{\
			T* pObj = static_cast<T*>(ObjectPtr);\
			return (pObj->*Method)();\
		}\
\
	public:\
		void BindFunction(void(*Function)())\
		{\
			FunctionStubPtr = Function;\
		}\
\
		void UnbindFunction()\
		{\
			FunctionStubPtr = nullptr;\
		}\
\
		template <class T, void (T::* Method)()>\
		void BindMethod(T* ObjectPtr)\
		{\
			ObjPtr = static_cast<void*>(ObjectPtr);\
			MethodStubPtr = &MethodStub<T, Method>;\
		}\
\
		void UnbindMethod()\
		{\
			ObjPtr = nullptr;\
			MethodStubPtr = nullptr;\
		}\
\
		void BindLambda(void(*Lambda)())\
		{\
			LambdaStubPtr = Lambda;\
		}\
\
		void UnbindLambda()\
		{\
			LambdaStubPtr = nullptr;\
		}\
\
		void Clear()\
		{\
			UnbindFunction();\
			UnbindMethod();\
			UnbindLambda();\
		}\
\
		void Execute()\
		{\
			if (FunctionStubPtr)\
			{\
				FunctionStubPtr();\
			}\
\
			if (ObjPtr && MethodStubPtr)\
			{\
				(*MethodStubPtr)(ObjPtr);\
			}\
\
			if (LambdaStubPtr)\
			{\
				LambdaStubPtr();\
			}\
		}\
	};

#define DECLARE_MULTI_DELEGATE(Name)\
	class Name\
	{\
	private:\
		using FunctionStubType = void(*)();\
		using MethodStubType = void(*)(void* object_ptr);\
		using LambdaStubType = void(*)();\
\
		std::vector<FunctionStubType> FunctionStubPtrs = {};\
		std::vector<std::pair<void*, MethodStubType>> MethodStubPtrs = {};\
		std::vector<LambdaStubType> LambdaStubPtrs = {};\
\
		template <class T, void (T::* Method)()>\
		static void MethodStub(void* ObjectPtr)\
		{\
			T* pObj = static_cast<T*>(ObjectPtr);\
			return (pObj->*Method)();\
		}\
\
	public:\
		void AddFunction(void(*Function)())\
		{\
			FunctionStubPtrs.push_back(Function);\
		}\
\
		void RemoveFunction(void(*Function)())\
		{\
			FunctionStubPtrs.erase(std::remove(FunctionStubPtrs.begin(), FunctionStubPtrs.end(), Function));\
		}\
\
		template <class T, void (T::* Method)()>\
		void AddMethod(T* ObjectPtr)\
		{\
			MethodStubPtrs.emplace_back(static_cast<void*>(ObjectPtr), &MethodStub<T, Method>);\
		}\
\
		template <class T, void (T::* Method)()>\
		void RemoveMethod(T* ObjectPtr)\
		{\
			MethodStubPtrs.erase(std::remove(MethodStubPtrs.begin(), MethodStubPtrs.end(), std::pair<void*, MethodStubType>(static_cast<void*>(ObjectPtr), &MethodStub<T, Method>)));\
		}\
\
		void AddLambda(void(*Lambda)())\
		{\
			LambdaStubPtrs.push_back(Lambda);\
		}\
\
		void RemoveLambda(void(*Lambda)())\
		{\
			LambdaStubPtrs.erase(std::remove(LambdaStubPtrs.begin(), LambdaStubPtrs.end(), Lambda));\
		}\
\
		void Clear()\
		{\
			FunctionStubPtrs.clear();\
			MethodStubPtrs.clear();\
			LambdaStubPtrs.clear();\
		}\
\
		void Execute()\
		{\
			for (const FunctionStubType& Stub : FunctionStubPtrs)\
			{\
				if (Stub)\
				{\
					Stub();\
				}\
			}\
\
			for (const std::pair<void*, MethodStubType>& Stub : MethodStubPtrs)\
			{\
				if (Stub.first && Stub.second)\
				{\
					(*Stub.second)(Stub.first);\
				}\
			}\
\
			for (const LambdaStubType& Stub : LambdaStubPtrs)\
			{\
				if (Stub)\
				{\
					Stub();\
				}\
			}\
		}\
	};
