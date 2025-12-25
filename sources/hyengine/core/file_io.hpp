#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include "hyengine/common/sized_numerics.hpp"




namespace hyengine
{
    struct asset_image_data
    {
        u8* data; //RGBA ordered, with the number of channels included as specified below. 8-bit channels
        u32 width;
        u32 height;
        u32 num_channels;
    };

    void set_primary_asset_directory(const std::string_view& directory);
    void set_override_asset_directory(const std::string_view& directory);

    [[nodiscard]] std::string_view get_primary_asset_directory();
    [[nodiscard]] std::string_view get_override_asset_directory();

    [[nodiscard]] std::filesystem::path get_asset_path(const std::string_view& asset_id);
    [[nodiscard]] std::filesystem::path get_asset_directory(const std::string_view& asset_id);
    [[nodiscard]] std::string get_asset_name(const std::string_view& asset_id);
    [[nodiscard]] std::string get_asset_type(const std::string_view& asset_id);
    [[nodiscard]] std::string_view get_asset_extension(const std::string_view& type);

    [[nodiscard]] bool asset_exists(const std::string_view& asset_id);
    [[nodiscard]] bool is_asset_overriden(const std::string_view& asset_id);

    void delete_asset(const std::string_view& asset_id);
    void delete_asset_directory(const std::string_view& asset_id);

    [[nodiscard]] std::string load_asset_text(const std::string_view& id);
    [[nodiscard]] std::string load_raw_asset_text(const std::string_view& id);
    [[nodiscard]] std::string load_preprocessed_asset_text(const std::string_view& id);
    [[nodiscard]] std::vector<u8> load_asset_bytes(const std::string_view& id);

    [[nodiscard]] asset_image_data load_asset_image(const std::string_view& id);

    template <typename data>
    [[nodiscard]] data load_asset_struct(const std::string_view& id)
    {
        const std::vector<u8> bytes = load_asset_bytes(id);
        data result;
        memcpy(&result, bytes.data(), sizeof(data));
        return result;
    }

    [[nodiscard]] bool save_raw_asset(const std::string_view& id, const u8* data, const u32 size);
    [[nodiscard]] bool save_asset_text(const std::string_view& id, const std::string_view& text);

    template <typename data>
    [[nodiscard]] bool save_asset_struct(const std::string_view& id, const data& value)
    {
        return save_raw_asset(id, &value, sizeof(data));
    }


    [[nodiscard]] std::string inject_text_includes(const std::string_view& text);



    [[nodiscard]] bool has_directive(const std::string_view& text, const std::string_view& directive);
    [[nodiscard]] std::string find_directive(const std::string_view& text, const std::string_view& directive);
    void replace_directive(std::string& text, const std::string_view& directive, const std::string_view& replacement);
}
