#include <bits/stdc++.h>
using namespace std;

enum class TransactionType
{
    WITHDRAW,
    BALANCE_INQUIRY
};


class BankAccount {
    private:
        string accountNumber;
        string pin;
        double balance;

    public:
        BankAccount(const string &accountNumber, const string &pin, double balance) : accountNumber(accountNumber) , pin(pin) , balance(balance) {}

        bool validatePIN(const string& enteredPIN) const {
            return enteredPIN == pin;
        }

        bool withdraw(double amount) {
            if(amount <= 0) return false;

            if(amount > balance) return false;

            balance -= amount;

            return true;
        }

        void deposit(double amount) {
            if(amount <= 0)
                return;

            balance += amount;
        }

        double getBalance() const {
            return balance;
        }

        string getAccountNumber() const {
            return accountNumber;
        }
};


class Card {
    private:
        string cardNumber;
        string expiryDate;
        string cvv;
        BankAccount* account;

    public:
        Card(const string &cardNumber , const string &expiryDate , const string &cvv , BankAccount* account) : cardNumber(cardNumber) , expiryDate(expiryDate) , cvv(cvv) , account(account) {}

        string getCardNumber() const
        {
            return cardNumber;
        }

        BankAccount* getBankAccount() const
        {
            return account;
        }

        bool authenticate(const string &enteredPIN) const {
            return account -> validatePIN(enteredPIN);
        }
};



// Chain of Responsibility for Cash Dispensing
class CashHandler {
    protected:
        int denomination;
        int noteCount;
        CashHandler* nextHandler;

    public:
        CashHandler(int denomination, int noteCount) : denomination(denomination), noteCount(noteCount), nextHandler(nullptr) {}

        void setNextHandler(CashHandler* next)
        {
            nextHandler = next;
        }

        // Returns true if the full amount could be dispensed.
        bool dispense(int amount)
        {
            if(amount <= 0) return true;

            int notesToUse = min(amount / denomination, noteCount);

            if(notesToUse > 0) {
                noteCount -= notesToUse;
                amount -= notesToUse * denomination;
                cout << "Dispensing " << notesToUse << " x Rs." << denomination << " note(s)" << endl;
            }

            if(amount == 0) return true;

            if(nextHandler != nullptr) return nextHandler->dispense(amount);

            return false;
        }

        virtual ~CashHandler() {}
};


// State Pattern of ATM
class IdleState;
class CardInsertedState;
class AuthenticatedState;
class TransactionState;

class IATMState {
    public:

        virtual void insertCard(class ATM& atm , Card& card) = 0;

        virtual void authenticate(class ATM& atm , const string& pin) = 0;

        virtual void selectTransaction(class ATM& atm , TransactionType type , double amount = 0) = 0;

        virtual void ejectCard(class ATM& atm) = 0;

        virtual ~IATMState() {}
};




class ATM {
    private:
        IATMState* currentState;
        Card* currentCard;
        double totalCash;
        CashHandler* dispenserChain;

    public:
        ATM(IATMState* state, double cash, CashHandler* dispenserChain)
            : currentState(state), currentCard(nullptr), totalCash(cash), dispenserChain(dispenserChain) {}

        // ---------------- State ----------------

        void setState(IATMState* state)
        {
            currentState = state;
        }

        IATMState* getState() const
        {
            return currentState;
        }

        // ---------------- Card ----------------

        void setCard(Card* card)
        {
            currentCard = card;
        }

        Card* getCard() const
        {
            return currentCard;
        }

        void removeCard()
        {
            currentCard = nullptr;
        }

        // ---------------- Cash ----------------

        double getCash() const
        {
            return totalCash;
        }

        // Runs the amount through the denomination chain and only deducts
        // totalCash if the chain could dispense it in full.
        bool dispenseCash(double amount)
        {
            int wholeAmount = (int) amount;

            if(!dispenserChain->dispense(wholeAmount)) return false;

            totalCash -= amount;
            return true;
        }

        // ---------------- User Operations ----------------

        void insertCard(Card* card)
        {
            currentState -> insertCard(*this, *card);
        }

        void authenticate(const string& pin)
        {
            currentState -> authenticate(*this, pin);
        }

        void selectTransaction(TransactionType type, double amount = 0)
        {
            currentState -> selectTransaction(*this, type, amount);
        }

        void ejectCard()
        {
            currentState -> ejectCard(*this);
        }
};



// ---------------- Concrete States ----------------

class IdleState : public IATMState {
    public:
        void insertCard(ATM& atm, Card& card) override;
        void authenticate(ATM& atm, const string& pin) override;
        void selectTransaction(ATM& atm, TransactionType type, double amount = 0) override;
        void ejectCard(ATM& atm) override;
};

class CardInsertedState : public IATMState {
    public:
        void insertCard(ATM& atm, Card& card) override;
        void authenticate(ATM& atm, const string& pin) override;
        void selectTransaction(ATM& atm, TransactionType type, double amount = 0) override;
        void ejectCard(ATM& atm) override;
};

class AuthenticatedState : public IATMState {
    public:
        void insertCard(ATM& atm, Card& card) override;
        void authenticate(ATM& atm, const string& pin) override;
        void selectTransaction(ATM& atm, TransactionType type, double amount = 0) override;
        void ejectCard(ATM& atm) override;
};

class TransactionState : public IATMState {
    public:
        void insertCard(ATM& atm, Card& card) override;
        void authenticate(ATM& atm, const string& pin) override;
        void selectTransaction(ATM& atm, TransactionType type, double amount = 0) override;
        void ejectCard(ATM& atm) override;
};


// Singleton-ish shared state instances, so setState() calls don't need `new`.
IdleState idleState;
CardInsertedState cardInsertedState;
AuthenticatedState authenticatedState;
TransactionState transactionState;


// ---------------- IdleState ----------------

void IdleState::insertCard(ATM& atm, Card& card)
{
    cout << "Card inserted: " << card.getCardNumber() << endl;
    atm.setCard(&card);
    atm.setState(&cardInsertedState);
}

void IdleState::authenticate(ATM& atm, const string& pin)
{
    cout << "Please insert a card first." << endl;
}

void IdleState::selectTransaction(ATM& atm, TransactionType type, double amount)
{
    cout << "Please insert a card first." << endl;
}

void IdleState::ejectCard(ATM& atm)
{
    cout << "No card to eject." << endl;
}


// ---------------- CardInsertedState ----------------

void CardInsertedState::insertCard(ATM& atm, Card& card)
{
    cout << "A card is already inserted." << endl;
}

void CardInsertedState::authenticate(ATM& atm, const string& pin)
{
    Card* card = atm.getCard();

    if(card != nullptr && card->authenticate(pin)) {
        cout << "Authentication successful." << endl;
        atm.setState(&authenticatedState);
    } else {
        cout << "Incorrect PIN." << endl;
    }
}

void CardInsertedState::selectTransaction(ATM& atm, TransactionType type, double amount)
{
    cout << "Please authenticate first." << endl;
}

void CardInsertedState::ejectCard(ATM& atm)
{
    cout << "Card ejected." << endl;
    atm.removeCard();
    atm.setState(&idleState);
}


// ---------------- AuthenticatedState ----------------

void AuthenticatedState::insertCard(ATM& atm, Card& card)
{
    cout << "A card is already inserted." << endl;
}

void AuthenticatedState::authenticate(ATM& atm, const string& pin)
{
    cout << "Already authenticated." << endl;
}

void AuthenticatedState::selectTransaction(ATM& atm, TransactionType type, double amount)
{
    BankAccount* account = atm.getCard()->getBankAccount();

    switch(type) {
        case TransactionType::BALANCE_INQUIRY:
            cout << "Your balance is: " << account->getBalance() << endl;
            break;

        case TransactionType::WITHDRAW:
            if(amount <= 0) {
                cout << "Invalid withdrawal amount." << endl;
                break;
            }

            if(amount > atm.getCash()) {
                cout << "ATM has insufficient cash." << endl;
                break;
            }

            if(!account->withdraw(amount)) {
                cout << "Insufficient balance." << endl;
                break;
            }

            if(!atm.dispenseCash(amount)) {
                // Chain couldn't break the amount into available notes; refund the account.
                account->deposit(amount);
                cout << "ATM cannot dispense this amount with available denominations." << endl;
                break;
            }

            cout << "Please collect your cash: " << amount << endl;
            break;
    }

    atm.setState(&transactionState);
}

void AuthenticatedState::ejectCard(ATM& atm)
{
    cout << "Card ejected." << endl;
    atm.removeCard();
    atm.setState(&idleState);
}


// ---------------- TransactionState ----------------

void TransactionState::insertCard(ATM& atm, Card& card)
{
    cout << "A card is already inserted." << endl;
}

void TransactionState::authenticate(ATM& atm, const string& pin)
{
    cout << "Already authenticated." << endl;
}

void TransactionState::selectTransaction(ATM& atm, TransactionType type, double amount)
{
    // Delegate back to AuthenticatedState behavior so multiple
    // transactions can be performed in one session.
    authenticatedState.selectTransaction(atm, type, amount);
}

void TransactionState::ejectCard(ATM& atm)
{
    cout << "Transaction complete. Card ejected." << endl;
    atm.removeCard();
    atm.setState(&idleState);
}


// ---------------- Demo ----------------

int main()
{
    BankAccount account("ACC1001", "1234", 5000.0);
    Card card("CARD1001", "12/29", "123", &account);

    // Chain of Responsibility: Rs.2000 -> Rs.500 -> Rs.100
    CashHandler rs2000Handler(2000, 20);
    CashHandler rs500Handler(500, 40);
    CashHandler rs100Handler(100, 100);

    rs2000Handler.setNextHandler(&rs500Handler);
    rs500Handler.setNextHandler(&rs100Handler);

    ATM atm(&idleState, 100000.0, &rs2000Handler);

    atm.selectTransaction(TransactionType::BALANCE_INQUIRY);   // no card yet

    atm.insertCard(&card);
    atm.authenticate("0000");                                  // wrong pin
    atm.authenticate("1234");                                  // correct pin

    atm.selectTransaction(TransactionType::BALANCE_INQUIRY);
    atm.selectTransaction(TransactionType::WITHDRAW, 1500.0);
    atm.selectTransaction(TransactionType::BALANCE_INQUIRY);

    atm.ejectCard();

    return 0;
}