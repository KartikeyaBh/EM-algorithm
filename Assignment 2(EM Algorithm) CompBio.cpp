// Assignment 2(EM Algorithm) CompBio.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define _USE_MATH_DEFINES

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;


// Todo: Take filename from command line and as parameter here.
vector<double> ReadFileToVector()
{
    ifstream inFile;
    inFile.open("dataFile.txt");

    vector<double> ret;
    std::string   line;
    // Read one line at a time into the variable line:
    while (getline(inFile, line))
    {
        std::stringstream  lineStream(line);

        double value;
        // Read an integer at a time from the line
        while (lineStream >> value)
        {
            ret.push_back(value);
        }
    }

    return ret;
}

class EMClustering
{
public:
    EMClustering(vector<double> trainingSet):
        m_trainingSet(trainingSet)
        {
        }

    void Train();
private:
    vector<double> m_trainingSet;
};

void EMClustering::Train()
{
    // Define the number of clusters.
    int numOfClusters = 1;
    int countRestart = 1;

    for (double clusterCount = 3; clusterCount <= 5; clusterCount++)
    {
        for (int restartTimes = 0; restartTimes < countRestart; restartTimes++)
        {
            // Initialize means with random inputs
            vector<double> means = vector<double>(clusterCount, 0);
            vector<vector<double>> z = vector<vector<double>>(m_trainingSet.size(), vector<double>(clusterCount, 0));
            for (int k = 0; k < clusterCount; k++)
            {
                int trainingCount = m_trainingSet.size();
                int usableRandomMax = ((RAND_MAX) / (trainingCount));
                usableRandomMax = usableRandomMax * (trainingCount);
                int randomIndex = RAND_MAX;
                while (randomIndex > usableRandomMax) { randomIndex = rand(); }
                randomIndex = randomIndex % (trainingCount);
                means[k] = m_trainingSet[randomIndex];
            }

            bool shouldContinue = true;
            int iteration = 0;
            double logLikelihood = 0;
            while (shouldContinue)
            {
                // E step
                for (int i = 0; i < m_trainingSet.size(); i++)
                {
                    double denominator = 0;
                    double numerator = 0;
                    for (int j = 0; j < clusterCount; j++)
                    {
                        double x_minus_u = m_trainingSet[i] - means[j];
                        denominator += exp( -0.5 * x_minus_u * x_minus_u);
                    }

                    for (int j = 0; j < clusterCount; j++)
                    {
                        double x_minus_u = m_trainingSet[i] - means[j];
                        numerator = exp( -0.5 * x_minus_u * x_minus_u);
                        if (numerator == 0 || denominator == 0) 
                        {
                            z[i][j] = 0;
                        }
                        else
                        {
                            z[i][j] = numerator / denominator;
                        }
                    }
                }

                // M step
                // We need to calculate mean of every cluster.
                // By multiplying zij with xi
                for (int i = 0; i < clusterCount; i++)
                {
                    double numerator = 0;
                    double denominator = 0;
                    for (int j = 0; j < m_trainingSet.size(); j++)
                    {
                        double tempNum = z[j][i] * m_trainingSet[j];
                        numerator += tempNum;
                        double tempDeno = z[j][i];
                        denominator += tempDeno;
                    }
                    means[i] = numerator / denominator;
                }

                // check if we should continue or not
                if (iteration != 0)
                {
                    double newLogLikelihood = 0;
                    for (int i = 0; i < m_trainingSet.size(); i++)
                    {
                        double prob_i = 0;
                        for (int j = 0; j < clusterCount; j++)
                        {
                            double x_minus_u = m_trainingSet[i] - means[j];
                            prob_i += (1.0/sqrt(2.0 * M_PI)) * exp( -0.5 * x_minus_u * x_minus_u );
                            prob_i = prob_i / clusterCount;
                        }
                        newLogLikelihood += log(prob_i);
                    }
                    newLogLikelihood = newLogLikelihood;

                    if (abs(newLogLikelihood - logLikelihood) < 0.001)
                    {
                        shouldContinue = false;
                    }
                    else
                    {
                        logLikelihood = newLogLikelihood;
                    }
                }

                iteration++;
            }
        }
    }
}

int main()
{
    EMClustering emClustering(ReadFileToVector());

    emClustering.Train();
    return 0;
}

