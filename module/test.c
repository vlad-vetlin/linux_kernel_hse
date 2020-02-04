#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_MAJOR 44
#define DEVICE_MAX_MINORS 1
#define DEVICE_NAME "test"
#define BUF_LEN 80
#define BOOK_SIZE 2000

struct device_data {
    struct cdev cdev;
    size_t size;
    char buffer[BUF_LEN];
    char phone_book[BOOK_SIZE];
    size_t book_size;
};

struct device_data global_data;

static int test_open(struct inode *inode, struct file *file) {
    struct device_data *data;

    data = container_of(inode->i_cdev, struct device_data, cdev);   

    file->private_data = data;

    try_module_get(THIS_MODULE);

    data->size = BUF_LEN;

    return 0;
}

static ssize_t test_read(struct file* file, char __user *user_buffer, size_t size, loff_t *offset) {
    ssize_t len;

    struct device_data *data;

    data = (struct device_data*) file->private_data;
    len = min(global_data.book_size - *offset, size);

    //printk(KERN_INFO "test: read %ld %ld %ld", len, global_data.book_size, size);
    if (len <= 0) {
        return 0;
    }

    if (copy_to_user(user_buffer, data->phone_book + *offset, len)) {
        return -EFAULT;
    }

    *offset += len;

    return len;
}

static ssize_t test_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset) {
    ssize_t len;
    size_t res_size;
    struct device_data *data;

    data = (struct device_data *) file->private_data;
    len = min(data->size - *offset, size);

    if (len <= 0) {
        return 0;
    }

    if (copy_from_user(data->buffer, user_buffer + *offset, len)) {
        return -EFAULT;
    }
    
    //printk("test: write %s", global_data.buffer);
    strcat(data->phone_book, global_data.buffer + *offset);
    data->book_size = strlen(data->phone_book);
    //printk(KERN_INFO "test: write %ld %s", data->book_size, data->phone_book);

    *offset += len;
    return len;
}

static int test_release(struct inode *inode, struct file* file) {
    module_put(THIS_MODULE);
    return 0;
}

const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = test_open,
    .read = test_read,
    .write = test_write,
    .release = test_release
};

static int __init test_init(void)
{
    int i;
    int err = register_chrdev_region(MKDEV(DEVICE_MAJOR, 0), DEVICE_MAX_MINORS, DEVICE_NAME);

    if (err != 0) {
        printk(KERN_ERR "test: Error when trying to insert\n");
        return err;
    }

    global_data.book_size = 0;
    
    cdev_init(&global_data.cdev, &fops);
    cdev_add(&global_data.cdev, MKDEV(DEVICE_MAJOR, 0), 1);
    
    printk(KERN_INFO "test: I am alive\n");
    return 0;
}

static void __exit test_cleanup(void)
{
    cdev_del(&global_data.cdev);

    unregister_chrdev_region(MKDEV(DEVICE_MAJOR, 0), DEVICE_MAX_MINORS); 
    
    printk(KERN_INFO "test: I am dead.\n");
}

module_init(test_init);
module_exit(test_cleanup);

MODULE_DESCRIPTION("Simple phone book");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Vetlin Vladislav <vsvetlin@edu.hse.ru>");
MODULE_SUPPORTED_DEVICE("test");
