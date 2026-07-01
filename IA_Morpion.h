#include<iostream>
#include"IA_Morpion.cpp"

/*
 * Compte le nombre de pions adverses alignés dans une direction donnée
 * tab : le tableau SizexSize
 * N : la taille du tableau
 * x : la coordonnée de la case sélectionnée sur un axe vertical x
 * y : la coordonnée de la case sélectionnée sur un axe vertical y
 * dx : le facteur qui indique la direction horizontale de la recherche
 * dy : le facteur qui indique la direction verticale de la recherche
 * pawn : le pion recherché
 */
int countAligned(char** tab, int N, int x, int y, int dx, int dy, char pawn);

/*
 * Calcule le score de la case sélectionnée suivant le pion indiqué
 * tab : le tableau SizexSize
 * N : la taille du tableau
 * K : le nombre de pion à aligner pour gagner la partie
 * x : la coordonnée de la case sélectionnée sur un axe vertical x
 * y : la coordonnée de la case sélectionnée sur un axe vertical y
 * pawn : le pion recherché
 * score : le score de la case actuellement sélectionnée
 */
void scoreCalculation(char** tab, int N, int K, int x, int y, char pawn, int &score);

/*
 * Code de l'IA
 * tab : le tableau SizexSize
 * N : la taille du tableau
 * K : le nombre de pion à aligner pour gagner la partie
 */
void jouerX(char** tab, Gammatoe &G, int N, int K);