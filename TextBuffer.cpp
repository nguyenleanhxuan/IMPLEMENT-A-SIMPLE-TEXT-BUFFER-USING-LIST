#include "TextBuffer.h"

// ----------------- DoublyLinkedList -----------------
template <typename T>
DoublyLinkedList<T>::DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}

template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
    Node* current = head;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    head = tail = nullptr;
    count = 0;
}

template <typename T>
void DoublyLinkedList<T>::insertAtHead(T data) {
    Node* newNode = new Node(data, nullptr, head);
    if (head) head->prev = newNode;
    head = newNode;
    if (!tail) tail = newNode;
    ++count;
}

template <typename T>
void DoublyLinkedList<T>::insertAtTail(T data) {
    Node* newNode = new Node(data, tail, nullptr);
    if (tail) tail->next = newNode;
    tail = newNode;
    if (!head) head = newNode;
    ++count;
}

template <typename T>
void DoublyLinkedList<T>::insertAt(int index, T data) {
    if (index < 0 || index > count) throw std::out_of_range("Index is invalid!");
    if (index == 0) {
        insertAtHead(data);
        return;
    }
    if (index == count) {
        insertAtTail(data);
        return;
    }
    Node* curr = head;
    for (int i = 0; i < index; ++i) curr = curr->next;
    Node* newNode = new Node(data, curr->prev, curr);
    curr->prev->next = newNode;
    curr->prev = newNode;
    ++count;
}

template <typename T>
void DoublyLinkedList<T>::deleteAt(int index) {
    if (index < 0 || index >= count) throw std::out_of_range("Index is invalid!");
    Node* curr = head;
    for (int i = 0; i < index; ++i) curr = curr->next;
    if (curr->prev) curr->prev->next = curr->next;
    else head = curr->next;
    if (curr->next) curr->next->prev = curr->prev;
    else tail = curr->prev;
    delete curr;
    --count;
}

template <typename T>
T& DoublyLinkedList<T>::get(int index) const {
    if (index < 0 || index >= count) throw std::out_of_range("Index is invalid!");
    Node* curr = head;
    for (int i = 0; i < index; ++i) curr = curr->next;
    return curr->data;
}

template <typename T>
int DoublyLinkedList<T>::indexOf(T item) const {
    Node* curr = head;
    int idx = 0;
    while (curr) {
        if (curr->data == item) return idx;
        curr = curr->next;
        ++idx;
    }
    return -1;
}

template <typename T>
bool DoublyLinkedList<T>::contains(T item) const {
    return indexOf(item) != -1;
}

template <typename T>
int DoublyLinkedList<T>::size() const {
    return count;
}

template <typename T>
void DoublyLinkedList<T>::reverse() {
    Node* curr = head;
    Node* temp = nullptr;
    while (curr) {
        temp = curr->prev;
        curr->prev = curr->next;
        curr->next = temp;
        curr = curr->prev;
    }
    temp = head;
    head = tail;
    tail = temp;
}

template <typename T>
string DoublyLinkedList<T>::toString(string (*convert2str)(T&) ) const {
    std::ostringstream oss;
    oss << "[";
    Node* curr = head;
    int idx = 0;
    while (curr) {
        if (idx > 0) oss << ", ";
        if (convert2str) oss << convert2str(curr->data);
        else oss << curr->data;
        curr = curr->next;
        ++idx;
    }
    oss << "]";
    return oss.str();
}

// TODO: implement other methods of DoublyLinkedList

// ----------------- TextBuffer -----------------
TextBuffer::TextBuffer() : buffer(), cursorPos(0) {
    // Khởi tạo buffer rỗng, con trỏ ở đầu
}

TextBuffer::~TextBuffer() {
    // Không cần giải phóng gì thêm vì DoublyLinkedList tự giải phóng
}

void TextBuffer::insert(char c) {
    buffer.insertAt(cursorPos, c);
    history.addAction("insert", cursorPos, c);
    ++cursorPos;
    // Xóa stack redo nếu có (nếu bạn mở rộng cho undo/redo hoàn chỉnh)
}

void TextBuffer::deleteChar() {
    if (cursorPos == 0) return; // Không có ký tự nào phía trước để xóa
    char deleted = buffer.get(cursorPos - 1);
    buffer.deleteAt(cursorPos - 1);
    history.addAction("delete", cursorPos, deleted);
    --cursorPos;
    // Xóa stack redo nếu có
}

void TextBuffer::moveCursorLeft() {
    if (cursorPos == 0) throw cursor_error();
    history.addAction("move", cursorPos, 'L');
    --cursorPos;
    // Xóa stack redo nếu có
}

void TextBuffer::moveCursorRight() {
    if (cursorPos == buffer.size()) throw cursor_error();
    history.addAction("move", cursorPos, 'R');
    ++cursorPos;
    // Xóa stack redo nếu có
}

void TextBuffer::moveCursorTo(int index) {
    if (index < 0 || index > buffer.size()) throw std::out_of_range("Index is invalid!");
    history.addAction("move", cursorPos, 'J');
    cursorPos = index;
    // Xóa stack redo nếu có
}

std::string TextBuffer::getContent() const {
    std::ostringstream oss;
    for (int i = 0; i < buffer.size(); ++i) {
        oss << buffer.get(i);
    }
    return oss.str();
}

int TextBuffer::getCursorPos() const {
    return cursorPos;
}

int TextBuffer::findFirstOccurrence(char c) const {
    return buffer.indexOf(c);
}

int* TextBuffer::findAllOccurrences(char c, int &count) const {
    count = 0;
    int sz = buffer.size();
    // Đếm số lượng trước để cấp phát mảng
    for (int i = 0; i < sz; ++i) {
        if (buffer.get(i) == c) ++count;
    }
    if (count == 0) return nullptr;
    int* result = new int[count];
    int idx = 0;
    for (int i = 0; i < sz; ++i) {
        if (buffer.get(i) == c) result[idx++] = i;
    }
    return result;
}

void merge(char* arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    char* L = new char[n1];
    char* R = new char[n2];
    for (int i = 0; i < n1; ++i) L[i] = arr[l + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[m + 1 + j];
    int i = 0, j = 0, k = l;
    auto cmp = [](char a, char b) -> bool {
        if (std::isalpha(a) && std::isalpha(b)) {
            if (std::tolower(a) == std::tolower(b)) return std::isupper(a);
            return std::tolower(a) < std::tolower(b);
        }
        return a < b;
    };
    while (i < n1 && j < n2) {
        if (cmp(L[i], R[j])) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    delete[] L;
    delete[] R;
}
void mergeSort(char* arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}
void TextBuffer::sortAscending() {
    int sz = buffer.size();
    if (sz <= 1) {
        cursorPos = 0;
        return;
    }
    char* arr = new char[sz];
    for (int i = 0; i < sz; ++i) arr[i] = buffer.get(i);
    mergeSort(arr, 0, sz - 1);
    for (int i = 0; i < sz; ++i) buffer.get(i) = arr[i];
    delete[] arr;
    history.addAction("sort", cursorPos, '\0');
    cursorPos = 0;
    // Xóa stack redo nếu có
}

void TextBuffer::deleteAllOccurrences(char c) {
    int sz = buffer.size();
    int newCursor = cursorPos;
    bool deleted = false;
    for (int i = sz - 1; i >= 0; --i) {
        if (buffer.get(i) == c) {
            buffer.deleteAt(i);
            if (i < newCursor) --newCursor;
            deleted = true;
        }
    }
    if (deleted) cursorPos = 0;
    else cursorPos = newCursor;
}

void TextBuffer::undo() {
    if (history.size() == 0) return;
    HistoryManager::Action last = history.popLastAction();
    if (last.actionName == "insert") {
        buffer.deleteAt(last.cursorPos);
        cursorPos = last.cursorPos;
    } else if (last.actionName == "delete") {
        buffer.insertAt(last.cursorPos - 1, last.c);
        cursorPos = last.cursorPos;
    } else if (last.actionName == "move") {
        cursorPos = last.cursorPos;
    } else if (last.actionName == "sort") {
        // Không thể undo sort nếu không lưu trạng thái trước đó
        // Có thể mở rộng nếu cần
    }
    redoStack.insertAtTail(last);
}

void TextBuffer::redo() {
    if (redoStack.size() == 0) return;
    HistoryManager::Action last = redoStack.get(redoStack.size() - 1);
    redoStack.deleteAt(redoStack.size() - 1);
    if (last.actionName == "insert") {
        buffer.insertAt(last.cursorPos, last.c);
        cursorPos = last.cursorPos + 1;
    } else if (last.actionName == "delete") {
        buffer.deleteAt(last.cursorPos - 1);
        cursorPos = last.cursorPos - 1;
    } else if (last.actionName == "move") {
        if (last.c == 'L') cursorPos = last.cursorPos - 1;
        else if (last.c == 'R') cursorPos = last.cursorPos + 1;
        else if (last.c == 'J') cursorPos = last.cursorPos; // hoặc lưu thêm thông tin vị trí mới
    } else if (last.actionName == "sort") {
        // Không thể redo sort nếu không lưu trạng thái sau đó
        // Có thể mở rộng nếu cần
    }
    history.addAction(last.actionName, last.cursorPos, last.c);
}

// TODO: implement other methods of TextBuffer

// ----------------- HistoryManager -----------------


TextBuffer::HistoryManager::HistoryManager() : head(nullptr), tail(nullptr), count(0) {}

TextBuffer::HistoryManager::~HistoryManager() {
    Node* curr = head;
    while (curr) {
        Node* next = curr->next;
        delete curr;
        curr = next;
    }
    head = tail = nullptr;
    count = 0;
}

void TextBuffer::HistoryManager::addAction(const std::string &actionName, int cursorPos, char c) {
    Action action(actionName, cursorPos, c);
    Node* newNode = new Node(action);
    if (!head) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    ++count;
}

void TextBuffer::HistoryManager::printHistory() const {
    std::cout << "[";
    Node* curr = head;
    bool first = true;
    while (curr) {
        if (!first) std::cout << ", ";
        std::cout << "(" << curr->data.actionName << ", " << curr->data.cursorPos << ", ";
        if (curr->data.c == '\0') std::cout << "\\0";
        else std::cout << curr->data.c;
        std::cout << ")";
        curr = curr->next;
        first = false;
    }
    std::cout << "]" << std::endl;
}

int TextBuffer::HistoryManager::size() const {
    return count;
}

TextBuffer::HistoryManager::Action TextBuffer::HistoryManager::popLastAction() {
    if (!head) throw std::out_of_range("No action to pop");
    if (head == tail) {
        Action res = head->data;
        delete head;
        head = tail = nullptr;
        count = 0;
        return res;
    }
    Node* prev = head;
    while (prev->next != tail) prev = prev->next;
    Action res = tail->data;
    delete tail;
    tail = prev;
    tail->next = nullptr;
    --count;
    return res;
}

void TextBuffer::HistoryManager::removeLastAction() {
    if (!head) return;
    if (head == tail) {
        delete head;
        head = tail = nullptr;
        count = 0;
        return;
    }
    Node* prev = head;
    while (prev->next != tail) prev = prev->next;
    delete tail;
    tail = prev;
    tail->next = nullptr;
    --count;
}

//TODO: implement other methods of HistoryManager


// Explicit template instantiation for char, string, int, double, float, and Point
template class DoublyLinkedList<char>;
template class DoublyLinkedList<string>;
template class DoublyLinkedList<int>;
template class DoublyLinkedList<double>;
template class DoublyLinkedList<float>;
template class DoublyLinkedList<Point>;