#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include <cmath>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"
#include "helper.h"
using namespace std;

//Point x and y  attributes swapped or not.
bool swappedAttributes = false;


/*Checks if a triangle faced back to the camera*/
bool backfaceCulling(const Camera &cam,const Matrice &a, const Matrice &b, const Matrice &c) {
    Vec3 center = findCenterOfTriangle(a, b, c);
    Vec3 surfaceNormal = findSurfaceNormal(a,b,c);
    Vec3 dif = subtractVec3(center, cam.pos);
    if(dotProductVec3(surfaceNormal, dif) < 0)
        return false;
    else 
        return true;
}

void midpointAlgorithm(ColorfulPoint p0, ColorfulPoint p1, string s) {
    double y = p0.y;
    double d;
    if ( s=="+"){
        d = 2*(p0.y-p1.y) + (p1.x-p0.x);
    }else{
        d = 2*(p1.y-p0.y) + (p1.x-p0.x);
    }

     for(int x = p0.x; x <= p1.x; x++){             
        double alpha = (x-p0.x) / (p1.x-p0.x);
        if(swappedAttributes == true)
            ColorfulPoint::paint(y, x, alpha, p0, p1);
        else
            ColorfulPoint::paint(x, y, alpha, p0, p1);

        if (d<0) {

            if ( s=="+"){
                 y = y+1;
                d += 2*(p0.y-p1.y+p1.x-p0.x);
            }else{
                y = y-1;
                d += 2*(p1.y-p0.y+p1.x-p0.x);

            }

        }
        
        else{
            if ( s=="+"){
                d += 2*(p0.y-p1.y);
            }else{
                d += 2*(p1.y-p0.y);
            }
        }
    }
    swappedAttributes = false;
}


void midPointCheckSlope(ColorfulPoint p0, ColorfulPoint p1, string s) {
        double m = ColorfulPoint::calculateM(p0, p1);
        // The case that m = (0,1]
        if(s == "+"){
            if(m > 0 && m <= 1)
               midpointAlgorithm(p0, p1, s);
        
            // The case that m = (1, inf)
            else {
                ColorfulPoint::swapPointAttributes(p0, p1);
                swappedAttributes = true;
                midpointAlgorithm(p0, p1, s);
               
            }
        }
        if(s == "-"){
             if(m < 0 && m >= -1)
               midpointAlgorithm(p0, p1, s);
        
            // The case that m = (1, inf)
            else {
                ColorfulPoint::swapPointAttributes(p0, p1);
                swappedAttributes = true;
                midpointAlgorithm(p1, p0, s);
               
            }

        }

}

void midpointCheckY(ColorfulPoint p0, ColorfulPoint p1) {
    // The case that y0 < y1
    if(p0.y < p1.y)
        midPointCheckSlope(p0, p1, "+");

    // The case that y0 >= y1
    else if(p0.y >= p1.y){
        midPointCheckSlope(p0, p1, "-");
    }
    
}

void midpointCheckX(ColorfulPoint p0, ColorfulPoint p1) {

        // The case that x0 < x1
        if(p0.x < p1.x) 
            midpointCheckY(p0, p1);
        
        // The case that x0 > x1
        else if(p0.x >= p1.x) {
            ColorfulPoint::swapPoints(p0, p1);
            midpointCheckY(p0, p1);
        }
}

void lineRasterization(ColorfulPoint &p0, ColorfulPoint &p1, ColorfulPoint &p2) {
    midpointCheckX(p0, p1);
    midpointCheckX(p1, p2);
    midpointCheckX(p2, p0);
}

void triangleRasterization(ColorfulPoint p0, ColorfulPoint p1, ColorfulPoint p2) {
    double xminimum = minimum(p0.x, p1.x, p2.x);
    double yminimum = minimum(p0.y, p1.y, p2.y);
    double xmaximum = maximum(p0.x, p1.x, p2.x);
    double ymaximum = maximum(p0.y, p1.y, p2.y);
    
    for(int y=yminimum; y<=ymaximum; y++){
        for(int x=xminimum; x<=xmaximum; x++){
            double alpha = ColorfulPoint::findAlpha(x, y, p0, p1, p2);
            double beta =  ColorfulPoint::findBeta(x, y, p0, p1, p2);
            double gamma = ColorfulPoint::findGamma(x, y, p0, p1, p2);
            if(alpha>=0 && beta>=0 && gamma>=0){
                    image[x-1][y-1].r = p0.color.r * alpha + p1.color.r* beta + p2.color.r* gamma;
                    image[x-1][y-1].g = p0.color.g * alpha + p1.color.g* beta + p2.color.g* gamma;
                    image[x-1][y-1].b = p0.color.b * alpha + p1.color.b* beta + p2.color.b* gamma;
    
                    ///DRAWWW!!!!

            }
        }
    }
    image[(int)round(p0.x)][(int)round(p0.y)].r = p0.color.r;
    image[(int)round(p0.x)][(int)round(p0.y)].g = p0.color.g;
    image[(int)round(p0.x)][(int)round(p0.y)].b = p0.color.b;

    image[(int)round(p1.x)][(int)round(p1.y)].r = p1.color.r;
    image[(int)round(p1.x)][(int)round(p1.y)].g = p1.color.g;
    image[(int)round(p1.x)][(int)round(p1.y)].b = p1.color.b;

    image[(int)round(p2.x)][(int)round(p2.y)].r = p2.color.r;
    image[(int)round(p2.x)][(int)round(p2.y)].g = p2.color.g;
    image[(int)round(p2.x)][(int)round(p2.y)].b = p2.color.b;
}


/*
    Transformations, culling, rasterization are done here.
    You can define helper functions inside this file (rasterizer.cpp) only.
    Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/
void forwardRenderingPipeline(Camera cam) {
    // TODO: IMPLEMENT HERE
    /*View port matrix*/
    Matrice vpMatrice {{cam.sizeX/2.0, 0, 0, (cam.sizeX-1)/2.0},
                       {0, cam.sizeY/2.0, 0, (cam.sizeY-1)/2.0},
                       {0, 0, 0.5, 0.5}
                     };
    
    Matrice perAndCamMatrice = calculatePerAndCamMatrice(cam);


    for (int i = 0; i< numberOfModels; i++){

           
            Matrice modelingMatrice = executeModelingTransformations(models[i].numberOfTransformations, models[i].transformationTypes ,models[i].transformationIDs);
            /////CALCULATE TRANSFORMATION MATRICE
   

            if(models[i].type == 0){  /// WIREFRAME TYPE
                for ( int j = 0; j<models[i].numberOfTriangles; j++){

                    
                    
                    /*Get vertices of the triangle*/
                    Vec3 v_0 = vertices[models[i].triangles[j].vertexIds[0]];
                    Vec3 v_1 = vertices[models[i].triangles[j].vertexIds[1]];
                    Vec3 v_2 = vertices[models[i].triangles[j].vertexIds[2]];


                    /*Get per * cam * model * vertice*/
                    Matrice v_0_2D = multiplyMatrices(modelingMatrice,{{v_0.x}, {v_0.y}, {v_0.z}, {1}});
                    Matrice v_1_2D = multiplyMatrices(modelingMatrice,{{v_1.x}, {v_1.y}, {v_1.z}, {1}});
                    Matrice v_2_2D = multiplyMatrices(modelingMatrice,{{v_2.x}, {v_2.y}, {v_2.z}, {1}});

                    if(backfaceCulling(cam, v_0_2D, v_1_2D, v_2_2D))
                        continue;

                    v_0_2D = multiplyMatrices(perAndCamMatrice, v_0_2D);
                    v_1_2D = multiplyMatrices(perAndCamMatrice, v_1_2D);
                    v_2_2D = multiplyMatrices(perAndCamMatrice, v_2_2D);
                    
                    /*Do perspective division*/
                    for(int i = 0; i<4;i++){
                        v_0_2D[i][0] = v_0_2D[i][0] / v_0_2D[3][0];
                        v_1_2D[i][0] = v_1_2D[i][0] / v_1_2D[3][0];
                        v_2_2D[i][0] = v_2_2D[i][0] / v_2_2D[3][0];

                    }

                    /*Map vertices from 3D to 2D using viewport matrice*/
                    v_0_2D = multiplyMatrices(vpMatrice, v_0_2D);
                    v_1_2D = multiplyMatrices(vpMatrice, v_1_2D);
                    v_2_2D = multiplyMatrices(vpMatrice, v_2_2D);
                    
                    /*Create corresponding points for the vertices in 2D*/
                    ColorfulPoint p0(v_0_2D[0][0], v_0_2D[1][0], colors[v_0.colorId]);
                    ColorfulPoint p1(v_1_2D[0][0], v_1_2D[1][0], colors[v_1.colorId]);
                    ColorfulPoint p2(v_2_2D[0][0], v_2_2D[1][0], colors[v_2.colorId]);
                    
                    /*Midpoint algorithm implementation*/
                    lineRasterization(p0, p1, p2);
                   

                  


                }
               
            }else{ ///SOLID TYPE

                for ( int j = 0; j<models[i].numberOfTriangles; j++){
                    
                    /*if the face of the triangle faced  back to the camera*/
                    
                    /*Get vertices of the triangle*/
                    Vec3 v_0 = vertices[models[i].triangles[j].vertexIds[0]];
                    Vec3 v_1 = vertices[models[i].triangles[j].vertexIds[1]];
                    Vec3 v_2 = vertices[models[i].triangles[j].vertexIds[2]];


                    /*Get model * vertice*/
                    Matrice v_0_2D = multiplyMatrices(modelingMatrice,{{v_0.x}, {v_0.y}, {v_0.z}, {1}});
                    Matrice v_1_2D = multiplyMatrices(modelingMatrice,{{v_1.x}, {v_1.y}, {v_1.z}, {1}});
                    Matrice v_2_2D = multiplyMatrices(modelingMatrice,{{v_2.x}, {v_2.y}, {v_2.z}, {1}});
                    
                    if(backfaceCulling(cam, v_0_2D, v_1_2D, v_2_2D))
                        continue;
                   
                   /*Get per * cam * model * vertice*/
                    v_0_2D = multiplyMatrices(perAndCamMatrice, v_0_2D);
                    v_1_2D = multiplyMatrices(perAndCamMatrice, v_1_2D);
                    v_2_2D = multiplyMatrices(perAndCamMatrice, v_2_2D);
                    
                    /*Do perspective division*/
                    for(int i = 0; i<4;i++){
                        v_0_2D[i][0] = v_0_2D[i][0] / v_0_2D[3][0];
                        v_1_2D[i][0] = v_1_2D[i][0] / v_1_2D[3][0];
                        v_2_2D[i][0] = v_2_2D[i][0] / v_2_2D[3][0];

                    }


                    /*Map vertices from 3D to 2D using viewport matrice*/
                    v_0_2D = multiplyMatrices(vpMatrice, v_0_2D);
                    v_1_2D = multiplyMatrices(vpMatrice, v_1_2D);
                    v_2_2D = multiplyMatrices(vpMatrice, v_2_2D);
                    
                    /*Create corresponding points for the vertices in 2D*/
                   

                    ColorfulPoint p0(v_0_2D[0][0], v_0_2D[1][0], colors[v_0.colorId]);
                    ColorfulPoint p1(v_1_2D[0][0], v_1_2D[1][0], colors[v_1.colorId]);
                    ColorfulPoint p2(v_2_2D[0][0], v_2_2D[1][0], colors[v_2.colorId]);
                    
                    triangleRasterization(p0, p1, p2);
                    
                    

                }
            }
        }

}


int main(int argc, char **argv) {
    int i, j;

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    image = 0;

    for (i = 0; i < numberOfCameras; i++) {

        // allocate memory for image
        if (image) {
            for (j = 0; j < cameras[i].sizeX; j++) {
                delete image[j];
            }

            delete[] image;
        }

        image = new Color*[cameras[i].sizeX];

        if (image == NULL) {
            std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
            exit(1);
        }

        for (j = 0; j < cameras[i].sizeX; j++) {
            image[j] = new Color[cameras[i].sizeY];
            if (image[j] == NULL) {
                std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
                exit(1);
            }
        }


        // initialize image with basic values
        initializeImage(cameras[i]);

        // do forward rendering pipeline operations
        forwardRenderingPipeline(cameras[i]);

        // generate PPM file
        writeImageToPPMFile(cameras[i]);

        // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
        // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
        // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
        convertPPMToPNG(cameras[i].outputFileName, 99);
    }

    return 0;

}
