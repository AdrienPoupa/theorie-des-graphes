#include <iostream>

using namespace std;

#include <fstream>

#define FICHIER_GRAPHE "buob-barbot-poupa.txt"

typedef struct {
    int  nbSommets;
    bool **  MAdj; // MAdj[x][y] = TRUE <==> il existe arc (x,y)
    int ** MVal; // Si MAdj[x][y] = TRUE alors MVal[x][y] = valeur de l’arc (x,y)
} t_graphe;

int main () {
    // Déclaration graphe
    t_graphe * G = new t_graphe;

    // Lecture du graphe sur fichier
    ifstream fg (FICHIER_GRAPHE);

    // Lecture du nombre de sommets et allocation dynamique des SdD
    fg >> G->nbSommets;

    G->MAdj = new bool * [G->nbSommets];
    for (int s = 0; s < G->nbSommets; s++) {
        G->MAdj[s] = new bool [G->nbSommets];
        for (int extTerm = 0; extTerm < G->nbSommets; extTerm++) {
            G->MAdj[s][extTerm] = false;
        }
    }

    G->MVal = new int * [G->nbSommets];

    for (int s = 0; s < G->nbSommets; s++) {
        G->MVal[s] = new int [G->nbSommets];
        for (int extTerm = 0; extTerm < G->nbSommets; extTerm++) {
            G->MVal[s][extTerm] = 0;
        }
    }

    // Lecture des arcs
    int extInit, extTerm, valeur;

    fg >> extInit;
    while (extInit != -1) {
        fg >> extTerm;
        fg >> valeur;
        G->MAdj[extInit][extTerm] = true;
        G->MVal[extInit][extTerm] = valeur;
        fg >> extInit;
    }

    // Impression graphe
    for (int x = 0; x < G->nbSommets; x++) {
        cout << x << "\t";
        for (int y = 0; y < G->nbSommets; y ++) {
            if (G->MAdj[x][y] == true) {
                cout << "X\t";
            } else {
                cout << "\t";
            }
        }
        cout << endl;
    }

    for (int x = 0; x < G->nbSommets; x++) {
        cout << x << "\t";
        for (int y = 0; y < G->nbSommets; y ++) {
            if (G->MAdj[x][y] == true) {
                cout << G->MVal[x][y] << "\t";
            } else {
                cout << "\t";
            }
        }
        cout << endl;
    }

    system("PAUSE");
    return 1;
}
