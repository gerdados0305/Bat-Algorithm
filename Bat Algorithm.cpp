#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
using namespace std;

// ---------------------------------------------------------
// 🔹 Definición del problema de la mochila
// ---------------------------------------------------------
const int NUM_OBJETOS = 10;
const int CAPACIDAD_MAX = 35;

int valores[NUM_OBJETOS] = {10, 5, 15, 7, 6, 18, 3, 12, 8, 9};
int pesos[NUM_OBJETOS]   = {2, 5, 7, 1, 4, 6, 3, 6, 3, 2};

double fitness(const vector<int>& solucion) {
    int peso_total = 0;
    int valor_total = 0;

    for (int i = 0; i < NUM_OBJETOS; i++) {
        peso_total += pesos[i] * solucion[i];
        valor_total += valores[i] * solucion[i];
    }

    // penalización si se pasa de capacidad
    if (peso_total > CAPACIDAD_MAX)
        return 0;

    return valor_total;
}

// ---------------------------------------------------------
// 🔹 Parámetros del Algoritmo de Murciélago
// ---------------------------------------------------------
const int NUM_MURCIELAGOS = 20;
const int MAX_ITER = 100;
const double FREQ_MIN = 0.0;
const double FREQ_MAX = 2.0;
const double ALPHA = 0.9;
const double GAMMA = 0.9;

double rand01() { return (double)rand() / RAND_MAX; }

// ---------------------------------------------------------
// 🔹 Función para binarizar una solución continua (0-1)
// ---------------------------------------------------------
int binarize(double value) {
    return (rand01() < 1.0 / (1.0 + exp(-10 * (value - 0.5)))) ? 1 : 0;
}

// ---------------------------------------------------------
// 🔹 Programa principal
// ---------------------------------------------------------
int main() {
    srand(time(0));

    vector<vector<double>> posiciones(NUM_MURCIELAGOS, vector<double>(NUM_OBJETOS, 0.0));
    vector<vector<double>> velocidades(NUM_MURCIELAGOS, vector<double>(NUM_OBJETOS, 0.0));
    vector<double> frecuencia(NUM_MURCIELAGOS, 0.0);
    vector<double> loudness(NUM_MURCIELAGOS, 1.0);
    vector<double> pulse_rate(NUM_MURCIELAGOS, 0.5);
    vector<double> fitness_val(NUM_MURCIELAGOS, 0.0);

    // Inicializar murciélagos con soluciones aleatorias
    for (int i = 0; i < NUM_MURCIELAGOS; i++) {
        for (int j = 0; j < NUM_OBJETOS; j++) {
            posiciones[i][j] = rand01(); // posición continua
        }

        // Convertir a binario para evaluar
        vector<int> sol(NUM_OBJETOS);
        for (int j = 0; j < NUM_OBJETOS; j++)
            sol[j] = binarize(posiciones[i][j]);

        fitness_val[i] = fitness(sol);
    }

    // Mejor solución inicial
    int best_index = 0;
    double best_fitness = fitness_val[0];
    vector<double> best_pos = posiciones[0];

    for (int i = 1; i < NUM_MURCIELAGOS; i++) {
        if (fitness_val[i] > best_fitness) {
            best_fitness = fitness_val[i];
            best_pos = posiciones[i];
        }
    }

    // -----------------------------------------------------
    // 🔹 Bucle principal del algoritmo
    // -----------------------------------------------------
    for (int t = 0; t < MAX_ITER; t++) {
        for (int i = 0; i < NUM_MURCIELAGOS; i++) {
            frecuencia[i] = FREQ_MIN + (FREQ_MAX - FREQ_MIN) * rand01();

            // Actualizar velocidad y posición
            for (int j = 0; j < NUM_OBJETOS; j++) {
                velocidades[i][j] += (posiciones[i][j] - best_pos[j]) * frecuencia[i];
                posiciones[i][j] += velocidades[i][j];

                // mantener valores en [0,1]
                if (posiciones[i][j] < 0) posiciones[i][j] = 0;
                if (posiciones[i][j] > 1) posiciones[i][j] = 1;
            }

            // Nueva solución local aleatoria
            if (rand01() > pulse_rate[i]) {
                for (int j = 0; j < NUM_OBJETOS; j++) {
                    posiciones[i][j] = best_pos[j] + 0.001 * rand01();
                    if (posiciones[i][j] < 0) posiciones[i][j] = 0;
                    if (posiciones[i][j] > 1) posiciones[i][j] = 1;
                }
            }

            // Convertir a binario
            vector<int> sol(NUM_OBJETOS);
            for (int j = 0; j < NUM_OBJETOS; j++)
                sol[j] = binarize(posiciones[i][j]);

            double new_fitness = fitness(sol);

            // Aceptar la nueva solución si mejora o con cierta probabilidad
            if (new_fitness >= fitness_val[i] && rand01() < loudness[i]) {
                fitness_val[i] = new_fitness;
                loudness[i] *= ALPHA;
                pulse_rate[i] = pulse_rate[i] * (1 - exp(-GAMMA * t));
            }

            // Actualizar mejor solución global
            if (new_fitness > best_fitness) {
                best_fitness = new_fitness;
                best_pos = posiciones[i];
            }
        }
    }

    // -----------------------------------------------------
    // 🔹 Resultado final
    // -----------------------------------------------------
    vector<int> best_sol(NUM_OBJETOS);
    for (int j = 0; j < NUM_OBJETOS; j++)
        best_sol[j] = binarize(best_pos[j]);

    int total_peso = 0;
    int total_valor = 0;
    cout << "\n===== RESULTADOS FINALES =====" << endl;
    cout << "Objetos seleccionados: ";

    for (int j = 0; j < NUM_OBJETOS; j++) {
        cout << best_sol[j] << " ";
        total_peso += pesos[j] * best_sol[j];
        total_valor += valores[j] * best_sol[j];
    }

    cout << "\nPeso total: " << total_peso;
    cout << "\nValor total: " << total_valor;
    cout << "\nFitness (valor máximo): " << best_fitness << endl;
    cout << "===================================" << endl;

    return 0;
}
