use dirs::home_dir;
use gtk::gdk;
use gtk::gdk::keys::Key;
use gtk::glib::Propagation;
use gtk::{prelude::*, Orientation};
use gtk::{Application, ApplicationWindow, Button, CssProvider, IconSize};

use gui_rs::modes::{get_mode, set_mode, MODES};

pub fn build_ui(app: &Application) {
    const WIN_WIDTH: i32 = 480;
    const WIN_HEIGHT: i32 = 140;

    const BUTTON_WIDTH: i32 = WIN_WIDTH / MODES.len() as i32;
    const BUTTON_HEIGHT: i32 = WIN_HEIGHT;

    let hbox = gtk::Box::builder()
        .orientation(Orientation::Horizontal)
        .spacing(5)
        .margin(5)
        .build();

    hbox.style_context().add_class("box");

    let current_mode_value = get_mode().unwrap();

    for mode in MODES.iter() {
        let button_icon = gtk::Image::from_icon_name(Some(mode.icon), IconSize::LargeToolbar);
        button_icon.set_pixel_size(64);

        let button = Button::builder().child(&button_icon).build();
        //button.set_label(mode.name);

        button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);

        button.set_always_show_image(true);
        button.connect_clicked(move |_| {
            set_mode(mode.value as u32)
                .expect(format!("Failed to set {} mode", mode.name).as_str());
        });

        if mode.value == current_mode_value {
            button.set_sensitive(false);
        }

        // add css class to the button
        button
            .style_context()
            .add_class(format!("button-mode-{}", mode.name).as_str());

        hbox.add(&button);
    }

    // Create a window
    let window = ApplicationWindow::builder()
        .application(app)
        .title("KB-ENCH")
        .default_width(WIN_WIDTH)
        .width_request(WIN_WIDTH)
        .default_height(WIN_HEIGHT)
        .height_request(WIN_HEIGHT)
        .resizable(false)
        .child(&hbox)
        .build();

    window.connect_focus_out_event(move |win, _| {
        win.close(); // Close the window
        Propagation::Stop
    });

    window.connect_key_press_event(move |win, event| match event.keyval() {
        gdk::keys::constants::Escape | gdk::keys::constants::q => {
            win.close();
            Propagation::Stop
        }
        _ => Propagation::Proceed,
    });

    window.style_context().add_class("window");

    window.set_decorated(false);
    window.set_position(gtk::WindowPosition::Center);
    window.show_all();
}

const CSS_PATH: &str = "/.config/kb-ench/theme.css";

/*
 * Load the CSS file and add it to the provider
 * Add the provider to the default screen
 * If the CSS file is not found, nothing happens, will use the default theme
*/
pub fn load_css() {
    // Load the CSS file and add it to the provider
    let provider = CssProvider::new();
    let path = format!("{}/{}", home_dir().unwrap().to_str().unwrap(), CSS_PATH);
    provider.load_from_path(path.as_str()).unwrap_or(());

    // Add the provider to the default screen
    gtk::StyleContext::add_provider_for_screen(
        &gdk::Screen::default().unwrap_or(gdk::Screen::default().unwrap()),
        &provider,
        gtk::STYLE_PROVIDER_PRIORITY_APPLICATION,
    );
}

/*
 * Display an error dialog with the given message
*/
pub fn error_dialog(message: &str) {
    let dialog = gtk::MessageDialog::new(
        None::<&gtk::Window>,
        gtk::DialogFlags::MODAL,
        gtk::MessageType::Error,
        gtk::ButtonsType::Ok,
        message,
    );

    dialog.set_title("Error");
    dialog.run();
    dialog.close();
}
