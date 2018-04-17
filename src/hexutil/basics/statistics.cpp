#include "common.h"

#include "hexutil/basics/statistics.h"


namespace hex {

static std::unordered_map<Counter *, Atom> counters;
static std::vector<std::pair<Atom, Counter::counter_type> > pending;
static time_t last_log = std::time(NULL);

void register_counter(Counter *counter, const Atom name) {
    counters[counter] = name;
}

void register_counter(Counter *counter, const Counter *existing) {
    auto found = counters.find(const_cast<Counter *>(existing));
    if (found == counters.end())
        return;
    register_counter(counter, found->second);
}

void unregister_counter(Counter *counter) {
    auto found = counters.find(counter);
    if (found == counters.end())
        return;

    Counter::counter_type value = counter->reset();
    if (value != 0) {
        std::pair<Atom, Counter::counter_type> p("", value);
        pending.push_back(p);
        std::swap(pending.rbegin()->first, found->second);
    }

    counters.erase(found);
}

void log_statistics(const std::string& label) {
    time_t seconds = std::time(NULL) - last_log;

    std::map<Atom, Counter::counter_type> summary;
    for (auto iter = counters.begin(); iter != counters.end(); iter++) {
        Counter::counter_type value = iter->first->reset();
        if (value == 0)
            continue;

        auto found = summary.find(iter->second);
        if (found == summary.end()) {
            summary[iter->second] = value;
        } else {
            found->second += value;
        }
    }
    for (auto iter = pending.begin(); iter != pending.end(); iter++) {
        auto found = summary.find(iter->first);
        if (found == summary.end()) {
            summary[iter->first] = iter->second;
        } else {
            found->second += iter->second;
        }
    }
    pending.clear();

    std::ostringstream ss;
    ss << "Statistics (over " << seconds << "s; " << label << "):";
    for (auto iter = summary.begin(); iter != summary.end(); iter++) {
        ss << "\n" << iter->first << " = " << iter->second;
    }
    BOOST_LOG_TRIVIAL(info) << ss.str();

    last_log = std::time(NULL);
}

};
