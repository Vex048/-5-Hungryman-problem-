class hungryPerson { // Klasa głodomora
    public:
        hungryPerson(int _id){ id =_id; }; // Konstruktor klasy 
        int getId();
        void think();
        void eat();
        int getAverageBalance();
        void addToBalance(int mealWeight);
        int getState();
        int getMealWeight();
        void setMeal(int mealSet);
        void setAverageBalance(int temp);
        void endt();
    private:
        int id; // Id głodomora
        int averageBalance; // Ogólny priorytet/balans
        int state; // Stan głodomora
        int meal; // Waga posiłku

};