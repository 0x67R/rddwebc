# (RDDWEBCrawling)
RDDWEBC is software for searching web directories.

# Features
RDDWEBC uses the 'HTTP HEAD METHOD' to make requests, which means higher performance, does not overload the CPU, 
ram usage almost imperceptible, is less than 35 kb in size meaning less disk space.

You can use the manual request method, where you will pass a string to search, 
or use the brute force method where you will pass a wordlist.

Tested on Debian, Ubuntu, and Mint.

## Requirements
- libcurl4-openssl-dev

## How to build
```
g++ -std=c++2a -m64 -O2 -fstack-protector-all -s  rddwebc.cpp -o rddwebc -lcurl -Wall -Werror
```

## License
- Copyright (C) 2019, Rafael Godoy, <0x67rafael@protonmail.com>
- This project is licensed under the GNU General Public License v3.0 - see the [LICENSE.md](https://github.com/0x67R/rddwebc/blob/master/LICENSE.md)
 file for details
