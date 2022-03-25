# FTX Zorro Plugin
FTX Plugin for Zorro Trader

# Instructions

- Build as the x86 (32 bit) architecture & place the output dynamic library FTX.dll into Zorro/Plugin folder.
- Place dependencies into the Zorro root folder:
  - fmt.dll
  - libcrypto-1_1.dll
  - libssl-1_1.dll
- Plugin logs all issues into Zorro/Log/ftx.log file.

# Dependencies

- https://github.com/gabime/spdlog
- https://github.com/aantron/better-enums
- https://github.com/nlohmann/json
- https://www.boost.org

# Limitations

- Supports perpetual futures only