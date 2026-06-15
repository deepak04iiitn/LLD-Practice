#include <bits/stdc++.h>
using namespace std;

class Item {
private:
    string name;
    int price;

public:
    Item(string name, int price) {
        this -> name = name;
        this -> price = price;
    }

    int getPrice() {
        return price;
    }

    string getName() {
        return name;
    }
};


class Checkout {

public:
    Checkout() {}

    void pay(int price) {
        cout << "\nPayment of Rs " << price << " successful!\n";
    }
};


class Cart {
private:
    vector<Item*> items;
    Checkout* payment;

public:
    Cart() {
        payment = new Checkout();
    }

    void add(Item* item) {
        items.push_back(item);

        cout << item -> getName() << " added to cart successfully!\n";
    }

    int totalPrice() {

        int totalprice = 0;

        for(auto it : items) {
            totalprice += it -> getPrice();
        }

        return totalprice;
    }

    void showCart() {

        cout << "\nItems in Cart:\n";

        for(auto it : items) {
            cout << "- " << it -> getName() << " : Rs " << it -> getPrice() << endl;
        }

        cout << "Total Price = Rs " << totalPrice() << endl;
    }

    void buyNow() {
        payment -> pay(totalPrice());
    }
};


class User {

private:
    string name;
    string address;
    string contactnum;
    Cart* cart;

public:
    User(string name, string address, string contactnum) {

        this->name = name;
        this->address = address;
        this->contactnum = contactnum;

        cart = new Cart();
    }

    void addToCart(Item* item) {
        cart -> add(item);
    }

    void checkout() {
        cart -> showCart();
        cart -> buyNow();
    }

    string getContactnum() {
        return contactnum;
    }
};


class Restaurant {

private:
    vector<Item*> menu;
    string name;
    string address;

public:
    Restaurant(string name, string address) {

        this->name = name;
        this->address = address;
    }

    void addItems(Item* item) {

        menu.push_back(item);

        cout << item->getName() << " added to menu successfully!!\n";
    }
};


class NotificationSystem {

private:
    User* u;

public:
    NotificationSystem(User* u) {
        this -> u = u;
    }

    void send() {

        cout << "\nNotification sent to " << u -> getContactnum() << " successfully!\n";
    }
};


int main() {

    User* user = new User("Deepak Yadav", "Jaipur", "1020304050");

    Item* item1 = new Item("Chicken Biryani", 250);
    Item* item2 = new Item("Noodles", 200);
    Item* item3 = new Item("Pizza", 350);
    Item* item4 = new Item("Burger", 100);
    Item* item5 = new Item("Maggie", 80);

    Restaurant* res1 = new Restaurant("Chinese Corner", "Jodhpur");

    Restaurant* res2 = new Restaurant("Baba Dhaba", "Basti");

    res1 -> addItems(item1);
    res1 -> addItems(item2);
    res1 -> addItems(item3);

    res2 -> addItems(item4);
    res2 -> addItems(item5);

    user -> addToCart(item1);
    user -> addToCart(item5);
    user -> addToCart(item2);

    user -> checkout();

    NotificationSystem* notify = new NotificationSystem(user);

    notify -> send();

    return 0;
}