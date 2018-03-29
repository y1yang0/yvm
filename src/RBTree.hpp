#ifndef YVM_RBTREE_H
#define YVM_RBTREE_H

#include <type_traits>

template <typename _KeyType, typename _ValueType>
class RBTree {
    using _ResolvedKeyType = typename std::conditional<std::is_class<_KeyType>::value,
                                                       typename std::add_lvalue_reference<_KeyType>::type,
                                                       _KeyType>::type;

    using _ResolvedValueType = typename std::conditional<std::is_class<_ValueType>::value,
                                                         typename std::add_lvalue_reference<_ValueType>::type,
                                                         _ValueType>::type;

public:
    struct Node {
        explicit Node() : left(nullptr), right(nullptr), parent(nullptr) {}

        Node* left;
        Node* right;
        Node* parent;

        _KeyType key;
        _ValueType value;
        bool black;
    };

public:
    inline explicit RBTree() {
        root = nullptr;
    }

    inline ~RBTree() {
        clear();
    }

    inline bool empty() {
        return root == nullptr ? true : false;
    }

    inline void clear() {
        recursiveClear(root);
        root = nullptr;
    }

public :
    void add(_ResolvedKeyType key, _ResolvedValueType value);

    void del(_ResolvedKeyType key);

    bool exist(_ResolvedKeyType key);

    _ValueType get(_ResolvedKeyType key);

private:
    void recursiveClear(Node* node);

    Node* minNode(Node* start);

    void addFixup(Node* current);

    void deleteFixup(Node* current);

    void rotateLeft(Node* node);

    void rotateRight(Node* node);

    void transplant(Node* u, Node* v);

private:
    inline bool isRed(Node* node) const {
        return node->black == false;
    }

    inline bool isBlack(Node* node) const {
        return node->black == true;
    }

    inline bool exist(Node* node) const {
        return node != nullptr;
    }

private:
    Node* root;
};

template <typename _KeyType, typename _ValueType>
void RBTree<_KeyType, _ValueType>::add(_ResolvedKeyType key, _ResolvedValueType value) {
    Node* traverse = root;
    Node* validNode = nullptr;
    bool insertLeft = false;

    while (traverse != nullptr) {
        if (key == traverse->key) {
            traverse->value = value;
            return;
        }
        else if (key < traverse->key) {
            validNode = traverse;
            insertLeft = true;
            traverse = traverse->left;
        }
        else if (key > traverse->key) {
            validNode = traverse;
            insertLeft = false;
            traverse = traverse->right;
        }
    }


    Node* newNode = new Node();
    newNode->parent = validNode;

    if (root == nullptr) {
        //if the tree was empty
        root = newNode;
    }
    else {
        if (insertLeft) {
            validNode->left = newNode;
        }
        else {
            validNode->right = newNode;
        }
    }
    newNode->black = false;
    newNode->key = key;
    newNode->value = value;
    addFixup(newNode);
}

template <typename _KeyType, typename _ValueType>
void RBTree<_KeyType, _ValueType>::del(_ResolvedKeyType key) {
    if (root == nullptr) {
        return;
    }

    Node* traverse = root;

    while (traverse != nullptr) {
        if (key == traverse->key) {
            break;
        }
        else if (key < traverse->key) {
            traverse = traverse->left;
        }
        else if (key > traverse->key) {
            traverse = traverse->right;
        }
    }

    //if the specified key does not exist
    if (traverse == nullptr) {
        return;
    }

    Node* temp = traverse;
    Node* successor = nullptr;
    bool originalColor = temp->black;
    if (traverse->left == nullptr && traverse->right == nullptr) {
        if (traverse == root) {
            root = nullptr;
            delete temp;
            if (successor && originalColor == true) {
                deleteFixup(successor);
            }
            return;
        }
        else {
            if (traverse->parent->left == traverse) {
                traverse->parent->left = nullptr;
                delete temp;
                return;
            }
            else if (traverse->parent->right == traverse) {
                traverse->parent->right = nullptr;
                delete temp;
                return;
            }
        }
    }
    else if (traverse->left == nullptr && traverse->right != nullptr) {
        successor = traverse->right;
        successor->black = originalColor;
        transplant(traverse, traverse->right);
        delete temp;
        return;
    }
    else if (traverse->right == nullptr && traverse->left != nullptr) {
        successor = traverse->left;
        successor->black = originalColor;
        transplant(traverse, traverse->left);
        delete temp;
        return;
    }
    else {
        temp = minNode(traverse->right);
        originalColor = temp->black;
        successor = temp->right;

        if (temp->parent != traverse) {
            if (temp->right) {
                transplant(temp, temp->right);
            }
            else {
                if (temp->parent->right == temp) {
                    temp->parent->right == nullptr;
                }
                else {
                    temp->parent->left = nullptr;
                }
            }
            temp->right = traverse->right;
            temp->right->parent = temp;
        }
        transplant(traverse, temp);
        temp->left = traverse->left;
        temp->left->parent = temp;
        temp->black = traverse->black;
    }
    if (successor && originalColor) {
        deleteFixup(successor);
    }
    delete traverse;
}

template <typename _KeyType, typename _ValueType>
bool RBTree<_KeyType, _ValueType>::exist(_ResolvedKeyType key) {
    Node* traverse = root;

    while (traverse != nullptr) {
        if (key == traverse->key) {
            break;
        }
        else if (key < traverse->key) {
            traverse = traverse->left;
        }
        else if (key > traverse->key) {
            traverse = traverse->right;
        }
    }

    //if the specified key does not exist
    return traverse == nullptr ? false : true;
}

template <typename _KeyType, typename _ValueType>
_ValueType RBTree<_KeyType, _ValueType>::get(_ResolvedKeyType key) {
    Node* traverse = root;

    while (traverse != nullptr) {
        if (key == traverse->key) {
            break;
        }
        else if (key < traverse->key) {
            traverse = traverse->left;
        }
        else if (key > traverse->key) {
            traverse = traverse->right;
        }
    }

    return traverse->value;
}

template <typename _KeyType, typename _ValueType>
void RBTree<_KeyType, _ValueType>::recursiveClear(Node* node) {
    if (node == nullptr) {
        return;
    }
    recursiveClear(node->left);
    recursiveClear(node->right);
    delete node;
}

template <typename _KeyType, typename _ValueType>
typename RBTree<_KeyType, _ValueType>::Node* RBTree<_KeyType, _ValueType>::minNode(Node* start) {
    if (root == nullptr) {
        return nullptr;
    }

    Node* temp = start;

    while (temp->left != nullptr) {
        temp = temp->left;
    }
    return temp;
}

template <typename _KeyType, typename _ValueType>
void RBTree<_KeyType, _ValueType>::rotateLeft(Node* node) {
    Node* rightNode = node->right;
    node->right = rightNode->left;
    if (rightNode->left != nullptr) {
        rightNode->left->parent = node;
    }
    rightNode->parent = node->parent;

    if (node->parent == nullptr) {
        root = rightNode;
    }
    else if (node->parent->left == node) {
        node->parent->left = rightNode;
    }
    else {
        node->parent->right = rightNode;
    }
    rightNode->left = node;
    node->parent = rightNode;
}

template <typename _KeyType, typename _ValueType>
void RBTree<_KeyType, _ValueType>::rotateRight(Node* node) {
    Node* leftNode = node->left;
    node->left = leftNode->right;
    if (leftNode->right != nullptr) {
        leftNode->right->parent = node;
    }

    leftNode->parent = node->parent;

    if (node->parent == nullptr) {
        root = leftNode;
    }
    else if (node->parent->right == node) {
        node->parent->right = leftNode;
    }
    else {
        node->parent->left = leftNode;
    }
    leftNode->right = node;
    node->parent = leftNode;
}

template <typename _KeyType, typename _ValueType>
void RBTree<_KeyType, _ValueType>::transplant(Node* u, Node* v) {
    if (u->parent == nullptr) {
        root = v;
    }
    else if (u->parent->left == u) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

template <typename _KeyType, typename _ValueType>
void RBTree<_KeyType, _ValueType>::addFixup(Node* current) {
    while (current->parent && current->parent->black == false) {
        if (current->parent->parent->left == current->parent) {
            //if the father node is the left child of its father
            Node* uncle = current->parent->parent->right;
            if (uncle && uncle->black == false) {
                //case 1
                current->parent->black = true;
                uncle->black = true;
                current->parent->parent->black = false;
                current = current->parent->parent;
            }
            else if (current == current->parent->right) {
                //case 2
                current = current->parent;
                rotateLeft(current);
            }
            else if (current == current->parent->left) {
                //case 3
                current->parent->black = true;
                current->parent->parent->black = false;
                rotateRight(current->parent->parent);
            }
            else {
                static_assert(true, "should not reach here");
            }
        }
        else {
            //otherwise, the father node is the right child of its father
            Node* uncle = current->parent->parent->left;
            if (uncle && uncle->black == false) {
                //case 1
                current->parent->black = true;
                uncle->black = true;
                current->parent->parent->black = false;
                current = current->parent->parent;
            }
            else if (current == current->parent->left) {
                //case 2
                current = current->parent;
                rotateRight(current);
            }
            else if (current == current->parent->right) {
                //case 3
                current->parent->black = true;
                current->parent->parent->black = false;
                rotateLeft(current->parent->parent);
            }
            else {
                static_assert(true, "should not reach here");
            }
        }

    }
    root->black = true;
}

template <typename _KeyType, typename _ValueType>
void RBTree<_KeyType, _ValueType>::deleteFixup(Node* current) {
    while (current && current != root && current->black == true) {
        if (current == current->parent->left) {
            Node* brother = current->parent->right;
            if (exist(brother)) {
                if (isRed(brother)) {
                    brother->black = true;
                    current->parent->black = false;
                    rotateLeft(current->parent);
                    brother = current->parent->right;
                }
                else if (
                    (isBlack(brother) && !exist(brother->left) && !exist(brother->right)) ||
                    (isBlack(brother) && exist(brother->left) && isBlack(brother->left) &&
                        !exist(brother->right)) ||
                    (isBlack(brother) && exist(brother->right) && isBlack(brother->right) &&
                        !exist(brother->left)) ||
                    (isBlack(brother) && exist(brother->right) && isBlack(brother->right) && exist(brother->left) &&
                        isBlack(brother->left))
                ) {
                    brother->black = false;
                    current = current->parent;
                }
                else if (
                    (isBlack(brother) && exist(brother->left) && isRed(brother->left) && exist(brother->right) &&
                        isBlack(brother->right)) ||
                    (isBlack(brother) && exist(brother->left) && isRed(brother->left) && !exist(brother->right))
                ) {
                    brother->left->black = true;
                    brother->black = false;
                    rotateRight(brother);
                    brother = current->parent->right;
                }
                else if (exist(brother->right) &&
                    isBlack(brother) &&
                    isRed(brother->right)) {
                    brother->black = current->parent->black;
                    current->parent->black = true;
                    brother->right->black = true;
                    rotateLeft(current->parent);
                    current = root;
                }
                else {
                    static_assert(true, "should not reach here");
                }
            }
            else {
                break;
            }
        }
        else {
            Node* brother = current->parent->left;
            if (exist(brother)) {
                if (isRed(brother)) {
                    brother->black = true;
                    current->parent->black = false;
                    rotateRight(current->parent);
                    brother = current->parent->left;
                }
                else if (
                    (isBlack(brother) && !exist(brother->left) && !exist(brother->right)) ||
                    (isBlack(brother) && exist(brother->left) && isBlack(brother->left) &&
                        !exist(brother->right)) ||
                    (isBlack(brother) && exist(brother->right) && isBlack(brother->right) &&
                        !exist(brother->left)) ||
                    (isBlack(brother) && exist(brother->right) && isBlack(brother->right) && exist(brother->left) &&
                        isBlack(brother->left))
                ) {
                    brother->black = false;
                    current = current->parent;
                }
                else if (
                    (isBlack(brother) && exist(brother->right) && isRed(brother->right) && exist(brother->left) &&
                        isBlack(brother->left)) ||
                    (isBlack(brother) && exist(brother->right) && isRed(brother->right) && !exist(brother->left))
                ) {
                    brother->right->black = true;
                    brother->black = false;
                    rotateLeft(brother);
                    brother = current->parent->left;
                }
                else if (exist(brother->left) &&
                    isBlack(brother) &&
                    isRed(brother->left)) {
                    brother->black = current->parent->black;
                    current->parent->black = true;
                    brother->left->black = true;
                    rotateRight(current->parent);
                    current = root;
                }
                else {
                    static_assert(true, "should not reach here");
                }
            }
            else {
                break;
            }
        }
        current->black = true;
    }
}

#endif //YVM_RBTREE_H
