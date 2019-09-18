/*
   	The MIT License (MIT)

 	Copyright (C) 2019, Rafael Godoy, <0x67rafael@protonmail.com> .

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

    ______________________________________________________________________

	Curl and libcurl are true Open Source/Free Software and meet all definitions as such. It means that you are free to modify and
	redistribute all contents of the curl distributed archives. You may also freely use curl and libcurl in your commercial projects.

	Curl and libcurl are licensed under the license below, which is inspired by MIT/X, but not identical.

	Copyright (C) 1998 - 2015, Daniel Stenberg, <daniel@haxx.se>, et al.

	This software is licensed as described in the file COPYING, which
 	you should have received as part of this distribution. The terms
 	are also available at https://curl.haxx.se/docs/copyright.html.
	You may opt to use, copy, modify, merge, publish, distribute and/or sell
 	copies of the Software, and permit persons to whom the Software is
	furnished to do so, under the terms of the COPYING file.
	This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
	KIND, either express or implied.

*/

#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include <getopt.h>
#include <curl/curl.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>

// For readability //

using std::string;
using std::to_string;
using std::getenv;
using std::getline;
using std::fstream;
using std::ios_base;
using std::list;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::time_t;
using std::ctime;
using std::iterator;
using capture_exception = std::exception;

using namespace std::chrono;
using namespace boost;
using namespace boost::algorithm;

typedef signed long int SLONGINT;

#define ERROR_INTERN_RDDWEBCGET 9999;

//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

static void method_usage_rddwebc (void)
{
    cout << "\033[1;31m\n\n[!]\033[0m Usage: rddwebc [options...]\n\n"
         "  -u       'URL'        ||   Request website\n"
         "  -m       'MODE'       ||   Set '-m bforce' for BRUTEFORCE or '-m manual' build a URL manually for the request\n"
         "  -p       'PATH'       ||   Enter the wordlist path if you have chosen BRUTEFORCE. If not, type a STRING\n"
         "  -a       'USER-AGENT' ||   To use the default user-agent, set '-a default', "
                                      "if you want to use one of your choice, set '-a <YOUR USER-AGENT>\n" << endl;
}

static void banner_rddwebc (void)
{
    cout << "\033[1;31m\n\t========= <RDDWEBC WEB CRAWLING> [FOCUSED ON DIRECTORIES]  =========\n"
         "\033[0m\t"
         "\033[1;37mCopyright (C) 2019, Rafael Godoy, \033[4;37m<0x67rafael@protonmail.com>\033[0m\n" << endl;
}

size_t non_verbose (void *buffer, size_t size, size_t nmemb, void *userp)
{
    return size * nmemb;
}

static SLONGINT rddwebc_get (const string &url, const string &user_agent)
{

    CURL *rddwebc;
    CURLcode response_code;
    SLONGINT http_responde_code;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    rddwebc = curl_easy_init();

    if (rddwebc) {
        curl_easy_setopt(rddwebc, CURLOPT_USERAGENT, user_agent.c_str());
        curl_easy_setopt(rddwebc, CURLOPT_URL, url.c_str());
        curl_easy_setopt(rddwebc, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(rddwebc, CURLOPT_WRITEFUNCTION, non_verbose);

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
            string curl_error = (curl_easy_strerror(response_code));
            algorithm::to_upper(curl_error);
            cerr << "\033[1;31m[!!!]\033[0m WARNING, RDDWEBC ERROR: " << curl_error  << endl;
            curl_easy_cleanup(rddwebc);
            curl_global_cleanup();
            return ERROR_INTERN_RDDWEBCGET;
        }

        curl_easy_getinfo(rddwebc, CURLINFO_RESPONSE_CODE, &http_responde_code);

        curl_easy_cleanup(rddwebc);
    }

    curl_global_cleanup();

    return http_responde_code;
}

static list<string> bytes_for_request (const string &mode, const string &path)
{
    if (mode == "manual") {
        list<string> string_for_request;
        string_for_request.push_back(path);
        return string_for_request;
    } else if (mode == "bforce") {
        fstream path_reading;
        path_reading.open(path, ios_base::in);

        if (!(path_reading.is_open())) {
            string error_path = strerror(errno);
            algorithm::to_upper(error_path);
            cerr << "\033[1;31m[!]\033[0m FAILURE TO OPEN WORDLIST: " << error_path << endl;
            cerr << "\033[1;31m[!]\033[0m PARAMETER ENTERED FOR OPTION [PATH] IS INVALID\n\033[1;31m[!]\033[0m WARNING, ABORTING SOFTWARE" << endl;
            _Exit(1);
        }
        list<string> w_list;
        string tmp_buf;

        while (getline(path_reading, tmp_buf)) {
            algorithm::trim(tmp_buf);
            w_list.push_back(tmp_buf);
        }
        tmp_buf.clear();
        path_reading.close();
        return w_list;
    } else {
        cerr << "\033[1;31m[!]\033[0m PARAMETER ENTERED FOR OPTION [MODE] IS INVALID\n\033[1;31m[!]\033[0m WARNING, ABORTING SOFTWARE" << endl;
        _Exit(1);
    }
}

static bool w_logs(string r_logs)
{
    string path_home_user = getenv("HOME");
    path_home_user.append("/rddwebc_logs/");
    if (filesystem::exists(path_home_user)) {
        if (!(filesystem::is_directory(path_home_user)))
            if (filesystem::remove(path_home_user) == true) {
                if (!(filesystem::create_directory(path_home_user))) {
                    cerr << "\033[1;31m[!]\033[0m FAILED TO CREATE DIRECTORY" << endl;
                    cerr << "\033[1;31m[!]\033[0m THE LOGS WERE NOT CREATED" << endl;
                    return false;
                }
            } else {
                cerr << "\033[1;31m[!]FAILURE TO REMOVE PATH (NOT DIRECTORY): " << path_home_user << endl;
                cerr << "[!] THE LOGS WERE NOT CREATED\033[0m" << endl;
                return false;
            }
    } else if (!(filesystem::create_directory(path_home_user))) {
        cerr << "\033[1;31m[!]\033[0m FAILED TO CREATE DIRECTORY" << endl;
        cerr << "\033[1;31m[!]\033[0m THE LOGS WERE NOT CREATED" << endl;
        return false;
    }

    fstream buffer_open_path;
    buffer_open_path.open(path_home_user.append("rddwebc_logs.txt"), ios_base::app);
    if (!(buffer_open_path.is_open()))
        return false;

    buffer_open_path << r_logs;
    r_logs.clear();
    buffer_open_path.close();
    return true;
}

static string date_time_request (void)
{
    auto start_time = system_clock::now();
    auto end_time = system_clock::now();
    time_t end_time_t = system_clock::to_time_t(end_time);

    string r_date_time_request;
    r_date_time_request.append((ctime(&end_time_t)));
    algorithm::to_upper(r_date_time_request);
    return r_date_time_request;
}

static void rddwebc_crawling (string url, list<string> b_request, string agent)
{
    algorithm::trim(url);
    algorithm::trim(agent);
    if (agent == "default")
        agent = "Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)";

    string rddwebc_logs_buffer;

    banner_rddwebc();
    cout << "\033[1;32m\n\n[*]\033[0m INITIATING THE REQUISITIONS..\n" << endl;

    for (list<string>::iterator b_request_iterator = b_request.begin(); b_request_iterator != b_request.end(); b_request_iterator++) {
        string url_crawling = url;
        url_crawling.append(("/" + *b_request_iterator));
        SLONGINT rddwebc_get_return = rddwebc_get(url_crawling, agent);

        rddwebc_logs_buffer.append(("[***] URL REQUEST: " +  url_crawling +
                                    "\n[***] USER-AGENT: " + agent +
                                    "\n[***] HTTP RESPONSE STATUS CODE: " +
                                    (to_string(rddwebc_get_return)) +
                                    "\n[***] REQUISITION DATE AND TIME: " + date_time_request() + "\n\n"));

        cout << "\033[1;32m[***]\033[0m URL REQUEST: " << url_crawling <<
                "\n\033[1;32m[***]\033[0m USER-AGENT: " << agent <<
                "\n\033[1;32m[***]\033[0m HTTP RESPONSE STATUS CODE: " << rddwebc_get_return <<
                "\n\033[1;32m[***]\033[0m REQUISITION DATE AND TIME: " << date_time_request() << "\n" << endl;
    }

    if (w_logs(rddwebc_logs_buffer) == true)
        cout << "\033[1;32m[+]\033[0m LOGS WERE SUCCESSFULLY WRITTEN" << endl;
    else
        cerr << "\033[1;31m[!]\033[0m FAILED TO WRITE LOGS" << endl;
}

int main (int argc, char **argv)
{

    string url, path_stringorwordlist, mode, agent;
    int arguments;

    while ((arguments = getopt (argc, argv, "u:m:p:a:")) != -1) {
        switch (arguments) {
            case 'u':
                url = optarg;
                break;
            case 'm':
                mode = optarg;
                break;
            case 'p':
                path_stringorwordlist = optarg;
                break;
            case 'a':
                agent = optarg;
                break;
            default:
                banner_rddwebc();
                method_usage_rddwebc();
                _Exit(1);
        }
    }

    if (optind != 9) {
        banner_rddwebc();
        method_usage_rddwebc();
        _Exit(1);
    }

    rddwebc_crawling(url, bytes_for_request(mode, path_stringorwordlist), agent);

    return 0;
}

