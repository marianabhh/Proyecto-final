# Módulo FIFO para Linux Kernel
Este proyecto consiste en la implementación de un módulo del kernel de Linux que crea un dispositivo de carácter accesible desde `/dev`. El módulo permite la comunicación entre el espacio de usuario y el kernel utilizando un buffer FIFO (First In, First Out), con sincronización y manejo de errores.

## Objetivo
Facilitar la interacción entre el kernel y múltiples procesos en user space usando operaciones estándar (`open`, `read`, `write`, `close`) y un buffer seguro y concurrente.

## Estado del proyecto
🔧 En desarrollo

## Requisitos
- Kernel de Linux
- Herramientas de compilación (`make`, `gcc`)
- Permisos de root para cargar módulos

## Autores
- Mariana Bernal Hernández
- Maikel Ordeñana Prado
- Kevin Baules Rojas
