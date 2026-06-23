#include "morpion.h"
#include "Gammatoe_core.cpp"

int main()
{

	char **tab;
	int size = 20;
	int nbPion = 5;

	init_morpion(tab, size);
	Gammatoe AI(size, nbPion);

	bool alignement = false;
	bool joueur = false;

	int nbPl = 0;

	while (!alignement && nbPl < size * size)
	{

		affichage_morpion(tab, size);

		char pion;
		(joueur ? pion = 'X' : pion = 'O');
		joueur = !(joueur);

		int x = -1;
		int y = -1;

		do
		{

			if (pion == 'X')
			{
				auto [x_temp, y_temp] = AI.play();
				x = x_temp;
				y = y_temp;
			}
			else
			{
				std::cin >> x >> y;
			}
			std::cout << std::endl;

		} while (!estLibre_morpion(tab, size, x, y));

		placer_morpion(tab, size, x, y, pion);
		affichage_morpion(tab, size);

		if (pion == 'O')
		{
			AI.play_opponent(x, y);
			alignement = victoire_morpion(tab, size, nbPion, 'O');
		}
		nbPl++;
	}

	delete_morpion(tab, size);
}
