#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/rwlock.h>
#include <linux/uaccess.h>

static int major = 0;
static rwlock_t lock;
static char test_string[15] = "Hello!\n\0";

ssize_t test_read(struct file *fd, char __user *buff, size_t size, loff_t *off)
{
    size_t rc;

    read_lock(&lock);
    rc = simple_read_from_buffer(buff, size, off, test_string, 15);
    read_unlock(&lock);

    return rc;
}

ssize_t test_write(struct file *fd, const char __user *buff, size_t size, loff_t *off)
{
    size_t rc = 0;
    if (size > 15)
        return -EINVAL;

    write_lock(&lock);
    if (__copy_from_user(test_string, buff, size) == 0) {
        rc = size;
    } else {
        rc = -EFAULT;
    }
    write_unlock(&lock);

    return rc;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = test_read,
    .write = test_write
};

int init_module(void)
{
    pr_info("Test module is loaded.\n");
    rwlock_init(&lock);
    major = register_chrdev(major, "test3", &fops);

    if (major < 0)
        return major;
    pr_info("Major number is %d.\n", major);

    return 0;
}

void cleanup_module(void)
{
    unregister_chrdev(major, "test3");
}

MODULE_LICENSE("GPL");
