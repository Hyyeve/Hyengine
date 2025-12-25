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

    ///Sets the primary directory (relative path) that the engine will look for assets in. Defaults to 'assets'
    void set_primary_asset_directory(const std::string_view& directory);

    ///Sets an override directory (relative path) that the engine will look for assets in. Defaults to empty (will fallback to primary directory)
    ///If an asset is found in the override directory, it will be preferred. If it is not, the engine will fall back to the primary directory to find it.
    void set_override_asset_directory(const std::string_view& directory);

    [[nodiscard]] std::string_view get_primary_asset_directory();
    [[nodiscard]] std::string_view get_override_asset_directory();

    ///Locates an asset based on ID. IDs are in the form ....'folder.type.name' where there may be any number of folders prior to the type and name.
    ///The type is ALSO an additional folder in the path, which is used to infer the asset type. For instance, 'assets.scene.shader.tree' is
    ///expected to be in the directory 'assets/scene/shader/' and will be inferred to be called 'tree.glsl' based on the type (last directory name) being 'shader'
    [[nodiscard]] std::filesystem::path get_asset_path(const std::string_view& asset_id);

    ///Locates an asset directory based on ID. See get_asset_path for full asset ID details.
    [[nodiscard]] std::filesystem::path get_asset_directory(const std::string_view& asset_id);

    ///Gets the name of an asset based on the ID. For instance, 'assets.scene.shader.tree' => 'tree'
    [[nodiscard]] std::string get_asset_name(const std::string_view& asset_id);

    ///Gets the type (last directory name) of an asset based on the ID. For instance, 'assets.scene.shader.tree' => 'shader'
    [[nodiscard]] std::string get_asset_type(const std::string_view& asset_id);

    ///Gets the file extension of an asset based on the type. For instance, 'shader' => 'glsl'
    [[nodiscard]] std::string_view get_asset_extension(const std::string_view& type);

    [[nodiscard]] bool asset_exists(const std::string_view& asset_id);

    ///Checks if an asset is being sourced from the override directory or primary directory.
    [[nodiscard]] bool is_asset_overriden(const std::string_view& asset_id);

    void delete_asset(const std::string_view& asset_id);
    void delete_asset_directory(const std::string_view& asset_id);

    //Loads an asset as text without doing any directive processing.
    [[nodiscard]] std::string load_asset_text_raw(const std::string_view& id);

    ///Loads an asset as text, and runs include processing on it (automatic replacement of <include=assetid> with the contents of assetid. Note that the FULL LINE containing the directive is overwritten)
    [[nodiscard]] std::string load_asset_text(const std::string_view& id);

    ///Loads an asset as bytes
    [[nodiscard]] std::vector<u8> load_asset_bytes(const std::string_view& id);

    ///Loads an image asset
    [[nodiscard]] asset_image_data load_asset_image(const std::string_view& id);

    ///Loads an asset as bytes at converts it to the given data type. Returns default_result if the loaded amount of bytes does not match the data type size.
    template <typename data>
    [[nodiscard]] data load_asset_struct(const std::string_view& id, const data default_result)
    {
        const std::vector<u8> bytes = load_asset_bytes(id);
        data result;
        if (bytes.size() != sizeof(data)) return default_result;
        memcpy(&result, bytes.data(), sizeof(data));
        return result;
    }

    ///Saves raw bytes to an asset file.
    [[nodiscard]] bool save_raw_asset(const std::string_view& id, const u8* data, const u32 size);

    ///Saves text to an asset file.
    [[nodiscard]] bool save_asset_text(const std::string_view& id, const std::string_view& text);

    ///Saves the bytes of a data type to an asset file.
    template <typename data>
    [[nodiscard]] bool save_asset_struct(const std::string_view& id, const data& value)
    {
        return save_raw_asset(id, &value, sizeof(data));
    }

    ///Performs automatic replacement of <include=assetid> with the contents of assetid. Note that the FULL LINE containing the directive is overwritten)
    [[nodiscard]] std::string inject_text_includes(const std::string_view& text);

    ///Checks for a directive of the form <directive=var> in the text
    [[nodiscard]] bool has_directive(const std::string_view& text, const std::string_view& directive);

    ///Checks for a directive of the form <directive=var> in the text, and returns var if it exists.
    [[nodiscard]] std::string find_directive(const std::string_view& text, const std::string_view& directive);

    ///Replaces a directive of the form <directive=var> with the given replacement. Note that the FULL LINE containing the directive is overwritten.
    void replace_directive(std::string& text, const std::string_view& directive, const std::string_view& replacement);
}
