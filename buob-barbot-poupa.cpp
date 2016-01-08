#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#define FICHIER_GRAPHE "buob-barbot-poupa-rang.txt"

using namespace std;

// Structure utilis�e pour nos graphes
typedef struct {
    int  nbSommets; // Nombre de sommets (indiqu� en premi�re ligne du .txt)
    bool **  MAdj; // MAdj[x][y] = TRUE <==> il existe arc (x,y)
    int ** MVal; // Si MAdj[x][y] = TRUE alors MVal[x][y] = valeur de l�arc (x,y)
} t_graphe;

// Header
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
map<int, int> calendrierAuPlusTot(t_graphe * graphe, map<int, int> dureeSommet);
int dateAuPlusTard(t_graphe * graphe, int sommet, map<int, int> dureeSommet);
map<int, int> calendrierAuPlusTard(t_graphe * graphe, map<int, int> dureeSommet);
bool validation(t_graphe * graphe);
void editeur(t_graphe * graphe);
void addDeleteTask(t_graphe * graphe);
void addDeleteConstraint(t_graphe * graphe);
void editDuration(t_graphe * graphe);
set<int> sortieGraphe(t_graphe * graphe);
void copieGrapheAvecSuppressionSommet(t_graphe * original, t_graphe * copie, int sommet);

// G�n�ration de la matrice vide
void generateMatriceVide(t_graphe * target, int nbSommets)
{
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

// Copie d'un graphe de l'original (argument 1 � l'argument 2, nouvelle variable)
void copieGraphe(t_graphe * original, t_graphe * copie)
{
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

// Copie du graphe en supprimant le sommet sp�cifi� en 3�me argument
void copieGrapheAvecSuppressionSommet(t_graphe * original, t_graphe * copie, int sommet)
{
    // R�duction du nombre de sommets
    copie->nbSommets = original->nbSommets - 1;

    copie->MAdj = new bool * [copie->nbSommets];
    copie->MVal = new int * [copie->nbSommets];

    int comptJ = 0, comptI = 0;

    for (int i = 0; i < original->nbSommets; i++) {
        // Tant qu'on n'est pas sur le sommet � supprimer, on copie
        if (i != sommet) {
            // Initialisation du compteur des colonnes, pour ne pas �tre
            // d�synchronis� puisqu'on supprime un sommet
            comptJ = 0;

            // R�servation des adresses des lignes
            copie->MAdj[comptI] = new bool [copie->nbSommets];
            copie->MVal[comptI] = new int [copie->nbSommets];

            for (int j = 0; j < original->nbSommets; j++) {
                if (j != sommet) {
                    // Remplissage des cases
                    copie->MAdj[comptI][comptJ] = original->MAdj[i][j];
                    copie->MVal[comptI][comptJ] = original->MVal[i][j];
                    comptJ++;
                }
            }
            comptI++;
        }
    }
}

// Insertion des donn�es dans notre structure de donn�es
// � partir du fichier de graphe
void generateFromFile(t_graphe * target) {
    // Lecture du graphe sur fichier
    ifstream fg (FICHIER_GRAPHE);

    int nbSommets;

    // Lecture du nombre de sommets et allocation dynamique des SdD
    fg >> nbSommets;

    generateMatriceVide(target, nbSommets);

    // Lecture des arcs
    int extInit, extTerm, valeur;

    fg >> extInit;
    // -1 signifie la fin du fichier
    while (extInit != -1) {
        fg >> extTerm;
        fg >> valeur;
        target->MAdj[extInit][extTerm] = true;
        target->MVal[extInit][extTerm] = valeur;
        fg >> extInit;
    }
}

// Insertion des donn�es dans notre structure de donn�es
// � partir du fichier de graphe ayant des contraintes
map<int, int> generateFromFileTask(t_graphe * target) {
    // Lecture du graphe sur fichier
    ifstream fg (FICHIER_GRAPHE);

    int nbSommets;

    // Lecture du nombre de sommets et allocation dynamique des SdD
    fg >> nbSommets;
    // On ajoute 2 sommets au nombre du fichier (un �tat de d�part et un �tat d'arriv�e)
    nbSommets += 2;
    cout << "Nombre sommets: " << nbSommets - 2 << " + 2 (0 et " << nbSommets - 1 <<")"  << endl;

    // On cr�e notre matrice vide dans laquelle on va copier notre nouveau graphe
    generateMatriceVide(target, nbSommets);

    // Lecture des arcs
    int extInit, contrainte, duree;

    // Initialisation des contraintes et des dur�es possibles
    set<int> contrainteList = set<int>();
    map<int, int> extInitDuree = map<int, int>();

    for (int i = 1; i < nbSommets-1; i++)
    {
        // Insertion des contraintes
        contrainteList.insert(i);
    }

    // /!\ ATTENTION : les contraintes et init d�marrent � 1 dans le fichier, 0 dans la matrice

    // lecture du nom du sommet
    fg >> extInit;
    // -1 signifie la fin du fichier
    if (extInit != -1)
    {
        cout << "sommet : " << extInit;
        while (extInit != -1)
        {
            fg >> duree;
            cout << ", duree: " << duree;
            extInitDuree[extInit] = duree;
            fg >> contrainte;
            int cCount = 0;
            cout <<  ", contrainte : ";
            // -1 signifie la fin de la ligne
            while (contrainte != -1)
            {
                // Chaque fois qu'on passe sur une nouvelle contrainte, on la
                // retire de la liste pour trouver les sommets dont les autres
                // ne d�pendent pas
                contrainteList.erase(contrainte);
                target->MAdj[contrainte][extInit] = true;
                cout << (cCount != 0 ? ", ": "") << contrainte;
                cCount++;
                fg >> contrainte;
            }
            if (contrainte == -1 && cCount == 0)
            {
                // Pas de contraintes = entr�es
                cout << "Pas de contraintes ...";
                target->MAdj[0][extInit] = true;
                target->MVal[0][extInit] = 0;
            }
            cout << endl;

            fg >> extInit;
            if (extInit != -1)
            {
                cout << "sommet : " << extInit;
            }

        }
    }
    // Pour les sommets qui ne sont pas dans les contraintes
    // Ils ne sont attendus par personne
    for (auto const elem: contrainteList) {
        target->MAdj[elem][nbSommets-1] = true;
        target->MVal[elem][nbSommets-1] = 0;
    }


    for (int i = 0; i < nbSommets; i++) {
        for (int j = 0; j < nbSommets; j++) {
            if (target->MAdj[i][j]) {
                target->MVal[i][j] = extInitDuree[i];
            }
        }
    }

    return extInitDuree;
}

// Affichage de la matrice adjacente
void afficheMatriceAdjacente(t_graphe * target) {
    /*
        Jusqu'� 10, on peur avoir un bel affichage condens� avec le nom des sommets
        Au dela, on n'a plus le nom des sommets
    */

    for (int x = -1; x < target->nbSommets; x++) {
        if (x == -1)
        {
            cout << "  ";
        }
        else
        {
            if (x < 10)
            {
                cout << x << " ";
            }
            else
            {
                cout << "+ ";
            }
        }
    }
    cout << endl;

    for (int x = 0; x < target->nbSommets; x++) {
        if (x < 10)
        {
            cout << x << " ";
        }
        else
        {
            cout << "+ ";
        }
        for (int y = 0; y < target->nbSommets; y ++) {
            if (target->MAdj[x][y] == true) {
                cout << "X";
                if (y != target->nbSommets-1) {
                    cout << ".";
                }
            }
            else if (y == target->nbSommets-1) {
                cout << ".";
            }
            else {
                cout << "..";
            }
        }
        cout << endl;
    }
}

// Affichage de la matrice d'incidence
void afficheMatriceIncidence(t_graphe * target) {
    /*
        Jusqu'� 10, on peur avoir un bel affichage condens� avec le nom des sommets
        Au dela, on n'a plus le nom des sommets
    */

    for (int x = -1; x < target->nbSommets; x++) {
        if (x == -1)
        {
            cout << "  ";
        }
        else
        {
            if (x < 10)
            {
                cout << x << " ";
            }
            else
            {
                cout << "+ ";
            }
        }
    }
    cout << endl;

    for (int x = 0; x < target->nbSommets; x++) {
        if (x < 10)
        {
            cout << x << " ";
        }
        else
        {
            cout << "+ ";
        }
        for (int y = 0; y < target->nbSommets; y ++) {
            if (target->MAdj[x][y] == true) {
                cout << target->MVal[x][y]; // Probl�me si valeur > 9 ou < 0
                if (y != target->nbSommets-1) {
                    cout << ".";
                }
            }
            else if (y == target->nbSommets-1) {
                cout << ".";
            }
            else {
                cout << "..";
            }
        }
        cout << endl;
    }
}

// L'affichage complet du graphe : matrices d'ajacence et d'incidence
void afficheCompletGraphe(t_graphe * target) {
    afficheMatriceAdjacente(target);
    afficheMatriceIncidence(target);
}

/* Remplissage de la matrice transitive (deuxi�me argument) � partir
   de la matrice originale (premier argument)
   le 3�me argument d�clenche l'affichage (ou pas) */
void transitive(t_graphe * original, t_graphe * target, bool display) {
    t_graphe * m = new t_graphe;

    // On g�n�re deux matrices vides
    generateMatriceVide(m, original->nbSommets);
    generateMatriceVide(target, original->nbSommets);

    /*
        Pour calculer la matrice transitive :
        On multiplie notre matrice d'adjacence par elle-m�me nbSommets - 1 fois
        D�s qu'on l�, on r�unit toutes les puissances, c'est � dire que si on a
        eu un TRUE une fois dans une case, la matrice transitive aura ce TRUE
        Cf http://www.info2.uqam.ca/~inf1130/documents/FermetureMatrices.pdf
    */
    for (int n = 2; n <= original->nbSommets - 1; n++) {
        for (int i = 0; i < m->nbSommets; i++) {
            for (int j = 0; j < m->nbSommets; j++) {
                for (int k = 0; k < m->nbSommets; k++) {
                    if (n == 2) {
                        m->MAdj[i][j] += (bool)((int)original->MAdj[i][k] * (int)original->MAdj[k][j]);
                    }
                    else{
                        m->MAdj[i][j] += (bool)((int)original->MAdj[i][k] * (int)m->MAdj[k][j]);
                    }
                }
                if (n == 2) {
                    target->MAdj[i][j] = m->MAdj[i][j] || original->MAdj[i][j];
                }
                else{
                    target->MAdj[i][j] = m->MAdj[i][j] || target->MAdj[i][j];
                }
            }
        }

        // Affichage de toutes les puissances calcul�es et de la matrice transitive d�termin�e
        if (display)
        {
            cout << "m^" << n <<" : " << endl;
            afficheMatriceAdjacente(m);

            cout << "transitive pour n = " << n << " : " << endl;
            afficheMatriceAdjacente(target);
        }
    }
}

// Recherche de circuit � partir de la matrice transitive
bool aUnCircuit(t_graphe * matriceTransitive) {
    for (int i = 0 ; i < matriceTransitive->nbSommets ; i++)
    {
        if (matriceTransitive->MAdj[i][i] == true)
        {
            // On a un 1 sur la diagonale: il y a un circuit et on sort
            return true;
        }
    }

    // Si on n'est pas sorti jusque l�, il y a un circuit
    return false;
}

// Calcul du demi degr� adjacent
map<int, int> demiDegreAdjacent(t_graphe * graphe) {

    // On utilise une map pour les aretes entrantes
    // map<num�ro arete, nombre arete entrante>
    map<int, int> aretesEntrantes = map<int, int>();

    for (int x = 0; x < graphe->nbSommets; x++) {
        // Initialisation de toutes les aretes entrantes � 0
        aretesEntrantes[x] = 0;
    }

    for (int i = 0 ; i < graphe->nbSommets ; i++)
    {
        for (int j = 0 ; j < graphe->nbSommets ; j++)
        {
            if (graphe->MAdj[i][j] == true)
            {
                // Incr�mentation de l'arete entrante num�ro j
                aretesEntrantes[j]++;
            }
        }
    }

    return aretesEntrantes;
}

// Retourne le premier entier d'une map
int findFirstWhereEntier(map<int, int> m, int v) {
    for (auto const elem: m) {
        if (elem.second == v)
        {
            return elem.first;
        }
    }

    // Retour de -1 si non trouv�
    return -1;
}

// Calcul du rang
map<int, int> rang(t_graphe * graphe)
{
    /*
        Calcul de rang
        On v�rifie s'il n'y a pas de circuit
        On traite ensuite les sommets 1 � 1 en les supprimant de la matrice
        On ins�re le rang actuel dans le map rangIte qui contient nos rangs finaux
        On doit �galement avoir une autre variable pour garder l'ancien nom pour l'affichage
    */
    bool circuit = aUnCircuit(graphe);
    t_graphe * workGraphe = new t_graphe();
    t_graphe * tempGraphe = new t_graphe();

    copieGraphe(graphe, workGraphe);

    // Tableau contenant le rang et l'arete associ�e
    map<int, int> rangSommets = map<int, int>();

    if (circuit == true)
    {
        // Si le graphe a un circuit, on sort
        cout << "Erreur : le graphe a un circuit" << endl;
    }
    else
    {
        int rangIte = 0;
        map<int, int> aretesEntrantes;
        int sommet = 0;

        // On garde l'ancien nom
        int sommetRealname[graphe->nbSommets];

        // Initialisation des anciens noms des sommets
        for (int n = 0; n < graphe->nbSommets; n++) {
            sommetRealname[n] = n;
        }

        // Tant qu'on a encore des sommets dans le map
        while (sommet >= 0) {
            // On r�cup�re la liste des sommets � traiter
            aretesEntrantes = demiDegreAdjacent(workGraphe);

            // S�lection du sommet a traiter (c'est � dire � retirer)
            sommet = findFirstWhereEntier(aretesEntrantes, 0);

            if (sommet != -1) {
                rangSommets[sommetRealname[sommet]] = rangIte;

                bool found = false;
                for (int n = 0; n < graphe->nbSommets; n++) {
                    if (n == sommet) {
                        found = true;
                    }
                    if (found) {
                        // Correction du nom d'affichage : sans cela, on ne pourrait pas
                        // dire quel sommet a �t� supprim� � chaque it�ration
                        sommetRealname[n] = (n == graphe->nbSommets - 1 ? 0 : sommetRealname[n+1]);
                    }
                }

                rangIte++;

                // Supprimer ligne et colonne du sommet dans la matrice
                copieGrapheAvecSuppressionSommet(workGraphe, tempGraphe, sommet);

                delete workGraphe;
                workGraphe = tempGraphe;
                tempGraphe = new t_graphe();
            }
        }
    }

    return rangSommets;
}

// It�rateur permettant l'affichage propre d'un rang
void affichageRang(map<int, int> rS) {
    for (auto const elem: rS) {
        cout << "sommet : "<< elem.first << " , rang : " << elem.second << endl;
    }
}

// Calcul de la date au plus t�t
// Arguments : graphe, sommet dont on recherche la date, map de la dur�e de tous les sommets
int dateAuPlusTot(t_graphe * graphe, int sommet, map<int, int> dureeSommet) {

    // Date au plus tot = max(date au plus tot predecesseurs + duree predecesseur)
    // Recherche des sommets predecesseurs

    // pred liste tous les pr�d�cesseurs du graphe
    set<int> pred = set<int>();
    for (int i = 0; i < graphe->nbSommets; i++) {
        if (graphe->MAdj[i][sommet])
        {
            pred.insert(i);
        }
    }

    // S�lectionner le max de datePred
    set<int> datePred = set<int>();
    for (auto const elem: pred) {
        datePred.insert(dureeSommet[elem] + dateAuPlusTot(graphe, elem, dureeSommet));
    }

    if (datePred.empty())
    {
        return 0;
    }
    else
    {
        return *datePred.rbegin(); // rbegin = dernier �l�ment
    }
}

// Affiche le calendrier au plus t�t, en se basant sur dateAuPlusTot
map<int, int> calendrierAuPlusTot(t_graphe * graphe, map<int, int> dureeSommet) {

    // R�cup�ration et affichage des rangs
    map<int, int> rangS = rang(graphe);
    affichageRang(rangS);

    // Initialisation du tableau des dates des sommets
    map<int, int> datesSommet = map<int, int>();

    // pour chaque sommet, on calcule sa date au plus tot
    for (auto const elem: rangS) {
        datesSommet[elem.first] = dateAuPlusTot(graphe, elem.first, dureeSommet);
    }

    for (auto const elem: datesSommet) {
        cout << "sommet: " << elem.first << ", date au plus tot : " << elem.second << endl;
    }

    return datesSommet;
}

// Calcul de la date au plus tard
// Arguments : graphe, sommet dont on recherche la date, map de la dur�e de tous les sommets
int dateAuPlusTard(t_graphe * graphe, int sommet, map<int, int> dureeSommet) {

    // Date au plus tard = min(dateAuPlusTard(successeurs)) - dureeSommet[sommet]
    // On cherche les successeurs

    set<int> succ = set<int>();
    for (int i = 0; i < graphe->nbSommets; i++) {
        if (graphe->MAdj[sommet][i]) succ.insert(i);
    }

    // selectionner le min de succ
    set<int> dateSucc = set<int>();
    for (auto const elem: succ) {
        dateSucc.insert(dateAuPlusTard(graphe, elem, dureeSommet) - dureeSommet[sommet]);
    }

    if (dateSucc.empty())
    {
        return dateAuPlusTot(graphe, sommet, dureeSommet);
    }
    else
    {
        return *dateSucc.begin();
    }
}

// Affiche le calendrier au plus tard, en se basant sur dateAuPlusTard
map<int, int> calendrierAuPlusTard(t_graphe * graphe, map<int, int> dureeSommet) {

    // R�cup�ration et affichage des rangs
    map<int, int> rangS = rang(graphe);
    affichageRang(rangS);

    // Initialisation du tableau des dates des sommets
    map<int, int> datesSommet = map<int, int>();

    // Pour chaque sommet, on calcule sa date au plus tard
    for (auto const elem: rangS) {
        datesSommet[elem.first] = dateAuPlusTard(graphe, elem.first, dureeSommet);
    }

    for (auto const elem: datesSommet) {
        cout << "sommet: " << elem.first << ", date au plus tard : " << elem.second << endl;
    }

    return datesSommet;
}

// R�cup�ration d'un set<int> de sorties
set<int> sortieGraphe(t_graphe * graphe)
{
    map<int, int> aretesSortantes = map<int, int>();
    set<int> sorties = set<int>();

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

    for (int x = 0; x < graphe->nbSommets; x++) {
        if (aretesSortantes[x] == 0) // Si on n'a que des 0 sur une ligne, c'est une sortie
        {
            sorties.insert(x);
        }
    }

    return sorties;
}

// Valide le graphe en suivant les 5 points du sujet
bool validation(t_graphe * graphe)
{
    cout << "Validation du graphe" << endl;

    bool valide = true;

    // a) 1 seul point d'entr�e
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
        if (aretesEntrantes[x] == 0) // Si on n'a que des 0 sur une colonne, c'est une entr�e
        {
            compteurEntrees++;
            entree = x; // On cr�e une variable contenant le num�ro de l'entr�e pour d)
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
            sortie = x; // On cr�e une variable contenant le num�ro de la sortie pour d)
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

    // d) Il existe un chemin du point d�entr�e � tout autre sommet
    bool dValide = true;
    // Cr�ation du graphe temporaire
    t_graphe * t = new t_graphe;
    // t re�oit la matrice d'adjacence du graphe transitif
    transitive(graphe, t, false);
    // On v�rifie qu'on a bien true sur la ligne d'entree (sauf lui-m�me) pour avoir d)
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

    // e) Il existe un chemin de n�importe quel sommet au point de sortie
    bool eValide = true;
    // On v�rifie qu'on a bien true sur la colonne de sortie (sauf lui-m�me) pour avoir e)
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

    // Rien n'a �t� retourn� jusque-l� ? Le graphe est valid� !
    return valide;
}

void editeur(t_graphe * graphe){
    // affichage menu d'options
    int choice = -1;
    do{
        do{
            cout << "Editeur: " << endl;
            cout << "1.Ajouter/Supprimer une tache" << endl;
            cout << "2.Ajouter/Supprimer une contrainte" << endl;
            cout << "3.Modifier une duree" << endl;
            cout << "0.Ne rien faire" << endl;
            cout << "Choix : " << endl;
            cin >> choice;
        }while(choice < 0 && choice > 3);
        
        if(choice == 0) return;
        
        switch(choice){
            case 1: // ajouter/supprimer t�che
                addDeleteTask(graphe);
                break;
            case 2: // ajouter/supprimer contrainte
                addDeleteConstraint(graphe);
                break;
            case 3: // modifier duree t�che.
                editDuration(graphe);
                break;
            default:
                return;
        }
    }while(choice != 0);
    
    return;
}

void addDeleteTask(t_graphe * graphe){
    return;
}
void addDeleteConstraint(t_graphe * graphe){
    return;
}
void editDuration(t_graphe * graphe){
    return;
}

int main ()
{
    // D�claration graphe
    t_graphe * G = new t_graphe;

    cout << "Generation a partir du fichier" << endl;
    generateFromFile(G);

    cout << "Matrice adjacente originale :" << endl;

    //afficheMatriceAdjacente(G);

    afficheCompletGraphe(G);

    t_graphe * t = new t_graphe;

    cout << "Transitivite :" << endl;
    transitive(G, t, true);

    bool valide = validation(G);

    if (valide)
    {
        cout << "Le graphe a ete valide" << endl;
    }
    else
    {
        cout << "Le graphe n'a pas ete valide" << endl;
    }

    cout << "Rang" << endl;
    affichageRang(rang(G));
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
