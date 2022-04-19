// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#include <jni.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>
#include <limits.h>
#include <iostream>
using namespace cv;
using namespace std;

Mat img;

struct Pixel {
    Vec3b pix;
    int clusterID;

    Pixel() {
        pix = 0;
        clusterID = 0;
    }

    Pixel(Vec3b p) {
        pix = p;
        clusterID = 0;
    }

    double calDistance(Pixel p) {
        double dist = pow(abs(p.pix[0] - this->pix[0]), 2) + pow(abs(p.pix[1] - this->pix[1]), 2) + pow(abs(p.pix[2] - this->pix[2]), 2);
        return sqrt(dist);
    }

    Pixel& operator= (const Pixel& p) {
        pix = p.pix;
        clusterID = p.clusterID;
        return *this;
    }

    bool operator== (const Pixel& p)
    {
        return (this->pix[0] == p.pix[0] && this->pix[1] == p.pix[1] && this->pix[2] == p.pix[2] && this->clusterID == p.clusterID);
    }

    bool operator!= (const Pixel& p)
    {
        return !(*this == p);
    }

    friend ostream& operator<<(ostream& os, const Pixel& pt) {
        os << "(" << pt.pix[0] << "," << pt.pix[1] << "," << pt.pix[2] << ")";
        return os;
    }
};



class Image {
    int N, K;
    vector <Pixel> pixels;
    struct Cluster {
        Pixel centeriod;
        vector <Pixel> clusterpixels;
    };
    vector <Cluster> clusters;

public:
    Image(int n) {
        N = n;
    }

    void pushPixel(Pixel p) {
        pixels.push_back(p);
    }

    // This function checks all pixels that which cluster it should belong based on Distance Formula
    void assignPixels() {
        for (int i = 0; i < N; i++) {
            double mindist = INT_MAX;
            for (int j = 0; j < K; j++) {
                double dist = pixels[i].calDistance(clusters[j].centeriod);
                if (dist < mindist) {
                    mindist = dist;
                    pixels[i].clusterID = j + 1;
                }
            }
        }
    }

    // This function checks Old and New centeriods
    bool compareCenters(vector<Pixel>& Newcenters) {
        for (int i = 0; i < K; i++) {
            if (clusters[i].centeriod != Newcenters[i])
                return false;
        }
        return true;
    }

    // This function applies Kmeans to Image and make clusters by taking NoofClusters and NoofIterations as input
    void KmeansSegmentation(int k, int maxIter) {
        K = k;
        clusters.resize(K);

        // starting with random centers
        vector<Pixel> centers;
        while (centers.size() < K)
        {
            int rid = rand() % N;
            if (std::find(centers.begin(), centers.end(), pixels[rid]) == centers.end())
            {
                centers.push_back(pixels[rid]);
            }
        }

        for (int i = 0; i < centers.size(); i++) {
            clusters[i].centeriod = centers[i];
        }

        while (maxIter != 0)
        {
            //assigning points to clusters
            assignPixels();

            //recompute means of centeriods
            vector<double> total(K, 0);
            vector<double> sumx(K, 0);
            vector<double> sumy(K, 0);
            vector<double> sumz(K, 0);
            for (int i = 0; i < N; i++)
            {
                int cID = pixels[i].clusterID;
                int ind = cID - 1;
                total[ind]++;
                sumx[ind] += pixels[i].pix[0];
                sumy[ind] += pixels[i].pix[1];
                sumz[ind] += pixels[i].pix[2];
            }

            //assign new centers
            vector<Pixel> Newcenters(K);
            for (int i = 0; i < K; i++)
            {
                Pixel temp;
                temp.pix[0] = sumx[i] / total[i];
                temp.pix[1] = sumy[i] / total[i];
                temp.pix[2] = sumz[i] / total[i];
                Newcenters[i] = temp;
            }

            // check if there is change in centeriods
            if (compareCenters(Newcenters) == 1) {
                break;
            }
            else {
                for (int i = 0; i < K; i++)
                    clusters[i].centeriod = Newcenters[i];
            }
            maxIter--;
        }

        // Assigning pixels to each cluster
        for (int i = 0; i < N; i++) {
            int ind = pixels[i].clusterID - 1;
            clusters[ind].clusterpixels.push_back(pixels[i]);
        }

        int pixelIndex = 0;
        for (int row = 0; row < img.rows; row++) {
            for (int col = 0; col < img.cols; col++) {
                int clusterIndex = pixels[pixelIndex].clusterID - 1;
                img.at<Vec3b>(row, col) = clusters[clusterIndex].centeriod.pix;
                pixelIndex++;
            }
        }

    }

};

extern "C" {
    JNIEXPORT void JNICALL Java_application_Main_KmeansSegmentation
    (JNIEnv* env, jobject, jstring fname, jint k, jint maxIter) {
        const char* str = env->GetStringUTFChars(fname, 0);
        img = imread(str);
        env->ReleaseStringUTFChars(fname, str);
        int n = img.rows * img.cols;
        Image image(n);
        for (int r = 0; r < img.rows; ++r) {
            for (int c = 0; c < img.cols; ++c) {
                Vec3b pix = img.at<Vec3b>(r, c);
                image.pushPixel(Pixel(pix));
            }
        }

        image.KmeansSegmentation(k, maxIter);

        imshow("Compressed", img);
        waitKey(0);
    }
}

