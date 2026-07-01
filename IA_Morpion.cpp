#include<algorithm>
#include"morpion.h"
#include"Gammatoe.h"

int countAligned(char** tab, int N, int x, int y, int dx, int dy, char pawn)
{
    int counter = 0; //compteur de pions
    x += dx; //variable qui déplace la recherche sur un axe horizontal
    y += dy; //variable qui déplace la recherche sur un axe vertical

    while (x >= 0 && x < N && y >= 0 && y < N && tab[x][y] == pawn) //tant que (vérification de la conformiter des positions dans la grille + le pion correspond)
    {
        ++counter; //compteur de pion + 1
        x += dx; //déplacement selon dx
        y += dy; //déplacement selon dy
    }
    return counter;
}
	
void scoreCalculation(char** tab, int N, int K, int x, int y, char pawn, int &score)
{
	//compte les pions alignés horizontalement
	int h1Aligned = countAligned(tab, N, x, y, 1, 0, pawn);
	int h2Aligned = countAligned(tab, N, x, y, -1, 0, pawn);
	int hAligned  = h1Aligned + h2Aligned + 1;
	
	//compte les pions alignés verticalement
	int v1Aligned = countAligned(tab, N, x, y, 0, 1, pawn);
	int v2Aligned = countAligned(tab, N, x, y, 0, -1, pawn);
	int vAligned  = v1Aligned + v2Aligned + 1;
	
	//compte les pions alignés diagonalement
	int d11Aligned = countAligned(tab, N, x, y, 1, 1, pawn);
	int d12Aligned = countAligned(tab, N, x, y, -1, -1, pawn);
	int d1Aligned = d11Aligned + d12Aligned + 1;
	int d21Aligned = countAligned(tab, N, x, y, 1, -1, pawn);
	int d22Aligned = countAligned(tab, N, x, y, -1, 1, pawn);
	int d2Aligned = d21Aligned + d22Aligned + 1;
	
	//valeur maximale de pions alignés
	int maxAligned = std::max({hAligned, vAligned, d1Aligned, d2Aligned});
	
	if (maxAligned >= K)
	{
		if (pawn == 'X')
		{
			score += 20000; //score si le prochain pion posé causerait une victoire
		}
		else
		{
			score += 10000; //score si le prochain pion posé causerait une défaite
		}
	}
	else if (maxAligned == K - 1)
	{
		if (pawn == 'X')
		{
			score += 400; //score si le prochain pion posé -1 causerait une victoire
		}
		else
		{
			score += 300; //score si le prochain pion posé -1 causerait une défaite
		}
	}
	else
	{
		for (int i = K - 2; i > 0; --i)
		{
			if (maxAligned == i)
			{
				score += maxAligned * maxAligned * i; //score basique si aucun cas particulier n'est détecté
			}
			
		}
		
	}
}

void jouerX(char** tab, Gammatoe &G, int N, int K)
{
	int best = -1;
	int x = -1;
	int y = -1;
	
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (!estLibre_morpion(tab, N, i, j)) {continue;}
			
			int score = 0;
			
			scoreCalculation(tab, N, K, i, j, 'O', score); //score de défense
			scoreCalculation(tab, N, K, i, j, 'X', score); //score d'attaque
			
			if (score > best)
			{
				best = score;
				x = i;
				y = j;
			}
		}
	}
	
	if (x != -1 && y != -1 && estLibre_morpion(tab, N, x, y)) {
		G.play_opponent(x,y);
		placer_morpion(tab, N, x, y, 'X');}
}