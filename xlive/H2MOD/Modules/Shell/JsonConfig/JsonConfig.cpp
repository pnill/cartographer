#include "stdafx.h"
#include <rapidjson/prettywriter.h>

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/pointer.h"
using namespace rapidjson;

enum json_config_error : int
{
	success,
    file_open_error,
    json_parse_error
};

class json_config {

private:
    std::wstring filename_;
    Document doc_;
    std::vector<std::string> key_path;
public:
    json_config(const std::wstring& filename)
        : filename_(filename) {}

    json_config_error load() {
        std::ifstream ifs(filename_);
        if (!ifs) {
            // The file does not exist, so create it.
            std::ofstream ofs(filename_);
            if (!ofs) {
                throw std::runtime_error("Failed to create file");
            }
            ofs << "{}";  // Initialize with an empty JSON object.
            ofs.close();
            ifs.open(filename_);
        }
        IStreamWrapper isw(ifs);
        Document d;
        doc_.SetObject();
        if (doc_.ParseStream(isw).HasParseError())
            return json_config_error::json_parse_error;

        return json_config_error::success;
    }

    void save() {
        std::ofstream ofs(filename_);
        OStreamWrapper osw(ofs);
        PrettyWriter<OStreamWrapper> writer(osw);
        writer.SetIndent('\t', 1);  // Set indentation to use tabs.
        doc_.Accept(writer);
    }

    Value* get_current_pointer()
    {
        // Check if path is empty, if so, return a pointer to the root document object
        if (key_path.empty()) {
            return Pointer("").Get(doc_);
        }

        // Obtain a pointer to the root document object
        Value* value = Pointer("").Get(doc_);

        // Iterate through each element in the path vector
        for (const auto& element : key_path)
        {
            // Check if the current value is an object
            if (value->IsObject()) {
                // If the current element is not a member of the object, add a new member with the given name
                if (!value->HasMember(element.c_str())) {
                    Value key;
                    key.SetString(element.c_str(), doc_.GetAllocator());
                    value->AddMember(key, rapidjson::Value(rapidjson::kObjectType), doc_.GetAllocator());
                }
                // Update the value pointer to point to the member with the given name
                value = &((*value)[element.c_str()]);
            }
            else {
                // If the current value is not an object, return a null pointer
                return nullptr;
            }
        }

        // Return a pointer to the final value object in the path
        return value;
    }

    json_config& operator[](const char* key)
    {
        key_path.push_back(key);
        return *this;
    }

    // A Map to store default variables for the current instance of the class.
    //
    // json_config h2config(config.json);
    // h2config.defaultValues_["crosshair_offset"] = NaN
    //
    std::unordered_map<std::string, Value> defaultValues_;



    template<typename T>
    T get(const char* key, T defaultValue = T{}) {
        // Check if the key exists in the document

        Value* current_object = get_current_pointer();
        key_path.clear();
        Value v;

        auto it = defaultValues_.find(key);
        if (!current_object->HasMember(key)) {
            if (it == defaultValues_.end())
                return defaultValue;
        	v = defaultValues_[key];
        }
        
        v = (*current_object)[key];

    	constexpr bool is_rapidjson_type_v =
            std::is_same_v<T, bool> ||
            std::is_same_v<T, int> ||
            std::is_same_v<T, long> ||
            std::is_same_v<T, unsigned> ||
            std::is_same_v<T, int64_t> ||
            std::is_same_v<T, uint64_t> ||
            std::is_same_v<T, double> ||
            std::is_same_v<T, float> ||
            std::is_same_v<T, const char*>;

        if constexpr (is_rapidjson_type_v) {
            if (v.Is<T>())
                return v.Get<T>();

            if (defaultValue == T{} && it != defaultValues_.end())
                return defaultValues_[key].Get<T>();

            return defaultValue;
        }
        else if constexpr (std::is_same_v<T, short>) {
            if(v.Is<int>())
                return static_cast<short>(v.GetInt());

            if (defaultValue == T{} && it != defaultValues_.end())
                return static_cast<short>(defaultValues_[key].GetInt());

            return defaultValue;
        }
        else if constexpr (std::is_same_v<T, unsigned short>) {
            if(v.Is<unsigned int>())
                return static_cast<unsigned short>(v.GetUint());

        	if (defaultValue == T{} && it != defaultValues_.end())
                return static_cast<unsigned short>(defaultValues_[key].GetUint());

            return defaultValue;
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            if(v.Is<const char*>())
                return v.GetString();

            if (defaultValue == T{} && it != defaultValues_.end())
                return defaultValues_[key].GetString();

            return defaultValue;
        }
        else if constexpr (std::is_same_v<T, real_point3d>) {
            if (v[0].Is<float>() && v[1].Is<float>() && v[2].Is<float>()) {
                auto x = v[0].GetFloat();
                auto y = v[1].GetFloat();
                auto z = v[2].GetFloat();
                return real_point3d(x, y, z);
            }

            if (defaultValue == T{} && it != defaultValues_.end())
            {
                auto x = defaultValues_[key][0].GetFloat();
                auto y = defaultValues_[key][1].GetFloat();
                auto z = defaultValues_[key][2].GetFloat();
                return real_point3d(x, y, z);
            }
            return defaultValue;
        }
        else {
            // Unsupported type
            static_assert(sizeof(T) == 0, "Unsupported type in json_config::get()");
        }
    }

    template<typename T>
    void get(const char* key, T* out_variable, T defaultValue = T{})
    {
        T val = get(key, defaultValue);
        *out_variable = val;
    }

    template<typename T>
    void set(const char* key, T value, bool auto_save = false) {

        Value* current_object = get_current_pointer();
        key_path.clear();

        bool is_new = !current_object->HasMember(key);
        Value k(key, doc_.GetAllocator());

        constexpr bool is_rapidjson_type_v =
            std::is_same_v<T, bool> ||
            std::is_same_v<T, int> ||
            std::is_same_v<T, long> ||
            std::is_same_v<T, unsigned> ||
            std::is_same_v<T, int64_t> ||
            std::is_same_v<T, uint64_t> ||
            std::is_same_v<T, double> ||
            std::is_same_v<T, float>;
			//Not included due to the requirement of creating a String Reference for the string.
    		//std::is_same_v<T, const char*>;

        if constexpr (is_rapidjson_type_v) {
            if (is_new)
                current_object->AddMember(k, value, doc_.GetAllocator());
            else
                (*current_object)[key].Set<T>(value);
        }
        else if constexpr (std::is_same_v<T, short>) {
            if(is_new)
                current_object->AddMember(k, value, doc_.GetAllocator());
            else
				(*current_object)[key].SetInt(value);
        }
        else if constexpr (std::is_same_v<T, unsigned short>) {
            if (is_new)
                current_object->AddMember(k, value, doc_.GetAllocator());
            else
        		(*current_object)[key].SetUint(value);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            if (is_new)
            {
                Value v(value.c_str(), value.size(), doc_.GetAllocator());
                current_object->AddMember(k, v, doc_.GetAllocator());
            }
            else
                (*current_object)[key].SetString(value.c_str(), doc_.GetAllocator());
        }
        else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
            if (is_new)
                current_object->AddMember(k, StringRef(value), doc_.GetAllocator());
            else
                (*current_object)[key].SetString(StringRef(value));
        }
        else if constexpr (std::is_same_v<T, real_point3d>) {
            if (is_new)
            {
                Value vals(rapidjson::kArrayType);
                vals.PushBack(value.x, doc_.GetAllocator());
                vals.PushBack(value.y, doc_.GetAllocator());
                vals.PushBack(value.z, doc_.GetAllocator());
                current_object->AddMember(k, vals, doc_.GetAllocator());
            }
            else 
            {
                (*current_object)[key].Clear();
                (*current_object)[key].PushBack(value.x, doc_.GetAllocator());
                (*current_object)[key].PushBack(value.y, doc_.GetAllocator());
                (*current_object)[key].PushBack(value.z, doc_.GetAllocator());
            }
        }
        else {
            // Unsupported type
            static_assert(sizeof(T) == 0, "Unsupported type in json_config::set()");
        }

        if (auto_save)
            save();
    }
    template<typename T>
    void setDefaultValue(const char* key, T value) {
        defaultValues_[key] = value;
    }

};