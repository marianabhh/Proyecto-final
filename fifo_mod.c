
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>              // Funciones de sistema de archivos
#include <linux/cdev.h>            // Para crear el char device
#include <linux/device.h>          // Para clase y dispositivo
#include <linux/uaccess.h>         // Para copy_to_user / copy_from_user
#include <linux/kfifo.h>           // FIFO circular del kernel
#include <linux/mutex.h>           // Mutex para sincronización

// Definir el nombre del dispositivo, clase y tamaño del buffer FIFO
#define DEVICE_NAME "mi_dispositivo"
#define CLASS_NAME  "mi_clase"
#define FIFO_SIZE   4096  // 4 KB

// Información básica del módulo
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mariana");
MODULE_DESCRIPTION("Dispositivo de carácter con buffer FIFO sincronizado");
MODULE_VERSION("1.0");


static dev_t dev_num;                             // Número de dispositivo
static struct cdev mi_cdev;                       // Estructura cdev
static struct class *mi_clase = NULL;             // Clase del dispositivo
static struct device *mi_dispositivo = NULL;      // Representación en /dev

static DEFINE_KFIFO(mi_fifo, char, FIFO_SIZE);    // FIFO circular
static DEFINE_MUTEX(mi_mutex);                    // Mutex para acceso concurrente

// open, registra cuando se abre el dispositivo
static int mi_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s: dispositivo abierto\n", DEVICE_NAME);
    return 0;
}

// release, registra cuando se cierra el dispositivo
static int mi_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s: dispositivo cerrado\n", DEVICE_NAME);
    return 0;
}

// read
static ssize_t mi_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    int ret;
    unsigned int copied;

    // Intenta adquirir el mutex. Si el proceso fue interrumpido, retorna error.
    if (mutex_lock_interruptible(&mi_mutex))
        return -ERESTARTSYS;

    // Si el FIFO está vacío, se libera el mutex y se retorna 0 (EOF).
    if (kfifo_is_empty(&mi_fifo)) {
        mutex_unlock(&mi_mutex);
        return 0; // No hay más datos que leer
    }

    // Copia datos del FIFO del kernel al buffer del usuario
    ret = kfifo_to_user(&mi_fifo, buf, len, &copied);

    // Libera el mutex
    mutex_unlock(&mi_mutex);

    // Si hubo error al copiar, devuelve ese error. Si no, devuelve la cantidad copiada.
    return ret ? ret : copied;
}


// write
static ssize_t mi_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    int ret;
    unsigned int copied;

    // Adquiere el mutex (bloquea hasta que esté disponible o interrumpido)
    if (mutex_lock_interruptible(&mi_mutex))
        return -ERESTARTSYS;

    // Si no hay espacio disponible en el FIFO, retorna error
    if (kfifo_avail(&mi_fifo) == 0) {
        mutex_unlock(&mi_mutex);
        return -ENOSPC; // Buffer lleno
    }

    // Copia datos desde el buffer del usuario al FIFO del kernel
    ret = kfifo_from_user(&mi_fifo, buf, len, &copied);

    // Libera el mutex
    mutex_unlock(&mi_mutex);

    // Devuelve error si ocurrió; si no, devuelve cuántos bytes se copiaron
    return ret ? ret : copied;
}


// file_operations
static const struct file_operations fops = {
    .owner   = THIS_MODULE,  // El módulo actual es el dueño del dispositivo
    .open    = mi_open,      // Función a llamar cuando se abre
    .release = mi_release,   // Función a llamar cuando se cierra
    .read    = mi_read,      // Función de lectura
    .write   = mi_write,     // Función de escritura
};


static int __init mi_init(void) {
    int ret;

    // 1. Reserva un número de dispositivo mayor y menor dinámicamente
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    // 2. Inicializa la estructura cdev con las operaciones
    cdev_init(&mi_cdev, &fops);
    mi_cdev.owner = THIS_MODULE;

    // 3. Registra el cdev en el sistema
    ret = cdev_add(&mi_cdev, dev_num, 1);
    if (ret < 0)
        goto unregister_dev;

    // 4. Crea una clase para que aparezca en /sys/class
    mi_clase = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(mi_clase)) {
        ret = PTR_ERR(mi_clase);
        goto del_cdev;
    }

    // 5. Crea el archivo de dispositivo en /dev
    mi_dispositivo = device_create(mi_clase, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(mi_dispositivo)) {
        ret = PTR_ERR(mi_dispositivo);
        goto destroy_class;
    }

    // 6. Inicializa el mutex y el FIFO
    mutex_init(&mi_mutex);
    INIT_KFIFO(mi_fifo);

    printk(KERN_INFO "%s: módulo cargado\n", DEVICE_NAME);
    return 0;

    // Manejo de errores
destroy_class:
    class_destroy(mi_clase);
del_cdev:
    cdev_del(&mi_cdev);
unregister_dev:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}


static void __exit mi_exit(void) {
    device_destroy(mi_clase, dev_num);         // Elimina /dev/mi_dispositivo
    class_destroy(mi_clase);                   // Elimina la clase de sysfs
    cdev_del(&mi_cdev);                        // Elimina el cdev registrado
    unregister_chrdev_region(dev_num, 1);      // Libera los números de dispositivo
    mutex_destroy(&mi_mutex);                  // Destruye el mutex
    printk(KERN_INFO "%s: módulo descargado\n", DEVICE_NAME);
}


module_init(mi_init);  // Función que se ejecuta al insertar el módulo
module_exit(mi_exit);  // Función que se ejecuta al remover el módulo
=======
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
    mm_segment_t old_fs;
    loff_t pos = 0;

    printk(KERN_INFO "fifo_mod: Cargando módulo...\n");

    // Permitir acceso a espacio de usuario
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    fifo_file = filp_open("/tmp/mi_fifo", O_WRONLY | O_CREAT, 0666);
    if (IS_ERR(fifo_file)) {
        printk(KERN_ALERT "fifo_mod: No se pudo abrir el FIFO\n");
        set_fs(old_fs);
        return PTR_ERR(fifo_file);
    }

    kernel_write(fifo_file, mensaje, strlen(mensaje), &pos);
    filp_close(fifo_file, NULL);

    set_fs(old_fs);
    printk(KERN_INFO "fifo_mod: Mensaje escrito en el FIFO\n");
    return 0;
}

static void __exit fifo_mod_exit(void) {
    printk(KERN_INFO "fifo_mod: Saliendo del módulo...\n");
}

module_init(fifo_mod_init);
module_exit(fifo_mod_exit);

