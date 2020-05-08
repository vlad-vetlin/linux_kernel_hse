#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static struct user_data ** users;
static unsigned int users_size;

const unsigned int PHONE_SIZE = 1;

struct user_data {
    char* surname;
    unsigned int len;

    char* phone;
};

unsigned int users_count() {
    return users_size;
}

long swap_users(const unsigned int index1, const unsigned int index2)
{
    unsigned int user_len = users_count();

    if (index1 >= user_len) {
        return -1;
    }

    if (index2 >= user_len) {
        return -1;
    }

    struct user_data* tmp = users[index1];
    users[index1] = users[index2];
    users[index2] = tmp;

    return 0;
}

long pop_user()
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
    struct user_data** new_users = malloc((users_size + 1) * sizeof(*new_users));

    ++users_size;

    copy_users(new_users);
    free(users);
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


char* get_user(const char* surname, unsigned int len)
{
    long user_index = get_user_index(surname, len);

    if (user_index == -1) {
        return "\0";
    }

    struct user_data* data = get_user_data(user_index);

    return data->phone;
}

long add_user(struct user_data* input_data)
{
    push_user(input_data);

    return 0;
}

long del_user(const char* surname, unsigned int len)
{
    long user_index = get_user_index(surname, len);

    if (user_index == -1) {
        return -1;
    }

    unsigned int last_index = users_count() - 1;

    swap_users(user_index, last_index);

    pop_user();

    return 0;
}

int main() {
    while (1) {
        int task;
        char* surname;
        int len;

        scanf("%d", &task);
        scanf("%d", &len);
        
        surname = malloc(len * sizeof(surname));

        scanf("%s", surname);

        if (task == 1) {
            char* phone;

            phone = malloc(PHONE_SIZE * sizeof(*phone));

            scanf("%s", phone);

            struct user_data* data = malloc(sizeof(*data));
            data->surname = surname;
            data->len = len;
            data->phone = phone;

            add_user(data);

            printf("success\n");
        } else if (task == 2) {
            char* phone = get_user(surname, len);

            printf("%s\n", phone);
        } else {
            if (del_user(surname, len) == 0) {
                printf("success\n");
            } else {
                printf("fail\n");
            }
        }
    }
}
