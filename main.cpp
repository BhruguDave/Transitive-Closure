/* AUTHORS:
 * Mihir Desai      - 201801033@daiict.ac.in
 * Bhrugu Dave      - 201801401@daiict.ac.in
 * Smit Kumbhani    - 201801404@daiict.ac.in
 * Sarthak Patel    - 201801435@daiict.ac.in
*/

#include<iostream>
#include<vector>
#include<omp.h>
#include<fstream>
#include<cstring>
#include<stdlib.h>
#include<unistd.h>
#include<algorithm>
#include<sstream>
#include<queue>

using namespace std;

void runV1(bool runSerial, vector<int> processes, vector<string> datasets, vector<int> sizeOfDataset);
void runV2(bool runSerial, vector<int> processes, vector<string> datasets, vector<int> sizeOfDataset);
void runV3(bool runSerial, vector<int> processes, vector<string> datasets, vector<int> sizeOfDataset);

ofstream output;

int main(int argc, char** argv){

    vector<string> algorithms;
    algorithms.push_back("V1");     // Adjacency List Based Approach
    algorithms.push_back("V2");     // Adjacency Matrix Based Approach
    algorithms.push_back("V3");     // Cache Optimized Adjacency Matrix Based Approach

    bool runSerial = true;          // Set to true for evaluating sequential version of selected algorithm(s)

    vector<int> numberOfProcesses = {2, 4, 8, 16, 32, 64, 128, 176};  // Append number of threads that you want to run the parallelized version for

    vector<string> datasets;        // path to the datasets to be used for evaluation
    vector<int> sizeOfDataset;      // Number of Vertices in Coresponding Dataset

    datasets.push_back("../datasets/Kohonen.csv");            sizeOfDataset.push_back(4470);
    datasets.push_back("../datasets/wing_nodal.csv");         sizeOfDataset.push_back(10937);
    datasets.push_back("../datasets/delaunay_n16.csv");       sizeOfDataset.push_back(65536);
    datasets.push_back("../datasets/usroads.csv");            sizeOfDataset.push_back(129164);
    datasets.push_back("../datasets/md2010.csv");             sizeOfDataset.push_back(145247);
    datasets.push_back("../datasets/caidaRouterLevel.csv");   sizeOfDataset.push_back(192244); 
    datasets.push_back("../datasets/coAuthorsCiteseer.csv");  sizeOfDataset.push_back(227320);
    datasets.push_back("../datasets/citationCiteseer.csv");   sizeOfDataset.push_back(268495);
    datasets.push_back("../datasets/mc2depi.csv");            sizeOfDataset.push_back(525825);
    datasets.push_back("../datasets/roadNet-PA.csv");         sizeOfDataset.push_back(1090920);
    datasets.push_back("../datasets/roadNet-TX.csv");         sizeOfDataset.push_back(1393383);
    datasets.push_back("../datasets/roadNet-CA.csv");         sizeOfDataset.push_back(1971281);

    string resultsFileName = "results.txt";
    output.open(resultsFileName);

    if(datasets.size() == 0){
        cout << "No datasets have been selected! Please select atleast one dataset to continue!" << endl;
        output.close();
        return 0;
    }

    for (string algo : algorithms){
        if (algo == "V1"){
            output << "----------------------------------------- Running V1 (Adjacency List Based Approach) -----------------------------------------\n";
            cout << "----------------------------------------- Running V1 (Adjacency List Based Approach) -----------------------------------------\n";
            runV1(runSerial, numberOfProcesses, datasets, sizeOfDataset);
        }
        if (algo == "V2"){
            output << "----------------------------------------- Running V2 (Adjacency Matrix Based Approach) -----------------------------------------\n";
            cout << "----------------------------------------- Running V2 (Adjacency Matrix Based Approach) -----------------------------------------\n";
            runV2(runSerial, numberOfProcesses, datasets, sizeOfDataset);
        }
        if (algo == "V3"){
            output << "----------------------------------------- Running V3 (Cache Optimized Adjacency Matrix Based Approach) -----------------------------------------\n";
            cout << "----------------------------------------- Running V3 (Cache Optimized Adjacency Matrix Based Approach) -----------------------------------------\n";
            runV3(runSerial, numberOfProcesses, datasets, sizeOfDataset);
        }

        cout << "\n\n\n";
        output << "\n\n";
    }

    return 0;
}

void runV1(bool runSerial, vector<int> processes, vector<string> datasets, vector<int> sizeOfDataset){

    for(int dataset_id = 0; dataset_id < datasets.size(); dataset_id++){
        
        vector<int> x, y;
        int tempx, tempy;
        double startTime, endTime, runtime;

        fstream fin;
        fin.open(datasets[dataset_id], ios::in);
        
        vector<string> row;
        string line, word;
        
        while(fin >> line)
        {
            row.clear();
            stringstream s(line);

            while(getline(s, word, ','))
            {
                row.push_back(word);
            }

            tempx = stoi(row[0]);
            tempy = stoi(row[1]);

            x.push_back(tempx - 1);
            y.push_back(tempy - 1);
        }

        long long int n = sizeOfDataset[dataset_id];

        vector<vector<int> > input_adj_list(n, vector<int>());
        vector<vector<int> > TC_adj_list(n, vector<int>());

        for(unsigned int i = 0; i < x.size(); i++)
            input_adj_list[x[i]].push_back(y[i]);

        long long int number_of_edges_in_input=0, number_of_edges_in_tc=0;
        for(int i = 0; i < n; i++)
            number_of_edges_in_input += input_adj_list[i].size();
        
        output << "\n\n--------------------------------------------------\n";
        output << "Dataset : " << datasets[dataset_id] << endl;
        output << "Number of edges in input : " << number_of_edges_in_input << endl;
        output << "--------------------------------------------------\n";

        cout << "\n\n--------------------------------------------------\n";
        cout << "Dataset : " << datasets[dataset_id] << endl;
        cout << "Number of edges in input : " << number_of_edges_in_input << endl;
        cout << "--------------------------------------------------\n";

        // Serial Implementation of V3
        if(runSerial){

            startTime = omp_get_wtime();

            for(int Vs = 0; Vs < n; Vs++)
            {
                queue<int> Q;
                Q.push(Vs);

                while(!Q.empty())
                {
                    auto u = Q.front();
                    Q.pop();

                    for (auto Vi : input_adj_list[u])
                    {
                        if(std::find(TC_adj_list[Vs].begin(), TC_adj_list[Vs].end(), Vi) 
                                == TC_adj_list[Vs].end())
                        {
                            TC_adj_list[Vs].push_back(Vi);
                            Q.push(Vi);
                        }
                    }
                }
            }

            endTime = omp_get_wtime();
            runtime = endTime - startTime;

            number_of_edges_in_tc = 0;
            for(int i = 0; i < n; i++)
                number_of_edges_in_tc += TC_adj_list[i].size();
            
            output << "For 1 core : " << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
            cout << "For 1 core :\t" << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
        }

        // Parallel Implementation of V1
        for(unsigned int procIndex = 0; procIndex < processes.size(); procIndex++){
            
            TC_adj_list = vector<vector<int> > (n, vector<int>());

            omp_set_num_threads(processes[procIndex]);

            startTime = omp_get_wtime();

            #pragma omp parallel
            {
                int id = omp_get_thread_num();
                int total_threads_spawned = omp_get_num_threads();

                #pragma omp for schedule(dynamic, 4)
                for(int Vs = 0; Vs < n; Vs++){
                    queue<int> Q;
                    Q.push(Vs);

                    while(!Q.empty())
                    {
                        auto u = Q.front();
                        Q.pop();

                        for (auto Vi : input_adj_list[u])
                        {
                            if(std::find(TC_adj_list[Vs].begin(), TC_adj_list[Vs].end(), Vi) 
                                == TC_adj_list[Vs].end())
                            {
                                TC_adj_list[Vs].push_back(Vi);
                                Q.push(Vi);
                            }
                        }
                    }
                }
            }

            endTime = omp_get_wtime();
            runtime = endTime - startTime;

            number_of_edges_in_tc = 0;
            for(int i = 0; i < n; i++)
                number_of_edges_in_tc += TC_adj_list[i].size();

            output << "For " << processes[procIndex] << " core : " << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
            cout << "For " << processes[procIndex] << " core :\t" << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
        }
    }
}

void runV2(bool runSerial, vector<int> processes, vector<string> datasets, vector<int> sizeOfDataset){

    for(int dataset_id = 0; dataset_id < datasets.size(); dataset_id++){
        
        vector<int> x, y;
        int tempx, tempy, maxVertexLabel;
        double startTime, endTime, runtime;

        fstream fin;
        fin.open(datasets[dataset_id], ios::in);
        
        vector<string> row;
        string line, word;
        
        while(fin >> line)
        {
            row.clear();
            stringstream s(line);

            while(getline(s, word, ','))
            {
                row.push_back(word);
            }

            tempx = stoi(row[0]);
            tempy = stoi(row[1]);

            x.push_back(tempx - 1);
            y.push_back(tempy - 1);
        }

        long long int n = sizeOfDataset[dataset_id];

        vector<vector<int> > input_adj_list(n, vector<int>());
        vector<vector<bool> > TC(n, vector<bool>(n, false));

        for(unsigned int i = 0; i < x.size(); i++)
            input_adj_list[x[i]].push_back(y[i]);

        long long int number_of_edges_in_input=0, number_of_edges_in_tc=0;
        for(int i = 0; i < n; i++)
            number_of_edges_in_input += input_adj_list[i].size();
        
        output << "\n\n--------------------------------------------------\n";
        output << "Dataset : " << datasets[dataset_id] << endl;
        output << "Number of edges in input : " << number_of_edges_in_input << endl;
        output << "--------------------------------------------------\n";

        cout << "\n\n--------------------------------------------------\n";
        cout << "Dataset : " << datasets[dataset_id] << endl;
        cout << "Number of edges in input : " << number_of_edges_in_input << endl;
        cout << "--------------------------------------------------\n";

        // Serial Implementation of V3
        if(runSerial){

            startTime = omp_get_wtime();

            for(int Vs = 0; Vs < n; Vs++)
            {
                queue<int> Q;
                Q.push(Vs);

                while(!Q.empty())
                {
                    auto u = Q.front();
                    Q.pop();

                    for (auto Vi : input_adj_list[u])
                    {
                        if(TC[Vs][Vi] == false)
                        {
                            TC[Vs][Vi] = true;
                            Q.push(Vi);
                        }
                    }
                }
            }

            endTime = omp_get_wtime();
            runtime = endTime - startTime;

            number_of_edges_in_tc = 0;
            for(int i = 0; i < n; i++)
                for(int j = 0; j < n; j++)
                    if(TC[i][j] == true)
                        number_of_edges_in_tc++;
            
            output << "For 1 core : " << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
            cout << "For 1 core :\t" << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
        }

        // Parallel Implementation of V2
        for(unsigned int procIndex = 0; procIndex < processes.size(); procIndex++){
            
            vector<vector<bool> >().swap(TC);
            TC = vector<vector<bool> >(n, vector<bool>(n, false));

            omp_set_num_threads(processes[procIndex]);

            startTime = omp_get_wtime();

            #pragma omp parallel
            {
                int id = omp_get_thread_num();
                int total_threads_spawned = omp_get_num_threads();

                #pragma omp for schedule(dynamic, 4)
                for(int Vs = 0; Vs < n; Vs++){
                    queue<int> Q;
                    Q.push(Vs);

                    while(!Q.empty())
                    {
                        auto u = Q.front();
                        Q.pop();

                        for (auto Vi : input_adj_list[u])
                        {
                            if(TC[Vs][Vi] == false)
                            {
                                TC[Vs][Vi] = true;
                                Q.push(Vi);
                            }
                        }
                    }
                }
            }

            endTime = omp_get_wtime();
            runtime = endTime - startTime;

            number_of_edges_in_tc = 0;
            for(int i = 0; i < n; i++)
                for(int j = 0; j < n; j++)
                    if(TC[i][j] == true)
                        number_of_edges_in_tc++;

            output << "For " << processes[procIndex] << " core : " << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
            cout << "For " << processes[procIndex] << " core :\t" << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
        }
    }
}

void runV3(bool runSerial, vector<int> processes, vector<string> datasets, vector<int> sizeOfDataset){

    for(int dataset_id = 0; dataset_id < datasets.size(); dataset_id++){
        
        vector<int> x, y;
        int tempx, tempy, maxVertexLabel;
        double startTime, endTime, runtime;

        fstream fin;
        fin.open(datasets[dataset_id], ios::in);
        
        vector<string> row;
        string line, word;
        
        while(fin >> line)
        {
            row.clear();
            stringstream s(line);

            while(getline(s, word, ','))
            {
                row.push_back(word);
            }

            tempx = stoi(row[0]);
            tempy = stoi(row[1]);

            x.push_back(tempx - 1);
            y.push_back(tempy - 1);
        }

        long long int n = sizeOfDataset[dataset_id];

        vector<vector<int> > input_adj_list(n, vector<int>());
        vector<vector<bool> > TC(n, vector<bool>(n, false));

        for(unsigned int i = 0; i < x.size(); i++)
            input_adj_list[x[i]].push_back(y[i]);

        long long int number_of_edges_in_input=0, number_of_edges_in_tc=0;
        for(int i = 0; i < n; i++)
            number_of_edges_in_input += input_adj_list[i].size();
        
        output << "\n\n--------------------------------------------------\n";
        output << "Dataset : " << datasets[dataset_id] << endl;
        output << "Number of edges in input : " << number_of_edges_in_input << endl;
        output << "--------------------------------------------------\n";

        cout << "\n\n--------------------------------------------------\n";
        cout << "Dataset : " << datasets[dataset_id] << endl;
        cout << "Number of edges in input : " << number_of_edges_in_input << endl;
        cout << "--------------------------------------------------\n";

        // Serial Implementation of V3
        if(runSerial){

            startTime = omp_get_wtime();

            for(int Vs = 0; Vs < n; Vs++)
            {
                queue<int> Q;
                Q.push(Vs);
                vector<bool> auxiliary(n, false);

                while(!Q.empty())
                {
                    auto u = Q.front();
                    Q.pop();

                    for (auto Vi : input_adj_list[u])
                    {
                        if(auxiliary[Vi] == false)
                        {
                            auxiliary[Vi] = true;
                            Q.push(Vi);
                        }
                    }
                }
                TC[Vs] = auxiliary;
            }

            endTime = omp_get_wtime();
            runtime = endTime - startTime;

            number_of_edges_in_tc = 0;
            for(int i = 0; i < n; i++)
                for(int j = 0; j < n; j++)
                    if(TC[i][j] == true)
                        number_of_edges_in_tc++;
            
            output << "For 1 core : " << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
            cout << "For 1 core :\t" << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
        }

        // Parallel Implementation of V3
        for(unsigned int procIndex = 0; procIndex < processes.size(); procIndex++){
            
            vector<vector<bool> >().swap(TC);
            TC = vector<vector<bool> >(n, vector<bool>(n, false));

            omp_set_num_threads(processes[procIndex]);

            startTime = omp_get_wtime();

            #pragma omp parallel
            {
                int id = omp_get_thread_num();
                int total_threads_spawned = omp_get_num_threads();

                #pragma omp for schedule(dynamic, 4)
                for(int Vs = 0; Vs < n; Vs++){
                    queue<int> Q;
                    Q.push(Vs);
                    vector<bool> auxiliary(n, false);

                    while(!Q.empty())
                    {
                        auto u = Q.front();
                        Q.pop();

                        for (auto Vi : input_adj_list[u])
                        {
                            if(auxiliary[Vi] == false)
                            {
                                auxiliary[Vi] = true;
                                Q.push(Vi);
                            }
                        }
                    }
                    TC[Vs] = auxiliary;
                }
            }

            endTime = omp_get_wtime();
            runtime = endTime - startTime;

            number_of_edges_in_tc = 0;
            for(int i = 0; i < n; i++)
                for(int j = 0; j < n; j++)
                    if(TC[i][j] == true)
                        number_of_edges_in_tc++;

            output << "For " << processes[procIndex] << " core : " << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
            cout << "For " << processes[procIndex] << " core :\t" << runtime << " seconds \t\t" << "Number of edges in TC : " << number_of_edges_in_tc << endl;
        }
    }
}