#include "ctftools/ctftools.hpp"

#include <chrono>
#include <curl/curl.h>
#include <curl/easy.h>
#include <memory>

//#define CTFTOOLSVERBOSE

//-------------------------------------------------------------------------------------------------------------------------------
// RAII wrapppers

using Curl = std::unique_ptr<CURL, void(*)(CURL*)>;

struct EasyCurlSlist {
    DO_RULE_OF_5_NO_COPY(EasyCurlSlist)

    explicit EasyCurlSlist() : slist(nullptr) {
    }

    ~EasyCurlSlist() {
        curl_slist_free_all(slist);
    }

    void Append(const char* header) {
        curl_slist* tmp = curl_slist_append(slist, header);
        if (tmp) {
            slist = tmp;
        }
    }

    curl_slist* slist;
};

//-------------------------------------------------------------------------------------------------------------------------------
// Curl callbacks

size_t AddToString(char *ptr, size_t size, size_t nmemb, void *userdata) {
    reinterpret_cast<std::string*>(userdata)->append(ptr, size * nmemb);
    return size * nmemb;
}

size_t AddToVector(char *ptr, size_t size, size_t nmemb, void *userdata) {
    reinterpret_cast<std::vector<std::string>*>(userdata)->emplace_back(ptr, size * nmemb);
    return size * nmemb;
}

//-------------------------------------------------------------------------------------------------------------------------------
// fast
//-------------------------------------------------------------------------------------------------------------------------------
// easy ~~ Http subsection
ctf::easy::HttpResponse ctf::easy::Get(const std::string& url, const std::vector<std::string>& headers) {
    Curl curl{curl_easy_init(), curl_easy_cleanup};
    HttpResponse result;
    curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, AddToString);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &result.body);
    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION, AddToVector);
    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA, &result.headers);
    EasyCurlSlist slist;
    for (auto &x : headers) {
        slist.Append(x.c_str());
    }
    curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, slist.slist);
#ifdef CTFTOOLSVERBOSE
    curl_easy_setopt(curl.get(), CURLOPT_VERBOSE, 1);
#endif
    curl_easy_perform(curl.get());
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &result.status_code);
    return std::move(result);
}

ctf::easy::HttpResponse ctf::easy::Post(const std::string& url, const char* data, size_t size, const std::vector<std::string>& headers) {
    Curl curl{curl_easy_init(), curl_easy_cleanup};
    HttpResponse result;
    curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, AddToString);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &result.body);
    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION, AddToVector);
    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA, &result.headers);
    curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
    curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE_LARGE, size);
    EasyCurlSlist slist;
    for (auto &x : headers) {
        slist.Append(x.c_str());
    }
    curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, slist.slist);
#ifdef CTFTOOLSVERBOSE
    curl_easy_setopt(curl.get(), CURLOPT_VERBOSE, 1);
#endif
    curl_easy_perform(curl.get());
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &result.status_code);
    return std::move(result);
}

ctf::easy::Session::Session() {
}

ctf::easy::HttpResponse ctf::easy::Session::Get(const std::string& url, const std::vector<std::string>& headers) {
    HttpResponse result;
    if (!cookies.empty()) {
        std::string cookie_header;
        DumpCookies(cookie_header);
        result = ctf::easy::Get(url, {cookie_header});
    }
    else {
        result = ctf::easy::Get(url);
    }
    LoadCookies(result.headers);
    return std::move(result);
}

ctf::easy::HttpResponse ctf::easy::Session::Post(const std::string& url, const char* data, size_t size, const std::vector<std::string>& headers) {
    HttpResponse result;
    if (!cookies.empty()) {
        std::string cookie_header;
        DumpCookies(cookie_header);
        result = ctf::easy::Post(url, data, size, {cookie_header});
    }
    else {
        result = ctf::easy::Post(url, data, size);
    }
    LoadCookies(result.headers);
    return std::move(result);
}

void ctf::easy::Session::DumpCookies(std::string& cookie_header) {
    cookie_header = "Cookie: ";
    if (cookies.empty()) {
        return;
    }
    auto it = cookies.begin();
    while (true) {
        auto it_plus_one = it;
        ++it_plus_one;
        cookie_header += it->first + "=" + it->second;
        if (it_plus_one != cookies.end()) {
            cookie_header += "; ";
            it = it_plus_one;
        }
        else {
            break;
        }
    }
}

void ctf::easy::Session::LoadCookies(const std::vector<std::string>& headers) {
    for (auto &x : headers) {
        if (x.substr(0, 12) == "Set-Cookie: ") {
            size_t name_start = x.find_first_of(' ');
            size_t name_end = x.find_first_of('=');
            size_t cookie_end = x.find_first_of(';');
            if (name_start == std::string::npos || name_end == std::string::npos || cookie_end == std::string::npos) {
                continue;
            }
            ++name_start;
            size_t cookie_start = name_end + 1;
            if (cookie_start > cookie_end || name_start > name_end) {
                continue;
            }
            cookies[x.substr(name_start, name_end - name_start)] = x.substr(cookie_start, cookie_end - cookie_start);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------------
// easy ~~ Random subsection

int32_t ctf::easy::Random::i32() {
    uint64_t t = GetInstance().rnd_();
    return *reinterpret_cast<int32_t*>(&t);
}

uint32_t ctf::easy::Random::u32() {
    uint64_t t = GetInstance().rnd_();
    return *reinterpret_cast<uint32_t*>(&t);
}

int64_t ctf::easy::Random::i64() {
    uint64_t t = GetInstance().rnd_();
    return *reinterpret_cast<int64_t*>(&t);
}

uint64_t ctf::easy::Random::u64() {
    return GetInstance().rnd_();
}

char ctf::easy::Random::AlNum() {
    return ctf::easy::Random::alnum_[GetInstance().i32() % 62];
}

std::string ctf::easy::Random::AlNumString(size_t length) {
    std::string result(length, ' ');
    ctf::easy::Random& r = GetInstance();
    for (auto &x : result) {
        x = ctf::easy::Random::alnum_[r.u64() % 62];
    }
    return result;
}

ctf::easy::Random& ctf::easy::Random::GetInstance() {
    static ctf::easy::Random instance;
    return instance;
}

ctf::easy::Random::Random() : rnd_(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {
}

//-------------------------------------------------------------------------------------------------------------------------------
// easy ~~ ThreadPool subsection

ctf::easy::SpinThreadPool::SpinThreadPool(int count) : count_(count) {
}

void ctf::easy::SpinThreadPool::Start(std::function<SpinThreadPoolStepStatus(int)> fun) {
    auto wrapped_runner = [=](int ind) {
        while (true) {
            auto status = fun(ind);
            if (status == ctf::easy::SpinThreadPoolStepStatus::Stop || terminated_) {
                break;
            }
        }
    };

    for (int i = 0; i < count_; ++i) {
        threads_.emplace_back(std::thread(wrapped_runner, i));
    }
}

void ctf::easy::SpinThreadPool::Wait() {
    for (auto &x : threads_) {
        x.join();
    }
}

void ctf::easy::SpinThreadPool::Stop() {
    terminated_ = true;
}

//-------------------------------------------------------------------------------------------------------------------------------
// verbose
//-------------------------------------------------------------------------------------------------------------------------------
