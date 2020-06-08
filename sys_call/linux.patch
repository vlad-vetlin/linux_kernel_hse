diff --git a/arch/x86/entry/syscalls/syscall_64.tbl b/arch/x86/entry/syscalls/syscall_64.tbl
index dca84657a..6c3fff7f8 100644
--- a/arch/x86/entry/syscalls/syscall_64.tbl
+++ b/arch/x86/entry/syscalls/syscall_64.tbl
@@ -358,6 +358,9 @@
 434	common	pidfd_open		__x64_sys_pidfd_open
 435	common	clone3			__x64_sys_clone3/ptregs
 436	common	hello_world_kek		__x64_sys_hello_world_kek
+437	common	get_user		__x64_sys_get_user
+438	common	add_user		__x64_sys_add_user
+439	common	del_user		__x64_sys_del_user
 
 #
 # x32-specific system call numbers start at 512 to avoid cache impact
diff --git a/include/linux/syscalls.h b/include/linux/syscalls.h
index d93678f06..1c58330fd 100644
--- a/include/linux/syscalls.h
+++ b/include/linux/syscalls.h
@@ -69,6 +69,7 @@ struct rseq;
 union bpf_attr;
 struct io_uring_params;
 struct clone_args;
+struct user_data;
 
 #include <linux/types.h>
 #include <linux/aio_abi.h>
@@ -287,6 +288,9 @@ static inline void addr_limit_user_check(void)
  */
 #ifndef CONFIG_ARCH_HAS_SYSCALL_WRAPPER
 asmlinkage long sys_hello_world_kek(void);
+asmlinkage long sys_get_user(const char* surname, unsigned int len, struct user_data* output_data);
+asmlinkage long sys_add_user(struct user_data* input_data);
+asmlinkage long sys_del_user(const char* surname, unsigned int len);
 asmlinkage long sys_io_setup(unsigned nr_reqs, aio_context_t __user *ctx);
 asmlinkage long sys_io_destroy(aio_context_t ctx);
 asmlinkage long sys_io_submit(aio_context_t, long,
diff --git a/kernel/Makefile b/kernel/Makefile
index 53b69fd83..c073623c4 100644
--- a/kernel/Makefile
+++ b/kernel/Makefile
@@ -10,7 +10,7 @@ obj-y     = fork.o exec_domain.o panic.o \
 	    extable.o params.o \
 	    kthread.o sys_ni.o nsproxy.o \
 	    notifier.o ksysfs.o cred.o reboot.o \
-	    async.o range.o smpboot.o ucount.o hello_world_kek.o
+	    async.o range.o smpboot.o ucount.o hello_world_kek.o phone_book.o
 
 obj-$(CONFIG_MODULES) += kmod.o
 obj-$(CONFIG_MULTIUSER) += groups.o
diff --git a/kernel/phone_book.c b/kernel/phone_book.c
new file mode 100644
index 000000000..eec0578e7
--- /dev/null
+++ b/kernel/phone_book.c
@@ -0,0 +1,164 @@
+#include <linux/slab.h>
+#include <linux/string.h>
+#include <linux/printk.h>
+#include <linux/module.h>
+#include <linux/kernel.h>
+#include <linux/init.h>
+
+#include <linux/ctype.h>
+#include <linux/export.h>
+#include <linux/kexec.h>
+#include <linux/kmod.h>
+#include <linux/kmsg_dump.h>
+#include <linux/syscore_ops.h>
+#include <linux/uaccess.h>
+
+#include <linux/syscalls.h>
+#include <linux/types.h>
+
+struct user_data {
+	char* surname;
+	unsigned int len;
+	char* phone;
+};
+
+static struct user_data ** users;
+static unsigned int users_size;
+
+const unsigned int PHONE_SIZE = 3;
+
+void printLog(char* str)
+{
+    printk(KERN_INFO "phone_dict: %s\n", str);
+}
+
+unsigned int users_count(void) {
+    return users_size;
+}
+
+long swap_users(const unsigned int index1, const unsigned int index2)
+{
+    unsigned int user_len = users_count();
+    struct user_data* tmp;
+
+    if (index1 >= user_len) {
+        return -1;
+    }
+
+    if (index2 >= user_len) {
+        return -1;
+    }
+
+    tmp = users[index1];
+    users[index1] = users[index2];
+    users[index2] = tmp;
+
+    return 0;
+}
+
+long pop_user(void)
+{
+    if (users_count() == 0) {
+        return -1;
+    }
+
+    --users_size;
+
+    return 0;
+}
+
+long copy_users(struct user_data* * distance_to_copy)
+{
+    unsigned int user_len = users_count();
+    int i;
+
+    for (i = 0; i < user_len - 1; ++i) {
+        distance_to_copy[i] = users[i];
+    }
+
+    return 0;
+}
+
+
+long push_user(struct user_data* input_data)
+{
+    struct user_data** new_users = kmalloc((users_size + 1) * sizeof(*new_users), GFP_KERNEL);
+
+    ++users_size;
+
+    copy_users(new_users);
+    kfree(users);
+    users = new_users;
+
+    users[users_size - 1] = input_data;
+
+    return 0;
+}
+
+struct user_data* get_user_data(const unsigned int index)
+{
+    // может упасть, когда index > len
+    return users[index];
+}
+
+short is_this_user(struct user_data* input_data, const char* surname, unsigned int len)
+{
+    return strcmp(input_data->surname, surname) == 0;
+}
+
+long get_user_index(const char* surname, unsigned int len){
+    int i;
+
+    for (i = 0; i < users_count(); ++i) {
+        struct user_data * cur_user = get_user_data(i);
+
+        if (is_this_user(cur_user, surname, len)) {
+            return i;
+        }
+    }
+
+    return -1;
+}
+
+SYSCALL_DEFINE3(get_user, const char*, surname, unsigned int, len, struct user_data*, user)
+{
+    struct user_data* data;
+    long user_index = get_user_index(surname, len);
+
+    if (user_index == -1) {
+        return 1;
+    }
+
+    data = get_user_data(user_index);
+
+    user->phone = data->phone;
+    user->surname = data->surname;
+    user->len = data->len;
+
+    return 0;
+}
+
+SYSCALL_DEFINE1(add_user, struct user_data*, input_data)
+{
+    push_user(input_data);
+
+    return 0;
+}
+
+SYSCALL_DEFINE2(del_user, const char*, surname, unsigned int, len)
+{
+    long user_index = get_user_index(surname, len);
+    unsigned int last_index;
+
+    if (user_index == -1) {
+        return -1;
+    }
+
+    last_index = users_count() - 1;
+
+    swap_users(user_index, last_index);
+
+    pop_user();
+
+    return 0;
+}
