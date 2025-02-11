#!/bin/bash

# Autor: Chatgpt
#  UNTESTED!!!

find . -type f \( -name "*.c" -o -name "*.h" \) -exec  \
sed -i \
-e 's|#include <glad.h>|#include <glad/glad.h>|g' \
-e 's|#include <glfw3.h>|#include <GLFW/glfw3.h>|g' \
{} +

