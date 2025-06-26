#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DEVICE_PATH "/dev/mi_dispositivo"
#define BUFFER_SIZE 1024

// Función para escribir en el dispositivo
void write_to_device(int fd) {
    char buffer[BUFFER_SIZE];
    printf("Ingrese el mensaje a enviar al kernel: ");
    fgets(buffer, BUFFER_SIZE, stdin); // Lee incluso espacios

    ssize_t bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written < 0) {
        perror("Error al escribir");
    } else {
        printf("¡Escritos %zd bytes al dispositivo!\n", bytes_written);
    }
}

// Función para leer desde el dispositivo
void read_from_device(int fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
    
    if (bytes_read < 0) {
        perror("Error al leer");
    } else {
        buffer[bytes_read] = '\0'; // Null-terminate
        printf("Mensaje del kernel: %s\n", buffer);
    }
}

// Menú interactivo
void show_menu(int fd) {
    int choice;
    do {
        printf("\n Menú de Pruebas \n");
        printf("1. Escribir al dispositivo\n");
        printf("2. Leer del dispositivo\n");
        printf("3. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &choice);
        getchar(); // Limpia el '\n' del buffer

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
    } while (choice != 3);
}

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "No se pudo abrir %s. Error: %s\n", DEVICE_PATH, strerror(errno));
        return EXIT_FAILURE;
    }

    printf("¡Dispositivo %s abierto correctamente!\n", DEVICE_PATH);
    show_menu(fd);

    close(fd);
    return EXIT_SUCCESS;
}