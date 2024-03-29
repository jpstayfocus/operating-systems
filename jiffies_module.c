#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "jiffies"

ssize_t proc_read(
    struct file *file,
    char __user *usr_buf,
    size_t count,
    loff_t *pos
);

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
};

MODULE_LICENSE("GPL");

// This function is called when the module is loaded.
int proc_init(void) {
    // creates the /proc/jiffies entry
    proc_create(PROC_NAME, 0666, NULL, (const struct proc_ops *) &proc_ops);
    return 0;
}

// This function is called when the module is removed.
void proc_exit(void) {
    // removes the /proc/jiffies
    remove_proc_entry(PROC_NAME, NULL);
}

// This function is called each time /proc/jiffies is read.
ssize_t proc_read(
        struct file *file,
        char __user *usr_buf,
        size_t count,
        loff_t *pos) {
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    if (completed) {
        completed = 0;
        return 0;
    }
    completed = 1;
    rv = sprintf(buffer, "Jiffies time: %lu\n", jiffies);
    
    if (rv < 0) {
        return rv; // Return error code if sprintf fails
    }
    if (raw_copy_to_user(usr_buf, buffer, rv)) {
        return -EFAULT; // Return error code if raw_copy_to_user fails
    }
    return rv;
}

module_init(proc_init);
module_exit(proc_exit);