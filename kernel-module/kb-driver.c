#include <linux/device.h>
#include <linux/hid.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>

#include "kb-event.h"

#define EXPAND_TO_8_BYTES(hex)                                                                         \
    ((unsigned char[]){(hex >> 56) & 0xFF, (hex >> 48) & 0xFF, (hex >> 40) & 0xFF, (hex >> 32) & 0xFF, \
                       (hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF})

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CiZ");
MODULE_DESCRIPTION("Custom Keyboard Driver");
MODULE_VERSION("0.1");

#define USB_PRODUCT_ID_MYKB_SPECIAL_KEYS 0xfa07
#define HID_BUS_TYPE_KB 0x0005
#define USB_VENDOR_ID_SUN 0x3554

#define KB_NAME "KB-ENCH"

#define map_key_clear(c) hid_map_usage_clear(hi, usage, bit, max, EV_KEY, (c))

#define send_null_key(data) \
    data[1] = 0;            \
    hid_report_raw_event(hdev, HID_INPUT_REPORT, data, size, 0)

#define is_wheel_key(key) (key == WHEEL_UP || key == WHEEL_DOWN || key == WHEEL_PUSH)

#define KB_IOCTL_MAGIC 'K'
#define HID_SET_MODE _IOW(KB_IOCTL_MAGIC, 1, int)
#define HID_GET_MODE _IOR(KB_IOCTL_MAGIC, 2, int)

static int MAJOR_NUM = 0;
dev_t dev_num;
static struct cdev mykb_cdev;
static struct class *kb_ench_class = NULL;

static u8 current_mode = 0;
static DEFINE_MUTEX(mutex__current_mode);
static ssize_t mode_show(const struct class *class, const struct class_attribute *attr, char *buf)
{
    ssize_t ret;
    mutex_lock(&mutex__current_mode);
    ret = sprintf(buf, "%u\n", current_mode);
    mutex_unlock(&mutex__current_mode);
    return ret;
}

static ssize_t mode_store(const struct class *class, const struct class_attribute *attr, const char *buf, size_t count)
{
    int new_mode;
    if (kstrtoint(buf, 10, &new_mode))
    {
        return -EINVAL;
    }

    if (new_mode < 0 || new_mode > 2)
    {
        return -EINVAL;
    }

    mutex_lock(&mutex__current_mode);
    current_mode = (u8)new_mode;
    mutex_unlock(&mutex__current_mode);
    printk(KERN_INFO "Mode changed to %d\n", current_mode);

    return count;
}

CLASS_ATTR_RW(mode);

static const struct file_operations mykb_fops = {
    .owner = THIS_MODULE,
};

static int mykb_raw_event(struct hid_device *hdev, struct hid_report *report, u8 *data, int size)
{

    if (!is_wheel_key(data[1]))
    {
        return 0;
    }

    struct input_dev *input;
    struct hid_input *hidinput;
    hidinput = list_entry(hdev->inputs.next, struct hid_input, list);
    input = hidinput->input;

    u8 key_scancode = data[1];

    if (key_scancode == WHEEL_PUSH)
    {
        current_mode = (current_mode + 1) % 3;
        kb_event__change_mode(input);
        data[1] = 0xff;
        input_sync(input);
        return 0;
    }

    switch (current_mode)
    {
    case 0: // volume mode
        return 0;
    case 1: // brightness mode
        kb_event__brightness(input, key_scancode);
        break;
    case 2: // zoom mode
        kb_event__zoom(input, key_scancode);
        break;
    default:
        printk(KERN_ERR "Invalid mode: %d\n", current_mode);
        return 0;
    }

    input_sync(input);
    send_null_key(data);

    return 0;
}
static int mykb_input_mapping(struct hid_device *hdev, struct hid_input *hi, struct hid_field *field,
                              struct hid_usage *usage, unsigned long **bit, int *max)
{

    return 0;
}

static void mykb_remove(struct hid_device *hdev)
{

    // ---- CLEANUP SYSFS ATTRIBUTE ----
    class_remove_file(kb_ench_class, &class_attr_mode);
    class_destroy(kb_ench_class);

    // ---- CLEANUP HID DEVICE ----
    hid_hw_stop(hdev);
    dev_printk(KERN_INFO, &hdev->dev, "KB-ENCH removed\n");
}

static int mykb_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
    printk("KEYBOARD detected: %x, %s\n", hdev->type, hdev->name);
    int ret;

    //  ---- INIT SYSFS ATTRIBUTE ----
    kb_ench_class = class_create("kb_ench");
    if (IS_ERR(kb_ench_class))
    {
        printk(KERN_ERR "failed to create class\n");
        return PTR_ERR(kb_ench_class);
    }

    ret = class_create_file(kb_ench_class, &class_attr_mode);
    if (ret)
    {
        printk(KERN_ERR "failed to create mode attribute\n");
        class_destroy(kb_ench_class);
        return ret;
    }

    // ---- INIT HID DEVICE ----
    ret = hid_parse(hdev);
    if (ret)
    {
        hid_err(hdev, "Error parsing Compaq Internet Keyboard\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
    if (ret)
    {
        hid_err(hdev, "Error failed to start Compaq Internet Keyboard\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    return 0;
}

/// Device strings
static const struct hid_device_id mykb_id_table[] = {
    {HID_DEVICE(HID_BUS_ANY, HID_GROUP_ANY, USB_VENDOR_ID_SUN, USB_PRODUCT_ID_MYKB_SPECIAL_KEYS)}, {}};

MODULE_DEVICE_TABLE(hid, mykb_id_table);

static struct hid_driver mykb_driver = {
    .name = "mykb",
    .id_table = mykb_id_table,
    /// This component is only for tweaking the report descriptor on events. We don't need to do this and can just
    /// use the scancodes from the device directly
    //  .report_fixup		=
    /// This callback runs after the kernel has positively identified our device. You can use this to check if the
    /// device is working by sending out a sample command, etc, but we don't need it here.
    .probe = mykb_probe,
    /// This callback runs after the device has been removed
    .remove = mykb_remove,
    /// This callback is the event handler for a keyboard event.
    .raw_event = mykb_raw_event,
    .input_mapping = mykb_input_mapping,
};

module_hid_driver(mykb_driver);
