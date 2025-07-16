#include "file_io.hpp"

#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <tracy/Tracy.hpp>

#include "../core/logger.hpp"
#include "portability.hpp"
#include "stblib/include/stb_image.hpp"

namespace hyengine::common::file_io {

   constexpr std::string_view raw_directory = "assets";
   constexpr std::string_view logger_tag = "FileIO";

   std::string asset_id_to_relative_path(std::string asset_id)
   {
      ZoneScoped;
      common::portability::string_replace(asset_id, ':', std::filesystem::path::preferred_separator);
      common::portability::string_replace(asset_id, '.', std::filesystem::path::preferred_separator);
      return asset_id;
   }

   std::string get_asset_type(const std::string& asset_id)
   {
      ZoneScoped;
      return asset_id.substr(0, asset_id.find(':'));
   }

   std::string get_asset_name(const std::string& asset_id)
   {
      ZoneScoped;
      return asset_id.substr(asset_id.find(':') + 1);
   }

   std::string_view get_asset_extension(const std::string_view type)
   {
      ZoneScoped;
      if (type == "shader")  return "glsl";
      if (type == "shader.bin") return "bin";
      if (type == "png") return "png";
      return "hy";
   }

   bool should_preprocess(const std::string_view type)
   {
      ZoneScoped;
      if (type == "shader")  return true;
      return false;
   }

   std::regex directive_data_pattern(const std::string& directive)
   {
      ZoneScoped;
      const std::string pattern = "<" + directive + "=([^>]+)>";
      return std::regex(pattern);
   }

   std::regex directive_line_pattern(const std::string& directive)
   {
      ZoneScoped;
      const std::string pattern ="\\n?.*<" + directive + "=.+>\\n?";
      return std::regex(pattern);
   }

   constexpr std::string_view get_primary_asset_directory()
   {
      return raw_directory;
   }

   constexpr std::string_view get_fallback_asset_directory()
   {
      return raw_directory;
   }

   std::filesystem::path get_asset_path(const std::string& asset_id)
   {
      ZoneScoped;
      const std::string relative_path = asset_id_to_relative_path(asset_id);
      return std::filesystem::path(get_primary_asset_directory()).append(relative_path).replace_extension(get_asset_extension(get_asset_type(asset_id)));
   }

   std::filesystem::path get_asset_directory(const std::string& asset_id)
   {
      ZoneScoped;
      const std::string relative_path = asset_id_to_relative_path(get_asset_type(asset_id));
      return std::filesystem::path(get_primary_asset_directory()).append(relative_path);
   }

   std::string read_raw_asset_text(const std::string& id)
   {
      ZoneScoped;
      if (!asset_exists(id))
      {
         logger::message_error(logger::format("Could not read asset \'", id, "\' !"), logger_tag);
         return "";
      }

      logger::message_info(logger::format("Loading asset \'", id, "\'"), logger_tag);

      const std::filesystem::path path = get_asset_path(id);
      std::ifstream file(path, std::ios::in);

      if (!file.is_open() || file.bad())
      {
         logger::error(logger_tag, "Couldn't load asset - bad file");
         return "";
      }

      std::stringstream buffer;
      buffer << file.rdbuf();
      file.close();
      return buffer.str();
   }

   std::string find_directive(const std::string& text, const std::string& directive)
   {
      ZoneScoped;
      std::smatch regex_match;
      if (std::regex_search(text.cbegin(), text.cend(), regex_match, directive_data_pattern(directive)))
      {
         return regex_match.str(1);
      }

      return "";
   }

   void replace_directive(std::string& text, const std::string& directive, const std::string& replacement)
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

   bool asset_exists(const std::string& asset_id)
   {
      ZoneScoped;
      return std::filesystem::exists(get_asset_path(asset_id));
   }

   std::string inject_text_includes(const std::string_view text)
   {
      ZoneScoped;
      std::smatch include_match;
      std::string new_text(text);

      for (int i = 0; i < std::numeric_limits<char>::max(); i++)
      {
         std::string include_asset = find_directive(new_text, "include");
         if (!include_asset.empty())
         {
            std::string asset_text = read_raw_asset_text(include_asset);
            replace_directive(new_text, "include", asset_text);
            continue;
         }

         break;
      }

      return new_text;
   }

   std::string read_preprocessed_asset_text(const std::string& id)
   {
      ZoneScoped;
      const std::string text = read_raw_asset_text(id);
      return inject_text_includes(text);
   }

   std::vector<unsigned char> read_asset_bytes(const std::string& id)
   {
      ZoneScoped;
      if (!asset_exists(id))
      {
         logger::message_error(logger::format("Could not read asset \'", id, "\' !"), logger_tag);
         return {};
      }

      logger::message_info(logger::format("Loading asset \'", id, "\'"), logger_tag);

      const std::filesystem::path path = get_asset_path(id);
      std::ifstream file(path, std::ios::binary | std::ios::in);

      if (!file.is_open() || file.bad())
      {
         logger::error(logger_tag, "Couldn't load asset - bad file");
         return {};
      }

      file.unsetf(std::ios::skipws);

      file.seekg(0, std::ios::end);
      const std::streampos size = file.tellg();
      file.seekg(0, std::ios::beg);

      std::vector<unsigned char> data;
      data.reserve(size);
      data.insert(data.begin(), std::istream_iterator<unsigned char>(file), std::istream_iterator<unsigned char>());

      file.close();

      return data;
   }


   image_data read_asset_image(const std::string& id)
   {
      ZoneScoped;
      if (!asset_exists(id))
      {
         logger::message_error(logger::format("Could not read asset \'", id, "\' !"), logger_tag);
         return { nullptr, 0, 0, 0 };
      }

      image_data result;
      int temp_width;
      int temp_height;
      int temp_channels;

      result.data = stbi_load(get_asset_path(id).string().c_str(), &temp_width, &temp_height, &temp_channels, 0);
      if (result.data == nullptr)
      {
         logger::error(logger_tag, "Couldn't load asset - bad file");
         return { nullptr, 0, 0, 0 };
      }

      result.width = temp_width;
      result.height = temp_height;
      result.num_channels = temp_channels;
      return result;
   }

   bool save_raw_asset(const std::string& id, const std::vector<unsigned char>& data)
   {
      ZoneScoped;
      const std::filesystem::path directory = get_asset_directory(id);
      const std::filesystem::path path = get_asset_path(id);

      if (!std::filesystem::exists(directory) && !std::filesystem::create_directories(directory))
      {
         logger::error(logger_tag, "Couldn't create directory \'", directory.string(), "\'");
         return false;
      }

      std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
      if (!file.is_open() || file.bad())
      {
          logger::error(logger_tag, "Couldn't save asset - bad file");
         return false;
      }

      logger::info(logger_tag, "Saving asset \'", id, "\'");

      file.write(reinterpret_cast<const char*>(data.data()), data.size());
      file.close();

      return true;
   }

   bool save_asset_text(const std::string& id, const std::string_view& text)
   {
      ZoneScoped;
      const std::filesystem::path directory = get_asset_directory(id);
      const std::filesystem::path path = get_asset_path(id);

      if (!std::filesystem::create_directories(directory))
      {
         logger::error(logger_tag, "Couldn't create directory \'", directory, "\'");
         return false;
      }

      std::ofstream file(path, std::ios::out | std::ios::trunc);
      if (!file.is_open() || file.bad())
      {
         logger::error(logger_tag, "Couldn't save asset - bad file");
         return false;
      }

      logger::info(logger_tag, "Saving asset \'", id, "\'");

      file.write(text.data(), text.size());
      file.close();

      return true;
   }

   void delete_asset(const std::string& asset_id)
   {
      ZoneScoped;
      const std::filesystem::path path = get_asset_path(asset_id);
      if (!std::filesystem::exists(path))
      {
         logger::warn(logger_tag, "Couldn't delete asset at \'", path.string(), "\' - doesn't exist.");
         return;
      }

      logger::info(logger_tag, "Deleting asset at \'", path.string(), "\'");
      std::filesystem::remove_all(path);
   }

   void delete_asset_directory(const std::string& asset_id)
   {
      ZoneScoped;
      const std::filesystem::path directory = get_asset_directory(asset_id);
      if (!std::filesystem::exists(directory))
      {
         logger::warn(logger_tag, "Couldn't delete directory at \'", directory.string(), "\' - doesn't exist.");
         return;
      }

      logger::info(logger_tag, "Deleting directory at \'", directory.string(), "\'");
      std::filesystem::remove_all(directory);
   }

   std::string read_asset_text(const std::string& id)
   {
      ZoneScoped;
      if (should_preprocess(get_asset_type(id)))
      {
         return read_preprocessed_asset_text(id);
      }

      return read_raw_asset_text(id);
   }
}
