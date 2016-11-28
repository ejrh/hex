#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/loader.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/messaging/builtin_messages.h"


void MessageLoader::receive(Message *msg) {
    switch (msg->type) {
        case IncludeResource: {
            auto upd = dynamic_cast<IncludeResourceMessage *>(msg);
            include(upd->data);
        } break;

        case IncludeIfResourceExists: {
            auto upd = dynamic_cast<IncludeIfResourceExistsMessage *>(msg);
            include(upd->data, true);
        } break;

        default: {
            handle_message(msg);
        }
    }
}

void MessageLoader::load(const std::string& filename) {
    BOOST_LOG_TRIVIAL(info) << "Loading resources from: " << filename;
    current_files.push_back(filename);
    replay_messages(filename, *this);
    current_files.pop_back();
}

void MessageLoader::include(const std::string& filename, bool skip_missing) {
    boost::filesystem::path path = boost::filesystem::path(current_files.back()).parent_path();
    path /= boost::filesystem::path(filename);
    std::string relative_filename = path.string();

    if (std::find(current_files.begin(), current_files.end(), relative_filename) != current_files.end()) {
        BOOST_LOG_TRIVIAL(warning) << "Self-inclusion of resource file will be ignored: %s" << relative_filename;
        return;
    }

    if (skip_missing && !boost::filesystem::exists(boost::filesystem::path(relative_filename))) {
        BOOST_LOG_TRIVIAL(warning) << "Skipping missing resource: " << relative_filename;
        return;
    }

    BOOST_LOG_TRIVIAL(info) << "Including resources from: " << filename;
    current_files.push_back(relative_filename);
    replay_messages(relative_filename, *this);
    current_files.pop_back();
}

const std::string& MessageLoader::get_current_file() const {
    return current_files.back();
}
