#pragma once
#include <string>
#include <vector>

#include "Hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    struct asset_image_data
    {
        u8* data; //RGBA ordered, with the number of channels included as specified below. 8-bit channels
        u32 width;
        u32 height;
        u32 num_channels;
    };

    std::string load_asset_text(const std::string_view& id);
    std::string load_raw_asset_text(const std::string_view& id);
    std::string load_preprocessed_asset_text(const std::string_view& id);
    std::vector<u8> load_asset_bytes(const std::string_view& id);

    asset_image_data load_asset_image(const std::string_view& id);

    template <typename data>
    data load_asset_struct(const std::string_view& id)
    {
        const std::vector<u8> bytes = load_asset_bytes(id);
        data result;
        memcpy(&result, bytes.data(), sizeof(data));
        return result;
    }

    bool save_raw_asset(const std::string_view& id, const u8* data, const u32 size);
    bool save_asset_text(const std::string_view& id, const std::string_view& text);

    template <typename data>
    void save_asset_struct(const std::string_view& id, const data& value)
    {
        save_raw_asset(id, &value, sizeof(data));
    }


    void delete_asset(const std::string_view& asset_id);
    void delete_asset_directory(const std::string_view& asset_id);

    std::string inject_text_includes(const std::string_view& text);
    std::string asset_id_to_relative_path(const std::string_view& asset_id);

    std::string get_asset_type(const std::string_view& asset_id);
    std::string get_asset_name(const std::string_view& asset_id);
    std::string_view get_asset_extension(const std::string_view& type);

    std::string find_directive(const std::string_view& text, const std::string_view& directive);
    void replace_directive(std::string& text, const std::string_view& directive, const std::string_view& replacement);
    bool asset_exists(const std::string_view& asset_id);
}
