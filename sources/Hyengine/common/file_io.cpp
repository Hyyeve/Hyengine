#include "file_io.hpp"

#include <filesystem>
#include <fstream>
#include <regex>
#include <string>

#include "../core/logger.hpp"
#include "portability.hpp"

namespace hyengine::common::file_io {

   constexpr std::string_view raw_directory = "assets";
   constexpr std::string_view logger_tag = "FileIO";

   std::string asset_id_to_relative_path(std::string asset_id)
   {
      common::portability::string_replace(asset_id, ':', std::filesystem::path::preferred_separator);
      common::portability::string_replace(asset_id, '.', std::filesystem::path::preferred_separator);
      return asset_id;
   }

   std::string get_asset_type(const std::string& asset_id)
   {
      return asset_id.substr(0, asset_id.find(':'));
   }

   std::string_view get_asset_extension(const std::string_view type)
   {
      if (type == "shader")  return "glsl";
      if (type == "text")  return "hytx";
      return "hy";
   }

   bool should_preprocess(const std::string_view type)
   {
      if (type == "shader")  return true;
      return false;
   }

   std::regex directive_data_pattern(const std::string& directive)
   {
      const std::string pattern = "<" + directive + "=([^>]+)>";
      return std::regex(pattern);
   }

   std::regex directive_line_pattern(const std::string& directive)
   {
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
      const std::string relative_path = asset_id_to_relative_path(asset_id);
      return std::filesystem::path(get_primary_asset_directory()).append(relative_path).replace_extension(get_asset_extension(get_asset_type(asset_id)));
   }

   std::string read_raw_asset_text(const std::string& id)
   {
      const std::filesystem::path path = get_asset_path(id);

      if (!std::filesystem::exists(path))
      {
         logger::message_error(logger::format("Could not read asset \'", id, "\' !"), logger_tag);
         return "";
      }

      logger::message_info(logger::format("Loading asset \'", id, "\'"), logger_tag);

      const std::ifstream file(path);
      std::stringstream buffer;
      buffer << file.rdbuf();
      return buffer.str();
   }

   std::string find_directive(const std::string& text, const std::string& directive)
   {
      std::smatch regex_match;
      if (std::regex_search(text.cbegin(), text.cend(), regex_match, directive_data_pattern(directive)))
      {
         return regex_match.str(1);
      }

      return "";
   }

   void replace_directive(std::string& text, const std::string& directive, const std::string& replacement)
   {
      std::smatch regex_match;
      if (std::regex_search(text.cbegin(), text.cend(), regex_match, directive_line_pattern(directive)))
      {
         text.erase(regex_match.position(), regex_match.length());
         text.insert(regex_match.position(), replacement);
         text.insert(regex_match.position(), "\n");
      }
   }

   std::string inject_text_includes(const std::string_view text)
   {
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
      const std::string text = read_raw_asset_text(id);
      return inject_text_includes(text);
   }

   std::string read_asset_text(const std::string& id)
   {
      if (should_preprocess(get_asset_type(id)))
      {
         return read_preprocessed_asset_text(id);
      }

      return read_raw_asset_text(id);
   }
}
