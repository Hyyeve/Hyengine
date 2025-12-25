#include "file_io.hpp"

#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <tracy/Tracy.hpp>

#include "logger.hpp"
#include "../common/common.hpp"
#include "stblib/include/stb_image.hpp"

/*
 ASSET ID STRUCTURE
 Needs to include:
 - Prefix path
 - Type
 - Name

 Prefix path only changes the starting directory
 Type is used as both part of the path AND to identify the file extension
 Name is used to identify the filename

 prefix.type.name format - corresponds well with actual full path

 sub-types and prefix depth pose issues

 disallow subtypes? use last part of prefix path only
 so, would become

 folder1.folder2.type.filename

 if we drop the implicit type detection we could have:
 hyengine.font.image.buycat.png

 but i like the implicit detection, so
 hyengine.font.image.buycat
                ^^^ implies png

 */
namespace hyengine
{
    static std::string root_directory = "assets";
    static std::string override_directory;

    bool should_preprocess(const std::string_view& type)
    {
        switch (string_hash(type))
        {
            case string_hash("shader"): return true;
            default: return false;
        }
    }

    std::regex directive_data_pattern(const std::string_view& directive)
    {
        const std::string pattern = stringify("<", directive, "=([^>]+)>");
        return std::regex(pattern);
    }

    std::regex directive_line_pattern(const std::string_view& directive)
    {
        const std::string pattern = stringify("\\n?.*<", directive, "=.+>\\n?");
        return std::regex(pattern);
    }

    void set_primary_asset_directory(const std::string_view& directory)
    {
        root_directory = directory;
    }

    void set_override_asset_directory(const std::string_view& directory)
    {
        override_directory = directory;
    }

    std::string_view get_primary_asset_directory()
    {
        return root_directory;
    }

    std::string_view get_override_asset_directory()
    {
        return override_directory.empty() ? root_directory : override_directory;
    }

    std::filesystem::path get_asset_path(const std::string_view& asset_id)
    {
        std::string relative_path = std::string(asset_id);
        string_replace(relative_path, '.', std::filesystem::path::preferred_separator);
        const std::string asset_type = get_asset_type(asset_id);
        const std::string_view extension = get_asset_extension(asset_type);

        std::filesystem::path override_path = std::filesystem::path(get_override_asset_directory()).append(relative_path).replace_extension(extension);
        if (std::filesystem::exists(override_path)) return override_path;

        std::filesystem::path primary_path = std::filesystem::path(get_primary_asset_directory()).append(relative_path).replace_extension(extension);
        return primary_path;
    }

    std::filesystem::path get_asset_directory(const std::string_view& asset_id)
    {
        return get_asset_path(asset_id).parent_path();
    }

    std::string get_asset_name(const std::string_view& asset_id)
    {
        return std::string(asset_id).substr(asset_id.find_last_of('.') + 1);
    }

    std::string get_asset_type(const std::string_view& asset_id)
    {
        std::string relative_path = std::string(asset_id);
        string_replace(relative_path, '.', std::filesystem::path::preferred_separator);
        const std::filesystem::path path = std::filesystem::path(relative_path);
        return path.parent_path().filename().string();
    }

    std::string_view get_asset_extension(const std::string_view& type)
    {
        switch (string_hash(type))
        {
            case string_hash("png"):
            case string_hash("image"): return "png";
            case string_hash("shader"): return "glsl";
            case string_hash("bin"): return "bin";
            case string_hash("meta"): return "csv";
            default: return "hy";
        }
    }

    bool asset_exists(const std::string_view& asset_id)
    {
        return std::filesystem::exists(get_asset_path(asset_id));
    }

    bool is_asset_overriden(const std::string_view& asset_id)
    {
        const std::string asset_root = get_asset_path(asset_id).begin()->string();
        const std::string_view expected_root = get_primary_asset_directory();
        return asset_root != expected_root;
    }

    void delete_asset(const std::string_view& asset_id)
    {
        ZoneScoped;
        const std::filesystem::path path = get_asset_path(asset_id);
        if (!std::filesystem::exists(path))
        {
            log_warn(logger_tags::FILEIO, "Couldn't delete asset at \'", path.string(), "\' - doesn't exist.");
            return;
        }

        log_info(logger_tags::FILEIO, "Deleting asset at \'", path.string(), "\'");
        std::filesystem::remove_all(path);
    }

    void delete_asset_directory(const std::string_view& asset_id)
    {
        ZoneScoped;
        const std::filesystem::path directory = get_asset_directory(asset_id);
        if (!std::filesystem::exists(directory))
        {
            log_warn(logger_tags::FILEIO, "Couldn't delete directory at \'", directory.string(), "\' - doesn't exist.");
            return;
        }

        log_info(logger_tags::FILEIO, "Deleting directory at \'", directory.string(), "\'");
        std::filesystem::remove_all(directory);
    }

    std::string load_asset_text(const std::string_view& id)
    {
        if (should_preprocess(get_asset_type(id)))
        {
            return load_preprocessed_asset_text(id);
        }

        return load_raw_asset_text(id);
    }

    std::string load_raw_asset_text(const std::string_view& id)
    {
        ZoneScoped;
        if (!asset_exists(id))
        {
            log_error(logger_tags::FILEIO, "Could not read asset \'", id, "\' !");
            return "";
        }

        log_debug(logger_tags::FILEIO, "Loading asset \'", id, "\'");

        const std::filesystem::path path = get_asset_path(id);

        if (is_asset_overriden(id))
        {
            log_debug(logger_tags::FILEIO, "Asset is overriden to '", path.string(), "'");
        }

        std::ifstream file(path, std::ios::in);

        if (!file.is_open() || file.bad())
        {
            log_error(logger_tags::FILEIO, "Could not read asset \'", id, "\' !");
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }

    std::string load_preprocessed_asset_text(const std::string_view& id)
    {
        ZoneScoped;
        const std::string text = load_raw_asset_text(id);
        return inject_text_includes(text);
    }

    std::vector<u8> load_asset_bytes(const std::string_view& id)
    {
        ZoneScoped;
        if (!asset_exists(id))
        {
            log_error(logger_tags::FILEIO, "Could not read asset \'", id, "\' !");
            return {};
        }

        log_debug(logger_tags::FILEIO, "Loading asset \'", id, "\'");

        const std::filesystem::path path = get_asset_path(id);

        if (is_asset_overriden(id))
        {
            log_debug(logger_tags::FILEIO, "Asset is overriden to '", path.string(), "'");
        }

        std::ifstream file(path, std::ios::binary | std::ios::in);

        if (!file.is_open() || file.bad())
        {
            log_error(logger_tags::FILEIO, "Couldn't load asset \'", id, "\' - bad file");
            return {};
        }

        file.unsetf(std::ios::skipws);

        file.seekg(0, std::ios::end);
        const std::streampos size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<u8> data;
        data.reserve(size);
        data.insert(data.begin(), std::istream_iterator<u8>(file), std::istream_iterator<u8>());

        file.close();

        return data;
    }


    asset_image_data load_asset_image(const std::string_view& id)
    {
        ZoneScoped;
        if (!asset_exists(id))
        {
            log_error(logger_tags::FILEIO, "Could not read asset \'", id, "\' !");
            return {nullptr, 0, 0, 0};
        }

        const std::filesystem::path path = get_asset_path(id);

        if (is_asset_overriden(id))
        {
            log_debug(logger_tags::FILEIO, "Asset is overriden to '", path.string(), "'");
        }

        log_debug(logger_tags::FILEIO, "Loading asset \'", id, "\'");

        asset_image_data result {};
        i32 temp_width;
        i32 temp_height;
        i32 temp_channels;

        stbi_set_unpremultiply_on_load(true);
        stbi_set_flip_vertically_on_load(true);
        result.data = stbi_load(path.string().c_str(), &temp_width, &temp_height, &temp_channels, 0);
        if (result.data == nullptr)
        {
            log_error(logger_tags::FILEIO, "Couldn't load asset \'", id, "\' - bad file");
            return {nullptr, 0, 0, 0};
        }

        result.width = temp_width;
        result.height = temp_height;
        result.num_channels = temp_channels;
        return result;
    }

    bool save_raw_asset(const std::string_view& id, const u8* data, const u32 size)
    {
        ZoneScoped;
        const std::filesystem::path directory = get_asset_directory(id);
        const std::filesystem::path path = get_asset_path(id);

        if (!std::filesystem::exists(directory) && !std::filesystem::create_directories(directory))
        {
            log_error(logger_tags::FILEIO, "Couldn't create directory \'", directory.string(), "\'");
            return false;
        }

        std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!file.is_open() || file.bad())
        {
            log_error(logger_tags::FILEIO, "Couldn't save asset \'", id, "\' - bad file");
            return false;
        }

        log_debug(logger_tags::FILEIO, "Saving asset \'", id, "\'");

        file.write(reinterpret_cast<const char8*>(data), size);
        file.close();

        return true;
    }

    bool save_asset_text(const std::string_view& id, const std::string_view& text)
    {
        ZoneScoped;
        const std::filesystem::path directory = get_asset_directory(id);
        const std::filesystem::path path = get_asset_path(id);

        if (!std::filesystem::create_directories(directory))
        {
            log_error(logger_tags::FILEIO, "Couldn't create directory \'", directory, "\'");
            return false;
        }

        std::ofstream file(path, std::ios::out | std::ios::trunc);
        if (!file.is_open() || file.bad())
        {
            log_error(logger_tags::FILEIO, "Couldn't save asset \'", id, "\' - bad file");
            return false;
        }

        log_debug(logger_tags::FILEIO, "Saving asset \'", id, "\'");

        file.write(text.data(), text.size());
        file.close();

        return true;
    }

    std::string inject_text_includes(const std::string_view& text)
    {
        ZoneScoped;
        std::smatch include_match;
        std::string new_text(text);

        for (i32 i = 0; i < std::numeric_limits<char>::max(); i++)
        {
            std::string include_asset = find_directive(new_text, "include");
            if (!include_asset.empty())
            {
                std::string asset_text = load_raw_asset_text(include_asset);
                replace_directive(new_text, "include", asset_text);
                continue;
            }

            break;
        }

        return new_text;
    }

    bool has_directive(const std::string_view& text, const std::string_view& directive)
    {
        return !find_directive(text, directive).empty();
    }

    std::string find_directive(const std::string_view& text, const std::string_view& directive)
    {
        ZoneScoped;
        std::smatch regex_match;
        const std::string search_text = std::string(text);
        if (std::regex_search(search_text.cbegin(), search_text.cend(), regex_match, directive_data_pattern(directive)))
        {
            return regex_match.str(1);
        }

        return "";
    }

    void replace_directive(std::string& text, const std::string_view& directive, const std::string_view& replacement)
    {
        ZoneScoped;
        std::smatch regex_match;
        if (std::regex_search(text.cbegin(), text.cend(), regex_match, directive_line_pattern(directive)))
        {
            text.erase(regex_match.position(), regex_match.length());
            text.insert(regex_match.position(), replacement);
            text.insert(regex_match.position(), "\n");
        }
    }
}
