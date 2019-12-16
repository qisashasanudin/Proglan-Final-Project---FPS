# 3D Open-World Game Sederhana

* [Description](#description)
* [Installation](#installation)
* [How to Play](#howtoplay)
* [License](#license)

## Description
Merupakan program yang berupa game Open World 3D. Pemain dapat bebas menjelajahi area yang telah disiapkan baik itu dengan mode biasa atau mode spectator. Selain itu, pemain juga akan ditemani eksistensi lain dalam game ini. Program game ini memakai library open source OpenGL (freeglut) dalam bahasa C dengan compiler C++. Bentuk kontur tanah dan tekstur area dapat diatur oleh pengguna dengan memodifikasi program. 

## Installation
System Requirements: Windows
<br>
Untuk dapat meng-compile program ini, Anda perlu melakukan hal berikut :
1. Mendownload source-code nya terlebih dahulu dengan cara menekan tombol download disamping.
2. Buka source-code menggunakan C compiler. Jika Anda tidak memilikinya, [download disini](https://sourceforge.net/projects/orwelldevcpp/).
3. Mendowload library OpenGL (freeglut) lalu di-linkan dengan compiler [download disini](https://www.transmissionzero.co.uk/software/freeglut-devel/).
3. Kemudian, jalankan dengan cara meng-compile main-program.cpp atau langsung meng-run main-program.exe.

## How to Play
- Maju            : W
- Mundur          : S
- Jalan ke kiri   : A
- Jalan ke kanan  : D
- Lompat          : Spasi
- Jongkok         : C
- Lari / sprint   : W + E

### Menjelajahi Area
![eksplor](/ss/eksplor.png)
![eksplor2](/ss/eksplor2.png)

### Menjelajahi Area dengan Mode Spectator
![eksplorspec](/ss/eksplorspec.png)

Pemain dapat mengklik tombol |~| pada keyboard untuk masuk ke mode ini. Untuk kembali ke mode biasa pemain dapat mengklik tombol yang sama.
### Exit
Sebelum keluar dari program pemain dapat mempause game dengan mengklik tombol |Esc|. Untuk keluar game, pemain dapat menggunakan kombinasi Alt+F4 (force close) namun dianjurkan untuk mengklik tombol |=| terlebih dahulu. Hal ini ditujukan untuk mengclear memory yang terpakai ketika program dijalankan.

## License

MIT License
<details>
  <summary>Copyright (c) 2019 qisashasanudin & edwiansyah18</summary>

<p align="justify">Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:</p>

<p align="justify">The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.</p>

<p align="justify">The software is provided "as is", without warranty of any kind, express or
Implied, including but not limited to the warranties of merchantability,
Fitness for a particular purpose and noninfringement. In no event shall the
Authors or copyright holders be liable for any claim, damages or other
Liability, whether in an action of contract, tort or otherwise, arising from,
Out of or in connection with the software or the use or other dealings in the
Software.</p>

</details>
