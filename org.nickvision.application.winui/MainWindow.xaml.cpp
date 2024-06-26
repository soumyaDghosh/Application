#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <format>
#include <libnick/helpers/codehelpers.h>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/notifications/shellnotification.h>
#include <libnick/localization/gettext.h>
#include "SettingsPage.xaml.h"

using namespace ::Nickvision;
using namespace ::Nickvision::Events;
using namespace ::Nickvision::Notifications;
using namespace ::Nickvision::Application::Shared::Controllers;
using namespace ::Nickvision::Application::Shared::Models;
using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Dispatching;
using namespace winrt::Microsoft::UI::Input;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;
using namespace winrt::Microsoft::UI::Xaml::Input;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Windows::ApplicationModel::DataTransfer;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;
using namespace winrt::Windows::System;

namespace winrt::Nickvision::Application::WinUI::implementation 
{
    static std::vector<std::string> keys(const std::unordered_map<std::string, std::string>& m)
    {
        std::vector<std::string> k;
        for(std::unordered_map<std::string, std::string>::const_iterator it = m.begin(); it != m.end(); it++)
        {
            k.push_back(it->first);
        }
        return k;
    }

    MainWindow::MainWindow()
        : m_opened{ false },
        m_isActivated{ true },
        m_notificationClickToken{ 0 }
    {
        InitializeComponent();
        this->m_inner.as<::IWindowNative>()->get_WindowHandle(&m_hwnd);
        //Set TitleBar
        TitleBarTitle().Text(winrt::to_hstring(m_controller->getAppInfo().getShortName()));
        AppWindow().TitleBar().ExtendsContentIntoTitleBar(true);
        AppWindow().TitleBar().PreferredHeightOption(TitleBarHeightOption::Tall);
        AppWindow().TitleBar().ButtonBackgroundColor(Colors::Transparent());
        AppWindow().TitleBar().ButtonInactiveBackgroundColor(Colors::Transparent());
        AppWindow().Title(TitleBarTitle().Text());
        AppWindow().SetIcon(L"resources\\org.nickvision.application.ico");
        TitleBar().Loaded([&](const IInspectable sender, const RoutedEventArgs& args) { SetDragRegionForCustomTitleBar(); });
        TitleBar().SizeChanged([&](const IInspectable sender, const SizeChangedEventArgs& args) { SetDragRegionForCustomTitleBar(); });
        //Localize Strings
        TitleBarSearchBox().PlaceholderText(winrt::to_hstring(_("Search for Files")));
        NavViewHome().Content(winrt::box_value(winrt::to_hstring(_("Home"))));
        NavViewFolder().Content(winrt::box_value(winrt::to_hstring(_("Folder"))));
        NavViewHelp().Content(winrt::box_value(winrt::to_hstring(_("Help"))));
        ToolTipService::SetToolTip(BtnCheckForUpdates(), winrt::box_value(winrt::to_hstring(_("Check for Updates"))));
        ToolTipService::SetToolTip(BtnCopyDebugInfo(), winrt::box_value(winrt::to_hstring(_("Copy Debug Information"))));
        LblChangelog().Text(winrt::to_hstring(_("Changelog")));
        BtnGitHubRepo().Content(winrt::box_value(winrt::to_hstring(_("GitHub Repo"))));
        BtnReportABug().Content(winrt::box_value(winrt::to_hstring(_("Report a Bug"))));
        BtnDiscussions().Content(winrt::box_value(winrt::to_hstring(_("Discussions"))));
        LblCredits().Text(winrt::to_hstring(_("Credits")));
        NavViewSettings().Content(winrt::box_value(winrt::to_hstring(_("Settings"))));
        StatusPageHome().Description(winrt::to_hstring(_("Open a folder (or drag one into the app) to get started")));
        HomeOpenFolderButtonLabel().Text(winrt::to_hstring(_("Open Folder")));
        FolderOpenFolderButton().Label(winrt::to_hstring(_("Open")));
        ToolTipService::SetToolTip(FolderCloseFolderButton(), winrt::box_value(winrt::to_hstring(_("Close (Ctrl+W)"))));
    }

    void MainWindow::SetController(const std::shared_ptr<MainWindowController>& controller, ElementTheme systemTheme)
    {
        m_controller = controller;
        m_systemTheme = systemTheme;
        //Register Events
        AppWindow().Closing({ this, &MainWindow::OnClosing });
        m_controller->configurationSaved() += [&](const EventArgs& args) { OnConfigurationSaved(args); };
        m_controller->notificationSent() += [&](const NotificationSentEventArgs& args) { OnNotificationSent(args); };
        m_controller->shellNotificationSent() += [&](const ShellNotificationSentEventArgs& args) { OnShellNotificationSent(args); };
        m_controller->folderChanged() += [&](const EventArgs& args) { OnFolderChanged(args); };
        //Localize Strings
        NavView().PaneTitle(m_controller->isDevVersion() ? winrt::to_hstring(_("PREVIEW")) : L"");
        LblAppName().Text(winrt::to_hstring(m_controller->getAppInfo().getShortName()));
        LblAppDescription().Text(winrt::to_hstring(m_controller->getAppInfo().getDescription()));
        LblAppVersion().Text(winrt::to_hstring(m_controller->getAppInfo().getVersion().toString()));
        LblAppChangelog().Text(winrt::to_hstring(m_controller->getAppInfo().getChangelog()));
        if(m_controller->getAppInfo().getTranslatorNames().size() == 1 && m_controller->getAppInfo().getTranslatorNames()[0] == "translator-credits")
        {
            LblAppCredits().Text(winrt::to_hstring(std::vformat(_("Developers:\n{}\nDesigners:\n{}\nArtists:\n{}"), std::make_format_args(CodeHelpers::unmove(StringHelpers::join(keys(m_controller->getAppInfo().getDevelopers()), "\n")), CodeHelpers::unmove(StringHelpers::join(keys(m_controller->getAppInfo().getDesigners()), "\n")), CodeHelpers::unmove(StringHelpers::join(keys(m_controller->getAppInfo().getArtists()), "\n", false))))));

        }
        else
        {
            LblAppCredits().Text(winrt::to_hstring(std::vformat(_("Developers:\n{}\nDesigners:\n{}\nArtists:\n{}\nTranslators:\n{}"), std::make_format_args(CodeHelpers::unmove(StringHelpers::join(keys(m_controller->getAppInfo().getDevelopers()), "\n")), CodeHelpers::unmove(StringHelpers::join(keys(m_controller->getAppInfo().getDesigners()), "\n")), CodeHelpers::unmove(StringHelpers::join(keys(m_controller->getAppInfo().getArtists()), "\n")), CodeHelpers::unmove(StringHelpers::join(m_controller->getAppInfo().getTranslatorNames(), "\n", false))))));
        }
        StatusPageHome().Title(winrt::to_hstring(m_controller->getGreeting()));
    }

    void MainWindow::OnLoaded(const IInspectable& sender, const RoutedEventArgs& args)
    {
        if (!m_opened)
        {
            if (!m_controller)
            {
                throw std::logic_error("MainWindow::SetController() must be called before using the window.");
            }
            m_controller->connectTaskbar(m_hwnd);
            m_controller->startup();
            NavViewHome().IsSelected(true);
            m_opened = true;
        }
    }

    void MainWindow::OnClosing(const Microsoft::UI::Windowing::AppWindow& sender, const AppWindowClosingEventArgs& args)
    {
        //args.Cancel(true);
    }

    void MainWindow::OnActivated(const IInspectable& sender, const WindowActivatedEventArgs& args)
    {
        m_isActivated = args.WindowActivationState() != WindowActivationState::Deactivated;
        if(m_isActivated)
        {
            OnThemeChanged(MainGrid(), sender);
        }
        else
        {
            TitleBarTitle().Foreground(SolidColorBrush(Colors::Gray()));
            AppWindow().TitleBar().ButtonForegroundColor(Colors::Gray());
        }
    }

    void MainWindow::OnThemeChanged(const FrameworkElement& sender, const IInspectable& args)
    {
        switch(MainGrid().ActualTheme())
        {
        case ElementTheme::Light:
            TitleBarTitle().Foreground(SolidColorBrush(Colors::Black()));
            AppWindow().TitleBar().ButtonForegroundColor(Colors::Black());
            AppWindow().TitleBar().ButtonInactiveForegroundColor(Colors::Black());
            break;
        case ElementTheme::Dark:
            TitleBarTitle().Foreground(SolidColorBrush(Colors::White()));
            AppWindow().TitleBar().ButtonForegroundColor(Colors::White());
            AppWindow().TitleBar().ButtonInactiveForegroundColor(Colors::White());
            break;
        default:
            break;
        }
    }

    void MainWindow::OnDragOver(const IInspectable& sender, const DragEventArgs& args)
    {
        args.AcceptedOperation(DataPackageOperation::Copy | DataPackageOperation::Link);
        args.DragUIOverride().Caption(winrt::to_hstring(_("Drop here to open folder")));
        args.DragUIOverride().IsGlyphVisible(true);
        args.DragUIOverride().IsContentVisible(true);
        args.DragUIOverride().IsCaptionVisible(true);
    }

    Windows::Foundation::IAsyncAction MainWindow::OnDrop(const IInspectable& sender, const DragEventArgs& args)
    {
        if (args.DataView().Contains(StandardDataFormats::StorageItems()))
        {
            IVectorView<IStorageItem> items{ co_await args.DataView().GetStorageItemsAsync() };
            if (items.Size() > 0)
            {
                m_controller->openFolder(winrt::to_string(items.GetAt(0).Path()));
            }
        }
    }

    void MainWindow::OnConfigurationSaved(const EventArgs& args)
    {
        switch (m_controller->getTheme())
        {
        case Theme::Light:
            MainGrid().RequestedTheme(ElementTheme::Light);
            break;
        case Theme::Dark:
            MainGrid().RequestedTheme(ElementTheme::Dark);
            break;
        default:
            MainGrid().RequestedTheme(m_systemTheme);
            break;
        }
    }

    void MainWindow::OnNotificationSent(const NotificationSentEventArgs& args)
    {
        DispatcherQueue().TryEnqueue([this, args]()
        {
            InfoBar().Message(winrt::to_hstring(args.getMessage()));
            switch(args.getSeverity())
            {
            case NotificationSeverity::Success:
                InfoBar().Severity(InfoBarSeverity::Success);
                break;
            case NotificationSeverity::Warning:
                InfoBar().Severity(InfoBarSeverity::Warning);
                break;
            case NotificationSeverity::Error:
                InfoBar().Severity(InfoBarSeverity::Error);
                break;
            default:
                InfoBar().Severity(InfoBarSeverity::Informational);
                break;
            }
            if(m_notificationClickToken)
            {
                BtnInfoBar().Click(m_notificationClickToken);
            }
            if(args.getAction() == "error")
            {
                if(m_controller->isFolderOpened())
                {
                    NavViewFolder().IsSelected(true);
                }
                else
                {
                    NavViewHome().IsSelected(true);
                }
            }
            else if(args.getAction() == "update")
            {
                BtnInfoBar().Content(winrt::box_value(winrt::to_hstring(_("Update"))));
                m_notificationClickToken = BtnInfoBar().Click({ this, &MainWindow::WindowsUpdate });
            }
            else if(args.getAction() == "close")
            {
                BtnInfoBar().Content(winrt::box_value(winrt::to_hstring(_("Close"))));
                m_notificationClickToken = BtnInfoBar().Click({ this, &MainWindow::CloseFolder });
            }
            BtnInfoBar().Visibility(!args.getAction().empty() ? Visibility::Visible : Visibility::Collapsed);
            InfoBar().IsOpen(true);
        });
    }

    void MainWindow::OnShellNotificationSent(const ShellNotificationSentEventArgs& args)
    {
        ShellNotification::send(args, m_hwnd);
    }

    void MainWindow::OnNavSelectionChanged(const NavigationView& sender, const NavigationViewSelectionChangedEventArgs& args)
    {
        winrt::hstring tag{ NavView().SelectedItem().as<NavigationViewItem>().Tag().as<winrt::hstring>() };
        if(tag == L"Home")
        {
            ViewStack().CurrentPage(L"Home");
        }
        else if(tag == L"Folder")
        {
            ViewStack().CurrentPage(L"Folder");
        }
        else if(tag == L"Settings")
        {
            WinUI::SettingsPage page{ winrt::make<SettingsPage>() };
            page.as<SettingsPage>()->SetController(m_controller->createPreferencesViewController());
            ViewStack().CurrentPage(L"Custom");
            FrameCustom().Content(winrt::box_value(page));
        }
        TitleBarSearchBox().Visibility(tag == L"Folder" ? Visibility::Visible : Visibility::Collapsed);
        SetDragRegionForCustomTitleBar();
    }

    void MainWindow::OnNavViewItemTapped(const IInspectable& sender, const TappedRoutedEventArgs& args)
    {
        FlyoutBase::ShowAttachedFlyout(sender.as<FrameworkElement>());
    }

    void MainWindow::CheckForUpdates(const IInspectable& sender, const RoutedEventArgs& args)
    {
        FlyoutBase::GetAttachedFlyout(NavViewHelp().as<FrameworkElement>()).Hide();
        m_controller->checkForUpdates();
    }

    void MainWindow::WindowsUpdate(const IInspectable& sender, const RoutedEventArgs& args)
    {
        InfoBar().IsOpen(false);
        NavView().IsEnabled(false);
        TitleBarSearchBox().Visibility(Visibility::Collapsed);
        ViewStack().CurrentPage(L"Spinner");
        SetDragRegionForCustomTitleBar();
        m_controller->windowsUpdate();
    }

    void MainWindow::CopyDebugInformation(const IInspectable& sender, const RoutedEventArgs& args)
    {
        FlyoutBase::GetAttachedFlyout(NavViewHelp().as<FrameworkElement>()).Hide();
        DataPackage dataPackage;
        dataPackage.SetText(winrt::to_hstring(m_controller->getDebugInformation()));
        Clipboard::SetContent(dataPackage);
        OnNotificationSent({ _("Debug information copied to clipboard."), NotificationSeverity::Success });
    }

    Windows::Foundation::IAsyncAction MainWindow::GitHubRepo(const IInspectable& sender, const RoutedEventArgs& args)
    {
        co_await Launcher::LaunchUriAsync(Windows::Foundation::Uri{ winrt::to_hstring(m_controller->getAppInfo().getSourceRepo()) });
    }

    Windows::Foundation::IAsyncAction MainWindow::ReportABug(const IInspectable& sender, const RoutedEventArgs& args)
    {
        co_await Launcher::LaunchUriAsync(Windows::Foundation::Uri{ winrt::to_hstring(m_controller->getAppInfo().getIssueTracker()) });
    }

    Windows::Foundation::IAsyncAction MainWindow::Discussions(const IInspectable& sender, const RoutedEventArgs& args)
    {
        co_await Launcher::LaunchUriAsync(Windows::Foundation::Uri{ winrt::to_hstring(m_controller->getAppInfo().getSupportUrl()) });
    }

    void MainWindow::OnFolderChanged(const EventArgs& args)
    {
        NavViewHome().IsSelected(!m_controller->isFolderOpened());
        NavViewFolder().IsEnabled(m_controller->isFolderOpened());
        NavViewFolder().IsSelected(m_controller->isFolderOpened());
        ListFiles().Items().Clear();
        if(m_controller->isFolderOpened())
        {
            StatusPageFiles().Description(winrt::to_hstring(std::vformat(_n("There is {} file in the folder.", "There are {} files in the folder.", m_controller->getFiles().size()), std::make_format_args(CodeHelpers::unmove(m_controller->getFiles().size())))));
            for(const std::filesystem::path& file : m_controller->getFiles())
            {
                StackPanel fileRow;
                fileRow.Margin({ 6, 6, 6, 6 });
                fileRow.Orientation(Orientation::Vertical);
                fileRow.Spacing(6);
                TextBlock lblFilename;
                lblFilename.Text(winrt::to_hstring(file.filename().string()));
                TextBlock lblPath;
                lblPath.Text(winrt::to_hstring(file.string()));
                lblPath.Foreground(SolidColorBrush(Colors::Gray()));
                fileRow.Children().Append(lblFilename);
                fileRow.Children().Append(lblPath);
                ListFiles().Items().Append(fileRow);
            }
        }
    }

    Windows::Foundation::IAsyncAction MainWindow::OpenFolder(const IInspectable& sender, const RoutedEventArgs& args)
    {
        FolderPicker picker;
        picker.as<::IInitializeWithWindow>()->Initialize(m_hwnd);
        picker.FileTypeFilter().Append(L"*");
        StorageFolder folder{ co_await picker.PickSingleFolderAsync() };
        if(folder)
        {
            m_controller->openFolder(winrt::to_string(folder.Path()));
        }
    }

    void MainWindow::CloseFolder(const IInspectable& sender, const RoutedEventArgs& args)
    {
        InfoBar().IsOpen(false);
        m_controller->closeFolder();
    }

    void MainWindow::SetDragRegionForCustomTitleBar()
    {
        double scaleAdjustment{ TitleBar().XamlRoot().RasterizationScale() };
        RightPaddingColumn().Width({ AppWindow().TitleBar().RightInset() / scaleAdjustment });
        LeftPaddingColumn().Width({ AppWindow().TitleBar().LeftInset() / scaleAdjustment });
        GeneralTransform transformSearch{ TitleBarSearchBox().TransformToVisual(nullptr) };
        Windows::Foundation::Rect boundsSearch{ transformSearch.TransformBounds({ 0, 0, static_cast<float>(TitleBarSearchBox().ActualWidth()), static_cast<float>(TitleBarSearchBox().ActualHeight()) }) };
        RectInt32 searchBoxRect{ static_cast<int>(std::round(boundsSearch.X * scaleAdjustment)), static_cast<int>(std::round(boundsSearch.Y * scaleAdjustment)), static_cast<int>(std::round(boundsSearch.Width * scaleAdjustment)), static_cast<int>(std::round(boundsSearch.Height * scaleAdjustment)) };
        RectInt32 rectArray[1]{ searchBoxRect };
        InputNonClientPointerSource nonClientInputSrc{ InputNonClientPointerSource::GetForWindowId(AppWindow().Id()) };
        nonClientInputSrc.SetRegionRects(NonClientRegionKind::Passthrough, rectArray);
    }
}
