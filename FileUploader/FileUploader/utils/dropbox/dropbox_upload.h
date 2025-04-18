
std::string parse_share_response(const std::string& response) {
    try {
        auto json = nlohmann::json::parse(response);
        std::string url = json["url"];

        size_t pos = url.find("www.dropbox.com");
        if (pos != std::string::npos) {
            url.replace(pos, 15, "dl.dropbox.com");
        }

        return url;
    }
    catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return "";
    }
}

std::string upload_to_dropbox(const std::string& file_path, const std::string& access_token) {
    if (!fs::exists(file_path)) {
        std::cerr << "[Dropbox] Ошибка: Файл не найден: " << file_path << std::endl;
        return "";
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[Dropbox] Ошибка: Не удалось инициализировать cURL" << std::endl;
        return "";
    }

    std::string filename = fs::path(file_path).filename().string();
    std::string api_arg = R"({"path":"/)" + filename + R"(","mode":"add"})";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
    headers = curl_slist_append(headers, ("Dropbox-API-Arg: " + api_arg).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/octet-stream");

    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "[Dropbox] Ошибка: Не удалось открыть файл: " << file_path << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return "";
    }

    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(file_size);
    if (!file.read(buffer.data(), file_size)) {
        std::cerr << "[Dropbox] Ошибка чтения файла: " << file_path << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return "";
    }
    file.close();

    curl_easy_setopt(curl, CURLOPT_URL, "https://content.dropboxapi.com/2/files/upload");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.data());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, file_size);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    fs::path caBundlePath = fs::current_path() / "cacert.pem";
    if (fs::exists(caBundlePath)) {
        curl_easy_setopt(curl, CURLOPT_CAINFO, caBundlePath.string().c_str());
    }
    else {
        std::cerr << "[Dropbox] Предупреждение: CA bundle не найден" << std::endl;
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        std::cerr << "[Dropbox] Ошибка загрузки: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return "";
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (http_code != 200) {
        std::cerr << "[Dropbox] Ошибка HTTP: " << http_code << std::endl;
        std::cerr << "[Dropbox] Ответ сервера: " << response << std::endl;
        curl_easy_cleanup(curl);
        return "";
    }

    curl_easy_cleanup(curl);

    try {
        auto json = nlohmann::json::parse(response);
        if (json.contains("path_display")) {
            return json["path_display"].get<std::string>();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[Dropbox] Ошибка парсинга JSON: " << e.what() << std::endl;
    }

    return "";
}
