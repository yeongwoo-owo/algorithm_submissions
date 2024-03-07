#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int rawDataIdx;
    char key[31];
}data;

int getKeyIndex(FILE* inputFile){
    int index=0;
    char c;

    while((c=fgetc(inputFile))!='\0'){
        if(c==':'){
            index++;
        }else if(c=='*'){
            break;
        }
    }

    while((c=fgetc(inputFile))!='$'){    }
    fgetc(inputFile);

    return index;
}

void getInputData(FILE* inputFile, data** keyData, char** rawData, int n, int keyIndex){
    for(int i=0;i<n;i++){
        keyData[i]->rawDataIdx=i;

        fscanf(inputFile, "%s", rawData[i]);
        int index=0;
        int j=0;
    
        while(index!=keyIndex){
            if(rawData[i][j]==':'){
                index++;
            }
            j++;
        }

        char temp[31];
        int tempIdx=0;

        while(rawData[i][j]!=':'&&rawData[i][j]!='\0'){
            temp[tempIdx++]=rawData[i][j];
            j++;
        }
        
        strcpy(keyData[i]->key, temp);
    }
}

int comp(data** keyData, int a, int b){
    if(strcmp(keyData[a]->key, keyData[b]->key)!=0){
		return strcmp(keyData[a]->key, keyData[b]->key);
	}else{
		return (keyData[a]->rawDataIdx)-(keyData[b]->rawDataIdx);
    }
}

void swap(data** keyData, int a, int b){
    char tempKey[31];
    long tempPos;

    strcpy(tempKey, keyData[a]->key);
    tempPos=keyData[a]->rawDataIdx;

    strcpy(keyData[a]->key, keyData[b]->key);
    keyData[a]->rawDataIdx=keyData[b]->rawDataIdx;

    strcpy(keyData[b]->key, tempKey);
    keyData[b]->rawDataIdx=tempPos;
}

int median(data** keyData, int a, int b, int c){
	if(comp(keyData, a, b)<0){
		if(comp(keyData, b, c)<0){
			return b;
		}else{
			return comp(keyData, a, c)<0?c:a;
		}
	}else{
		if(comp(keyData, a, c)<0){
			return a;
		}else{
			return comp(keyData, b, c)<0?c:b;
		}
	}
}

void quickSort(data** keyData, int start, int end) {
    if(start>=end){
        return;
    }

    int pivot=median(keyData, start, (start+end)/2, end);
    int i=start+1;
    int j=end;

	swap(keyData, start, pivot);

    while(i<=j){
        while(i<=end&&comp(keyData, i, start)<=0){
            i++;
        }
        while(j>start&&comp(keyData, j, start)>=0){
            j--;
        }

        if(i>j){
            swap(keyData, start, j);
        }else{
            swap(keyData, i, j);
        }
    }

    quickSort(keyData, start, j-1);
    quickSort(keyData, j+1, end);
}

void sortData(data** keyData, int n){
    quickSort(keyData, 0, n-1);
}

void printData(FILE* outputFile, char** rawData, data** keyData, int n){
    for(int i=0;i<n;i++){
        fputs(rawData[keyData[i]->rawDataIdx], outputFile);
        fputc('\n', outputFile);
    }
}

int main(void){
    FILE* inputFile=fopen("hw1_input.txt", "r");
    FILE* outputFile=fopen("hw1_output.txt", "w");

    int n;
    fscanf(inputFile, "%d", &n);
    fgetc(inputFile);fgetc(inputFile);

    int keyIndex=getKeyIndex(inputFile);

    data** keyData=(data**)malloc(sizeof(data)*n);
    for(int i=0;i<n;i++){
        keyData[i]=(data*)malloc(sizeof(data));
    }

    char** rawData=(char**)malloc(sizeof(char)*n*1500);
    for(int i=0;i<n;i++){
        rawData[i]=(char*)malloc(sizeof(char)*1500);
    }

    getInputData(inputFile, keyData, rawData, n, keyIndex);
    sortData(keyData, n);
    printData(outputFile, rawData, keyData, n);

    for(int i=0;i<n;i++){
        free(keyData[i]);
    }
    free(keyData);

    for(int i=0;i<n;i++){
        free(rawData[i]);
    }
    free(rawData);

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}
