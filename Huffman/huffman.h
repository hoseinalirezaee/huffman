#pragma once
#include <string>
#include <fstream>

using namespace std;

#define LOOKUP_TABLE_SIZE 512
#define LOOKUP_TABLE_ENRTY_SIZE 10 //In bytes, include char / word part or address part
#define BIT_IN_NUMBER 9 //Number of bits to process each time
#define UINT_MAX 4294967295
#define FIRST_NINE_BITS_MASK 511

class File;
class CompressedFile;
class Dictionary;
class Analyzer;
class FileInput;
class CompressedFile;
class Node;

/*Class Word keeps pairs of characters (or a word) and frequency of each charactor.*/
class Word
{
	char c; //Character
	unsigned int f; //Frequentcy of character

	int huffBinCode; //Huffman binary code in integer form
	int huffBinCodeSize; //Size (number of bits) of huffman binary code

public:

	Word(char c, unsigned int f);
	Word(char c);
	Word(const Word &word);
	Word();

	char getChar() const;
	void setChar(char c);

	unsigned int getFreq() const;
	void incFreq();
	void decFreq();
	void setFreq(unsigned int f);

	int getHuffBinCode();
	void setHuffBinCode(int huffBinCode);
	
	int getHuffBinCodeSize();
	void setHuffBinCodeSize(int huffBinCodeSize);
	void incHuffBinCodeSize();

private:
	void setInfo(char c, unsigned int d, int huffBinCode, int huffBinCodeSize);
};

/*This class is a dictionary which keeps words of different characters. This class
  is used to map a character to variable binary code from huffman tree.*/
class Dictionary
{
	Word *words;
	unsigned int dSize; //Size (number of words) of dictionary

public:
	Dictionary(unsigned int size);
	Dictionary(Word * words, unsigned int size);
	Dictionary(const Dictionary &dic);
	Dictionary();

	~Dictionary();

	Dictionary &operator=(const Dictionary &dic);

	void addWord(Word word, int pos);
	Word &getWord(int i) const;
	unsigned int size() const;

	unsigned int getRealSize() const;

	void sortByHuffCodeSize();


private:

	static void swap(Word *words, int a, int b);

	static void quickSort(Word *words, int p, int r);

	static int partition(Word *words, int p, int r);

};

/*This class is used to analyze the input file to count frequency of each character.*/
class Analyzer
{

public:
	
	static Dictionary analyze(const File &file);

private:
};

/*This class keeps the input file to compress / or the file to be saved after decompressed*/
class File
{
	char *data; //The array of characters from input file
	unsigned int fileSize; //Size of input / output file (in byte)

public:

	File(char *data, unsigned int dataSize);

	char *getData() const;

	unsigned int size() const;

	void deleteFile();
private:
	void setData(char *data, unsigned int dataSize);
};

/*This class is use to read data from file.*/
class FileInput
{
	string path; //Path to file that must be read
	ifstream inputStream; //Input stream to file

public:

	FileInput(string path);
	~FileInput();

	File read(); //This method is used to read data to be compressed.
	CompressedFile readCompressedFile(); //This method is used to read data to be decomopressed.
	void close();
private:
};


class Huffman
{

public:

	static void makeHuffBinCode(Dictionary &dic);

	static CompressedFile compress(File file);

	static File decompress(CompressedFile cFile);

	static char **makeLookupTable(Dictionary &dic);

private:

	static Node huffmanTree(Node *&nodes, unsigned int size);

	static void searchTree(Node node, int postfix, int postfixSize);

	static unsigned int bitsSize(Dictionary &dic);


	static unsigned int reverseHuffBinCoce(unsigned int huffBinCode, int size);

	static void addWordToLookup(char** &rootTable, Word word);

	static void deleteHuffmanTree(Node *root);
	static void deleteLookupTable(char** root);
 
};


/*This class is used to write data to file.*/
class FileOutput
{
	string path; //Path to file that data must be written in
	ofstream outputStream; //Output stream to file

public:
	FileOutput(string path);
	~FileOutput();
	void close();

	void write(File file); //This method is used to write decompressed data to file.
	void write(CompressedFile cFile); //This method is used to write compressed data to file.

private:
};

/*This class keeps compressed data*/
class CompressedFile : public File
{

	Dictionary dic; //The dictionary that data must be compressed with
public:

	CompressedFile(char *data, unsigned int dataSize, Dictionary dic);

	Dictionary getDic();
};

class Node
{
	Node *leftChild;
	Node *rightChild;

	unsigned int tFreq;

	Word *data;

public:
	Node();

	void setLeftChild(Node *lNode);

	void setRightChild(Node *rNode);

	void setWord(Word &data);

	void setTFreq(unsigned int tFreq);


	Node *getLeftChild() const;

	Node *getRightChild() const;

	Word &getWord() const;

	unsigned int getTFreq();


	bool isLeaf() const;


private:
};