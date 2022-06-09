#include <iostream>
#include <set>
#include <vector>
#include <time.h>
#include <sstream>
#include <fstream>
#include <limits>
#include <cmath>

using namespace std;

typedef vector<vector<double> > Data;

void printFeatures(set<int> features){
    int count = 0;
    cout << "{";

    for(set<int>::iterator it = features.begin(); it != features.end(); it++){
        if(count < features.size()-1)
            cout << *it << ", ";
        else
            cout << *it;
        count++;
    }
}

vector<vector<double> > parseData(const string &fileName)
{
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

    cout << "\nThis dataset has " << cols - 1 << " features (not including the class attribute), with " << rows << " instances.\n";
    return data;
}

double leaveOneOutCrossValidation(const Data& data, set<int> current_set, int feature_to_add){
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
                sum += pow((data[i][feature_to_add] - data[j][feature_to_add]), 2);

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

void featureSearch(const Data& data){
    set<int> current_set_of_features;

    for(int i = 1; i <= data[0].size()-1; i++){
        //cout << "On the " << i << "th level of the search tree\n";

        int feature_to_add_at_this_level = 0;
        double best_so_far_accuracy = 0;

        for(int j = 1; j <= data[0].size()-1; j++){
            if(current_set_of_features.find(j) == current_set_of_features.end()){
                double accuracy = leaveOneOutCrossValidation(data, current_set_of_features, j);

                cout << "\tUsing feature(s) ";
                if(current_set_of_features.empty())
                    cout << "{" << j; 
                else{
                    printFeatures(current_set_of_features);
                    cout << ", " << j;
                }
                cout << "} accuracy is " << accuracy << "%\n";

                if(accuracy > best_so_far_accuracy){
                    best_so_far_accuracy = accuracy;
                    feature_to_add_at_this_level = j;
                }
            }
        }
        current_set_of_features.insert(feature_to_add_at_this_level);
        cout << "Feature set ";
        printFeatures(current_set_of_features);
        cout << "} was best, accuracy is " << best_so_far_accuracy << "%\n";
    }
}

void forwardSelection(){
    cout << "Forward selection\n";
}

void backwardElimination(){
    cout << "Backward Elimination\n";
}

int main(){
    //cout.precision(4);

    cout << "Welcome to Celvin Lizama Pena's Feature Selection Algorithm.\nType in the name of the file to test:\n";
    string filename;
    cin >> filename;

    cout << "\nType the number of the algorithm you want to run.\n1)\tForward Selection\n2)\tBackward Elimination\n\n";
    int choice;
    cin >> choice;

    set<int> current_set;
    int feature_to_add;
    Data data = parseData(filename);
    set<int> all_features;
    int num_features = 0;

    for(int i = 1; i < data[0].size(); i++){
        num_features++;

        if(i != data[0].size()-1)
            all_features.insert(i);
        else
            feature_to_add = i;
    }

    printFeatures(all_features);
    cout << ", " << feature_to_add << "}\n";
    cout << "\nRunning nearest neighbor with all " << num_features << " features, using \"leave-one-out\" evaluation, I get an accuracy of " << leaveOneOutCrossValidation(data, all_features, feature_to_add) << "%\n\n"; 

    switch(choice){
        case 1:
            cout << "Beginning search\n";
            featureSearch(data);
            break;
        case 2:
            backwardElimination();
            break;
        default:
            break;

    }
}
