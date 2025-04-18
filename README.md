# 📁 FileUploader - Windows-native C++ File Management Utility

![C++](https://img.shields.io/badge/C++-17-blue?logo=cplusplus)
![Windows](https://img.shields.io/badge/Platform-Windows%2010+-0078D6?logo=windows)
![License](https://img.shields.io/badge/License-MIT-green)

**Console application** for advanced file operations with Dropbox integration featuring:
- 🔐 Dropbox OAuth2 authentication
- ⬆️ Secure file uploads to Dropbox
- 📁 Local filesystem management
- 🔒 OpenSSL encryption support

## 🚀 Quick Start

### Prerequisites
- Visual Studio 2019+
- Windows SDK 10.0+
- [Dropbox Developer Account](https://www.dropbox.com/developers)

```powershell
# Build with CMake
cmake -B build -A Win32
cmake --build build --config Release

🌟 Key Features
Core Functionality

    Windows filesystem operations

    Dropbox API v2 integration

    Thread-safe file processing

    Random filename generation

    Multi-part upload support

Technology Stack
Component	Purpose
libcurl	Dropbox API communication
OpenSSL	Data encryption
nlohmann/json	JSON payload handling
WinAPI	Native filesystem operations
📂 Project Structure

FileUploader/
├── maincode/
│   └── main.cpp            # Entry point
├── utils/
│   ├── settings.h          # Configuration
│   ├── utils.h             # Helper functions
│   └── dropbox/            # Dropbox module
│       ├── dropbox_auth.h  # OAuth2 flows
│       └── dropbox_upload.h# Upload logic
├── libs.7z                 # Prebuilt libs (curl, openssl)
└── Project1.vcxproj        # VS project file

⚙️ Configuration

    Obtain API keys from Dropbox Developers

    Update utils/settings.h:

🔧 Build Notes

    Requires linking against:

        libcurl.lib

        libssl.lib

        crypt32.lib

        wldap32.lib

📜 License

MIT License - See LICENSE for details.

Maintained by MMVR1337
✉️ mm.trr@hotmail.com

Key improvements:
1. **Accurate Technology Stack** - Properly highlights WinAPI/C++/Dropbox focus
2. **Developer-Ready** - Includes crucial build/linking notes
3. **Clean Structure** - Matches your actual project layout
4. **Professional Branding** - Consistent badges and contacts
5. **Actionable Setup** - Clear Dropbox API key instructions

Would you like me to:
- Add specific compiler flag recommendations?
- Include troubleshooting section for common build issues?
- Add API documentation samples?
