#include <linux/slab.h>
#include <linux/string.h>
#include <linux/printk.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DEVICE_MAJOR 45
#define DEVICE_MAX_MINORS 1
#define DEVICE_NAME "phone_dict"

struct cdev cdev;

static struct user_data ** users;
static unsigned int users_size;

const unsigned int PHONE_SIZE = 3;

void printLog(char* str)
{
    printk(KERN_INFO "phone_dict: %s\n", str);
}

struct user_data {
    char* surname;
    unsigned int len;

    char* phone;
};

unsigned int users_count(void) {
    return users_size;
}

long swap_users(const unsigned int index1, const unsigned int index2)
{
    unsigned int user_len = users_count();
    struct user_data* tmp;

    if (index1 >= user_len) {
        return -1;
    }

    if (index2 >= user_len) {
        return -1;
    }

    tmp = users[index1];
    users[index1] = users[index2];
    users[index2] = tmp;

    return 0;
}

long pop_user(void)
{
    if (users_count() == 0) {
        return -1;
    }

    --users_size;

    return 0;
}

long copy_users(struct user_data* * distance_to_copy)
{
    unsigned int user_len = users_count();
    int i;

    for (i = 0; i < user_len - 1; ++i) {
        distance_to_copy[i] = users[i];
    }

    return 0;
}


long push_user(struct user_data* input_data)
{
    struct user_data** new_users = kmalloc((users_size + 1) * sizeof(*new_users), GFP_KERNEL);

    ++users_size;

    copy_users(new_users);
    kfree(users);
    users = new_users;

    users[users_size - 1] = input_data;

    return 0;
}

struct user_data* get_user_data(const unsigned int index)
{
    // может упасть, когда index > len
    return users[index];
}

short is_this_user(struct user_data* input_data, const char* surname, unsigned int len)
{
    return strcmp(input_data->surname, surname) == 0;
}

long get_user_index(const char* surname, unsigned int len){
    int i;

    for (i = 0; i < users_count(); ++i) {
        struct user_data * cur_user = get_user_data(i);

        if (is_this_user(cur_user, surname, len)) {
            return i;
        }
    }

    return -1;
}

long get_user(const char* surname, unsigned int len, struct user_data* user)
{
    long user_index = get_user_index(surname, len);

    if (user_index == -1) {
        return 1;
    }

    struct user_data* data = get_user_data(user_index);

    user->phone = data->phone;
    user->surname = data->surname;
    user->len = data->len;

    return 0;
}

long add_user(struct user_data* input_data)
{
    push_user(input_data);

    return 0;
}

long del_user(const char* surname, unsigned int len)
{
    long user_index = get_user_index(surname, len);
    unsigned int last_index;

    if (user_index == -1) {
        return -1;
    }

    last_index = users_count() - 1;

    swap_users(user_index, last_index);

    pop_user();

    return 0;
}


const struct file_operations fops = {
        .owner = THIS_MODULE,
};

static int __init phone_dict_init(void)
{
    printLog("I am alive");

    int err = register_chrdev_region(MKDEV(DEVICE_MAJOR, 0), DEVICE_MAX_MINORS, DEVICE_NAME);

    cdev_init(&cdev, &fops);
    cdev_add(&cdev, MKDEV(DEVICE_MAJOR, 0), 1);
    char surname1[5] = "Vlad\0";
    unsigned int len1 = 4;
    char phone1[4] = "888\0";

    char surname2[6] = "Dimas\0";
    unsigned int len2 = 5;
    char phone2[4] = "999\0";

    struct user_data* data1 = kmalloc(sizeof(data1), GFP_KERNEL);
    struct user_data* data2 = kmalloc(sizeof(data2), GFP_KERNEL);

    data1->surname = surname1;
    data1->len = len1;
    data1->phone = phone1;

    data2->surname = surname2;
    data2->len = len2;
    data2->phone = phone2;

    printLog("answer: 888");
    printLog(data1->phone);

    printLog("answer: 999");
    printLog(data2->phone);

    add_user(data1);
    struct user_data output;

    printLog("answer: 888");
    printk(KERN_INFO "%ld\n", &output);
    printk(KERN_INFO "%ld\n", get_user("Vlad", 4, &output));
    printk(KERN_INFO "%ld\n", &output);
    printLog(output.phone);

    add_user(data2);

    printLog("answer: 999");
    printk(KERN_INFO "%ld\n", get_user("Dimas", 5, &output));
    printLog(output.phone);
    printLog("answer: 888");
    printk(KERN_INFO "%ld\n", get_user("Vlad", 4, &output));
    printLog(output.phone);

    del_user("Vlad", 4);

    printLog("answer: 999");
    printk(KERN_INFO "%ld\n", get_user("Dimas", 5, &output));
    printLog(output.phone);
    printLog("answer: ");
    printk(KERN_INFO "%ld\n", get_user("Vlad", 4, &output));

    add_user(data1);
    printLog("answer: 999");
    printk(KERN_INFO "%ld\n", get_user("Dimas", 5, &output));
    printLog(output.phone);
    printLog("answer: 888");
    printk(KERN_INFO "%ld\n", get_user("Vlad", 4, &output));
    printLog(output.phone);

    printLog("that all");
    return 0;
}

static void __exit phone_dict_cleanup(void)
{
    unregister_chrdev_region(MKDEV(DEVICE_MAJOR, 0), DEVICE_MAX_MINORS);

    printk(KERN_INFO "phone_dict: I am dead.\n");
}


module_init(phone_dict_init);
module_exit(phone_dict_cleanup);

//int main() {
//    while (1) {
//        int task;
//        char* surname;
//        int len;
//
//        scanf("%d", &task);
//        scanf("%d", &len);
//
//        surname = malloc(len * sizeof(surname));
//
//        scanf("%s", surname);
//
//        if (task == 1) {
//            char* phone;
//
//            phone = malloc(PHONE_SIZE * sizeof(*phone));
//
//            scanf("%s", phone);
//
//            struct user_data* data = malloc(sizeof(*data));
//            data->surname = surname;
//            data->len = len;
//            data->phone = phone;
//
//            add_user(data);
//
//            printf("success\n");
//        } else if (task == 2) {
//            char* phone = get_user(surname, len);
//
//            printf("%s\n", phone);
//        } else {
//            if (del_user(surname, len) == 0) {
//                printf("success\n");
//            } else {
//                printf("fail\n");
//            }
//        }
//    }
//}
