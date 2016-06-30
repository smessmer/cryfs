#include "FakeHttpClient.h"

using std::string;
using boost::optional;
using boost::none;

namespace cpputils {
    FakeHttpClient::FakeHttpClient(): _sites() {
    }

    void FakeHttpClient::addWebsite(const string &url, const string &content) {
        _sites[url] = content;
    }

    optional<string> FakeHttpClient::get(const string &url, optional<long> timeoutMsec) {
        UNUSED(timeoutMsec);
        auto found = _sites.find(url);
        if (found == _sites.end()) {
            return none;
        }
        return found->second;
    }
}
