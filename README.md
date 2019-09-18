# (RDDWEBCrawling)
RDDWEBC is software for searching web directories.

# Features
RDDWEBC uses the 'HTTP HEAD METHOD' to make requests, which means higher performance, does not overload the CPU, 
ram usage almost imperceptible, is less than 60 kb in size meaning less disk space.

All requests are saved in the path $HOME/rddwebc_logs/rddwebc_logs.txt, 
with the URL name, the code returned by the request, and the timestamp.

You can define the directory that rddwebc will look for manually or use the brute force method, 
which will use a TXT file of your choice, this file should contain the directories names you want to search.

Tested on Debian, Ubuntu, and Mint.

## Requirements
- libboost-all-dev
- libcurl4-openssl-dev

## How to build
```
g++ -std=c++17 -m64 -fstack-protector-all rddwebc.cpp -o rddwebc -lcurl -lpthread -lboost_system -lboost_filesystem
```

## License
- Copyright (C) 2019, Rafael Godoy, <0x67rafael@protonmail.com>
- This project is licensed under the MIT License - see the [LICENSE.md](https://github.com/0x67R/rddwebc/blob/master/LICENSE.md)
 file for details
