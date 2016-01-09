#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
#ifdef _WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif

using namespace std;

// Structure utilisée pour nos graphes
typedef struct {
    int  nbSommets; // Nombre de sommets (indiqué en première ligne du .txt)
    bool **  MAdj; // MAdj[x][y] = TRUE <==> il existe arc (x,y)
    int ** MVal; // Si MAdj[x][y] = TRUE alors MVal[x][y] = valeur de l’arc (x,y)
} t_graphe;

// Header
void addDeleteTask(t_graphe * graphe);
void addDeleteConstraint(t_graphe * graphe);
void afficheCompletGraphe(t_graphe * target);
void afficheMatriceAdjacente(t_graphe * target);
void afficheMatriceValeurs(t_graphe * target);
void affichageRang(map<int, int> rS);
bool aUnCircuit(t_graphe * matriceTransitive);

int calculRang(t_graphe * graphe, int sommet);
map<int, int> calendrierAuPlusTard(t_graphe * graphe);
map<int, int> calendrierAuPlusTot(t_graphe * graphe);
void copieGraphe(t_graphe * original, t_graphe * copie);
void copieGrapheAvecSuppressionSommet(t_graphe * original, t_graphe * copie, int sommet);
void copieGrapheAvecAjoutSommet(t_graphe * original, t_graphe * copie);

int dateAuPlusTard(t_graphe * graphe, int sommet);
int dateAuPlusTot(t_graphe * graphe, int sommet);
map<int, int> demiDegreAdjacent(t_graphe * graphe);

void editDuration(t_graphe * graphe);
void editeur(t_graphe * graphe);
set<int> entreeGraphe(t_graphe * graphe);

int findFirstWhereEntier(map<int, int> m, int v);

void generateMatriceVide(t_graphe * target, int nbSommets);
void generateFromFile(t_graphe * target, string filePath);
void generateFromFileTask(t_graphe * target, string filePath);

void loadFromFile(t_graphe * graphe);

void mainMenu();

map<int, int> rang(t_graphe * graphe);

set<int> sortieGraphe(t_graphe * graphe);

void transitive(t_graphe * original, t_graphe * target, bool display = false);

bool validation(t_graphe * graphe);

// MAIN
int main() {

    // Lancement du menu
    mainMenu();

    return 1;
}

// Ajout/Suppression de tâche
void addDeleteTask(t_graphe * graphe) {
    int choixUtilisateur;

    cout << "1. Ajout de tache" << endl;
    cout << "2. Suppression de tache" << endl;
    cout << "Choix :" << endl;
    cin >> choixUtilisateur;

    // Variable de travail locale
    t_graphe * nouveauGraphe = new t_graphe();

    // Ajout
    if (choixUtilisateur == 1)
    {
        int duree, contrainte;
        string choix;
        set<int> contrainteS = set<int>();

        cout << "Ajout de tache" << endl;
        cout << "Saisissez la duree de la tache :" << endl;
        cin >> duree;

        cout << "Saisie des contraintes" << endl;
        cout << "Tapez 'O' pour saisir des contraintes, 'N' sinon :" << endl;
        cin >> choix;

        // On a un sommet de plus
        int nombreNouveauxSommets = graphe->nbSommets + 1;

        // Création de la nouvelle matrice, copie de l'ancienne
        generateMatriceVide(nouveauGraphe, nombreNouveauxSommets);
        copieGrapheAvecAjoutSommet(graphe, nouveauGraphe);

        if (choix == "O")
        {
            do
            {
                cout << "Saisie de la contrainte, -1 pour arreter" << endl;
                cin >> contrainte;

                cout << "Contrainte saisie : " << contrainte << endl;

                // Ajout de la nouvelle contrainte dans la nouvelle colonne, si elle est valide
                if (contrainte < (nombreNouveauxSommets - 1) && contrainte != -1)
                {
                    contrainteS.insert(contrainte);
                    nouveauGraphe->MAdj[contrainte][nombreNouveauxSommets-1] = true;
                    nouveauGraphe->MVal[contrainte][nombreNouveauxSommets-1] = duree;
                }

            } while (contrainte != -1);
        }
        else
        {
            cout << "Pas de contrainte saisie" << endl;
        }

        cout << "Recapitulatif: " << endl;
        cout << " - sommet: " << nombreNouveauxSommets - 1 << endl;
        cout << " - duree: " << duree << endl;
        cout << " - contraintes: ";
        // Récapitulatif des contraintes ajoutées plus haut
        for (set<int>::iterator i = contrainteS.begin(); i != contrainteS.end(); i++) {
            cout << *(i) << (*i == *contrainteS.rbegin() ? "" : ", ");
        }
        cout << endl << endl;
    }
    else
    {
        int choix;

        cout << "Suppression de tache" << endl;
        cout << "Saisissez la tache a supprimer : " << endl;
        cin >> choix;

        cout << "Tache a supprimer : " << choix << endl;

        // On supprime un sommet
        int nombreNouveauxSommets = graphe->nbSommets - 1;

        // Création de la nouvelle matrice, copie de l'ancienne
        generateMatriceVide(nouveauGraphe, nombreNouveauxSommets);
        copieGrapheAvecSuppressionSommet(graphe, nouveauGraphe, choix);
    }

    // Copie du nouveau graphe dans la copie temporaire de l'éditeur
    copieGraphe(nouveauGraphe, graphe);
}

// Ajout/Suppression de contrainte
void addDeleteConstraint(t_graphe * graphe) {
    int choixUtilisateur, contrainte, sommet;

    // Variable de travail locale
    t_graphe * workGraphe = new t_graphe();
    copieGraphe(graphe, workGraphe);

    cout << "Pour les sommets a contrainte unique, pensez a ajouter une nouvelle contrainte avant de la supprimer" << endl;
    cout << "1. Ajout de contrainte" << endl;
    cout << "2. Suppression de contrainte" << endl;
    cout << "Choix :" << endl;
    cin >> choixUtilisateur;

    if (choixUtilisateur == 1)
    {
        cout << "Ajout de contrainte" << endl;
        cout << "Saisissez la contrainte a ajouter :" << endl;
        cin >> contrainte;

        cout << "Saisissez le sommet pour cette nouvelle contrainte :" << endl;
        cin >> sommet;

        // On récupère la durée sur la colonne
        for (int i = 0; i < workGraphe->nbSommets; i++)
        {
            // Si on est sur une valeur
            if (workGraphe->MAdj[contrainte][i] == true)
            {
                // Copie de la valeur
                workGraphe->MVal[contrainte][sommet] = workGraphe->MVal[contrainte][i];
                break;
            }
        }

        // Mise à jour de la matrice d'adjacence
        workGraphe->MAdj[contrainte][sommet] = true;
    }
    else
    {
        cout << "Suppression de contrainte" << endl;
        cout << "Saisissez la contrainte a supprimer :" << endl;
        cin >> contrainte;

        cout << "Saisissez le sommet pour cette suppression :" << endl;
        cin >> sommet;

        // La suppression est triviale
        workGraphe->MAdj[contrainte][sommet] = false;
        workGraphe->MVal[contrainte][sommet] = 0;
    }

    // Copie du nouveau graphe dans la copie temporaire de l'éditeur
    copieGraphe(workGraphe, graphe);
}

// L'affichage complet du graphe : matrices d'adjacence et de valeurs
void afficheCompletGraphe(t_graphe * target) {
    afficheMatriceAdjacente(target);
    cout << endl;
    afficheMatriceValeurs(target);
}

// Affichage de la matrice adjacente
void afficheMatriceAdjacente(t_graphe * target) {

    int fieldSize = 4;
    for (int x = -1; x < target->nbSommets; x++) {
        if (x == -1)
        {
            cout << setfill(' ') << setw(fieldSize) << "";
        }
        else
        {
            cout << setfill(' ') << setw(fieldSize) << x;
        }
    }
    cout << endl;

    for (int x = 0; x < target->nbSommets; x++) {

        cout << setfill(' ') << setw(fieldSize) << x;

        for (int y = 0; y < target->nbSommets; y ++) {
            if (target->MAdj[x][y] == true) {
                cout << setfill(' ') << setw(fieldSize) << "X";
            }
            else{
                cout << setfill(' ') << setw(fieldSize) << ".";
            }
        }
        cout << endl;
    }
}

// Affichage de la matrice de valeurs, support de 3 chiffres
void afficheMatriceValeurs(t_graphe * target) {

    int fieldSize = 4;
    for (int x = -1; x < target->nbSommets; x++) {
        if (x == -1)
        {
            cout << setfill(' ') << setw(fieldSize) << "";
        }
        else
        {
            cout << setfill(' ') << setw(fieldSize) << x;
        }
    }
    cout << endl;

    for (int x = 0; x < target->nbSommets; x++) {

        cout << setfill(' ') << setw(fieldSize) << x;

        for (int y = 0; y < target->nbSommets; y ++) {
            if (target->MAdj[x][y] == true) {
                cout << setfill(' ') << setw(fieldSize) << target->MVal[x][y];;
            }
            else {
                cout << setfill(' ') << setw(fieldSize) << ".";
            }
        }
        cout << endl;
    }
}

// Itérateur permettant l'affichage propre d'un rang
void affichageRang(map<int, int> rS) {
    if (rS.size() > 0) {
        cout << "Sommet |";
        for (auto const elem: rS) {
            cout << setfill(' ') << setw(3) << elem.first;
        }
        cout << endl;
        cout << "Rang   |";
        for (auto const elem: rS) {
            cout << setfill(' ') << setw(3) << elem.second;
        }
        cout << endl;
    }
}

// Recherche de circuit à partir de la matrice transitive
bool aUnCircuit(t_graphe * matriceTransitive) {
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

// Calcul de rang
int calculRang(t_graphe * graphe, int sommet) {

    set<int> pred;

    for (int i = 0; i < graphe->nbSommets; i++) {
        if (graphe->MAdj[i][sommet]) { // trouve predecesseur de sommet
            pred.insert(calculRang(graphe, i));
        }
    }

    if (pred.size() == 0) {
        return 0;
    }
    else {
        return *pred.rbegin() + 1;
    }
}

// Affiche le calendrier au plus tard, en se basant sur dateAuPlusTard
map<int, int> calendrierAuPlusTard(t_graphe * graphe) {

    // Récupération et affichage des rangs
    map<int, int> rangS = rang(graphe);
    //affichageRang(rangS);

    // Initialisation du tableau des dates des sommets
    map<int, int> datesSommet = map<int, int>();

    // Pour chaque sommet, on calcule sa date au plus tard
    for (auto const elem: rangS) {
        datesSommet[elem.first] = dateAuPlusTard(graphe, elem.first);
    }

    cout << "Calendrier au plus tard: " << endl;
    for (auto const elem: datesSommet) {
        cout << "sommet: " << elem.first << ", date au plus tard : " << elem.second << endl;
    }
    cout << endl;

    return datesSommet;
}

// Affiche le calendrier au plus tôt, en se basant sur dateAuPlusTot
map<int, int> calendrierAuPlusTot(t_graphe * graphe) {

    // Récupération et affichage des rangs
    map<int, int> rangS = rang(graphe);

    // Initialisation du tableau des dates des sommets
    map<int, int> datesSommet = map<int, int>();

    // pour chaque sommet, on calcule sa date au plus tot
    for (auto const elem: rangS) {
        datesSommet[elem.first] = dateAuPlusTot(graphe, elem.first);
    }

    cout << "Calendrier au plus tot: " << endl;
    for (auto const elem: datesSommet) {
        cout << "sommet: " << elem.first << ", date au plus tot : " << elem.second << endl;
    }
    cout << endl;

    return datesSommet;
}

// Copie d'un graphe de l'original (argument 1 à l'argument 2, nouvelle variable)
void copieGraphe(t_graphe * original, t_graphe * copie) {
    copie->nbSommets = original->nbSommets;

    // On crée notre nouvelle matrice vide
    generateMatriceVide(copie, copie->nbSommets);

    copie->MAdj = new bool * [copie->nbSommets];
    copie->MVal = new int * [copie->nbSommets];

    for (int i = 0; i < copie->nbSommets; i++) {
        copie->MAdj[i] = new bool [copie->nbSommets];
        copie->MVal[i] = new int [copie->nbSommets];

        for (int j = 0; j < copie->nbSommets; j++) {
            // Copie des valeurs dans les matrices d'adjacence et de valeurs
            copie->MAdj[i][j] = original->MAdj[i][j];
            copie->MVal[i][j] = original->MVal[i][j];
        }
    }
}

// Copie du graphe en supprimant le sommet spécifié en 3ème argument
void copieGrapheAvecSuppressionSommet(t_graphe * original, t_graphe * copie, int sommet) {
    // Réduction du nombre de sommets
    copie->nbSommets = original->nbSommets - 1;

    copie->MAdj = new bool * [copie->nbSommets];
    copie->MVal = new int * [copie->nbSommets];

    int comptJ = 0, comptI = 0;

    for (int i = 0; i < original->nbSommets; i++) {
        // Tant qu'on n'est pas sur le sommet à supprimer, on copie
        if (i != sommet) {
            // Initialisation du compteur des colonnes, pour ne pas ętre
            // désynchronisé puisqu'on supprime un sommet
            comptJ = 0;

            // Réservation des adresses des lignes
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

// Copie du graphe en ajoutant un sommet
void copieGrapheAvecAjoutSommet(t_graphe * original, t_graphe * copie) {
    // Incrémentation du nombre de sommets
    copie->nbSommets = original->nbSommets + 1;

    copie->MAdj = new bool * [copie->nbSommets];
    copie->MVal = new int * [copie->nbSommets];

    int comptJ = 0, comptI = 0;

    for (int i = 0; i < copie->nbSommets; i++) {
        // Initialisation du compteur des colonnes, pour ne pas ętre
        // désynchronisé puisqu'on supprime un sommet
        comptJ = 0;

        // Réservation des adresses des lignes
        copie->MAdj[comptI] = new bool [copie->nbSommets];
        copie->MVal[comptI] = new int [copie->nbSommets];

        for (int j = 0; j < copie->nbSommets; j++) {
            if (i < original->nbSommets && j < original->nbSommets)
            {
                // Remplissage des cases
                copie->MAdj[comptI][comptJ] = original->MAdj[i][j];
                copie->MVal[comptI][comptJ] = original->MVal[i][j];
            }
            else
            {
                copie->MAdj[comptI][comptJ] = false;
                copie->MVal[comptI][comptJ] = 0;
            }
            comptJ++;
        }
        comptI++;
    }
}

// Calcul de la date au plus tard
// Arguments : graphe, sommet dont on recherche la date, map de la durée de tous les sommets
int dateAuPlusTard(t_graphe * graphe, int sommet) {

    // Date au plus tard = min(dateAuPlusTard(successeurs)) - dureeSommet[sommet]
    // On cherche les successeurs

    set<int> succ = set<int>();
    for (int i = 0; i < graphe->nbSommets; i++) {
        if (graphe->MAdj[sommet][i]) succ.insert(i);
    }

    // Sélectionner le min de succ
    set<int> dateSucc = set<int>();
    for (auto const elem: succ) {
        dateSucc.insert(dateAuPlusTard(graphe, elem) - graphe->MVal[sommet][elem]);
    }

    if (dateSucc.empty())
    {
        return dateAuPlusTot(graphe, sommet);
    }
    else
    {
        return *dateSucc.begin();
    }
}

// Calcul de la date au plus tôt
// Arguments : graphe, sommet dont on recherche la date, map de la durée de tous les sommets
int dateAuPlusTot(t_graphe * graphe, int sommet) {

    // Date au plus tot = max(date au plus tot predecesseurs + duree predecesseur)
    // Recherche des sommets predecesseurs

    // pred liste tous les prédécesseurs du graphe
    set<int> pred = set<int>();
    for (int i = 0; i < graphe->nbSommets; i++) {
        if (graphe->MAdj[i][sommet])
        {
            pred.insert(i);
        }
    }

    // Sélectionner le max de datePred
    set<int> datePred = set<int>();
    for (auto const elem: pred) {
        datePred.insert(graphe->MVal[elem][sommet] + dateAuPlusTot(graphe, elem));
    }

    if (datePred.empty())
    {
        return 0;
    }
    else
    {
        return *datePred.rbegin(); // rbegin = dernier élément
    }
}

// Calcul du demi-degré adjacent
map<int, int> demiDegreAdjacent(t_graphe * graphe) {

    // On utilise une map pour les aretes entrantes
    // map<numéro arete, nombre arete entrante>
    map<int, int> aretesEntrantes = map<int, int>();

    for (int x = 0; x < graphe->nbSommets; x++) {
        // Initialisation de toutes les aretes entrantes à 0
        aretesEntrantes[x] = 0;
    }

    for (int i = 0 ; i < graphe->nbSommets ; i++)
    {
        for (int j = 0 ; j < graphe->nbSommets ; j++)
        {
            if (graphe->MAdj[i][j] == true)
            {
                // Incrémentation de l'arete entrante numéro j
                aretesEntrantes[j]++;
            }
        }
    }

    return aretesEntrantes;
}

// Récuperer la duree d'execution pour chaque sommet
void editDuration(t_graphe * graphe) {

    t_graphe * tmpGraphe = new t_graphe();
    copieGraphe(graphe, tmpGraphe);

    map<int, int> dureeSommet = map<int, int>();
    bool found;
    for (int i = 0; i < graphe->nbSommets; i++) {
        found  = false;
        for (int j = 0; j < graphe->nbSommets; j++) {
            if (graphe->MAdj[i][j]) {
                dureeSommet[i] = graphe->MVal[i][j];
                // Sortie de boucle quand on a trouve la durée, identique sur toute la ligne
                found = true;
                break;
            }
        }
        if (!found) dureeSommet[i] = 0;
    }

    cout << "Rappel des durees: " << endl;
    for (auto const elem: dureeSommet) {
        cout << "- sommet " << elem.first << " dure " << elem.second << endl;
    }

    int choiceSommet;
    bool saisieFail;
    do{
        saisieFail = false;
        cout << "Choix du sommet:" << endl;
        cin >> choiceSommet;
        if (cin.fail()) {
            saisieFail = true;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (saisieFail || dureeSommet.find(choiceSommet) == dureeSommet.end());

    int nouvelleDuree;
    do{
        saisieFail = false;
        cout << "Nouvelle duree: " << endl;
        cin >> nouvelleDuree;
        if (cin.fail()) {
            saisieFail = true;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (saisieFail || nouvelleDuree < 0);

    for (int i = 0; i < tmpGraphe->nbSommets; i++) {
        if (tmpGraphe->MAdj[choiceSommet][i])
            tmpGraphe->MVal[choiceSommet][i] = nouvelleDuree;
    }

}

/*
   Rassemble les fonctions d'édition
   Permet de travailler sur une copie propre du graphe valide,
   qu'on recopie dans le graphe de départ si les modifications sont validées
*/
void editeur(t_graphe * graphe) {
    // affichage menu d'options
    int choice = -1;
    bool hasModif = false;
    t_graphe * nouveauGraphe = new t_graphe();
    copieGraphe(graphe, nouveauGraphe);

    do{
        do{
            cout << "Editeur: " << endl;
            cout << "1. Ajouter/Supprimer une tache" << endl;
            cout << "2. Ajouter/Supprimer une contrainte" << endl;
            cout << "3. Modifier une duree" << endl;
            if (hasModif) {
                cout << "4. Quitter et sauvegarder les modifications" << endl;
            }
            cout << "0. Quitter" << (hasModif ? " et annuler les modifications": "") << endl;
            cout << "Choix : " << endl;
            cin >> choice;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        } while (choice < 0 && (choice > 3 || (choice > 4 && hasModif)));

        switch(choice) {
            case 1: // ajouter/supprimer tâche
                addDeleteTask(nouveauGraphe);
                hasModif = true;
                break;
            case 2: // ajouter/supprimer contrainte
                addDeleteConstraint(nouveauGraphe);
                hasModif = true;
                break;
            case 3: // modifier duree tâche.
                editDuration(nouveauGraphe);
                hasModif = true;
                break;
            default:
                break;
        }

    } while (choice != 0 && (hasModif && choice != 4));


    if (choice == 4) {
        if (validation(nouveauGraphe))
        {
            cout << "f) ";
            set<int> sortieS = sortieGraphe(nouveauGraphe);
            if (sortieS.size() == 1) {
                int datePlusTard = dateAuPlusTard(graphe, *sortieS.begin());
                int nouvelleDateAuPlusTot = dateAuPlusTot(nouveauGraphe, *sortieS.begin());

                cout << "faisabilite (date au plus tard: " << datePlusTard << ", nouvelle date au plus tot: " << nouvelleDateAuPlusTot << ") : ";
                if (nouvelleDateAuPlusTot <= datePlusTard) {
                    cout << "OK !" << endl;
                    cout << "Le graphe est valide : votre modification est enregistree" << endl << endl;
                    copieGraphe(nouveauGraphe, graphe);
                    afficheCompletGraphe(graphe);
                }
                else
                    cout << "Erreur !" << endl;
            }
            return;
        }
        else
        {
            cout << "Votre modification entraine une corruption du graphe : elle n'est pas enregistree" << endl;
        }
    }

    return;
}

// Récupération d'un set<int> d'entree du graphe
set<int> entreeGraphe(t_graphe * graphe) {
    map<int, int> aretesEntrantes = map<int, int>();
    set<int> entrees = set<int>();

    for (int x = 0; x < graphe->nbSommets; x++) {
        aretesEntrantes[x] = 0;
    }

    for (int i = 0 ; i < graphe->nbSommets ; i++)
    {
        for (int j = 0 ; j < graphe->nbSommets ; j++)
        {
            if (graphe->MAdj[j][i] == true)
            {
                aretesEntrantes[i]++;
            }
        }
    }

    for (int x = 0; x < graphe->nbSommets; x++) {
        if (aretesEntrantes[x] == 0) // Si on n'a que des 0 sur une colonne, c'est une entreee
        {
            entrees.insert(x);
        }
    }

    return entrees;
}

// Retourne le premier entier d'une map
int findFirstWhereEntier(map<int, int> m, int v) {
    for (auto const elem: m) {
        if (elem.second == v)
        {
            return elem.first;
        }
    }

    // Retour de -1 si non trouvé
    return -1;
}

// Insertion des données dans notre structure de données
// a partir du fichier de graphe
void generateFromFile(t_graphe * target, string filePath) {
    // Lecture du graphe sur fichier
    ifstream fg (filePath);

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

    fg.close();
}

// Insertion des donnees dans notre structure de données
// a partir du fichier de graphe ayant des contraintes
void generateFromFileTask(t_graphe * target, string filePath) {
    // Lecture du graphe sur fichier
    ifstream fg (filePath);

    int nbSommets;

    // Lecture du nombre de sommets et allocation dynamique des SdD
    fg >> nbSommets;
    // On ajoute 2 sommets au nombre du fichier (un état de départ et un état d'arrivée)
    nbSommets += 2;
    cout << "Nombre sommets: " << nbSommets - 2 << " + 2 (0 et " << nbSommets - 1 <<")"  << endl;

    // On crée notre matrice vide dans laquelle on va copier notre nouveau graphe
    generateMatriceVide(target, nbSommets);

    // Lecture des arcs
    int extInit, contrainte, duree;

    // Initialisation des contraintes et des durées possibles
    set<int> contrainteList = set<int>();
    map<int, int> extInitDuree = map<int, int>();

    for (int i = 1; i < nbSommets-1; i++)
    {
        // Insertion des contraintes
        contrainteList.insert(i);
    }

    // /!\ ATTENTION : les contraintes et init démarrent à 1 dans le fichier, 0 dans la matrice

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
                // ne dépendent pas
                contrainteList.erase(contrainte);
                target->MAdj[contrainte][extInit] = true;
                cout << (cCount != 0 ? ", ": "") << contrainte;
                cCount++;
                fg >> contrainte;
            }
            if (contrainte == -1 && cCount == 0)
            {
                // Pas de contraintes = entrées
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

    cout << "Rajout d'une entree et d'une sortie ..." << endl;

    fg.close();
}

// Génération de la matrice vide (premier argument) à partir du nombre de sommets (second)
void generateMatriceVide(t_graphe * target, int nbSommets) {
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

// Chargement de fichier depuis le menu
void loadFromFile(t_graphe * graphe) {
    /**
     list of available files:
     - buob-barbot-poupa-test1.txt
     - buob-barbot-poupa-test2.txt
     - buob-barbot-poupa-test3.txt
     - buob-barbot-poupa-test4.txt
     - buob-barbot-poupa-test5.txt
     - buob-barbot-poupa-test6.txt
     - buob-barbot-poupa-test7.txt
     */

    int fileChoice = 0;
    vector<string> files = {
        "./buob-barbot-poupa-test1.txt",
        "./buob-barbot-poupa-test2.txt",
        "./buob-barbot-poupa-test3.txt",
        "./buob-barbot-poupa-test4.txt",
        "./buob-barbot-poupa-test5.txt",
        "./buob-barbot-poupa-test6.txt",
        "./buob-barbot-poupa-test7.txt",
    };

    do {
        cout << "Charger fichier : " << endl;
        cout << "1. Annexe test 1" << endl;
        cout << "2. Annexe test 2" << endl;
        cout << "3. Annexe test 3" << endl;
        cout << "4. Annexe test 4" << endl;
        cout << "5. Annexe test 5" << endl;
        cout << "6. Annexe test 6" << endl;
        cout << "7. Annexe test 7" << endl;
        cout << "8. Saisir chemin du fichier" << endl;
        cout << "Choix : " << endl;
        cin >> fileChoice;
    } while (fileChoice < 1 || fileChoice > 8);

    string filePath;
    if (fileChoice <= 7) {
        filePath = string(".") + string(SEPARATOR) + files[fileChoice-1];
    }
    switch(fileChoice) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            generateFromFile(graphe, filePath);
            break;
        case 6:
        case 7:
            generateFromFileTask(graphe, filePath);
            break;
        default:
            ifstream test;
            do{
                cout << "chemin de votre matrice: " << endl;
                cin >> filePath;
                test.open(filePath);
                if (!test) {
                    cout << "chemin incorrect ..." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    filePath = "";
                }
            } while (!test);

            test.close();

            // On n'utilise pas les mêmes fonctions de lecture en fonction du type de fichier
            char isConstraint;
            do{
                cout << "S'agit-il d'un tableau de contraintes ? [O/n]" << endl;
                cin >> isConstraint;
            } while (isConstraint != 'n' && isConstraint != 'O');

            if (isConstraint == 'n') {
                generateFromFile(graphe, filePath);
            }
            else{
                generateFromFileTask(graphe, filePath);
            }
    }
    return;
}

void mainMenu() {
    cout << "-----------------------" << endl;
    cout << "| Theorie des graphes |" << endl;
    cout << "|     mini-projet     |" << endl;
    cout << "|                     |" << endl;
    cout << "| Realise par:        |" << endl;
    cout << "|   - Barbot Timothee |" << endl;
    cout << "|   - Buob Edgar      |" << endl;
    cout << "|   - Poupa Adrien    |" << endl;
    cout << "-----------------------" << endl;

    int choice = 0;
    bool fileLoaded = false;
    t_graphe * graphe = new t_graphe();
    t_graphe * tempTransitive = new t_graphe();
    do{
        do{
            cout << "Menu:" << endl;
            cout << "1. Charger fichier" << endl;
            if (fileLoaded) {
                cout << "2. Afficher graphe" << endl;
                cout << "3. Fermeture transitive" << endl;
                cout << "4. Detection de circuit" << endl;
                cout << "5. Calcul de rang" << endl;
                cout << "6. Calcul calendrier au plus tot et au plus tard" << endl;
                cout << "7. Validation du graphe" << endl;
                cout << "8. Editer graphe" << endl;
            }
            cout << "0. Quitter le programme" << endl;
            cout << "Choix: " << endl;
            cin >> choice;
        } while (choice < 0 || (fileLoaded && choice > 8) || (!fileLoaded && choice > 1));

        switch(choice) {
            case 0:
                cout << "Quitter !" << endl;
                break;
            case 1:
                loadFromFile(graphe);
                fileLoaded = true;
                break;
            case 2:
                afficheCompletGraphe(graphe);
                break;
            case 3:
                transitive(graphe, tempTransitive, true);
                break;
            case 4:{
                transitive(graphe, tempTransitive, true);
                bool res = aUnCircuit(tempTransitive);
                if (res) {
                    cout << "Le graphe a un circuit" << endl;
                }
                else{
                    cout << "Le graphe n'a pas de circuit" << endl;
                }
                break;
            }
            case 5:{
                map<int, int> rangs = rang(graphe);
                if (rangs.size() > 0) {
                    cout << "Recapitulatif: " << endl;
                    affichageRang(rangs);
                }
                break;
            }
            case 6:
                calendrierAuPlusTot(graphe);
                calendrierAuPlusTard(graphe);
                break;
            case 7:
                if (validation(graphe)) {
                    cout << " -> Graphe valide !" << endl;
                }
                else{
                    cout << " -> Graphe non valide !" << endl;
                }
                break;
            case 8:
                editeur(graphe);
                break;
            default:
                cout << "Menu inconnu ... Retour au menu !" << endl;
        }
        cout << endl << endl;
    } while (choice != 0);
    return;
}

// Calcul du rang
map<int, int> rang(t_graphe * graphe) {
    /*
     Calcul de rang
     On vérifie s'il n'y a pas de circuit
     On traite ensuite les sommets 1 à 1 en les supprimant de la matrice
     On insère le rang actuel dans le map rangIte qui contient nos rangs finaux
     On doit également avoir une autre variable pour garder l'ancien nom pour l'affichage
     */

    t_graphe * tmpGraphe = new t_graphe();
    transitive(graphe, tmpGraphe, false);

    // Tableau contenant le rang et l'arete associée
    map<int, int> rangSommets = map<int, int>();

    if (aUnCircuit(tmpGraphe))
    {
        // Si le graphe a un circuit, on sort
        cout << "Erreur : le graphe a un circuit" << endl;
    }
    else
    {
        for (int i = 0; i < graphe->nbSommets; i++) {
            cout << "Sommet : " << i;
            rangSommets[i] = calculRang(graphe, i);
            cout << ", Rang: " << rangSommets[i] << endl;
        }
    }

    return rangSommets;
}

// Récupération d'un set<int> de sorties
set<int> sortieGraphe(t_graphe * graphe) {
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

/* Remplissage de la matrice transitive (deuxieme argument) a partir
   de la matrice originale (premier argument)
   le 3eme argument déclenche l'affichage (ou pas) */
void transitive(t_graphe * original, t_graphe * target, bool display) {
    t_graphe * m = new t_graphe;

    // On génère deux matrices vides
    generateMatriceVide(m, original->nbSommets);
    generateMatriceVide(target, original->nbSommets);

    /*
        Pour calculer la matrice transitive :
        On multiplie notre matrice d'adjacence par elle-męme nbSommets - 1 fois
        Dès qu'on là, on réunit toutes les puissances, c'est à dire que si on a
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

        // Affichage de toutes les puissances calculées et de la matrice transitive déterminée
        if (display)
        {
            cout << "m^" << n <<" : " << endl;
            afficheMatriceAdjacente(m);
            cout << "transitive pour n = " << n << " : " << endl;
            afficheMatriceAdjacente(target);
            cout << endl;
        }
    }
}

// Valide le graphe en suivant les 5 points du sujet
bool validation(t_graphe * graphe) {
    cout << "Validation du graphe" << endl;

    bool valide = true;

    // a) 1 seul point d'entrée
    set<int> entreeS = entreeGraphe(graphe);

    if (entreeS.size() == 0)
    {
        cout << "a) Erreur : Il n'y a pas d'entree" << endl;
        valide = false;
    }
    else if (entreeS.size() > 1)
    {
        cout << "a) Erreur : Il y a " << entreeS.size() << " entrees" << endl;
        valide = false;
    }
    else
    {
        cout << "a) OK : Il y a bien une unique entree" << endl;
    }


    // b) 1 seul point de sortie
    set<int> sortieS = sortieGraphe(graphe);

    if (sortieS.size() == 0)
    {
        cout << "b) Erreur : Il n'y a pas de sortie" << endl;
        valide = false;
    }
    else if (sortieS.size() > 1)
    {
        cout << "b) Erreur : Il y a " << sortieS.size() << " sorties" << endl;
        valide = false;
    }
    else
    {
        cout << "b) OK : Il y a bien une unique sortie" << endl;
    }

    // c) Pas de circuit
    bool circuit = aUnCircuit(graphe);

    if (circuit == true)
    {
        cout << "c) Erreur : Le graphe a un circuit" << endl;
        valide = false;
    }
    else
    {
        cout << "c) OK : Il n'y a pas de circuit" << endl;
    }

    // d) Il existe un chemin du point d’entrée à tout autre sommet
    // Création du graphe temporaire
    t_graphe * t = new t_graphe;
    // t reçoit la matrice d'adjacence du graphe transitif
    transitive(graphe, t, false);

    if (entreeS.size() == 1) {

        bool dValide = true;
        int entree = *entreeS.begin();

        // On vérifie qu'on a bien true sur la ligne d'entree (sauf lui-męme) pour avoir d)
        for (int i = 0 ; i < t->nbSommets ; i++)
        {
            if (t->MAdj[entree][i] == false && i != entree)
            {
                cout << "d) Erreur : Il n'existe pas un chemin du point d'entree a tout autre sommet" << endl;
                valide = false;
                dValide = false;
                break;
            }
        }

        if (dValide)
        {
            cout << "d) OK : Il y a bien un chemin du point d'entree a tout autre sommet" << endl;
        }
    }

    // e) Il existe un chemin de n’importe quel sommet au point de sortie
    if (sortieS.size() == 1) {

        bool eValide = true;
        int sortie = *sortieS.begin();

        // On vérifie qu'on a bien true sur la colonne de sortie (sauf lui-męme) pour avoir e)
        for (int i = 0 ; i < t->nbSommets ; i++)
        {
            if (t->MAdj[i][sortie] == false && i != sortie)
            {
                cout << "e) Erreur : Il n'existe pas un chemin de n'importe quel sommet au point de sortie" << endl;
                valide = false;
                eValide = false;
                break;
            }
        }

        if (eValide)
        {
            cout << "e) OK : Il y a bien un chemin de n'importe quel sommet au point de sortie" << endl;
        }
    }

    // Rien n'a été retourné jusque-là ? Le graphe est validé !
    return valide;
}
