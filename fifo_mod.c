#include <linux/init.h>      // Macros __init y __exit
#include <linux/module.h>    // Funciones básicas de módulo
#include <linux/kernel.h>    // printk()
#include <linux/fs.h>        // struct file, filp_open
#include <linux/uaccess.h>   // copy_to_user
#include <linux/slab.h>      // kmalloc y kfree

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mariana");
MODULE_DESCRIPTION("Módulo simple que escribe en un FIFO");
MODULE_VERSION("0.1");

static char *mensaje = "Hola desde el kernel!\n";
static struct file *fifo_file;

static int __init fifo_mod_init(void) {
    
    loff_t pos = 0;

    printk(KERN_INFO "fifo_mod: Cargando módulo...\n");

    

    fifo_file = filp_open("/tmp/mi_fifo", O_WRONLY | O_CREAT, 0666);
    if (IS_ERR(fifo_file)) {
        printk(KERN_ALERT "fifo_mod: No se pudo abrir el FIFO\n");
        
        return PTR_ERR(fifo_file);
    }

    kernel_write(fifo_file, mensaje, strlen(mensaje), &pos);
    filp_close(fifo_file, NULL);

    
    printk(KERN_INFO "fifo_mod: Mensaje escrito en el FIFO\n");
    return 0;
}

static void __exit fifo_mod_exit(void) {
    printk(KERN_INFO "fifo_mod: Saliendo del módulo...\n");
}

module_init(fifo_mod_init);
module_exit(fifo_mod_exit);
