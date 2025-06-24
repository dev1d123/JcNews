#include "btree.h"

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

template<typename T>
struct Clave {
    T valor;
    long registro;
};

template<typename T>
class BTree;

template<typename T>
class BNode {
public:
    BNode(int grado) : clavesUsadas(0), padre(nullptr) {
        claves.resize(grado);
        hijos.resize(grado + 1, nullptr);
    }

    int clavesUsadas;
    vector<Clave<T>> claves;
    vector<shared_ptr<BNode>> hijos;
    weak_ptr<BNode> padre;

    friend class BTree<T>;
};

template<typename T>
class BTree {
public:
    BTree(int grado) : grado(grado), minClaves(grado / 2), raiz(nullptr) {}

    bool insertar(const Clave<T>& clave);
    long buscar(const T& valor);
    void mostrar();

private:
    shared_ptr<BNode<T>> raiz;
    int grado;
    int minClaves;

    void insertarRec(const Clave<T>& clave, shared_ptr<BNode<T>> nodo, shared_ptr<BNode<T>> h1, shared_ptr<BNode<T>> h2);
    void ver(shared_ptr<BNode<T>> nodo, int nivel = 0);
};

template<typename T>
bool BTree<T>::insertar(const Clave<T>& clave) {
    if (!raiz) {
        raiz = make_shared<BNode<T>>(grado);
        raiz->claves[0] = clave;
        raiz->clavesUsadas = 1;
        return true;
    }

    shared_ptr<BNode<T>> nodo = raiz;
    shared_ptr<BNode<T>> padre = nullptr;

    while (nodo) {
        padre = nodo;
        int i = 0;
        while (i < nodo->clavesUsadas && clave.valor > nodo->claves[i].valor) i++;
        if (i < nodo->clavesUsadas && clave.valor == nodo->claves[i].valor) return false;
        nodo = nodo->hijos[i];
    }

    insertarRec(clave, padre, nullptr, nullptr);
    return true;
}

template<typename T>
void BTree<T>::insertarRec(const Clave<T>& clave, shared_ptr<BNode<T>> nodo, shared_ptr<BNode<T>> hijo1, shared_ptr<BNode<T>> hijo2) {
    Clave<T> sube = clave;
    shared_ptr<BNode<T>> izquierda = hijo1, derecha = hijo2;

    while (true) {
        int i = 0;
        while (i < nodo->clavesUsadas && sube.valor > nodo->claves[i].valor) i++;

        // Mover claves e hijos a la derecha
        for (int j = nodo->clavesUsadas; j > i; --j)
            nodo->claves[j] = nodo->claves[j - 1];
        for (int j = nodo->clavesUsadas + 1; j > i + 1; --j)
            nodo->hijos[j] = nodo->hijos[j - 1];

        nodo->claves[i] = sube;
        nodo->hijos[i] = izquierda;
        nodo->hijos[i + 1] = derecha;
        nodo->clavesUsadas++;

        if (nodo->clavesUsadas < grado) return;

        // División
        int medio = grado / 2;
        sube = nodo->claves[medio];

        auto nuevo = make_shared<BNode<T>>(grado);
        nuevo->clavesUsadas = nodo->clavesUsadas - medio - 1;

        for (int j = 0; j < nuevo->clavesUsadas; ++j)
            nuevo->claves[j] = nodo->claves[medio + 1 + j];
        for (int j = 0; j <= nuevo->clavesUsadas; ++j)
            nuevo->hijos[j] = nodo->hijos[medio + 1 + j];

        nodo->clavesUsadas = medio;

        if (nodo == raiz) {
            raiz = make_shared<BNode<T>>(grado);
            raiz->claves[0] = sube;
            raiz->clavesUsadas = 1;
            raiz->hijos[0] = nodo;
            raiz->hijos[1] = nuevo;
            return;
        }

        // Continuar hacia arriba
        nodo = nodo->padre.lock();
        izquierda = hijo1 = nodo->hijos[0]; // ya no se necesita exactitud aquí
        derecha = nuevo;
    }
}

template<typename T>
long BTree<T>::buscar(const T& valor) {
    shared_ptr<BNode<T>> nodo = raiz;

    while (nodo) {
        int i = 0;
        while (i < nodo->clavesUsadas && valor > nodo->claves[i].valor) i++;
        if (i < nodo->clavesUsadas && valor == nodo->claves[i].valor)
            return nodo->claves[i].registro;
        nodo = nodo->hijos[i];
    }
    return -1;
}

template<typename T>
void BTree<T>::mostrar() {
    cout << "Árbol B:" << endl;
    ver(raiz);
    cout << "------------------" << endl;
}

template<typename T>
void BTree<T>::ver(shared_ptr<BNode<T>> nodo, int nivel) {
    if (!nodo) return;
    cout << string(nivel * 2, ' ');
    for (int i = 0; i < nodo->clavesUsadas; i++) {
        cout << nodo->claves[i].valor << " ";
    }
    cout << endl;
    for (int i = 0; i <= nodo->clavesUsadas; i++) {
        ver(nodo->hijos[i], nivel + 1);
    }
}
