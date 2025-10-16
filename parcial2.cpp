//Juan David Vasquez Pomar Parcial 2 Sistemas Operativos 
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

class Bloque {
public:
    string proceso;
    int tamano;
    bool libre;

    Bloque(string p, int t, bool l) {
        proceso = p;
        tamano = t;
        libre = l;
    }
};

class Memoria {
public:
    vector<Bloque> bloques;
    int total;

    Memoria(int t) {
        total = t;
        bloques.push_back(Bloque("Libre", t, true));
    }

    // Fusiona bloques libres contiguos
    void fusionarHuecos() {
        vector<Bloque> nueva;
        int i = 0;
        while (i < bloques.size()) {
            if (!nueva.empty() && nueva[nueva.size() - 1].libre && bloques[i].libre) {
                nueva[nueva.size() - 1].tamano = nueva[nueva.size() - 1].tamano + bloques[i].tamano;
            } else {
                nueva.push_back(bloques[i]);
            }
            i++;
        }
        bloques = nueva;
    }

    // Muestra el estado de la memoria en el formato requerido
    void mostrar() {
        int i = 0;
        while (i < bloques.size()) {
            cout << "[" << bloques[i].proceso << ":" << bloques[i].tamano << "]";
            i++;
        }
        cout << endl;
        cout << "Fragmentacion interna: " << fragmentacionInterna() << " unidades" << endl;
        cout << "Fragmentacion externa: " << fragmentacionExterna() << " unidades" << endl;
    }

    // Asigna memoria según la política seleccionada
    void asignar(string proceso, int tam, string politica) {
        int pos = -1;
        int i = 0;

        if (politica == "FIRST") {
            // First Fit: primer bloque que entre
            while (i < bloques.size()) {
                if (bloques[i].libre && bloques[i].tamano >= tam && pos == -1) {
                    pos = i;  // Solo asignar si aún no se ha encontrado (pos == -1)
                }
                i++;
            }
        }
        else if (politica == "BEST") {
            // Best Fit: bloque más pequeño que entre
            int mejor = 999999;
            while (i < bloques.size()) {
                if (bloques[i].libre && bloques[i].tamano >= tam && bloques[i].tamano < mejor) {
                    mejor = bloques[i].tamano;
                    pos = i;
                }
                i++;
            }
        }
        else if (politica == "WORST") {
            // Worst Fit: bloque más grande disponible
            int peor = -1;
            while (i < bloques.size()) {
                if (bloques[i].libre && bloques[i].tamano >= tam && bloques[i].tamano > peor) {
                    peor = bloques[i].tamano;
                    pos = i;
                }
                i++;
            }
        }

        if (pos == -1) {
            cout << "ERROR: No hay suficiente memoria para " << proceso << " (" << tam << " unidades)" << endl;
        } else {
            int sobrante = bloques[pos].tamano - tam;
            bloques[pos] = Bloque(proceso, tam, false);
            if (sobrante > 0) {
                bloques.insert(bloques.begin() + pos + 1, Bloque("Libre", sobrante, true));
            }
            cout << "Proceso " << proceso << " asignado correctamente" << endl;
        }
    }

    // Libera la memoria de un proceso
    void liberar(string proceso) {
        bool encontrado = false;
        int i = 0;
        while (i < bloques.size()) {
            if (!bloques[i].libre && bloques[i].proceso == proceso && encontrado == false) {
                bloques[i].proceso = "Libre";
                bloques[i].libre = true;
                encontrado = true;
            }
            i++;
        }

        if (encontrado == false) {
            cout << "ERROR: No se encontro el proceso " << proceso << endl;
        } else {
            cout << "Proceso " << proceso << " liberado correctamente" << endl;
            fusionarHuecos();
        }
    }


    int fragmentacionInterna() {
        return 0;
    }

    // Calcula fragmentación externa (suma de huecos pequeños que no pueden usarse)
    int fragmentacionExterna() {
        int totalLibre = 0;
        int mayorHueco = 0;
        int i = 0;

        while (i < bloques.size()) {
            if (bloques[i].libre) {
                totalLibre = totalLibre + bloques[i].tamano;
                if (bloques[i].tamano > mayorHueco) {
                    mayorHueco = bloques[i].tamano;
                }
            }
            i++;
        }

        return totalLibre - mayorHueco;
    }
};


void procesarComandos(Memoria& mem, string politica, istream& entrada) {
    string linea;
    while (getline(entrada, linea)) {
        if (linea.empty() || linea[0] == '#') continue;  // Ignorar líneas vacías y comentarios

        stringstream ss(linea);
        char operacion;
        ss >> operacion;

        if (operacion == 'A') {
            string proceso;
            int tamano;
            ss >> proceso >> tamano;
            mem.asignar(proceso, tamano, politica);
        }
        else if (operacion == 'L') {
            string proceso;
            ss >> proceso;
            mem.liberar(proceso);
        }
        else if (operacion == 'M') {
            mem.mostrar();
        }
        else {
            cout << "Comando desconocido: " << operacion << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    // memoria por default 100
    int tamanoMemoria = 100;
    string politica = "FIRST";
    string archivoEntrada = "";

    // leer cmd 
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-m" && i + 1 < argc) {
            tamanoMemoria = stoi(argv[++i]);
        }
        else if (arg == "-p" && i + 1 < argc) {
            politica = argv[++i];
        }
        else if (arg == "-f" && i + 1 < argc) {
            archivoEntrada = argv[++i];
        }
    }

    cout << "=== SIMULADOR DE ASIGNACION DE MEMORIA ===" << endl;
    cout << "Tamano de memoria: " << tamanoMemoria << " unidades" << endl;
    cout << "Politica: " << politica << endl;
    cout << "===========================================" << endl << endl;

    Memoria mem(tamanoMemoria);

    // Procesar desde archivo o consola
    if (!archivoEntrada.empty()) {
        ifstream archivo(archivoEntrada);
        if (!archivo.is_open()) {
            cerr << "Error: No se pudo abrir el archivo " << archivoEntrada << endl;
            return 1;
        }
        cout << "Leyendo comandos desde: " << archivoEntrada << endl << endl;
        procesarComandos(mem, politica, archivo);
        archivo.close();
    } else {
        cout << "Modo interactivo. Comandos disponibles:" << endl;
        cout << "  A <proceso> <tamano>  - Asignar memoria" << endl;
        cout << "  L <proceso>           - Liberar memoria" << endl;
        cout << "  M                     - Mostrar estado" << endl;
        cout << "  (Ctrl+D o Ctrl+Z para terminar)" << endl << endl;
        procesarComandos(mem, politica, cin);
    }

    return 0;
}

