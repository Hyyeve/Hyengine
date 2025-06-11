#pragma once
#include <string>
#include <vector>

namespace hyengine::common::file_io {

    std::string read_asset_text(const std::string& id);
    std::string read_raw_asset_text(const std::string& id);
    std::string read_preprocessed_asset_text(const std::string& id);
    std::vector<unsigned char> read_raw_asset_bytes(const std::string& id);

    bool save_raw_asset(const std::string& id, const std::vector<unsigned char>& data);
    bool save_asset_text(const std::string& id, const std::string_view& text);

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
