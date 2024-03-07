#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node{
    char c;
    int weight;
    char huf[2];
    struct _node* parent;
    struct _node* lChild;
    struct _node* rChild;
}node;

typedef struct{
    char c;
    char hufCode[30];
}hufCode;

//encode function

void init(node** alphabet){
    for(int i=0;i<26;i++){
        alphabet[i]->c=i+'a';
        alphabet[i]->weight=0;
        alphabet[i]->parent=NULL;
        alphabet[i]->lChild=NULL;
        alphabet[i]->rChild=NULL;
    }

    for(int i=26;i<51;i++){
        alphabet[i]->c=0;
        alphabet[i]->weight=0;
        alphabet[i]->parent=NULL;
        alphabet[i]->lChild=NULL;
        alphabet[i]->rChild=NULL;
    }
}

void getInput(FILE* input, node** alphabet, char* inputData, int* nInput){
    char c;
    while((c=fgetc(input))!=EOF){
        if(c>='a'&&c<='z'){
            alphabet[c-'a']->weight++;
            inputData[(*nInput)++]=c;
        }else if(c>='A'&&c<='Z'){
            alphabet[c-'A']->weight++;
            inputData[(*nInput)++]=c-('a'-'A');
        }
    }
}

void copy(node* dest, node* source){
    dest->weight=source->weight;
    dest->c=source->c;
}

void insert(node** alphabet, int idx){
    node temp;
    copy(&temp, alphabet[idx]);

    int i;
    for(i=idx+1;i<26;i++){
        if(temp.weight<=alphabet[i]->weight){
            break;
        }
    }

    for(int j=idx+1;j<i;j++){
        copy(alphabet[j-1], alphabet[j]);
    }
    copy(alphabet[i-1], &temp);
}

void insertionSort(node** alphabet){
    for(int i=25;i>=0;i--){
        insert(alphabet, i);
    }
}

int getNCode(node** alphabet){
    int n=0;

    for(int i=0;i<26;i++){
        if(alphabet[i]->weight==0){
            n++;
        }else{
            break;
        }
    }

    return 26-n;
}

void makeTree(node** alphabet, int nCode){
    for(int i=0;i<nCode-1;i++){
        alphabet[26+i]->c='\0';
        alphabet[26+i]->lChild=alphabet[26-nCode+i*2];
        alphabet[26-nCode+i*2]->parent=alphabet[26+i];
        strcpy(alphabet[26-nCode+i*2]->huf, "0\0");
        alphabet[26+i]->rChild=alphabet[26-nCode+i*2+1];
        alphabet[26-nCode+i*2+1]->parent=alphabet[26+i];
        strcpy(alphabet[26-nCode+i*2+1]->huf, "1\0");
        alphabet[26+i]->weight=alphabet[26-nCode+i*2]->weight+alphabet[26-nCode+i*2+1]->weight;
        insert(alphabet, 26+i);
    }
}

void printTreeStructure(FILE* output, node* nd){
    if(nd->c=='\0'){
        fprintf(output, "(");
        printTreeStructure(output, nd->lChild);
        fprintf(output, ",");
        printTreeStructure(output, nd->rChild);
        fprintf(output, ")");
    }else{
        fprintf(output, "%c", nd->c);
    }
}

int isRoot(node* nd){
    if(nd->parent==NULL){
        return 1;
    }
    
    return 0;
}

void makeHTable(node** alphabet, hufCode** hCode, int nCode){
    for(int i=0;i<nCode;i++){
        hCode[i]->c=alphabet[26-nCode+i]->c;
        node* newNode=alphabet[26-nCode+i];
        char hufCode[30]="\0";
        char temp[30]="\0";
        while(!isRoot(newNode)){
            strcpy(temp, hufCode);
            strcpy(hufCode, newNode->huf);
            strcat(hufCode, temp);
            newNode=newNode->parent;
        }

        strcpy(hCode[i]->hufCode, hufCode);
    }
}

void encode(FILE* output, hufCode** hTable, int nCode, char* inputData, int nInput){
    for(int i=0;i<nInput;i++){
        for(int j=0;j<nCode;j++){
            if(inputData[i]==hTable[j]->c){
                fprintf(output, hTable[j]->hufCode);
                break;
            }
        }
    }
}

//decode functions

void readInputData(FILE* input, char* treeTable, char* inputHufCode){
    char c;
    int i=0;
    while((c=fgetc(input))!=EOF){
        if(c=='H'){
            break;
        }
        treeTable[i++]=c;
    }
    treeTable[i]='\0';
    i=0;
    while((c=fgetc(input))!=EOF){
        if(c=='1'){
            inputHufCode[i++]=c;
            break;
        }
    }
    while((c=fgetc(input))!=EOF){
        inputHufCode[i++]=c;
    }
    inputHufCode[i]='\0';
}

void saveHufCode(node* nd, hufCode** hTable, int* nCode){
    for(int i=0;i<*nCode;i++){
        if(nd->c<hTable[i]->c){
            for(int j=*nCode;j>=i;j--){
                hTable[j+1]->c=hTable[j]->c;
                strcpy(hTable[j+1]->hufCode, hTable[j]->hufCode);
            }
            hTable[i]->c=nd->c;
            strcpy(hTable[i]->hufCode, nd->huf);
            (*nCode)++;
            return;
        }
    }

    hTable[*nCode]->c=nd->c;
    strcpy(hTable[*nCode]->hufCode, nd->huf);
    (*nCode)++;
}

int weight=0;

void reconstructHufTree(node* nd, char* treeData, char* childFlag, hufCode** hTable, int* nCode){
    if(treeData[0]!='('){
        strcpy(nd->huf, childFlag);
        nd->c=treeData[0];
        nd->lChild=NULL;
        nd->rChild=NULL;
        saveHufCode(nd, hTable, nCode);
        return;
    }
    int nBracket=1;
    int index=0;
    int before=1;
    char c;
    while(treeData[index++]!='\0'){
        if(treeData[index]=='('){
            nBracket++;
        }else if(treeData[index]==')'){
            nBracket--;
        }else if(nBracket==1&&treeData[index]==','){
            char newTreeData[500];
            strncpy(newTreeData, treeData+1, index-before);
            newTreeData[index-before]='\0';
            before=index+1;
            node* lNode=(node*)malloc(sizeof(node));
            lNode->c='\0';
            strcpy(lNode->huf, childFlag);
            nd->lChild=lNode;
            lNode->parent=nd;
            lNode->weight=weight++;
            char newChildFlag[30];
            strcpy(newChildFlag, childFlag);
            strcat(newChildFlag, "0\0");
            reconstructHufTree(lNode, newTreeData, newChildFlag, hTable, nCode);
        }else if(nBracket==0){
            char newTreeData[500];
            strncpy(newTreeData, treeData+before, index-before-1);
            newTreeData[index-before]='\0';
            node* rNode=(node*)malloc(sizeof(node));
            rNode->c='\0';
            strcpy(rNode->huf, childFlag);
            nd->rChild=rNode;
            rNode->parent=nd;
            rNode->weight=weight++;
            char newChildFlag[30];
            strcpy(newChildFlag, childFlag);
            strcat(newChildFlag, "1\0");
            reconstructHufTree(rNode, newTreeData, newChildFlag, hTable, nCode);
            return;
        }
    }
}

void printHufCode(FILE* output, hufCode** hTable, int nCode){
    for(int i=0;i<nCode;i++){
        fprintf(output, "%c: %s\n", hTable[i]->c, hTable[i]->hufCode);
    }
}

int isLeafNode(node* nd){
    if(nd->lChild==NULL&&nd->rChild==NULL){
        return 1;
    }

    return 0;
}

void decode(FILE* output, char* inputHufCode, node* root){
    node* curNode=root;
    int index=0;
    while(inputHufCode[index]!='\0'){
        if(inputHufCode[index]=='0'){
            curNode=curNode->lChild;
        }else{
            curNode=curNode->rChild;
        }

        if(isLeafNode(curNode)){
            fprintf(output, "%c", curNode->c);
            curNode=root;
        }
        index++;
    }
}

int main(void){
    //encode
    FILE* input1=fopen("hw3_input.txt", "r");
    node** alphabet=(node**)malloc(sizeof(node)*51);
    for(int i=0;i<51;i++){
        alphabet[i]=(node*)malloc(sizeof(node));
    }

    char inputData[30000000];
    int nInput=0;
    init(alphabet);
    getInput(input1, alphabet, inputData, &nInput);
    
    fclose(input1);
    
    insertionSort(alphabet);

    int nCode=getNCode(alphabet);

    makeTree(alphabet, nCode);

    FILE* output1=fopen("hw3_output1.txt", "w");
    printTreeStructure(output1, alphabet[26+nCode-2]);
    fprintf(output1, "HEADEREND");

    hufCode** hTable1=(hufCode**)malloc(sizeof(hufCode)*nCode);
    for(int i=0;i<nCode;i++){
        hTable1[i]=(hufCode*)malloc(sizeof(hufCode));
    }
    makeHTable(alphabet, hTable1, nCode);
    encode(output1, hTable1, nCode, inputData, nInput);

    for(int i=0;i<nCode;i++){
        free(hTable1[i]);
    }
    free(hTable1);
    fclose(output1);

    //decode
    FILE* input2=fopen("hw3_output1.txt", "r");
    FILE* output2=fopen("hw3_output2.txt", "w");

    char treeData[500];
    char inputHufCode[600000];
    readInputData(input2, treeData, inputHufCode);

    hufCode** hTable2=(hufCode**)malloc(sizeof(hufCode)*26);
    for(int i=0;i<26;i++){
        hTable2[i]=(hufCode*)malloc(sizeof(hufCode));
    }
    int nCode2=0;

    node* root=(node*)malloc(sizeof(node));
    reconstructHufTree(root, treeData, "\0", hTable2, &nCode2);
    printHufCode(output2, hTable2, nCode2);
    decode(output2, inputHufCode, root);
    
    fclose(input2);
    fclose(output2);
    for(int i=0;i<51;i++){
        free(alphabet[i]);
    }
    free(alphabet);
    return 0;
}