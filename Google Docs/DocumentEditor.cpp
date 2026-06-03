#include <iostream>
#include <vector>
#include <string>
#include<fstream>

using namespace std;

class DocumentElement {
    public:
        virtual string render() = 0;
};

class TextElement : public DocumentElement {
    private:
        string text;

    public:
        TextElement(string text) {
            this -> text = text;
        }

        string render() override {
            return text;
        }
};

class ImageElement : public DocumentElement {
    private:
        string path;

    public:
        ImageElement(string path) {
            this -> path = path;
        }

        string render() override {
            return path;
        }
};


class Document {
    private:
        vector<DocumentElement*>elements;

    public:
        void addElement(DocumentElement* ele) {
            elements.push_back(ele);
        }

        string renderDocument() {
            string result;

            for(auto element : elements) {
                result += element -> render();
            }

            return result;
        }

};


class Persistence {
    public:
        virtual void save(string data) = 0;
};

class saveToDB : public Persistence {
    public:
        void save(string data) override {
            cout << "Saving to DB...";
        }
};

class saveToFile : public Persistence {
    public:
        void save(string data) override {
            cout << "Saving to File...";
        }
};


class DocumentEditor {
    private:
        Document* doc;
        Persistence* db;
        string renderedDocument;

    public:
        DocumentEditor(Document* document, Persistence* storage) {
            this -> doc = document;
            this -> db = storage;
        }

        void addText(string text) {
            doc -> addElement(new TextElement(text));
        }

        void addImage(string path) {
            doc -> addElement(new ImageElement(path));
        }

        string renderDocument() {
            if(renderedDocument.empty()) {
                renderedDocument = doc -> renderDocument();
            }

            return renderedDocument;
        }

        void saveDocument() {
            db -> save(renderDocument());
        }
};


int main() {
    Document* document = new Document();
    Persistence* persistence = new saveToDB();

    DocumentEditor* editor = new DocumentEditor(document , persistence);

    // Simulate a client using the editor with common text formatting features.
    editor -> addText("Hello, world!");
    editor -> addImage("picture.jpg");

    // Render and display the final document.
    cout << editor -> renderDocument() << endl;

    editor -> saveDocument();

    return 0;

}

