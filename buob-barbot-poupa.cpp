#include <iostream>

using namespace std;

#include <fstream>

#define FICHIER_GRAPHE "buob-barbot-poupa.txt"

typedef struct {
    int  nbSommets;
    bool **  MAdj; // MAdj[x][y] = TRUE <==> il existe arc (x,y)
    int ** MVal; // Si MAdj[x][y] = TRUE alors MVal[x][y] = valeur de l’arc (x,y)
} t_graphe;

void generateFromFile(t_graphe * target){
    // Lecture du graphe sur fichier
    ifstream fg (FICHIER_GRAPHE);
    
    // Lecture du nombre de sommets et allocation dynamique des SdD
    fg >> target->nbSommets;
    
    target->MAdj = new bool * [target->nbSommets];
    for (int s = 0; s < target->nbSommets; s++) {
        target->MAdj[s] = new bool [target->nbSommets];
        for (int extTerm = 0; extTerm < target->nbSommets; extTerm++) {
            target->MAdj[s][extTerm] = false;
        }
    }
    
    target->MVal = new int * [target->nbSommets];
    
    for (int s = 0; s < target->nbSommets; s++) {
        target->MVal[s] = new int [target->nbSommets];
        for (int extTerm = 0; extTerm < target->nbSommets; extTerm++) {
            target->MVal[s][extTerm] = 0;
        }
    }
    
    // Lecture des arcs
    int extInit, extTerm, valeur;
    
    fg >> extInit;
    while (extInit != -1) {
        fg >> extTerm;
        fg >> valeur;
        target->MAdj[extInit][extTerm] = true;
        target->MVal[extInit][extTerm] = valeur;
        fg >> extInit;
    }
}

void afficheMatriceAdjacente(t_graphe * target){
    // Impression graphe
    for (int x = 0; x < target->nbSommets; x++) {
        cout << x << "\t";
        for (int y = 0; y < target->nbSommets; y ++) {
            if (target->MAdj[x][y] == true) {
                cout << "X\t";
            } else {
                cout << "\t";
            }
        }
        cout << endl;
    }
}

void afficheMatriceIncidence(t_graphe * target){
    for (int x = 0; x < target->nbSommets; x++) {
        cout << x << "\t";
        for (int y = 0; y < target->nbSommets; y ++) {
            if (target->MAdj[x][y] == true) {
                cout << target->MVal[x][y] << "\t";
            } else {
                cout << "\t";
            }
        }
        cout << endl;
    }
}

void afficheCompletGraphe(t_graphe * target){
    afficheMatriceAdjacente(target);
    afficheMatriceIncidence(target);
}

int main () {
    // Déclaration graphe
    t_graphe * G = new t_graphe;

    generateFromFile(G);
    
    afficheCompletGraphe(G);

    return 1;
}
