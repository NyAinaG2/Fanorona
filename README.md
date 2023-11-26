# Fanorona
Fanorona Game made with C++

Ceci est mon premier jeu vidéo qui a été réalisé dans le cadre de mon parcours scolaire
Ce jeu a été programmé en C++
Il s’agit d’une adaptation numérique du jeu de société malgache « Fanorona »

Je tiens à preciser que le jeu comporte certaines imperfections,que ce soit au niveau du graphisme ou du mécanisme

Courage à vous dans l'exploration du code

Pour tout commentaire, veillez contacter à l'adresse-mail: adrandriamanga@gmail.com

Pour la c*mpilation du programme
-------------------Commande pour générer le fichier objet Fanorona.o-------------------------------------
g++.exe -Wall -g -c Fanorona.cpp -o Fanorona.o

-------------------Commande pour générer l'exécutable Fanorona_MISEI_2023.exe----------------------------
g++.exe -o Fanorona_MISEI_2023.exe Fanorona.o -lgdi32 -luser32 -lkernel32 -lcomctl32 -mwindows
