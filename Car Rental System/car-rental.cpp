#include <bits/stdc++.h>
using namespace std;


// For now, let's support only 1 day booking

enum class MODIFY_TYPE {
    CAR_CHANGE,
    DATE_CHANGE
};


class Car {
    private:
        string model;
        string licenseNo;
        double price;
        unordered_set<string> bookedDates;

    public:
        Car(string model, string license, double price) :
            model(model), licenseNo(license), price(price) {};

        string getModel() {
            return model;
        }

        string getLicenseNo() {
            return licenseNo;
        }

        double getPrice() {
            return price;
        }

        // A date with no entry is available by default (unlike a map<string,bool>,
        // whose operator[] would silently insert "false" for unseen dates).
        bool isAvailable(string date) {
            return bookedDates.find(date) == bookedDates.end();
        }

        bool reserveCar(string date) {
            if (!isAvailable(date)) return false;

            bookedDates.insert(date);

            cout << "Making the payment..." << endl;
            cout << "Your reservation of the car " << model << " " << licenseNo
                 << " for " << date << " is confirmed." << endl;
            return true;
        }

        void releaseCar(string date) {
            bookedDates.erase(date);
        }
};


class Reservation {
    private:
        static int nextId;

        int id;
        string customerName;
        Car* car;
        string date;
        bool active;

    public:
        Reservation(string customerName, Car* car, string date) :
            id(nextId++), customerName(customerName), car(car), date(date), active(true) {};

        int getId() {
            return id;
        }

        Car* getCar() {
            return car;
        }

        string getDate() {
            return date;
        }

        bool isActive() {
            return active;
        }

        void setCar(Car* newCar) {
            car = newCar;
        }

        void setDate(string newDate) {
            date = newDate;
        }

        void cancel() {
            active = false;
        }

        void print() {
            cout << "Reservation #" << id << " -> " << car->getModel() << " ("
                 << car->getLicenseNo() << ") on " << date
                 << (active ? "" : " [cancelled]") << endl;
        }
};

int Reservation::nextId = 1;


// Holds the fleet of cars available for rent and the search/filter logic
// that works over that fleet.
class CarRentalSystem {
    private:
        vector<Car*> cars;

    public:
        void addCar(Car* c) {
            cars.push_back(c);
        }

        vector<Car*> searchByModelName(string modelQuery) {
            string query = modelQuery;
            transform(query.begin(), query.end(), query.begin(), ::tolower);

            vector<Car*> result;
            for (auto c : cars) {
                string model = c->getModel();
                transform(model.begin(), model.end(), model.begin(), ::tolower);

                if (model.find(query) != string::npos) {
                    result.push_back(c);
                }
            }
            return result;
        }

        vector<Car*> filterByPrice(vector<Car*> in, double minPrice, double maxPrice) {
            vector<Car*> result;
            for (auto c : in) {
                if (c->getPrice() >= minPrice && c->getPrice() <= maxPrice) {
                    result.push_back(c);
                }
            }
            return result;
        }

        vector<Car*> filterByAvailability(vector<Car*> in, string date) {
            vector<Car*> result;
            for (auto c : in) {
                if (c->isAvailable(date)) {
                    result.push_back(c);
                }
            }
            return result;
        }
};


class Customer {
    private:
        string name;
        string phone;
        string DLNo;
        CarRentalSystem* system;
        vector<Reservation*> reservations;

    public:
        Customer(string name, string phone, string DLNo, CarRentalSystem* system) :
            name(name), phone(phone), DLNo(DLNo), system(system) {};

        vector<Car*> search(string modelQuery, double minPrice, double maxPrice, string date) {
            vector<Car*> result = system->searchByModelName(modelQuery);
            result = system->filterByPrice(result, minPrice, maxPrice);
            result = system->filterByAvailability(result, date);
            return result;
        }

        Reservation* selectCar(Car* c, string date) {
            if (!c->reserveCar(date)) {
                cout << "Sorry, " << c->getModel() << " is not available on " << date << endl;
                return nullptr;
            }

            Reservation* r = new Reservation(name, c, date);
            reservations.push_back(r);
            return r;
        }

        bool updateReservation(Reservation* r, MODIFY_TYPE type, Car* newCar, string newDate) {
            if (!r || !r->isActive()) {
                cout << "Invalid or inactive reservation." << endl;
                return false;
            }

            if (type == MODIFY_TYPE::CAR_CHANGE) {
                if (!newCar->reserveCar(r->getDate())) {
                    cout << "Sorry, " << newCar->getModel() << " is not available on " << r->getDate() << endl;
                    return false;
                }
                r->getCar()->releaseCar(r->getDate());
                r->setCar(newCar);
            } else { // DATE_CHANGE
                if (!r->getCar()->reserveCar(newDate)) {
                    cout << "Sorry, that car is not available on " << newDate << endl;
                    return false;
                }
                r->getCar()->releaseCar(r->getDate());
                r->setDate(newDate);
            }

            cout << "Reservation #" << r->getId() << " updated." << endl;
            return true;
        }

        void cancelReservation(Reservation* r) {
            if (!r || !r->isActive()) return;
            r->getCar()->releaseCar(r->getDate());
            r->cancel();
        }

        void printReservations() {
            for (auto r : reservations) r->print();
        }
};


int main() {
    CarRentalSystem system;

    Car* swift = new Car("Maruti Swift", "KA-01-1234", 1500);
    Car* creta = new Car("Hyundai Creta", "KA-01-5678", 3000);
    Car* innova = new Car("Toyota Innova", "KA-01-9999", 4000);

    system.addCar(swift);
    system.addCar(creta);
    system.addCar(innova);

    Customer alice("Alice", "9999999999", "DL123", &system);

    // Search + filter
    vector<Car*> options = alice.search("swift", 1000, 3500, "2026-08-10");
    cout << "Search results:" << endl;
    for (auto c : options) {
        cout << " - " << c->getModel() << " (" << c->getPrice() << ")" << endl;
    }

    // Reserve
    Reservation* booking = alice.selectCar(swift, "2026-08-10");

    // Car should now be unavailable on that date...
    cout << "Swift available on 2026-08-10? " << (swift->isAvailable("2026-08-10") ? "yes" : "no") << endl;

    // ...but free again on any other date
    cout << "Swift available on 2026-08-11? " << (swift->isAvailable("2026-08-11") ? "yes" : "no") << endl;

    // Update reservation: change the date
    alice.updateReservation(booking, MODIFY_TYPE::DATE_CHANGE, nullptr, "2026-08-12");
    cout << "Swift available on 2026-08-10 after date change? " << (swift->isAvailable("2026-08-10") ? "yes" : "no") << endl;

    // Update reservation: change the car
    alice.updateReservation(booking, MODIFY_TYPE::CAR_CHANGE, creta, "");

    alice.printReservations();

    // Cancel and confirm the car frees up again
    alice.cancelReservation(booking);
    cout << "Creta available on 2026-08-12 after cancel? " << (creta->isAvailable("2026-08-12") ? "yes" : "no") << endl;

    return 0;
}
