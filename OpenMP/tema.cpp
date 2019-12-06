#include <bits/stdc++.h>
#include <omp.h>
#include <sys/time.h>
using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

vector< pair<double, double> > clusters[100];

vector< pair<double, double> > centroids;

vector< pair<double, double> > points;

omp_lock_t Mutex[100];

vector<int> final_result;

int iters = 100;

double dist(pair<double, double> pa1, pair<double, double> pa2) {
    return (pa1.first - pa2.first) * (pa1.first - pa2.first) +
        (pa1.second - pa2.second) * (pa1.second - pa2.second);
}

int main() {
    int N, K;
    srand(time(NULL));

    cout << "Introduce nr of iterations: ";
    cin >> iters;

    fin >> K;
    fin >> N;
    int i, j, cl, k, l;
    double meanx, meany;
    double Min;
    for (i = 0; i < N; i++) {
        double a, b;
        fin >> a >> b;
        points.push_back(make_pair(a, b));
    }

    for (i = 0; i < K; i++)
        centroids.push_back(points[rand() % N]);


    for (int i = 0; i < K; i++)
        omp_init_lock(&Mutex[i]);
    final_result.resize(N);
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    for (i = 0; i < iters; i++) {
        #pragma omp parallel shared(points, centroids, clusters, i, Mutex, N, K) private(j, Min, cl, k, l, meanx, meany)
        {
            #pragma omp for
            for (j = 0; j < N; j++) {
                cl = -1;
                Min = 100000000;

                for (k = 0; k < K; k++) {
                    if (Min > dist(points[j], centroids[k])) {
                        Min = dist(points[j], centroids[k]);
                        cl = k;
                    }
                }

                if (cl != -1) {
                    omp_set_lock(&Mutex[cl]);
                    clusters[cl].push_back( points[j] );
                    omp_unset_lock(&Mutex[cl]);
                } else {
                    //cout << "Alert\n";
                }
            }

            #pragma omp barrier

            #pragma omp for
            for (j = 0; j < K; j++) {
               meanx = 0;
               meany = 0;
                
                for (l = 0; l < clusters[j].size(); l++) {
                    meanx += clusters[j][l].first;
                    meany += clusters[j][l].second;
                }
                
                if (clusters[j].size() != 0) {
                    centroids[j].first = meanx / clusters[j].size();
                    centroids[j].second = meany / clusters[j].size();
                }
                
                clusters[j].clear();
            }

            #pragma omp barrier

        }

    }


    #pragma omp parallel shared(points, centroids, final_result, N, K) private(i, j, Min, cl)
    {
        #pragma omp for
        for (i = 0; i < N; i++) {
            cl = -1;
            Min = 100000000;
            
            for (j = 0; j < K; j++)
                if (Min > dist(points[i], centroids[j])) {
                    Min = dist(points[i], centroids[j]);
                    cl = j;
                }
            
            final_result[i] = cl;
        }
    }
    gettimeofday(&stop, NULL);

    printf("Parallel KMeans took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    for (int i = 0; i < K; i++)
        omp_destroy_lock(&Mutex[i]);

    for (int i = 0; i < N; i++)
        fout << final_result[i] << '\n';

    fout.close();

    return 0;
}