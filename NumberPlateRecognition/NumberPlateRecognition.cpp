// NumberPlateRecognition.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//TODO Using sockets to transport file to be proccessed information

#include "pch.h"
#include <iostream>
#include <mysql.h>
#include "TinyEXIF.h"
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

    MYSQL * conn;
	conn = mysql_init(0);


	conn = mysql_real_connect(conn, address, username, password, database, port, NULL, 0);


    for(int i = 0; i < fileAmt; i++)
    {
        
        finalPath = path + files[i];
        
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
		//Meta Datas
		vector<double> lattitude = NULL;
		vector<double> longitude = NULL;
		string imageTime = NULL;

		// read entire image file
		std::ifstream file(files[i], std::ifstream::in | std::ifstream::binary);
		file.seekg(0, std::ios::end);
		std::streampos length = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<uint8_t> data(length);
		file.read((char*)data.data(), length);

		// parse image EXIF and XMP metadata
		TinyEXIF::EXIFInfo imageEXIF(data.data(), length);

		if (imageEXIF.GeoLocation.hasLatLon()) {
			lattitude = imageEXIF.GeoLocation.Latitude ;
			longitude = imageEXIF.GeoLocation.Longitude;
		}

		imageTime = imageEXIF.DateTime;


        if (PlateName.size() != PlateConfidence.size())
        {
            cout << "There was error with the Plate arrays size - 006" << endl;
            exit(EXIT_FAILURE);
        }

        if (conn)
        {
            puts("Successful connection to database!");
            for(int i = 0; i < PlateName.size(); i++)
            {
                string query = "INSERT INTO numberplates_testing (plate,confidence,lattitude,longitude,time) values (" + PlateName[i] + ","+ PlateConfidence[i]+","+lattitude+","+longitude+","+imageTime");";
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



	

}

vector<string> fileQuery(string query, const char * address, const char * username, const char * password, const char * database,int port)
{
    vector<string> files;
    MYSQL * conn;
	MYSQL_ROW row;
	MYSQL_RES *res;
	conn = mysql_init(0);


	conn = mysql_real_connect(conn, address, username, password, database, port, NULL, 0);


	if (conn)
	{
		puts("Successful connection to database!");


		const char* q = query.c_str();
		qstate = mysql_query(conn, q);
		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
			{
				files.push_back(row[0]);
			}
		}
		else
		{
			cout << "Query failed - 001: " << mysql_error(conn) << endl;
		}
	}
	else
	{
		puts("Connection to database had failed! - 002");
	}
    return files;

}

vector<string> config(string configPath)
{
    string file = "OCR_config.txt";
    
    ifstream config(configPath);
    vector<string> settings;
    string line;
    string output;

    if(config.is_open())
    {
        cout << "Config File Was Opened." << endl;
    }
    else
    {
        cout << "There was a error opening the config file. -005" << endl;

    }

    while ( getline(config,line) )    
    // get next line in file
    {
        stringstream ss(line);

        ss >> output;
        settings.push_back(output);
        
    }
    return settings;
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

    vector<string> fileTODO;
    
    fs::path currentPath = fs::current_path();
    //cout << currentPath.parent_path() << endl;
    string configFileName = "\\OCR_config.txt";
    std::ostringstream configStr;
    configStr << currentPath.parent_path();
    workPath = configStr.str();
    workPath.erase(0,1);
    int PathLen = workPath.size() - 1;
    cout << "Path Length " << PathLen << endl;
    workPath.erase(PathLen,1);
    configPath = workPath + configFileName; 
	cout << configPath << " :This is the Config Path" << endl;


	configArr = config(configPath);

	string addressS = configArr[1];
	const char *address = addressS.c_str();
	string UserNameS = configArr[2];
	const char * UserName = UserNameS.c_str();
	string passwordS = configArr[3];
	const char *password = passwordS.c_str();
	string databaseNameS = configArr[4];
	const char *databaseName = databaseNameS.c_str();
	string portNameS = configArr[5];

	stringstream portConvert(portNameS);
	portConvert >> portName;

    string query = "";

    fileTODO = fileQuery(query,address,UserName,password,databaseName,port);


    static const int num_threads = 8;

    thread OCR[num_threads];

    for(int q = 0; q < num_threads; q++)
    {
        OCR[q] = std::thread(PlateCheck,);
        q = q + 1;
        threadCounter = threadCounter + 1;
        } 

    }
    
    for(int z = 0; z < num_threads; z++)
    {
        OCR[z].join();
        z = z + 1;
    }
    



}

