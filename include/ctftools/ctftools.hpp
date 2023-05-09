#include <unordered_map>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <functional>
#include <mutex>
#include <atomic>

#define DO_RULE_OF_5_DEFAULT(type)                \
    type() = default;                             \
    type(const type& other) = default;            \
    type(type&& other) = default;                 \
    type& operator=(const type& other) = default; \
    type& operator=(type&& other) = default;      \

#define DO_RULE_OF_5_NO_COPY(type)                \
    type(const type& other) = delete;             \
    type(type&& other) = delete;                  \
    type& operator=(const type& other) = delete;  \
    type& operator=(type&& other) = delete;       \

namespace ctf {
    // TODO : add crypto

    namespace fast {
        // TODO : (much later) Split easy into submethods, optimize it
        // (e.g. Only retrieve body from http request and pass it as const char* level)
    };

    namespace easy {
        struct HttpResponse {
            long status_code;
            std::vector<std::string> headers;
            std::string body;

            DO_RULE_OF_5_DEFAULT(HttpResponse)
        };

        HttpResponse Get(const std::string& url, const std::vector<std::string>& headers = {});
        HttpResponse Post(const std::string& url, const char* data, size_t size, const std::vector<std::string>& headers = {});

        class Session {
        public:
            DO_RULE_OF_5_NO_COPY(Session)

            explicit Session();

            HttpResponse Get(const std::string& url, const std::vector<std::string>& headers = {});
            HttpResponse Post(const std::string& url, const char* data, size_t size, const std::vector<std::string>& headers = {});

            void DumpCookies(std::string& cookie_header);

        private:
            void LoadCookies(const std::vector<std::string>& headers);

            // TODO : Currently only support for HttpOnly cookies, no research was done for other types
            // so they are ignored on encounter
            // TODO : No support for additional parameters currently
            // TODO : No idea what it's supposed to do when cookie/value is empty => I just skip it for now
            std::unordered_map<std::string, std::string> cookies;
        };

        class Random {
        public:
            DO_RULE_OF_5_NO_COPY(Random)

            static int32_t i32();
            static uint32_t u32();
            static int64_t i64();
            static uint64_t u64();
            static char AlNum();

            // TODO : Surely something useful here

            static std::string AlNumString(size_t length);

            static Random& GetInstance();

        private:
            explicit Random();
            std::mt19937_64 rnd_;
            static constexpr char alnum_[63] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        };

        class StaticRandom {
            // TODO : Random that gives the same result every time
        };

        // TODO : Rename so there's no illusion it's spinlock based
        // TODO : Add a runner that is not step-based
        enum class SpinThreadPoolStepStatus {
            Success = 0,
            Retry = 1,
            Stop = 2
        };

        class SpinThreadPool {
        public:
            DO_RULE_OF_5_NO_COPY(SpinThreadPool)

            SpinThreadPool(int count);

            // TODO : Maybe return Args... to pass to the function
            void Start(std::function<SpinThreadPoolStepStatus(int)> fun);

            // Has to be called after Stop().
            // Else it'll wait indefinitely
            // No check for the sake of easy-style interface
            void Wait();

            void Stop();

        private:
            // TODO : add shared threads support across multiple pools ?
            // TODO : add launch of thread on Creation of pool, and then give the tasks to the threads ?
            int count_;
            std::vector<std::thread> threads_;
            std::atomic<bool> terminated_ = false;
            // TODO : Gather statistics for each thread
        };
    }

    namespace verbose {
        // TODO : (much later) Add debug info to easy methods. More info, less speed, easier to debug
        // (e.g. check for success of curl calls, maybe return CURL_CODE, smth like that)
    }
};

//-------------------------------------------------------------------------------------------------------------------------------
// easy ~~ Template definitions
