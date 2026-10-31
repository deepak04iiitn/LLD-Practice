#include <bits/stdc++.h>
using namespace std;

class User {
    private:
        string userid;
        string name;
        // vector<Group*>groups;
        // unordered_map<string , pair<string , int>>mp;   // <groupid , <userid , amount(give) >>
        // unordered_map<string , pair<string , int>>mp;   // <groupid , <userid , amount(take) >>

    public: 
        User(string id , string name) : userid(id) , name(name) {};

        string getUserid() {
            return userid;
        }

        string getName() {
            return name;
        }
};


class Group {
    private:
        string grpid;
        string grpname;

    public:
        Group(string id , string name) : grpid(id) , grpname(name) {};

        string getGrpid() {
            return grpid;
        }

        string getGrpname() {
            return grpname;
        }
};


// id -> user lookup, so logs can print names instead of ids
class UserRegistry {
    private:
        unordered_map<string , User*>users;

    public:
        void addUser(User* u) {
            users[u -> getUserid()] = u;
        }

        string nameOf(string userid) {
            auto it = users.find(userid);
            return it == users.end() ? userid : it -> second -> getName();
        }
};


class Expense {
    private:
        string expid;
        string name;
        int amount;

    public:
        Expense(string id , string name , int amt) : expid(id) , name(name) , amount(amt) {};

        string getExpid() {
            return expid;
        }

        string getName() {
            return name;
        }

        int getAmount() {
            return amount;
        }
};


class GroupManagementService {
    private:
        vector<Group*>groups;
        unordered_map<string , Group*>idToGroup;
        unordered_map<string , vector<string>>mp;  // <groupId , <vector<userid>>
        UserRegistry* registry;

    public:
        GroupManagementService(UserRegistry* r) : registry(r) {};

        string nameOf(string userid) {
            return registry -> nameOf(userid);
        }

        string groupNameOf(string grpid) {
            auto it = idToGroup.find(grpid);
            return it == idToGroup.end() ? grpid : it -> second -> getGrpname();
        }

        void addGroup(Group* g , string createdBy) {
            groups.push_back(g);
            idToGroup[g -> getGrpid()] = g;
            cout << "[GROUP] \"" << g -> getGrpname() << "\" created by " << nameOf(createdBy) << "\n";
        }

        void addUsersInGroup(string userid , string grpid) {
            mp[grpid].push_back(userid);
            cout << "[GROUP] " << nameOf(userid) << " added to \"" << groupNameOf(grpid) << "\"\n";
        }

        void removeUserFromGroup(string userid , string grpid) {
            auto it = mp.find(grpid);

            if(it == mp.end()) return;

            auto &users = it -> second;

            auto userIt = find(users.begin() , users.end() , userid);

            if(userIt != users.end()) {
                users.erase(userIt);
                cout << "[GROUP] " << nameOf(userid) << " removed from \"" << groupNameOf(grpid) << "\"\n";
            }

        }
};



class PaymentStrategy {
    public:
        virtual void pay(int amount) = 0;
        virtual ~PaymentStrategy() {};
};


class CreditCardPayment : public PaymentStrategy {
    public:
        void pay(int amount) override {
            cout << "Paid " << amount << " using Credit Card\n";
        }
};

class UPIPayment : public PaymentStrategy {
    public:
        void pay(int amount) override {
            cout << "Paid " << amount << " using UPI\n";
        }
};


class PaymentService {
    private:
        PaymentStrategy* strategy;

    public:
        PaymentService(PaymentStrategy* s) : strategy(s) {};

        void checkout(int amount) {
            strategy -> pay(amount);
        }
};


class SplitStrategy {
    public:
        // returns <userid , share> for every user
        virtual unordered_map<string , int> split(int amount , vector<string>& users) = 0;
        virtual string getName() = 0;
        virtual ~SplitStrategy() {};
};


class EqualSplit : public SplitStrategy {
    public:
        string getName() override {
            return "EQUAL";
        }

        unordered_map<string , int> split(int amount , vector<string>& users) override {
            unordered_map<string , int> shares;
            int n = users.size();
            if(n == 0) return shares;

            for(string u : users) shares[u] = amount / n;
            shares[users[0]] += amount % n;   // leftover paise goes to first user, so total stays == amount

            return shares;
        }
};


class CustomSplit : public SplitStrategy {
    private:
        unordered_map<string , int> custom;   // <userid , amount>

    public:
        CustomSplit(unordered_map<string , int> c) : custom(c) {};

        string getName() override {
            return "CUSTOM";
        }

        unordered_map<string , int> split(int amount , vector<string>& users) override {
            int sum = 0;
            for(string u : users) sum += custom[u];

            if(sum != amount) {
                cout << "Custom split total " << sum << " != expense amount " << amount << "\n";
                return {};
            }

            unordered_map<string , int> shares;
            for(string u : users) shares[u] = custom[u];
            return shares;
        }
};


class ExpenseManagementService {
    private:
        unordered_map<string , vector<string>>grpToExp;                  // <groupid , vector<expid>>
        unordered_map<string , unordered_map<string , int>>expToShares;  // <expid , <userid , share>>
        unordered_map<string , string>expToPayer;                        // <expid , userid>  ;  is user ne exp pay kiya h
        unordered_map<string , int>expToAmount;                          // <expid , amount>
        UserRegistry* registry;

    public:
        ExpenseManagementService(UserRegistry* r) : registry(r) {};

        void addExpenseToGroup(string grpid , string expid) {
            grpToExp[grpid].push_back(expid);
            cout << "[EXPENSE] " << expid << " added to group " << grpid << "\n";
        }

        // users = everyone who shares the expense (payer included if he also consumed)
        void createExpense(string userid , string expid , int amount , vector<string> users , SplitStrategy* strategy) {
            auto shares = strategy -> split(amount , users);
            if(shares.empty()) {
                cout << "[EXPENSE] " << expid << " NOT created, invalid split\n";
                return;
            }

            expToPayer[expid] = userid;
            expToAmount[expid] = amount;
            expToShares[expid] = shares;

            cout << "[EXPENSE] " << expid << " of Rs " << amount << " created by " << registry -> nameOf(userid)
                 << " (paid by " << registry -> nameOf(userid) << ")\n";
            cout << "          split " << strategy -> getName() << " between: ";
            for(int i = 0; i < users.size(); i++) {
                if(i) cout << ", ";
                cout << registry -> nameOf(users[i]) << " = Rs " << shares[users[i]];
            }
            cout << "\n";
        }

        // how much this user's share is in this expense (0 if not part of it)
        int getShare(string expid , string userid) {
            auto it = expToShares.find(expid);
            if(it == expToShares.end()) return 0;

            auto sit = it -> second.find(userid);
            return sit == it -> second.end() ? 0 : sit -> second;
        }

        vector<string> getExpensesOfGroup(string grpid) {
            auto it = grpToExp.find(grpid);
            return it == grpToExp.end() ? vector<string>() : it -> second;
        }

        string getPayer(string expid) {
            return expToPayer[expid];
        }

        int getAmount(string expid) {
            return expToAmount[expid];
        }
};


class SettlementManagementService {
    private:
        ExpenseManagementService* ems;
        GroupManagementService* gms;

    public:
        SettlementManagementService(ExpenseManagementService* e , GroupManagementService* g) 
            : ems(e) , gms(g) {};

        // how much money u1 has to take from u2 , or , how much u2 has to give u1
        // result > 0 : u2 owes u1 ;  result < 0 : u1 owes u2
        int calcMoney(string u1 , string u2 , string grp) {
            int net = 0;

            for(string expid : ems -> getExpensesOfGroup(grp)) {
                string payer = ems -> getPayer(expid);

                if(payer == u1) net += ems -> getShare(expid , u2);       // u1 paid, u2 owes his share
                else if(payer == u2) net -= ems -> getShare(expid , u1);  // u2 paid, u1 owes his share
            }

            return net;
        }

        // debtor pays creditor whatever he owes in this group, using given payment method
        void settleUp(string debtor , string creditor , string grp , PaymentStrategy* method) {
            int owed = calcMoney(creditor , debtor , grp);

            if(owed <= 0) {
                cout << "[SETTLE] " << gms -> nameOf(debtor) << " owes nothing to " << gms -> nameOf(creditor)
                     << " in \"" << gms -> groupNameOf(grp) << "\"\n";
                return;
            }

            cout << "[SETTLE] " << gms -> nameOf(debtor) << " pays " << gms -> nameOf(creditor) << " Rs " << owed << " -> ";
            PaymentService ps(method);
            ps.checkout(owed);

            // record payment as an expense : debtor paid , only creditor "consumed" it , so the balance becomes 0
            string expid = "settle" + to_string(++settleCount);
            CustomSplit onlyCreditor({{creditor , owed}});
            ems -> createExpense(debtor , expid , owed , {creditor} , &onlyCreditor);
            ems -> addExpenseToGroup(grp , expid);
        }

    private:
        int settleCount = 0;
};


void printBalance(SettlementManagementService& sms , UserRegistry& reg , string u1 , string u2 , string grp) {
    int money = sms.calcMoney(u1 , u2 , grp);
    string n1 = reg.nameOf(u1) , n2 = reg.nameOf(u2);

    if(money > 0) cout << "[BALANCE] " << n2 << " owes " << n1 << " Rs " << money << "\n";
    else if(money < 0) cout << "[BALANCE] " << n1 << " owes " << n2 << " Rs " << -money << "\n";
    else cout << "[BALANCE] " << n1 << " and " << n2 << " are settled up\n";
}


int main() {
    User u1("u1" , "Amit");
    User u2("u2" , "Ravi");
    User u3("u3" , "Sam");
    Group g1("g1" , "Goa Trip");

    UserRegistry registry;
    registry.addUser(&u1);
    registry.addUser(&u2);
    registry.addUser(&u3);

    GroupManagementService gms(&registry);
    ExpenseManagementService ems(&registry);
    SettlementManagementService sms(&ems , &gms);

    cout << "--- Group setup ---\n";
    gms.addGroup(&g1 , "u1");
    gms.addUsersInGroup("u1" , "g1");
    gms.addUsersInGroup("u2" , "g1");
    gms.addUsersInGroup("u3" , "g1");

    cout << "\n--- Expenses ---\n";
    // e1 : Amit pays 300 dinner, split equally among all 3 -> 100 each
    EqualSplit equal;
    ems.createExpense("u1" , "e1" , 300 , {"u1" , "u2" , "u3"} , &equal);
    ems.addExpenseToGroup("g1" , "e1");

    // e2 : Ravi pays 300 cab, custom split Amit=50 Ravi=150 Sam=100
    CustomSplit custom({{"u1" , 50} , {"u2" , 150} , {"u3" , 100}});
    ems.createExpense("u2" , "e2" , 300 , {"u1" , "u2" , "u3"} , &custom);
    ems.addExpenseToGroup("g1" , "e2");

    // e3 : bad custom split (total 250 != 300) -> rejected
    CustomSplit bad({{"u1" , 100} , {"u2" , 150}});
    ems.createExpense("u3" , "e3" , 300 , {"u1" , "u2"} , &bad);

    cout << "\n--- Before settlement ---\n";
    printBalance(sms , registry , "u1" , "u2" , "g1");
    printBalance(sms , registry , "u1" , "u3" , "g1");
    printBalance(sms , registry , "u2" , "u3" , "g1");

    cout << "\n--- Settlement ---\n";
    UPIPayment upi;
    CreditCardPayment card;
    sms.settleUp("u2" , "u1" , "g1" , &upi);
    sms.settleUp("u3" , "u1" , "g1" , &card);
    sms.settleUp("u3" , "u1" , "g1" , &card);

    cout << "\n--- After settlement ---\n";
    printBalance(sms , registry , "u1" , "u2" , "g1");
    printBalance(sms , registry , "u1" , "u3" , "g1");
    printBalance(sms , registry , "u2" , "u3" , "g1");

    return 0;
}
