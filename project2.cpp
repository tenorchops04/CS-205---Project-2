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

    cout << "This dataset has " << cols - 1 << " features (not including the class attribute), with " << rows << " instances.\n";
    return data;
}

double leaveOneOutCrossValidation(Data data, set<int> current_set, int feature_to_add){
    int num_correctly_classified = 0;

    for(int i = 0; i < data.size(); i++){
        vector<double> object_to_classify;
        int label_object_to_classify = data[i][0];

        for(int j = 1; j < data[i].size(); j++){
            object_to_classify.push_back(data[i][j]);
        }

        double nearest_neighbor_distance = numeric_limits<double>::max();
        double nearest_neighbor_location = numeric_limits<double>::max();
        int nearest_neighbor_label = 0;

        for(int j = 0; j < data.size(); j++){
            if(j!=i){
                double sum = 0;
                for(int k = 1; k < data[j].size(); k++){
                    sum+=(object_to_classify[k] - data[j][k]);
                }

                double distance = sqrt(pow(sum, 2));

                if(distance < nearest_neighbor_distance){
                    nearest_neighbor_distance = distance;
                    nearest_neighbor_location = j;
                    nearest_neighbor_label = data[nearest_neighbor_location][0];
                }
            }
        }

        if(label_object_to_classify == nearest_neighbor_label)
            num_correctly_classified++;
        cout << "Object " << i << " is class " << label_object_to_classify << endl;
        cout << "--Its nearest neighbor is " << nearest_neighbor_location << " which is in class " << nearest_neighbor_label << endl;
    }
    cout << data.size() << endl;
    double accuracy = double(num_correctly_classified)/double(data.size());
    cout << "The accuracy is " << accuracy*100 << "%\n";

    return accuracy;
}

void featureSearch(const Data& data){
    set<int> current_set_of_features;

    for(int i = 1; i <= data[0].size()-1; i++){
        cout << "On the " << i << "th level of the search tree\n";

        int feature_to_add_at_this_level = 0;
        double best_so_far_accuracy = 0;

        for(int j = 1; j <= data[0].size()-1; j++){
            if(current_set_of_features.find(j) == current_set_of_features.end()){
                cout << "--Consider adding the " << j << " feature\n";
                double accuracy = leaveOneOutCrossValidation(data, current_set_of_features, j+1);

                if(accuracy > best_so_far_accuracy){
                    best_so_far_accuracy = accuracy;
                    feature_to_add_at_this_level = j;
                }
            }
        }
        current_set_of_features.insert(feature_to_add_at_this_level);
        cout << "On level " << i << ", I added feature " << feature_to_add_at_this_level << " to current set\n";
    }
}

int main(){
    srand(time(NULL));
    cout.precision(4);

    //cout << leaveOneOutCrossValidation(data, current_set, feature_to_add) << endl;

    cout << "Welcome to Celvin Lizama Pena's Feature Selection Algorithm.\nType in the name of the file to test:\n";
    string filename;
    cin >> filename;

    cout << "Type the number of the algorithm you want to run.\n1)\tForward Selection\n2)\tBackward Elimination\n";
    int choice;
    cin >> choice;

    set<int> current_set;
    int feature_to_add;
    Data data = parseData(filename);

    //featureSearch(data);
    leaveOneOutCrossValidation(data, current_set, feature_to_add);
}
