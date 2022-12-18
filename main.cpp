#include <iostream>
#include <cmath>
#include <fstream>
#include <chrono>
#include "Cluster.cpp"
#include <omp.h>
#include<vector>
#include<iterator>
#include<fstream>

using namespace std;
using namespace std::chrono;





vector<Point> initalize_points(int num_point); 
vector<Cluster> initalize_clusters(int num_cluster); 
double distance(Point pt, Cluster cl);
void assign_centroid(vector<Point> &points, vector<Cluster> &clusters);
void update_centroids(vector<Cluster> &clusters);
void draw_chart_gnu(vector<Point> &points);

int main() {
    auto start = std::chrono::system_clock::now();
    int num_point;
    int num_cluster;
    int num_iterations;
    
    
    printf("Insert Number of points \n");
    cin>>num_point;
    printf("Insert number of clusters\n");
    cin>>num_cluster;
    printf("Insert number of iterations\n");
    cin>>num_iterations;
    
    vector<Point> points = initalize_points(num_point);

    vector<Cluster> clusters = initalize_clusters(num_cluster);
    
   for(int i=0;i<num_iterations;i++){
        assign_centroid(points, clusters);
        update_centroids(clusters);
        
   }

    try{
        printf("Drawing the chart...\n");
        draw_chart_gnu(points);
    }catch(int e){
        printf("Chart not available, gnuplot not found");
    }

    auto end = std::chrono::system_clock::now();
    double duration = chrono::duration_cast<chrono::milliseconds>(end-start).count();
    
    
    ofstream outfile;
    outfile.open("Results.txt", fstream::app);
    outfile<<"Num points: "<<num_point<<endl<<"Num clusters: "<<num_cluster<<endl<<"Num iterations: "<<num_iterations<<endl<<"Total milliseconds: "<<duration<<endl;
    outfile.close();
}

vector <Point> initalize_points(int num_point){
    //*****************************************************************************************************************
    //INITALIZAITONS
    //*****************************************************************************************************************
    vector<Point> points;

    //*****************************************************************************************************************
    //FILLS VECTOR OF POINTS RANDOMLY GENERATED
    //*****************************************************************************************************************
    for(int i=0;i<num_point; i++){
        Point pt (rand(), rand());
        points.push_back(pt);
        
    }
    return points;
}

vector <Cluster> initalize_clusters(int num_cluster){
    //*****************************************************************************************************************
    //INITALIZAITONS
    //*****************************************************************************************************************
    vector<Cluster> clusters;

    //*****************************************************************************************************************
    //FILLS VECTOR OF CLUSTERS RANDOMLY GENERATED
    //*****************************************************************************************************************
    for(int i=0;i<num_cluster; i++){
        Cluster cl (rand(), rand(), i);
        clusters.push_back(cl);
    }
    return clusters;
}

//Calculate the ecludian distance for the given Point pt and Cluster cl
double distance(Point pt, Cluster cl){

    double distance = sqrt(pow(pt.get_coord_x() - cl.get_coord_x(),2) +
                           pow(pt.get_coord_y() - cl.get_coord_y(), 2));

    return distance;
}

//Assign a point to its cluster centroid based on the MIN Euclidian Distance
void assign_centroid(vector<Point> &points, vector<Cluster> &clusters){
    //*****************************************************************************************************************
    //Double loop points and clusters in order to find the MIN distance between those two. It then assign the point to the cluster which is closer.
    //*****************************************************************************************************************
    double min_distance;
    #pragma omp parallel
    #pragma omp for
    for(Point& p: points){
        Cluster temp_Cluster = clusters[0];
        min_distance = distance(p, temp_Cluster);
        int cluster_id = 0; //Necessary in case there is not better distance and cluster_id is not updated
        for(int i = 1;i<clusters.size();i++){
            double now_distance = distance(p, clusters[i]);
            if(now_distance<min_distance){
                min_distance = now_distance;
                cluster_id = clusters[i].get_cluster_Id();
                
            }
        }
        p.set_id_c(cluster_id); //The cluster id identify the position of this cluster in the clusters array
        #pragma omp crtical
        clusters[cluster_id].add_point(p);
    }
   
}

//Update centroids
void update_centroids(vector<Cluster> &clusters){
       
        for(int i=0;i<clusters.size();i++){
            clusters[i].update_coords();
        }
        
}

//Draw point plot with gnuplot
void draw_chart_gnu(vector<Point> &points){

    ofstream outfile("data.txt");

    for(int i = 0; i < points.size(); i++){

        Point point = points[i];
        outfile << point.get_coord_x() << " " << point.get_coord_y() << " " << point.get_id_c() << std::endl;

    }

    outfile.close();
    system("gnuplot -p -e \"plot 'data.txt' using 1:2:3 with points palette notitle\"");
    remove("data.txt");

}