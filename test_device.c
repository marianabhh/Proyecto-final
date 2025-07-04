#include <stdio.h>      // printf, perror, fgets
#include <stdlib.h>     // EXIT_SUCCESS, EXIT_FAILURE
#include <fcntl.h>      // open
#include <unistd.h>     // read, write, close
#include <string.h>     // strlen
#include <errno.h>      // strerror

#define DEVICE_PATH "/dev/mi_dispositivo"  // Ruta al dispositivo, sin esto no funciona
#define BUFFER_SIZE 1024                   // Tamaño del buffer de lectura/escritura

// Función para escribir un mensaje al dispositivo
void write_to_device(int fd) {
    char buffer[BUFFER_SIZE];
    printf("Ingrese el mensaje a enviar al kernel: ");
    fgets(buffer, BUFFER_SIZE, stdin);  // Captura la entrada del usuario

    // Escribe el mensaje al dispositivo
    ssize_t bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written < 0) {
        perror("Error al escribir");
    } else {
        printf("¡Escritos %zd bytes al dispositivo!\n", bytes_written);
    }
}

//Función para leer un mensaje desde el dispositivo
void read_from_device(int fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE - 1);  // -1 para dejar espacio para "\0"

    if (bytes_read < 0) {
        perror("Error al leer");
    } else {
        buffer[bytes_read] = '\0';  // Asegura que el string esté terminado en null
        printf("Mensaje del kernel: %s\n", buffer);
    }
}

// Menú interactivo para el usuario
void show_menu(int fd) {
    int choice;
    do {
        printf("\n Menú de Pruebas \n");
        printf("1. Escribir al dispositivo\n");
        printf("2. Leer del dispositivo\n");
        printf("3. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &choice);
        getchar();  // Limpia el '\n' que queda en el buffer

        switch (choice) {
            case 1:
                write_to_device(fd);
                break;
            case 2:
                read_from_device(fd);
                break;
            case 3:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción inválida.\n");
        }
    } while (choice != 3); // Para que se ingrese sólo los números válidos
}

// Función principal
int main() {
    // Abre el dispositivo con permisos de lectura y escritura
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "No se pudo abrir %s. Error: %s\n", DEVICE_PATH, strerror(errno));
        return EXIT_FAILURE;
    }

    printf("¡Dispositivo %s abierto correctamente!\n", DEVICE_PATH);
    show_menu(fd);  // Lanza el menú interactivo

    close(fd);  // Cierra el dispositivo
    return EXIT_SUCCESS;
}