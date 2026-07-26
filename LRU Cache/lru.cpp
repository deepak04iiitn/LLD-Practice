#include <bits/stdc++.h>
using namespace std;

class Node {
    public:
        int key;
        int value;

        Node* prev;
        Node* next;

        Node(int key , int value) : key(key), value(value), prev(nullptr), next(nullptr) {}
};


class Cache {
    private:
        int capacity;
        unordered_map<int, Node*> map;

        // dummy head/tail: head -> next is most recently used, tail -> prev is least recently used
        Node* head;
        Node* tail;

        void removeNode(Node* node) {
            node -> prev -> next = node -> next;
            node -> next -> prev = node -> prev;
        }

        void insertAtFront(Node* node) {
            node -> next = head -> next;
            node -> prev = head;
            head -> next -> prev = node;
            head -> next = node;
        }

    public:
        Cache(int cap) : capacity(cap) {
            head = new Node(-1, -1);
            tail = new Node(-1, -1);
            head -> next = tail;
            tail -> prev = head;
        }

        ~Cache() {
            Node* curr = head;
            while(curr) {
                Node* next = curr -> next;
                delete curr;
                curr = next;
            }
        }

        int get(int key) {

            if(map.find(key) == map.end()) {
                return -1;
            }

            Node* node = map[key];
            removeNode(node);
            insertAtFront(node);

            return node -> value;
        }

        void put(int key, int value) {

            if(map.find(key) != map.end()) {
                Node* node = map[key];
                node -> value = value;
                removeNode(node);
                insertAtFront(node);
                return;
            }

            if((int)map.size() == capacity) {
                Node* lru = tail -> prev;
                removeNode(lru);
                map.erase(lru -> key);
                delete lru;
            }

            Node* node = new Node(key, value);
            map[key] = node;
            insertAtFront(node);
        }
};


int main() {

    Cache cache(2);

    cache.put(1, 1);
    cache.put(2, 2);
    cout << cache.get(1) << endl; // 1

    cache.put(3, 3);              // evicts key 2
    cout << cache.get(2) << endl; // -1

    cache.put(4, 4);              // evicts key 1
    cout << cache.get(1) << endl; // -1
    cout << cache.get(3) << endl; // 3
    cout << cache.get(4) << endl; // 4

    return 0;
}
