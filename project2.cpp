#include <iostream>
#include <set>
#include <vector>
#include <time.h>
#include <sstream>
#include <fstream>
#include <limits>
#include <cmath>
#include <chrono>
#include <ctime>

using namespace std;

typedef vector<vector<double> > Data;
ofstream fout;

void printFeatures(set<int> features, string output_filename){
    // fout.open(output_filename, ofstream::app);
    int count = 0;
    fout << "{";
    cout << "{";

    for(set<int>::iterator it = features.begin(); it != features.end(); it++){
        if(count < features.size()-1){
            fout << *it << ", ";
            cout << *it << ", ";
        }
        else{
            fout << *it;
            cout << *it;
        }
        count++;
    }
    // fout.close();
}

vector<vector<double> > parseData(string fileName, string output_filename)
{
    // fout.open(output_filename, ofstream::app);
    int rows = 0, cols = 0;
    string line;
    double item;
    vector<vector<double> > data;

    ifstream file(fileName);

    while (getline(file, line))
    {
        cols = 0;
        rows++;
        vector<double> row;

        stringstream ss(line); // Set up up a stream from this line
        while (ss >> item)
        {
            cols++; // Each item delineated by spaces adds one to cols
            row.push_back(item);
        }
        data.push_back(row);
    }
    file.close();

    fout << "\nThis dataset has " << cols - 1 << " features (not including the class attribute), with " << rows << " instances.\n";
    cout << "\nThis dataset has " << cols - 1 << " features (not including the class attribute), with " << rows << " instances.\n";

    // fout.close();
    return data;
}

double leaveOneOutCrossValidation(const Data& data, set<int> current_set){
    int num_correctly_classified = 0;

    for(int i = 0; i < data.size(); i++){
        int label_object_to_classify = data[i][0];

        double nearest_neighbor_distance = numeric_limits<double>::max();
        double nearest_neighbor_location = numeric_limits<double>::max();
        double nearest_neighbor_label = 0;

        for(int j = 0; j < data.size(); j++){
            if(j!=i){
                double sum = 0;

                for(set<int>::iterator it = current_set.begin(); it != current_set.end(); it++){
                    sum += pow((data[i][*it] - data[j][*it]),2);
                }

                double distance = sqrt(sum);

                if(distance < nearest_neighbor_distance){
                    nearest_neighbor_distance = distance;
                    nearest_neighbor_location = j;
                    nearest_neighbor_label = data[nearest_neighbor_location][0];
                }
            }
        }

        if(label_object_to_classify == nearest_neighbor_label)
            num_correctly_classified++;
    }
    double accuracy = double(num_correctly_classified)/double(data.size());

    return accuracy*100;
}

void featureSearch(const Data& data, string output_filename){
    // fout.open(output_filename, ofstream::app);

    set<int> current_set_of_features;
    set<int> overall_best_features;
    double overall_best_accuracy = 0;
    bool accuracy_has_decreased = false;

    // Iterate through each level of the graph
    for(int i = 1; i <= data[0].size()-1; i++){
        int feature_to_add_at_this_level = 0;
        double best_so_far_accuracy = 0;

        // Iterate through each feature
        for(int j = 1; j <= data[0].size()-1; j++){
            if(current_set_of_features.find(j) == current_set_of_features.end()){
                set<int> new_set_of_features = current_set_of_features;
                new_set_of_features.insert(j);
                double accuracy = leaveOneOutCrossValidation(data, new_set_of_features);

                fout << "\tUsing feature(s) ";
                cout << "\tUsing feature(s) ";
                if(current_set_of_features.empty()){
                    fout << "{" << j; 
                    cout << "{" << j; 
                }
                else{
                    printFeatures(current_set_of_features, output_filename);
                    fout << ", " << j;
                    cout << ", " << j;
                }
                fout << "} accuracy is " << accuracy << "%\n";
                cout << "} accuracy is " << accuracy << "%\n";

                if(accuracy > best_so_far_accuracy){
                    best_so_far_accuracy = accuracy;
                    feature_to_add_at_this_level = j;
                }
            }
        }

        if(best_so_far_accuracy > overall_best_accuracy){
            accuracy_has_decreased = false;
        }
        else if(best_so_far_accuracy < overall_best_accuracy && !accuracy_has_decreased){
            accuracy_has_decreased = true;
            fout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)\n";
            cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)\n";
        }
        current_set_of_features.insert(feature_to_add_at_this_level);
        fout << "Feature set ";
        cout << "Feature set ";
        printFeatures(current_set_of_features, output_filename);
        fout << "} was best, accuracy is " << best_so_far_accuracy << "%\n";
        cout << "} was best, accuracy is " << best_so_far_accuracy << "%\n";

        if(best_so_far_accuracy > overall_best_accuracy){
            overall_best_accuracy = best_so_far_accuracy;
            overall_best_features = current_set_of_features;
        }
    }
    fout << "Finished search!! The best feature subset is ";
    cout << "Finished search!! The best feature subset is ";
    printFeatures(overall_best_features, output_filename);
    fout << "} which has an accuracy of " << overall_best_accuracy << "%\n\n";
    cout << "} which has an accuracy of " << overall_best_accuracy << "%\n\n";

    // fout.close();
}

void backwardElimination(const Data& data, string output_filename){
    // fout.open(output_filename, ofstream::app);

    set<int> current_set_of_features;
    set<int> overall_best_features;
    double overall_best_accuracy = 0;
    bool accuracy_has_decreased = false;

    for(int i = 1; i < data[0].size(); i++){
        current_set_of_features.insert(i);
    }

    fout << "Beginning search.\n\n";
    cout << "Beginning search.\n\n";
    double best_so_far_accuracy = leaveOneOutCrossValidation(data, current_set_of_features);

    fout << "Using feature(s) ";
    cout << "Using feature(s) ";
    printFeatures(current_set_of_features, output_filename);
    fout << "}, accuracy is " << best_so_far_accuracy << "%\n";
    cout << "}, accuracy is " << best_so_far_accuracy << "%\n";

    for(int i = 1; i < data[0].size(); i++){
        int feature_to_eliminate_at_this_level;

        for(set<int>::iterator it = current_set_of_features.begin(); it != current_set_of_features.end(); it++){

            set<int> new_feature_set = current_set_of_features;
            new_feature_set.erase(*it);


            double accuracy = leaveOneOutCrossValidation(data, new_feature_set);

            fout << "\tUsing feature(s) ";
            cout << "\tUsing feature(s) ";
            printFeatures(new_feature_set, output_filename);
            fout << "}, accuracy is " << accuracy << "%\n";
            cout << "}, accuracy is " << accuracy << "%\n";

            if(accuracy > best_so_far_accuracy){
                best_so_far_accuracy = accuracy;
                feature_to_eliminate_at_this_level = *it;
            }
        }

        if(best_so_far_accuracy > overall_best_accuracy){
            accuracy_has_decreased = false;
        }
        else if(best_so_far_accuracy < overall_best_accuracy && !accuracy_has_decreased){
            accuracy_has_decreased = true;
            fout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)\n";
            cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)\n";
        }

        current_set_of_features.erase(feature_to_eliminate_at_this_level);
        fout << "Feature set ";
        cout << "Feature set ";
        printFeatures(current_set_of_features, output_filename);
        fout << "} was best, accuracy is " << best_so_far_accuracy << "%\n";
        cout << "} was best, accuracy is " << best_so_far_accuracy << "%\n";

        if(best_so_far_accuracy > overall_best_accuracy){
            overall_best_accuracy = best_so_far_accuracy;
            overall_best_features = current_set_of_features;
        }
        best_so_far_accuracy = 0;

    }
    fout << "Finished search!! The best feature subset is ";
    cout << "Finished search!! The best feature subset is ";
    printFeatures(overall_best_features, output_filename);
    fout << "} which has an accuracy of " << overall_best_accuracy << "%\n\n";
    cout << "} which has an accuracy of " << overall_best_accuracy << "%\n\n";

    // fout.close();
}

int main(){
    cout << "Welcome to Celvin Lizama Pena's Feature Selection Algorithm.\nType in the name of the file to test:\n";
    string filename;
    cin >> filename;

    cout << "\nType in the name of the file to write trace to:\n";
    string output_filename;
    cin >> output_filename;

    fout.open(output_filename, ofstream::out);

    cout << "\nType the number of the algorithm you want to run.\n1)\tForward Selection\n2)\tBackward Elimination\n\n";
    int choice;
    cin >> choice;

    set<int> current_set;
    int feature_to_add;
    Data data = parseData(filename, output_filename);
    set<int> all_features;
    int num_features = 0;

    for(int i = 1; i < data[0].size(); i++){
        num_features++;
        all_features.insert(i);
    }

    fout << "\nRunning nearest neighbor with all " << num_features << " features, using \"leave-one-out\" evaluation, I get an accuracy of " << leaveOneOutCrossValidation(data, all_features) << "%\n\n"; 
    cout << "\nRunning nearest neighbor with all " << num_features << " features, using \"leave-one-out\" evaluation, I get an accuracy of " << leaveOneOutCrossValidation(data, all_features) << "%\n\n"; 

    chrono::time_point<chrono::system_clock> start, end;

    start = chrono::system_clock::now();
    switch(choice){
        case 1:
            fout << "Beginning search\n";
            cout << "Beginning search\n";
            featureSearch(data, output_filename);
            break;
        case 2:
            backwardElimination(data, output_filename);
            break;
        default:
            break;

    }
    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    fout << "Runtime duration: " << elapsed_seconds.count() << " seconds\n";
    cout << "Runtime duration: " << elapsed_seconds.count() << " seconds\n";
    fout.close();
}
