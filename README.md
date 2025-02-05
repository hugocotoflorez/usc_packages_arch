# usc packages arch

Guia para instalar la mierda que mandan en arch

- [StarUML 6.1.2](./StarUML/INSTALL.md)
- [Nvim](https://github.com/hugocotoflorez/nvim)
- [Excel](./excel/INSTALL.md)

# quickstart

Como clonar el repo
```sh
git clone https://github.com/hugocotoflorez/usc_packages_arch
```
o por ssh
```sh
git clone git@github.com:hugocotoflorez/usc_packages_arch
```

# wine

```sh
sudo pacman -S wine
```

Si el paquete no se encuentra, habilita multilib:

```sh
sudo nvim /etc/pacman.conf
```
busca `# [multilib]` y descomenta esa seccion
y el include de debajo.
