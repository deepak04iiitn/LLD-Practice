#include <bits/stdc++.h>
using namespace std;

// Publisher, Subscriber, Topic

class Message {
    private:
        string message;

    public:
        Message(string mess) : message(mess) {};

        string getMessage() {
            return message;
        }
};


class Subscriber {
    private:
        string name;

    public:
        Subscriber(string name) : name(name) {};

        void onMessage(string topicName, Message* message) {
            cout << "[" << name << "] received on \"" << topicName << "\": "
                 << message -> getMessage() << endl;
        }

        string getName() {
            return name;
        }
};


class Topic {
    private:
        vector<Message*> messages;
        vector<Subscriber*> subscribers;
        string name;
        string id;

    public:
        Topic(string name, string id) : name(name), id(id) {};

        void publishMessage(Message* message) {
            messages.push_back(message);
            notify(message);
        }

        void addSubscriber(Subscriber* sub) {
            subscribers.push_back(sub);
        }

        void removeSubscriber(Subscriber* sub) {
            subscribers.erase(
                remove(subscribers.begin(), subscribers.end(), sub),
                subscribers.end()
            );
        }

        void notify(Message* message) {
            for(Subscriber* sub : subscribers) {
                sub -> onMessage(name, message);
            }
        }

        string getTopicName() {
            return name;
        }

        string getTopicId() {
            return id;
        }
};

class Publisher {
    private:
        string name;

    public:
        Publisher(string name) : name(name) {};

        void publish(Topic* topic, string content) {
            Message* message = new Message(content);
            cout << "[" << name << "] publishing to \"" << topic -> getTopicName() << "\": "
                 << content << endl;
            topic -> publishMessage(message);
        }

        string getName() {
            return name;
        }
};

class PubSubSystem {
    private:
        unordered_map<string, Topic*> topics;

    public:
        Topic* createTopic(string name, string id) {
            if(topics.count(id)) {
                return topics[id];
            }

            Topic* topic = new Topic(name, id);
            topics[id] = topic;
            return topic;
        }

        Topic* getTopic(string id) {
            if(!topics.count(id)) {
                return nullptr;
            }

            return topics[id];
        }

        void subscribe(string topicId, Subscriber* sub) {
            Topic* topic = getTopic(topicId);
            if(topic) {
                topic -> addSubscriber(sub);
            }
        }

        void unsubscribe(string topicId, Subscriber* sub) {
            Topic* topic = getTopic(topicId);
            if(topic) {
                topic -> removeSubscriber(sub);
            }
        }
};

int main() {
    PubSubSystem pubSub;

    Topic* sportsTopic = pubSub.createTopic("Sports", "t1");
    Topic* techTopic = pubSub.createTopic("Tech", "t2");

    Subscriber* alice = new Subscriber("Alice");
    Subscriber* bob = new Subscriber("Bob");
    Subscriber* charlie = new Subscriber("Charlie");

    pubSub.subscribe("t1", alice);
    pubSub.subscribe("t1", bob);
    pubSub.subscribe("t2", bob);
    pubSub.subscribe("t2", charlie);

    Publisher* espn = new Publisher("ESPN");
    Publisher* techCrunch = new Publisher("TechCrunch");

    espn -> publish(sportsTopic, "Team A wins the championship!");
    techCrunch -> publish(techTopic, "New AI model released today.");

    pubSub.unsubscribe("t1", bob);
    espn -> publish(sportsTopic, "Match postponed due to rain.");

    return 0;
}
