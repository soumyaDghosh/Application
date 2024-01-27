#ifndef SETTINGSROW_H
#define SETTINGSROW_H

#include "includes.h"
#include "Controls/SettingsRow.g.h"

namespace winrt::Nickvision::Application::WinUI::Controls::implementation 
{
    /**
     * @brief A row control to display a configurable application setting. 
     */
    class SettingsRow : public SettingsRowT<SettingsRow>
    {
    public:
        /**
         * @brief Constructs a SettingsRow. 
         */
        SettingsRow();
        /**
         * @brief Gets the icon glyph for the row.
         * @returns The row icon glyph
         */
        winrt::hstring Glyph() const noexcept;
        /**
         * @brief Sets the icon glyph for the row.
         * @param glyph The new icon glyph
         */
        void Glyph(const winrt::hstring& glyph) noexcept;
        /**
         * @brief Gets the title for the row.
         * @return The row title 
         */
        winrt::hstring Title() const noexcept;
        /**
         * @brief Sets the title for the row.
         * @param title The new title 
         */
        void Title(const winrt::hstring& title) noexcept;
        /**
         * @brief Gets the description for the row.
         * @return The row description 
         */
        winrt::hstring Description() const noexcept;
        /**
         * @brief Sets the description for the row. 
         * @param description The new description
         */
        void Description(const winrt::hstring& description) noexcept;
        /**
         * @brief Gets the child for the row.
         * @returns The row child
         */
        IInspectable Child() const noexcept;
        /**
         * @brief Sets the child for the row.
         * @param child The new child 
         */
        void Child(const IInspectable& child) noexcept;
        /**
         * @brief Subscribes a handler to the property changed event.
         * @return The token for the newly subscribed handler.
         */
        winrt::event_token PropertyChanged(const Microsoft::UI::Xaml::Data::PropertyChangedEventHandler& handler) noexcept;
        /**
         * @brief Unsubscribes a handler from the property changed event.
         * @param token The token of the handler to unsubscribe. 
         */
        void PropertyChanged(const winrt::event_token& token) noexcept;

    private:
        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;

    public:
        /**
         * @brief Gets the row's glyph dependency property.
         * @return The glyph dependency property 
         */
        static const Microsoft::UI::Xaml::DependencyProperty& GlyphProperty() noexcept;
        /**
         * @brief Gets the row's title dependency property.
         * @return The title dependency property 
         */
        static const Microsoft::UI::Xaml::DependencyProperty& TitleProperty() noexcept;
        /**
         * @brief Gets the row's description dependency property.
         * @return The description dependency property 
         */
        static const Microsoft::UI::Xaml::DependencyProperty& DescriptionProperty() noexcept;
        /**
         * @brief Gets the row's child dependency property.
         * @return The child dependency property 
         */
        static const Microsoft::UI::Xaml::DependencyProperty& ChildProperty() noexcept;
        /**
         * @brief Handles when a property of the row is changed.
         * @param d Microsoft::UI::Xaml::DependencyObject
         * @param args Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs
         */
        static void OnPropertyChanged(const Microsoft::UI::Xaml::DependencyObject& d, const Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs& args);

    private:
        static Microsoft::UI::Xaml::DependencyProperty m_glyphProperty;
        static Microsoft::UI::Xaml::DependencyProperty m_titleProperty;
        static Microsoft::UI::Xaml::DependencyProperty m_descriptionProperty;
        static Microsoft::UI::Xaml::DependencyProperty m_childProperty;
    };
}

namespace winrt::Nickvision::Application::WinUI::Controls::factory_implementation 
{
    class SettingsRow : public SettingsRowT<SettingsRow, implementation::SettingsRow>
    {

    };
}

#endif //SETTINGSROW_H