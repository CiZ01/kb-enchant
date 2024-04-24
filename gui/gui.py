import gi
from controller import KBController, MODES

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Gdk


def setButtonActive(button_name: str):
    btn = builder.get_object(button_name)
    style_ctx = btn.get_style_context()
    style_ctx.add_class("active")


def setButtonInactive(button_name: str):
    btn = builder.get_object(button_name)
    style_ctx = btn.get_style_context()
    style_ctx.remove_class("active")


class Handler:
    def __init__(self, builder):
        self.controller = KBController()
        self.last_mode = None
        self.builder = builder
        self.curr_mode = None

        self.curr_mode = self.controller.get_mode()
        if self.curr_mode is not None:
            self.last_mode = self.curr_mode
            setButtonActive(MODES[self.curr_mode])
        else:
            print("[ERROR]: Failed to get current mode")

    def onDestroy(self, *args):
        Gtk.main_quit()

    def onFocusOutEvent(self, widget, event):
        Gtk.main_quit()

    def setMode(self, button):
        if self.curr_mode == MODES.index(button.get_name()):
            return
        if self.controller.set_mode(button.get_name()):
            self.last_mode = self.curr_mode
            self.curr_mode = MODES.index(button.get_name())
            setButtonInactive(MODES[self.last_mode])
            setButtonActive(MODES[self.curr_mode])
            Gtk.main_quit()


builder = Gtk.Builder()

builder.add_from_file("template.glade")
builder.connect_signals(Handler(builder))

screen = Gdk.Screen.get_default()
provider = Gtk.CssProvider()
provider.load_from_path("/home/cizzo/Pubblici/kb-enchant/gui/style.css")
Gtk.StyleContext.add_provider_for_screen(
    screen, provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
)

window = builder.get_object("overlay_window")
window.show_all()


def showOverlay():
    Gtk.main()
    pass


def hideOverlay():
    Gtk.main_quit()


if __name__ == "__main__":
    showOverlay()
