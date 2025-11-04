#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;

// Parámetros del problema
const int NUM_OBJETOS = 10;
const int CAPACIDAD_MAX = 35;

// Parámetros del algoritmo
const int NUM_MURCIELAGOS = 20;
const int MAX_ITER = 100;
const double FREQ_MIN = 0.0;
const double FREQ_MAX = 2.0;
const double ALPHA = 0.9;   // reducción del volumen
const double GAMMA = 0.9;   // incremento del pulso

// Datos del problema (valores y pesos)
int valores[NUM_OBJETOS]  = {10, 5, 15, 7, 6, 18, 3, 12, 8, 9};
int pesos[NUM_OBJETOS]    = {2, 3, 5, 7, 1, 4, 1, 6, 3, 2};

// Función para evaluar el fitness de una solución
double fitness(const vector<int>& solucion) {
    int peso_total = 0, valor_total = 0;
    for (int i = 0; i < NUM_OBJETOS; i++) {
        peso_total += pesos[i] * solucion[i];
        valor_total += valores[i] * solucion[i];
    }
    if (peso_total > CAPACIDAD_MAX) return 0; // penalización
    return valor_total;
}

// Generar un número aleatorio 0,1
double rand01() {
    return (double)rand() / RAND_MAX;
}

int main() {
    srand(time(0));

    vector<vector<int>> murcielagos(NUM_MURCIELAGOS, vector<int>(NUM_OBJETOS));
    vector<vector<double>> velocidad(NUM_MURCIELAGOS, vector<double>(NUM_OBJETOS, 0.0));
    vector<double> frecuencia(NUM_MURCIELAGOS, 0.0);
    vector<double> loudness(NUM_MURCIELAGOS, 1.0);
    vector<double> pulse_rate(NUM_MURCIELAGOS, 0.5);
    vector<double> fitness_val(NUM_MURCIELAGOS, 0.0);

    // Inicialización aleatoria
    for (int i = 0; i < NUM_MURCIELAGOS; i++) {
        for (int j = 0; j < NUM_OBJETOS; j++) {
            murcielagos[i][j] = rand() % 2;
        }
        fitness_val[i] = fitness(murcielagos[i]);
    }

    // Encontrar el mejor inicial
    int best_index = 0;
    for (int i = 1; i < NUM_MURCIELAGOS; i++)
        if (fitness_val[i] > fitness_val[best_index]) best_index = i;
    vector<int> best_solution = murcielagos[best_index];
    double best_fitness = fitness_val[best_index];

    // Iteraciones principales
    for (int t = 0; t < MAX_ITER; t++) {
        for (int i = 0; i < NUM_MURCIELAGOS; i++) {

            // 1. Actualizar frecuencia
            frecuencia[i] = FREQ_MIN + (FREQ_MAX - FREQ_MIN) * rand01();

            // 2. Actualizar velocidad
            for (int j = 0; j < NUM_OBJETOS; j++) {
                velocidad[i][j] += (murcielagos[i][j] - best_solution[j]) * frecuencia[i];
            }

            // 3. Actualizar posición (usando función sigmoide)
            for (int j = 0; j < NUM_OBJETOS; j++) {
                double S = 1.0 / (1.0 + exp(-velocidad[i][j]));
                if (rand01() < S)
                    murcielagos[i][j] = 1 - murcielagos[i][j]; // flip bit
            }

            // 4. Generar una solución local con cierta probabilidad
            if (rand01() > pulse_rate[i]) {
                for (int j = 0; j < NUM_OBJETOS; j++) {
                    if (rand01() < 0.1) murcielagos[i][j] = 1 - murcielagos[i][j];
                }
            }

            // 5. Evaluar
            double f_new = fitness(murcielagos[i]);

            // 6. Aceptar solución si mejora
            if (f_new > fitness_val[i] && rand01() < loudness[i]) {
                fitness_val[i] = f_new;
                loudness[i] *= ALPHA;
                pulse_rate[i] = pulse_rate[i] * (1 - exp(-GAMMA * t));
            }

            // Actualizar mejor global
            if (f_new > best_fitness) {
                best_fitness = f_new;
                best_solution = murcielagos[i];
            }
        }
    }

    // Mostrar resultado final
    cout << "Mejor valor encontrado: " << best_fitness << endl;
    cout << "Objetos seleccionados: ";
    for (int i = 0; i < NUM_OBJETOS; i++)
        if (best_solution[i]) cout << i + 1 << " ";
    cout << endl;

    return 0;
}
