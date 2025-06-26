#!/bin/bash
DEVICE="/dev/mi_dispositivo"

# el dispositivo existe?
if [ ! -c "$DEVICE" ]; then
    echo "Error: $DEVICE no existe o no es un dispositivo de carácter."
    exit 1
fi

# Escribir y leer
echo "[+] Escribiendo al dispositivo..."
echo "TEST$(date)" | sudo tee $DEVICE

echo "[+] Leyendo del dispositivo..."
sudo cat $DEVICE