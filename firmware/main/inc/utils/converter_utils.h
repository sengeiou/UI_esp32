#ifndef CONVERTER_UTILS_H_
#define CONVERTER_UTILS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "parson.h"
#include <string>
#include <limits>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>


namespace utils
{
    template<typename T>
    inline std::string int2hex(T i)
    {
        std::stringstream stream;
        stream << "0x" << std::setfill ('0') << std::setw(sizeof(T)*2) << std::hex << i;
        return stream.str();
    }
}

namespace utils::json
{
    inline JSON_Value* getObject(std::string json, std::string json_key)
    {
        JSON_Value* object = NULL;

        JSON_Value* root_value = json_parse_string(json.c_str());
        JSON_Object* root_object = json_value_get_object(root_value);
        
        JSON_Value* tmp_value = json_object_dotget_value(root_object, json_key.c_str());
        if(NULL == tmp_value)
        {
            object = NULL;
        }
        else
        {
            object = json_value_deep_copy(tmp_value);
        }
    
        json_value_free(root_value);

        return object;
    }

    template<class T>
    inline T getValue(std::string json, std::string json_key, T min_value, T max_value)
    {
        JSON_Value* root_value = json_parse_string(json.c_str());
        JSON_Object* root_object = json_value_get_object(root_value);

        JSON_Value* json_val;
            
        json_val = json_object_dotget_value(root_object, json_key.c_str());
        
        T new_value;
        if(NULL == json_val)
        {
            new_value = std::numeric_limits<T>::quiet_NaN();
        }
        else
        {
            new_value = static_cast<T>(json_value_get_number(json_val));
            
            //Range validation with saturation
            if(min_value != max_value)
            {
                new_value = (new_value > max_value) ? max_value : new_value;
                new_value = (new_value < min_value) ? min_value : new_value;
            }
        }
    
        json_value_free(root_value);

        return new_value;
    }
    
    template<class T>
    inline T getValue(std::string json, std::string json_key)
    {
        return getValue<T>(json, json_key, static_cast<T>(0), static_cast<T>(0));
    }

    template <>
    inline std::string getValue<std::string>(std::string json, std::string json_key)
    {
        JSON_Value* root_value = json_parse_string(json.c_str());
        JSON_Object* root_object = json_value_get_object(root_value);

        JSON_Value* json_val;
            
        json_val = json_object_dotget_value(root_object, json_key.c_str());
        
        std::string new_value;
        if(NULL == json_val)
        {
            new_value = "";
        }
        else
        {
            new_value = json_value_get_string(json_val);
        }
    
        json_value_free(root_value);

        return new_value;
    }

    template <>
    inline bool getValue<bool>(std::string json, std::string json_key)
    {
        JSON_Value* root_value = json_parse_string(json.c_str());
        JSON_Object* root_object = json_value_get_object(root_value);

        JSON_Value* json_val;
            
        json_val = json_object_dotget_value(root_object, json_key.c_str());
        
        bool new_value;
        if(NULL == json_val)
        {
            new_value = std::numeric_limits<bool>::quiet_NaN();
        }
        else
        {
            new_value = json_value_get_boolean(json_val);
        }
    
        json_value_free(root_value);

        return new_value;
    }
}

#endif // CONVERTER_UTILS_H_