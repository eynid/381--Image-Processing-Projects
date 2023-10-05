/******************

The focus of this project was to understand the basics of morphology and 
how to manipulate images. 

******************/
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

class Morphology{
    public:
    int imgrow, imgcol;
    int structrow, structcol;
    int rowOrigin, colOrigin, rowFrame, colFrame;
    int extraRow, extraCol, rowSize, colSize;

    int ** tempAry;
    int ** zeroFramedAry;
    int ** morphAry;
    int ** structAry;
///*** Two extra arrays as temporary placeholders for all the extra limbs. 
    int ** limbHolder;
    int ** bodyHolder;
    
    Morphology(int irow, int icol, 
               int srow, int scol,
               int rorigin, int corigin){
        imgrow = irow;
        imgcol = icol;
        
        structrow = srow;
        structcol = scol;
 
        rowFrame = structrow/2;
        colFrame = structcol/2;
        extraCol = colFrame * 2;
        extraRow = rowFrame * 2;

        rowOrigin = rorigin;
        colOrigin = corigin;
        rowSize = imgrow + extraRow;
        colSize = imgcol + extraCol;
        
        structAry = new int*[structrow];

        tempAry = new int*[rowSize];
        zeroFramedAry = new int*[rowSize];
        morphAry = new int*[rowSize];
        limbHolder = new int*[rowSize];
        bodyHolder = new int*[rowSize];

        for(int i = 0; i < rowSize; i++){
            tempAry[i] = new int[colSize];
            zeroFramedAry[i] = new int[colSize];
            morphAry[i] = new int[colSize];
            limbHolder[i] = new int[colSize];
            bodyHolder[i] = new int[colSize];
        }

        for(int i = 0; i < structrow; i++){
            structAry[i] = new int[structcol];
        }
    }

    void loadImg(ifstream& infile, int ** zeroFramedAry){
        int i = rowOrigin;

        while(!infile.eof()){
            int j = colOrigin;
            while(j < colSize- colFrame){
                infile >> zeroFramedAry[i][j];
                j++;

            }
            i++;
        }

    }

    void loadStruct(ifstream& structfile, int ** structAry){
        int i = 0;

        while(!structfile.eof()){
            int j = 0;
            while(j < structcol){
               structfile >> structAry[i][j];
               j++;
            }
            i++;
        }
        
    }

    void zero2DAry(int ** ary, int rows, int cols){
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                ary[i][j] = 0;
            }
        }

    }

    void computeDilation(int ** inary, int ** outary){
        int i = rowFrame;
        while(i < rowSize){
            int j = colFrame;
            while(j < colSize){
                if(inary[i][j] > 0){
                    onePixelDilation(i, j, inary, outary);
                }
                j++;
 
            }
            i++;       
        }

    }

    void onePixelDilation(int i, int j, int ** inary, int **outary){
        int ioffset = i - rowOrigin;
        int joffset = j - colOrigin;
        int rindex = 0;
        while(rindex < structrow){
            int cindex = 0;
            while(cindex < structcol){
                if(structAry[rindex][cindex] > 0){
                    outary[ioffset + rindex][joffset + cindex] = 1;
                }
                cindex++;
            }
            rindex++;
        }
    }

    void onePixelErosion(int i, int j, int ** inary, int **outary){
        int ioffset = i - rowOrigin;
        int joffset = j - colOrigin;
        int rindex = 0;
        bool matchFlag = true;
        
        while((matchFlag == true) && (rindex < structrow)){
            int cindex = 0;
            while((matchFlag == true) && (cindex < structcol)){
                if(structAry[rindex][cindex] > 0 && inary[ioffset + rindex][joffset + cindex] <= 0){
                    matchFlag = false;
                }
                
                cindex++;
            }
            rindex++;

        }
        if(matchFlag == true){
            outary[i][j] = 1;
        }
        else{
            outary[i][j] = 0;
        }

    }

    void computeErosion(int ** inary, int ** outary){
        int i = rowFrame;
        while(i < rowSize){
            int j = colFrame;
            while(j < colSize){
                if(inary[i][j] > 0){
                  onePixelErosion(i, j, inary, outary);
                }
                j++;

            }
            i++;
        }

    }

    void computeOpening(int ** inary, int ** outary, int ** temp){
        computeErosion(inary, temp);
        computeDilation(temp, outary);

    }

    void computeClosing(int ** inary, int ** outary, int ** temp){
        computeDilation(inary, temp);
        computeErosion(temp, outary);
    }

    void subtractImages(int **image, int ** resultImage, int rows, int cols){
        for(int i = 0; i < rows; i++){
            for(int j = 0; j< cols; j++){
                if(image[i][j] == 1 && resultImage[i][j] == 1){
                    image[i][j] = 0;
                }
            }
        }
    }

    void printAry(int ** ary ,ofstream& out, int rows, int cols){
        for(int i = 0; i < rows; i++){
           for(int j = 0; j < cols; j++){
               out << ary[i][j] << " ";
           }

           out << "\n";
        }
    }

    void prettyPrint(int ** ary, ofstream& outfile, int rows, int cols){
        for(int i = 0; i < rows; i++){
           for(int j = 0; j < cols; j++){

               if(ary[i][j] == 0){
                  outfile << ". ";
               }
               else{
                  outfile << "1 ";
               }

           }
           outfile << "\n";
        }
    }
};

int main (int argc, char* argv[]){
    ifstream inFile;
    ofstream outFile;
    ofstream outFile2;
    ifstream elementFile;
    ifstream imageOne;
    ifstream imageTwo;
    ifstream oneStruct;
    ifstream limbStruct;
    ifstream bodyStruct;

    inFile.open(argv[1]);
    elementFile.open(argv[2]);
    imageOne.open(argv[3]);
    imageTwo.open(argv[4]);
    outFile.open(argv[5]);
    outFile2.open(argv[6]);
    oneStruct.open(argv[7]);
    limbStruct.open(argv[8]);
    bodyStruct.open(argv[9]);

    int numIrows, numIcols, minIval, maxIval;
    int numSrows, numScols, minSval, maxSval;
    int rowOrigin, colOrigin;

    inFile >> numIrows >> numIcols >> minIval >> maxIval;
    elementFile >> numSrows >> numScols >> minSval >> maxSval;
    elementFile >> rowOrigin >> colOrigin;

    Morphology * MorphCall = new Morphology(numIrows, numIcols, numSrows, numScols,rowOrigin, colOrigin);

    MorphCall->zero2DAry(MorphCall->zeroFramedAry, MorphCall->rowSize, MorphCall->colSize);
    MorphCall->loadImg(inFile, MorphCall->zeroFramedAry);
    outFile << "Data Picture \n"; 
    MorphCall->prettyPrint(MorphCall->zeroFramedAry, outFile,  MorphCall->rowSize, MorphCall->colSize);

    MorphCall->zero2DAry(MorphCall->structAry, numSrows, numScols);
    MorphCall->loadStruct(elementFile, MorphCall->structAry);
    outFile << "Image Structure \n";
    MorphCall->prettyPrint(MorphCall->structAry, outFile, numSrows, numScols);

    MorphCall->zero2DAry(MorphCall->morphAry, MorphCall->rowSize, MorphCall->colSize);
    MorphCall->computeDilation(MorphCall->zeroFramedAry, MorphCall->morphAry);
    outFile << "Dilation \n";
    MorphCall->prettyPrint(MorphCall->morphAry, outFile, MorphCall->rowSize, MorphCall->colSize);

    MorphCall->zero2DAry(MorphCall->morphAry, MorphCall->rowSize, MorphCall->colSize);
    MorphCall->computeErosion(MorphCall->zeroFramedAry, MorphCall->morphAry);
    outFile << "Erosion \n";
    MorphCall->prettyPrint(MorphCall->morphAry, outFile, MorphCall->rowSize, MorphCall->colSize);

    MorphCall->zero2DAry(MorphCall->morphAry, MorphCall->rowSize, MorphCall->colSize);
    MorphCall->computeOpening(MorphCall->zeroFramedAry, MorphCall->morphAry, MorphCall->tempAry);
    outFile << "Opening \n";
    MorphCall->prettyPrint(MorphCall->tempAry, outFile, MorphCall->rowSize, MorphCall->colSize);

    MorphCall->zero2DAry(MorphCall->morphAry, MorphCall->rowSize, MorphCall->colSize);
    MorphCall->computeClosing(MorphCall->zeroFramedAry, MorphCall->morphAry, MorphCall->tempAry);
    outFile << "Closing \n";
    MorphCall->prettyPrint(MorphCall->morphAry, outFile, MorphCall->rowSize, MorphCall->colSize);


    //TASK 2
    //In addition to the general idea of morphology, this task is selected from two options, the task I picked
    //Involves mimg2.txt, the goal of this image is to separate all possible limbs from the whole body.
     //Temp Array - Holds the limbs. 
     //Morph Array - Holds the head and body. 
     //Zero Frame - Holds the Entire Image. 
     //In order to obtain the head, i would need to use another structuring element to isolate the body. 
     //In Total, I would need 3 separate structuring elements to isolate all of the limbs.

    int sRow, sCol;
    int iRow, iCol, imin, imax;
    int rOrigin, cOrigin;
    imageTwo >> iRow >> iCol >> imin >> imax;
    oneStruct >> sRow >> sCol >> rOrigin >> cOrigin;

    Morphology * imgStruct = new Morphology(iRow, iCol, sRow, sCol, rOrigin, cOrigin);
    //Set the array to 0, load image, and print. Generally speaking there might be a better way to structure all of this,
    // but I'll think of a method.
    // 
    imgStruct->zero2DAry(imgStruct->zeroFramedAry, imgStruct->rowSize, imgStruct->colSize);
    imgStruct->loadImg(imageTwo, imgStruct->zeroFramedAry);
    outFile2 << "Data Picture \n"; 
    imgStruct->prettyPrint(imgStruct->zeroFramedAry, outFile2, imgStruct->rowSize, imgStruct->colSize);

    imgStruct->zero2DAry(imgStruct->structAry, sRow, sCol);
    imgStruct->loadStruct(oneStruct, imgStruct->structAry);
    outFile2 << "Image Structure \n";
    imgStruct->prettyPrint(imgStruct->structAry, outFile2, sRow, sCol);

    imgStruct->zero2DAry(imgStruct->morphAry,imgStruct->rowSize, imgStruct->colSize);
    imgStruct->computeOpening(imgStruct->zeroFramedAry, imgStruct->morphAry, imgStruct->tempAry);
    outFile2 << "Opening \n";
    imgStruct->prettyPrint(imgStruct->morphAry, outFile2, imgStruct->rowSize, imgStruct->colSize);

    imgStruct->tempAry = imgStruct->zeroFramedAry;
    imgStruct->subtractImages(imgStruct->tempAry, imgStruct->morphAry, imgStruct->rowSize, imgStruct->colSize);
    outFile2 << "Subtracting Limbs \n";
    imgStruct->prettyPrint(imgStruct->tempAry, outFile2, imgStruct->rowSize, imgStruct->colSize);

    imgStruct->zero2DAry(imgStruct->structAry, sRow, sCol);
    imgStruct->loadStruct(limbStruct, imgStruct->structAry);
    outFile2 << "Structure Element\n";
    imgStruct->prettyPrint(imgStruct->structAry, outFile2, sRow, sCol);

    imgStruct->zero2DAry(imgStruct->limbHolder, imgStruct->rowSize, imgStruct->colSize);
    imgStruct->computeOpening(imgStruct->tempAry, imgStruct->limbHolder, imgStruct->bodyHolder);
    outFile2 << "Eroding Limbs \n";
    imgStruct->prettyPrint(imgStruct->limbHolder, outFile2, imgStruct->rowSize, imgStruct->colSize);

    imgStruct->subtractImages(imgStruct->tempAry, imgStruct->limbHolder, imgStruct->rowSize, imgStruct->colSize);
    outFile2 << "Subtraction \n";
    imgStruct->prettyPrint(imgStruct->tempAry, outFile2, imgStruct->rowSize, imgStruct->colSize);

    imgStruct->zero2DAry(imgStruct->structAry, sRow, sCol);
    imgStruct->loadStruct(bodyStruct, imgStruct->structAry);
    outFile2 << "Structure Element\n";
    imgStruct->prettyPrint(imgStruct->structAry, outFile2, sRow, sCol);

    imgStruct->zero2DAry(imgStruct->bodyHolder, imgStruct->rowSize, imgStruct->colSize);
    imgStruct->zero2DAry(imgStruct->limbHolder,imgStruct->rowSize, imgStruct->colSize);

    imgStruct->computeOpening(imgStruct->morphAry, imgStruct->bodyHolder, imgStruct->limbHolder);
    outFile2 << "Opening Body \n";
    imgStruct->prettyPrint(imgStruct->bodyHolder, outFile2, imgStruct->rowSize, imgStruct->colSize);

    imgStruct->subtractImages(imgStruct->morphAry, imgStruct->bodyHolder, imgStruct->rowSize, imgStruct->colSize);
    outFile2 << "Subtracting Body \n";
    imgStruct->prettyPrint(imgStruct->morphAry, outFile2, imgStruct->rowSize, imgStruct->colSize);

    inFile.close();
    elementFile.close();
    imageOne.close();
    imageTwo.close();
    outFile.close();
    outFile2.close();
    oneStruct.close();
    limbStruct.close();
    bodyStruct.close();
}
