# Image Filtering

**MECANISM DE FUNCTIONARE**

In implementarea aplicatiei, am folosit MPI, distribuind
procesul de filtrare a imaginilor pe diferite procese.
Fiecare proces primea o bucata din imaginea originala
(un numar de linii din aceasta) + marginile acestei bucati
si aplica pentru fiecare pixel filtrarea rezultata din
inmultirea matricii imaginii cu matricea filtrului 3x3.
Rezultatul este salvat intr-o noua imagine care este
pregatita pentru aplicarea unui nou filtru.
In implementarea mea, procesul cu rank 0 (MASTER) are
rolul de a citi imaginea din fisier si de a o imparti
intre procese. Inainte de aplicarea filtrelor, MASTER-ul
trimite fiecarui proces o portiune din imaginea citita.
Pentru fiecare filtru, MASTER-ul trimite celorlalte 
procese marginile bucatilor de imagine pe care acestea le au. 
Apoi, fiecare dintre procese (inclusiv MASTER-ul) va
aplica filtrarea pe propria portiune de imagine si va
trimite inapoi MASTER-ului marginile portiunii imaginii filtrate.
Procesul se repeta pentru fiecare filtru in parte. Dupa aplicarea 
tuturor filtrelor, MASTER-ul va recolta portiunile de imagini 
si va recompune imaginea. Dupa aplicarea tuturor filtrelor, 
MASTER-ul are rolul de a scrie in fisier imaginea finala.


**INSTRUCTIUNI DE RULARE A TEMEI**

Pentru a compila tema se recomanda apelarea comenzii "make". 
De asemenea, Makefile-ul din arhiva pune la dispozitie 
comanda "make clean" pentru stergerea executabilului si a
fisierelor create de program (.pgm/.pnm). Pentru a rula
tema se foloseste comanda: 

mpirun -np <N> ./tema3 <image_in> <image_out> <filter1> <filter2> ... <filterX>


**SPECIFICATIILE MASINII LOCALE**
Masina fizica pe care a fost scrisa si testata tema este
un laptop ASUS F542UN-DM017 cu urmatoarele specificatii:
* CPU: Intel Core i7 8500U, 4.00 GHz - Quad Core
* RAM: 8 GB

**ANALIZA SCALABILITATII APLICATIEI**

Pentru analiza scalabilitatii, am rulat aplicatia pe cea
mai mare imagine alb-negru si color pentru 1, 2, 3, respectiv 4 procese.
M-am oprit la 4 deoarece masina fizica pe care am testat are 4 nuclee.
Pentru fiecare test am rulat de 10 ori si am facut o medie a timpului.
Pentru fiecare test s-au aplicat urmatoarele filtre: blur smooth sharpen 
emboss mean blur smooth sharpen emboss mean.

1. Imagine alb-negru (.pgm) - rorschach.pgm
* Testul 1 - 1 proces ......................... 2,783 s 
* Testul 2 - 2 procese ........................ 2,008 s
* Testul 3 - 3 procese ........................ 1,687 s
* Testul 4 - 4 procese ........................ 1,650 s

2. Imagine color (.pnm) - landscape.pnm
* Testul 1 - 1 proces ......................... 13,804 s
* Testul 2 - 2 procese ........................ 12,377 s
* Testul 3 - 3 procese ........................ 11,123 s
* Testul 4 - 4 procese ........................  9,604 s
