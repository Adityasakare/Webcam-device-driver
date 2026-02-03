#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>          // for open()
#include <unistd.h>         // for close()
#include <sys/ioctl.h>      // for ioctl()
#include <linux/videodev2.h> // V4L2 Header


int main(void)
{
    int fd;
    struct v4l2_capability cap;

    // open device node 
    fd = open("/dev/video0", O_RDWR);
    if(fd < 0)
    {
        perror("Failed to open device");
        return -1;
    }
    printf("Sucessfully opend /deb/video1\n");

    // 2. Use icotl() to query the driver
    if(ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0)
    {
        perror("Failed VIDIOC_QUERYCAP");
        close(fd);
        return -1;
    }

    //3. print the information return by driver
    printf("Driver Name:    %s\n", cap.driver);
    printf("Card Name:      %s\n", cap.card);
    printf("Bus Info:       %s\n", cap.bus_info);

    //4. close fd
    close(fd);
    printf("Device closed\n");
    return 0;
}

