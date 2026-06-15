#include <bits/stdc++.h>
using namespace std;

class ISubscriber {
    public:
        virtual void update() = 0;
        virtual ~ISubscriber() {};
};


class IChannel {
    public:
        virtual void subscribe(ISubscriber* subscriber) = 0;
        virtual void unsubscribe(ISubscriber* subscriber) = 0;
        virtual void notify() = 0;
        virtual ~IChannel() {};
};


class Channel : public IChannel {
    private:
        vector<ISubscriber*>subscribers;
        string name;
        string latestVideo;

    public:
        Channel(string name) {
            this -> name = name;
        }

        void subscribe(ISubscriber* subscriber) override {
            if (find(subscribers.begin(), subscribers.end(), subscriber) == subscribers.end()) {
                subscribers.push_back(subscriber);
            }
        }

        void unsubscribe(ISubscriber* subscriber) override {
            auto it = find(subscribers.begin(), subscribers.end(), subscriber);
            if (it != subscribers.end()) {
                subscribers.erase(it);
            }
        }

        void notify() override {
            for(auto sub : subscribers) {
                sub -> update();
            }
        }

        void uploadVideo(const string& title) {
            latestVideo = title;
            cout << "\n[" << name << " uploaded \"" << title << "\"]\n";
            notify();
        }

        // Read video data
        string getVideoData() {
            return "\nCheckout our new Video : " + latestVideo + "\n";
        }
};


class Subscriber : public ISubscriber {
    private:
        Channel* channel;
        string name;

    public:
        Subscriber(string name , Channel* channel) {
            this -> name = name;
            this -> channel = channel;
        }

        // Called by Channel; prints notification message
        void update() override {
            cout << "Hey " << name << "," << channel -> getVideoData();
        }
};


int main() {
    // Create a channel and subscribers
    Channel* channel = new Channel("CoderArmy");

    Subscriber* subs1 = new Subscriber("Varun", channel);
    Subscriber* subs2 = new Subscriber("Tarun", channel);

    // Varun and Tarun subscribe to CoderArmy
    channel->subscribe(subs1);
    channel->subscribe(subs2);

    // Upload a video: both Varun and Tarun are notified
    channel->uploadVideo("Observer Pattern Tutorial");

    // Varun unsubscribes; Tarun remains subscribed
    channel->unsubscribe(subs1);

    // Upload another video: only Tarun is notified
    channel->uploadVideo("Decorator Pattern Tutorial");

    return 0;
}