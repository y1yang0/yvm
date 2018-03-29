#pragma once

template <typename ElemType>
class Stack {
public:
    explicit Stack();
    ~Stack();

    void push(const ElemType &  element);

    ElemType pop();

    ElemType peek() const;

    bool isEmpty() const;

private:
    Stack(const Stack &) = delete;
    Stack& operator= (const Stack&) = delete;

    struct Node {
        ElemType elem;
        Node * next;
    };
    Node * top = nullptr;
};

template <typename ElemType>
Stack<ElemType>::~Stack() {
    
}

template<typename ElemType>
inline void Stack<ElemType>::push(const ElemType & element) {
    Node *newNode = new Node();
    newNode->elem = element;
    newNode->next = top;
    top = newNode;
}

template<typename ElemType>
inline ElemType Stack<ElemType>::pop() {
    ElemType elem = top->elem;
    Node * tempTop = top;
    top = top->next;
    delete tempTop;
    return elem;

}

template<typename ElemType>
inline ElemType Stack<ElemType>::peek() const {
    return top->elem;
}

template<typename ElemType>
bool Stack<ElemType>::isEmpty() const {
    return top == nullptr;
}
