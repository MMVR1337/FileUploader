#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <shlobj.h>
#include <fstream>

#include <filesystem>
#include <random>
#include <curl/curl.h>
#include <json.hpp>

#include "utils/settings.h"

#include "utils/utils.h"

#include "utils/dropbox/dropbox_auth.h"
#include "utils/dropbox/dropbox_upload.h"

#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Secur32.lib")