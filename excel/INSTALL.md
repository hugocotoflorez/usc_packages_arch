# Excel 2021

Descargamos el instalador de office desde una pagina
de dudosa legitimidad
```sh
wget https://officecdn.microsoft.com/db/492350F6-3A01-4F97-B9C0-C7C6DDF67D60/media/es-ES/HomeStudent2021Retail.img
```

Se necesita tener wine

Montamos la imagen para completar la instalacion con wine
```sh
sudo mount -o loop HomeStudent2021Retail.img /mnt
cd /mnt
wine setup.exe
```
> [!CAUTION]
> A mi no me funciona el instalador

