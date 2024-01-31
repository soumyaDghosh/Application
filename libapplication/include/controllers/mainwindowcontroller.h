#if (defined(_WIN32) && !defined(_CRT_SECURE_NO_WARNINGS))
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <libnick/app/appinfo.h>
#include <libnick/events/event.h>
#include <libnick/notifications/notificationsenteventargs.h>
#include <libnick/notifications/shellnotificationsenteventargs.h>
#include <libnick/taskbar/taskbaritem.h>
#include <libnick/update/updater.h>
#include "controllers/preferencesviewcontroller.h"
#include "models/theme.h"

namespace Nickvision::Application::Shared::Controllers
{
    /**
     * @brief A controller for a MainWindow.
     */
    class MainWindowController
    {
    public:
        /**
         * @brief Constructs a MainWindowController.
         * @param args A list of argument strings for the application
         */
        MainWindowController();
        /**
         * @brief Gets the AppInfo object for the application
         * @return The current AppInfo object
         */
        Nickvision::App::AppInfo& getAppInfo() const;
        /**
         * @brief Gets whether or not the specified version is a development (preview) version.
         * @return True for preview version, else false
         */
        bool isDevVersion() const;
        /**
         * @brief Gets the preferred theme for the application.
         * @return The preferred theme
         */
        Models::Theme getTheme() const;
        /**
         * @brief Gets the Saved event for the application's configuration.
         * @return The configuration Saved event
         */
        Nickvision::Events::Event<Nickvision::Events::EventArgs>& configurationSaved();
        /**
         * @brief Gets the event for when a notification is sent.
         * @return The notification sent event
         */
        Nickvision::Events::Event<Nickvision::Notifications::NotificationSentEventArgs>& notificationSent();
        /**
         * @brief Gets the event for when a shell notification is sent.
         * @return The shell notification sent event
         */
        Nickvision::Events::Event<Nickvision::Notifications::ShellNotificationSentEventArgs>& shellNotificationSent();
        /**
         * @brief Gets the debugging information for the application.
         * @param extraInformation Extra, ui-specific, information to include in the debug info statement
         * @return The application's debug information
         */
        std::string getDebugInformation(const std::string& extraInformation = "") const;
        /**
         * @brief Gets the path of the current open folder.
         * @return The open folder path. Empty if no folder is open
         */
        const std::filesystem::path& getFolderPath() const;
        /**
         * @brief Gets the list of paths of files in the open folder.
         * @return The list of file paths in the open folder
         */
        const std::vector<std::filesystem::path>& getFiles() const;
        /**
         * @brief Gets whether or not a folder is opened.
         * @return True if folder is opened, else false
         */
        bool isFolderOpened() const;
        /**
         * @brief Gets the event for when the folder is changed (opened or closed).
         * @return The folder changed event
         */
        Nickvision::Events::Event<Nickvision::Events::EventArgs>& folderChanged();
        /**
         * @brief Gets the string for greeting on the home page.
         * @return The greeting string
         */
        std::string getGreeting() const;
        /**
         * @brief Gets a PreferencesViewController.
         * @return The PreferencesViewController
         */
        std::shared_ptr<PreferencesViewController> createPreferencesViewController() const;
        /**
         * @brief Starts the application.
         * @brief Will only have an effect on the first time called.
         */
        void startup();
        /**
         * @brief Checks for an application update and sends a notification if one is available.
         */
        void checkForUpdates();
#ifdef _WIN32
        /**
         * @brief Downloads and installs the latest application update in the background.
         * @brief Will send a notification if the update fails.
         * @brief MainWindowController::checkForUpdates() must be called before this method.
         */
        void windowsUpdate();
        /**
         * @brief Connects the main window to the taskbar interface.
         * @param hwnd The main window handle
         * @return True if connection successful, else false
         */
        bool connectTaskbar(HWND hwnd);
#elif defined(__linux__)
        /**
         * @brief Connects the application to the taskbar interface.
         * @param desktopFile The desktop file name (with the extension) of the running application
         * @return True if connection successful, else false
         */
        bool connectTaskbar(const std::string& desktopFile);
#endif
        /**
         * @brief Opens a folder.
         * @param path The path of the file to open
         * @return True if the folder was successfully opened, else false
         */
        bool openFolder(const std::filesystem::path& path);
        /**
         * @brief Closes a folder, if one is open.
         */
        void closeFolder();

    private:
        /**
         * @brief Obtains the paths of files in an open folder for the files list.
         * @brief This method only scans the top-level directory for files.
         * @brief Other sub-directory paths are not added to the files list.
         */
        void loadFiles();
        std::shared_ptr<Nickvision::Update::Updater> m_updater;
        Nickvision::Taskbar::TaskbarItem m_taskbar;
        Nickvision::Events::Event<Nickvision::Notifications::NotificationSentEventArgs> m_notificationSent;
        Nickvision::Events::Event<Nickvision::Notifications::ShellNotificationSentEventArgs> m_shellNotificationSent;
        std::filesystem::path m_folderPath;
        std::vector<std::filesystem::path> m_files;
        Nickvision::Events::Event<Nickvision::Events::EventArgs> m_folderChanged;
    };
}

#endif //MAINWINDOWCONTROLLER_H