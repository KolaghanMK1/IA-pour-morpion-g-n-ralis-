#include <iostream>
void affichage_morpion(char** tab, int size) {

	std::cout<<std::endl<<"   ";
	for(int i = 0; i < size; ++i) std::cout<<" "<<i<<"  ";
	std::cout<<std::endl;
	for(int i = 0; i < size; ++i) {

		std::cout<<i<<" | "; 
		for(int j = 0; j < size; ++j) {
	
			std::cout<<tab[i][j]<<" | "; 
		
		}
		std::cout<<std::endl;
	}
}


void init_morpion(char** & tab, int size) {

	tab = new char*[size];
	
	for(int i =0; i < size; ++i) tab[i] = new char[size];

	for(int i =0; i < size; ++i)
	for(int j =0; j < size; ++j) tab[i][j] = ' ';	
	
}


void delete_morpion(char** tab, int size) {

	for(int i =0; i < size; ++i) delete [] tab[i];
	
	delete [] tab;

}

void placer_morpion(char** tab, int size, int x, int y, char pion) {

	if(x < size && x >= 0 && y >= 0 && y < size && tab[x][y]==' ' ) {
		tab[x][y] = pion;
	}

}

bool estLibre_morpion(char** tab, int size, int x, int y) {

	return (x >= 0 && y >= 0 && x < size && y < size && tab[x][y]==' ');

}

bool victoire_morpion(char** tab, int size, int nbpion, char pion) {

	for(int i = 0; i < size; ++i) {
	
		for(int j = 0; j < size; ++j) if (tab[i][j] == pion) {
		
			int align = 0, i2, j2; 
			
			i2 = i; align = 0;
			while( i2 < size && (i2-i) < nbpion ) { align = align + (tab[i2][j] == pion); i2++; }
			if(align == nbpion) return true;
			
			j2 = j; align = 0;
			while( j2 < size && (j2-j) < nbpion ) { align = align + (tab[i][j2] == pion); j2++; }
			if(align == nbpion) return true;
			
			j2 = j; i2 = i; align = 0;
			while( i2 < size && (j2-j) < nbpion && j2 < size && (j2-j) < nbpion) { align = align + (tab[i2][j2] == pion); i2++; j2++; }
			if(align == nbpion) return true;
			
			j2 = j; i2 = i; align = 0;
			while( i2 < size && (j2-j) < nbpion && j2 >=0 && (j-j2) < nbpion) { align = align + (tab[i2][j2] == pion); i2++; j2--; }
			if(align == nbpion) return true;
			
		}
	}
	
	return false;
}

char** copie_morpion(char** tab, int size) {

	char ** cop = new char*[size];
	
	for(int i =0; i < size; ++i) cop[i] = new char[size];

	for(int i =0; i < size; ++i)
	for(int j =0; j < size; ++j) cop[i][j] = tab[i][j];	
	
	return cop;
}


