#include <bits/stdc++.h>
using namespace std;


// ─── Channel Types ────────────────────────────────────────────────────────────

enum class ChannelType {
    EMAIL,
    SMS,
    PUSH
};


// ─── User ─────────────────────────────────────────────────────────────────────

class User {
private:
    string userId;
    string email;
    string phone;
    string deviceToken;
    vector<ChannelType> preferences;

public:
    User() {}

    User(string userId, string email, string phone, string deviceToken, vector<ChannelType> preferences) {
        this -> userId      = userId;
        this -> email       = email;
        this -> phone       = phone;
        this -> deviceToken = deviceToken;
        this -> preferences = preferences;
    }

    string getUserId() const {
        return userId;
    }

    string getEmail() const {
        return email;
    }

    string getPhone() const {
        return phone;
    }

    string getDeviceToken() const {
        return deviceToken;
    }

    vector<ChannelType> getPreferences() const {
        return preferences;
    }
};


// ─── Notification ─────────────────────────────────────────────────────────────

class Notification {
private:
    string recipientId;
    string message;

public:
    Notification(string recipientId, string message) {
        this -> recipientId = recipientId;
        this -> message     = message;
    }

    string getRecipientId() const {
        return recipientId;
    }

    string getMessage() const {
        return message;
    }
};


// ─── Channel Interface ────────────────────────────────────────────────────────

class INotificationChannel {
public:
    virtual ChannelType getType() const = 0;
    virtual void send(const User& user, const Notification& notification) = 0;
    virtual ~INotificationChannel() {}
};


// ─── Channel Implementations ──────────────────────────────────────────────────

class EmailChannel : public INotificationChannel {
public:
    ChannelType getType() const override {
        return ChannelType::EMAIL;
    }

    void send(const User& user, const Notification& notification) override {
        cout << "[Email -> " << user.getEmail() << "]  " << notification.getMessage() << endl;
    }
};


class SMSChannel : public INotificationChannel {
public:
    ChannelType getType() const override {
        return ChannelType::SMS;
    }

    void send(const User& user, const Notification& notification) override {
        cout << "[SMS -> " << user.getPhone() << "]  " << notification.getMessage() << endl;
    }
};


class PushChannel : public INotificationChannel {
public:
    ChannelType getType() const override {
        return ChannelType::PUSH;
    }

    void send(const User& user, const Notification& notification) override {
        cout << "[Push -> " << user.getDeviceToken() << "]  " << notification.getMessage() << endl;
    }
};


// ─── Channel Factory ──────────────────────────────────────────────────────────

class ChannelFactory {
private:
    unordered_map<ChannelType, INotificationChannel*> channels;

    ChannelFactory() {
        channels[ChannelType::EMAIL] = new EmailChannel();
        channels[ChannelType::SMS]   = new SMSChannel();
        channels[ChannelType::PUSH]  = new PushChannel();
    }

public:
    static ChannelFactory& instance() {
        static ChannelFactory factory;
        return factory;
    }

    INotificationChannel* get(ChannelType type) {
        if (channels.count(type)) {
            return channels[type];
        }
        return nullptr;
    }

    ~ChannelFactory() {
        for (auto& [_, ch] : channels) {
            delete ch;
        }
    }
};


// ─── Notification Service ─────────────────────────────────────────────────────

class NotificationService {
private:
    unordered_map<string, User> users;

public:
    void registerUser(const User& user) {
        users[user.getUserId()] = user;
    }

    void send(const Notification& notification) {
        auto it = users.find(notification.getRecipientId());

        if (it == users.end()) {
            cout << "User not found: " << notification.getRecipientId() << endl;
            return;
        }

        const User& user = it->second;

        for (ChannelType type : user.getPreferences()) {
            INotificationChannel* channel = ChannelFactory::instance().get(type);
            if (channel) {
                channel->send(user, notification);
            }
        }
    }
};


// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    NotificationService service;

    User alice(
        "alice",
        "alice@example.com",
        "+91-9000",
        "tok_alice",
        {ChannelType::EMAIL, ChannelType::SMS}
    );

    User bob(
        "bob",
        "bob@example.com",
        "+91-8000",
        "tok_bob",
        {ChannelType::PUSH}
    );

    service.registerUser(alice);
    service.registerUser(bob);

    service.send(Notification("alice", "Paisa bhar jaldi"));
    service.send(Notification("bob",   "App update ready"));

    return 0;
}
