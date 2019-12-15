# 3D Open-World Game Sederhana

* [Description](#description)
* [Installation](#installation)
* [How to Play](#howtoplay)
* [License](#license)

## Description
Merupakan program yang berupa game Open World 3D. Pemain dapat bebas menjelajahi area yang telah disiapkan baik itu dengan mode biasa atau mode spectator. Selain itu, pemain juga akan ditemani eksistensi lain dalam game ini. Program game ini memakain OpenGL dalam bahasa C dengan compiler C++. Bentuk kontur tanah dan tekstur area dapat diatur oleh pengguna dengan memodifikasi program. 

## Installation
System Requirements: Windows
<br>
Untuk dapat menggunakan Hangman Battle, Anda perlu melakukan hal berikut :
1. Mendownload source-code nya terlebih dahulu dengan cara menekan tombol download disamping.
2. Buka source-code menggunakan C compiler. Jika Anda tidak memilikinya, [download disini](https://sourceforge.net/projects/orwelldevcpp/files/latest/download)
3. Mendowload library OpenGL lalu di-linkan dengan compiler.
3. Kemudian, jalankan dengan cara meng-compile main-program.cpp atau langsung meng-run main-program.exe.

## Usage
### Menu
![Menu](/ss/menu.png)

Pengguna akan masuk ke tampilan menu,.pilih play untuk memainkan game.
### Input Player's Name
![Input Nama](/ss/inputNama.png)

Kedua pemain memasukan nama yang dipergunakan dalam game.
### Give a Question
![Question](/ss/gibQues.png)

Salah satu pemain akan ditunjuk sebagai 'penanya' secara random kemudian memberikan kata untuk ditebak kepada lawan.
### Guess the Word
![Guess](/ss/gibAns.png)
![Guess](/ss/gibAns2.png)

Pemain penjawab diharuskan untuk menjawab. Tebakan dapat dilakukan dengan menginput satu huruf atau lebih.
### Swap the Role
![Swap](/ss/gibQues2.png)

Jika penjawab berhasil menebak kata, posisi para pemain akan di swap. Penanya akan menjadi penjawab dan penjawab akan menjadi penanya.
### Guess the Word Until Win
![Lose](/ss/loseAns2.png)

Gambar Hangman akan terbentuk jika penjawab salah menebak kata, pemain hanya memiliki toleransi salah menebak maksimal lima kali.
## We Got the Winner
![Winner](/ss/lose.png)

Pemenang akan didapatkan jika penjawab gagal menebak kata yang diberikan dan telah melewati batas maksimal lima kali salah menebak.
## Exit

Ketika game telah selesai, tampilan akan dialihkan ke menu. Pilih exit untuk keluar dari aplikasi.

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
