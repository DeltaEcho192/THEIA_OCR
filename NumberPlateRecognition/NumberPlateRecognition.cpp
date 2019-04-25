// NumberPlateRecognition.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//TODO Using sockets to transport file to be proccessed information

#include "pch.h"
#include <iostream>
#include <mysql.h>
#include <vector>
#include <filesystem>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <typeinfo>
#include <ctime>
#include <thread>
#include <filesystem>
#include <iostream> 
#include <string.h> 


using namespace std;

void PlateCheck(const vector<string> files,const string path,const char * address,const char * username, const char * password, const char * database,int port)
{
    int fileAmt = files.size();
    vector<string> PlateName;
    vector<double> PlateConfidence;
    string finalPath;

    for(int i = 0; i < fileAmt; i++)
    {
        fileName = files[i];
        finalPath = path + fileName;
        
        // Recognize an image file. Alternatively, you could provide the image bytes in-memory.
        alpr::AlprResults results = openalpr.recognize(finalPath);

        // Carefully observe the results. There may be multiple plates in an image,
        // and each plate returns the top N candidates.
        for (int i = 0; i < results.plates.size(); i++)
        {
            alpr::AlprPlateResult plate = results.plates[i];
            std::cout << "plate" << i << ": " << plate.topNPlates.size() << " results" << std::endl;

                for (int k = 0; k < plate.topNPlates.size(); k++)
                {
                    alpr::AlprPlate candidate = plate.topNPlates[k];
                    string Plate = candidate.character;
                    double confidence = candidate.overall_confidence;
                    PlateName.push_back(Plate);
                    PlateConfidence.push_back(confidence);
                }
        }

    }
    //Meta Datas

    if(PlateName.size() != PlateConfidence)
    {
        cout << "There was error with the Plate arrays size - 006" << endl;
        exit(EXIT_FAILURE);
    }

    //SQL Insert
    MYSQL * conn;
	conn = mysql_init(0);


	conn = mysql_real_connect(conn, address, username, password, database, port, NULL, 0);


	if (conn)
	{
		puts("Successful connection to database!");
        //!!!! adjust Names
		for(int i = 0; i < PlateName.size(); i++)
		{
            //!!!!! Change
			string query = "INSERT INTO dir_list_test (name) values ('" + newFiles[i] + "');";
			cout << query << endl;
			const char* q = query.c_str();
			qstate = mysql_query(conn, q);
			cout << qstate << endl;
			if(qstate == 0)
			{
				cout << "Insertion was successful" << endl;
			}
			else
			{
				cout << "There was a failure - 003" << mysql_error(conn) << endl;
			}
		}
	}
	else
	{
		puts("Connection to database had failed! - 002");
	}


}

int main()
{
    //TODO Getting path from config file
    //TODO Create config reader

    //SOCKET CODE Recives Array with file Names to be proccessed
    alpr::Alpr openalpr("us", "/path/to/openalpr.conf");

    // Optionally, you can specify the top N possible plates to return (with confidences). The default is ten.
    openalpr.setTopN(1);

    // Optionally, you can provide the library with a region for pattern matching. This improves accuracy by
    // comparing the plate text with the regional pattern.
    openalpr.setDefaultRegion("md");

    // Make sure the library loads before continuing.
    // For example, it could fail if the config/runtime_data is not found.
    if (openalpr.isLoaded() == false)
    {
        std::cerr << "Error loading OpenALPR - 007" << std::endl;
        return 1;
    }



}

