#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <bitset>
using namespace std;

struct nodo {
    float prob;
    char letra;
    nodo *izq;
    nodo *der;

    nodo(char c, float p) : letra(c), prob(p), izq(nullptr), der(nullptr) {}
};

struct Comparador {
    bool operator()(nodo* a, nodo* b) {
        return a->prob > b->prob;
    }
};

int menu();
void mostrarArbolHuffman(nodo* raiz, int espacio = 0);
nodo* construirArbolHuffman(vector<nodo *> &vec);
void insertarNodo(vector<nodo *> &nodos);
void leerArchivoYConstruirHuffman(const string &nombreArchivo, nodo*& raiz);
void generarTablaCodigos(nodo* raiz, unordered_map<char, string> &tabla, string codigo = "");
void comprimirArchivo(const string &archivoOriginal, const string &archivoComprimido, nodo* raiz);
void descomprimirArchivo(const string &archivoComprimido, const string &archivoRecuperado, nodo* raiz);
void calcularMetricas(const string &archivoOriginal, const string &archivoComprimido);

int main() {
    int op;
    vector<nodo *> nodos;
    nodo* raiz = nullptr;

    do {
        op = menu();
        switch (op) {
            case 1:
                insertarNodo(nodos);
                break;
            case 2:
                raiz = construirArbolHuffman(nodos);
                cout << "\nArbol de Huffman generado con los nodos insertados:" << endl;
                mostrarArbolHuffman(raiz);
                break;
            case 3:
                leerArchivoYConstruirHuffman("texto.txt", raiz);
                break;
            case 4:
                if (raiz) {
                    comprimirArchivo("texto.txt", "texto_comprimido.huff", raiz);
                } else {
                    cout << "Primero genera un Arbol de Huffman." << endl;
                }
                break;
            case 5:
                if (raiz) {
                    descomprimirArchivo("texto_comprimido.huff", "texto_recuperado.txt", raiz);
                } else {
                    cout << "Primero genera un Arbol de Huffman." << endl;
                }
                break;
            case 6:
                if (raiz) {
                    calcularMetricas("texto.txt", "texto_comprimido.huff");
                } else {
                    cout << "Primero genera un Arbol de Huffman." << endl;
                }
                break;
            case 7:
                cout << "Saliendo..." << endl;
                break;
            default:
                cout << "Opcion no valida." << endl;
                break;
        }
    } while (op != 7);

    return 0;
}

int menu() {
    int opt = 0;
    cout << "\n*** Menu ***" << endl;
    cout << "1. Insertar nodo" << endl;
    cout << "2. Ordenar nodos y mostrar Arbol de Huffman" << endl;
    cout << "3. Leer archivo y construir Arbol de Huffman" << endl;
    cout << "4. Comprimir archivo" << endl;
    cout << "5. Descomprimir archivo" << endl;
    cout << "6. Calcular metricas" << endl;
    cout << "7. Salir" << endl;
    cout << "Selecciona tu opcion: ";
    cin >> opt;
    return opt;
}

void insertarNodo(vector<nodo *> &nodos) {
    char letra;
    float prob;
    cout << "Ingresa la letra: ";
    cin >> letra;
    cout << "Ingresa la probabilidad: ";
    cin >> prob;
    nodo* nuevoNodo = new nodo(letra, prob);
    nodos.push_back(nuevoNodo);
    cout << "Nodo insertado correctamente." << endl;
}

nodo* construirArbolHuffman(vector<nodo *> &vec) {
    priority_queue<nodo*, vector<nodo*>, Comparador> pq;
    for (nodo* n : vec) {
        pq.push(n);
    }
    while (pq.size() > 1) {
        nodo* izquierdo = pq.top(); pq.pop();
        nodo* derecho = pq.top(); pq.pop();
        nodo* nuevoNodo = new nodo(' ', izquierdo->prob + derecho->prob);
        nuevoNodo->izq = izquierdo;
        nuevoNodo->der = derecho;
        pq.push(nuevoNodo);
    }
    return pq.top();
}

void mostrarArbolHuffman(nodo* raiz, int espacio) {
    if (raiz == nullptr) return;
    espacio += 10;
    mostrarArbolHuffman(raiz->der, espacio);
    cout << endl;
    for (int i = 10; i < espacio; i++) {
        cout << " ";
    }
    cout << raiz->prob << " (" << raiz->letra << ")" << endl;
    mostrarArbolHuffman(raiz->izq, espacio);
}

void leerArchivoYConstruirHuffman(const string &nombreArchivo, nodo*& raiz) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo." << endl;
        return;
    }
    unordered_map<char, int> freqMap;
    char c;
    while (archivo.get(c)) {
        freqMap[c]++;
    }
    archivo.close();

    vector<nodo *> nodos;
    for (const auto &pair : freqMap) {
        nodos.push_back(new nodo(pair.first, pair.second));
    }

    raiz = construirArbolHuffman(nodos);
    cout << "\nArbol de Huffman generado desde el archivo:" << endl;
    mostrarArbolHuffman(raiz);
}

void generarTablaCodigos(nodo* raiz, unordered_map<char, string> &tabla, string codigo) {
    if (!raiz) return;
    if (raiz->letra != ' ') {
        tabla[raiz->letra] = codigo;
    }
    generarTablaCodigos(raiz->izq, tabla, codigo + "0");
    generarTablaCodigos(raiz->der, tabla, codigo + "1");
}

void comprimirArchivo(const string &archivoOriginal, const string &archivoComprimido, nodo* raiz) {
    unordered_map<char, string> tablaCodigos;
    generarTablaCodigos(raiz, tablaCodigos);

    ifstream archivoEntrada(archivoOriginal, ios::binary);
    ofstream archivoSalida(archivoComprimido, ios::binary);

    if (!archivoEntrada.is_open() || !archivoSalida.is_open()) {
        cout << "Error al abrir los archivos." << endl;
        return;
    }

    stringstream ss;
    char c;
    while (archivoEntrada.get(c)) {
        ss << tablaCodigos[c];
    }

    string contenidoCodificado = ss.str();
    archivoSalida << contenidoCodificado;

    archivoEntrada.close();
    archivoSalida.close();

    cout << "Archivo comprimido correctamente." << endl;
}

void descomprimirArchivo(const string &archivoComprimido, const string &archivoRecuperado, nodo* raiz) {
    ifstream archivoEntrada(archivoComprimido, ios::binary);
    ofstream archivoSalida(archivoRecuperado, ios::binary);

    if (!archivoEntrada.is_open() || !archivoSalida.is_open()) {
        cout << "Error al abrir los archivos." << endl;
        return;
    }

    nodo* actual = raiz;
    stringstream contenidoCodificado;
    char bit;
    while (archivoEntrada.get(bit)) {
        contenidoCodificado << bit;
    }

    string contenido = contenidoCodificado.str();
    for (char bit : contenido) {
        actual = (bit == '0') ? actual->izq : actual->der;
        if (!actual->izq && !actual->der) {
            archivoSalida.put(actual->letra);
            actual = raiz;
        }
    }

    archivoEntrada.close();
    archivoSalida.close();

    cout << "Archivo descomprimido correctamente." << endl;
}

void calcularMetricas(const string &archivoOriginal, const string &archivoComprimido) {
    ifstream archivoO(archivoOriginal, ios::binary | ios::ate);
    ifstream archivoC(archivoComprimido, ios::binary | ios::ate);

    if (!archivoO.is_open() || !archivoC.is_open()) {
        cout << "Error al abrir los archivos para metricas." << endl;
        return;
    }

    size_t tamOriginal = archivoO.tellg();
    size_t tamComprimido = archivoC.tellg();
    double factorCompresion = (double)tamOriginal / tamComprimido;

    cout << "Tamano original: " << tamOriginal << " bytes" << endl;
    cout << "Tamano comprimido: " << tamComprimido << " bytes" << endl;
    cout << "Factor de compresion: " << factorCompresion << endl;

    archivoO.close();
    archivoC.close();
}
