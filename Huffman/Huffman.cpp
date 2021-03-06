#include "huffman.h"

using namespace std;


//Implemention of class File

void File::setData(char *data, unsigned int dataSize)
{
	this->data = data;
	this->fileSize = dataSize;
}

char *File::getData() const
{
	return this->data;
}

File::File(char *data, unsigned int dataSize)
{
	setData(data, dataSize);
}

unsigned int File::size() const
{
	return this->fileSize;
}

void File::deleteFile()
{
	delete this->data;
}

//Implemention of class Analyzer

Dictionary Analyzer::analyze(const File &file)
{
	char *data = file.getData();

	unsigned int size = file.size();

	unsigned int freq[256];

	for (int i = 0; i < 256; i++)
	{
		freq[i] = 0;
	}

	for (int i = 0; i < size; i++)
	{
		freq[data[i] + 128]++;
	}

	Dictionary dic(256);

	Word word;

	for (int i = 0; i < 256; i++)
	{
		word.setChar(i - 128);
		word.setFreq(freq[i]);
		dic.addWord(word, i);
	}

	return dic;
}

//Implemention of class Word

Word::Word(char c)
{
	setInfo(c, 0, 0, 0);
}

Word::Word(const Word &word)
{
	setInfo(word.c, word.f, word.huffBinCode, word.huffBinCodeSize);
}

Word::Word(char c, unsigned int f)
{
	setInfo(c, f, 0, 0);
}

Word::Word()
{
	setInfo(0, 0, 0, 0);
}

void Word::setInfo(char c, unsigned int f, int huffBinCode, int huffBinCodeSize)
{
	this->c = c;
	this->f = f;
	this->huffBinCode = huffBinCode;
	this->huffBinCodeSize = huffBinCodeSize;
}

char Word::getChar() const
{
	return this->c;
}

unsigned int Word::getFreq() const
{
	return this->f;
}

void Word::incFreq()
{
	this->f++;
}

void Word::decFreq()
{
	this->f--;
}

void Word::setChar(char c)
{
	this->c = c;
}

void Word::setFreq(unsigned int f)
{
	this->f = f;
}

int Word::getHuffBinCode()
{
	return this->huffBinCode;
}

int Word::getHuffBinCodeSize()
{
	return this->huffBinCodeSize;
}

void Word::setHuffBinCode(int huffBinCode)
{
	this->huffBinCode = huffBinCode;
}

void Word::setHuffBinCodeSize(int huffBinCodeSize)
{
	this->huffBinCodeSize = huffBinCodeSize;
}

void Word::incHuffBinCodeSize()
{
	this->huffBinCodeSize++;
}

//Implemention of class Dictionary

Dictionary::Dictionary(unsigned int size)
{
	this->words = new Word[size];
	this->dSize = size;
}

Dictionary::Dictionary(Word* words, unsigned int size)
{
	this->words = words;
	this->dSize = size;
}

void Dictionary::addWord(Word word, int pos)
{
	words[pos] = word;
}

Word &Dictionary::getWord(int i) const
{
	return this->words[i];
}

unsigned int Dictionary::size() const {
	return this->dSize;
}

Dictionary::Dictionary(const Dictionary &dic)
{
	this->dSize = dic.dSize;

	words = new Word[dSize];

	for (int i = 0; i < dSize; i++)
	{
		words[i] = dic.getWord(i);
	}
}

Dictionary::~Dictionary()
{
	if (this->words != nullptr)
	{
		delete[] this->words;
	}
}

Dictionary &Dictionary::operator=(const Dictionary &dic)
{

	if (this->words != nullptr)
	{
		delete[] this->words;
	}

	this->dSize = dic.dSize;

	this->words = new Word[dSize];

	for (int i = 0; i < dSize; i++)
	{
		this->words[i] = dic.getWord(i);
	}

	return *this;
}

Dictionary::Dictionary()
{
	dSize = 0;
}

unsigned int Dictionary::getRealSize() const
{
	unsigned int rSize = 0;
	for (int i = 0; i < dSize; i++)
	{
		if (words[i].getFreq() != 0)
		{
			rSize++;
		}
	}

	return rSize;
}

void Dictionary::sortByHuffCodeSize()
{
	quickSort(this->words, 0, this->dSize - 1);
}

void Dictionary::quickSort(Word *words, int p, int r)
{
	if (p < r)
	{
		int q = partition(words, p, r);
		quickSort(words, p, q - 1);
		quickSort(words, q + 1, r);
	}

}

int Dictionary::partition(Word *words, int p, int r)
{
	int pivot = words[r].getHuffBinCodeSize();

	int small_equal_pivot = p - 1;

	for (int i = p; i <= r; i++)
	{
		if (words[i].getHuffBinCodeSize() <= pivot)
		{
			small_equal_pivot++;
			swap(words, i, small_equal_pivot);
		}
	}
	return small_equal_pivot;
}

void Dictionary::swap(Word *words, int a, int b)
{
	Word temp = words[a];

	words[a] = words[b];

	words[b] = temp;
}
//Implemention of class FileInput

FileInput::FileInput(string path)
{
	this->path = path;
	inputStream.open(path, ios::binary);
}

File FileInput::read()
{
	int size;

	inputStream.seekg(0, ios::end);

	size = inputStream.tellg();

	inputStream.seekg(0, ios::beg);
	
	char *file = new char[size + 1]; // Last Byte for end of line character

	file[size] = 0;

	inputStream.read(file, size);
	
	return File(file, size);
}

FileInput::~FileInput()
{
	this->inputStream.close();
}

void FileInput::close()
{
	this->inputStream.close();
}

CompressedFile FileInput::readCompressedFile()
{
	unsigned int dSize;

	inputStream.read((char *)(&dSize), sizeof(unsigned int));

	int dicInfoSize = dSize * (sizeof(unsigned int) + sizeof(char));
	
	char *dicInfo = new char[dicInfoSize];

	inputStream.read(dicInfo, dicInfoSize);

	Dictionary dic(256);

	Word word;

	for (int i = 0; i < dSize; i++)
	{
		int index = i * (sizeof(unsigned int) + sizeof(char));
		word.setChar(dicInfo[index]);
		word.setFreq(*(unsigned int *)(dicInfo + index + 1));

		dic.addWord(word, word.getChar() + 128);
	}

	unsigned int compressedFileSize;

	inputStream.read((char *)(&compressedFileSize), sizeof(unsigned int));

	char *compressedData = new char[compressedFileSize + 1]; //Assign one more character for end of line character

	compressedData[compressedFileSize] = 0; //Add end of line character

	inputStream.read(compressedData, compressedFileSize);

	delete[] dicInfo;

	return CompressedFile(compressedData, compressedFileSize, dic);
}

//Class Huffman

void Huffman::makeHuffBinCode(Dictionary &dic)
{
	unsigned int realSize = dic.getRealSize();

	Node *nodes = new Node[realSize];

	int index = 0;
	for (int i = 0; i < dic.size(); i++)
	{
		if (dic.getWord(i).getFreq() != 0)
		{
			nodes[index].setWord(dic.getWord(i));
			nodes[index].setTFreq(dic.getWord(i).getFreq());
			index++;
		}
	}

	Node hTree = huffmanTree(nodes, realSize);

	delete[] nodes;

	searchTree(hTree, 0, 0);

	deleteHuffmanTree(hTree.getLeftChild());
	deleteHuffmanTree(hTree.getRightChild());
}

Node Huffman::huffmanTree(Node *&nodes, unsigned int size)
{
	for (int j = 0; j < size - 1; j++)
	{
		int min1Index = 0;

		for (int i = 0; i < size; i++)
		{
			if (nodes[min1Index].getTFreq() > nodes[i].getTFreq())
			{
				min1Index = i;
			}
		}

		unsigned int leftFreq = nodes[min1Index].getTFreq();

		nodes[min1Index].setTFreq(UINT_MAX);

		int min2Index = 0;

		for (int i = 0; i < size; i++)
		{
			if (nodes[min2Index].getTFreq() > nodes[i].getTFreq())
			{
				min2Index = i;
			}
		}

		unsigned int rightFreq = nodes[min2Index].getTFreq();

		nodes[min2Index].setTFreq(UINT_MAX);


		Node *leftChild = new Node;

		leftChild->setWord(nodes[min1Index].getWord());

		leftChild->setLeftChild(nodes[min1Index].getLeftChild());

		leftChild->setRightChild(nodes[min1Index].getRightChild());


		Node *rightChild = new Node;

		rightChild->setWord(nodes[min2Index].getWord());

		rightChild->setLeftChild(nodes[min2Index].getLeftChild());

		rightChild->setRightChild(nodes[min2Index].getRightChild());


		nodes[min1Index].setLeftChild(leftChild);

		nodes[min1Index].setRightChild(rightChild);

		nodes[min1Index].setTFreq(leftFreq + rightFreq);
	}

	int min = 0;

	for (int i = 0; i < size; i++)
	{
		if (nodes[min].getTFreq() > nodes[i].getTFreq())
		{
			min = i;
		}
	}

	return nodes[min];
}

void Huffman::searchTree(Node node, int postfix, int postfixSize)
{
	if (node.isLeaf())
	{
		return;
	}

	int temp = 0;

	if (node.getRightChild()->isLeaf())
	{
		temp = postfix | (1 << postfixSize); //Add a 1 to the left of code
		node.getRightChild()->getWord().setHuffBinCode(temp);
		node.getRightChild()->getWord().setHuffBinCodeSize(postfixSize + 1);
	}
	if (node.getLeftChild()->isLeaf())
	{
		temp = postfix; //Add a 0 to the left of the code
		node.getLeftChild()->getWord().setHuffBinCode(temp);
		node.getLeftChild()->getWord().setHuffBinCodeSize(postfixSize + 1);
	}

	searchTree(*node.getLeftChild(), postfix, postfixSize + 1);
	searchTree(*node.getRightChild(), postfix | 1 << postfixSize, postfixSize + 1);
}

CompressedFile Huffman::compress(File file)
{
	char *data = file.getData();

	Dictionary dic = Analyzer::analyze(file);

	Huffman::makeHuffBinCode(dic);

	unsigned int code[256]; //Stores huffman code here

	unsigned int freq[256]; //Stores huffman code size here

	for (int i = 0; i < 256; i++)
	{
		code[i] = dic.getWord(i).getHuffBinCode();
		freq[i] = dic.getWord(i).getHuffBinCodeSize();
	}

	unsigned int compressedFileSize = Huffman::bitsSize(dic) / 8 + 1;
	
	char *compressedData = new char[compressedFileSize];

	for (int i = 0; i < compressedFileSize; i++)
	{
		compressedData[i] = 0;
	}

	unsigned int fileSize = file.size();

	unsigned int index = 0;

	for (int i = 0; i < fileSize; i++)
	{
		*(int *)(compressedData + (index / 8))
			|=
			code[data[i] + 128] << (index & 7);

		index += freq[data[i] + 128];
	}

	return CompressedFile(compressedData, compressedFileSize, dic);
}

File Huffman::decompress(CompressedFile cFile)
{
	unsigned int cDataSize = cFile.size();

	char *cData = cFile.getData();

	Dictionary dic = cFile.getDic();

	makeHuffBinCode(dic);

	int reversedHuffBinCodes[512];
	
	for (int i = 0; i < 512; i++)
	{
		reversedHuffBinCodes[i] = reverseHuffBinCoce(i, BIT_IN_NUMBER);
	}


	/*
	//Decoding using huffman tree
	int dicSize = dic.size();
	
	int dicRSize = dic.getRealSize();

	Node *nodes = new Node[dicRSize];

	int index = 0;
	for (int i = 0; i < dic.size(); i++)
	{
		if (dic.getWord(i).getFreq() != 0)
		{
			nodes[index].setWord(dic.getWord(i));
			nodes[index].setTFreq(dic.getWord(i).getFreq());
			index++;
		}
	}

	Node hTreeRoot = huffmanTree(nodes, dicRSize);

	Node currentNode = hTreeRoot;*/

	char **firstLookupTable = makeLookupTable(dic);

	unsigned int fileSize = 0;
	unsigned int bitsSize = 0;


	for (int i = 0; i < dic.size(); i++)
	{
		if (dic.getWord(i).getFreq() != 0)
		{
			fileSize += dic.getWord(i).getFreq();

			bitsSize += dic.getWord(i).getFreq() * dic.getWord(i).getHuffBinCodeSize();
		}
	}

	char *data = new char[fileSize];

	unsigned int currentBitIndex = 0;

	unsigned int bufferBitSize = 0;

	unsigned int bufferBit = 0;

	unsigned int charIndex = 0;

	char **currentLookupTable = firstLookupTable;

	while (currentBitIndex < bitsSize)
	{
		//The following line insert some bits from compressed file to bufferBit to keep
		//size of bufferBit with BIT_IN_NUMBER
		bufferBit = ((*(int*)(cData + (currentBitIndex / 8)) >> (currentBitIndex & 7))
			<< bufferBitSize) | bufferBit;

		//The following line just keep first nine bits of bufferBit
		bufferBit &= FIRST_NINE_BITS_MASK;

		//The following line update the first index of compressed file bits that not used.
		currentBitIndex += (BIT_IN_NUMBER - bufferBitSize);

		bufferBitSize = BIT_IN_NUMBER;

		int reversedBufferBit = reversedHuffBinCodes[bufferBit];

		if (currentLookupTable[reversedBufferBit][LOOKUP_TABLE_ENRTY_SIZE - 1])
		{
			char **nextLookupTable = (char**)*(long long *)(currentLookupTable[reversedBufferBit] + 1);

			currentLookupTable = nextLookupTable;

			bufferBit = 0;

			bufferBitSize = 0;
		}
		else
		{
			int toBeRemoved = *(long long *)(currentLookupTable[reversedBufferBit] + 1);

			char c = currentLookupTable[reversedBufferBit][0];

			bufferBitSize -= toBeRemoved;

			bufferBit >>= toBeRemoved;

			data[charIndex++] = c;

			currentLookupTable = firstLookupTable;
		}
	}
	
	deleteLookupTable(firstLookupTable);

	return File(data, fileSize);
}

unsigned int Huffman::bitsSize(Dictionary &dic)
{
	unsigned int bitsSize = 0;

	for (int i = 0; i < dic.size(); i++)
	{
		bitsSize += dic.getWord(i).getFreq() * dic.getWord(i).getHuffBinCodeSize();
	}

	return bitsSize;
}

char **Huffman::makeLookupTable(Dictionary &dic)
{
	Dictionary temp = dic;

	//temp.sortByHuffCodeSize();

	char **firstLookupTable = new char*[LOOKUP_TABLE_SIZE];

	for (int i = 0; i < LOOKUP_TABLE_SIZE; i++)
	{
		/*Each block of table has LOOKUP_TABLE_ENTRY_SIZE size.
		The first byte of each block is used to keep character.
		This character is used to replace if input binary string 
		maches the index of the block.
		The last byte is used to identify a block if it is a pointer
		to another table. If == 1 it is a pointer.
		The rest of the block bytes are used to store pointer or
		number of bits that must be removed from the input stream.*/
		firstLookupTable[i] = new char[LOOKUP_TABLE_ENRTY_SIZE];

		firstLookupTable[i][LOOKUP_TABLE_ENRTY_SIZE - 1] = 0;
	}

	Word word;

	for (int i = 0; i < temp.size(); i++)
	{
		word = temp.getWord(i);

		addWordToLookup(firstLookupTable, word);
	}
	return firstLookupTable;
}

void Huffman::addWordToLookup(char** &rootTable, Word word)
{
	if (word.getHuffBinCodeSize() == 0)
	{
		return;
	}

	if (word.getHuffBinCodeSize() <= BIT_IN_NUMBER)
	{
		//The following line calculates the start index to be filled with pair
		//(character, huffBinCodeSize), if any, from there.
		unsigned int startIndex = reverseHuffBinCoce(word.getHuffBinCode(), BIT_IN_NUMBER);

		//The following create a mask to add (sizeof(UINT) - huffBinCodeSize) one
		//to the right of the huffBinCode. This mask is used to calculates the endIndex
		//that must be filled with the pair
		unsigned int mask = UINT_MAX << (word.getHuffBinCodeSize());

		//The following line calculates the end index to be filled with pair
		//(character, huffBinCode), if any (and not pointer to another table)
		//
		unsigned int endIndex = reverseHuffBinCoce(
			word.getHuffBinCode() | mask,
			BIT_IN_NUMBER);

		for (startIndex; startIndex <= endIndex; startIndex++)
		{
			//The following line assign the first byte of a block with character
			rootTable[startIndex][0] = word.getChar();

			//The following line assign middle byte the number of bits that must be
			//removed from the input binary string
			*(long long *)(rootTable[startIndex] + 1) = word.getHuffBinCodeSize();

			//Because we have all bits to identify the exact character, this block is not
			//a pointer. So the last byte must be set to 0
			rootTable[startIndex][LOOKUP_TABLE_ENRTY_SIZE - 1] = 0;
		}
	}
	else
	{
		/*If we have a character with more than BIN_IN_NUMBER in huffBinCodeSize
		then we must use two or more tables. And the corresponding block must
		points to the rigtht table that we create it in following lines of code.*/

		int index = reverseHuffBinCoce(word.getHuffBinCode() & FIRST_NINE_BITS_MASK,
			BIT_IN_NUMBER);

		if (!rootTable[index][LOOKUP_TABLE_ENRTY_SIZE - 1])
		{
			char **temp = new char*[LOOKUP_TABLE_SIZE];

			for (int i = 0; i < LOOKUP_TABLE_SIZE; i++)
			{
				temp[i] = new char[LOOKUP_TABLE_ENRTY_SIZE];

				temp[i][LOOKUP_TABLE_ENRTY_SIZE - 1] = 0;
			}
			
			*(long long*)(rootTable[index] + 1) = (long long)(temp);

			rootTable[index][LOOKUP_TABLE_ENRTY_SIZE - 1] = 1;
		}
		
		char **child = (char**)(*(long long*)(rootTable[index] + 1));
		
		word.setHuffBinCodeSize(word.getHuffBinCodeSize() - BIT_IN_NUMBER);

		word.setHuffBinCode(word.getHuffBinCode() >> BIT_IN_NUMBER);

		addWordToLookup(child, word);
	}

}

unsigned int Huffman::reverseHuffBinCoce(unsigned int huffBinCode, int size)
{
	int temp = 0;

	for (int i = 0; i < size; i++)
	{
		temp = ((huffBinCode >> i) & 1) | (temp << 1);
	}
	return temp;
}

void Huffman::deleteHuffmanTree(Node *root)
{
	if (root == nullptr)
	{
		return;
	}

	deleteHuffmanTree(root->getLeftChild());
	deleteHuffmanTree(root->getRightChild());

	delete root;
}

void Huffman::deleteLookupTable(char** root)
{
	for (int i = 0; i < LOOKUP_TABLE_SIZE; i++)
	{
		if (root[i][LOOKUP_TABLE_ENRTY_SIZE - 1])
		{
			char** address = (char**)*(long long *)(root[i] + 1);

			deleteLookupTable(address);
		}

		delete[] root[i];
	}

	delete[] root;
}
//Class FileOutput

FileOutput::FileOutput(string path)
{
	this->path = path;
	this->outputStream.open(path, ios::binary);
}

FileOutput::~FileOutput()
{
	this->outputStream.close();
}

void FileOutput::write(File file)
{
	this->outputStream.write(file.getData(), file.size());
}

void FileOutput::close()
{
	this->outputStream.close();
}

void FileOutput::write(CompressedFile cFile)
{
	unsigned int dSize = cFile.getDic().size();
	unsigned int dRSize = cFile.getDic().getRealSize();

	outputStream.write((char *)(&dRSize), sizeof(unsigned int));

	Word word;

	unsigned int f;

	char c;

	for (int i = 0; i < dSize; i++)
	{
		word = cFile.getDic().getWord(i);

		c = word.getChar();

		f = word.getFreq();

		if (f != 0)
		{
			outputStream.write(&c, sizeof(char));
			outputStream.write((char *)(&f), sizeof(unsigned int));
		}
	}

	unsigned int cFileSize = cFile.size();

	outputStream.write((char *)(&cFileSize), sizeof(unsigned int));

	outputStream.write(cFile.getData(), cFile.size());
}

//Class CompressedFile

CompressedFile::CompressedFile(char *data, unsigned int dataSize, Dictionary dic)
	: File(data, dataSize)
{
	this->dic = dic;
}

Dictionary CompressedFile::getDic()
{
	return this->dic;
}

//Class Node
Node::Node()
{
	this->leftChild = nullptr;
	this->rightChild = nullptr;
	this->tFreq = 0;
	this->data = nullptr;
}

void Node::setLeftChild(Node* lNode)
{
	this->leftChild = lNode;
}

void Node::setRightChild(Node* rNode)
{
	this->rightChild = rNode;
}

void Node::setWord(Word &data)
{
	this->data = &data;
}

Node *Node::getLeftChild() const
{
	return this->leftChild;
}

Node *Node::getRightChild() const
{
	return rightChild;
}

Word &Node::getWord() const
{
	return *this->data;
}

void Node::setTFreq(unsigned int tFreq)
{
	this->tFreq = tFreq;
}

unsigned int Node::getTFreq()
{
	return this->tFreq;
}

bool Node::isLeaf() const
{
	if (leftChild == nullptr && rightChild == nullptr)
	{
		return true;
	}

	return false;
}
