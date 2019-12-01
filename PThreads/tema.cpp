#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

pthread_barrier_t barrier1, barrier2;

pthread_mutex_t Mutex[100];

vector< pair<double, double> > clusters[100];

vector< pair<double, double> > centroids;

vector< pair<double, double> > points;

vector<int> final_result;

int iters = 100;

int nthreads = 4;

struct my_thread {
    int tid, K;
    int left, right;
    int cluster_left, cluster_right;
};

double dist(pair<double, double> pa1, pair<double, double> pa2) {
    return (pa1.first - pa2.first) * (pa1.first - pa2.first) +
        (pa1.second - pa2.second) * (pa1.second - pa2.second);
}

void *kFunc(void *data) {
    my_thread t = *((my_thread*)data);
    
    for (int st = 0; st < iters; st++) {
        for (int i = t.left; i < t.right; i++) {
            int cl = -1;
            double Min = 100000000;
            
            for (int j = 0; j < t.K; j++)
                if (Min > dist(points[i], centroids[j])) {
                    Min = dist(points[i], centroids[j]);
                    cl = j;
                }
            
            if (cl != -1) {
                pthread_mutex_lock(&Mutex[cl]);
                clusters[cl].push_back( points[i] );
                pthread_mutex_unlock(&Mutex[cl]);
            }
        }
        
        pthread_barrier_wait(&barrier1);
        
        if (t.cluster_left != -1) {

            for (int j = t.cluster_left; j < t.cluster_right; j++) {
                double meanx = 0, meany = 0;
                
                for (int l = 0; l < clusters[j].size(); l++) {
                    meanx += clusters[j][l].first;
                    meany += clusters[j][l].second;
                }
                
                if (clusters[j].size() != 0) {
                    centroids[j].first = meanx / clusters[j].size();
                    centroids[j].second = meany / clusters[j].size();
                }
                
                clusters[j].clear();
            }
        }
        
        pthread_barrier_wait(&barrier2);
    }
    
    
}

void *fFunc(void *data) {
    my_thread t = *((my_thread*)data);
    
    for (int i = t.left; i < t.right; i++) {
        int cl = -1;
        double Min = 100000000;
            
        for (int j = 0; j < t.K; j++)
            if (Min > dist(points[i], centroids[j])) {
                Min = dist(points[i], centroids[j]);
                cl = j;
            }
            
        final_result[i] = cl;
    }
}

int main() {
    
    int N, K;
    srand(time(NULL));
    cout << "Introduce nr of threads: ";
    cin >> nthreads;

    cout << "Introduce nr of iterations: ";
    cin >> iters;


    pthread_barrier_init(&barrier1, NULL, nthreads);
    pthread_barrier_init(&barrier2, NULL, nthreads);
    
    fin >> K;
    fin >> N;
    
    for (int i = 0; i < N; i++) {
        double a, b;
        fin >> a >> b;
        points.push_back(make_pair(a, b));
    }
    srand(time(NULL));
    for (int i = 0; i < K; i++)
        centroids.push_back(points[rand() % N]);
    for (int i = 0; i < K; i++)
        pthread_mutex_init(&Mutex[i], NULL);
    
    my_thread thr_str[nthreads];
    int C = N / nthreads;
    int C_cluster = K / nthreads;
    for (int i = 0; i < nthreads; i++) {
        thr_str[i].tid = i;
        thr_str[i].K = K;
        thr_str[i].left = i * C;
        thr_str[i].right = (i+1) * C;

        thr_str[i].cluster_left = thr_str[i].cluster_right = -1;

        if (C_cluster == 0 && i < K) {
            thr_str[i].cluster_left = i;
            thr_str[i].cluster_right = i+1;
        } else if (C_cluster > 0) {
            thr_str[i].cluster_left = i * C_cluster;
            thr_str[i].cluster_right = (i+1) * C_cluster;
        }
    }
    thr_str[nthreads-1].right = N;

    if (C_cluster > 0)
        thr_str[nthreads-1].cluster_right = K;
    pthread_t t[nthreads];
    
    final_result.resize(N);

    struct timeval stop, start;
    gettimeofday(&start, NULL);
    for (int i = 0; i < nthreads; i++)
        pthread_create(&(t[i]), NULL, kFunc, &(thr_str[i]));
    
    for (int i = 0; i < nthreads; i++)
        pthread_join(t[i], NULL);


    for (int i = 0; i < nthreads; i++)
        pthread_create(&(t[i]), NULL, fFunc, &(thr_str[i]));
    
    for (int i = 0; i < nthreads; i++)
        pthread_join(t[i], NULL);

    gettimeofday(&stop, NULL);

    printf("Parallel KMeans took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

    for (int i = 0; i < N; i++)
        fout << final_result[i] << '\n';

    fout.close();

    for (int i = 0; i < K; i++)
        pthread_mutex_destroy(&Mutex[i]);
    
    pthread_barrier_destroy(&barrier1);
    pthread_barrier_destroy(&barrier2);

    return 0;
}

