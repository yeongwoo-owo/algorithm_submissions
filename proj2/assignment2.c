#include <stdio.h>
#include <string.h>

#define TRUE    1
#define FALSE   0
typedef int Bool;

typedef char dnaSequence[101];
char base[4];
int pickedIdx[5][102];
int std[101];

int maxPicked=0;
int maxPickedIdx[5][102];

int nDna;
dnaSequence dna[5];
int dnaLen[5];

int nextIdxDp[5][4][102];

void copyInt(int a[5][102], int b[5][102]){
    for(int i=0;i<nDna;i++){
        for(int j=0;j<=maxPicked;j++){
            b[i][j]=a[i][j];
        }
    }
}

Bool findNextBase(int dnaNum, int target, int nPicked){
    int startIdx=pickedIdx[dnaNum][nPicked];
    int nextIdx=startIdx;
    if(nextIdxDp[dnaNum][target][startIdx]!=0){
        if(nextIdxDp[dnaNum][target][startIdx]==-1){
            return FALSE;
        }else{
            pickedIdx[dnaNum][nPicked+1]=nextIdxDp[dnaNum][target][startIdx];
            return TRUE;
        }
    }

	while(++nextIdx<dnaLen[dnaNum]){
		if(dna[dnaNum][nextIdx]==base[target]){
            nextIdxDp[dnaNum][target][startIdx]=nextIdx;
			pickedIdx[dnaNum][nPicked+1]=nextIdx;
            return TRUE;
		}
	}

    nextIdxDp[dnaNum][target][startIdx]=-1;
	return FALSE;
}

Bool findBase(int target, int nPicked){
    for(int i=0;i<nDna;i++){
        if(findNextBase(i, target, nPicked)==FALSE){
            return FALSE;
        }
    }

    return TRUE;
}

void lcs(int nPicked){
    for(int i=0;i<4;i++){
        if(findBase(i, nPicked)==FALSE){
            if(nPicked>maxPicked){
                maxPicked=nPicked;
                copyInt(pickedIdx, maxPickedIdx);
            }
        }else{
            lcs(nPicked+1);
        }
    }
}

int maxValue(int idx){
    int maxVal=pickedIdx[0][idx];

    for(int i=1;i<nDna;i++){
        if(maxVal<pickedIdx[i][idx]){
            maxVal=pickedIdx[i][idx];
        }
    }
    return maxVal;
}

void modified(){
    copyInt(maxPickedIdx, pickedIdx);           //pickedIdx: 복사본

    for(int i=0;i<nDna;i++){
        for(int j=maxPicked;j>0;j--){
            pickedIdx[i][j]-=pickedIdx[i][j-1];
        }
    }

    for(int i=0;i<nDna;i++){
        for(int j=0;j<=maxPicked;j++){
            std[j]=maxValue(j);
        }
    }
}

int getMaxLen(){
    int maxLen=0;
    for(int i=0;i<nDna;i++){
        int tempLen=dnaLen[i];
        for(int j=1;j<=maxPicked;j++){
            tempLen+=(std[j]-pickedIdx[i][j]);
        }

        if(maxLen<tempLen){
            maxLen=tempLen;
        }
    }

    return maxLen;
}

void print(FILE* output){
    for(int i=0;i<nDna;i++){
        maxPickedIdx[i][0]=0;
    }

    int maxLen=getMaxLen();

    for(int i=0;i<nDna;i++){
        int nBar=0;
        for(int j=1;j<=maxPicked;j++){
            int diff=std[j]-pickedIdx[i][j];
            nBar+=diff;

            for(int k=maxPickedIdx[i][j-1];k<maxPickedIdx[i][j];k++){
                fprintf(output, "%c", dna[i][k]);
            }

            for(int k=0;k<diff;k++){
                fprintf(output, "-");
            }
        }

        for(int j=maxPickedIdx[i][maxPicked];j<dnaLen[i];j++){
            fprintf(output, "%c", dna[i][j]);
        }

        for(int j=nBar+dnaLen[i];j<maxLen;j++){
            fprintf(output, "-");
        }
        fprintf(output, "\n");
    }

    std[0]=1;
    for(int i=1;i<=maxPicked;i++){
        for(int j=1;j<std[i];j++){
            fprintf(output, " ");
        }
        fprintf(output, "*");
    }
}

int main(int argc, char* argv[]){
    FILE* inputFile=fopen("hw2_input.txt", "r");
    FILE* outputFile=fopen("hw2_output.txt", "w");

    fscanf(inputFile, "%d", &nDna);
    fscanf(inputFile, "%s", dna[0]);              //remove $

    for(int i=0;i<nDna;i++){
        fscanf(inputFile, "%s", dna[i]);
    }

    base[0]='A', base[1]='T', base[2]='G', base[3]='C';
    for(int i=0;i<nDna;i++){
        pickedIdx[i][0]=-1;
        dnaLen[i]=strlen(dna[i]);
    }

    lcs(0);
    modified();
    print(outputFile);

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}