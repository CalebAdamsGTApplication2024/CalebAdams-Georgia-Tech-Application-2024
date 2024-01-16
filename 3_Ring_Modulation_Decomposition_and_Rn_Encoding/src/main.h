#pragma once
#include <vector>
#include <iostream>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <iomanip>
#include <cmath>


//typedef double(*RuleOperation)(double, double);
//typedef std::vector<double>(*xSolution)(double, double, double, double);

class Encoder
{
public:
	Encoder();
	~Encoder();

	std::vector<double> r(std::vector<double> a);
	void rFinal(std::vector<double> a);
	
	std::vector<double> createTest(std::vector<double> a);
	bool correctIncorrect();
	void reset();
	void setTestVals(std::vector<double> a);

	void test1();
	void test2();
	void rmdQuantizationTest();

	void printSolvedValues();

private:
	std::vector<double> solvedValues;

	std::vector<double> x1Solution1(double a1, double a2, double a3, double a4);
	std::vector<double> x1Solution2(double a1, double a2, double a3, double a4);
	std::vector<double> x1Solution3(double a1, double a2, double a3, double a4);

	std::vector<double> testVals;

	unsigned int trueNum = 0;
	unsigned int falseNum = 0;

	std::vector<double> cErrors;
};



#include "main.h"

// Solutions for x
std::vector<double> Encoder::x1Solution1(double a1, double a2, double a3, double a4)
{
	std::vector<double> outVec;
	outVec.push_back((a1 + a2) / 2.);
	outVec.push_back((a1 - a2) / 2.);
	for (auto i = 0; i < outVec.size(); ++i)
		if (outVec[i] <= 0)
			outVec.erase(outVec.begin() + i);
	return outVec;
}

std::vector<double> Encoder::x1Solution2(double a1, double a2, double a3, double a4)
{
	std::vector<double> outVec;
	outVec.push_back(a1 - a3 / 2. + a4 / 2.);
	outVec.push_back(a1 - a3 / 2. - a4 / 2.);
	for (auto i = 0; i < outVec.size(); ++i)
		if (outVec[i] <= 0)
			outVec.erase(outVec.begin() + i);
	return outVec;
}

std::vector<double> Encoder::x1Solution3(double a1, double a2, double a3, double a4)
{
	std::vector<double> outVec;
	outVec.push_back(a3 / 2. + a4 / 2. + a2);
	outVec.push_back(a3 / 2. - a4 / 2. + a2);
	outVec.push_back(a3 / 2. + a4 / 2. - a2);
	outVec.push_back(a3 / 2. - a4 / 2. - a2);
	for (auto i = 0; i < outVec.size(); ++i)
		if (outVec[i] <= 0)
			outVec.erase(outVec.begin() + i);
	return outVec;
}

Encoder::Encoder()
{
}

Encoder::~Encoder()
{
}

std::vector<double> Encoder::createTest(std::vector<double> a)
{
	std::vector<double> outVec;
	outVec.push_back(a[0]);

	for (auto i = 1; i < a.size(); ++i)
	{
		std::vector<double> calculationBlock;
		for (auto frequency : outVec)
		{
			calculationBlock.push_back(abs(frequency - a[i]));
			calculationBlock.push_back(frequency + a[i]);
		}
		
		outVec = calculationBlock;
	}

	std::sort(outVec.begin(), outVec.end());
	
	std::sort(a.begin(), a.end());
	testVals = a;

	return outVec;
}

std::vector<double> Encoder::r(std::vector<double> a)
{
	double a1 = a[3];
	double a2 = a[0];
	double a3 = a[2];
	double a4 = a[1];

	double y;
	std::vector<double> xSolutions;
	std::vector<double> outVec;

	auto tempSolutions = x1Solution1(a1, a2, a3, a4);
	for (auto temp : tempSolutions)
		xSolutions.push_back(temp);

	tempSolutions = x1Solution2(a1, a2, a3, a4);
	for (auto temp : tempSolutions)
		xSolutions.push_back(temp);
	for (auto temp : tempSolutions)
		xSolutions.push_back(temp);

	tempSolutions = x1Solution3(a1, a2, a3, a4);
	for (auto temp : tempSolutions)
		xSolutions.push_back(temp);
	for (auto temp : tempSolutions)
		xSolutions.push_back(temp);

	// round values to reduce problems with comparing doubles for equivalence
	std::sort(xSolutions.begin(), xSolutions.end());
	for (auto i = 0; i < xSolutions.size(); ++i)
		xSolutions[i] = std::round(xSolutions[i] * 1000.) / 1000.;


	double currentNumber = xSolutions[0];
	std::vector<double> mode;
	mode.push_back(currentNumber);
	unsigned int largestCount = 1;
	unsigned int currentCount = 1;

	for (auto i = 1; i < xSolutions.size(); ++i)
	{
		if (xSolutions[i] == currentNumber)
			currentCount += 1;
		else
		{
			if (currentCount > largestCount)
			{
				largestCount = currentCount;
				mode.clear();
				mode.push_back(currentNumber);
			}
			else if (currentCount == largestCount)
				mode.push_back(currentNumber);
			currentCount = 1;
			currentNumber = xSolutions[i];
		}
	}

	// if 1 obvious mode is present
	if (mode.size() == 1)
	{
		// find y
		y = a1 - mode[0];
		solvedValues.push_back(y);
		// find values of other xs 
		// x1
		outVec.push_back(a1 - y);
		// x2
		outVec.push_back(a3 - y);

	}
	else {
		outVec.push_back(0.);
		outVec.push_back(0.);
	}

	std::sort(outVec.begin(), outVec.end());
	return outVec;
}

void Encoder::rFinal(std::vector<double> a)
{
	auto xVals = x1Solution1(a[1], a[0], 0.0, 0.0);
	std::vector<double> yVals;
	std::vector<double> outVec;

	for (auto val : xVals)
	{
		yVals.push_back(a[1] - val);
		yVals.push_back(val + a[0]);
		yVals.push_back(val - a[0]);
	}

	// round values to reduce problems with comparing doubles for equivalence
	for (auto i = 0; i < yVals.size(); ++i)
		yVals[i] = std::round(yVals[i] * 1000.) / 1000.;

	double currentNumber = yVals[0];
	std::vector<double> mode;
	mode.push_back(currentNumber);
	unsigned int largestCount = 1;
	unsigned int currentCount = 1;

	for (auto i = 1; i < yVals.size(); ++i)
	{
		if (yVals[i] == currentNumber)
			currentCount += 1;
		else
		{
			if (currentCount > largestCount)
			{
				largestCount = currentCount;
				mode.clear();
				mode.push_back(currentNumber);
			}
			else if (currentCount == largestCount)
				mode.push_back(currentNumber);
			currentCount = 1;
			currentNumber = yVals[i];
		}
	}

	// 1 or 2, because for some solutions both correct solutions are equally present
	if (mode.size() == 1 || mode.size() == 2)
	{
		outVec.push_back(mode[0]);
		outVec.push_back(a[1] - mode[0]);
	}

	
	

	for (auto val : outVec)
		solvedValues.push_back(val);

	std::sort(solvedValues.begin(), solvedValues.end());
	
}

bool Encoder::correctIncorrect()
{	
	// round values to reduce problems with comparing doubles for equivalence
	for (auto i = 0; i < testVals.size(); ++i)
		testVals[i] = std::round(testVals[i] * 1000.) / 1000.;
	for (auto i = 0; i < solvedValues.size(); ++i)
		solvedValues[i] = std::round(solvedValues[i] * 1000.) / 1000.;
	
	for (auto i = 0; i < testVals.size(); ++i)
		if (testVals[i] != solvedValues[i])
			return false;
			

	return true;
}

void Encoder::reset()
{
	testVals.clear();
	solvedValues.clear();
}

void Encoder::setTestVals(std::vector<double> a)
{
	testVals = a;
}

void Encoder::printSolvedValues()
{
	std::cout << "Solved Values:\t";
	for (auto val : solvedValues)
		std::cout << val << ", ";
	std::cout << std::endl;
}

double random(int min, int max)
{
	static bool first = true;
	if (first)
	{
		srand(time(NULL));
		first = false;
	}

	auto decimalValue = (rand() % (max + 1)) / 100.;
	auto val = (min + rand() % ((max + 1) - min)) + decimalValue;
	auto roundedVal = std::round(val * 1000.) / 1000.;

	return roundedVal;
}

void Encoder::test1()
{
	std::cout << "Running Test 1" << std::endl;
	
	// vector a
	std::vector<double> testVec;
	testVec.push_back(random(1, 100));
	testVec.push_back(random(1, 100));
	testVec.push_back(random(1, 100));
	
	std::sort(testVec.begin(), testVec.end());

	std::cout << "Test Vals:\t";
	for (auto val : testVec)
		std::cout << val << ", ";

	std::cout << std::endl;

	// R^-1(a)
	std::vector<double> test = createTest(testVec);

	std::cout << "Expanded Test Vals:\t";
	for (auto val : test)
		std::cout << val << ", ";

	std::cout << std::endl;

	auto val = r(test);

	// R(R^-1(a))
	rFinal(val);

	printSolvedValues();

	bool correct = correctIncorrect();

	if (correct == true)
	{
		trueNum += 1;
		std::cout << "Set has value: true" << "\nNumber of true: "
			<< trueNum << "\nNumber of false: " << falseNum << "\n" << std::endl;
	}
	else {
		falseNum += 1;
		std::cout << "Set has value: false" << "\nNumber of true: "
			<< trueNum << "\nNumber of false: " << falseNum << "\n" << std::endl;
	}
	
	reset();
}

void Encoder::test2()
{
	std::cout << "Running Test 2" << std::endl;

	// vector b
	std::vector<double> testVec;
	testVec.push_back(random(1, 100));
	testVec.push_back(random(1, 100));
	testVec.push_back(random(1, 100));
	testVec.push_back(random(1, 100));


	std::sort(testVec.begin(), testVec.end());
	

	std::cout << "Test Vals: " << std::endl;
	for (auto val : testVec)
		std::cout << val << "\t";

	std::cout << std::endl;

	setTestVals(testVec);

	// R(b) -- Note, test fails here, vector b fails to successfully reduce. 
	// An incredibly small percentage of the time the reduction doesn't fail.
	// See section 2.3 and rmdQuantizationTest() for explanation and quantization process
	auto val = r(testVec);

	rFinal(val);

	bool correct = correctIncorrect();

	if (correct == true)
	{
		trueNum += 1;
		std::cout << "Set has value: true" << "\nNumber of true: "
			<< trueNum << "\nNumber of false: " << falseNum << "\n" << std::endl;
	}
	else {
		falseNum += 1;
		std::cout << "Set has value: false" << "\nNumber of true: "
			<< trueNum << "\nNumber of false: " << falseNum << "\n" << std::endl;
	}

	reset();
}

void Encoder::rmdQuantizationTest()
{
	std::cout << "Running RMD Quantization Test" << std::endl;

	// vector b
	std::vector<double> testVec;
	testVec.push_back(random(1, 100));
	testVec.push_back(random(1, 100));
	testVec.push_back(random(1, 100));
	testVec.push_back(random(1, 100));

	std::sort(testVec.begin(), testVec.end());

	std::cout << "Test Vals: " << "\t";
	for (auto val : testVec)
		std::cout << val << ", ";
	std::cout << std::endl;
	
	double c0 = testVec[0] + testVec[3];
	double c1 = testVec[1] + testVec[2];

	double cSum = 0;
	for (auto val : testVec)
		cSum += val;

	double cError0 = cSum - 2 * c0;
	double cError1 = cSum - 2 * c1;
	std::cout << "C-Error0: " << cError0 << std::endl;
	std::cout << "C-Error1: " << cError1 << std::endl;

	
	// RMD Quantization
	testVec[0] += (testVec[0] / (cSum)) * cError0;
	testVec[1] += (testVec[1] / (cSum)) * cError1;
	testVec[2] += (testVec[2] / (cSum)) * cError1;
	testVec[3] += (testVec[3] / (cSum)) * cError0;

	std::sort(testVec.begin(), testVec.end());
	std::cout << "C Error Quantized test vec: ";
	for (auto val : testVec)
		std::cout << val << ", ";
	std::cout << std::endl;

	auto val = r(testVec);

	// R(b)
	rFinal(val);

	printSolvedValues();

	// R^-1(R(b))
	std::vector<double> expandedVec = createTest(solvedValues);

	std::cout << "Expanded vec: ";
	for (auto val : expandedVec)
		std::cout << val << ", ";
	std::cout << std::endl;

	bool correct = correctIncorrect();

	if (correct == true)
	{
		trueNum += 1;
		std::cout << "Set has value: true" << "\nNumber of true: "
			<< trueNum << "\nNumber of false: " << falseNum << "\n" << std::endl;
	}
	else {
		falseNum += 1;
		std::cout << "Set has value: false" << "\nNumber of true: "
			<< trueNum << "\nNumber of false: " << falseNum << "\n" << std::endl;
	}

	reset();
}