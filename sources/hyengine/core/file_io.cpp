#include "file_io.hpp"

#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <tracy/Tracy.hpp>

#include "logger.hpp"
#include "../common/common.hpp"
#include "stblib/include/stb_image.hpp"

namespace hyengine
{
    constexpr std::string_view ROOT_DIRECTORY = "assets";
    constexpr std::string_view LOGGER_TAG = "FileIO";

    std::string asset_id_to_relative_path(const std::string_view& asset_id)
    {
        std::string result = std::string(asset_id);
        string_replace(result, ':', std::filesystem::path::preferred_separator);
        string_replace(result, '.', std::filesystem::path::preferred_separator);
        return result;
    }

    std::string get_asset_type(const std::string_view& asset_id)
    {
        return std::string(asset_id).substr(0, asset_id.find(':'));
    }

    std::string get_asset_name(const std::string_view& asset_id)
    {
        return std::string(asset_id).substr(asset_id.find(':') + 1);
    }

    std::string_view get_asset_extension(const std::string_view& type)
    {
        switch (string_hash(type))
        {
            case string_hash("png"): return "png";
            case string_hash("shader"): return "glsl";
            case string_hash("shader.bin"): return "bin";
            case string_hash("font.image"): return "png";
            case string_hash("font.meta"): return "csv";
            default: return "hy";
        }
    }

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

    constexpr std::string_view get_primary_asset_directory()
    {
        return ROOT_DIRECTORY;
    }

    constexpr std::string_view get_fallback_asset_directory()
    {
        return ROOT_DIRECTORY;
    }

    std::filesystem::path get_asset_path(const std::string_view& asset_id)
    {
        const std::string relative_path = asset_id_to_relative_path(asset_id);
        return std::filesystem::path(get_primary_asset_directory()).append(relative_path).replace_extension(get_asset_extension(get_asset_type(asset_id)));
    }

    std::filesystem::path get_asset_directory(const std::string_view& asset_id)
    {
        const std::string relative_path = asset_id_to_relative_path(asset_id);
        return std::filesystem::path(get_primary_asset_directory()).append(relative_path.substr(0, relative_path.find_last_of(std::filesystem::path::preferred_separator)));
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
            log_error(LOGGER_TAG, "Could not read asset \'", id, "\' !");
            return "";
        }

        log_info(LOGGER_TAG, "Loading asset \'", id, "\'");

        const std::filesystem::path path = get_asset_path(id);
        std::ifstream file(path, std::ios::in);

        if (!file.is_open() || file.bad())
        {
            log_error(LOGGER_TAG, "Could not read asset \'", id, "\' !");
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
            log_error(LOGGER_TAG, "Could not read asset \'", id, "\' !");
            return {};
        }

        log_info(LOGGER_TAG, "Loading asset \'", id, "\'");

        const std::filesystem::path path = get_asset_path(id);
        std::ifstream file(path, std::ios::binary | std::ios::in);

        if (!file.is_open() || file.bad())
        {
            log_error(LOGGER_TAG, "Couldn't load asset \'", id, "\' - bad file");
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
            log_error(LOGGER_TAG, "Could not read asset \'", id, "\' !");
            return {nullptr, 0, 0, 0};
        }

        asset_image_data result {};
        i32 temp_width;
        i32 temp_height;
        i32 temp_channels;

        stbi_set_unpremultiply_on_load(true);
        stbi_set_flip_vertically_on_load(true);
        result.data = stbi_load(get_asset_path(id).string().c_str(), &temp_width, &temp_height, &temp_channels, 0);
        if (result.data == nullptr)
        {
            log_error(LOGGER_TAG, "Couldn't load asset \'", id, "\' - bad file");
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
            log_error(LOGGER_TAG, "Couldn't create directory \'", directory.string(), "\'");
            return false;
        }

        std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!file.is_open() || file.bad())
        {
            log_error(LOGGER_TAG, "Couldn't save asset \'", id, "\' - bad file");
            return false;
        }

        log_info(LOGGER_TAG, "Saving asset \'", id, "\'");

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
            log_error(LOGGER_TAG, "Couldn't create directory \'", directory, "\'");
            return false;
        }

        std::ofstream file(path, std::ios::out | std::ios::trunc);
        if (!file.is_open() || file.bad())
        {
            log_error(LOGGER_TAG, "Couldn't save asset \'", id, "\' - bad file");
            return false;
        }

        log_info(LOGGER_TAG, "Saving asset \'", id, "\'");

        file.write(text.data(), text.size());
        file.close();

        return true;
    }

    void delete_asset(const std::string_view& asset_id)
    {
        ZoneScoped;
        const std::filesystem::path path = get_asset_path(asset_id);
        if (!std::filesystem::exists(path))
        {
            log_warn(LOGGER_TAG, "Couldn't delete asset at \'", path.string(), "\' - doesn't exist.");
            return;
        }

        log_info(LOGGER_TAG, "Deleting asset at \'", path.string(), "\'");
        std::filesystem::remove_all(path);
    }


    void delete_asset_directory(const std::string_view& asset_id)
    {
        ZoneScoped;
        const std::filesystem::path directory = get_asset_directory(asset_id);
        if (!std::filesystem::exists(directory))
        {
            log_warn(LOGGER_TAG, "Couldn't delete directory at \'", directory.string(), "\' - doesn't exist.");
            return;
        }

        log_info(LOGGER_TAG, "Deleting directory at \'", directory.string(), "\'");
        std::filesystem::remove_all(directory);
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

    bool asset_exists(const std::string_view& asset_id)
    {
        return std::filesystem::exists(get_asset_path(asset_id));
    }
}
