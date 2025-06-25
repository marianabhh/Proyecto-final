# Nombre del módulo
obj-m := fifo_mod.o

# Ruta al kernel actual
KDIR := /lib/modules/$(shell uname -r)/build

# Directorio actual
PWD := $(shell pwd)

# Compilación
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Limpieza de archivos generados
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
