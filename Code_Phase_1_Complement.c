#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>


#define MAX_FILES 15
#define MAX_SUB 24

#define MOYENNEX 0.00096087
#define MOYENNEY 0.05525659
#define MOYENNEZ 0.0352192

#define ECARTTYPEX 0.38875666
#define ECARTTYPEY 0.61937128
#define ECARTTYPEZ 0.4300345

#define FILE_PATH "data/A_DeviceMotion_data/%s/sub_%d.csv" 
#define FILE_NAME "verif.csv"
#define MAX_CHAR_NAME_MOVEMENTS 50


typedef struct node Node;
struct node {
    double value;
    char type;
    int line;
    Node* next;
};


typedef struct infoSub InfoSub;
struct infoSub {
    int nbValeursAberrantes;
    int nbValeursAberrantesX;
    int nbValeursAberrantesY;
    int nbValeursAberrantesZ;
    int nbLignes;
    Node* numbers;
};

InfoSub traitementSub(char filesNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], int iFile, int iSub);
void insertValue(Node** head, double val, int lineNumbers, char type);
InfoSub extractLastThreeNumbers(Node** numbers, char filesNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], int iFile, int iSub);
bool estAberrante(double nombre, double moyenne, double ecartType);
void freeList(Node** head);
Node* copyList(Node* head);
void printCSV(InfoSub infoSubs[MAX_FILES][MAX_SUB], char filesNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], char fileName[MAX_CHAR_NAME_MOVEMENTS]);



void main(void) {
    char filesNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS] = { "dws_1","dws_2","dws_11","jog_9","jog_16","sit_5","sit_13","std_6","std_14","ups_3","ups_4","ups_12","wlk_7","wlk_8","wlk_15" };
    InfoSub infoSubs[MAX_FILES][MAX_SUB];

    for (int iFile = 0; iFile < MAX_FILES; iFile++) {
        for (int iSub = 0; iSub < MAX_SUB; iSub++) {

            infoSubs[iFile][iSub] = traitementSub(filesNames, iFile, iSub + 1);

            Node* temp = infoSubs[iFile][iSub].numbers;
            
            printf("Dossier %d Sub %d\n", iFile + 1, iSub + 1);

            while (temp != NULL) {
                printf("Ligne %d de type %c : %.17lf\n", temp->line, temp->type, temp->value);
                temp = temp->next;
            }
            printf("nbValeursAberrantes : %d\n", infoSubs[iFile][iSub].nbValeursAberrantes);
            printf("-------------------\n");
        }
    }
    
    printCSV(infoSubs,filesNames, FILE_NAME);
}


InfoSub traitementSub(char filesNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], int iFile, int iSub) {
    InfoSub infoSub;
    Node* numbers = NULL;

    infoSub = extractLastThreeNumbers(&numbers, filesNames, iFile, iSub);
    infoSub.numbers = copyList(numbers);
    freeList(&numbers);

    return infoSub;
}

void insertValue(Node** head, double val,int lineNumbers,char type) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode != NULL) {
        newNode->value = val;
        newNode->type = type;
        newNode->line = lineNumbers;
        newNode->next = NULL;

        if (*head == NULL || (*head)->type > type) {
            newNode->next = *head;
            *head = newNode;
        }
        else {
            Node* temp = *head;
            while (temp->next != NULL && temp->next->type < type) {
                temp = temp->next;
            }
            newNode->next = temp->next;
            temp->next = newNode;
        }
    }
}

InfoSub extractLastThreeNumbers(Node** numbers, char filesNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], int iFile, int iSub) {

    InfoSub infoSub;

    double x, y, z;
    char type;
    infoSub.nbValeursAberrantes = 0;
    infoSub.nbValeursAberrantesX = 0;
    infoSub.nbValeursAberrantesY = 0;
    infoSub.nbValeursAberrantesZ = 0;
    infoSub.nbLignes = 0;

    char filename[MAX_CHAR_NAME_MOVEMENTS];
    sprintf_s(filename, sizeof(filename), FILE_PATH, filesNames[iFile], iSub);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        return infoSub;
    }
    else {
        char line[1024];
        fgets(line, sizeof(line), file); // Ignorer la premi re ligne

        while (fgets(line, sizeof(line), file)) {
            sscanf_s(line, "%*f,%*f,%*f,%*f,%*f,%*f,%*f,%*f,%*f,%*f,%lf,%lf,%lf", &x, &y, &z);

            if (estAberrante(x, MOYENNEX, ECARTTYPEX)) {
                type = 'x';
                insertValue(numbers, x,infoSub.nbLignes,type);
                infoSub.nbValeursAberrantes++;
                infoSub.nbValeursAberrantesX++;
            }
            if (estAberrante(y, MOYENNEY, ECARTTYPEY)) {
                type = 'y';
                insertValue(numbers, y, infoSub.nbLignes,type);
                infoSub.nbValeursAberrantes++;
                infoSub.nbValeursAberrantesY++;
            }
            if (estAberrante(z, MOYENNEZ, ECARTTYPEZ)) {
                type = 'z';
                insertValue(numbers, z, infoSub.nbLignes,type);
                infoSub.nbValeursAberrantes++;
                infoSub.nbValeursAberrantesZ++;
            }
            infoSub.nbLignes++;
        }

        fclose(file);
        return infoSub;
    }
}

bool estAberrante(double nombre, double moyenne, double ecartType) {
    return nombre > (moyenne + (3 * ecartType)) || nombre < (moyenne - (3 * ecartType));
}

void freeList(Node** head) {
    Node* tmp;

    while (*head != NULL) {
        tmp = *head;
        *head = (*head)->next;
        free(tmp);
    }
}

Node* copyList(Node* head) {
    if (head == NULL) {
        return NULL;
    }

    Node* newHead = (Node*)malloc(sizeof(Node));
    if (newHead == NULL) {
        printf("Erreur de d'allocation m moire");
        return NULL;
    }
    else {
        newHead->value = head->value;
        newHead->type = head->type;
        newHead->line = head->line;
        newHead->next = copyList(head->next);
        return newHead;
    }
}

void printCSV(InfoSub infoSubs[MAX_FILES][MAX_SUB], char filesNames[MAX_FILES][MAX_CHAR_NAME_MOVEMENTS], char fileName[MAX_CHAR_NAME_MOVEMENTS]) {
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du file\n");
    }
    else {
        char currentType;
        for (int iFile = 0; iFile < MAX_FILES; iFile++) {
            for (int iSub = 0; iSub < MAX_SUB; iSub++) {
                fprintf(file, "Mouvement,Sub,nbLignes,nbValAb\n");
                Node* temp = infoSubs[iFile][iSub].numbers;
                fprintf(file, "%s,%d,%d,%d\n\n", filesNames[iFile], iSub + 1, infoSubs[iFile][iSub].nbLignes, infoSubs[iFile][iSub].nbValeursAberrantes);
                fprintf(file, "nbValAbX,%d\n", infoSubs[iFile][iSub].nbValeursAberrantesX);
                fprintf(file, "nbValAbY,%d\n", infoSubs[iFile][iSub].nbValeursAberrantesY);
                fprintf(file, "nbValAbZ,%d\n", infoSubs[iFile][iSub].nbValeursAberrantesZ);

                currentType = '\0';
                while (temp != NULL) {
                    if (temp->type != currentType) {
                        fprintf(file, "\n%c :,", temp->type);
                        currentType = temp->type;
                    }
                    fprintf(file, "%.17f ,", temp->value);
                    temp = temp->next;
                }
                fprintf(file, "\n\n");
            }
        }
        fclose(file);
    }
}
