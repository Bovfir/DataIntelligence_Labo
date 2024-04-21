#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_TEMPS 601
#define MAX_LINES_TESTSET 30
#define MAX_MOVEMENTS 6
#define MAX_CHAR_NAME_MOVEMENTS 50
#define MAX_CHAR_LINE 50000
#define MAX_LINES_PATTERN 6
#define MAX_LINES_IGNORE_PATTERN 1
#define MAX_LINES_IGNORE_TESTSET 3

#define TESTSET_NAME "testSet.csv"
#define MODELS_NAME "pattern.csv"


typedef struct models models;
struct models
{
    int move;
    double v_acc[MAX_TEMPS];
};


void convertFileToTable(FILE* file, models models[], char fileName[MAX_CHAR_NAME_MOVEMENTS], int nbLines, int nbColumnsToIngore, char movementsNames[MAX_MOVEMENTS][MAX_CHAR_NAME_MOVEMENTS]);
double calcul(double testSet[], double models[]);
void traitement(models testSet[], models models[]);


void main(void) {
    char movementsNames[MAX_MOVEMENTS][MAX_CHAR_NAME_MOVEMENTS] = { "dws","jog","sit","std","ups","wlk" };

    models* testSet = malloc(MAX_LINES_TESTSET * sizeof(*testSet));
    models* models = malloc(MAX_MOVEMENTS * sizeof(*models));

    FILE* pFiTest = fopen(TESTSET_NAME, "r");
    FILE* pFiModel = fopen(MODELS_NAME, "r");

    if (pFiTest == NULL || pFiModel == NULL) {
        printf("Impossible d'ouvrir le fichier");
    }
    else {
        if (testSet == NULL || models == NULL) {
            printf("Erreur d'allocation m moire");
        }
        else {
            convertFileToTable(pFiTest, testSet, TESTSET_NAME, MAX_LINES_TESTSET, MAX_LINES_IGNORE_TESTSET, movementsNames);
            convertFileToTable(pFiModel, models, MODELS_NAME, MAX_LINES_PATTERN, MAX_LINES_IGNORE_PATTERN, movementsNames);

            traitement(testSet, models);

            free(testSet);
            free(models);
        }
        fclose(pFiTest);
        fclose(pFiModel);
    }

}


void convertFileToTable(FILE* file, models models[], char fileName[MAX_CHAR_NAME_MOVEMENTS], int nbLines, int nbColumnsToIngore, char movementsNames[MAX_MOVEMENTS][MAX_CHAR_NAME_MOVEMENTS]) {
    char* line = malloc(MAX_CHAR_LINE * sizeof(char)); // Allocation dynamique de la m moire pour line
    if (line == NULL) {
        printf("Erreur lors de l'allocation de m moire\n");
    }
    else {
        // Ignorer la premi re ligne (ent te)
        fgets(line, MAX_CHAR_LINE, file);

        for (int lineNumber = 0; lineNumber < nbLines; lineNumber++) {

            fgets(line, MAX_CHAR_LINE, file);

            char* nextToken = NULL;
            char* token = strtok_s(line, ",", &nextToken);

            // Rechercher son indice en fonction de son nom
            int iName = 0;
            while (iName < MAX_MOVEMENTS && strncmp(token, movementsNames[iName], 3) != 0) {
                iName++;
            }
            models[lineNumber].move = iName + 1;

            // Extraction des valeurs de chaque ligne en ignorant le nombre de colonnes n cessaire
            // Pour le testSet , il faut ignorer les 3 premieres colonnes
            // Pour le pattern, il faut en ignorer qu'une , d'o  la variable nbColumnsToIngore
            for (int iTemps = 0; iTemps < MAX_TEMPS + nbColumnsToIngore && token != NULL; ++iTemps) {

                if (iTemps >= nbColumnsToIngore) {
                    sscanf_s(token, "%lf", &models[lineNumber].v_acc[iTemps - nbColumnsToIngore]);
                }
                token = strtok_s(NULL, ",", &nextToken);
            }
        }
        free(line);
    }
}

void traitement(models testSet[], models models[]) {

    int realClasses[MAX_LINES_TESTSET] = { 0 };
    int estimateClasses[MAX_LINES_TESTSET] = { 0 };

    double distance;
    double lowValue;
    int iMovementLowDistance;
    double nbIdenticalValues = 0;

    int mouvementEnCours;
    int iLine = 0;
    int count = 0;

    while (iLine < MAX_LINES_TESTSET) {

        nbIdenticalValues = 0;
        count = 0;
        mouvementEnCours = testSet[iLine].move;

        while (iLine < MAX_LINES_TESTSET && mouvementEnCours == testSet[iLine].move) {
            
            realClasses[iLine] = mouvementEnCours;
            distance = 0;
            lowValue = HUGE_VAL;
            iMovementLowDistance = 0;
            
            for (int iMovement = 0; iMovement < 6; iMovement++) {

                distance = calcul(testSet[iLine].v_acc, models[iMovement].v_acc);

                if (distance < lowValue) {
                    lowValue = distance;
                    iMovementLowDistance = iMovement;
                }
                //printf("%d : %.17lf\n", iMovement + 1, distance);
            }
            
            //printf("Low Values : %.17lf\n", lowValue);
            estimateClasses[iLine] = iMovementLowDistance + 1;

            printf("RealClasses : %d et EstimateClasses : %d\n", realClasses[iLine], estimateClasses[iLine]);
            if (realClasses[iLine] == estimateClasses[iLine]) {
                nbIdenticalValues++;
            }
            count++;
            iLine++;
        }
        printf("Pourcentage de valeur identique : %.2lf%%\n", (nbIdenticalValues / count) * 100);
    }
    //printf("Pourcentage de valeur identique : %.2lf%%", (nbIdenticalValues / MAX_LINES_TESTSET) * 100);

}

double calcul(double testSet[], double models[]) {

    double somme = 0;

    for (int iTemps = 0; iTemps < MAX_TEMPS; iTemps++) {

        somme += (testSet[iTemps] - models[iTemps]) * (testSet[iTemps] - models[iTemps]);
    }

    return sqrt(somme);
}
