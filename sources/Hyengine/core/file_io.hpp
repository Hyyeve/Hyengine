#pragma once
#include <string>
#include <vector>

#include "Hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    struct asset_image_data
    {
        unsigned char* data; //RGBA ordered, with the number of channels included as specified below. 8-bit channels
        u32 width;
        u32 height;
        u32 num_channels;
    };

    std::string load_asset_text(const std::string& id);
    std::string load_raw_asset_text(const std::string& id);
    std::string load_preprocessed_asset_text(const std::string& id);
    std::vector<unsigned char> load_asset_bytes(const std::string& id);

    asset_image_data load_asset_image(const std::string& id);

    template <typename data>
    data load_asset_struct(const std::string& id)
    {
        const std::vector<unsigned char> bytes = load_asset_bytes(id);
        data result;
        memcpy(&result, bytes.data(), sizeof(data));
        return result;
    }

    bool save_raw_asset(const std::string& id, const std::vector<unsigned char>& data);
    bool save_asset_text(const std::string& id, const std::string_view& text);

    template <typename data>
    void save_asset_struct(const std::string& id, const data& value)
    {
        std::vector<unsigned char> bytes;
        bytes.resize(sizeof(data));
        memcpy(bytes.data(), &value, sizeof(data));
        save_raw_asset(id, bytes);
    }


    void delete_asset(const std::string& asset_id);
    void delete_asset_directory(const std::string& asset_id);

    std::string inject_text_includes(const std::string_view text);
    std::string asset_id_to_relative_path(std::string asset_id);

    std::string get_asset_type(const std::string& asset_id);
    std::string get_asset_name(const std::string& asset_id);
    std::string_view get_asset_extension(const std::string_view type);

    std::string find_directive(const std::string& text, const std::string& directive);
    void replace_directive(std::string& text, const std::string& directive, const std::string& replacement);
    bool asset_exists(const std::string& asset_id);
}
