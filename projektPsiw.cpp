#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include "hungryPerson.h"
using namespace std;

void checkingPriority(hungryPerson* hungryPersonMain,hungryPerson* hungryPersonLeft,hungryPerson* hungryPersonRight){
    // Funkcja, która sprawdza, który Głodomór ma pierwszeństwo do widelcy
    int balanceMain,balanceLeft,balanceRight,stateLeft,stateRight;
    while (1){
        balanceMain=hungryPersonMain->getAverageBalance();
        balanceLeft=hungryPersonLeft->getAverageBalance();
        balanceRight=hungryPersonRight->getAverageBalance();
        stateLeft=hungryPersonLeft->getState();
        stateRight=hungryPersonRight->getState();
    if ((balanceMain < balanceLeft) && (balanceMain < balanceRight) ){  // Porównujemy balans głodomorów, jeżeli jest mniejszy od dwóch głodomorów siedzących obok niego, wtedy fukncja się kończy i aktualny głodomór je
        
        break;
    } else if (((balanceMain <= balanceLeft) && (balanceMain <= balanceRight)) || ((balanceMain <= balanceLeft) && (balanceMain < balanceRight)) || ((balanceMain < balanceLeft) && (balanceMain <= balanceRight))){
        // Drugi przypadek, w którym priorytet lewgo albo prawego głodomora może być równy priorytetowi aktualnego
        break;

    } else if((stateLeft ==2) && (stateRight ==2)){ // Przypadek, w którym głodomór lewy i prawy skończyli już jeść
        break;

    } else if (((stateLeft ==2) && (balanceMain <= balanceRight))){ // Przypadek, w którym głodomór lewy skończył już jeść
        break;

    } else if(((stateRight ==2) && (balanceMain <= balanceLeft))){ // Przypadek, w którym głodomór prawy skończył już jeść
        break;

    }
    }
    

}

void startEating(vector<hungryPerson*> hungryPeople,int id, mutex* forks,mutex* printLock,int sec){
    chrono::time_point start = chrono::steady_clock::now(); // rozpoczecie odliczania
    int k;
    id=id-1; // zmniejszenie id o -1 ponieważ, vector i widelce są indeksowane od 0 do 4

    while (1){
        k=(rand()%3); // wybor losowej liczby
        this_thread::sleep_for(chrono::seconds(k)); // thread zasypia na k second
        hungryPeople[id]->think(); // glodomor mysli i zmnienia swoj stan
        printLock[0].lock(); // lockuje terminal 
        cout << "Głodomór " <<  hungryPeople[id]->getId() << " think" << endl;
        printLock[0].unlock(); // odblokowuje terminal
        this_thread::sleep_for(chrono::seconds(k)); // thread zasypia na k second
        

        
        if (id == 0){ //  funkcja, ktora sprawdza priorytet dla glodomorow  
            checkingPriority(hungryPeople[id],hungryPeople[4],hungryPeople[(id+1)%5]);
        }
        else{
            checkingPriority(hungryPeople[id],hungryPeople[(id-1)%5],hungryPeople[(id+1)%5]);
        }
        // lockowanie widelcow
        forks[id].lock();
        forks[(id+1)%5].lock();

        
        hungryPeople[id]->eat(); // zmiana stanu głodomora na jedzenie

        printLock[0].lock();
        cout << "Głodomór " <<  hungryPeople[id]->getId() << " eat" << endl;
        printLock[0].unlock();

        k=(rand()%3);
        this_thread::sleep_for(chrono::seconds(k));
        // odblokowanie widelcow
        forks[id].unlock();
        forks[(id+1)%5].unlock();

        hungryPeople[id]->addToBalance(hungryPeople[id]->getMealWeight()); //dodanie posiłku do balansu 

        printLock[0].lock();
        cout << "Głodomór " <<  hungryPeople[id]->getId() << " balance: " << hungryPeople[id]->getAverageBalance() << endl;
        printLock[0].unlock();

        k=(rand()%3);
        this_thread::sleep_for(chrono::seconds(k)); // zasniecie threadu na k sekund

        if(chrono::steady_clock::now() - start > chrono::seconds(sec)){ // Po x sekundach, Głodomór kończy jedzenie
            printLock[0].lock();
            cout << "Głodomór " <<  hungryPeople[id]->getId() << " Skonczyl jesc" << endl;
            printLock[0].unlock();
            hungryPeople[id]->endt(); // Głodomor kończy jedzenie i ustawia stan na 2
            break;
            }
            
    }
}

int main(){
    srand(time(0));
    int n=10, k, lb=1,sec;
    mutex forks[5]; // Tablica mutexów, ktora przedstawia widelce
    mutex printLock[1]; // Mutex, który powoduje, ze dostep do terminala, ma tylko jeden filozof naraz, żeby outputy nie nachodziły na siebie
    vector<hungryPerson*> hungryPeople;  // vector Głodomorow
    vector<thread> ThreadsForPeople; // vector threadow 
    cout << "Przez jakis czas ma działaś program (w sekundach)" << endl;
    cin >> sec;
    for (int i=0; i<5;++i){
        hungryPeople.push_back(new hungryPerson(i+1)); // tworzenie nowych obiektów Głodomorow i pushowanie ich do vectora
        k=(rand()%n)+lb; // Losowanie wag posiłków dla głodomorów
        hungryPeople[i]->setMeal(k); // ustawianie wartosci posiłku dla głodomora
        cout << "Waga posiłku Głodomora " << i+1 << ": " << hungryPeople[i]->getMealWeight() << endl;
        hungryPeople[i]->setAverageBalance(0); //ustawienie ogólnego balansu = 0
    }
    for (int i=1;i<6;++i){
        ThreadsForPeople.push_back(thread(startEating,hungryPeople,i,forks,printLock,sec)); // wrzucenie do vectora threadow 
    }
       
    for (auto& it : ThreadsForPeople){
        it.join(); // rozpoczecie threadow
    }

 
    cout << "Balans 1 Głodomora " << hungryPeople[0]->getAverageBalance()<< endl;
    cout << "Balans 2 Głodomora " << hungryPeople[1]->getAverageBalance()<< endl;
    cout << "Balans 3 Głodomora " << hungryPeople[2]->getAverageBalance()<< endl;
    cout << "Balans 4 Głodomora " << hungryPeople[3]->getAverageBalance()<< endl;
    cout << "Balans 5 Głodomora " << hungryPeople[4]->getAverageBalance()<< endl;


    return 0;
}




int hungryPerson::getId(){
    return id;  // zwraca id
}

void hungryPerson::setAverageBalance(int temp){
    averageBalance=temp;    // ustawia ogólny balans
}
int hungryPerson::getAverageBalance(){
    return averageBalance; // zwraca ogólny balans
}
int hungryPerson::getState(){
    return state;   // zwraca stan
}
void hungryPerson::eat(){ 
    state=1; // 1 Status kiedy Głodomor je
}
void hungryPerson::think(){
    state=0; // 0 Status kiedy Głodomor mysli
}
void hungryPerson::addToBalance(int mealWeight){
    averageBalance += mealWeight;    // Dodanie do ogólnego balansu posiłku
}
int hungryPerson::getMealWeight(){
    return meal;    // Otrzymanie wartosci posiłku
}
void hungryPerson::setMeal(int mealSet){
    meal=mealSet; // Ustawienie wartosci posiłku 
}
void hungryPerson::endt(){
    state=2; // 2 status kiedy Głodomor skonczy jesc
}
