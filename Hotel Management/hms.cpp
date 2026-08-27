#include <bits/stdc++.h>
using namespace std;


enum class RoomType {
    SINGLE,
    DOUBLE,
    DELUXE,
    SUITE
};

enum class RoomStatus {
    AVAILABLE,
    RESERVED,
    OCCUPIED
};

enum class ReservationStatus {
    CONFIRMED,
    CANCELLED,
    COMPLETED
};

string roomTypeToString(RoomType type) {
    switch(type) {
        case RoomType::SINGLE: return "SINGLE";
        case RoomType::DOUBLE: return "DOUBLE";
        case RoomType::DELUXE: return "DELUXE";
        default: return "SUITE";
    }
}


class Guest {
    private:
        string guestId;
        string name;
        int age;

    public:
        Guest(string id, string name, int age) : guestId(id), name(name), age(age) {};

        string getGuestId() {
            return guestId;
        }

        string getName() {
            return name;
        }

        int getAge() {
            return age;
        }
};


class Room {
    private:
        int roomNumber;
        RoomType type;
        RoomStatus status;

    public: 
        Room(int roomNo, RoomType type) : roomNumber(roomNo), type(type), status(RoomStatus::AVAILABLE) {};

        void checkIn() {
            status = RoomStatus::OCCUPIED;
        }

        void checkOut() {
            status = RoomStatus::AVAILABLE;
        }

        int getRoomNumber() {
            return roomNumber;
        }

        RoomType getType() {
            return type;
        }

        RoomStatus getStatus() {
            return status;
        }
};


class Reservation {
    private:
        string reservationId;
        Guest* guest;
        Room* room;
        string checkinDate;
        string checkoutDate;
        ReservationStatus status;

    public:
        Reservation(string id, Guest* guest, Room* room, string checkinDate, string checkoutDate) : 
            reservationId(id), guest(guest), room(room), checkinDate(checkinDate), checkoutDate(checkoutDate), status(ReservationStatus::CONFIRMED) {};

        void cancel() {
            status = ReservationStatus::CANCELLED;
        }

        void completed() {
            status = ReservationStatus::COMPLETED;
        }

        string getReservationId() {
            return reservationId;
        }

        Guest* getGuest() {
            return guest;
        }

        Room* getRoom() {
            return room;
        }

        string getCheckinDate() {
            return checkinDate;
        }

        string getCheckoutDate() {
            return checkoutDate;
        }

        ReservationStatus getStatus() {
            return status;
        }
};


// Days since a fixed epoch, for "YYYY-MM-DD" formatted dates.
int dateToDays(const string& date) {
    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));

    tm t{};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;

    time_t time = mktime(&t);
    return static_cast<int>(time / (60 * 60 * 24));
}

class BillingService {
    public:
        int calculateBill(Reservation* reservation) {
            Room* room = reservation -> getRoom();

            int price = 0;
            if(room -> getType() == RoomType::SINGLE) {
                price = 800;
            } else if(room -> getType() == RoomType::DOUBLE) {
                price = 1000;
            } else if(room -> getType() == RoomType::DELUXE) {
                price = 1500;
            } else {
                price = 2000;
            }

            int totalDays = dateToDays(reservation -> getCheckoutDate()) - dateToDays(reservation -> getCheckinDate());
            if(totalDays <= 0) {
                totalDays = 1;
            }

            int amount = price * totalDays;
            cout << "[Billing] Room " << room -> getRoomNumber() << " (" << roomTypeToString(room -> getType())
                 << ") @ Rs." << price << "/night x " << totalDays << " night(s) = Rs." << amount << endl;

            return amount;
        }
};


class PaymentService {
    public:
        virtual void makePayment(int amount) = 0;
        virtual ~PaymentService() = default;
};


class UPIService : public PaymentService {
    public:
        void makePayment(int amount) {
            cout << "[Payment] Charging Rs." << amount << " via UPI..." << endl;
            cout << "[Payment] Paid Rs." << amount << " via UPI" << endl;
        }
};


class HotelService {
    private:
        vector<Room*>rooms;
        vector<Reservation*>reservations;
        BillingService* billing;
        PaymentService* payment;

    public:
        HotelService(BillingService* billing, PaymentService* payment) : billing(billing), payment(payment) {};

        void addRoom(Room* room) {
            rooms.push_back(room);
            cout << "[Hotel] Added room " << room -> getRoomNumber() << " (" << roomTypeToString(room -> getType()) << ")" << endl;
        }

        vector<Room*> searchAvailableRooms(RoomType type) {

            vector<Room*>availableRooms;

            for(auto room : rooms) {

                if(room -> getStatus() == RoomStatus::AVAILABLE && room -> getType() == type) {
                    availableRooms.push_back(room);
                }
            }

            return availableRooms;
        }

        void createReservation(Reservation* reservation) {
            reservations.push_back(reservation);
            cout << "[Hotel] Created reservation " << reservation -> getReservationId()
                 << " for guest " << reservation -> getGuest() -> getName()
                 << " in room " << reservation -> getRoom() -> getRoomNumber()
                 << " from " << reservation -> getCheckinDate() << " to " << reservation -> getCheckoutDate() << endl;
        }

        void checkIn(Room* room) {
            room -> checkIn();
            cout << "[Hotel] Checked in to room " << room -> getRoomNumber() << endl;
        }

        void checkOut(Reservation* reservation) {
            cout << "[Hotel] Checking out reservation " << reservation -> getReservationId() << endl;

            reservation -> getRoom() -> checkOut();
            reservation -> completed();
            cout << "[Hotel] Room " << reservation -> getRoom() -> getRoomNumber() << " is now available" << endl;

            int amount = billing -> calculateBill(reservation);
            payment -> makePayment(amount);

            cout << "[Hotel] Checkout complete for reservation " << reservation -> getReservationId() << endl;
        }
};


int main() {
    BillingService billing;
    UPIService upi;
    HotelService hotel(&billing, &upi);

    Room room101(101, RoomType::DELUXE);
    hotel.addRoom(&room101);

    Guest guest("G1", "Alice", 30);
    Reservation reservation("R1", &guest, &room101, "2026-08-20", "2026-08-25");
    hotel.createReservation(&reservation);

    hotel.checkIn(&room101);
    hotel.checkOut(&reservation);

    return 0;
}
