/*************************************************************************/
/*  register_types.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "register_types.h"
#include <core/reference.h>
#include <NativeDialog.h>
#include <codecvt>
#include <locale>

#ifdef ND_PLATFORM_GTK
#include <gtk/gtk.h>
#endif

class GDNativeDialog : public Reference {
    GDCLASS(GDNativeDialog, Reference);
public:
    GDNativeDialog(): m_pDialog(nullptr) { }

    virtual void show() {
        m_pDialog->setTitle(str2std(title));
        m_pDialog->show();
    }

    void set_title(const String& p_title) { title = p_title; }
    String get_title() const { return title; }

protected:
    String title;
    NativeDialog::Dialog * m_pDialog;

    std::string str2std(const String& str) const {
        std::wstring_convert<std::codecvt_utf8<CharType>> conv;
        return conv.to_bytes(str.c_str());
    }

    virtual void _bind_callbacks() {

        m_pDialog->setDecideHandler([this](const NativeDialog::Dialog&){
            this->emit_signal("confirmed");
        });

        m_pDialog->setCancelHandler([this](const NativeDialog::Dialog&){
            this->emit_signal("canceled");
        });
    }

    static void _bind_methods() {

        ClassDB::bind_method(D_METHOD("set_title", "p_title"), &GDNativeDialog::set_title);
        ClassDB::bind_method(D_METHOD("get_title"), &GDNativeDialog::get_title);
        ClassDB::bind_method(D_METHOD("show"), &GDNativeDialog::show);


        ADD_PROPERTY(PropertyInfo(Variant::STRING, "title"), "set_title", "get_title");

        ADD_SIGNAL(MethodInfo("confirmed"));
        ADD_SIGNAL(MethodInfo("canceled"));
    }

};

class NativeMessageDialog : public  GDNativeDialog {
    GDCLASS(NativeMessageDialog, GDNativeDialog);

public:
    NativeMessageDialog():GDNativeDialog() {
        m_pDialog = memnew(NativeDialog::MessageDialog("", "", {}));
        _bind_callbacks();
    }

    ~NativeMessageDialog() {
        memdelete(static_cast<NativeDialog::MessageDialog*>(m_pDialog));
    }

    virtual void show() override {
        auto dialog = dynamic_cast<NativeDialog::MessageDialog*>(m_pDialog);
        if (dialog) {
            dialog->setMessage(message.utf8().ptr());
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
        if(auto dialog = dynamic_cast<NativeDialog::MessageDialog*>(m_pDialog))
            ss.parse_utf8(dialog->responseButtonTitle().c_str());
        return ss;
    }

    int get_response_button_index() const {
        if(auto dialog = dynamic_cast<NativeDialog::MessageDialog*>(m_pDialog))
            return dialog->responseButtonIndex();
        return -1;
    }

    void set_message(const String& p_message) { message = p_message; }
    String get_message() const { return message; }

    void set_buttons(Array p_buttons) { buttons = p_buttons; }
    Array get_buttons() const { return buttons; }

protected:
    String message;
    Array buttons;

    static void _bind_methods() {

        ClassDB::bind_method(D_METHOD("set_message", "message"), &NativeMessageDialog::set_message);
        ClassDB::bind_method(D_METHOD("get_message"), &NativeMessageDialog::get_message);
        ClassDB::bind_method(D_METHOD("set_buttons", "buttons"), &NativeMessageDialog::set_buttons);
        ClassDB::bind_method(D_METHOD("get_buttons"), &NativeMessageDialog::get_buttons);
        ClassDB::bind_method(D_METHOD("get_response_button_title"), &NativeMessageDialog::get_response_button_title);
        ClassDB::bind_method(D_METHOD("get_response_button_index"), &NativeMessageDialog::get_response_button_index);

        ADD_PROPERTY(PropertyInfo(Variant::STRING, "message"), "set_message", "get_message");
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "buttons"), "set_buttons", "get_buttons");
    }
};

class NativeFileDialog : public  GDNativeDialog {
    GDCLASS(NativeFileDialog, GDNativeDialog);

public:

    enum Mode {
        MODE_SELECT_FILE     = NativeDialog::FileDialog::SELECT_FILE,
        MODE_SELECT_DIR      = NativeDialog::FileDialog::SELECT_DIR,
        MODE_SELECT_FILE_DIR = NativeDialog::FileDialog::SELECT_FILE_DIR ,
        MODE_MULTI_SELECT    = NativeDialog::FileDialog::MULTI_SELECT,
        MODE_SAVE  = NativeDialog::FileDialog::SELECT_TO_SAVE
    };

    NativeFileDialog():GDNativeDialog() {
        m_pDialog = memnew(NativeDialog::FileDialog());
        _bind_callbacks();
    }

    ~NativeFileDialog() {
        memdelete(static_cast<NativeDialog::FileDialog*>(m_pDialog));
    }

    virtual void show() override {
        auto dialog = dynamic_cast<NativeDialog::FileDialog*>(m_pDialog);
        if (dialog) {
			dialog->setMode(int(mode));
#ifdef WIN32
			dialog->setDefaultPath(str2std(default_path.replace("\\", "/")));
#else
			dialog->setDefaultPath(str2std(default_path));
#endif
        }
        GDNativeDialog::show();
    }

    void set_mode(Mode p_mode) { mode = p_mode; }
    Mode get_mode() const { return mode; }

    void set_default_path(const String& p_path) { default_path = p_path; }
    String get_default_path() const { return default_path; }

    Array get_selected_pathes() const {
        Array pathes;

        if (auto dialog = dynamic_cast<NativeDialog::FileDialog*>(m_pDialog) ) {
            for(const std::string& p : dialog->selectedPathes()) {
				String ss = "";
				ss.parse_utf8(p.c_str(), p.length());
                pathes.append(ss);
			}
        }
        return pathes;
    }

    void add_filter(const String& title, const String& extentions) {
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
    Mode mode = MODE_SELECT_FILE;

    static void _bind_methods() {

        ClassDB::bind_method(D_METHOD("set_mode", "mode"), &NativeFileDialog::set_mode);
        ClassDB::bind_method(D_METHOD("get_mode"), &NativeFileDialog::get_mode);
        ClassDB::bind_method(D_METHOD("set_default_path", "buttons"), &NativeFileDialog::set_default_path);
        ClassDB::bind_method(D_METHOD("get_default_path"), &NativeFileDialog::get_default_path);

        ClassDB::bind_method(D_METHOD("add_filter", "title", "extentions"), &NativeFileDialog::add_filter);
        ClassDB::bind_method(D_METHOD("clear_filters"), &NativeFileDialog::clear_filters);
        ClassDB::bind_method(D_METHOD("cleanup"), &NativeFileDialog::cleanup);
        ClassDB::bind_method(D_METHOD("get_selected_pathes"), &NativeFileDialog::get_selected_pathes);

        ADD_PROPERTY(PropertyInfo(Variant::INT, "mode"), "set_mode", "get_mode");
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "default_path"), "set_default_path", "get_default_path");

        BIND_CONSTANT(MODE_SELECT_FILE);
        BIND_CONSTANT(MODE_SELECT_DIR);
        BIND_CONSTANT(MODE_SELECT_FILE_DIR);
        BIND_CONSTANT(MODE_MULTI_SELECT);
        BIND_CONSTANT(MODE_SAVE);
    }
};

class NativeColorDialog : public  GDNativeDialog {
    GDCLASS(NativeColorDialog, GDNativeDialog);
public:
    NativeColorDialog():GDNativeDialog() {
        m_pDialog = memnew(NativeDialog::ColorPickerDialog(""));
        _bind_callbacks();
    }

    ~NativeColorDialog() {
        memdelete(static_cast<NativeDialog::ColorPickerDialog*>(m_pDialog));
    }

    virtual void show() override {
        if (auto dialog = dynamic_cast<NativeDialog::ColorPickerDialog*>(m_pDialog)) {
            dialog->setColor({color.r, color.g, color.b, color.a});
        }
        GDNativeDialog::show();
    }

    Color get_color() const {
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

    static void _bind_methods() {

        ClassDB::bind_method(D_METHOD("set_color", "color"), &NativeColorDialog::set_color);
        ClassDB::bind_method(D_METHOD("get_color"), &NativeColorDialog::get_color);

        ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
    }
};

VARIANT_ENUM_CAST(NativeFileDialog::Mode);

void register_NativeDialogs_types() {
    ClassDB::register_virtual_class<GDNativeDialog>();
    ClassDB::register_class<NativeMessageDialog>();
    ClassDB::register_class<NativeFileDialog>();
    ClassDB::register_class<NativeColorDialog>();

#ifdef ND_PLATFORM_GTK
    gtk_init(nullptr, nullptr);
#endif

}

void unregister_NativeDialogs_types() {
}
