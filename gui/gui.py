import gi
from controller import KBController

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Gdk


class Handler:
    def __init__(self, builder):
        self.controller = KBController()
        self.last_mode = None
        self.builder = builder

    def onDestroy(self, *args):
        print("Focus out event")
        Gtk.main_quit()

    def onFocusOutEvent(self, widget, event):
        print("Focus out event")
        Gtk.main_quit()

    def setMode(self, button):
        print("Set mode to: ", button.get_name())
        if self.controller.set_mode(button.get_name()):
            style_ctx = button.get_style_context()
            style_ctx.add_class("active") # not working


builder = Gtk.Builder()
builder.add_from_file("template.glade")
builder.connect_signals(Handler(builder))

window = builder.get_object("overlay_window")
window.show_all()

Gtk.main()
