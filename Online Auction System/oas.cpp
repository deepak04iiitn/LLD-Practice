#include <bits/stdc++.h>
using namespace std;


enum class AuctionStatus {
    ACTIVE,
    ENDED
};


enum class Category {
    ELECTRONICS,
    CLOTHING,
    FURNITURE,
    BOOKS,
    OTHER
};


class AuctionObserver {
    public:
        virtual void update(string message) = 0;
        virtual ~AuctionObserver() = default;
};


class User : public AuctionObserver {
    private:
        int userId;
        string name;
        string email_;
        string password;

    public:
        User() = default;

        User(int userId, string name, string email, string password) :
            userId(userId), name(name), email_(email), password(password) {};

        int getUserId() {
            return userId;
        }

        string getName() {
            return name;
        }

        string getEmail() {
            return email_;
        }

        string getPassword() {
            return password;
        }

        void update(string message) override {
            cout << "Notification for " << name << ": " << message << endl;
        }
};


class AuthenticationService {
    private:
        unordered_map<string, User>users;
        int nextUserId = 1;

    public:
        bool registerUser(string name, string email, string password) {

            if(users.find(email) != users.end()) {
                return false;
            }

            User user(
                nextUserId++,
                name,
                email,
                password
            );

            users[email] = user;

            return true;
        }


        User* loginUser(string email, string password) {

            if(users.find(email) == users.end()) {
                return nullptr;
            }

            User &user = users[email];

            if(user.getPassword() != password) {
                return nullptr;
            }

            return &user;
        }
};


class Bid {
    private:
        User* bidder;
        int amount;
        string timestamp;

    public:
        Bid(User* bidder, int amount, string timestamp) :
            bidder(bidder), amount(amount), timestamp(timestamp) {};

        User* getBidder() {
            return bidder;
        }

        int getAmount() {
            return amount;
        }

        string getTimestamp() {
            return timestamp;
        }
};


class AuctionListing {
    private:
        string itemName;
        string description;
        int startingPrice;
        int duration; // seconds
        chrono::system_clock::time_point startTime;
        chrono::system_clock::time_point endTime;
        AuctionStatus status;
        Category category;
        vector<Bid*>bids;
        vector<AuctionObserver*>observers;

        void notifyObservers(string message) {
            for(auto observer : observers) {
                observer -> update(message);
            }
        }

    public:
        AuctionListing(string itemName, string description, int startingPrice, int duration) :
            itemName(itemName), description(description), startingPrice(startingPrice), duration(duration), status(AuctionStatus::ACTIVE) {
            startTime = chrono::system_clock::now();
            endTime = startTime + chrono::seconds(duration);
        };

        void setCategory(Category c) {
            category = c;
        }

        void addObserver(AuctionObserver* observer) {
            if(find(observers.begin(), observers.end(), observer) == observers.end()) {
                observers.push_back(observer);
            }
        }

        void addBids(Bid* b) {
            bids.push_back(b);

            Bid* highestBid = getHighestBid();
            notifyObservers(
                "A new bid of " + to_string(b -> getAmount()) + " has been placed on " + itemName +
                ". Current highest bid: " + to_string(highestBid -> getAmount())
            );
        }

        Bid* getHighestBid() {
            int maxAmount = INT_MIN;
            Bid* highestBid = nullptr;

            for(auto bid : bids) {
                if(bid -> getAmount() > maxAmount) {
                    maxAmount = bid -> getAmount();
                    highestBid = bid;
                }
            }

            return highestBid;
        }

        User* getWinner() {
            Bid* highestBid = getHighestBid();

            if(highestBid == nullptr) return nullptr;

            return highestBid -> getBidder();
        }

        string getItemName() {
            return itemName;
        }

        string getDescription() {
            return description;
        }

        int getStartingPrice() {
            return startingPrice;
        }

        int getDuration() {
            return duration;
        }

        // Lazily transitions ACTIVE -> ENDED once endTime has passed, notifying
        // observers exactly once on the transition (no background thread needed).
        AuctionStatus getStatus() {
            if(status == AuctionStatus::ACTIVE && chrono::system_clock::now() >= endTime) {
                status = AuctionStatus::ENDED;

                Bid* highestBid = getHighestBid();
                string winnerName = highestBid ? highestBid -> getBidder() -> getName() : "no one";
                notifyObservers("Auction for " + itemName + " has ended. Winner: " + winnerName);
            }
            return status;
        }

        Category getCategory() {
            return category;
        }
};


class SearchCriteria {
public:
    string itemName;
    Category category;
    int minPrice;
    int maxPrice;
    AuctionStatus status;
};


class AuctionService {
    private:
        vector<AuctionListing*>listings;

    public:
        AuctionService() {};

        void createListing(AuctionListing* listing) {
            listings.push_back(listing);
        }

        vector<AuctionListing*> searchListings(SearchCriteria criteria) {
            vector<AuctionListing*> result;

            for(auto listing : listings) {

                if(!criteria.itemName.empty() && listing -> getItemName() != criteria.itemName) {
                    continue;
                }

                if(listing -> getCategory() != criteria.category) {
                    continue;
                }

                if(listing -> getStartingPrice() < criteria.minPrice || listing -> getStartingPrice() > criteria.maxPrice) {
                    continue;
                }

                if(listing -> getStatus() != criteria.status) {
                    continue;
                }

                result.push_back(listing);
            }

            return result;
        }

        void placeBid(Bid* bid, AuctionListing* listing) {

            if(listing -> getStatus() != AuctionStatus::ACTIVE) {
                return;
            }

            listing -> addObserver(bid -> getBidder());
            listing -> addBids(bid);
        }
};


int main() {
    AuthenticationService authService;

    authService.registerUser("Alice", "alice@mail.com", "pw123");
    authService.registerUser("Bob", "bob@mail.com", "pw456");

    User* alice = authService.loginUser("alice@mail.com", "pw123");
    User* bob = authService.loginUser("bob@mail.com", "pw456");

    if(!alice || !bob) {
        cout << "Login failed." << endl;
        return 1;
    }

    // wrong password should be rejected
    User* badLogin = authService.loginUser("bob@mail.com", "wrong-password");
    cout << "Bad login result: " << (badLogin == nullptr ? "rejected" : "accepted") << endl;

    AuctionService auctionService;

    AuctionListing listing("Vintage Watch", "A fine old watch", 100, 2 /* seconds */);
    listing.setCategory(Category::OTHER);
    auctionService.createListing(&listing);

    Bid bid1(alice, 120, "t1");
    Bid bid2(bob, 150, "t2");

    auctionService.placeBid(&bid1, &listing);
    auctionService.placeBid(&bid2, &listing);

    // search while the auction is still active
    SearchCriteria activeSearch;
    activeSearch.itemName = "";
    activeSearch.category = Category::OTHER;
    activeSearch.minPrice = 0;
    activeSearch.maxPrice = 1000;
    activeSearch.status = AuctionStatus::ACTIVE;

    auto activeResults = auctionService.searchListings(activeSearch);
    cout << "Active listings found: " << activeResults.size() << endl;
    for(auto l : activeResults) {
        cout << " - " << l -> getItemName() << " (starting price " << l -> getStartingPrice() << ")" << endl;
    }

    cout << "Waiting for auction to end...\n";
    this_thread::sleep_for(chrono::seconds(3));

    // searching by ACTIVE again after expiry should now find nothing,
    // since getStatus() lazily flips the listing to ENDED as a side effect of the search
    auto stillActive = auctionService.searchListings(activeSearch);
    cout << "Active listings after expiry: " << stillActive.size() << endl;

    SearchCriteria endedSearch = activeSearch;
    endedSearch.status = AuctionStatus::ENDED;
    auto endedResults = auctionService.searchListings(endedSearch);
    cout << "Ended listings found: " << endedResults.size() << endl;
    for(auto l : endedResults) {
        User* winner = l -> getWinner();
        cout << " - " << l -> getItemName() << " won by "
             << (winner ? winner -> getName() : "no one") << endl;
    }

    return 0;
}
