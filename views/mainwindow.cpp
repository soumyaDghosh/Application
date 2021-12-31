#include "mainwindow.h"
#include "../models/configuration.h"
#include "../models/update/updater.h"
#include "settingsdialog.h"

namespace NickvisionApplication::Views
{
    using namespace NickvisionApplication::Models;
    using namespace NickvisionApplication::Models::Update;

    MainWindow::MainWindow()
    {
        //==Settings==//
        set_default_size(800, 600);
        set_title("NickvisionApplication");
        set_titlebar(m_headerBar);
        //==HeaderBar==//
        m_headerBar.getSignalClickedOpenFolder().connect(sigc::mem_fun(*this, &MainWindow::openFolder));
        m_headerBar.getSignalClickedSettings().connect(sigc::mem_fun(*this, &MainWindow::settings));
        m_headerBar.getSignalActivateCheckForUpdates().connect(sigc::mem_fun(*this, &MainWindow::checkForUpdates));
        m_headerBar.getSignalActivateGitHubRepo().connect(sigc::mem_fun(*this, &MainWindow::gitHubRepo));
        m_headerBar.getSignalActivateReportABug().connect(sigc::mem_fun(*this, &MainWindow::reportABug));
        m_headerBar.getSignalActivateChangelog().connect(sigc::mem_fun(*this, &MainWindow::changelog));
        m_headerBar.getSignalActivateAbout().connect(sigc::mem_fun(*this, &MainWindow::about));
        //==Name Field==//
        Gtk::Label lblName("Name", Gtk::Align::START);
        lblName.set_margin_start(6);
        lblName.set_margin_top(6);
        m_txtName.set_margin(6);
        m_txtName.set_placeholder_text("Enter name here");
        //==Layout==//
        Gtk::Box mainBox(Gtk::Orientation::VERTICAL);
        mainBox.append(m_infoBar);
        mainBox.append(lblName);
        mainBox.append(m_txtName);
        set_child(mainBox);
        maximize();
        //==Load Config==//
        Configuration configuration;
        if(configuration.isFirstTimeOpen())
        {
            configuration.setIsFirstTimeOpen(false);
        }
        configuration.save();
    }

    MainWindow::~MainWindow()
    {
        //==Save Config==//
        Configuration configuration;
        configuration.save();
    }

    void MainWindow::openFolder()
    {
        Gtk::FileChooserDialog* folderDialog = new Gtk::FileChooserDialog(*this, "Select Folder", Gtk::FileChooserDialog::Action::SELECT_FOLDER, true);
        folderDialog->set_modal(true);
        folderDialog->add_button("_Select", Gtk::ResponseType::OK);
        folderDialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
        folderDialog->signal_response().connect(sigc::bind([&](int response, Gtk::FileChooserDialog* dialog)
        {
            if(response == Gtk::ResponseType::OK)
            {
                set_title("NickvisionApplication (" + dialog->get_file()->get_path() + ")");
            }
            delete dialog;
        }, folderDialog));
        folderDialog->show();
    }

    void MainWindow::settings()
    {
        SettingsDialog* settingsDialog = new SettingsDialog(*this);
        settingsDialog->signal_hide().connect(sigc::bind([](SettingsDialog* dialog)
        {
            delete dialog;
        }, settingsDialog));
        settingsDialog->show();
    }

    void MainWindow::checkForUpdates(const Glib::VariantBase& args)
    {
        Updater updater("https://raw.githubusercontent.com/nlogozzo/NickvisionApplication/main/UpdateConfig.json", { "2021.12.0" });
        m_infoBar.showMessage("Please Wait", "Checking for updates...", false);
        updater.checkForUpdates();
        m_infoBar.hide();
        if(updater.updateAvailable())
        {
            Gtk::MessageDialog* updateDialog = new Gtk::MessageDialog(*this, "Update Available", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK, true);
            updateDialog->set_secondary_text("\n===V" + updater.getLatestVersion()->toString() + " Changelog===\n" + updater.getChangelog() + "\n\nPlease visit the GitHub repo or update through your package manager to get the latest version.");
            updateDialog->signal_response().connect(sigc::bind([](int response, Gtk::MessageDialog* dialog)
            {
               delete dialog;
            }, updateDialog));
            updateDialog->show();
        }
        else
        {
            m_infoBar.showMessage("No Update Available", "There is no update at this time. Please check again later.");
        }
    }

    void MainWindow::gitHubRepo(const Glib::VariantBase& args)
    {
        system("xdg-open https://github.com/nlogozzo/NickvisionApplication");
    }

    void MainWindow::reportABug(const Glib::VariantBase& args)
    {
        system("xdg-open https://github.com/nlogozzo/NickvisionApplication/issues/new");
    }

    void MainWindow::changelog(const Glib::VariantBase& args)
    {
        Gtk::MessageDialog* changelogDialog = new Gtk::MessageDialog(*this, "What's New?", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK, true);
        changelogDialog->set_secondary_text("\n- Initial Release");
        changelogDialog->set_default_size(300, -1);
        changelogDialog->signal_response().connect(sigc::bind([](int response, Gtk::MessageDialog* dialog)
        {
           delete dialog;
        }, changelogDialog));
        changelogDialog->show();
    }

    void MainWindow::about(const Glib::VariantBase& args)
    {
        Gtk::AboutDialog* aboutDialog = new Gtk::AboutDialog();
        aboutDialog->set_transient_for(*this);
        aboutDialog->set_modal(true);
        aboutDialog->set_hide_on_close(true);
        aboutDialog->set_program_name("NickvisionApplication");
        aboutDialog->set_version("2021.12.0");
        aboutDialog->set_comments("A template for creating Nickvision applications.");
        aboutDialog->set_copyright("(C) Nickvision 2021");
        aboutDialog->set_license_type(Gtk::License::GPL_3_0);
        aboutDialog->set_website("https://github.com/nlogozzo");
        aboutDialog->set_website_label("GitHub");
        aboutDialog->set_authors({ "Nicholas Logozzo" });
        aboutDialog->signal_hide().connect(sigc::bind([](Gtk::AboutDialog* dialog)
        {
           delete dialog;
        }, aboutDialog));
        aboutDialog->show();
    }
}