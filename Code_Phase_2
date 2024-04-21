#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEMPS 601
#define MAX_LINES_TRAINSET 330
#define MAX_MOVEMENTS 6
#define MAX_CHAR_NAME_MOVEMENTS 50
#define MAX_CHAR_LINE 50000

#define TRAINSET_NAME "trainSet.csv"
#define TESTSET_NAME "testSet.csv"
#define PATTERN_NAME "pattern.csv"


typedef struct subStats SubStats;
struct subStats
{
    char fileName[MAX_CHAR_NAME_MOVEMENTS];
    double stats[MAX_TEMPS];
};

void extractNumbersCSV(SubStats numbers[]);
void printCSVAverages(double data[MAX_MOVEMENTS][MAX_TEMPS], char movementsNames[MAX_MOVEMENTS][MAX_CHAR_NAME_MOVEMENTS]);

void initTab(double tab[MAX_TEMPS]);

void main(void) {

    //SubStats(*statsSub)[MAX_TEMPS] = malloc(MAX_LINES_TRAINSET * sizeof(*statsSub));
    SubStats *statsSub = malloc(MAX_LINES_TRAINSET * sizeof(*statsSub));

    double(*averageMovements)[MAX_TEMPS] = malloc(MAX_MOVEMENTS * sizeof(*averageMovements));
    char movementsNames[MAX_MOVEMENTS][MAX_CHAR_NAME_MOVEMENTS] = { "dws","jog","sit","std","ups","wlk" };

    if (statsSub == NULL || averageMovements == NULL) {
        printf("Erreur lors de l'allocation de m moire\n");
    }
    else {
        for (int i = 0; i < MAX_LINES_TRAINSET; i++) {
            initTab(statsSub[i].stats);
        }

        for (int i = 0; i < MAX_MOVEMENTS; i++) {
            initTab(averageMovements[i]);
        }
        extractNumbersCSV(statsSub);


        int iLine = 0;
        int indexMovement = 0;
        double marker[MAX_TEMPS];
        char currentMovement[MAX_CHAR_NAME_MOVEMENTS];

        // \frac{ 38.62319439 }{65} = 0.5942029906

        while (iLine < MAX_LINES_TRAINSET) {
            strcpy_s(currentMovement, MAX_CHAR_NAME_MOVEMENTS, movementsNames[indexMovement]); // Ex : dws
            initTab(marker);
            while (iLine < MAX_LINES_TRAINSET && strncmp(currentMovement, statsSub[iLine].fileName, 3) == 0) {

                for (int iTemps = 0; iTemps < MAX_TEMPS; iTemps++) {
                    averageMovements[indexMovement][iTemps] += statsSub[iLine].stats[iTemps];
                    
                    if (statsSub[iLine].stats[iTemps] != 0) {
                        marker[iTemps] += 1;
                    }
                    
                }
                iLine++;
            }
            for (int iTemps = 0; iTemps < MAX_TEMPS; iTemps++) {
                averageMovements[indexMovement][iTemps] /= marker[iTemps];
            }
            indexMovement++;
        }

        printCSVAverages(averageMovements, movementsNames);
        free(statsSub);
        printf("Traitement termine");
    }
}

void initTab(double tab[MAX_TEMPS]) {
    for (int i = 0; i < MAX_TEMPS; i++) {
        tab[i] = 0.0;
    }
}


void extractNumbersCSV(SubStats numbers[]) {
    FILE* file = fopen(TRAINSET_NAME, "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
    }
    else {
        char* line = malloc(MAX_CHAR_LINE * sizeof(char)); // Allocation dynamique de la m moire pour line
        if (line == NULL) {
            printf("Erreur lors de l'allocation de m moire\n");
        }
        else {
            // Ignorer la premi re ligne (ent te)
            fgets(line, MAX_CHAR_LINE, file);

            for (int lineNumber = 0; lineNumber < MAX_LINES_TRAINSET; lineNumber++) {
                fgets(line, MAX_CHAR_LINE, file);
                char* nextToken = NULL;
                // Utiliser strtok_s pour ignorer les trois premiers  l ments et stocker les 601  l ments suivants
                char* token = strtok_s(line, ",", &nextToken);

                strcpy_s(numbers[lineNumber].fileName, MAX_CHAR_NAME_MOVEMENTS, token);

                for (int iTemps = 0; iTemps < MAX_TEMPS + 3 && token != NULL; ++iTemps) {

                    if (iTemps >= 3) {
                        sscanf_s(token, "%lf", &numbers[lineNumber].stats[iTemps - 3]);
                    }
                    token = strtok_s(NULL, ",", &nextToken);
                }
            }
            free(line);
            fclose(file);
        }
    }
}

void printCSVAverages(double data[MAX_MOVEMENTS][MAX_TEMPS], char movementsNames[MAX_MOVEMENTS][MAX_CHAR_NAME_MOVEMENTS]) {
    FILE* file = fopen(PATTERN_NAME, "w");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du file\n");
    }
    else {
        fprintf(file, "Mouvement");
        for (int iTemps = 0; iTemps < MAX_TEMPS; iTemps++) {
            fprintf(file, ",Vacc %d", iTemps);
        }
        fprintf(file, "\n");
        for (int iMouvement = 0; iMouvement < MAX_MOVEMENTS; iMouvement++) {
            fprintf(file, "%s,", movementsNames[iMouvement]);
            for (int iTemps = 0; iTemps < MAX_TEMPS; iTemps++) {
                fprintf(file, "%.17f,", data[iMouvement][iTemps]);
            }
            fprintf(file, "\n");
        }
    }
}







