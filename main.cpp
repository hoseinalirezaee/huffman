#include <iostream>
#include "huffman.h"
#include <fstream>
#include <chrono>
#include <cstring>


//#define COMMAND
#define COMPRESS


int main()
{

#ifndef COMMAND
#ifdef COMPRESS


	chrono::high_resolution_clock::time_point inS, inT, S, T, outS, outT;

	
	string path = "test.txt";// path to file that should be compressed

	inS = chrono::high_resolution_clock::now();

	FileInput input(path);

	File file = input.read();

	input.close();

	inT = chrono::high_resolution_clock::now();

	S = chrono::high_resolution_clock::now();

	CompressedFile cFile = Huffman::compress(file);

	
	T = chrono::high_resolution_clock::now();

	FileOutput output("1.huff");//path to where compressed file should be saved

	outS = chrono::high_resolution_clock::now();

	output.write(cFile);
	output.close();

	outT = chrono::high_resolution_clock::now();

	

	double inTIME = chrono::duration_cast<chrono::milliseconds>(inT - inS).count() / 1000.0;
	double TIME = chrono::duration_cast<chrono::milliseconds>(T - S).count() / 1000.0;
	double outTIME = chrono::duration_cast<chrono::milliseconds>(outT - outS).count() / 1000.0;


	std::cout << "File read time: " << inTIME << "\n";
	std::cout << "File compressing time: " << TIME << "\n";
	std::cout << "File write time: " << outTIME << endl;
	std::cout << "Overall taken time: " << inTIME + TIME + outTIME << endl;

	
	if (file.size() > 1024 * 1024)
	{
		std::cout << (file.size() / (1024.0 * 1024.0)) << " (MB) ";
	}
	else if (file.size() > 1024)
	{
			std::cout << (file.size() / 1024.0) << " (KB) ";
		
	}
	else
	{
		std::wcout << file.size() << " (B) ";
	}

	std::wcout << "---------> ";

	if (file.size() > 1024 * 1024)
	{
		std::cout << (cFile.size() / (1024.0 * 1024.0)) << " (MB) ";
	}
	else if (file.size() > 1024)
	{
		std::cout << (cFile.size() / 1024.0) << " (KB) ";

	}
	else
	{
		std::wcout << cFile.size() << " (B) ";
	}

	std::wcout << endl;

#endif
#ifdef DECOMPRESS

	chrono::high_resolution_clock::time_point inS, inT, S, T, outS, outT;

	string path = "1.huff";


	FileInput input(path);

	inS = chrono::high_resolution_clock::now();

	CompressedFile cFile = input.readCompressedFile();

	input.close();

	inT = chrono::high_resolution_clock::now();

	S = chrono::high_resolution_clock::now();

	File file = Huffman::decompress(cFile);

	T = chrono::high_resolution_clock::now();

	FileOutput output("yellow.mp3");

	outS = chrono::high_resolution_clock::now();

	output.write(file);

	output.close();

	outT = chrono::high_resolution_clock::now();

	double inTIME = chrono::duration_cast<chrono::milliseconds>(inT - inS).count() / 1000.0;
	double TIME = chrono::duration_cast<chrono::milliseconds>(T - S).count() / 1000.0;
	double outTIME = chrono::duration_cast<chrono::milliseconds>(outT - outS).count() / 1000.0;

	std::cout << "File read time: " << inTIME << "\n";
	std::cout << "File decompressing time: " << TIME << "\n";
	std::cout << "File write time: " << outTIME << endl;
	std::cout << "Overall taken time: " << inTIME + TIME + outTIME << endl;

#endif // DECOMPRESS
#endif // !COMMAND

#ifdef COMMAND

	cout << "Enter mode (compress/decompress): ";

	string mode;

	cin >> mode;

	cout << "Enter source location: ";

	string source;

	cin >> source;

	cout << "Enter destination location: ";

	string destination;

	cin >> destination;

	
	chrono::high_resolution_clock::time_point s, t;

	if (!mode.compare("compress"))
	{
		FileInput input(source);

		File file = input.read();

		input.close();

		s = chrono::high_resolution_clock::now();

		CompressedFile cFile = Huffman::compress(file);

		t = chrono::high_resolution_clock::now();

		FileOutput output(destination);

		output.write(cFile);

		output.close();

		file.deleteFile();

		cFile.deleteFile();
	}
	else if (!mode.compare("decompress"))
	{
		FileInput input(source);

		CompressedFile cFile = input.readCompressedFile();

		input.close();

		s = chrono::high_resolution_clock::now();

		File file = Huffman::decompress(cFile);

		t = chrono::high_resolution_clock::now();

		FileOutput output(destination);

		output.write(file);

		output.close();

		cFile.deleteFile();

		file.deleteFile();
	}

	cout << "Operation done successfully. \n";
	cout << "Taken time is : " << chrono::duration_cast<chrono::nanoseconds>(t - s).count() << endl;
		
#endif // COMMAND

	

	system("pause");
}