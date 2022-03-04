// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// Helper functions

#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include <util/Logger.h>

#include "il2cpp-metadata-version.h"

#define GetSingleton(tpname) reinterpret_cast<app:: ## tpname ## *>(app::Singleton_GetInstance(nullptr, *app::Singleton_1_ ## tpname ## __get_Instance__MethodInfo))

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
    auto size = static_cast<size_t>(size_s);
    auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

template<typename ElementT>
struct UniArray {
    void* klass;
    MonitorData* monitor;
    Il2CppArrayBounds* bounds;
    il2cpp_array_size_t max_length;
    ElementT vector[32];
};

template<typename KeyT, typename ValT>
struct __declspec(align(8)) UniDict {
    void* klass;
    MonitorData* monitor;
    struct app::Int32__Array* table;
    struct app::Link__Array* linkSlots;
    struct UniArray<KeyT>* keySlots;
    struct UniArray<ValT>* valueSlots;
    int32_t touchedSlots;
    int32_t emptySlot;
    int32_t count;
    int32_t threshold;
    void* hcp;
    void* serialization_info;
    int32_t generation;
};


template<typename KeyT, typename ValT>
std::vector<std::pair<KeyT, ValT>> getUniDictPairs(UniDict<KeyT, ValT>* dictionary) {
    auto pairs = std::vector<std::pair<KeyT, ValT>>();

#define DictCheckNull(field, msg) if (field == nullptr) { LOG_WARNING("Failed to get dict pairs: %s", msg); return pairs; }

    DictCheckNull(dictionary, "Dict pointer is null");
    DictCheckNull(dictionary->linkSlots, "LinkSlots pointer is null.");
    DictCheckNull(dictionary->keySlots, "Key slots is null.");
    DictCheckNull(dictionary->valueSlots, "ValueSlots pointer is null.");

#undef DictCheckNull

    size_t next = 0;
    const int HASH_FLAG = 0x80000000;
    while (next < dictionary->touchedSlots)
    {
        int cur = next++;
        if ((dictionary->linkSlots->vector[cur].HashCode & HASH_FLAG) != 0)
        {
            pairs.push_back(
                std::make_pair(
                    dictionary->keySlots->vector[cur],
                    dictionary->valueSlots->vector[cur]
                )
            );
        }
    }
    return pairs;
}

// Helper function to get the module base address
uintptr_t il2cppi_get_base_address();

// Helpre function to get the UnityPlayer.dll base address
uintptr_t il2cppi_get_unity_address();

// Helper function to open a new console window and redirect stdout there
void il2cppi_new_console();
void il2cppi_close_console();

#if _MSC_VER >= 1920
// Helper function to convert Il2CppString to std::string
std::string il2cppi_to_string(Il2CppString* str);

// Helper function to convert System.String to std::string
std::string il2cppi_to_string(app::String* str);

std::string il2cppi_to_string(app::Vector vec);

std::string il2cppi_to_string(app::Vector2 vec);

std::string il2cppi_to_string(app::Vector3 vec);
#endif

// Helper function to check if a metadata usage pointer is initialized
template<typename T> bool il2cppi_is_initialized(T* metadataItem) {
#if __IL2CPP_METADATA_VERISON < 270
    return *metadataItem != 0;
#else
    // Metadata >=27 (Unity 2020.2)
    return !((uintptr_t) *metadataItem & 1);
#endif
}

// Helper function to convert a pointer to hex
template<typename T> std::string to_hex_string(T i) {
    std::stringstream stream;
    stream << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << i;
    return stream.str();
}