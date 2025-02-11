#!/bin/bash

# Autor: Chatgpt

# Directorio base (por defecto el actual)
DIRECTORIO="${1:-.}"

# Buscar y reemplazar en todos los archivos .c y .h
find "$DIRECTORIO" -type f \( -name "*.c" -o -name "*.h" \) -exec  \
sed -i \
-e 's|#include <glad.h>|#include <glad/glad.h>|g' \
-e 's|#include <glfw3.h>|#include <GLFW/glfw3.h>|g' \
{} +

