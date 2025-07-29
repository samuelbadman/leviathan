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

#define DECLARE_SINGLE_DELEGATE_OneParam(Name, Param1Type)\
	class Name\
	{\
	private:\
		using FunctionStubType = void(*)(Param1Type);\
		using MethodStubType = void(*)(void* object_ptr, Param1Type);\
		using LambdaStubType = void(*)(Param1Type);\
\
		FunctionStubType FunctionStubPtr = nullptr;\
\
		void* ObjPtr = nullptr;\
		MethodStubType MethodStubPtr = nullptr;\
\
		LambdaStubType LambdaStubPtr = nullptr;\
\
		template <class T, void (T::* Method)(Param1Type)>\
		static void MethodStub(void* ObjectPtr, Param1Type Param1)\
		{\
			T* pObj = static_cast<T*>(ObjectPtr);\
			return (pObj->*Method)(Param1);\
		}\
\
	public:\
		void BindFunction(void(*Function)(Param1Type))\
		{\
			FunctionStubPtr = Function;\
		}\
\
		void UnbindFunction()\
		{\
			FunctionStubPtr = nullptr;\
		}\
\
		template <class T, void (T::* Method)(Param1Type)>\
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
		void BindLambda(void(*Lambda)(Param1Type))\
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
		void Execute(Param1Type Param1)\
		{\
			if (FunctionStubPtr)\
			{\
				FunctionStubPtr(Param1);\
			}\
\
			if (ObjPtr && MethodStubPtr)\
			{\
				(*MethodStubPtr)(ObjPtr, Param1);\
			}\
\
			if (LambdaStubPtr)\
			{\
				LambdaStubPtr(Param1);\
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

#define DECLARE_MULTI_DELEGATE_OneParam(Name, Param1Type)\
	class Name\
	{\
	private:\
		using FunctionStubType = void(*)(Param1Type);\
		using MethodStubType = void(*)(void* object_ptr, Param1Type);\
		using LambdaStubType = void(*)(Param1Type);\
\
		std::vector<FunctionStubType> FunctionStubPtrs = {};\
		std::vector<std::pair<void*, MethodStubType>> MethodStubPtrs = {};\
		std::vector<LambdaStubType> LambdaStubPtrs = {};\
\
		template <class T, void (T::* Method)(Param1Type)>\
		static void MethodStub(void* ObjectPtr, Param1Type Param1)\
		{\
			T* pObj = static_cast<T*>(ObjectPtr);\
			return (pObj->*Method)(Param1);\
		}\
\
	public:\
		void AddFunction(void(*Function)(Param1Type))\
		{\
			FunctionStubPtrs.push_back(Function);\
		}\
\
		void RemoveFunction(void(*Function)(Param1Type))\
		{\
			FunctionStubPtrs.erase(std::remove(FunctionStubPtrs.begin(), FunctionStubPtrs.end(), Function));\
		}\
\
		template <class T, void (T::* Method)(Param1Type)>\
		void AddMethod(T* ObjectPtr)\
		{\
			MethodStubPtrs.emplace_back(static_cast<void*>(ObjectPtr), &MethodStub<T, Method>);\
		}\
\
		template <class T, void (T::* Method)(Param1Type)>\
		void RemoveMethod(T* ObjectPtr)\
		{\
			MethodStubPtrs.erase(std::remove(MethodStubPtrs.begin(), MethodStubPtrs.end(), std::pair<void*, MethodStubType>(static_cast<void*>(ObjectPtr), &MethodStub<T, Method>)));\
		}\
\
		void AddLambda(void(*Lambda)(Param1Type))\
		{\
			LambdaStubPtrs.push_back(Lambda);\
		}\
\
		void RemoveLambda(void(*Lambda)(Param1Type))\
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
		void Execute(Param1Type Param1)\
		{\
			for (const FunctionStubType& Stub : FunctionStubPtrs)\
			{\
				if (Stub)\
				{\
					Stub(Param1);\
				}\
			}\
\
			for (const std::pair<void*, MethodStubType>& Stub : MethodStubPtrs)\
			{\
				if (Stub.first && Stub.second)\
				{\
					(*Stub.second)(Stub.first, Param1);\
				}\
			}\
\
			for (const LambdaStubType& Stub : LambdaStubPtrs)\
			{\
				if (Stub)\
				{\
					Stub(Param1);\
				}\
			}\
		}\
	};
