#include <core/Godot.hpp>
#include <Reference.hpp>
#include "NativeDialogs/NativeDialog.h"
#include <codecvt>
#include <locale>
#include <iostream>
#include <string>
using namespace godot;

namespace godot {

    class GDNativeDialog : public Reference {
        GODOT_SUBCLASS(GDNativeDialog, Reference);
    public:
        GDNativeDialog(): m_pDialog(nullptr) { }

        virtual void show() {
            m_pDialog->setTitle(str2std(title));
            m_pDialog->show();
        }

        void set_title(String p_title) { title = p_title; }
        String get_title() const { return title; }

    protected:
        String title;
        NativeDialog::Dialog * m_pDialog;

        std::string str2std(const String& str) const {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            return conv.to_bytes(str.unicode_str());
        }

        virtual void _bind_callbacks() {

            m_pDialog->setDecideHandler([this](const NativeDialog::Dialog&){
                this->emit_signal("confirmed");
            });

            m_pDialog->setCancelHandler([this](const NativeDialog::Dialog&){
                this->emit_signal("canceled");
            });
        }

    public:
        static void _register_methods() {
            register_method("set_title", &GDNativeDialog::set_title);
            register_method("get_title", &GDNativeDialog::get_title);
            register_method("show", &GDNativeDialog::show);
            register_property<GDNativeDialog, String>("title", &GDNativeDialog::title, "");
			godot::String confirmed = "confirmed";
            godot::String canceled = "canceled";
            register_signal<GDNativeDialog>(confirmed, {});
            register_signal<GDNativeDialog>(canceled, {});
        }

    };

    class NativeMessageDialog : public  GDNativeDialog {
        GODOT_SUBCLASS(NativeMessageDialog, GDNativeDialog);

    public:
        NativeMessageDialog():GDNativeDialog() {
            m_pDialog = new NativeDialog::MessageDialog("", "", {});
            _bind_callbacks();
        }
        ~NativeMessageDialog() {
            delete (static_cast<NativeDialog::MessageDialog*>(m_pDialog));
        }
        virtual void show() override {
            auto dialog = dynamic_cast<NativeDialog::MessageDialog*>(m_pDialog);
            if (dialog) {
                dialog->setMessage(str2std(message));
                std::vector<std::string> vbtns;
                for (int i=0; i < buttons.size(); ++i) {
                    vbtns.push_back(str2std(buttons[i]));
                }
                dialog->setButtons(vbtns);
            }
            GDNativeDialog::show();
        }

        String get_response_button_title() const {
            String ss;
            if(auto dialog = dynamic_cast<NativeDialog::MessageDialog*>(m_pDialog)) {
                ss = String(dialog->responseButtonTitle().c_str());
            }
            return ss;
        }

        int get_response_button_index() const {
            if(auto dialog = dynamic_cast<NativeDialog::MessageDialog*>(m_pDialog))
                return dialog->responseButtonIndex();
            return -1;
        }

        void set_message(String p_message) { message = p_message; }
        String get_message() const { return message; }

        void set_buttons(Array p_buttons) { buttons = p_buttons; }
        Array get_buttons() const { return buttons; }


    protected:
        String message;
        Array buttons;

    public:
        static void _register_methods() {
            register_method("set_message", &NativeMessageDialog::set_message);
            register_method("get_message", &NativeMessageDialog::get_message);
            register_method("set_buttons", &NativeMessageDialog::set_buttons);
            register_method("get_buttons", &NativeMessageDialog::get_buttons);
            register_method("get_response_button_title", &NativeMessageDialog::get_response_button_title);
            register_method("get_response_button_index", &NativeMessageDialog::get_response_button_index);
            register_property<NativeMessageDialog, String>("message", &NativeMessageDialog::message, "");
            register_property<NativeMessageDialog, Array>("buttons", &NativeMessageDialog::buttons, Array());
        }
    };

    class NativeFileDialog : public  GDNativeDialog {
        GODOT_SUBCLASS(NativeFileDialog, GDNativeDialog);
    public:

        enum Mode {
          MODE_SELECT_FILE     = NativeDialog::FileDialog::SELECT_FILE,
            MODE_SELECT_DIR      = NativeDialog::FileDialog::SELECT_DIR,
            MODE_SELECT_FILE_DIR = NativeDialog::FileDialog::SELECT_FILE_DIR ,
            MODE_MULTI_SELECT    = NativeDialog::FileDialog::MULTI_SELECT,
            MODE_SAVE  = NativeDialog::FileDialog::SELECT_TO_SAVE
        };

        NativeFileDialog():GDNativeDialog() {
            m_pDialog = new NativeDialog::FileDialog();
            _bind_callbacks();
        }

        ~NativeFileDialog() {
            delete (static_cast<NativeDialog::FileDialog*>(m_pDialog));
        }

        virtual void show() override {
            auto dialog = dynamic_cast<NativeDialog::FileDialog*>(m_pDialog);
            if (dialog) {
                dialog->setMode(mode);
                dialog->setDefaultPath(str2std(default_path));
            }
            GDNativeDialog::show();
        }

        void set_mode(int p_mode) { mode = p_mode; }
        int get_mode() const { return mode; }

        void set_default_path(String p_path) { default_path = p_path; }
        String get_default_path() const { return default_path; }

        Array get_selected_pathes() const {
            Array pathes;

            if (auto dialog = dynamic_cast<NativeDialog::FileDialog*>(m_pDialog) ) {
                for(const std::string& p : dialog->selectedPathes()) {
                    String ss(p.c_str());
                    pathes.append(ss);
                }
            }
            return pathes;
        }

        void add_filter(String title, String extentions) {
            if (auto dialog = dynamic_cast<NativeDialog::FileDialog*>(m_pDialog) ) {
                dialog->addFilter(str2std(title), str2std((extentions)));
            }
        }

        void clear_filters() {
            if (auto dialog = dynamic_cast<NativeDialog::FileDialog*>(m_pDialog) ) {
                dialog->clearFilters();
            }
        }

        void cleanup() {
            if (auto dialog = dynamic_cast<NativeDialog::FileDialog*>(m_pDialog) ) {
                dialog->cleanUp();
            }
        }


    protected:
        String default_path;
        int mode = MODE_SELECT_FILE;

    public:
        static void _register_methods() {

            register_method("set_mode", &NativeFileDialog::set_mode);
            register_method("get_mode", &NativeFileDialog::get_mode);
            register_method("set_default_path", &NativeFileDialog::set_default_path);
            register_method("get_default_path", &NativeFileDialog::get_default_path);

            register_method("add_filter", &NativeFileDialog::add_filter);
            register_method("clear_filters", &NativeFileDialog::clear_filters);
            register_method("cleanup", &NativeFileDialog::cleanup);
            register_method("get_selected_pathes", &NativeFileDialog::get_selected_pathes);

            register_property<NativeFileDialog, int>("mode", &NativeFileDialog::mode, MODE_SELECT_FILE);
            register_property<NativeFileDialog, String>("default_path", &NativeFileDialog::default_path, "");
        }
    };

    class NativeColorDialog : public  GDNativeDialog {
        GODOT_SUBCLASS(NativeColorDialog, GDNativeDialog);
    public:
        NativeColorDialog():GDNativeDialog() {
            m_pDialog = new NativeDialog::ColorPickerDialog("");
            _bind_callbacks();
        }

        ~NativeColorDialog() {
            delete (static_cast<NativeDialog::ColorPickerDialog*>(m_pDialog));
        }

        virtual void show() override {
            if (auto dialog = dynamic_cast<NativeDialog::ColorPickerDialog*>(m_pDialog)) {
                dialog->setColor({color.r, color.g, color.b, color.a});
            }
            GDNativeDialog::show();
        }

        Color get_color() {
            Color c;
            if (auto dialog = dynamic_cast<NativeDialog::ColorPickerDialog*>(m_pDialog)) {
                auto cc = dialog->color();
                c = Color(cc.r, cc.g, cc.b, cc.a);
            }
            return c;
        }
        void set_color(Color p_color) { color = p_color; }

    protected:
        Color color;
    public:
        static void _register_methods() {

            register_method("set_color", &NativeColorDialog::set_color);
            register_method("get_color", &NativeColorDialog::get_color);
            register_property<NativeColorDialog, Color>("color", &NativeColorDialog::set_color, &NativeColorDialog::get_color, Color());
        }
    };
}


#ifdef ND_PLATFORM_GTK
#include <gtk/gtk.h>
#endif

/** GDNative Initialize **/
void GDNATIVE_INIT(godot_gdnative_init_options *options) {
#ifdef ND_PLATFORM_GTK
    gtk_init(nullptr, nullptr);
#endif
}

/** GDNative Terminate **/
void GDNATIVE_TERMINATE(godot_gdnative_terminate_options *options) {
}

/** NativeScript Initialize **/
void NATIVESCRIPT_INIT() {
    register_class<GDNativeDialog>();
    register_class<NativeMessageDialog>();
    register_class<NativeFileDialog>();
    register_class<NativeColorDialog>();
}
