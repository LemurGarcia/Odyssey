
#pragma once

#include <string>
#include <map>

class Json
{
public:
    class Value
    {
    public:
        Value(const char *value);
        Value(bool value);
        Value(int value);
        Value(float value);
        const std::string &GetStringValue() const;
        bool GetBoolValue() const;
        int GetIntValue() const;
        float GetFloatValue() const;
        bool Empty() const;

    private:
        enum class Type
        {
            Bool, Int, Float, String,
        };
        const Type m_type;
        const std::string m_str_value;
        union
        {
            const bool m_bool_value;
            const int m_int_value;
            const float m_float_value;
        };
    };

public:
    Json(const std::string &file_name);

    template<typename Type>
    Value GetValue(const std::string &section_name, const std::string &value_name, Type def_value) const
    {
        Value def(def_value);
        return GetValue(section_name, value_name, def);
    }

private:
    Value GetValue(const std::string &section_name, const std::string &value_name, const Value &def_value) const;

private:
    // section name -> value name -> value
    std::map<std::string, std::map<std::string, Value>> m_values;
};
