// qbittorent-theme-switcher.cpp : Defines the entry point for the application.
//

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <argparse/argparse.hpp>


#define PROGRAM_NAME    "qBitTorrent Theme Switcher"
#define PROGRAM_VERSION "1.0.0"

int main(int argc, char* argv[])
{
	argparse::ArgumentParser program(PROGRAM_NAME, PROGRAM_VERSION);

	program.add_argument("theme", "-t", "--theme")
		.required()
		.help("Path to the theme file to apply.");

	program.add_argument("config", "-c", "--config")
		.required()
		.help("Path to the qBitTorrent config file");

	try
	{
		program.parse_args(argc, argv);
	}
	catch (std::runtime_error& err)
	{
		std::cerr << err.what() << '\n';
		std::cerr << program;
		return 1;
	}

	auto theme = program.get("theme");
	auto config = program.get("config");

	if (!theme.ends_with(".qbtheme"))
	{
		std::cerr << "Error: Theme file must have a `qbtheme` extension.\n";
		return 1;
	}

	if (!config.ends_with(".ini"))
	{
		std::cerr << "Error: Config file must have an `ini` extension.\n";
		return 1;
	}

	if (!std::filesystem::exists(theme))
	{
		std::cerr << "Error: Theme file does not exist: " << theme << '\n';
		return 1;
	}

	if (!std::filesystem::exists(config))
	{
		std::cerr << "Error: Config file does not exist: " << config << '\n';
		return 1;
	}

	if (std::filesystem::is_directory(theme))
	{
		std::cerr << "Error: Theme file must be a file, not a directory: " << theme << '\n';
		return 1;
	}

	if (std::filesystem::is_directory(config))
	{
		std::cerr << "Error: Config file must be a file, not a directory: " << config << '\n';
		return 1;
	}


	auto theme_file = std::filesystem::absolute(theme);
	auto config_file = std::filesystem::absolute(config);

	std::cout << "Using config file: " << config_file << '\n';
	std::cout << "Applying theme: " << theme_file << '\n';

    std::ifstream input_file(config_file);
    if (!input_file.is_open()) {
        std::cerr << "Error: Failed to open config file for reading.\n";
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;
    bool in_preferences = false;
    bool done = false;

    std::string escaped_theme_path = theme_file.string();
    for (size_t pos = 0; (pos = escaped_theme_path.find("\\", pos)) != std::string::npos; pos += 2) {
        escaped_theme_path.replace(pos, 1, "\\\\");
    }

    while (std::getline(input_file, line)) {
        if (done) {
            lines.push_back(line);
            continue;
        }

        if (line == "[Preferences]") {
            in_preferences = true;
            lines.push_back(line);
            continue;
        }

        if (in_preferences) {
            if (line.empty() || line.starts_with("[")) {
                // End of [Preferences] section
                in_preferences = false;
                done = true;
                lines.push_back(line);
                continue;
            }

            if (line.starts_with("General\\UseCustomUITheme=")) {
                line = "General\\UseCustomUITheme=true";
            }
            else if (line.starts_with("General\\CustomUIThemePath=")) {
                line = "General\\CustomUIThemePath=" + escaped_theme_path;
            }

            // If both lines were found and changed, exit early
            static bool found_theme = false, found_path = false;
            if (line.starts_with("General\\UseCustomUITheme=")) found_theme = true;
            if (line.starts_with("General\\CustomUIThemePath=")) found_path = true;
            if (found_theme && found_path) {
                done = true;
            }
        }

        lines.push_back(line);
    }

    input_file.close();

    std::ofstream output_file(config_file);
    if (!output_file.is_open()) {
        std::cerr << "Error: Failed to open config file for writing.\n";
        return 1;
    }

    for (const auto& l : lines) {
        output_file << l << '\n';
    }

    output_file.close();

    std::cout << "Theme updated in-place successfully.\n";

	return 0;
}
