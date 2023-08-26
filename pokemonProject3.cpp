#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include "BTree.h"
using namespace std;

//used for testing time
#include <cstdlib>
#include <ctime>
#include <chrono>

using chrono::high_resolution_clock;
using chrono::duration_cast;
using chrono::duration;
chrono::microseconds; //setting timescale to microseconds



void readData(vector<int>& pokedex_num, vector<string>& names, vector<string>& type1, vector<string>& type2,
    vector<int>& total, vector<int>& hp, vector<int>& attack, vector<int>& defense, vector<int>& sp_atk,
    vector<int>& sp_def, vector<int>& speed, vector<int>& generation, vector<bool>& legendary) {
    ifstream infile("Pokemon.csv"); // open the CSV file

    if (infile.good()) {
        cout << "File opened successfully \n" << endl;
    }
    else {
        cout << "Error opening file \n" << endl;
    }

    string line;
    getline(infile, line); // read the header line and discard it

    while (getline(infile, line)) { // read the data lines
        istringstream iss(line);
        string token;

        // read in each element of the line and store it in the appropriate vector
        getline(iss, token, ',');
        pokedex_num.push_back(stoi(token));
        getline(iss, token, ',');
        names.push_back(token);
        getline(iss, token, ',');
        type1.push_back(token);
        getline(iss, token, ',');
        type2.push_back(token);
        getline(iss, token, ',');
        total.push_back(stoi(token));
        getline(iss, token, ',');
        hp.push_back(stoi(token));
        getline(iss, token, ',');
        attack.push_back(stoi(token));
        getline(iss, token, ',');
        defense.push_back(stoi(token));
        getline(iss, token, ',');
        sp_atk.push_back(stoi(token));
        getline(iss, token, ',');
        sp_def.push_back(stoi(token));
        getline(iss, token, ',');
        speed.push_back(stoi(token));
        getline(iss, token, ',');
        generation.push_back(stoi(token));
        getline(iss, token, ',');
        legendary.push_back(token == "TRUE");
    }

    infile.close();
}

Pokemon* modifyStats(Pokemon* p) {
    //takes exisiting base stat pokemon and applies game accurate stat combonations that are randomized 
    p->setAtt(p->getAtt() + rand() % 32);
    p->setDef(p->getDef() + rand() % 32);
    p->setSpcAtt(p->getSpcAtt() + rand() % 32);
    p->setSpcDef(p->getDef() + rand() % 32);
    p->setSpeed(p->getSpeed() + rand() % 32);

    int totalIncrease = 0;
    while (totalIncrease > 128) {
        int attackIncrease = rand() % (64 - p->getAtt());
        int defenseIncrease = rand() % (64 - p->getDef());
        int specialAttackIncrease = rand() % (64 - p->getSpcAtt());
        int specialDefenseIncrease = rand() % (64 - p->getSpcDef());
        int speedIncrease = rand() % (64 - p->getSpeed());

        if (totalIncrease + attackIncrease + defenseIncrease + specialAttackIncrease + specialDefenseIncrease + speedIncrease <= 128) {
            //if all stats added in the second increase are less than 128, add them and move on
            p->setAtt(p->getAtt() + attackIncrease);
            p->setDef(p->getDef() + defenseIncrease);
            p->setSpcAtt(p->getSpcAtt() + specialAttackIncrease);
            p->setSpcDef(p->getDef() + specialDefenseIncrease);
            p->setSpeed(p->getSpeed() + speedIncrease);

            totalIncrease += attackIncrease + defenseIncrease + specialAttackIncrease + specialDefenseIncrease + speedIncrease;
        }
    }
    return p;
}

Pokemon* createPokemon(const vector<string>& names, const vector<int>& hp, const vector<int>& attack, const vector<int>& defense,
    const vector<int>& sp_atk, const vector<int>& sp_def, const vector<int>& speed, const vector<int>& pokedex_num) {
    Pokemon* poke = new Pokemon();
    //creates a default stat pokemon to be motified later

    int dex = rand() % pokedex_num.size();
    poke->setAtt(attack.at(dex));
    poke->setName(names.at(dex));
    poke->setDef(defense.at(dex));
    poke->setSpcAtt(sp_atk.at(dex));
    poke->setSpcDef(sp_def.at(dex));
    poke->setSpeed(speed.at(dex));
    poke->setHealth(hp.at(dex));

    poke = modifyStats(poke);

    return poke;
}

void addToMinheap(vector<Pokemon*>& inVector, Pokemon* pokemon, string searchType) {
    pokemon->updateSearchedVal(searchType); //executes min heap insert
    if (inVector.size() == 0) {
        inVector.push_back(pokemon);
    }
    else {
        inVector.push_back(pokemon);
        int newIndex = inVector.size() - 1;

        while (newIndex != 0) {
            Pokemon* pokeParent = inVector[(newIndex - 1) / 2];

            if (inVector[newIndex]->getSearchedValue() >= pokeParent->getSearchedValue()) {
                break;
            }
            else {
                swap(inVector[newIndex], inVector[(newIndex - 1) / 2]);
                newIndex = (newIndex - 1) / 2;
            }

        }

    }

}

vector<Pokemon*> createMinHeap(vector<Pokemon*> unsortedVector, string searchType) {

    vector<Pokemon*> returnVector;
    //creates the heap and adds all pokemon to it
    for (Pokemon* pokemon : unsortedVector) {
        addToMinheap(returnVector, pokemon, searchType);
    }

    return returnVector;
}

void searchMinHeap(vector<Pokemon*>& heap, int size, int min, int max, int index, vector<Pokemon*>& heapResults, int searchSize)
{
    if (searchSize == heapResults.size()) {
        return;
    }
    //cout << "Searching vertex " << heap[index]->getSearchedValue() << endl;
    if (index >= size) {
        // Element not found, return -1 or handle the absence accordingly
        return;
    }

    if (heap[index]->getSearchedValue() >= min && heap[index]->getSearchedValue() <= max) {
        // Element found, return the index
        heapResults.push_back(heap[index]);
    }



    // Recursively search in the left and right subtrees
    int leftChildIndex = 2 * index + 1;
    int rightChildIndex = 2 * index + 2;

    if (leftChildIndex < heap.size()) {
        if (heap[leftChildIndex]->getSearchedValue() <= max)
            searchMinHeap(heap, size, min, max, leftChildIndex, heapResults, searchSize);
    }

    if (rightChildIndex < heap.size()) {
        if (heap[rightChildIndex]->getSearchedValue() <= max)
            searchMinHeap(heap, size, min, max, rightChildIndex, heapResults, searchSize);
    }

}

void printPokemonStats(const vector<Pokemon*>& pokemonVec) {
    //displays header
    cout << left << setw(14) << "Name" << setw(8) << "|HP, " << setw(8) << "Attack, " << setw(8) << "Defense, " << setw(13) << "Special Attack, " << setw(13) << "Special Defense, " << setw(8) << "Speed" << '\n';

    //displays rest of the pokemon
    for (const auto& pokemon : pokemonVec) {
        cout << left << setw(14) << pokemon->getName() << "|" << setw(7) << pokemon->getHealth() << setw(8) << pokemon->getAtt() << setw(9) << pokemon->getDef() << setw(16) << pokemon->getSpcAtt() << setw(17) << pokemon->getSpcDef() << setw(7) << pokemon->getSpeed() << '\n';
        cout << "--------------|---------------------------------------------------------------" << endl;
    }
}

void printPokemon(vector<Pokemon*> pokemon, int inputHP, int inputATT, int inputDEF, int inputSPCATT, int inputSPCDEF, int inputSPD, int range, int count) {
    vector<Pokemon*> printvec;
    int i = 0;
    for (Pokemon* p : pokemon)
    {
        if (i == count) {
            printPokemonStats(printvec);
            return;
        } //determines the current pokemon fullfills all requierments + adds it to vector to print
        if ((p->getHealth() >= inputHP - range && p->getHealth() <= inputHP + range) || inputHP == 0)
            if ((p->getAtt() >= inputATT - range && p->getAtt() <= inputATT + range) || inputATT == 0)
                if ((p->getDef() >= inputDEF - range && p->getDef() <= inputDEF + range) || inputDEF == 0)
                    if ((p->getSpcAtt() >= inputSPCATT - range && p->getSpcAtt() <= inputSPCATT + range) || inputSPCATT == 0)
                        if ((p->getSpcDef() >= inputSPCDEF - range && p->getSpcDef() <= inputSPCDEF + range) || inputSPCDEF == 0)
                            if ((p->getSpeed() >= inputSPD - range && p->getSpeed() <= inputSPD + range) || inputSPD == 0)
                            {
                                printvec.push_back(p);
                                i++;
                            }
    }
    cout << "No Pokemon Found\n" << endl;
}

int main() {
    srand(time(0)); // seed the random number generator
    bool exitBool = true;
    string exitStr = "";

    vector<int> pokedex_num;  //declare all the vectors to read in from csv
    vector<string> names;
    vector<string> type1;
    vector<string> type2;
    vector<int> total;
    vector<int> hp;
    vector<int> attack;
    vector<int> defense;
    vector<int> sp_atk;
    vector<int> sp_def;
    vector<int> speed;
    vector<int> generation;
    vector<bool> legendary;

    readData(pokedex_num, names, type1, type2, total, hp, attack, defense, sp_atk, sp_def, speed, generation, legendary); //pass through csv data and read it in

    vector<Pokemon*> random_pokemon;
    int num_pokemon;

    int inputHP, inputATT, inputDEF, inputSPCATT, inputSPCDEF, inputSPD;
    string inputTYPE = "";

    cout << "How many random Pokemon would you like to generate? (we recommend 100,000+)" << endl;
    cin >> num_pokemon;
    cout << endl;

    for (int i = 0; i < num_pokemon; i++) {
        Pokemon* p = createPokemon(names, hp, attack, defense, sp_atk, sp_def, speed, pokedex_num);
        random_pokemon.push_back(p);    //creates all the pokemon and adds them to vector
    }
    while (exitBool) {
        cout << "Please enter your search parameters in the following format (Enter 0 for values you dont want to constrain):" << endl;
        cout << "  (type numbers seperated by spaces and note that numbers between 40-80 will return most results)" << endl;
        cout << "      Health, Attack, Defense, Special Attack, Special Defense, Speed " << endl;
        cin >> inputHP; cin >> inputATT; cin >> inputDEF; cin >> inputSPCATT; cin >> inputSPCDEF; cin >> inputSPD;
        cout << endl;

        int range = 0;
        cout << "Please enter the range you would like to bound your search:" << endl;
        cout << "(we recommend using 15-25 for most results, ex: if that stat if 60 and the range is 20 the program will return 40-80)" << endl;
        cin >> range;
        cout << endl;

        int count = 0;
        cout << "Please enter the number of matching pokemon you would like output (at the most):" << endl;
        cin >> count;
        cout << endl;

        string searchType = "";
        int usedSearchIndex = 0;
        //the following if tree will set the string search type to be the searched for permameter value that is non 0
        //the perameter value will determine what is searched for in the B tree//heap
        if (inputHP != 0)      
        {
            searchType = "health";
            usedSearchIndex = inputHP;
        }
        else if (inputATT != 0)
        {
            searchType = "attack";
            int usedSearchIndex = inputATT;
        }
        else if (inputDEF != 0)
        {
            searchType = "defense";
            int usedSearchIndex = inputDEF;
        }
        else if (inputSPCATT != 0)
        {
            searchType = "specialAttack";
            int usedSearchIndex = inputSPCATT;
        }
        else if (inputSPCDEF != 0)
        {
            searchType = "specialDefense";
            int usedSearchIndex = inputSPCDEF;
        }
        else if (inputSPD != 0)
        {
            searchType = "speed";
            int usedSearchIndex = inputSPD;
        }

        int maxIndex, minIndex;
        maxIndex = usedSearchIndex + range;
        minIndex = usedSearchIndex - range;
        if (minIndex <= 0)
            minIndex = 1;

        //start a timer for tree creation
        auto t1 = high_resolution_clock::now();

        BTree* tree = new BTree(searchType);    //initializes a B tree with the search type specified

        for (int i = 0; i < num_pokemon; i++) {
            tree->insert(random_pokemon.at(i));                    //inserts pokemon into the tree
        }

        //check timer after tree creation
        auto t2 = high_resolution_clock::now();
        duration<double> us_double = t2 - t1;
        cout << "Time to create tree: " << us_double.count() << " seconds." << endl;

        //start a timer for heap creation
        t1 = high_resolution_clock::now();

        vector<Pokemon*> heap = createMinHeap(random_pokemon, searchType);  //starts heap insertion of all elements in the vector

        //check timer after heap creation
        t2 = high_resolution_clock::now();
        us_double = t2 - t1;
        cout << "Time to create heap: " << us_double.count() << " seconds." << endl;

        //start a timer for tree search
        t1 = high_resolution_clock::now();
        //searches through the tree
        vector<Pokemon*> treeOutput = tree->findPokemon(maxIndex, minIndex, -1);
        vector<Pokemon*> heapOutput;
        //check timer after tree search
        t2 = high_resolution_clock::now();
        us_double = t2 - t1;
        cout << "Time to search tree: " << us_double.count() << " seconds." << endl;

        //start a timer for heap search
        t1 = high_resolution_clock::now();
        //searches through the heap
        searchMinHeap(heap, heap.size(), minIndex, maxIndex, 0, heapOutput, -1);
        //check timer after heap search
        t2 = high_resolution_clock::now();
        us_double = t2 - t1;
        cout << "Time to search heap: " << us_double.count() << " seconds." << endl;

        cout << endl << "BTree search results:" << endl;
        printPokemon(treeOutput, inputHP, inputATT, inputDEF, inputSPCATT, inputSPCDEF, inputSPD, range, count);
        cout << endl << "Heap search results:" << endl;
        printPokemon(heapOutput, inputHP, inputATT, inputDEF, inputSPCATT, inputSPCDEF, inputSPD, range, count);

        cout << "\n\nWould you like to search again? Y/N" << endl;
        cin >> exitStr;
        if (exitStr == "N" || exitStr == "n") {
            exitBool = false;
        }
        cout << endl;
        
    }
    // free the memory used by the dynamically allocated pokemon structs
    for (unsigned int i = 0; i < random_pokemon.size(); i++) {
        delete random_pokemon.at(i);
    }
    cout << "Memory Deallocated, goodbye" << endl;
    return 0;
}