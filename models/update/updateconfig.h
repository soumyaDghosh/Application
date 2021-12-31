#ifndef UPDATECONFIG_H
#define UPDATECONFIG_H

#include <string>
#include <optional>
#include "version.h"

namespace NickvisionApplication::Models::Update
{
    class UpdateConfig
    {
    public:
        UpdateConfig();
        static std::optional<UpdateConfig> loadFromUrl(const std::string& url);
        Version getLatestVersion() const;
        void setLatestVersion(const Version& latestVersion);
        std::string getChangelog() const;
        void setChangelog(const std::string& changelog);

    private:
        Version m_latestVersion;
        std::string m_changelog;
    };
}

#endif // UPDATECONFIG_H