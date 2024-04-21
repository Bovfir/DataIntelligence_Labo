#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>


#define MAX_NUMBERS 1803
#define MAX_LINES 601
#define MAX_PARAMS_CALCUL_ACC 3
#define MAX_FILES 15
#define MAX_SUB 24
#define MAX_TRAINSET_NUMBERS 22
#define MAX_TESTSET_NUMBERS 2
#define MAX_CHAR_NAME_MOVEMENTS 50

#define FILE_PATH "data/A_DeviceMotion_data/%s/sub_%d.csv" 
#define TRAINSET_NAME "trainSet.csv"
#define TESTSET_NAME "testSet.csv"

typedef struct subStats SubStats;
struct subStats
{
    int gender;
    int index;
    double stats[MAX_LINES];
};


void extractGender(double* last_numbers);
SubStats subProcessing(char fileNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], int iFile, int iSub);
void extractLastThreeNumbers(double numbers[MAX_NUMBERS], char fileNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], int iFile, int iSub);
void obtainingVectors(double numbers[MAX_NUMBERS], double sum[MAX_LINES]);
double calculAccelerationVector(double calcul[MAX_PARAMS_CALCUL_ACC]);
void printCSV(SubStats** data, char fileNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], int cellNumbers, char fileName[MAX_CHAR_NAME_MOVEMENTS]);
// Gestion de l'aleatoire
void generateRandomSubs(int picked_numbers[MAX_TRAINSET_NUMBERS], int is_picked[MAX_SUB], int missingNumbers[MAX_TESTSET_NUMBERS]);
void findMissingNumbers(int picked_numbers[MAX_TRAINSET_NUMBERS], int missingNumbers[MAX_TESTSET_NUMBERS]);
void resetTable(int tab[], int cellNumbers);
void tableSorting(int tab[], int cellNumbers);


void main(void) {
    char fileNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS] = { "dws_1","dws_2","dws_11","jog_9","jog_16","sit_5","sit_13","std_6","std_14","ups_3","ups_4","ups_12","wlk_7","wlk_8","wlk_15" };
    double genderNumber[MAX_SUB];
    int testSetNumbers[MAX_FILES][MAX_TESTSET_NUMBERS] = { 0 };
    int trainSetNumbers[MAX_FILES][MAX_TRAINSET_NUMBERS] = { 0 };
    int is_picked[MAX_SUB] = { 0 };  // Tableau pour suivre les nombres deja  choisis
    srand((unsigned int)time(NULL)); // Initialiser le generateur de nombres aleatoires

    // Allocation de memoire pour les data car grande quantite  de donnee traitee 
    SubStats** trainSet = malloc(MAX_FILES * sizeof(SubStats*));
    SubStats** testSet = malloc(MAX_FILES * sizeof(SubStats*));
    if (trainSet == NULL || testSet == NULL) {
        printf("1.Erreur d'allocation de memoire\n");
    }
    else {
        //Extraire les numeros de genre selon les index
        extractGender(genderNumber);

        // Traiter les dossiers et les subs 
        for (int iFile = 0; iFile < MAX_FILES; iFile++) {

            // Allocation de memoire pour chaque Sub d'un dossier
            trainSet[iFile] = malloc(MAX_TRAINSET_NUMBERS * sizeof(SubStats));
            testSet[iFile] = malloc(MAX_TESTSET_NUMBERS * sizeof(SubStats));
            if (trainSet[iFile] == NULL || testSet[iFile] == NULL) {
                printf("2.Erreur d'allocation de memoire\n");
            }
            else {

                // Generer aleatoirement les numeros des Sub du TrainSet et TestSet
                generateRandomSubs(trainSetNumbers[iFile], is_picked, testSetNumbers[iFile]);
                findMissingNumbers(trainSetNumbers[iFile], testSetNumbers[iFile]);
                resetTable(is_picked, MAX_SUB);

                //Traitement des TrainSet et TestSet
                for (int iSub = 0; iSub < MAX_TRAINSET_NUMBERS; iSub++) {
                    trainSet[iFile][iSub] = subProcessing(fileNames, iFile, trainSetNumbers[iFile][iSub]);
                    trainSet[iFile][iSub].gender = (int)genderNumber[trainSetNumbers[iFile][iSub] - 1];
                }
                for (int iSub = 0; iSub < MAX_TESTSET_NUMBERS; iSub++) {
                    testSet[iFile][iSub] = subProcessing(fileNames, iFile, testSetNumbers[iFile][iSub]);
                    testSet[iFile][iSub].gender = (int)genderNumber[testSetNumbers[iFile][iSub] - 1];
                }
            }
        }

        // Imprimer les resultats dans un csv
        printCSV(testSet, fileNames, MAX_TESTSET_NUMBERS, TESTSET_NAME);
        printCSV(trainSet, fileNames, MAX_TRAINSET_NUMBERS, TRAINSET_NAME);

        // Liberer la memoire apres manipulation 
        for (int i = 0; i < MAX_FILES; i++) {
            free(trainSet[i]);
            free(testSet[i]);
        }
        free(trainSet);
        free(testSet);
        printf("Traitement termine");
    }
}


void extractGender(double* last_numbers) {
    FILE* file = fopen("data_subjects_info.csv", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
    }
    else {
        char line[1024];
        int i = 0;
        // Sortir la première ligne des titres
        fgets(line, sizeof(line), file);

        while (fgets(line, sizeof(line), file)) {
            sscanf_s(line, "%*f,%*f,%*f,%*f,%lf", &last_numbers[i]);
            i++;
        }
        fclose(file);
    }
}

SubStats subProcessing(char fileNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], int iFile, int iSub) {
    SubStats subStats;
    double* numbers = calloc(MAX_NUMBERS, sizeof(double));

    extractLastThreeNumbers(numbers,fileNames,iFile,iSub);

    subStats.index = iSub;

    obtainingVectors(numbers, subStats.stats);

    free(numbers);

    return subStats;
}

void extractLastThreeNumbers(double numbers[MAX_NUMBERS], char fileNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], int iFile, int iSub) {
    char filename[MAX_CHAR_NAME_MOVEMENTS];
    sprintf_s(filename, sizeof(filename), FILE_PATH, fileNames[iFile], iSub);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du file\n");
    }
    else {
            char line[1024];
            int i = 0;
            int lineNumber = 0;
            fgets(line, sizeof(line), file);

            while (fgets(line, sizeof(line), file) && lineNumber < MAX_LINES) {
                sscanf_s(line, "%*f,%*f,%*f,%*f,%*f,%*f,%*f,%*f,%*f,%*f,%lf,%lf,%lf", &numbers[i], &numbers[i + 1], &numbers[i + 2]);
                i += 3;
                lineNumber++;
            }
            fclose(file);
    }
}

void obtainingVectors(double numbers[MAX_NUMBERS], double sum[MAX_LINES]) {
    int iVariable = 1;
    double calcul[MAX_PARAMS_CALCUL_ACC];
    int iResult = 0;

    for (int iNumber = 0; iNumber < MAX_NUMBERS; iNumber++) {
        if (iVariable <= MAX_PARAMS_CALCUL_ACC) {
            calcul[iVariable - 1] = numbers[iNumber];

            if (iVariable == MAX_PARAMS_CALCUL_ACC) {
                sum[iResult] = calculAccelerationVector(calcul);
                iResult++;
                iVariable = 0;
            }
        }
        iVariable++;
    }
}

double calculAccelerationVector(double calcul[MAX_PARAMS_CALCUL_ACC]) {

    double sum = 0;

    for (int iVariable = 0; iVariable < 3; iVariable++) {
        sum += calcul[iVariable] * calcul[iVariable];
    }

    return sqrt(sum);
}

void printCSV(SubStats** data, char fileNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], int cellNumbers, char fileName[MAX_CHAR_NAME_MOVEMENTS]) {
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du file\n");
    }
    else {

        //  Ecrire l'en-tete du file CSV
        fprintf(file, "Mouvement,Gender,Index");
        for (int iLine = 0; iLine < MAX_LINES; iLine++) {
            fprintf(file, ",VaccAcc %d", iLine);
        }
        fprintf(file, "\n");

        //  Ecrire les données de data
        int index = 0;
        for (int iFile = 0; iFile < MAX_FILES; iFile++) {
            for (int iSub = 0; iSub < cellNumbers; iSub++) {
                fprintf(file, "%s,%d,%d", fileNames[iFile], data[iFile][iSub].gender, index+1);
                for (int iLine = 0; iLine < MAX_LINES && data[iFile][iSub].stats[iLine] != 0; iLine++) {
                    fprintf(file, ",%.17f", data[iFile][iSub].stats[iLine]);
                }
                index++;
                fprintf(file, "\n");
            }
        }

        fclose(file);
    }
}

// Gestion de l'aleatoire 

void generateRandomSubs(int picked_numbers[MAX_TRAINSET_NUMBERS], int is_picked[MAX_SUB], int missingNumbers[MAX_TESTSET_NUMBERS]) {
    int count = 0;
    // Si le tableau est déjà remplis, mettre en priorité les nombres dans le tableau de choisi
    if (missingNumbers[0] != 0 && missingNumbers[1] != 0) {
        picked_numbers[0] = missingNumbers[0];
        picked_numbers[1] = missingNumbers[1];
        is_picked[missingNumbers[0] - 1] = 1;
        is_picked[missingNumbers[1] - 1] = 1;
        count = 2;

    }
    for (int i = count; i < MAX_TRAINSET_NUMBERS; i++) {
        int num;
        if (i < count + 2 && missingNumbers[i - count] != 0) {
            num = missingNumbers[i - count];
        }
        else {
            do {
                num = rand() % MAX_SUB + 1;
            } while (is_picked[num - 1]);  // Continuer   generer des nombres jusqu'a  ce qu'on en trouve un qui n'a pas encore  ete  choisi
        }
        picked_numbers[i] = num;
        is_picked[num - 1] = 1;  // Marquer le nombre comme choisi
    }

    // Simple triage du tableau pour plus de simplicite  
    tableSorting(picked_numbers, MAX_TRAINSET_NUMBERS);
}

void findMissingNumbers(int picked_numbers[MAX_TRAINSET_NUMBERS], int missingNumbers[MAX_TESTSET_NUMBERS]) {
    int allNumbers[24] = { 0 };  // Initialiser le tableau de suivi
    int nbMissingNumbers = 0;

    // Marquer les nombres choisis dans le tableau de suivi
    for (int i = 0; i < MAX_TRAINSET_NUMBERS; i++) {
        allNumbers[picked_numbers[i] - 1] = 1;
    }

    
    for (int i = 0; i < 24 && nbMissingNumbers < 2; i++) {
        if (allNumbers[i] == 0) {
            missingNumbers[nbMissingNumbers] = i + 1;
            nbMissingNumbers++;
        }
    }
}

void resetTable(int tab[], int cellNumbers) {
    for (int i = 0; i < cellNumbers; i++) {
        tab[i] = 0;
    }
}

void tableSorting(int tab[], int cellNumbers) {
    int iNext;
    int temp;
    for (int i = 0; i < cellNumbers; i++) {
        iNext = i + 1;
        while (iNext < cellNumbers) {
            if (tab[i] > tab[iNext]) {
                temp = tab[i];
                tab[i] = tab[iNext];
                tab[iNext] = temp;
            }
            iNext++;
        }
    }
}
