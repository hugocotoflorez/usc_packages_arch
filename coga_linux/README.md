# Como usar linux para coga

### Glad

[download link](https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D3.3&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=compatibility&loader=on)

Para instalarlo globalmente: incluir con <glad/glad.h>
y compolar copilar con -lglad
```sh
sudo mkdir -p /usr/include/glad
sudo mkdir -p /usr/include/KHR
sudo cp glad/include/glad/glad.h /usr/include/glad/
sudo cp glad/include/KHR/khrplatform.h /usr/include/KHR/

gcc -c glad/src/glad.c -o glad.o
ar rcs libglad.a glad.o
sudo mv libglad.a /usr/lib/
```

### glfw3

Se encuentra en GLFW/glfw3.h

## Como compilar

```sh
g++ file.cpp -lglad -lglfw
```

No hace falta incluir `-lGL` ni `-ldl` pero lo pongo
aqui por si da error ld.
