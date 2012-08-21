#include "stdafx.h"

using namespace System::Collections::Generic;

namespace CefSharp
{
    CefRefPtr<CefV8Value> convertToCef(Object^ obj, Type^ type)
    {
        CefRefPtr<CefV8Value> result;
		if (tryConvertToCef(obj, type, result))
		{
			return result;
		}
        //TODO: What exception type?
        throw gcnew Exception("Cannot convert object from CLR to Cef " + type->ToString() + ".");
    }	

	bool tryConvertToCef(Object^ obj, Type^ type, CefRefPtr<CefV8Value>& result)
    {
        if(type == Void::typeid)
        {
            result = CefV8Value::CreateUndefined();
			return true;
        }
        if(obj == nullptr)
        {
            result = CefV8Value::CreateNull();
			return true;
        }

        Type^ underlyingType = Nullable::GetUnderlyingType(type);
        if(underlyingType!=nullptr)type = underlyingType;

        if (type == Boolean::typeid)
        {
            result = CefV8Value::CreateBool(safe_cast<bool>(obj));
			return true;
        }
        if (type == Int32::typeid)
        {
            result = CefV8Value::CreateInt(safe_cast<int>(obj));
			return true;
        }
        if (type == String::typeid)
        {
            CefString str = toNative(safe_cast<String^>(obj));
            result = CefV8Value::CreateString(str);
			return true;
        }
        if (type == Double::typeid)
        {
            result = CefV8Value::CreateDouble(safe_cast<double>(obj));
			return true;
        }
        if (type == Decimal::typeid)
        {
            result = CefV8Value::CreateDouble( Convert::ToDouble(obj) );
			return true;
        }
        if (type == SByte::typeid)
        {
            result = CefV8Value::CreateInt( Convert::ToInt32(obj) );
			return true;
        }
        if (type == Int16::typeid)
        {
            result = CefV8Value::CreateInt( Convert::ToInt32(obj) );
			return true;
        }
        if (type == Int64::typeid)
        {
            result = CefV8Value::CreateDouble( Convert::ToDouble(obj) );
			return true;
        }
        if (type == Byte::typeid)
        {
            result = CefV8Value::CreateInt( Convert::ToInt32(obj) );
			return true;
        }
        if (type == UInt16::typeid)
        {
            result = CefV8Value::CreateInt( Convert::ToInt32(obj) );
			return true;
        }
        if (type == UInt32::typeid)
        {
            result = CefV8Value::CreateDouble( Convert::ToDouble(obj) );
			return true;
        }
        if (type == UInt64::typeid)
        {
            result = CefV8Value::CreateDouble( Convert::ToDouble(obj) );
			return true;
        }
        if (type == Single::typeid)
        {
            result = CefV8Value::CreateDouble( Convert::ToDouble(obj) );
			return true;
        }
        if (type == Char::typeid)
        {
            result = CefV8Value::CreateInt( Convert::ToInt32(obj) );
			return true;
        }
		if (type->IsArray)
        {
            System::Array^ managedArray = (System::Array^)obj;
            CefRefPtr<CefV8Value> cefArray = CefV8Value::CreateArray(managedArray->Length);

            for (int i = 0; i < managedArray->Length; i++)
            {
                System::Object^ arrObj;

                arrObj = managedArray->GetValue(i);

				if (arrObj != nullptr)
				{
					CefRefPtr<CefV8Value> cefObj = convertToCef(arrObj, arrObj->GetType());

					cefArray->SetValue(i, cefObj);
				}
				else
				{
					cefArray->SetValue(i, CefV8Value::CreateNull());
				}
            }

			result = cefArray;
            return true;
        }
        if (type->IsValueType && !type->IsPrimitive && !type->IsEnum)
        {
            cli::array<System::Reflection::FieldInfo^>^ fields = type->GetFields();
            CefRefPtr<CefV8Value> cefArray = CefV8Value::CreateArray(fields->Length);

            for (int i = 0; i < fields->Length; i++)
            {
                String^ fieldName = fields[i]->Name;

                CefString strFieldName = toNative(safe_cast<String^>(fieldName));

                Object^ fieldVal = fields[i]->GetValue(obj);

				if (fieldVal != nullptr)
				{
	                CefRefPtr<CefV8Value> cefVal = convertToCef(fieldVal, fieldVal->GetType());

		            cefArray->SetValue(strFieldName, cefVal, V8_PROPERTY_ATTRIBUTE_NONE);
				}
				else
				{
					cefArray->SetValue(strFieldName, CefV8Value::CreateNull(), V8_PROPERTY_ATTRIBUTE_NONE);
				}
            }

			result = cefArray;
            return true;
        }
        
		return false;
    }


	System::String^ stdToString(const std::string& s)
	{
		return gcnew System::String(s.c_str());
	}

    Object^ convertFromCef(CefRefPtr<CefV8Value> obj)
    {
		if (obj->IsNull() || obj->IsUndefined())
		{
			return nullptr;
		}

        if (obj->IsBool())
            return gcnew System::Boolean(obj->GetBoolValue());
        if (obj->IsInt())
            return gcnew System::Int32(obj->GetIntValue());
        if (obj->IsDouble())
            return gcnew System::Double(obj->GetDoubleValue());
        if (obj->IsString())
            return toClr(obj->GetStringValue());

		if (obj->IsArray())
		{
			int arrLength = obj->GetArrayLength();
			List<Object^>^ result = gcnew List<Object^>(arrLength);

			for (int i = 0; i < arrLength; i++)
			{
				result->Add(convertFromCef(obj->GetValue(i)));
			}

			return result;
			/*
			if (arrLength > 0)
			{
				std::vector<CefString> keys;
				if (obj->GetKeys(keys))
				{
					Dictionary<String^, Object^>^ result = gcnew Dictionary<String^, Object^>();

					for (int i = 0; i < arrLength; i++)
					{
						std::string p_key = keys[i].ToString();
						String^ p_keyStr = stdToString(p_key);

						if ((obj->HasValue(keys[i])) && (!p_keyStr->StartsWith("__")))
						{
							CefRefPtr<CefV8Value> data;

							data = obj->GetValue(keys[i]);
							if (data != nullptr)
							{
								Object^ p_data = convertFromCef(data);

								result->Add(p_keyStr, p_data);
							}
						}
					}

					return result;
				}
			}

			return nullptr;
			*/
		}

		if (obj->IsObject())
		{
			std::vector<CefString> keys;
			if (obj->GetKeys(keys))
			{
				int objLength = keys.size();
				if (objLength > 0)
				{
					Dictionary<String^, Object^>^ result = gcnew Dictionary<String^, Object^>();

					for (int i = 0; i < objLength; i++)
					{
						std::string p_key = keys[i].ToString();
						String^ p_keyStr = stdToString(p_key);

						if ((obj->HasValue(keys[i])) && (!p_keyStr->StartsWith("__")))
						{
							CefRefPtr<CefV8Value> data;

							data = obj->GetValue(keys[i]);
							if (data != nullptr)
							{
								Object^ p_data = convertFromCef(data);

								result->Add(p_keyStr, p_data);
							}
						}
					}

					return result;
				}
			}

			return nullptr;
		}

        //TODO: What exception type?
        throw gcnew Exception("Cannot convert object from Cef to CLR.");
    }
}