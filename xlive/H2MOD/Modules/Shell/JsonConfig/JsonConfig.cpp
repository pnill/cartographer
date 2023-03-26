#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <rapidjson/prettywriter.h>

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/writer.h"

using namespace rapidjson;

class json_config {
public:
    json_config(const std::wstring& filename)
        : filename_(filename), doc_(load(filename)) {}

    Document load(const std::wstring& filename) {
        std::ifstream ifs(filename);
        if (!ifs) {
            // The file does not exist, so create it.
            std::ofstream ofs(filename);
            if (!ofs) {
                throw std::runtime_error("Failed to create file");
            }
            ofs << "{}";  // Initialize with an empty JSON object.
            ofs.close();
            ifs.open(filename);
        }
        IStreamWrapper isw(ifs);
        Document d;
        d.ParseStream(isw);
        return d;
    }

    void save() {
        std::ofstream ofs(filename_);
        OStreamWrapper osw(ofs);
        PrettyWriter<OStreamWrapper> writer(osw);
        writer.SetIndent('\t', 1);  // Set indentation to use tabs.
        doc_.Accept(writer);
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
        if (!doc_.HasMember(key)) {
            if (defaultValues_.find(key) != defaultValues_.end()) {
                return defaultValues_[key];
            }
			return defaultValue;
        }

        const Value& v = doc_[key];
        // Check if the value has the correct data type
        if (!v.Is<T>()) {
            if (defaultValues_.find(key) != defaultValues_.end()) {
                return defaultValues_[key];
            }
            return defaultValue;
        }
        // If the value exists and has the correct data type, return it
        return v.Get<T>();
    }

    // Specialization for float
    template<>
    float get<float>(const char* key, float defaultValue) {
        if (!doc_.HasMember(key)) {
            if (defaultValues_.find(key) != defaultValues_.end()) {
                return defaultValues_[key].GetFloat();
            }
        }
        const Value& v = doc_[key];
        if (!v.IsFloat()) {
            if (defaultValues_.find(key) != defaultValues_.end()) {
                return defaultValues_[key].GetFloat();
            }
        }
        return v.GetFloat();
    }
    template<>
    std::string get(const char* key, std::string defaultValue) {
        if (!doc_.HasMember(key)) {
            if (defaultValues_.find(key) != defaultValues_.end()) {
                return defaultValues_[key].GetString();
            }
            return defaultValue;
        }
        const Value& v = doc_[key];
        if (!v.IsString()) {
            if (defaultValues_.find(key) != defaultValues_.end()) {
                return defaultValues_[key].GetString();
            }
            return defaultValue;
        }
        return v.GetString();
    }
    template<>
    int get(const char* key, int defaultValue) {
        if (!doc_.HasMember(key)) {
            if (defaultValues_.find(key) != defaultValues_.end()) {
                return defaultValues_[key].GetInt();
            }
            return defaultValue;
        }
        const Value& v = doc_[key];
        if (!v.IsInt()) {
            if (defaultValues_.find(key) != defaultValues_.end()) {
                return defaultValues_[key].GetInt();
            }
            return defaultValue;
        }
        return v.GetInt();
    }
    template<typename T>
    void set(const char* key, T value) {
        // Check if the key already exists in the document
        if (doc_.HasMember(key)) {
            doc_[key].Set<T>(value);
        }
        else {
            // If the key does not exist in the document, add a new key-value pair to the document
            doc_.AddMember(Value(key, doc_.GetAllocator()).Move(), Value(value), doc_.GetAllocator());
        }

    }
    // Specialization for float
    template<>
    void set(const char* key, float value) {
        if (doc_.HasMember(key)) {
            doc_[key].SetFloat(value);
        }
        else {
            doc_.AddMember(Value(key, doc_.GetAllocator()).Move(), value, doc_.GetAllocator());
        }

    }
    template<>
    void set(const char* key, std::string value) {
        Value v(value.c_str(), doc_.GetAllocator());
        if (doc_.HasMember(key)) {
            doc_[key].SetString(v.GetString(), doc_.GetAllocator());
        }
        else {
            doc_.AddMember(Value(key, doc_.GetAllocator()).Move(), v, doc_.GetAllocator());
        }
    }
    template<>
    void set(const char* key, int value) {
        Value v(value);
        if (doc_.HasMember(key)) {
            doc_[key].SetInt(value);
        }
        else {
            doc_.AddMember(Value(key, doc_.GetAllocator()).Move(), v, doc_.GetAllocator());
        }
    }

    template<typename T>
    void setDefaultValue(const char* key, T value) {
        defaultValues_[key] = value;
    }

private:
    std::wstring filename_;
    Document doc_;
};