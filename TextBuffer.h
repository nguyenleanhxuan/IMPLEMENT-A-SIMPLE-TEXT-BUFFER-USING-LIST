#ifndef __TEXT_BUFFER_H__
#define __TEXT_BUFFER_H__

#include "main.h"

template <typename T>
class DoublyLinkedList {
    // TODO: may provide some attributes

public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    void insertAtHead(T data);
    void insertAtTail(T data);
    void insertAt(int index, T data);
    void deleteAt(int index);
    T& get(int index) const;
    int indexOf(T item) const;
    bool contains(T item) const;
    int size() const;
    void reverse();
    string toString(string (*convert2str)(T&) = 0) const;

private:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        Node(const T& data, Node* prev = nullptr, Node* next = nullptr)
            : data(data), prev(prev), next(next) {}
    };
    Node* head;
    Node* tail;
    int count;
};

class TextBuffer {
public:
    class HistoryManager {
    public:
        struct Action {
            std::string actionName;
            int cursorPos;
            char c;
            Action(const std::string& name, int pos, char ch) : actionName(name), cursorPos(pos), c(ch) {}
        };
        struct Node {
            Action data;
            Node* next;
            Node(const Action& a, Node* n = nullptr) : data(a), next(n) {}
        };
    private:
        Node* head;
        Node* tail;
        int count;
    public:
        HistoryManager();
        ~HistoryManager();
        void addAction(const string &actionName, int cursorPos, char c);
        void printHistory() const;
        int size() const;
        Action popLastAction(); // Lấy và xóa action cuối cùng
        void removeLastAction(); // Xóa action cuối cùng (không trả về)
    };
private:
    DoublyLinkedList<char> buffer; 
    int cursorPos;
    HistoryManager history;
    DoublyLinkedList<HistoryManager::Action> redoStack;

    // TODO: may provide some attributes

public:
    TextBuffer();
    ~TextBuffer();

    void insert(char c);
    void deleteChar();
    void moveCursorLeft();
    void moveCursorRight();
    void moveCursorTo(int index);
    string getContent() const;
    int getCursorPos() const;
    int findFirstOccurrence(char c) const;
    int* findAllOccurrences(char c, int &count) const;
    void sortAscending();
    void deleteAllOccurrences(char c);
    void undo();
    void redo();
};

#endif // __TEXT_BUFFER_H__

