#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <vector>
#include <set>
#include <iterator>
#include <string>

#define struc_mapset std::map< set<int>, int >
#define struc_set std::set<int>
#define FOR_MAP(it,m) for(struc_mapset::iterator (it)=(m).begin();(it)!=(m).end();)//no ++it
#define FOR_MAPSET(it2,it) for(struc_set::iterator (it2)=(it)->first.begin();(it2)!=(it)->first.end();(it2)++)
using namespace std;

void printMap( map< set<int>, int > map){
    
    /*std::map< set<int>, int >::iterator it;
     for(it = map.begin(); it != map.end(); it++){
     std::set <int>::iterator it1;
     for(it1 = it->first.begin(); it1 != it->first.end(); it1++){
     cout << *it1 << ',';
     }
     cout << it->second;
     cout << endl;
     }*/
    FOR_MAP(it,map){
        FOR_MAPSET(it2, it){
            cout << *it2 << ',';
        }
        cout << it->second;
        cout << endl;
        ++it;
    }
}

void printVmap( vector< map < set<int>, int > > vmap){
    
    for(int i=0; i<vmap.size(); i++){
        printMap(vmap[i]);
    }
    
}

void printVset( vector< set<int> > vset){
    
    for(int i=0; i<vset.size(); i++){
        std::set<int>::iterator it;
        for(it = vset[i].begin(); it != vset[i].end(); it++){
            cout << *it << ',';
        }
        cout << endl;
    }
}

void printSet( set<int> set){
    
    struc_set::iterator it;
    for(it = set.begin(); it != set.end(); it++){
        cout << *it << ',';
    }
    //cout << endl;
}

int main() {
    
    vector< set<int> > data;//list to store data
    vector< map< set<int>,int > > vmap;//to save itemsets and counts
    map< set<int>,int > single;//to save single items and counts
    vector< set<int> > prunedData;
    //min_sup and conf
    double min_sup = 0;
    double min_conf = 0;
    //TODO:prompt for input
    
    string filename;
    ifstream read;
    //read file
    do{
        cout << "Please enter filename:";
        getline(cin,filename);
        read.open(filename.c_str(), ios::in);
    }while(!read.is_open());
    //prompt for min_sup,conf input
    do{
        cout << "Please input min support:";
        cin >> min_sup;
    } while (min_sup < 0 || min_sup > 1);
    do{
        cout << "Please input min confidence:";
        cin >> min_conf;
    } while (min_conf < 0 || min_conf > 1);
    
    //put data into vector
    string line;//temporarily save line for each data
    
    while(getline(read,line)){
        //tools to divide items by ','
        istringstream is(line);
        int inter;
        char ch;
        
        set<int> dataset;//saves characters in each line
        while( is >> inter){//read integers
            
            dataset.insert(inter);//put item into dataset
            //cnt item
            set<int> item;
            item.insert(inter);//put item into set(for single)
            single[item]++;//count of item in map +1
            
            is >> ch;//read ','
        }
        data.push_back(dataset);//put the set into data
        line.clear();//clear line for next data
    }
    prunedData = data;
    //cout << "DATA\n";
    //printVset(data);
    
    //cout << "Single\n";
    //printMap(single);
    
    double standard = min_sup*data.size();
    //cout << "Minimum Support:" << standard << endl;
    
    //prune single items and data
    FOR_MAP(ii, single){
        if( (ii->second) < standard){
            
            for(int i=0; i<prunedData.size();i++){//prune data
                
                int x = *(ii->first.begin());
                std::set<int>::iterator it;
                it = prunedData[i].find(x);
                if(it != prunedData[i].end()){
                    prunedData[i].erase(it);
                }
                if(prunedData[i].size()==0){//no more items in data
                    prunedData.erase(prunedData.begin()+i);
                }
            }
            ii = single.erase(ii);
        }
        else{
            ++ii;
        }
    }
    vmap.push_back(single);//put single itemsets and counts into vmap
    
    //cout << "PrunedSingle\n";
    //printMap(single);
    //cout << "Pruned DATA\n";
    //printVset(prunedData);
    
    //printVmap(vmap);
    int sourceVmap = 0;
    
    while(1){
        size_t mergedSize = vmap.size()+1;//merged size for next round
        //merge itemset of 2 items
        struc_mapset::iterator imap;
        struc_mapset tempMap;
        struc_set::iterator iset;
        for(imap = vmap[sourceVmap].begin(); imap != vmap[sourceVmap].end(); imap++){//get the first set to merge
            //cout << "First Set\n";
            //printSet(imap->first);
            struc_mapset::iterator nextimap;
            for(nextimap = imap; nextimap != vmap[sourceVmap].end(); nextimap++){//get the second set to merge
                struc_set unionSet;//the set to save union
                
                //cout << "Second Set\n";
                //printSet(nextimap->first);
                
                unionSet.insert(imap->first.begin(),imap->first.end());//merge first set
                unionSet.insert(nextimap->first.begin(),nextimap->first.end());//merge second set
                
                if(unionSet.size()==mergedSize){//want itemset of 2 items,vmap.size()+1 = 2 in this case
                    tempMap[unionSet]++;//save into tempMap and count times the set is merged
                }
            }
        }
        //cout << "TempMap\n";
        //printMap(tempMap);
        
        //count itemset of 2 items
        FOR_MAP(it, tempMap){//run every itemset in tempMap
            long int mergedTimes = mergedSize*(mergedSize-1)/2;
            if((it->second) != mergedTimes){//not valid merged set
                it = tempMap.erase(it);
            }
            else{
                it->second = 0;
                //cout << "search for subset:";
                //printSet(it->first);
                //cout << endl;
                for(size_t i=0; i<prunedData.size(); i++){//search in every data
                    //cout << "search in prunedData"<<i<<":";
                    //printSet(prunedData[i]);
                    //cout << endl;
                    if(prunedData[i].size()<mergedSize){//not enough items to compare
                        prunedData.erase(prunedData.begin()+i);
                    }
                    else if(std::includes( prunedData[i].begin(), prunedData[i].end(), it->first.begin(), it->first.end() ) ){//itemset is a subset of data
                        //cout << "found\n";
                        it->second++;//count of itemset ++
                    }
                }
                ++it;
            }
        }
        //cout << "ITEMSET and COUNTS\n";
        //printMap(tempMap);
        
        //prune itemset of 2 items
        FOR_MAP(it, tempMap){
            if( (it->second) < standard ){
                it = tempMap.erase(it);
            }
            else{
                ++it;
            }
        }
        
        //cout << "Pruned TEMPMAP\n";
        //cout << "SIZE:" << tempMap.size() << endl;
        //printMap(tempMap);
        //check if end of merging process
        if(tempMap.size()==1){//one itemset left
            vmap.push_back(tempMap);//put the itemset into vmap
            break;//end merging
        }
        else if(tempMap.size()==0){//no itemsets merged
            break;//end merging
        }
        else{
            vmap.push_back(tempMap);//put all itemsets into vmap and continue
        }
        //cout << "vmap\n";
        //printVmap(vmap);
        sourceVmap++;//the merging source for next round
    }
    //cout << "Frequent Itemsets\n";
    //printVmap(vmap);
    
    //find strong association rules
    for(int i=0; i<vmap.size(); i++){//every map for different itemsets(2,3...)
        //cout << "First map:" << i+1 << endl;
        for(int j=i; j<vmap.size(); j++){//another map
            //cout << "Second map:" << j+1 << endl;
            if(j != i){//skip same maps
                FOR_MAP(it1, vmap[i]){//every set in first map
                    
                    FOR_MAP(it2, vmap[j]){//every set in second map
                        
                        if(std::includes(it2->first.begin(),it2->first.end(),it1->first.begin(), it1->first.end())){//set in first map is a subset of set in second map
                            
                            /*cout << "set";
                             printSet(it1->first);
                             cout << "屬於 set";
                             printSet(it2->first);
                             cout << endl;*/
                            
                            double a = it2->second;
                            double b = it1->second;
                            //cout << a << '/' << b << endl;
                            double prob = a / b;
                            //cout << "prob=" << prob <<endl;
                            if(prob >= min_conf){//is strong association rule
                                //vector<int> dif(it2->first.size());
                                //std::vector<int>::iterator itv;
                                set<int> dif;
                                set_difference(it2->first.begin(), it2->first.end(), it1->first.begin(),it1->first.end(), inserter(dif, dif.begin()));
                                
                                //dif.resize(itv-dif.begin());
                                cout << '(';
                                printSet(it1->first);
                                cout << ')';
                                cout << " ==> ";
                                cout << '(';
                                /*for(itv=dif.begin(); itv!=dif.end(); itv++){
                                 cout << *itv << ',';
                                 }*/
                                printSet(dif);
                                cout << ')';
                                cout << endl;
                            }
                        }
                        ++it2;
                    }
                    ++it1;
                }
            }
        }
    }
    
    return 0;
}

