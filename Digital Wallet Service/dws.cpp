#include <bits/stdc++.h>
using namespace std;

enum class CurrencyCode {
    USD,
    INR,
    EUR,
    GBP,
    JPY
};

enum class TransactionType {
    DEPOSIT,
    WITHDRAW,
    TRANSFER,
    CONVERSION
};


class User {
    private:
        int userId;
        string name;
        string email;
        string password;
        string address;
        string contact;

    public:

        User() = default;

        User(int id, string name, string email, string password, string address, string contact) : 
            userId(id), name(name), email(email), password(password), address(address), contact(contact) {};

        int getUserId() {
            return userId;
        }

        string getName() {
            return name;
        }

        string getEmail() {
            return email;
        }

        string getPassword() {
            return password;
        }

        string getAddress() {
            return address;
        }

        string getContact() {
            return contact;
        }
};


class AuthService {
    private:
        unordered_map<string, User>users;
        int nextUserId = 1;

    public:
        bool registerUser(string name, string email, string password, string address, string contact) {

            if(users.find(email) != users.end()) {
                return false;
            }

            User user(
                nextUserId++,
                name,
                email,
                password,
                address,
                contact
            );

            users[email] = user;

            return true;
        }
};


class Currency {
    private:
        CurrencyCode code;
        string name;
        string symbol;

    public:
        Currency(CurrencyCode code, string name, string symbol) : 
            code(code), name(name), symbol(symbol) {};

        CurrencyCode getCode() {
            return code;
        }

        string getName() {
            return name;
        }

        string getSymbol() {
            return symbol;
        }
};

class CurrencyService {
    private:
        unordered_map<CurrencyCode, double> exchangeRates;

    public:

        CurrencyService() {
            exchangeRates[CurrencyCode::USD] = 1.0;
            exchangeRates[CurrencyCode::INR] = 83.0;
            exchangeRates[CurrencyCode::EUR] = 0.92;
            exchangeRates[CurrencyCode::GBP] = 0.79;
            exchangeRates[CurrencyCode::JPY] = 150.0;
        }

        long long convert(
            long long amount,
            CurrencyCode from,
            CurrencyCode to
        ) {

            if(from == to) {
                return amount;
            }

            double amountInUSD =
                amount / exchangeRates[from];

            double convertedAmount =
                amountInUSD * exchangeRates[to];

            return static_cast<long long>(convertedAmount);
        }
};


class PaymentService {
    public:
        virtual void pay(int amount) = 0;
        virtual ~PaymentService() {};
};

class CreditCardPayment : public PaymentService {
    public:
        void pay(int amount) override {
            cout << "Paid " << amount << " using Credit Card\n";
        }
};

class UPIPayment : public PaymentService {
    public:
        void pay(int amount) override {
            cout << "Paid " << amount << " using UPI\n";
        }
};


class Wallet {
    private:
        int walletId;
        int userId;

        unordered_map<CurrencyCode, long long> balances;

    public:
        Wallet(int walletId, int userId)
            : walletId(walletId), userId(userId) {}

        int getWalletId() {
            return walletId;
        }

        int getUserId() {
            return userId;
        }

        long long getBalance(CurrencyCode currency) {
            return balances[currency];
        }

        void addBalance(CurrencyCode currency, long long amount) {
            balances[currency] += amount;
        }

        bool deductBalance(CurrencyCode currency, long long amount) {
            if(balances[currency] < amount) {
                return false;
            }

            balances[currency] -= amount;
            return true;
        }
};


class Transaction {
    private:
        int transactionId;
        TransactionType type;

        int fromWalletId;
        int toWalletId;

        long long amount;
        CurrencyCode currency;

        long long convertedAmount;
        CurrencyCode convertedCurrency;

        time_t timestamp;

    public:

        Transaction(
            int transactionId,
            TransactionType type,
            int fromWalletId,
            int toWalletId,
            long long amount,
            CurrencyCode currency,
            long long convertedAmount = 0,
            CurrencyCode convertedCurrency = CurrencyCode::USD
        ) :
            transactionId(transactionId),
            type(type),
            fromWalletId(fromWalletId),
            toWalletId(toWalletId),
            amount(amount),
            currency(currency),
            convertedAmount(convertedAmount),
            convertedCurrency(convertedCurrency),
            timestamp(time(nullptr)) {}


        int getTransactionId() {
            return transactionId;
        }

        TransactionType getType() {
            return type;
        }

        int getFromWalletId() {
            return fromWalletId;
        }

        int getToWalletId() {
            return toWalletId;
        }

        long long getAmount() {
            return amount;
        }

        CurrencyCode getCurrency() {
            return currency;
        }

        long long getConvertedAmount() {
            return convertedAmount;
        }

        CurrencyCode getConvertedCurrency() {
            return convertedCurrency;
        }

        time_t getTimestamp() {
            return timestamp;
        }
};


class WalletService {
    private:
        unordered_map<int, Wallet*> wallets;
        unordered_map<int, vector<Transaction>> transactionHistory;

        CurrencyService* currencyService;

        int nextWalletId = 1;
        int nextTransactionId = 1;

    public:

        WalletService(CurrencyService* currencyService)
            : currencyService(currencyService) {
        }


        ~WalletService() {
            for(auto &[walletId, wallet] : wallets) {
                delete wallet;
            }
        }


        // =========================
        // Create Wallet
        // =========================

        Wallet* createWallet(int userId) {

            Wallet* wallet = new Wallet(
                nextWalletId++,
                userId
            );

            wallets[wallet->getWalletId()] = wallet;

            return wallet;
        }


        // =========================
        // Get Wallet
        // =========================

        Wallet* getWallet(int walletId) {

            if(wallets.find(walletId) == wallets.end()) {
                return nullptr;
            }

            return wallets[walletId];
        }


        // =========================
        // Add Funds
        // =========================

        bool addFunds(
            int walletId,
            long long amount,
            CurrencyCode currency
        ) {

            Wallet* wallet = getWallet(walletId);

            if(wallet == nullptr || amount <= 0) {
                return false;
            }

            wallet->addBalance(
                currency,
                amount
            );


            Transaction transaction(
                nextTransactionId++,
                TransactionType::DEPOSIT,
                -1,
                walletId,
                amount,
                currency
            );

            transactionHistory[walletId].push_back(
                transaction
            );

            return true;
        }


        // =========================
        // Withdraw Funds
        // =========================

        bool withdrawFunds(
            int walletId,
            long long amount,
            CurrencyCode currency
        ) {

            Wallet* wallet = getWallet(walletId);

            if(wallet == nullptr || amount <= 0) {
                return false;
            }

            if(!wallet->deductBalance(
                currency,
                amount
            )) {
                return false;
            }


            Transaction transaction(
                nextTransactionId++,
                TransactionType::WITHDRAW,
                walletId,
                -1,
                amount,
                currency
            );

            transactionHistory[walletId].push_back(
                transaction
            );

            return true;
        }


        // =========================
        // Transfer Funds
        // =========================

        bool transferFunds(
            int fromWalletId,
            int toWalletId,
            long long amount,
            CurrencyCode currency
        ) {

            if(
                fromWalletId == toWalletId ||
                amount <= 0
            ) {
                return false;
            }


            Wallet* fromWallet =
                getWallet(fromWalletId);

            Wallet* toWallet =
                getWallet(toWalletId);


            if(
                fromWallet == nullptr ||
                toWallet == nullptr
            ) {
                return false;
            }


            // Deduct from sender
            if(!fromWallet->deductBalance(
                currency,
                amount
            )) {
                return false;
            }


            // Credit receiver
            toWallet->addBalance(
                currency,
                amount
            );


            Transaction transaction(
                nextTransactionId++,
                TransactionType::TRANSFER,
                fromWalletId,
                toWalletId,
                amount,
                currency
            );


            // Store transaction for both wallets
            transactionHistory[fromWalletId]
                .push_back(transaction);

            transactionHistory[toWalletId]
                .push_back(transaction);


            return true;
        }


        // =========================
        // Currency Conversion
        // =========================

        bool convertCurrency(
            int walletId,
            long long amount,
            CurrencyCode from,
            CurrencyCode to
        ) {

            Wallet* wallet = getWallet(walletId);

            if(wallet == nullptr || amount <= 0) {
                return false;
            }

            if(from == to) {
                return false;
            }


            // Check and deduct source currency
            if(!wallet->deductBalance(
                from,
                amount
            )) {
                return false;
            }


            // Convert using CurrencyService
            long long convertedAmount =
                currencyService->convert(
                    amount,
                    from,
                    to
                );


            // Add converted currency
            wallet->addBalance(
                to,
                convertedAmount
            );


            // Record conversion
            Transaction transaction(
                nextTransactionId++,
                TransactionType::CONVERSION,
                walletId,
                walletId,
                amount,
                from,
                convertedAmount,
                to
            );


            transactionHistory[walletId]
                .push_back(transaction);


            return true;
        }


        // =========================
        // Transaction History
        // =========================

        vector<Transaction> getTransactionHistory(
            int walletId
        ) {

            if(
                transactionHistory.find(walletId)
                == transactionHistory.end()
            ) {
                return {};
            }

            return transactionHistory[walletId];
        }


        // =========================
        // Print Statement
        // =========================

        void printStatement(int walletId) {

            if(getWallet(walletId) == nullptr) {
                cout << "Wallet not found\n";
                return;
            }


            auto transactions =
                getTransactionHistory(walletId);


            cout << "\n";
            cout << "==============================\n";
            cout << "       WALLET STATEMENT\n";
            cout << "==============================\n";


            for(auto &transaction : transactions) {

                cout << "\nTransaction ID: "
                     << transaction.getTransactionId();


                cout << "\nAmount: "
                     << transaction.getAmount();


                cout << "\nFrom Wallet: "
                     << transaction.getFromWalletId();


                cout << "\nTo Wallet: "
                     << transaction.getToWalletId();


                cout << "\nType: ";

                if(
                    transaction.getType()
                    == TransactionType::DEPOSIT
                ) {
                    cout << "DEPOSIT";
                }
                else if(
                    transaction.getType()
                    == TransactionType::WITHDRAW
                ) {
                    cout << "WITHDRAW";
                }
                else if(
                    transaction.getType()
                    == TransactionType::TRANSFER
                ) {
                    cout << "TRANSFER";
                }
                else {
                    cout << "CONVERSION";

                    cout << "\nConverted Amount: "
                         << transaction.getConvertedAmount();
                }


                cout << "\n------------------------------";
            }

            cout << "\n";
        }
};


int main() {

    // =====================================
    // 1. Create Authentication Service
    // =====================================

    AuthService authService;


    authService.registerUser(
        "Deepak",
        "deepak@gmail.com",
        "password123",
        "Dehradun",
        "9876543210"
    );


    authService.registerUser(
        "Rahul",
        "rahul@gmail.com",
        "password456",
        "Delhi",
        "9876500000"
    );


    // =====================================
    // 2. Create Currency Service
    // =====================================

    CurrencyService currencyService;


    // =====================================
    // 3. Create Wallet Service
    // =====================================

    WalletService walletService(
        &currencyService
    );


    // =====================================
    // 4. Create Wallets
    // =====================================

    Wallet* deepakWallet =
        walletService.createWallet(1);


    Wallet* rahulWallet =
        walletService.createWallet(2);


    cout << "Deepak Wallet ID: "
         << deepakWallet -> getWalletId()
         << "\n";


    cout << "Rahul Wallet ID: "
         << rahulWallet -> getWalletId()
         << "\n";


    // =====================================
    // 5. Add Funds
    // =====================================

    walletService.addFunds(
        deepakWallet -> getWalletId(),
        100000,
        CurrencyCode::INR
    );


    walletService.addFunds(
        deepakWallet -> getWalletId(),
        500,
        CurrencyCode::USD
    );


    walletService.addFunds(
        rahulWallet -> getWalletId(),
        50000,
        CurrencyCode::INR
    );


    cout << "\n===== Initial Balances =====\n";


    cout << "Deepak INR: "
         << deepakWallet -> getBalance(
                CurrencyCode::INR
            )
         << "\n";


    cout << "Deepak USD: "
         << deepakWallet -> getBalance(
                CurrencyCode::USD
            )
         << "\n";


    cout << "Rahul INR: "
         << rahulWallet -> getBalance(
                CurrencyCode::INR
            )
         << "\n";


    // =====================================
    // 6. Currency Conversion
    // =====================================

    cout << "\n===== Currency Conversion =====\n";


    cout << "Converting 100 USD to INR...\n";


    bool conversionSuccessful =
        walletService.convertCurrency(
            deepakWallet -> getWalletId(),
            100,
            CurrencyCode::USD,
            CurrencyCode::INR
        );


    if(conversionSuccessful) {
        cout << "Conversion successful!\n";
    }
    else {
        cout << "Conversion failed!\n";
    }


    // =====================================
    // 7. Check Balance After Conversion
    // =====================================

    cout << "\n===== After Conversion =====\n";


    cout << "Deepak USD: "
         << deepakWallet -> getBalance(
                CurrencyCode::USD
            )
         << "\n";


    cout << "Deepak INR: "
         << deepakWallet -> getBalance(
                CurrencyCode::INR
            )
         << "\n";


    // =====================================
    // 8. Transfer Funds
    // =====================================

    cout << "\n===== Transfer =====\n";


    cout << "Transferring 25,000 INR "
         << "from Deepak to Rahul...\n";


    bool transferSuccessful =
        walletService.transferFunds(
            deepakWallet -> getWalletId(),
            rahulWallet -> getWalletId(),
            25000,
            CurrencyCode::INR
        );


    if(transferSuccessful) {
        cout << "Transfer successful!\n";
    }
    else {
        cout << "Transfer failed!\n";
    }


    // =====================================
    // 9. Check Balances
    // =====================================

    cout << "\n===== After Transfer =====\n";


    cout << "Deepak INR: "
         << deepakWallet -> getBalance(
                CurrencyCode::INR
            )
         << "\n";


    cout << "Rahul INR: "
         << rahulWallet -> getBalance(
                CurrencyCode::INR
            )
         << "\n";


    // =====================================
    // 10. Withdraw Funds
    // =====================================

    cout << "\n===== Withdrawal =====\n";


    cout << "Rahul withdrawing 10,000 INR...\n";


    bool withdrawalSuccessful =
        walletService.withdrawFunds(
            rahulWallet -> getWalletId(),
            10000,
            CurrencyCode::INR
        );


    if(withdrawalSuccessful) {
        cout << "Withdrawal successful!\n";
    }
    else {
        cout << "Withdrawal failed!\n";
    }


    // =====================================
    // 11. Final Balances
    // =====================================

    cout << "\n===== Final Balances =====\n";


    cout << "Deepak INR: "
         << deepakWallet -> getBalance(
                CurrencyCode::INR
            )
         << "\n";


    cout << "Deepak USD: "
         << deepakWallet -> getBalance(
                CurrencyCode::USD
            )
         << "\n";


    cout << "Rahul INR: "
         << rahulWallet -> getBalance(
                CurrencyCode::INR
            )
         << "\n";


    // =====================================
    // 12. Print Statements
    // =====================================

    cout << "\n\n";
    cout << "Deepak's Statement\n";

    walletService.printStatement(
        deepakWallet -> getWalletId()
    );


    cout << "\n\n";
    cout << "Rahul's Statement\n";

    walletService.printStatement(
        rahulWallet -> getWalletId()
    );


    return 0;
}