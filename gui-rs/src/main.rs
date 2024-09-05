use gtk::prelude::*;
use gtk::Application;
mod gui;
use std::fs;

const APP_ID: &str = "org.cizzo.kb-bench-gui";
const MODULE_NAME: &str = "kb_driver";

fn main() -> gtk::glib::ExitCode {
    let app = Application::new(Some(APP_ID), Default::default());

    // check if the module is loaded
    if !module_is_loaded(MODULE_NAME) {
        eprintln!("Module {} is not loaded", MODULE_NAME);
        app.connect_activate(|_| gui::error_dialog("Module not loaded"));
        app.run();
        return gtk::glib::ExitCode::FAILURE;
    }

    app.connect_startup(|_| gui::load_css());

    // Connect to "activate" signal of `app`
    app.connect_activate(gui::build_ui);

    // Run the application
    app.run()
}

fn module_is_loaded(module_name: &str) -> bool {
    let module_path = format!("/sys/module/{}", module_name);
    fs::metadata(&module_path).is_ok()
}
