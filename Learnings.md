# LLD Learnings

Design principles picked up while working through the practice problems in this repo, each backed by a real code example from one of the projects.

---

## 1. Separate out Authentication into its own service

Whenever a problem says "user can register and login", don't give `User` those responsibilities. `User` should stay a plain data holder. Create a dedicated `AuthenticationService` that owns the user store and handles register/login.

**Why:** Keeps `User` focused on representing a user, not on credential checks/storage. Auth logic (password hashing, tokens, lockouts, etc.) tends to grow independently of the user's core attributes, so it deserves its own class from day one.

**Example:** [`Online Auction System/oas.cpp:62-99`](Online%20Auction%20System/oas.cpp)

```cpp
class AuthenticationService {
    private:
        unordered_map<string, User> users;
        int nextUserId = 1;

    public:
        bool registerUser(string name, string email, string password) {
            if (users.find(email) != users.end()) {
                return false;
            }
            User user(nextUserId++, name, email, password);
            users[email] = user;
            return true;
        }

        User* loginUser(string email, string password) {
            if (users.find(email) == users.end()) {
                return nullptr;
            }
            User &user = users[email];
            if (user.getPassword() != password) {
                return nullptr;
            }
            return &user;
        }
};
```

Usage in `main()`:

```cpp
AuthenticationService authService;
authService.registerUser("Alice", "alice@mail.com", "pw123");
User* alice = authService.loginUser("alice@mail.com", "pw123");
```

`Digital Wallet Service/dws.cpp` has a similar `AuthService` class too, but it only implements `registerUser` (no login) — the auction system's version is the fuller example.

---

## 2. Bundle multi-criteria search into one `SearchCriteria` class

Whenever a user can search/browse by multiple criteria (name, price, date, category, status, ...), don't create a separate method per criterion and chain them together. Create one `SearchCriteria` (or `SearchFilter`) class that bundles the optional fields, and expose a single `search(criteria)` method that applies all of them together.

**Why:** A method-per-criterion design forces callers to know and chain the right sequence of filters, and adding a new filterable field means adding yet another method. A criteria object scales by just adding a field, and the filtering logic lives in one place.

**Good example:** [`Online Auction System/oas.cpp:231-238, 252-277`](Online%20Auction%20System/oas.cpp)

```cpp
class SearchCriteria {
public:
    string itemName;
    Category category;
    int minPrice;
    int maxPrice;
    AuctionStatus status;
};

class AuctionService {
    ...
    vector<AuctionListing*> searchListings(SearchCriteria criteria) {
        vector<AuctionListing*> result;

        for (auto listing : listings) {
            if (!criteria.itemName.empty() && listing->getItemName() != criteria.itemName) continue;
            if (listing->getCategory() != criteria.category) continue;
            if (listing->getStartingPrice() < criteria.minPrice || listing->getStartingPrice() > criteria.maxPrice) continue;
            if (listing->getStatus() != criteria.status) continue;

            result.push_back(listing);
        }

        return result;
    }
};
```

Usage — one call, all filters applied at once:

```cpp
SearchCriteria activeSearch;
activeSearch.itemName = "";
activeSearch.category = Category::OTHER;
activeSearch.minPrice = 0;
activeSearch.maxPrice = 1000;
activeSearch.status = AuctionStatus::ACTIVE;

auto activeResults = auctionService.searchListings(activeSearch);
```

**Anti-pattern for contrast:** [`Car Rental System/car-rental.cpp:122-176`](Car%20Rental%20System/car-rental.cpp) does the opposite — three separate methods that the caller has to chain manually:

```cpp
vector<Car*> searchByModelName(string modelQuery) { ... }
vector<Car*> filterByPrice(vector<Car*> in, double minPrice, double maxPrice) { ... }
vector<Car*> filterByAvailability(vector<Car*> in, string date) { ... }

// caller has to know to chain these in the right order:
vector<Car*> result = system->searchByModelName(modelQuery);
result = system->filterByPrice(result, minPrice, maxPrice);
result = system->filterByAvailability(result, date);
```

This works, but every new filter means a new method *and* a new line at every call site — exactly what a `SearchCriteria` object avoids.

---

## 3. Single Responsibility Principle: main service as a facade over action-specific services

Don't put every action (booking, billing, payment, notifications, ...) as a method inside one big service class. Split each concern into its own single-responsibility service class, and let the main/top-level service act as a **facade** — it holds references to those smaller services (via constructor injection) and delegates each request to the right one.

**Why:** Keeps each service focused and independently testable (billing logic doesn't need to know about payments), and the facade class stays thin — it only orchestrates the sequence of calls, it doesn't implement business logic itself.

**Example:** [`Hotel Management/hms.cpp:196-249`](Hotel%20Management/hms.cpp)

```cpp
class HotelService {
    private:
        vector<Room*> rooms;
        vector<Reservation*> reservations;
        BillingService* billing;   // injected, not owned
        PaymentService* payment;   // injected, abstract base (e.g. UPIService impl)

    public:
        HotelService(BillingService* billing, PaymentService* payment)
            : billing(billing), payment(payment) {};

        void addRoom(Room* room) { ... }

        vector<Room*> searchAvailableRooms(RoomType type) { ... }

        void createReservation(Reservation* reservation) { ... }

        void checkOut(Reservation* reservation) {
            reservation->getRoom()->checkOut();
            reservation->completed();

            int amount = billing->calculateBill(reservation);   // delegates to BillingService
            payment->makePayment(amount);                       // delegates to PaymentService

            cout << "[Hotel] Checkout complete for reservation "
                 << reservation->getReservationId() << endl;
        }
};
```

Wiring it together in `main()`:

```cpp
BillingService billing;
UPIService upi;
HotelService hotel(&billing, &upi);
```

`HotelService` never computes a bill or talks to a payment gateway itself — it just knows *when* to call `billing` and `payment`, which is exactly what a facade should do.

---

## 4. Modeling different user types: inheritance vs. role enum

Whenever a requirement says "the system should support different types of users" (e.g. passengers, airline staff, administrators), don't reach for inheritance automatically. First ask *how differently do they actually behave?*

**Why:** Passengers, airline staff, and admins share the same core identity fields (id, name, email, phone), but the split between them is either **behavioral** (different actions each type can perform) or purely **authorization-based** (same actions, just gated by role). Those two cases call for two different designs, and picking the wrong one leads to either an anemic inheritance hierarchy or a bloated `User` god-class.

> No existing project in this repo implements an airline system's user model yet, so the snippets below are a from-scratch skeleton (not pulled from a file in this repo) — the pattern to reach for the next time this requirement shows up.

### Option A — Inheritance, when behavior genuinely differs per role

```cpp
enum class UserRole {
    PASSENGER,
    AIRLINE_STAFF,
    ADMIN
};

class User {
protected:
    int userId;
    string name;
    string email;
    string phone;
    UserRole role;

public:
    User(int id, string name, string email, string phone, UserRole role)
        : userId(id), name(name), email(email), phone(phone), role(role) {}

    virtual ~User() = default;

    int getUserId() const { return userId; }
    UserRole getRole() const { return role; }
};

class Passenger : public User {
public:
    Passenger(int id, string name, string email, string phone)
        : User(id, name, email, phone, UserRole::PASSENGER) {}

    void searchFlights() { /* ... */ }
    void bookFlight() { /* ... */ }
    void cancelBooking() { /* ... */ }
};

class AirlineStaff : public User {
public:
    AirlineStaff(int id, string name, string email, string phone)
        : User(id, name, email, phone, UserRole::AIRLINE_STAFF) {}

    void manageFlights() { /* ... */ }
    void updateFlightStatus() { /* ... */ }
};

class Administrator : public User {
public:
    Administrator(int id, string name, string email, string phone)
        : User(id, name, email, phone, UserRole::ADMIN) {}

    void addAirline() { /* ... */ }
    void removeAirline() { /* ... */ }
    void manageUsers() { /* ... */ }
};
```

Class hierarchy:

```
                    User
                     |
          -----------------------
          |          |          |
      Passenger   Staff      Admin
```

| User type | Responsibilities |
|---|---|
| Passenger | Search/book/cancel flights |
| AirlineStaff | Manage flights, update flight status |
| Administrator | Manage airlines/users/system |

This avoids the god-class version, which violates SRP by cramming every role's methods into one class:

```cpp
class User {
    // bookFlight()
    // cancelFlight()
    // manageFlights()
    // addAirline()
    // removeUser()
    // ... irrelevant methods for whichever role you're actually holding
};
```

### Option B — Plain role enum, when roles only differ in permissions

If passengers/staff/admins would otherwise call the *same* operations and the only real difference is "who's allowed to do what," inheritance just adds ceremony. Keep one `User` class and gate access in the service layer instead:

```cpp
enum class Role {
    PASSENGER,
    AIRLINE_STAFF,
    ADMIN
};

class User {
private:
    int userId;
    string name;
    string email;
    Role role;
};

class BookingService {
public:
    void bookFlight(User* user, Flight* flight) {
        if (user->getRole() != Role::PASSENGER) {
            throw runtime_error("Only passengers can book flights");
        }
        // booking logic
    }
};
```

```
User
  |
  +-- Role
       ├── PASSENGER
       ├── AIRLINE_STAFF
       └── ADMIN
```

### Which one to pick

For an airline reservation system specifically, prefer **Option A (inheritance)** — the roles have substantially different behavior (a passenger books, staff manages flights, an admin manages the system). Reach for **Option B (role enum)** when the roles mainly differ in authorization over otherwise-shared operations.

**The rule of thumb:** model different *behavior* with polymorphism/inheritance; model different *permissions* with roles. If an interviewer asks "why didn't you just use an enum?", this distinction — behavior vs. permissions — is the answer.

---

## 5. Model a "reservation/booking" as its own class connecting the two parties

Whenever a problem mentions reservation or booking (a guest reserving a room, a passenger booking a flight, a renter reserving a car, ...), don't try to track the reservation as a flag or field on one of the two sides. Create a dedicated `Reservation`/`Booking` class that sits *between* the two parties involved and holds the relationship, its dates, and its own status.

**Why:** A reservation isn't really a property of the guest, and it isn't really a property of the room either — it's a fact about the two of them *together*, for a specific time window, that itself has a lifecycle (confirmed → cancelled/completed). Tacking `bool isReserved` or `Guest* currentGuest` onto `Room` throws that lifecycle and history away — you can't answer "who reserved this room last week?" or hold multiple reservations for the same room across different dates. A first-class `Reservation` class also gives billing/payment/cancellation something concrete to operate on, instead of poking at both `Guest` and `Room` directly.

**Example:** [`Hotel Management/hms.cpp:88-132`](Hotel%20Management/hms.cpp)

```cpp
enum class ReservationStatus {
    CONFIRMED,
    CANCELLED,
    COMPLETED
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

        Guest* getGuest() { return guest; }
        Room* getRoom() { return room; }
        string getCheckinDate() { return checkinDate; }
        string getCheckoutDate() { return checkoutDate; }
        ReservationStatus getStatus() { return status; }
};
```

Notice what `Reservation` holds that neither `Guest` nor `Room` could hold on its own: a pointer to *both* sides, the date range that's specific to this particular stay, and a status that tracks this booking's own lifecycle — independent of whether the room later gets reserved again by someone else.

`HotelService` (see [Learning 3](#3-single-responsibility-principle-main-service-as-a-facade-over-action-specific-services)) then operates on `Reservation` objects rather than mutating `Guest`/`Room` directly:

```cpp
void createReservation(Reservation* reservation) {
    reservations.push_back(reservation);
    ...
}

void checkOut(Reservation* reservation) {
    reservation->getRoom()->checkOut();
    reservation->completed();

    int amount = billing->calculateBill(reservation);   // billing reads dates/room off the reservation
    payment->makePayment(amount);
}
```

**The rule of thumb:** the moment a problem statement uses the word "reservation" or "booking", that word is telling you there's a class hiding in the requirement — don't skip straight to modeling only the two entities it connects.

---

## 6. Don't sprinkle `cout` inside every class/method — print from `main()` instead

Don't worry about instrumenting every service method or entity class with `cout` statements to show what's happening. Keep the classes doing pure logic — returning values or objects, or a plain `bool`/`nullptr` for success/failure — and do the printing/demo narration in `main()`, using the values those methods hand back.

**Why:** A class method that both computes something *and* prints about it is doing two jobs at once, and it means every caller is stuck with that class's chosen wording/format regardless of what the caller actually needs. Since these projects are demo-driven (`main()` is the "script" that walks through a scenario), `main()` is the natural place to narrate — it already knows the story being told, while the service classes shouldn't need to know or care that they're being demoed at all.

**Example:** [`Digital Wallet Service/dws.cpp`](Digital%20Wallet%20Service/dws.cpp)

`AuthService::registerUser` and `WalletService::createWallet` just do their job and return — no `cout` in sight:

```cpp
// AuthService — line 68
bool registerUser(string name, string email, string password, string address, string contact) {
    if (users.find(email) != users.end()) {
        return false;
    }
    User user(nextUserId++, name, email, password, address, contact);
    users[email] = user;
    return true;
}

// WalletService — line 312
Wallet* createWallet(int userId) {
    Wallet* wallet = new Wallet(nextWalletId++, userId);
    wallets[wallet->getWalletId()] = wallet;
    return wallet;
}
```

`main()` is the one narrating the scenario, using what came back from those calls:

```cpp
AuthService authService;
authService.registerUser("Deepak", "deepak@gmail.com", "password123", "Dehradun", "9876543210");

CurrencyService currencyService;
WalletService walletService(&currencyService);

Wallet* deepakWallet = walletService.createWallet(1);

cout << "Deepak Wallet ID: " << deepakWallet->getWalletId() << "\n";
```

**The rule of thumb:** if you're tempted to add a `cout` inside a service/entity method just to "show" that it worked, stop — return the result and print it from `main()` instead. Keeps the class reusable regardless of who's calling it, and keeps all the demo narration in one predictable place.

---

## 7.

