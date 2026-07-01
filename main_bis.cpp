#include <cstdlib> // rand, srand
#include <ctime>   // time
#include <chrono>
#include "IA_Morpion.h"

int main()
{

	std::srand(std::time(nullptr));

	char **tab;
	int size = 0;
	int nbPion = 0;
	bool afficher = false;
	std::cout << "size = ";
	std::cin >> size;
	std::cout << "nbPion = ";
	std::cin >> nbPion;
	std::cout << "Voulez-vous afficher le morpin ? (y pour oui)";
	std::string rep;
	std::cin >> rep;
	afficher = rep == "y";

	init_morpion(tab, size);
	Gammatoe G(size, nbPion);
	std::chrono::nanoseconds total_duration(0);
	int nb_coup = 0;

	bool alignement = false;
	bool joueur = std::rand() % 2;

	int nbPl = 0;

	bool firstmove = true;

	while (!alignement && nbPl < size * size)
	{

		if (firstmove)
		{
			affichage_morpion(tab, size);
		}
		firstmove = false;

		char pion;
		(joueur ? pion = 'X' : pion = 'O');
		joueur = !joueur;

		int x = -1;
		int y = -1;

		if (pion == 'O')
		{

			auto start = std::chrono::steady_clock::now();
			std::pair<int, int> coup = G.play();
			auto end = std::chrono::steady_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
			total_duration += duration;
			x = coup.first;
			y = coup.second;
			std::cout << "Gammatoe a joué en " << duration.count() << "ns\n";
			assert(estLibre_morpion(tab, size, x, y));

			placer_morpion(tab, size, x, y, pion);
			nb_coup++;
		}
		else
		{
			std::cout << "IA Thibault :\n";
			jouerX(tab, G, size, nbPion);
		}

		if (afficher)
			affichage_morpion(tab, size);

		if (pion == 'O')
			alignement = victoire_morpion(tab, size, nbPion, 'O');
		if (pion == 'X')
			alignement = victoire_morpion(tab, size, nbPion, 'X');
		nbPl++;
	}
	auto avg_duration = total_duration / nb_coup;

	std::cout << "AVG duration Gammatoe : " << avg_duration.count() << std::endl;

	delete_morpion(tab, size);
}