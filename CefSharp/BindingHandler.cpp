#include "stdafx.h"
#include "BindingHandler.h"
#include "ScriptingObjectAttribute.h"
#include "CefCallbackWrapper.h"

using namespace System::Threading;

namespace CefSharp
{
	bool BindingHandler::IsNullableType(Type^ type)
	{
		// This is check traditionaly perform by this C# code:
		// return (type.IsGenericType && type.GetGenericTypeDefinition().Equals(typeof(Nullable<>)));
		// But we have some problems with Nullable<>::typeid.
		return Nullable::GetUnderlyingType(type) != nullptr;
	}

	/// <summary></summary>
	/// <return>Returns conversion cost, or -1 if no conversion available.</return>
	int BindingHandler::GetChangeTypeCost(Object^ value, Type^ conversionType)
	{
		// TODO: temporary Int64 support fully disabled
		if (conversionType == Int64::typeid 
			|| conversionType == Nullable<Int64>::typeid
			|| conversionType == UInt64::typeid 
			|| conversionType == Nullable<UInt64>::typeid
			)
			return -1;

		// Null conversion
		if (value == nullptr)
		{
			// TODO: This is check for reference type, may be not accuracy.
			if (conversionType->IsValueType == false) return 0;

			// Nullable types also can be converted to null without penalty.
			if (IsNullableType(conversionType)) return 0;

			// Non-reference and non-nullable types can not be converted from null.
			return -1;
		}

		// value is not null

		// value have same type - no conversion required
		Type^ valueType = value->GetType();
		if (valueType == conversionType) return 0;

		int baseCost = 0;

		// but conversionType can be Nullable
		Type^ targetType = Nullable::GetUnderlyingType(conversionType);
		if (targetType != nullptr)
		{
			// this is a nullable type, and it cost + 1
			conversionType = targetType;
			baseCost++;
		}
		if (valueType == conversionType) return baseCost + 0;

		if (valueType->IsValueType )
		{
			if (conversionType == Object::typeid)
			{
				return baseCost + 1;
			}
			
			if (valueType == Boolean::typeid)
			{
				// Boolean can be converted only to Boolean
				if(conversionType == Boolean::typeid) return baseCost + 0;
				return -1;
			}
			
			if (valueType == Int32::typeid)
			{
				int int32Val = safe_cast<int>(value);

				if (conversionType == Int32::typeid) return baseCost + 0;
				else if (conversionType == UInt32::typeid && (int32Val >= 0)) return baseCost + 1;
				else if (conversionType == Int16::typeid && (int32Val >= Int16::MinValue && int32Val <= Int16::MaxValue)) return baseCost + 2;
				else if (conversionType == UInt16::typeid && (int32Val >= UInt16::MinValue && int32Val <= UInt16::MaxValue)) return baseCost + 3;
				else if (conversionType == Char::typeid && (int32Val >= Char::MinValue && int32Val <= Char::MaxValue)) return baseCost + 4;
				else if (conversionType == SByte::typeid && (int32Val >= SByte::MinValue && int32Val <= SByte::MaxValue)) return baseCost + 5;
				else if (conversionType == Byte::typeid && (int32Val >= Byte::MinValue && int32Val <= Byte::MaxValue)) return baseCost + 6;
				else if (conversionType == Double::typeid) return baseCost + 9;
				else if (conversionType == Single::typeid) return baseCost + 10;
				else if (conversionType == Decimal::typeid) return baseCost + 11;
				else if (conversionType == Int64::typeid) return -1;
				else if (conversionType == UInt64::typeid) return -1;
				return -1;
			}
			
			if(valueType == Double::typeid)
			{
				double doubleVal = safe_cast<double>(value);

				if (conversionType == Double::typeid) return baseCost + 0;
				else if (conversionType == Single::typeid && (doubleVal >= Single::MinValue && doubleVal <= Single::MaxValue)) return baseCost + 1;
				else if (conversionType == Decimal::typeid /* && (doubleVal >= Decimal::MinValue && doubleVal <= Decimal::MaxValue) */) return baseCost + 2;
				else if (conversionType == Int32::typeid && (doubleVal >= Int32::MinValue && doubleVal <= Int32::MaxValue)) return baseCost + 3;
				else if (conversionType == UInt32::typeid && (doubleVal >= UInt32::MinValue && doubleVal <= UInt32::MaxValue)) return baseCost + 4;
				else if (conversionType == Int16::typeid && (doubleVal >= Int16::MinValue && doubleVal <= Int16::MaxValue)) return baseCost + 5;
				else if (conversionType == UInt16::typeid && (doubleVal >= UInt16::MinValue && doubleVal <= UInt16::MaxValue)) return baseCost + 6;
				else if (conversionType == Char::typeid && (doubleVal >= Char::MinValue && doubleVal <= Char::MaxValue)) return baseCost + 6;
				else if (conversionType == SByte::typeid && (doubleVal >= SByte::MinValue && doubleVal <= SByte::MaxValue)) return baseCost + 8;
				else if (conversionType == Byte::typeid && (doubleVal >= Byte::MinValue && doubleVal <= Byte::MaxValue)) return baseCost + 9;
				else if (conversionType == Int64::typeid) return -1;
				else if (conversionType == UInt64::typeid) return -1;
				return -1;
			}

			if (conversionType == Object::typeid)
			{
				return baseCost + 1;
			}
		}
		else
		{
			if(valueType == String::typeid)
			{
				// String can be converted only to String
				if(conversionType == String::typeid) 
				{
					return baseCost + 0;
				}
				if (conversionType == Object::typeid)
				{
					return baseCost + 1;
				}
				return -1;
			}

			if (conversionType->IsAssignableFrom(valueType))
			{
				int relCost = 0;
				for (Type^ t = valueType->BaseType; t != nullptr; t = t->BaseType)
				{
					relCost++;				
					if (t == conversionType)
					{
						break;
					}
					if (!conversionType->IsAssignableFrom(t))
					{
						break;
					}
				}				
				return baseCost + relCost;
			}
		}
		
		// No conversion available
		return -1;        
	}

	Object^ BindingHandler::ChangeType(Object^ value, Type^ conversionType)
	{
		if (GetChangeTypeCost(value, conversionType) < 0)
		{
			throw gcnew Exception("No conversion available.");
		}

		if (value == nullptr) return nullptr;

		if (conversionType->IsAssignableFrom(value->GetType()))
		{
			return value;
		}

		Type^ targetType = Nullable::GetUnderlyingType(conversionType);
		if (targetType != nullptr) conversionType = targetType;

		return Convert::ChangeType(value, conversionType);
	}


	CefRefPtr<CefV8Value> BindingHandler::ConvertToCefWithScripting(Object^ obj, Type^ type, CefRefPtr<CefV8Value> window, Dictionary<Object^, IntPtr>^ cache)
	{
		CefRefPtr<CefV8Value> result;
		if (tryConvertToCef(obj, type, result))
		{
			return result;
		}       

		
		if (cache != nullptr)
		{
			IntPtr ptr;
			if (cache->TryGetValue(obj, ptr))
			{
				return CefRefPtr<CefV8Value>((CefV8Value*)ptr.ToPointer());
			}
		}

		if (obj->GetType()->GetCustomAttributes(ScriptingObjectAttribute::typeid, true)->Length > 0)
		{
			CefRefPtr<CefV8Value> value = ResolveCefObject(obj, window, cache);
			if (cache != nullptr)
			{
				CefV8Value *valuePtr = value.get();
				valuePtr->AddRef();
				cache[obj] = IntPtr(valuePtr);
			}
			return value;
		}		

		throw gcnew Exception("Cannot convert object from CLR to Cef " + type->ToString() + ".");
	}
	
	// stateObj is an array<Object^>
	// [0] : method (MethodInfo^)
	// [1] : 'this' (Object^)
	// [2] : arguments (array<Object^>^)
	void MethodRunner(Object^ stateObj)
	{
		MethodInfo^ method;
		Object^ self;
		array<Object^>^ args;
		try{
			array<Object^>^ stateArgs = static_cast<array<Object^>^>(stateObj);
			method = static_cast<MethodInfo^>(stateArgs[0]);
			self = stateArgs[1];
			args = static_cast<array<Object^>^>(stateArgs[2]);
			method->Invoke(self, args);
		}
		finally
		{
			for(int i = 0; i < args->Length; i++)
			{
				CefCallbackWrapper^ callback = dynamic_cast<CefCallbackWrapper^>(args[i]);
				if(callback != nullptr)
				{
					callback->~CefCallbackWrapper();
				}
			}
		}
	}

	bool BindingHandler::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
	{
		CefRefPtr<BindingData> bindingData = static_cast<BindingData*>(object->GetUserData().get());
		Object^ self = bindingData->Get();

		if(self == nullptr) 
		{
			exception = "Binding's CLR object is null.";
			return true;
		}

		String^ memberName = toClr(name);
		Type^ type = self->GetType();
		array<System::Reflection::MemberInfo^>^ members = type->GetMember(memberName, MemberTypes::Method, 
			BindingFlags::Instance | BindingFlags::Public);

		if(members->Length == 0)
		{
			exception = toNative("No member named " + memberName + ".");
			return true;
		}

		//TODO: cache for type info here

		array<System::Object^>^ suppliedArguments = gcnew array<Object^>(arguments.size());
		try
		{
			for(int i = 0; i < suppliedArguments->Length; i++) 
			{
				suppliedArguments[i] = convertFromCef(arguments[i]);
			}
		}
		catch(System::Exception^ err)
		{
			exception = toNative(err->Message);
			return true;
		}

		// choose best method
		MethodInfo^ bestMethod;
		array<Object^>^ bestMethodArguments;
		int bestMethodCost = -1;
		bool bestMethodParamsMode = false;

		for (int i = 0; i < members->Length; i++)
		{
			MethodInfo^ method = (MethodInfo^) members[i];
			array<ParameterInfo^>^ parametersInfo = method->GetParameters();
			Type^ paramsType = nullptr;
			if (parametersInfo->Length > 0)
			{
				ParameterInfo^ param = parametersInfo[parametersInfo->Length - 1];
				if (param->GetCustomAttributes(ParamArrayAttribute::typeid, true)->Length > 0)
				{
					if (param->ParameterType->IsArray && param->ParameterType->GetArrayRank() == 1)
					{
						paramsType = param->ParameterType->GetElementType();
					}
				}
			}	

			if (suppliedArguments->Length == parametersInfo->Length || paramsType != nullptr && suppliedArguments->Length >= parametersInfo->Length - 1)
			{
				int failed = 0;
				int cost = 0;
				array<Object^>^ arguments = gcnew array<Object^>(parametersInfo->Length);
				bool paramsMode = suppliedArguments->Length < parametersInfo->Length;
				array<Object^>^ paramsArguments = nullptr;

				try
				{
					for (int p = 0; p < suppliedArguments->Length; p++)
					{
						System::Type^ paramType = p < parametersInfo->Length ? parametersInfo[p]->ParameterType : paramsType;
						int paramCost = -1;

						// check for "params" type
						if (paramsType != nullptr && p == parametersInfo->Length - 1)
						{
							paramCost = GetChangeTypeCost(suppliedArguments[p], paramType);
							if (paramCost < 0)
							{
								paramType = paramsType;								
								paramsMode = true;
								paramsArguments = gcnew array<Object^>(suppliedArguments->Length - parametersInfo->Length + 1);
								arguments[p] = paramsArguments;
								paramCost = GetChangeTypeCost(suppliedArguments[p], paramType);
							}
						}
						else
						{
							paramCost = GetChangeTypeCost(suppliedArguments[p], paramType);
						}
												
						if (paramCost < 0 )
						{
							failed++;
							break;
						}
						else
						{
							if (paramsMode && p >= parametersInfo->Length - 1 )
							{
								paramsArguments[p - parametersInfo->Length + 1] = ChangeType(suppliedArguments[p], paramType);
							}
							else
							{
								arguments[p] = ChangeType(suppliedArguments[p], paramType);
							}
							cost += paramCost;
						}
					}
				}
				catch(Exception^)
				{
					failed++;
				}

				if (failed > 0)
				{
					continue;
				}				

				if ((paramsMode <= bestMethodParamsMode && cost < bestMethodCost) || bestMethodCost < 0)
				{
					bestMethod = method;
					bestMethodArguments = arguments;
					bestMethodCost = cost;
					bestMethodParamsMode = paramsMode;
				}

				// this is best as possible cost
				if (cost == 0)
					break;
			}
		}

		if (bestMethod != nullptr)
		{
			try
			{
				/*if(bestMethod->ReturnType == System::Void::typeid)
				{
					array<Object^>^ state = {bestMethod, self, bestMethodArguments};
					ThreadPool::QueueUserWorkItem(gcnew WaitCallback(MethodRunner), state);
				} 
				else*/
				{
					try
					{
						Object^ result = bestMethod->Invoke(self, bestMethodArguments);				
						retval = ConvertToCefWithScripting(result, bestMethod->ReturnType, object,  _objectCache);
					}
					finally
					{
						for (int i = 0; i < bestMethodArguments->Length; i++)
						{
							array<Object^>^ params;
							if (bestMethodParamsMode && i == bestMethodArguments->Length - 1 && (params = dynamic_cast<array<Object^>^>(bestMethodArguments[i])) != nullptr)
							{
								for (int j = 0; j < params->Length; j++)
								{
									CefCallbackWrapper^ callback = dynamic_cast<CefCallbackWrapper^>(params[j]);
									if(callback != nullptr)
									{
										callback->~CefCallbackWrapper();
									}
								}
							}
							
							CefCallbackWrapper^ callback = dynamic_cast<CefCallbackWrapper^>(bestMethodArguments[i]);
							if(callback != nullptr)
							{
								callback->~CefCallbackWrapper();
							}
						}												
					}
				}
				return true;
			}            
			catch(System::Exception^ err)
			{
				while (System::Reflection::TargetInvocationException::typeid->IsAssignableFrom(err->GetType()))
				{
					if (err->InnerException == nullptr)
					{
						break;
					}
					err = err->InnerException;
				}
				exception = toNative(err->Message);
			}
		}
		else
		{
			exception = toNative("Argument mismatch for method \"" + memberName + "\".");
		}
		return true;
	}

	CefRefPtr<CefV8Value> BindingHandler::ResolveCefObject(Object^ obj, CefRefPtr<CefV8Value> window, Dictionary<Object^, IntPtr>^ cache)
	{
		IntPtr ptr;
		if (cache != nullptr && cache->TryGetValue(obj, ptr))
		{
			return CefRefPtr<CefV8Value>((CefV8Value*)ptr.ToPointer());
		}
		
		// wrap the managed object in an unmanaged wrapper
		CefRefPtr<BindingData> bindingData = new BindingData(obj);
		CefRefPtr<CefBase> userData = static_cast<CefRefPtr<CefBase>>(bindingData);

		// create the javascript object and associate the wrapped object
		CefRefPtr<CefV8Value> wrappedObject = window->CreateObject(NULL);
		wrappedObject->SetUserData(userData);

		// build a list of methods on the bound object
		array<MethodInfo^>^ methods = obj->GetType()->GetMethods(BindingFlags::Instance | BindingFlags::Public);
		IDictionary<String^, Object^>^ methodNames = gcnew Dictionary<String^, Object^>();
		for each(MethodInfo^ method in methods) 
		{
			methodNames[method->Name] = nullptr;
		}

		// create a corresponding javascript method for each c# method
		CefRefPtr<CefV8Handler> handler = static_cast<CefV8Handler*>(new BindingHandler(cache));
		for each(String^ methodName in methodNames->Keys)
		{
			CefString nameStr = toNative(methodName);
			wrappedObject->SetValue(nameStr, CefV8Value::CreateFunction(nameStr, handler), V8_PROPERTY_ATTRIBUTE_NONE);
		}

		return wrappedObject;
	}

	void BindingHandler::Bind(String^ name, Object^ obj, CefRefPtr<CefV8Value> window)
	{
		// wrap the managed object in an unmanaged wrapper
		CefRefPtr<BindingData> bindingData = new BindingData(obj);
		CefRefPtr<CefBase> userData = static_cast<CefRefPtr<CefBase>>(bindingData);

		// create the javascript object and associate the wrapped object
		CefRefPtr<CefV8Value> wrappedObject = window->CreateObject(NULL);
		wrappedObject->SetUserData(userData);

		// build a list of methods on the bound object
		array<MethodInfo^>^ methods = obj->GetType()->GetMethods(BindingFlags::Instance | BindingFlags::Public);
		IDictionary<String^, Object^>^ methodNames = gcnew Dictionary<String^, Object^>();
		for each(MethodInfo^ method in methods) 
		{
			methodNames[method->Name] = nullptr;
		}

		// create a corresponding javascript method for each c# method
		CefRefPtr<CefV8Handler> handler = static_cast<CefV8Handler*>(new BindingHandler());
		for each(String^ methodName in methodNames->Keys)
		{
			CefString nameStr = toNative(methodName);
			wrappedObject->SetValue(nameStr, CefV8Value::CreateFunction(nameStr, handler), V8_PROPERTY_ATTRIBUTE_NONE);
		}

		window->SetValue(toNative(name), wrappedObject, V8_PROPERTY_ATTRIBUTE_NONE);
	}

	void BindingHandler::BindCached(String^ name, Object^ obj, CefRefPtr<CefV8Value> window, Dictionary<Object^, IntPtr>^ cache)
	{        
		window->SetValue(toNative(name), ResolveCefObject(obj, window, cache), V8_PROPERTY_ATTRIBUTE_NONE);
	}
}