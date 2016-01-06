#include <iostream>

using namespace std;

#include <fstream>

#define FICHIER_GRAPHE "buob-barbot-poupa.txt"

typedef struct {
    int  nbSommets;
    bool **  MAdj; // MAdj[x][y] = TRUE <==> il existe arc (x,y)
    int ** MVal; // Si MAdj[x][y] = TRUE alors MVal[x][y] = valeur de l’arc (x,y)
} t_graphe;

void generateMatriceVide(t_graphe * target, int nbSommets);
void generateFromFile(t_graphe * target);
void afficheCompletGraphe(t_graphe * target);
void afficheMatriceAdjacente(t_graphe * target);
void afficheMatriceIncidence(t_graphe * target);
t_graphe* transitive(t_graphe * original, t_graphe * target);
bool aUnCircuit(t_graphe * matriceTransitive);


void generateMatriceVide(t_graphe * target, int nbSommets){
    target->nbSommets = nbSommets;
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
}

void generateFromFile(t_graphe * target){
    // Lecture du graphe sur fichier
    ifstream fg (FICHIER_GRAPHE);

    int nbSommets;

    // Lecture du nombre de sommets et allocation dynamique des SdD
    fg >> nbSommets;

    generateMatriceVide(target, nbSommets);

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

t_graphe* transitive(t_graphe * original, t_graphe * target){
    t_graphe * m = new t_graphe;

    generateMatriceVide(m, original->nbSommets);
    generateMatriceVide(target, original->nbSommets);

    for(int n = 2; n <= original->nbSommets - 1; n++){
        for (int i = 0; i < m->nbSommets; i++) {
            for(int j = 0; j < m->nbSommets; j++){
                for(int k = 0; k < m->nbSommets; k++){
                    if(n == 2){
                        m->MAdj[i][j] += (bool)((int)original->MAdj[i][k] * (int)original->MAdj[k][j]);
                    }
                    else{
                        m->MAdj[i][j] += (bool)((int)original->MAdj[i][k] * (int)m->MAdj[k][j]);
                    }
                }
            }
        }

        cout << "m^" << n <<" : " << endl;
        afficheMatriceAdjacente(m);

        for (int i = 0; i < m->nbSommets; i++){
            for(int j = 0; j < m->nbSommets; j ++){
                if(n == 2){
                    target->MAdj[i][j] = m->MAdj[i][j] || original->MAdj[i][j];
                }
                else{
                    target->MAdj[i][j] = m->MAdj[i][j] || target->MAdj[i][j];
                }
            }
        }

        cout << "transitive pour n = " << n << " : " << endl;
        afficheMatriceAdjacente(target);
    }

    return target;
}

bool aUnCircuit(t_graphe * matriceTransitive)
{
    for (int i = 0 ; i < matriceTransitive->nbSommets ; i++)
    {
        if (matriceTransitive->MAdj[i][i] == true)
        {
            // On a un 1 sur la diagonale: il y a un circuit et on sort
            return true;
        }
    }

    // Si on n'est pas sorti jusque là, il y a un circuit
    return false;
}

void rang(t_graphe * graphe)
{
    bool circuit = aUnCircuit(graphe);

    if (circuit == true)
    {
        // Si le graphe a un circuit, on sort
        cout << "Erreur : le graphe a un circuit" << endl;
        return;
    }
}

int main ()
{
    // Déclaration graphe
    t_graphe * G = new t_graphe;

    cout << "Generation a partir du fichier" << endl;
    generateFromFile(G);

    cout << "Matrice adjacente originale :" << endl;

    afficheMatriceAdjacente(G);

    //afficheCompletGraphe(G);

    t_graphe * t = new t_graphe;

    // TODO: pointeurs?
    cout << "Transitivite :" << endl;
    G = transitive(G, t);

    // TODO: trace?
    cout << "Circuit :" << endl;
    bool circuit = aUnCircuit(G);

    if (circuit == true)
    {
        cout << "Le graphe a un circuit" << endl;
    }
    else
    {
        cout << "Le graphe n'a pas de circuit" << endl;
    }

    cout << "Rang" << endl;
    rang(G);

    return 1;
}
