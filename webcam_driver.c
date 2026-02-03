#include <linux/module.h>
#include <linux/usb.h>
#include <linux/videodev2.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>


static struct usb_device_id webcam_table[] = {
    { USB_DEVICE(0x2b7e, 0xb685) },      // vend, prod   
    { } 
};
MODULE_DEVICE_TABLE(usb, webcam_table);

// Structure to hold our device state
struct my_webcam {
    struct usb_device *udev;
    struct v4l2_device v4l2_dev;
    struct video_device vdev;
    struct mutex lock;
};

// Correct signature for vidioc_querycap
static int my_vidioc_querycap(struct file *file, void *priv, struct v4l2_capability *cap)
{
    strscpy(cap->driver, "my_webcam", sizeof(cap->driver));
    strscpy(cap->card, "My Scratch Webcam", sizeof(cap->card));
    // Set the capabilities (Video Capture and Streaming)
    cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
    cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;
    return 0;
}

static const struct v4l2_ioctl_ops my_v4l2_ioctl_ops = {
    .vidioc_querycap = my_vidioc_querycap, // Use the new helper
};

static const struct v4l2_file_operations my_v4l2_fops = {
    .owner          = THIS_MODULE,
    .open           = v4l2_fh_open,
    .release        = v4l2_fh_release,     // Standard V4L2 release
    .unlocked_ioctl = video_ioctl2,
};

// 3. The Probe Function - Called when USB is plugged in
static int webcam_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct my_webcam *cam;
    int ret;

    pr_info("Webcam: Probe started for Vendor=0x%x, Product=0x%x\n", id->idVendor, id->idProduct);

    // Allocate our custom device structure
    cam = kzalloc(sizeof(*cam), GFP_KERNEL);
    if (!cam) return -ENOMEM;

    cam->udev = interface_to_usbdev(interface);
    mutex_init(&cam->lock);

    // Register with V4L2 core
    ret = v4l2_device_register(&interface->dev, &cam->v4l2_dev);
    if (ret) goto err_free;

    // Setup the video device structure
    //strlcpy(cam->vdev.name, "My-Scratch-Webcam", sizeof(cam->vdev.name));
    strscpy(cam->vdev.name, "My-Scratch-Webcam", sizeof(cam->vdev.name));
    cam->vdev.v4l2_dev = &cam->v4l2_dev;
    cam->vdev.fops = &my_v4l2_fops;
    cam->vdev.ioctl_ops = &my_v4l2_ioctl_ops;
    cam->vdev.release = video_device_release_empty;
    cam->vdev.lock = &cam->lock;

    // Register /dev/videoX
    ret = video_register_device(&cam->vdev, VFL_TYPE_VIDEO, -1);
    if (ret) goto err_v4l2_unreg;

    // Save our structure in the interface for the disconnect function
    usb_set_intfdata(interface, cam);

    pr_info("Webcam: /dev/video%d registered successfully\n", cam->vdev.num);
    return 0;

err_v4l2_unreg:
    v4l2_device_unregister(&cam->v4l2_dev);
err_free:
    kfree(cam);
    return ret;
}

// 4. The Disconnect Function - Called when USB is removed
static void webcam_disconnect(struct usb_interface *interface)
{
    struct my_webcam *cam = usb_get_intfdata(interface);

    pr_info("Webcam: Disconnecting device /dev/video%d\n", cam->vdev.num);

    video_unregister_device(&cam->vdev);
    v4l2_device_unregister(&cam->v4l2_dev);
    kfree(cam);
}

// 5. Driver Registration with USB Core
static struct usb_driver my_webcam_driver = {
    .name       = "my_scratch_webcam",
    .probe      = webcam_probe,
    .disconnect = webcam_disconnect,
    .id_table   = webcam_table,
};

module_usb_driver(my_webcam_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aditya Sakare");
MODULE_DESCRIPTION("V4L2 USB Webcam Driver from Scratch");