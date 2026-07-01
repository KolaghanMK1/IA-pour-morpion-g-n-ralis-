#include<iostream>
#include"morpion.cpp"



/* 
 * Affichage du tableau
 * tab : le tableau SizexSize
 * size : la taille du tableau
 */
void affichage_morpion(char** tab, int size);

/* 
 * Initialise le tableau
 * tab : le tableau SizexSize
 * size : la taille du tableau
 */
void init_morpion(char** & tab, int size);


/* 
 * Supprime le tableau
 * tab : le tableau SizexSize
 * size : la taille du tableau
 */
void delete_morpion(char** tab, int size);



/* 
 * Place un pion dans une case si elle est libre
 * tab : le tableau SizexSize
 * size : la taille du tableau
 * x : ligne où placer le pion
 * y : colonne où placer le pion
 * pion : pion à placer 
 */
void placer_morpion(char** tab, int size, int x, int y, char pion);


/* 
 * Vérifie si la case est libre et adjacent à un pion
 * tab : le tableau SizexSize
 * size : la taille du tableau
 * x : ligne où placer le pion
 * y : colonne où placer le pion
 */
bool estLibre_morpion(char** tab, int size, int x, int y, int pion);


/* 
 * Vérifie si un joueur peut encore jouer
 * tab : le tableau SizexSize
 * nbPion : nombre de pions à aligner
 * size : la taille du tableau
 * pion : le pion du joueur
 */
bool victoire_morpion(char** tab, int size, int nbPion, char pion);

/* 
 * Vérifie si on peut jouer un pion sur une case donnée
 * tab : le tableau SizexSize
 * size : la taille du tableau
 * x : ligne où placer le pion
 * y : colonne où placer le pion
 * pion : pion à placer 
 */
bool voisin_morpion(char** tab, int size, int x, int y, char pion);

/* 
 * Retourne un pointeur vers un tableau Size x Size qui est copie tu tableau tab
 * tab : le tableau SizexSize
 * size : la taille du tableau
 */
char** copie_morpion(char** tab, int size);
