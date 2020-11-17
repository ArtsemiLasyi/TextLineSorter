// TextLineSorter.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <windows.h>
#include <fstream>
#include <string>
#include "ConcurrentTaskQueue.h"
#include "QueueExecutor.h"

#define BUFFER_SIZE 1024

typedef std::vector<std::string> StringVector;

bool GetLinesOfFile(std::string filename, StringVector* vector);

void PrintVector(StringVector* vector);

void WriteLinesToFile(std::string filename, StringVector* vector);

StringVector MergeTwoVectors(StringVector firstvector, StringVector secondvector);

StringVector MergeAllVectors(std::vector<StringVector*>* vectors);

void SplitAndSortVectors(std::vector<StringVector*>* vectors, StringVector* strings, ConcurrentTaskQueue* taskQueue);

std::wstring GetDirectory();

int main()
{
	ConcurrentTaskQueue* taskQueue;
	QueueExecutor* queueExecutor;
	StringVector* strings = new StringVector();
	std::vector<StringVector*>* vectors = new std::vector<StringVector*>();
	int numberOfThreads;

	std::string oldFileName;
	std::string newFileName;
	std::cout << "Welcome to stringsorter!" << std::endl;
	std::cout << "Write the name of file to sort" << std::endl;
	std::wcout << "Current directory is: " << GetDirectory() << std::endl;
	std::cin >> newFileName;
	oldFileName = newFileName;
	newFileName.insert(newFileName.find('.', 1), "new");
	std::cout << oldFileName << std::endl;
	std::cout << newFileName << std::endl;

	if (!GetLinesOfFile(oldFileName, strings))
	{
		std::cout << "This file does not exist!" << std::endl;
		return -1;
	}

	std::cout << "Input the number of threads to sort all lines" << std::endl;
	std::cin >> numberOfThreads;

	taskQueue = new ConcurrentTaskQueue();
	queueExecutor = new QueueExecutor(taskQueue, numberOfThreads);

	SplitAndSortVectors(vectors, strings, taskQueue);

	queueExecutor->Start();

	PrintVector(strings);
	StringVector sortedStrings = MergeAllVectors(vectors);
	WriteLinesToFile(newFileName, &sortedStrings);

	

	delete queueExecutor;
	delete taskQueue;
}


void PrintVector(StringVector* vector)
{
	for (unsigned int i = 0; i < vector->size(); i++)
	{
		std::cout << vector->data()[i];
	}
}


std::wstring GetDirectory()
{
	WCHAR buffer[BUFFER_SIZE];
	GetCurrentDirectory(BUFFER_SIZE, buffer);
	return std::wstring(buffer);
}

void WriteLinesToFile(std::string filename, StringVector* vector)
{
	std::ofstream fileStream;
	fileStream.open(filename, std::ios::out);
	for (unsigned int i = 0; i < vector->size(); i++)
	{
		fileStream << vector->data()[i];
	}
}


bool GetLinesOfFile(std::string filename, StringVector* stringsvector)
{
	std::ifstream fileStream(filename);
	std::string line;

	if (!fileStream.good())
		return false;

	while (std::getline(fileStream, line))
	{
		if (line.size() != 0)
		{
			std::string newLine;
			newLine = line + "\n";
			stringsvector->push_back(newLine);
		}
	}
	return true;
}

void SplitAndSortVectors(std::vector<StringVector*>* vectors, StringVector* strings, ConcurrentTaskQueue* taskQueue)
{
	for (int i = 0; i < strings->size(); i++) 
	{
		StringVector* newVector = new StringVector();
		vectors->push_back(newVector);
		std::string str = strings->data()[i];
		newVector->push_back(str);
		taskQueue->WinPush([newVector]() { std::sort(newVector->begin(), newVector->end());});
	}
}


StringVector MergeTwoVectors(StringVector firstvector, StringVector secondvector)
{ 
	int firstVectorSize = firstvector.size();
	int secondVectorSize = secondvector.size();

	StringVector resultVector;
	resultVector.reserve(firstVectorSize + secondVectorSize);

	int i = 0;
	int	j = 0;
	while (i < firstVectorSize && j < secondVectorSize)
	{
		if (firstvector.data()[i] <= secondvector.data()[j])
			resultVector.push_back(firstvector.data()[i++]);
		else
			resultVector.push_back(secondvector.data()[j++]);
	}
 
	while (i < firstVectorSize)
		resultVector.push_back(firstvector[i++]);

	while (j < secondVectorSize)
		resultVector.push_back(secondvector[j++]);

	return resultVector;
}

StringVector MergeAllVectors(std::vector<StringVector*>* vectors)
{
	StringVector tempVector;
	if (vectors->size() > 0)
		tempVector = *(*vectors).data()[0];
	for (int i = 1; i < vectors->size(); i++)
		tempVector = MergeTwoVectors(tempVector, *(*vectors).data()[i]);
	return tempVector;
}