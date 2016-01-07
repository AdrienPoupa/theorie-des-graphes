#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

#include <fstream>

#define FICHIER_GRAPHE "buob-barbot-poupa-rang.txt"

typedef struct {
    int  nbSommets;
    bool **  MAdj; // MAdj[x][y] = TRUE <==> il existe arc (x,y)
    int ** MVal; // Si MAdj[x][y] = TRUE alors MVal[x][y] = valeur de l’arc (x,y)
} t_graphe;

void generateMatriceVide(t_graphe * target, int nbSommets);
void generateFromFile(t_graphe * target);
map<int, int> generateFromFileTask(t_graphe * target);
void copieGraphe(t_graphe * original, t_graphe * copie);
void afficheCompletGraphe(t_graphe * target);
void afficheMatriceAdjacente(t_graphe * target);
void afficheMatriceIncidence(t_graphe * target);
void transitive(t_graphe * original, t_graphe * target, bool display = false);
bool aUnCircuit(t_graphe * matriceTransitive);
void affichageRang(map<int, int> rS);
map<int, int> demiDegreAdjacent(t_graphe * graphe);
int findFirstWhereEntier(map<int, int> m, int v);
map<int, int> rang(t_graphe * graphe);
int dateAuPlusTot(t_graphe * graphe, int sommet, map<int, int> dureeSommet);
void calendrierAuPlusTot(t_graphe * graphe, map<int, int> dureeSommet);
int dateAuPlusTard(t_graphe * graphe, int sommet, map<int, int> dureeSommet);
void calendrierAuPlusTard(t_graphe * graphe, map<int, int> dureeSommet);
bool validation(t_graphe * graphe);


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

map<int, int> generateFromFileTask(t_graphe * target){
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

    return extInitDuree;
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

void transitive(t_graphe * original, t_graphe * target, bool display){
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
                if(n == 2){
                    target->MAdj[i][j] = m->MAdj[i][j] || original->MAdj[i][j];
                }
                else{
                    target->MAdj[i][j] = m->MAdj[i][j] || target->MAdj[i][j];
                }
            }
        }

        if (display)
        {
            cout << "m^" << n <<" : " << endl;
            afficheMatriceAdjacente(m);

            cout << "transitive pour n = " << n << " : " << endl;
            afficheMatriceAdjacente(target);
        }
    }
}

bool aUnCircuit(t_graphe * matriceTransitive){
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

map<int, int> rang(t_graphe * graphe)
{
    bool circuit = aUnCircuit(graphe);
    t_graphe * workGraphe = new t_graphe();
    t_graphe * tempGraphe = new t_graphe();

    copieGraphe(graphe, workGraphe);

    map<int, int> rangSommets = map<int, int>();

    if (circuit == true)
    {
        // Si le graphe a un circuit, on sort
        cout << "Erreur : le graphe a un circuit" << endl;
    }
    else{
        int rangIte = 0;
        map<int, int> aretesEntrantes;
        int sommet = 0;

        int sommetRealname[graphe->nbSommets];

        for(int n = 0; n < graphe->nbSommets; n++){
            sommetRealname[n] = n;
        }

        while(sommet >= 0){
            aretesEntrantes = demiDegreAdjacent(workGraphe);

            // selection du sommet a traiter (retirer)
            sommet = findFirstWhereEntier(aretesEntrantes, 0);

            if(sommet != -1){
                rangSommets[sommetRealname[sommet]] = rangIte;

                // correct sommet name available
                bool found = false;
                for(int n = 0; n < graphe->nbSommets; n++){
                    if(n == sommet){
                        found = true;
                    }
                    if(found){
                        sommetRealname[n] = (n == graphe->nbSommets - 1 ? 0 : sommetRealname[n+1]);
                    }
                }

                rangIte++;

                // supprimer ligne et colonne du sommet dans la matrice
                copieGrapheAvecSuppressionSommet(workGraphe, tempGraphe, sommet);

                delete workGraphe;
                workGraphe = tempGraphe;
                tempGraphe = new t_graphe();
            }
        }
    }

    return rangSommets;
}

void affichageRang(map<int, int> rS){
    for(auto const elem: rS){
        cout << "sommet : "<< elem.first << " , rang : " << elem.second << endl;
    }
}

int dateAuPlusTot(t_graphe * graphe, int sommet, map<int, int> dureeSommet){
    // date au plus tot = max(date au plus tot predecesseurs + duree predecesseur)
    // chercher sommets predecesseurs
    set<int> pred = set<int>();
    for(int i = 0; i < graphe->nbSommets; i++){
        if(graphe->MAdj[i][sommet])
            pred.insert(i);
    }
    // selectionner le max de datePred
    set<int> datePred = set<int>();
    for(auto const elem: pred){
        datePred.insert(dureeSommet[elem] + dateAuPlusTot(graphe, elem, dureeSommet));
    }
    if(datePred.empty())
        return 0;
    else
        return *datePred.rbegin(); // rbegin = dernier élément
}

void calendrierAuPlusTot(t_graphe * graphe, map<int, int> dureeSommet){
    map<int, int> rangS = rang(graphe);
    affichageRang(rangS);
    map<int, int> datesSommet = map<int, int>();
    // pour chaque sommet, on calcule sa date au plus tot
    for(auto const elem: rangS){
        datesSommet[elem.first] = dateAuPlusTot(graphe, elem.first, dureeSommet);
    }

    for(auto const elem: datesSommet){
        cout << "sommet: " << elem.first << ", date au plus tot : " << elem.second << endl;
    }

}

int dateAuPlusTard(t_graphe * graphe, int sommet, map<int, int> dureeSommet){
    // date au plus tard = min(dateAuPlusTard(successeurs)) - dureeSommet[sommet]
    // on cherche les successeur
    set<int> succ = set<int>();
    for(int i = 0; i < graphe->nbSommets; i++){
        if(graphe->MAdj[sommet][i]) succ.insert(i);
    }
    // selectionner le min de succ
    set<int> dateSucc = set<int>();
    for(auto const elem: succ){
        dateSucc.insert(dateAuPlusTard(graphe, elem, dureeSommet) - dureeSommet[sommet]);
    }
    if(dateSucc.empty())
        return dateAuPlusTot(graphe, sommet, dureeSommet);
    else
        return *dateSucc.begin();
}

void calendrierAuPlusTard(t_graphe * graphe, map<int, int> dureeSommet){
    map<int, int> rangS = rang(graphe);
    affichageRang(rangS);
    map<int, int> datesSommet = map<int, int>();
    // pour chaque sommet, on calcul sa date au plus tard
    for(auto const elem: rangS){
        datesSommet[elem.first] = dateAuPlusTard(graphe, elem.first, dureeSommet);
    }

    for(auto const elem: datesSommet){
        cout << "sommet: " << elem.first << ", date au plus tard : " << elem.second << endl;
    }
}

bool validation(t_graphe * graphe)
{
    cout << "Validation du graphe" << endl;

    bool valide = true;

    // a) 1 seul point d'entrée
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

    int compteurEntrees = 0, entree;

    for (int x = 0; x < graphe->nbSommets; x++) {
        if (aretesEntrantes[x] == 0) // Si on n'a que des 0 sur une colonne, c'est une entrée
        {
            compteurEntrees++;
            entree = x; // On crée une variable contenant le numéro de l'entrée pour d)
        }
    }

    if (compteurEntrees == 0)
    {
        cout << "a) Erreur : Il n'y a pas d'entree" << endl;
        valide = false;
    }
    else if (compteurEntrees > 1)
    {
        cout << "a) Erreur : Il y a " << compteurEntrees << " entrees" << endl;
        valide = false;
    }
    else
    {
        cout << "a) Il y a bien une unique entree" << endl;
    }


    // b) 1 seul point de sortie
    map<int, int> aretesSortantes = map<int, int>();

    for (int x = 0; x < graphe->nbSommets; x++) {
        aretesSortantes[x] = 0;
    }

    for (int i = 0 ; i < graphe->nbSommets ; i++)
    {
        for (int j = 0 ; j < graphe->nbSommets ; j++)
        {
            if (graphe->MAdj[j][i] == true)
            {
                aretesSortantes[j]++;
            }
        }
    }

    int compteurSorties = 0, sortie;

    for (int x = 0; x < graphe->nbSommets; x++) {
        if (aretesSortantes[x] == 0) // Si on n'a que des 0 sur une ligne, c'est une sortie
        {
            compteurSorties++;
            sortie = x; // On crée une variable contenant le numéro de la sortie pour d)
        }
    }

    if (compteurSorties == 0)
    {
        cout << "b) Erreur : Il n'y a pas de sortie" << endl;
        valide = false;
    }
    else if (compteurSorties > 1)
    {
        cout << "b) Erreur : Il y a " << compteurSorties << " sorties" << endl;
        valide = false;
    }
    else
    {
        cout << "b) Il y a bien une unique sortie" << endl;
    }

    /*cout << "Entree : " << entree << endl;
    cout << "Sortie : " << sortie << endl;*/

    // c) Pas de circuit
    bool circuit = aUnCircuit(graphe);

    if (circuit == true)
    {
        cout << "c) Erreur : Le graphe a un circuit" << endl;
        valide = false;
    }
    else
    {
        cout << "c) Il n'y a pas de circuit" << endl;
    }

    // d) Il existe un chemin du point d’entrée à tout autre sommet
    bool dValide = true;
    // Création du graphe temporaire
    t_graphe * t = new t_graphe;
    // t reçoit la matrice d'adjacence du graphe transitif
    transitive(graphe, t, false);
    // On vérifie qu'on a bien true sur la ligne d'entree (sauf lui-même) pour avoir d)
    for (int i = 0 ; i < t->nbSommets ; i++)
    {
        if (t->MAdj[entree][i] == false && i != entree)
        {
            cout << "d) Erreur : Il n'existe pas un chemin du point d'entree a tout autre sommet" << endl;
            valide = dValide = false;
            break;
        }
    }

    if (dValide)
    {
        cout << "d) Il y a bien un chemin du point d'entree a tout autre sommet" << endl;
    }

    // e) Il existe un chemin de n’importe quel sommet au point de sortie
    bool eValide = true;
    // On vérifie qu'on a bien true sur la colonne de sortie (sauf lui-même) pour avoir e)
    for (int i = 0 ; i < t->nbSommets ; i++)
    {
        if (t->MAdj[i][sortie] == false && i != sortie)
        {
            cout << "e) Erreur : Il n'existe pas un chemin de n'importe quel sommet au point de sortie" << endl;
            valide = eValide = false;
            break;
        }
    }

    if (eValide)
    {
        cout << "e) Il y a bien un chemin de n'importe quel sommet au point de sortie" << endl;
    }

    // Rien n'a été retourné jusque-là ? Le graphe est validé !
    return valide;
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
    transitive(G, t, true);

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
    affichageRang(rang(G));

    bool valide = validation(G);

    if (valide)
    {
        cout << "Le graphe a ete valide" << endl;
    }
    else
    {
        cout << "Le graphe n'a pas ete valide" << endl;
    }
/*
    map<int, int> dureeSommet;
    cout << "Generation a partir du fichier" << endl;
    dureeSommet = generateFromFileTask(G);

//    cout << "Matrice adjacente originale :" << endl;
//    afficheMatriceAdjacente(G);

    cout << "Matrice complete :" << endl;
    afficheCompletGraphe(G);

    calendrierAuPlusTot(G, dureeSommet);
    calendrierAuPlusTard(G, dureeSommet);*/

    return 1;
}
