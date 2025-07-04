#!/bin/bash 

# Variables "macros"
DEVICE="/dev/mi_dispositivo"         # Ruta
NUM_PROCESOS=5
LOCKFILE="/tmp/mi_dispositivo.lock" # Archivo de bloqueo para sincronización, para evitar choques

#Colores
COLOR_GREEN="\033[0;32m"  # mensajes de éxito
COLOR_RED="\033[0;31m"    # mensajes de error
COLOR_RESET="\033[0m"     # Color por defecto

function success() {
    echo -e "${COLOR_GREEN}[✔] $1${COLOR_RESET}" # Mensaje de éxito
}

# Pone los errores en rojo
function error() {
    echo -e "${COLOR_RED}[✘] $1${COLOR_RESET}"
}

# Verifica si el dispositivo existe y es de tipo carácter
if [ ! -c "$DEVICE" ]; then
    error "$DEVICE no existe o no es un dispositivo de caracter."
    exit 1  # Termina todo si el dispositivo no está disponible
fi

# Función para lectura y escritura
function test_io() {
    local id=$1                            # ID del proceso
    local dato=$RANDOM                     # Dato aleatorio (número)
    local msg="Proceso $id dice: $dato"   # Mensaje

    (
        flock -x 200                       # Bloqueo para evitar las colisiones
        echo "[PID $$] Proceso $id escribiendo: '$msg'"
        echo "$msg" > "$DEVICE"           # Escribe el mensaje al dispositivo
        sleep 0.1                          # Pequeña pausa
        local resp=$(dd if="$DEVICE" bs=1 count=64 2>/dev/null)  # Lee hasta 64 bytes
        echo "[PID $$] Proceso $id leyó: '$resp'"
    ) 200>"$LOCKFILE"                      # Se asegura que solo pase un proceso a la vez
}

# Inicia los procesos
echo "Iniciando $NUM_PROCESOS procesos"
for i in $(seq 1 $NUM_PROCESOS); do
    test_io "$i"  # se puede agregar & aquí para que se hagan en paralelo: test_io "$i" & (se decidió no usar)
done

wait  # Espera a que todos los procesos terminen

success "Prueba finalizada"