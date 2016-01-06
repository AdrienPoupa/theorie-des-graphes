#include <iostream>
#include <map>
#include <set>
#include <vector>

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
void copieGraphe(t_graphe * original, t_graphe * copie);
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

void copieGraphe(t_graphe * original, t_graphe * copie){

    copie->nbSommets = original->nbSommets;

    copie->MAdj = new bool * [copie->nbSommets];
    copie->MVal = new int * [copie->nbSommets];

    for (int i = 0; i < copie->nbSommets; i++) {
        copie->MAdj[i] = new bool [copie->nbSommets];
        copie->MVal[i] = new int [copie->nbSommets];
        for (int j = 0; j < copie->nbSommets; j++) {
            copie->MAdj[i][j] = original->MAdj[i][j];
            copie->MVal[i][j] = original->MVal[i][j];
        }
    }
}

void copieGrapheAvecSuppressionSommet(t_graphe * original, t_graphe * copie, int sommet){

    copie->nbSommets = original->nbSommets - 1;

    copie->MAdj = new bool * [copie->nbSommets];
    copie->MVal = new int * [copie->nbSommets];

    int comptJ = 0, comptI = 0;

    for (int i = 0; i < original->nbSommets; i++) {
        if(i != sommet){
            comptJ = 0;
            copie->MAdj[comptI] = new bool [copie->nbSommets];
            copie->MVal[comptI] = new int [copie->nbSommets];
            for (int j = 0; j < original->nbSommets; j++) {
                if(j != sommet){
                    copie->MAdj[comptI][comptJ] = original->MAdj[i][j];
                    copie->MVal[comptI][comptJ] = original->MVal[i][j];
                    comptJ++;
                }
            }
            comptI++;
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

void generateFromFileTask(t_graphe * target){
    // Lecture du graphe sur fichier
    ifstream fg (FICHIER_GRAPHE);

    int nbSommets;

    // Lecture du nombre de sommets et allocation dynamique des SdD
    fg >> nbSommets;
    nbSommets += 2;
    cout << "Nombre sommets: " << nbSommets - 2 << " + 2 (0 et " << nbSommets - 1 <<")"  << endl;

    generateMatriceVide(target, nbSommets);

    // Lecture des arcs
    int extInit, contrainte, duree;

    // Initialisation des contraintes
    set<int> contrainteList = set<int>();
    map<int, int> extInitDuree = map<int, int>();

    for (int i = 1; i < nbSommets-1; i++)
    {
        contrainteList.insert(i);
    }

    // ATTENTION: les contraintes et init démarrent à 1 dans le fichier, 0 dans la matrice

    // lecture du nom du sommet
    fg >> extInit;
    if (extInit != -1)
    {
        cout << "sommet: " << extInit;
        while (extInit != -1)
        {
            fg >> duree;
            cout << ", duree: " << duree;
            extInitDuree[extInit] = duree;
            fg >> contrainte;
            int cCount = 0;
            cout <<  ", contrainte: ";
            while (contrainte != -1)
            {
                contrainteList.erase(contrainte);
                target->MAdj[contrainte][extInit] = true;
                cout << (cCount != 0 ? ", ": "") << contrainte;
                cCount++;
                fg >> contrainte;
            }
            if(contrainte == -1 && cCount == 0)
            {
                cout << "Pas de contraintes ...";
                target->MAdj[0][extInit] = true;
                target->MVal[0][extInit] = 0;
            }
            cout << endl;

            fg >> extInit;
            if (extInit != -1)
            {
                cout << "sommet: " << extInit;
            }

        }
    }
    for(auto const elem: contrainteList){
        target->MAdj[elem][nbSommets-1] = true;
        target->MVal[elem][nbSommets-1] = 0;
    }


    for(int i = 0; i < nbSommets; i++){
        for(int j = 0; j < nbSommets; j++){
            if(target->MAdj[i][j]){
                target->MVal[i][j] = extInitDuree[i];
            }
        }
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

map<int, int> demiDegreAdjacent(t_graphe * graphe){
    map<int, int> aretesEntrantes = map<int, int>();

    for (int x = 0; x < graphe->nbSommets; x++) {
        aretesEntrantes[x] = 0;
    }

    for (int i = 0 ; i < graphe->nbSommets ; i++)
    {
        for (int j = 0 ; j < graphe->nbSommets ; j++)
        {
            if (graphe->MAdj[i][j] == true)
            {
                aretesEntrantes[j]++;
            }
        }
    }

    return aretesEntrantes;
}

int findFirstWhereEntier(map<int, int> m, int v){
    for(auto const elem: m){
        if(elem.second == v) return elem.first;
    }
    return -1;
}

void rang(t_graphe * graphe)
{
    bool circuit = aUnCircuit(graphe);
    t_graphe * workGraphe = new t_graphe();
    t_graphe * tempGraphe = new t_graphe();

    copieGraphe(graphe, workGraphe);

    if (circuit == true)
    {
        // Si le graphe a un circuit, on sort
        cout << "Erreur : le graphe a un circuit" << endl;
        return;
    }

    map<int, int> rangSommets = map<int, int>();
    int rangIte = 0;
    map<int, int> aretesEntrantes;
    int sommet = 0;

    int sommetRealname[graphe->nbSommets];

    for(int n = 0; n < graphe->nbSommets; n++){
        sommetRealname[n] = n;
    }

    while(sommet >= 0){
        aretesEntrantes = demiDegreAdjacent(workGraphe);

//        for (auto const n: aretesEntrantes)
//        {
//            cout << n.first << " : " << n.second << endl;
//        }
        // selection du sommet a traiter (retirer)
        sommet = findFirstWhereEntier(aretesEntrantes, 0);

        if(sommet != -1){
            cout << "sommet : "<< sommetRealname[sommet] << " , rang : " << rangIte << endl;

            bool found = false;
            for(int n = 0; n < graphe->nbSommets; n++){
                if(n == sommet){
                    found = true;
                }
                if(found){
                    sommetRealname[n] = n == graphe->nbSommets - 1 ? 0 : sommetRealname[n+1];
                }
            }

            rangSommets[sommet] = rangIte;
            rangIte++;

            // supprimer ligne et colonne du sommet dans la matrice
            copieGrapheAvecSuppressionSommet(workGraphe, tempGraphe, sommet);

            delete workGraphe;
            workGraphe = tempGraphe;
            tempGraphe = new t_graphe();
        }
    }
}

int main ()
{
    // Déclaration graphe
    t_graphe * G = new t_graphe;

    /*cout << "Generation a partir du fichier" << endl;
    generateFromFile(G);

    cout << "Matrice adjacente originale :" << endl;

    afficheMatriceAdjacente(G);

    //afficheCompletGraphe(G);

    t_graphe * t = new t_graphe;

    // TODO: pointeurs?
    cout << "Transitivite :" << endl;
    transitive(G, t);

    // TODO: trace?
    cout << "Circuit :" << endl;
    bool circuit = aUnCircuit(t);

    if (circuit == true)
    {
        cout << "Le graphe a un circuit" << endl;
    }
    else
    {
        cout << "Le graphe n'a pas de circuit" << endl;
    }

    cout << "Rang" << endl;
    rang(G);*/

    cout << "Generation a partir du fichier" << endl;
    generateFromFileTask(G);

//    cout << "Matrice adjacente originale :" << endl;
//    afficheMatriceAdjacente(G);

    cout << "Matrice complete :" << endl;
    afficheCompletGraphe(G);

    return 1;
}
