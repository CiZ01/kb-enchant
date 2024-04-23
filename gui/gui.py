import gi

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Gdk


class Handler:
    def onDestroy(self, *args):
        print("Focus out event")
        Gtk.main_quit()

    def onFocusOutEvent(self, widget, event):
        print("Focus out event")
        Gtk.main_quit()

    def setMode(self, button):
        print("Set mode to: ", button.get_name())


builder = Gtk.Builder()
builder.add_from_file("template.glade")
builder.connect_signals(Handler())

window = builder.get_object("overlay_window")
window.show_all()

Gtk.main()
