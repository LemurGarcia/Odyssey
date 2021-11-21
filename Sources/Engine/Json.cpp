
#include "Json.h"
#include <global.h>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>

Json::Value::Value(const char *value)
    : m_type(Type::String)
    , m_str_value(value)
    , m_int_value(0)
{}

Json::Value::Value(bool value)
    : m_type(Type::Bool)
    , m_bool_value(value)
{}

Json::Value::Value(int value)
    : m_type(Type::Int)
    , m_int_value(value)
{}

Json::Value::Value(float value)
    : m_type(Type::Float)
    , m_float_value(value)
{}

const std::string &Json::Value::GetStringValue() const
{
    if (m_type != Type::String)
        PV_WARNING("Wrong type string for this value");

    return m_str_value;
}

bool Json::Value::GetBoolValue() const
{
    if (m_type != Type::Bool)
        PV_WARNING("Wrong type bool for this value");

    return m_bool_value;
}

int Json::Value::GetIntValue() const
{
    if (m_type != Type::Int)
        PV_WARNING("Wrong type int for this value");

    return m_int_value;
}

float Json::Value::GetFloatValue() const
{
    if (m_type != Type::Float)
        PV_WARNING("Wrong type float for this value");

    return m_float_value;
}

Json::Json(const std::string &file_name)
{
    std::ifstream config_file(file_name);
    if (!config_file)
    {
        PV_WARNING("Could not open config " << file_name);
        return;
    }

    rapidjson::IStreamWrapper config_stream(config_file);
    rapidjson::Document document;
    document.ParseStream(config_stream);
    if (document.HasParseError())
    {
        PV_WARNING("Invalid config " << file_name << " (" <<
            document.GetErrorOffset() << "): " <<
            document.GetParseError());
        return;
    }

    for (rapidjson::Value::MemberIterator it = document.MemberBegin(); it != document.MemberEnd(); ++it)
    {
        if (!it->name.IsString())
        {
            PV_WARNING("Wrong section name");
            continue;
        }
        if (!it->value.IsObject())
        {
            PV_WARNING("Wrong section value for " << it->name.GetString());
            continue;
        }

        std::map<std::string, Value> &values = m_values[it->name.GetString()];

        for (rapidjson::Value::MemberIterator member_it = it->value.MemberBegin(); member_it != it->value.MemberEnd(); ++member_it)
        {
            if (!member_it->name.IsString())
            {
                PV_WARNING("Wrong value name");
                continue;
            }

            std::string value_name = member_it->name.GetString();
            if (member_it->value.IsString())
                values.emplace(value_name, member_it->value.GetString());
            else if (member_it->value.IsBool())
                values.emplace(value_name, member_it->value.GetBool());
            else if (member_it->value.IsInt())
                values.emplace(value_name, member_it->value.GetInt());
            else if (member_it->value.IsUint())
                values.emplace(value_name, (int)member_it->value.GetUint());
            else if (member_it->value.IsInt64())
                values.emplace(value_name, (int)member_it->value.GetInt64());
            else if (member_it->value.IsUint64())
                values.emplace(value_name, (int)member_it->value.GetUint64());
            else if (member_it->value.IsFloat())
                values.emplace(value_name, member_it->value.GetFloat());
            else if (member_it->value.IsDouble())
                values.emplace(value_name, (float)member_it->value.GetDouble());
        }
    }
}

Json::Value Json::GetValue(const std::string &section_name, const std::string &value_name, const Value &def_value) const
{
    auto found_section = m_values.find(section_name);
    if (found_section == m_values.end())
    {
        PV_WARNING("Could not find section " << section_name);
        return def_value;
    }

    auto found_value = found_section->second.find(value_name);
    if (found_value == found_section->second.end())
    {
        PV_WARNING("Could not find value " << section_name << "::" << value_name);
        return def_value;
    }

    return found_value->second;
}
