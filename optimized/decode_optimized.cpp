#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <array>
#include <thread>
#include <mutex>

constexpr int SEQ_LENGTH = 1023;
constexpr int SAT_COUNT = 24;
constexpr int SHIFT_REGISTER_SIZE = 10;

using Signal = std::vector<int>;
using GoldCode = std::array<int, SEQ_LENGTH>;
using GoldCodeArray = std::array<GoldCode, SAT_COUNT>;

std::mutex coutMutex;

/// Liest eine Datei ein und speichert den Inhalt als String
void readFile(const std::string& fileName, std::string& fileContent) {
    std::ifstream file(fileName);
    if (file.is_open()) {
        std::getline(file, fileContent);
        file.close();
    } else {
        std::cerr << "Fehler: Datei konnte nicht geöffnet werden!" << std::endl;
    }
}

/// Konvertiert eine Zeile aus der Datei in eine Zahlenfolge (Signalvektor)
Signal parseLineToVector(const std::string& line) {
    Signal sequence;
    std::stringstream lineStream(line);
    int number;
    while (lineStream >> number) {
        sequence.push_back(number);
    }
    return sequence;
}

/// Kombinierte Funktion zum Einlesen und Umwandeln der Signaldatei
Signal getSignal(const std::string& file) {
    std::string content;
    readFile(file, content);
    return parseLineToVector(content);
}

/// Zyklisches Verschieben eines 10-Bit-Schieberegisters um eine Position
void shiftSequenceByOne(std::array<int, SHIFT_REGISTER_SIZE>& sequence) {
    int temp = sequence.back();
    for (int i = SHIFT_REGISTER_SIZE - 1; i > 0; --i) {
        sequence[i] = sequence[i - 1];
    }
    sequence[0] = temp;
}

/// Erzeugt einen Gold-Code für einen Satelliten mit gegebenen TAP-Paaren
void createGoldCodeForSat(GoldCode& goldCode, int x, int y) {
    std::array<int, SHIFT_REGISTER_SIZE> sequence1{1,1,1,1,1,1,1,1,1,1};
    std::array<int, SHIFT_REGISTER_SIZE> sequence2{1,1,1,1,1,1,1,1,1,1};

    for (int i = 0; i < SEQ_LENGTH; ++i) {
        int temp2 = sequence2[x - 1] ^ sequence2[y - 1];
        int new2 = sequence2[9] ^ sequence2[8] ^ sequence2[7] ^ sequence2[5] ^ sequence2[2] ^ sequence2[1];
        int new1 = sequence1[9] ^ sequence1[2];

        goldCode[i] = (temp2 ^ sequence1[9]) == 0 ? -1 : 1;

        shiftSequenceByOne(sequence1);
        shiftSequenceByOne(sequence2);
        sequence2[0] = new2;
        sequence1[0] = new1;
    }
}

/// Erstellt Gold-Codes für alle 24 Satelliten mit spezifischen Tap-Kombinationen
void createGoldCodes(GoldCodeArray& goldCodes) {
    const std::array<std::pair<int, int>, SAT_COUNT> tapPairs {{
        {2, 6}, {3, 7}, {4, 8}, {5, 9}, {1, 9}, {2, 10},
        {1, 8}, {2, 9}, {3, 10}, {2, 3}, {3, 4}, {5, 6},
        {6, 7}, {7, 8}, {8, 9}, {9, 10}, {1, 4}, {2, 5},
        {3, 6}, {4, 7}, {5, 8}, {6, 9}, {1, 3}, {4, 6}
    }};

    for (int i = 0; i < SAT_COUNT; ++i) {
        createGoldCodeForSat(goldCodes[i], tapPairs[i].first, tapPairs[i].second);
    }
}

/// Berechnet das Skalarprodukt (Korrelation) zwischen Signal und Gold-Code bei gegebener Verschiebung
float calculateScalar(const Signal& signal, const GoldCode& goldCode, int delta) {
    float scalar = 0.0f;
    for (int i = 0; i < SEQ_LENGTH; ++i) {
        scalar += signal[i] * goldCode[(i + delta) % SEQ_LENGTH];
    }
    return scalar;
}

/// Führt die Verarbeitung eines Satelliten in einem eigenen Thread aus
void interpretSingleSatellite(int satIndex, const Signal& signal, const GoldCodeArray& goldCodes,
                              float upperThreshold, float lowerThreshold) {
    for (int delta = 0; delta < SEQ_LENGTH + 1; ++delta) {
        float scalar = calculateScalar(signal, goldCodes[satIndex], delta);

        if (scalar >= upperThreshold || scalar <= lowerThreshold) {
            int bit = scalar >= upperThreshold ? 1 : 0;
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "Satellite " << (satIndex + 1)
                      << " has sent bit " << bit
                      << " (delta: " << delta << ")\n";
            return;
        }
    }
}

/// Interpretiert das GPS-Summensignal und identifiziert gesendete Bits der Satelliten (multithreaded)
void interpretSignal(const Signal& signal, const GoldCodeArray& goldCodes) {
    float thresholdMargin = 200.0f;
    float upperThreshold = SEQ_LENGTH - thresholdMargin;
    float lowerThreshold = -SEQ_LENGTH + thresholdMargin;

    std::vector<std::thread> threads;

    for (int satIndex = 0; satIndex < SAT_COUNT; ++satIndex) {
        threads.emplace_back(interpretSingleSatellite, satIndex, std::ref(signal), std::ref(goldCodes),
                             upperThreshold, lowerThreshold);
    }

    for (auto& t : threads) {
        t.join();
    }
}

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "Fehler: falsche Argumente" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    Signal signal = getSignal(fileName);

    GoldCodeArray goldCodes;
    createGoldCodes(goldCodes);
    interpretSignal(signal, goldCodes);

    return 0;
}
