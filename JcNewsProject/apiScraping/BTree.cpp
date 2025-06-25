#include <iostream>
#include <functional>
#include <string>
#include <vector>

using namespace std;

// Estructura para representar un artículo del JSON
struct Article {
    string id;
    string titulo;
    string fuente;
    string autor;
    string fecha;
    string url;
    string url_imagen;
    string descripcion;
};

// Nodo del Árbol B
template <class T, int Order, typename KeyType>
struct Node {
    int number_of_keys;
    int order;
    int position = -1;
    KeyType* keys;
    T* values;
    Node<T, Order, KeyType>** children;

    explicit Node(int order) : order(order), number_of_keys(0) {
        keys = new KeyType[order];
        values = new T[order];
        children = new Node<T, Order, KeyType>*[order + 1]();
    }

    ~Node() {
        delete[] keys;
        delete[] values;
        for (int i = 0; i <= number_of_keys; ++i) {
            delete children[i];
        }
        delete[] children;
    }

    int Insert(const KeyType& key, const T& value) {
        if (children[0] == nullptr) {
            // Nodo hoja
            keys[++position] = key;
            values[position] = value;
            ++number_of_keys;
            
            // Ordenar las claves y valores
            for (int i = position; i > 0 && keys[i] < keys[i-1]; --i) {
                swap(keys[i], keys[i-1]);
                swap(values[i], values[i-1]);
            }
        } else {
            // Nodo interno
            int i = 0;
            while (i < number_of_keys && key > keys[i]) i++;
            
            int check = children[i]->Insert(key, value);
            
            if (check) {
                KeyType mid_key;
                T mid_value;
                Node<T, Order, KeyType>* newNode = split(children[i], &mid_key, &mid_value);
                
                // Insertar la clave media en este nodo
                int j = number_of_keys;
                while (j > i) {
                    keys[j] = keys[j-1];
                    values[j] = values[j-1];
                    children[j+1] = children[j];
                    j--;
                }
                keys[i] = mid_key;
                values[i] = mid_value;
                children[i+1] = newNode;
                ++number_of_keys;
                ++position;
            }
        }
        
        return number_of_keys == order ? 1 : 0;
    }

    Node* split(Node* node, KeyType* med_key, T* med_value) {
        int mid = node->number_of_keys / 2;
        *med_key = node->keys[mid];
        *med_value = node->values[mid];
        
        auto* newNode = new Node<T, Order, KeyType>(order);
        
        // Copiar la mitad derecha al nuevo nodo
        for (int i = mid + 1; i < node->number_of_keys; ++i) {
            newNode->keys[++newNode->position] = node->keys[i];
            newNode->values[newNode->position] = node->values[i];
            newNode->children[newNode->position] = node->children[i];
            ++newNode->number_of_keys;
        }
        newNode->children[newNode->position + 1] = node->children[node->number_of_keys];
        
        // Actualizar el nodo original
        node->number_of_keys = mid;
        node->position = mid - 1;
        
        return newNode;
    }

    void Print(int level = 0) const {
        for (int i = 0; i < number_of_keys; ++i) {
            if (children[i] != nullptr) {
                children[i]->Print(level + 1);
            }
            
            cout << string(level * 4, ' ') << "[" << keys[i] << "] " 
                 << values[i].titulo.substr(0, 30) << "..." << endl;
        }
        
        if (children[number_of_keys] != nullptr) {
            children[number_of_keys]->Print(level + 1);
        }
    }
};

// Árbol B
template <class T, int Order, typename KeyType>
class BTree {
private:
    Node<T, Order, KeyType>* root;
    function<KeyType(const T&)> keyExtractor;

public:
    BTree(function<KeyType(const T&)> extractor) : root(nullptr), keyExtractor(extractor) {}
    
    ~BTree() {
        delete root;
    }

    void Insert(const T& value) {
        KeyType key = keyExtractor(value);
        
        if (root == nullptr) {
            root = new Node<T, Order, KeyType>(Order);
            root->keys[++root->position] = key;
            root->values[root->position] = value;
            root->number_of_keys = 1;
        } else {
            int check = root->Insert(key, value);
            if (check) {
                KeyType mid_key;
                T mid_value;
                Node<T, Order, KeyType>* splitNode = root->split(root, &mid_key, &mid_value);
                
                auto* newRoot = new Node<T, Order, KeyType>(Order);
                newRoot->keys[++newRoot->position] = mid_key;
                newRoot->values[newRoot->position] = mid_value;
                newRoot->children[0] = root;
                newRoot->children[1] = splitNode;
                newRoot->number_of_keys = 1;
                
                root = newRoot;
            }
        }
    }

    void Print() const {
        if (root != nullptr) {
            root->Print();
        } else {
            cout << "El árbol está vacío" << endl;
        }
    }
};

// Funciones para extraer diferentes campos del artículo
string extractTitulo(const Article& a) { return a.titulo; }
string extractFecha(const Article& a) { return a.fecha; }
string extractAutor(const Article& a) { return a.autor; }
string extractFuente(const Article& a) { return a.fuente; }

int main() {
    // Ejemplo de uso con diferentes claves
    
    // 1. Árbol ordenado por título
    BTree<Article, 3, string> tituloTree(extractTitulo);
    
    // 2. Árbol ordenado por fecha
    BTree<Article, 3, string> fechaTree(extractFecha);
    
    // 3. Árbol ordenado por autor
    BTree<Article, 3, string> autorTree(extractAutor);
    
    // Crear algunos artículos de ejemplo
    Article a1 = {
        "id1", 
        "Pese a no ganar títulos, a Inglaterra siempre le quedaba haber inventado el fútbol",
        "Xataka.com",
        "Miguel Jorge",
        "2025-05-22T14:31:47Z",
        "https://example.com/1",
        "https://example.com/image1.jpg",
        "Descripción del artículo 1"
    };
    
    Article a2 = {
        "id2",
        "El fichaje del verano ya está aquí: Striker Manager 3 x MARCA",
        "Marca",
        "Mario Blázquez",
        "2025-06-10T15:01:02Z",
        "https://example.com/2",
        "https://example.com/image2.jpg",
        "Descripción del artículo 2"
    };
    
    Article a3 = {
        "id3",
        "Botafogo y el grupo Eagle Football Holdings, tras Cristiano Ronaldo",
        "Marca",
        "Juan Pérez",
        "2025-05-28T10:15:33Z",
        "https://example.com/3",
        "https://example.com/image3.jpg",
        "Descripción del artículo 3"
    };
    
    // Insertar artículos en los diferentes árboles
    tituloTree.Insert(a1);
    tituloTree.Insert(a2);
    tituloTree.Insert(a3);
    
    fechaTree.Insert(a1);
    fechaTree.Insert(a2);
    fechaTree.Insert(a3);
    
    autorTree.Insert(a1);
    autorTree.Insert(a2);
    autorTree.Insert(a3);
    
    // Mostrar los árboles
    cout << "Árbol ordenado por título:\n";
    tituloTree.Print();
    
    cout << "\nÁrbol ordenado por fecha:\n";
    fechaTree.Print();
    
    cout << "\nÁrbol ordenado por autor:\n";
    autorTree.Print();
    
    return 0;
}
