# Módulo FIFO para Linux Kernel
Este proyecto consiste en la implementación de un módulo del kernel de Linux que crea un dispositivo de carácter accesible desde `/dev`. El módulo permite la comunicación entre el espacio de usuario y el kernel utilizando un buffer FIFO (First In, First Out), con sincronización y manejo de errores.

## Objetivo
Facilitar la interacción entre el kernel y múltiples procesos en user space usando operaciones estándar (`open`, `read`, `write`, `close`) y un buffer seguro y concurrente.

## Estado del proyecto
🔧 En desarrollo

## Pasos para ejecutar proyecto
-Instalar todos los archivos que incluyen:
    
    Makefile
    test_device.c
    test_device.sh
    mi_dispositivo.c
    
-Ejecutar el comando: make

### Para ejecutar el test_device.c
-Ejecutar el comando como root: sudo gcc -o test_device test_device.c

-Ejecutar comando: ./test_device

-Probar las opcines que se muestran en la terminal

### Para ejecutar el test_device.sh
-Ejecutar el comando: chmod +x test_device.sh

-Ejecutar como : ./test_device.sh 

En caso de tener problemas del tipo "el directorio no ha sido encontrado" buscar en el WIKI la solucion



## Requisitos
- Kernel de Linux
- Herramientas de compilación (`make`, `gcc`)
- Permisos de root para cargar módulos

## Autores
- Mariana Bernal Hernández
- Maikel Ordeñana Prado
- Kevin Baules Rojas
