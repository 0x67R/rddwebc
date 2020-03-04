/*
GNU General Public License v3.0

Copyright (C) 2019, Rafael Godoy, 0x67rafael@protonmail.com .

RDDWEBC is free software: you can redistribute it and/or modify it under the terms of the 
GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

RDDWEBC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with RDDWEBC. 
If not, see https://www.gnu.org/licenses/.

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Curl and libcurl are true Open Source/Free Software and meet all definitions as such. 
It means that you are free to modify and redistribute all contents of the curl distributed archives. 
You may also freely use curl and libcurl in your commercial projects.

Curl and libcurl are licensed under the license below, which is inspired by MIT/X, but not identical.

Copyright (C) 1998 - 2015, Daniel Stenberg, daniel@haxx.se, et al.

This software is licensed as described in the file COPYING, 
which you should have received as part of this distribution. 
The terms are also available at https://curl.haxx.se/docs/copyright.html. 
You may opt to use, copy, modify, merge, publish, distribute and/or sell copies of the Software, 
and permit persons to whom the Software is furnished to do so, under the terms of the COPYING file. 
This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.

*/



#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <getopt.h>
#include <curl/curl.h>
#include <cerrno>


typedef signed long int slong_int;

#define ERROR_INTERN_RDDWEBCGET 999;


static void method_usage_rddwebc(void)
{
    std::cout << "\n\n[!] Usage: rddwebc [options...]\n\n"
         "\t-u, --url    \t   Request website\n"
         "\t-p, --path   \t   Enter the wordlist path if you have chosen BRUTEFORCE. If not, type a STRING\n"
         "\t-a, --agent  \t   To use the default agent, set 'default', "
         "if you want to use one of your choice, set '-a <YOUR USER-AGENT>'\n" << std::endl;
}

static void banner_rddwebc(void)
{
    std::cout << "\n\t========= <RDDWEBC WEB CRAWLING> [FOCUSED ON DIRECTORIES]  =========\n"
            "\tCopyright (C) 2019, Rafael Godoy, <0x67rafael@protonmail.com>\n"
            "\t\t      Github: https://github.com/0x67R\n\n"
            "\t____________________________________________________________________\n" << std::endl;
}

static std::string time_stamp(void)
{
    time_t cal_time;
    cal_time = time(NULL);
    auto act_time_stamp = asctime(localtime(&cal_time));
    return act_time_stamp;
}

static std::list<std::string> wordlist(const std::string &path)
{
    std::ifstream path_reading;
    path_reading.open(path);

    std::list<std::string> word_list;
    std::string buffer_tmp;

    if (path_reading.is_open()) {
        while (std::getline(path_reading, buffer_tmp))
            word_list.push_back(buffer_tmp);
        buffer_tmp.clear();
        path_reading.close();
        return word_list;
    } else if (ENOENT) {
        word_list.push_back(path);
        path_reading.close();
        return word_list;
    } else {
        std::cerr << "[!] PARAMETER ENTERED FOR OPTION [PATH] IS INVALID\n"
                     "[!] WARNING, ABORTING SOFTWARE" << std::endl;

        exit(EXIT_FAILURE);
    }
}


static size_t not_verbose(void *buffer, size_t size, size_t nmemb, void *userp)
{
    return size * nmemb;
}

static slong_int rddwebc_get(const std::string &url, const std::string &user_agent)
{
    CURL *rddwebc;
    CURLcode response_code;
    slong_int http_responde_code;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    rddwebc = curl_easy_init();

    if (rddwebc) {
        curl_easy_setopt(rddwebc, CURLOPT_USERAGENT, user_agent.c_str());
        curl_easy_setopt(rddwebc, CURLOPT_URL, url.c_str());
        curl_easy_setopt(rddwebc, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(rddwebc, CURLOPT_WRITEFUNCTION, not_verbose);

        #ifdef SKIP_PEER_VERIFICATION

        /*
         * If you want to connect to a site who isn't using a certificate that is
         * signed by one of the certs in the CA bundle you have, you can skip the
         * verification of the server's certificate. This makes the connection
         * A LOT LESS SECURE.
         *
         * If you have a CA cert for the server stored someplace else than in the
         * default bundle, then the CURLOPT_CAPATH option might come handy for
         * you.
         */

        curl_easy_setopt(rddwebc, CURLOPT_SSL_VERIFYPEER, 0L);

        #endif

        #ifdef SKIP_HOSTNAME_VERIFICATION

        /*
         * If the site you're connecting to uses a different host name that what
         * they have mentioned in their server certificate's commonName (or
         * subjectAltName) fields, libcurl will refuse to connect. You can skip
         * this check, but this will make the connection less secure.
         */

        curl_easy_setopt(rddwebc, CURLOPT_SSL_VERIFYHOST, 0L);

        #endif

        response_code = curl_easy_perform(rddwebc);

        if (response_code != CURLE_OK) {
            std::cerr << "[!!!] WARNING, RDDWEBC ERROR: " << curl_easy_strerror(response_code) << std::endl;
            curl_easy_cleanup(rddwebc);
            curl_global_cleanup();
            return ERROR_INTERN_RDDWEBCGET;
        }

        curl_easy_getinfo(rddwebc, CURLINFO_RESPONSE_CODE, &http_responde_code);

        curl_easy_cleanup(rddwebc);

    } else {
        std::cerr << "[!!!] FATAL ERROR: " << strerror(errno) << std::endl;
        curl_easy_cleanup(rddwebc);
        curl_global_cleanup();
        exit(EXIT_FAILURE);
    }

    curl_global_cleanup();

    return http_responde_code;
};

static void ___rddwebcrawling___(const std::string &url, const std::list<std::string> &brut_wordlist, std::string agent)
{
    if (agent.empty() || agent == "default")
        agent = "Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)";

    banner_rddwebc();

    std::cout << "[*] INITIATING THE REQUISITIONS..\n" << std::endl;

    std::list<std::string>::const_iterator brut_wordlist_it;

    for (brut_wordlist_it = brut_wordlist.begin(); brut_wordlist_it != brut_wordlist.end(); brut_wordlist_it++) {

        std::string url_crawling = url;
        url_crawling.append(("/" + *brut_wordlist_it));
        slong_int rddwebc_get_return = rddwebc_get(url_crawling, agent);

        std::cout << "[***] URL REQUEST: " << url_crawling <<
                     "\n[***] HTTP RESPONSE STATUS CODE: [ " << rddwebc_get_return << " ] " <<
                     "\n[***] USER-AGENT: " << agent <<
                     "\n[***] REQUISITION DATE AND TIME: " << time_stamp() << std::endl;
    }
}


int main(int argc, char *argv[])
{
    std::string url, path_string_or_wordlist, agent;
    int option_index, arguments_parse;

    static const struct option long_options[] = {
        { "url",      required_argument,  NULL, 'u'},
        { "path",     required_argument,  NULL, 'p'},
        { "agent",    required_argument,  NULL, 'a'},
        {  0,               0,             0,    0 }
    };

    static const char *optstring = "u:p:a:";

    while ((arguments_parse = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1) {
        switch (arguments_parse) {
            case 'u':
                url = optarg;
                break;
            case 'p':
                path_string_or_wordlist = optarg;
                break;
            case 'a':
                agent = optarg;
                break;
            default:
                banner_rddwebc();
                method_usage_rddwebc();
                exit(EXIT_FAILURE);
        }
    }

    if (optind < 5 || url.empty()) {
        banner_rddwebc();
        method_usage_rddwebc();
        exit(EXIT_FAILURE);
    }

    ___rddwebcrawling___(url, wordlist(path_string_or_wordlist), agent);

    return 0;
}
