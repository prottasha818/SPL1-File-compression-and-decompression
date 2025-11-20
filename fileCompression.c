#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define MAX_FILENAME 256
#define MAX_TEXT_SIZE 100000000000
#define MAX_TREE_NODES 512

typedef enum{
    TEXT_FILE,
    UNKNOWN_FILE
}FileType;

FileType detectFileType(const char* fileName)
{
    const char* dot=strrchr(fileName,'.');
    if(!dot || dot==fileName) 
    {
        return UNKNOWN_FILE;
    }
    if(strcmp(dot,".txt")==0)
    {
        return TEXT_FILE;
    }
    return UNKNOWN_FILE;
}

typedef struct HuffmanNode{

    unsigned char data;
     int freq;
     struct HuffmanNode *left,*right;

} HuffmanNode;

typedef struct{
  
    unsigned  char character; ;
     char code[256];

    }HuffmanCode;


typedef struct {

HuffmanNode* nodes[MAX_TREE_NODES];
     int size;

} MinHeap;    

HuffmanNode* createNode(unsigned data, int freq)
{
     HuffmanNode* newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode));
        if (!newNode) 
         {
           fprintf(stderr, "Memory allocation failed\n");
           exit(EXIT_FAILURE);
         }

     newNode->data = data;
     newNode->freq = freq;
     newNode->left = newNode->right = NULL;

     return newNode;
}

void swapNodes(HuffmanNode** a,HuffmanNode** b)
{
    HuffmanNode* temp=*a;
    *a=*b;
    *b=temp;
}

void minHeapify(MinHeap *heap, int index)
{
    int smallest =index;
    int left=2*index+1;
    int right=2*index+2;

    if(left<heap->size && heap->nodes[left]->freq<heap->nodes[smallest]->freq)
    {
        smallest=left;
    }

    if(right <heap->size && heap->nodes[right]->freq<heap->nodes[smallest]->freq)
    {
        smallest=right;
    }

    if(smallest !=index)
    {
        swapNodes(&heap->nodes[smallest],&heap->nodes[index]);
        minHeapify(heap,smallest);  
    }

}

HuffmanNode* extractMin(MinHeap *heap)
{
    if (heap->size <= 0) 
    {
        return NULL;
    }

    HuffmanNode* root = heap->nodes[0];

    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;
    if (heap->size > 0)
    {
        minHeapify(heap, 0);
    }

    return root;
   
}

void insertMinHeap(MinHeap *heap,HuffmanNode* node)
{
    if (heap->size >= MAX_TREE_NODES) {
        fprintf(stderr, "Heap overflow\n");
        exit(EXIT_FAILURE);
    }
    int i = heap->size;
    heap->size++;
   
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap->nodes[parent]->freq <= node->freq) 
        {
            break;
        }

        heap->nodes[i] = heap->nodes[parent];
        i = parent;
    }

    heap->nodes[i] = node;
}
void buildHuffmanCodes(HuffmanNode* root, char* code, int top, HuffmanCode* huffmanCodes, int* index)
{
    if(root->left==NULL && root->right==NULL)
    {
        code[top]='\0';
        huffmanCodes[*index].character=root->data;
        strcpy(huffmanCodes[*index].code,code);
        (*index)++;
        return;
    }
    if(root->left)      
    {
        code[top]='0';
        buildHuffmanCodes(root->left,code,top+1,huffmanCodes,index);
    }
    if(root->right)
    {
        code[top]='1';
        buildHuffmanCodes(root->right,code,top+1,huffmanCodes,index);
    }
    
}

HuffmanNode* buildHuffmanTree(unsigned char *text, int size,HuffmanCode* codes, int * codeCount)
{
     if (size == 0) 
    {
        return NULL;
    }

    int freq[256]={0};

    for(int i=0;i<size;i++)
    {
        freq[text[i]]++;
    }

    MinHeap* heap=(MinHeap*)malloc(sizeof(MinHeap));
    if (!heap)
        { fprintf(stderr, "Memory allocation failed\n");
         exit(EXIT_FAILURE);
         }

    heap->size = 0;

    for(int i=0;i<256;i++)
    {
        if(freq[i]>0)
        {
            insertMinHeap(heap,createNode((unsigned char)i,freq[i]));
        }
    }
       if (heap->size == 1)
        {
          HuffmanNode* only = extractMin(heap);
          HuffmanNode* dummy = createNode('\0', 0);
          HuffmanNode* parent = createNode('\0', only->freq + dummy->freq);
          parent->left = only;
          parent->right = dummy;
          insertMinHeap(heap, parent);
       }


    while(heap->size>1)
    {
        HuffmanNode* left=extractMin(heap);
        HuffmanNode* right=extractMin(heap);

        HuffmanNode* internalNode=createNode('\0',left->freq+right->freq);
        internalNode->left=left;
        internalNode->right=right;

        insertMinHeap(heap,internalNode);
    }

    HuffmanNode* root=extractMin(heap);

    char code[256];
    *codeCount=0;
    buildHuffmanCodes(root,code,0,codes,codeCount);

     if (root)
      {
          buildHuffmanCodes(root, code, 0, codes, codeCount);
       }

    free(heap);
    return root;
}

void freeHuffmanTree(HuffmanNode* root)
{
    if (!root) 
         return;
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}

void compressTextHuffman(const char * inputFile)
{
    printf("Detected: Text File\n");
    printf("HUFFMAN CODING IS USED FOR COMPRESSION\n");
    
    FILE *fp=fopen(inputFile,"rb");
    if(fp==NULL)
    {
        printf("Error opening file: %s\n",inputFile);
        return;
    }

    fseek(fp,0,SEEK_END);
    long fileSize=ftell(fp);
    fseek(fp,0,SEEK_SET);

     if (fileSize == 0)
      {
        printf("Input file is empty. Nothing to compress.\n");
        fclose(fp);
        return;
     }

    unsigned char *text=(unsigned char*)malloc(fileSize);
    fread(text,1,fileSize,fp);
    fclose(fp);

    HuffmanCode huffmanCodes[256];
    int codeCount=0;
    HuffmanNode* root=buildHuffmanTree(text,fileSize,huffmanCodes,&codeCount);
    
     if (!root) 
     {
        fprintf(stderr, "Failed to build Huffman tree\n");
        free(text);
        return;
    }

   char compressedFile[MAX_FILENAME];
   strcpy(compressedFile, inputFile);
   char *ext=strrchr(compressedFile,'.');
   if(ext)*ext='\0';
   strcat(compressedFile,"_compressed.huff");

   fp= fopen(compressedFile,"wb");

    fwrite(&codeCount,sizeof(int),1,fp);
    for(int i=0;i<codeCount;i++){
        fwrite(&huffmanCodes[i].character,sizeof(unsigned char),1,fp);
        int codeLength=strlen(huffmanCodes[i].code);
        fwrite(&codeLength,sizeof(int),1,fp);
        fwrite(huffmanCodes[i].code,sizeof(char),codeLength,fp);
    }

    fwrite(&fileSize,sizeof(long),1,fp);

    char* compressed=(char*)malloc(fileSize*256);
    int compressedIndex=0;

    for(int i=0; i<fileSize;i++)
    {
        for(int j=0;j<codeCount;j++)
        {
            if(text[i]==huffmanCodes[j].character)
            {
                for(int k=0;k<strlen(huffmanCodes[j].code);k++)
                {
                    compressed[compressedIndex++]=huffmanCodes[j].code[k];
                }
                break;
            }
        }
    }   

    fwrite(&compressedIndex,sizeof(int),1,fp);
    fwrite(compressed,sizeof(char),compressedIndex,fp);
    fclose(fp);

    printf("File compressed successfully: %s\n",compressedFile);
    printf("Original Size: %ld bytes\n",fileSize);
    printf("Compressed Size: %d bytes\n",compressedIndex/8 + (compressedIndex%8 !=0 ? 1:0));

    fp=fopen(compressedFile,"rb");

    int readCodeCount;
    fread(&readCodeCount,sizeof(int),1,fp);

    HuffmanCode readCodes[256];

    for(int i=0;i<readCodeCount;i++)
    {
        fread(&readCodes[i].character,sizeof(unsigned char),1,fp);
        int codeLength;
        fread(&codeLength,sizeof(int),1,fp);
        fread(readCodes[i].code,sizeof(char),codeLength,fp);
        readCodes[i].code[codeLength]='\0'; 
    }
        long originalSize;
        fread(&originalSize,sizeof(long),1,fp);

        int readCompressedLength;
        fread(&readCompressedLength,sizeof(int),1,fp);
        char* readCompressed=(char*)malloc(readCompressedLength);
        fread(readCompressed,sizeof(char),readCompressedLength,fp);
        readCompressed[readCodeCount]='\0'; 
        fclose(fp);

        unsigned char* decompressedText=(unsigned char*)malloc(originalSize);
        int decompressedIndex=0;
        char currentCode[256]="";
        int currentCodeIndex=0;
        for(int i=0;i<readCompressedLength;i++)
        {
            currentCode[currentCodeIndex++]=readCompressed[i];
            currentCode[currentCodeIndex]='\0';

            for(int j=0;j<readCodeCount;j++)
            {
                if(strcmp(currentCode,readCodes[j].code)==0)
                {
                    decompressedText[decompressedIndex++]=readCodes[j].character;
                    currentCodeIndex=0;
                    currentCode[0]='\0';
                    break;
                }
            }
        }
    char decompressedFile[MAX_FILENAME];
    strcpy(decompressedFile, inputFile);
    ext = strrchr(decompressedFile, '.');
    if (ext) *ext = '\0';
    strcat(decompressedFile, "_decompressed.txt");
    
    fp = fopen(decompressedFile, "wb");
    fwrite(decompressedText, sizeof(unsigned char), decompressedIndex, fp);
    fclose(fp);
    printf("File decompressed successfully: %s\n", decompressedFile);
    free(text);
    free(compressed);
    free(decompressedText);
    

}

void processFile(const char* fileName)
{
    printf("File compression and decompression functionality is under development.\n");

    printf("Input file: %s\n", fileName);

    FileType type=detectFileType(fileName);
    switch(type)
    {
        case TEXT_FILE:
            printf("Processing text file: %s\n", fileName);
            break;
        default:
            printf("Unsupported file type for file: %s\n", fileName);
            break;
   }
printf("File processing completed for: %s\n", fileName);
}

int main()
{
    char filename[MAX_FILENAME];
    printf("----------Multi-Formate Compression and decompression Tool----------\n");
    printf("Supported Formates:\n");
    printf("1. Text File Compression\n");
    printf("2. Text File Decompression\n");
    printf("Enter the filename: ");
    if(fgets(filename,sizeof(filename),stdin)!=NULL)
    {
        filename[strcspn(filename,"\n")]=0;
        processFile(filename);
        
    }
    return 0;
}