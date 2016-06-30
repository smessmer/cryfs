#include "utils.h"
#include <algorithm>
#include <string>

using std::pair;
using std::make_pair;
using std::vector;
using std::string;

namespace cryfs {
    namespace program_options {
        pair<vector<string>, vector<string>> splitAtDoubleDash(const vector<string> &options) {
            auto doubleDashIterator = std::find(options.begin(), options.end(), string("--"));
            vector<string> beforeDoubleDash(options.begin(), doubleDashIterator);
            vector<string> afterDoubleDash;
            afterDoubleDash.reserve(options.size()-beforeDoubleDash.size());
            if (options.end() >= doubleDashIterator+1) {
                std::copy(doubleDashIterator + 1, options.end(), std::back_inserter(afterDoubleDash));
            }
            return make_pair(
                    beforeDoubleDash,
                    afterDoubleDash
            );
        }

    }
}
