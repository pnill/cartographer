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
                const Value& v = defaultValues_[key];
                if constexpr (std::is_same_v<T, bool>) {
                    return v.GetBool();
                }
                else if constexpr (std::is_same_v<T, int>) {
                    return v.GetInt();
                }
                else if constexpr (std::is_same_v<T, unsigned>) {
                    return v.GetUint();
                }
                else if constexpr (std::is_same_v < T, float>) {
                    return v.GetFloat();
                }
                else if constexpr (std::is_same_v<T, std::string>) {
                    return v.GetString();
                }
                else {
                    // Unsupported type
                    static_assert(sizeof(T) == 0, "Unsupported type in json_config::get()");
                }
            }
            return defaultValue;
        }

        const Value& v = doc_[key];
        // Use if constexpr to conditionally compile code based on T
        if constexpr (std::is_same_v<T, bool>) {
            return v.GetBool();
        }
        else if constexpr (std::is_same_v<T, int>) {
            return v.GetInt();
        }
        else if constexpr (std::is_same_v<T, unsigned>) {
            return v.GetUint();
        }
        else if constexpr (std::is_same_v < T, float> ) {
            return v.GetFloat();
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return v.GetString();
        }
        else {
            // Unsupported type
            static_assert(sizeof(T) == 0, "Unsupported type in json_config::get()");
        }
    }

    template<typename T>
    void set(const char* key, T value) {
        // Check if the key already exists in the document
        if (doc_.HasMember(key)) {
            if constexpr (std::is_same_v<T, bool>) {
                doc_[key].SetBool(value);
            }
            else if constexpr (std::is_same_v<T, int>) {
                doc_[key].SetInt(value);
            }
            else if constexpr (std::is_same_v<T, unsigned>) {
                doc_[key].SetUint(value);
            }
            else if constexpr (std::is_same_v<T, float>) {
                doc_[key].SetFloat(value);
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                //RapidJson handles std string strangely..
                doc_.RemoveMember(key);
                Value k(key, doc_.GetAllocator());
                Value v(value.c_str(), value.size(), doc_.GetAllocator());
                doc_.AddMember(k, v, doc_.GetAllocator());
            }
            else if constexpr(std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
                //This is 100% not the right way but I cannot figure out why I can't do it the correct way
            	//doc_[key].SetString(value, doc_.GetAllocator());


            	doc_.RemoveMember(key);
                Value k(key, doc_.GetAllocator());
                std::string temp(value);
                Value v(temp.c_str(), temp.size(), doc_.GetAllocator());
                doc_.AddMember(k, v, doc_.GetAllocator());
            }
            else {
                // Unsupported type
                static_assert(sizeof(T) == 0, "Unsupported type in json_config::set()");
            }
        }
        else {
            // If the key does not exist in the document, add a new key-value pair to the document
            Value k(key, doc_.GetAllocator());
            if constexpr (std::is_same_v<T, bool>) {
                doc_.AddMember(k, value, doc_.GetAllocator());
            }
            else if constexpr (std::is_same_v<T, int>) {
                doc_.AddMember(k, value, doc_.GetAllocator());
            }
            else if constexpr (std::is_same_v<T, unsigned>) {
                doc_.AddMember(k, value, doc_.GetAllocator());
            }
            else if constexpr (std::is_same_v<T, float>) {
                doc_.AddMember(k, value, doc_.GetAllocator());
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                Value v(value.c_str(), value.size(), doc_.GetAllocator());
                doc_.AddMember(k, v, doc_.GetAllocator());
            }
            else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
                //This is 100% not the right way but I cannot figure out why I can't do it the correct way
                //doc_.AddMember(k, value, doc_.GetAllocator());
                //Should work but it doesn't...

                std::string temp(value);
                Value v(temp.c_str(), temp.size(), doc_.GetAllocator());
                doc_.AddMember(k, v, doc_.GetAllocator());
            }
            else {
                // Unsupported type
                static_assert(sizeof(T) == 0, "Unsupported type in json_config::set()");
            }
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