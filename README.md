# spell-bot
My term paper for my first year at NURE University.
----------------
## Dependencies
Dependencies:
- CMake
- Boost
- OpenSSL
- ZLib
- Libcurl (optional unless you want to use curl-based http client CurlHttpClient).
----------
## Deps installation on Linux
For Debian-based distros:
```bash
sudo apt-get install g++ make binutils cmake libboost-system-dev libssl-dev zlib1g-dev libcurl4-openssl-dev
```

For Solus:

Install "devel" components:
```bash
sudo eopkg it -c system.devel
```
Install deps:
```bash
sudo eopkg it libboost-devel
```

## Compile library
```bash
git clone https://github.com/reo7sp/tgbot-cpp
```

```bash
cd tgbot-cpp
```
```bash
cmake .
```
```bash
make -j4
```
```bash
sudo make install
```

# How to build the bot without Cmake?
```bash
g++ telegram_bot.cpp -o telegram_bot --std=c++14 -I/usr/local/include -lTgBot -lboost_system -lssl -lcrypto -lpthread
```
