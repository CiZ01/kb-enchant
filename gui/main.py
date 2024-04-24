from pynput import keyboard
from gui import showOverlay


SPECIAL_CHANGE_MODE_KEY = "<0>"  # 00 ff 00 00 00 00


def on_press(key):
    try:
        print(isinstance(key, keyboard.HotKey))
        # ctrl_l + F12 = show overlay
        if key == keyboard.Key.alt and key == keyboard.Key.ctrl_r:
            print("Ctrl + F12 pressed")
            # showOverlay()  # is blocking
        elif str(key) == SPECIAL_CHANGE_MODE_KEY:
            print("Special key pressed")
        else:
            print("Key pressed: {0}".format(key))
    except AttributeError:
        pass


def triggerShowOverlay():
    print("Overlay shown")
    showOverlay()


def for_canonical(f):
    return lambda k: f(l.canonical(k))


hotkey = keyboard.HotKey(keyboard.HotKey.parse("<ctrl>+<alt>"), triggerShowOverlay)
with keyboard.Listener(on_press=for_canonical(hotkey.press)) as l:
    l.join()
