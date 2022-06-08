#include <iostream>
#include <set>
#include <vector>
#include <time.h>

using namespace std;

typedef vector<vector<double> > Data;

double leaveOneOutCrossValidation(Data data, set<int> current_set, int feature_to_add){
    double accuracy = rand()% 100;

    return accuracy;
}

int main(){
    srand(time(NULL));
    Data data;
    set<int> current_set;
    int feature_to_add;

    cout << leaveOneOutCrossValidation(data, current_set, feature_to_add) << endl;
}
