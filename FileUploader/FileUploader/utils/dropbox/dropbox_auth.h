std::string refresh_dropbox_token() {
    CURL* curl = curl_easy_init();
    if (!curl) return "";

    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    fs::path caBundlePath = fs::path(exePath).parent_path() / "cacert.pem";

    if (fs::exists(caBundlePath)) {
        curl_easy_setopt(curl, CURLOPT_CAINFO, caBundlePath.string().c_str());
        std::cout << "Using CA bundle: " << caBundlePath << std::endl;
    }
    else {
        std::cerr << "CA bundle not found, using system certificates" << std::endl;
    }

    std::string response;
    std::string post_data = "grant_type=refresh_token"
        "&refresh_token=" + Settings::DROPBOX_REFRESH_TOKEN +
        "&client_id=" + Settings::DROPBOX_APP_KEY +
        "&client_secret=" + Settings::DROPBOX_APP_SECRET;

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.dropbox.com/oauth2/token");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L); 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "Token refresh failed (" << res << "): " << curl_easy_strerror(res) << std::endl;
        return "";
    }

    size_t token_start = response.find("\"access_token\":\"") + 16;
    if (token_start == std::string::npos) {
        std::cerr << "Invalid token response format" << std::endl;
        return "";
    }

    size_t token_end = response.find("\"", token_start);
    return response.substr(token_start, token_end - token_start);
}