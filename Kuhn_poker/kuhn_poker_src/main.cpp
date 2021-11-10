#include <iostream>
#include "kuhn_poker_cfr_trainer.h"
#include "kuhn_poker.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <stdio.h>
#include "information_set.h"
int main() {
    kuhn_poker_cfr_trainer trainer = kuhn_poker_cfr_trainer();
    int num_iter = 100000;
    float result = trainer.train(num_iter);
    //for (auto c: trainer.hash_node_infoset)
    //{
        //cout << "history: " << c.first << " ; avg_strategy:" <<   endl;
        //for (auto i : c.second.get_average_strategy())
        //{
        //    cout << i << endl;
        //}
    //}

    cout.precision(3);
    cout << "average util " << result/float(num_iter)<< endl;
    for(map<string, information_set>::iterator i = trainer.hash_node_infoset.begin();i != trainer.hash_node_infoset.end();i++){
        cout << i->first << endl;
        for(int j = 0;j < i->second.get_average_strategy().size();j++) {
            cout << i->second.get_average_strategy()[j] << " ";
        }
        cout << endl;

    }
    return 0;
}
