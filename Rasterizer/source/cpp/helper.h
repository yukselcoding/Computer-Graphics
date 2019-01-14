#include <algorithm>
#include <vector>
#include <cmath>
#define  Matrice vector<vector<double>>
#define PI 3.14159265
using namespace std;


Camera cameras[100];
int numberOfCameras = 0;

Model models[1000];
int numberOfModels = 0;

Color colors[100000];
int numberOfColors = 0;

Translation translations[1000];
int numberOfTranslations = 0;

Rotation rotations[1000];
int numberOfRotations = 0;

Scaling scalings[1000];
int numberOfScalings = 0;

Vec3 vertices[100000];
int numberOfVertices = 0;

Color backgroundColor;

// backface culling setting, default disabled
int backfaceCullingSetting = 0;

Color **image;


struct ColorfulPoint {
    double x, y;
    Color color;
    ColorfulPoint(double x, double y, Color color): x(x), y(y), color(color) {}
    
    static double calculateM(const ColorfulPoint &p0, const ColorfulPoint &p1) {
        return (p1.y-p0.y)/(p1.x-p0.x);
    }

    /*Paints the pixel*/
    static void paint(const int &x, const int &y, const double &alpha, const ColorfulPoint &p0, const ColorfulPoint &p1) {
        Color c;
        c.r = (1-alpha)*p0.color.r + alpha*p1.color.r;
        c.g = (1-alpha)*p0.color.g + alpha*p1.color.g;
        c.b = (1-alpha)*p0.color.b + alpha*p1.color.b;
        image[x][y].r = c.r;
        image[x][y].g = c.g;
        image[x][y].b = c.b;
    }

    /*Swaps two points*/
    static void swapPoints(ColorfulPoint &p1, ColorfulPoint &p2) {
        ColorfulPoint temp = p1;
        p1 = p2;
        p2 = temp;
    }

    /*Swap point attributes*/
    static void swapPointAttributes(ColorfulPoint &p0, ColorfulPoint &p1) {
        ColorfulPoint temp0(p0.x, p0.y, p0.color);
        ColorfulPoint temp1(p1.x, p1.y, p1.color);
        p0.x = temp0.y;
        p0.y = temp0.x;
        p1.x = temp1.y;
        p1.y = temp1.x;
    }
    
    /*Find alpha constant*/
    static double findAlpha(const double &x, const double &y, const ColorfulPoint &p0, const ColorfulPoint &p1, const ColorfulPoint &p2) {
        return (f_12(x,y,p1.x,p1.y,p2.x,p2.y) / f_12(p0.x, p0.y,p1.x,p1.y,p2.x,p2.y));
    }
    
    /*Find beta constant*/
    static double findBeta(const double &x, const double &y, const ColorfulPoint &p0, const ColorfulPoint &p1, const ColorfulPoint &p2) {
        return (f_20(x,y,p2.x,p2.y,p0.x,p0.y) / f_20(p1.x, p1.y,p2.x,p2.y,p0.x,p0.y));
    }
    
    /*Find gamma constant*/
    static double findGamma(const double &x, const double &y, const ColorfulPoint &p0, const ColorfulPoint &p1, const ColorfulPoint &p2) {
        return (f_01(x,y,p0.x,p0.y,p1.x,p1.y) / f_01(p2.x, p2.y,p0.x,p0.y,p1.x,p1.y));
    }

    static double f_20(const int &x, const int &y, const int &x2, const int &y2, const int &x0, const int &y0){
        return x*(y2-y0) + y*(x0-x2) + x2*y0 - y2*x0;
    }

    static double f_01(const int &x, const int &y, const int &x0, const int &y0, const int &x1, const int &y1){
        return x*(y0-y1) + y*(x1-x0) + x0*y1 - y0*x1;
    }

    static double f_12(const int &x, const int &y, const int &x1, const int &y1, const int &x2, const int &y2){
        return x*(y1-y2) + y*(x2-x1) + x1*y2 - y1*x2;
    }

};





/*
    Initializes image with background color
*/
void initializeImage(Camera cam) {
    int i, j;

    for (i = 0; i < cam.sizeX; i++)
        for (j = 0; j < cam.sizeY; j++) {
            image[i][j].r = backgroundColor.r;
            image[i][j].g = backgroundColor.g;
            image[i][j].b = backgroundColor.b;

        }
}

/*Prints the matrice*/
void printMatrice(const Matrice &m){
    for(int i=0; i<m.size(); i++) {
        for(int j=0; j<m[0].size(); j++) {
            cout << m[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

}

/*Multiplies the matrices with any size, must be suitable for matrice multiplication of course :))*/
Matrice multiplyMatrices(const Matrice &m1, const Matrice &m2) {
    int rowFirst = m1.size();
    int columnFirst = m1[0].size();
    int rowSecond = m2.size();
    int columnSecond = m2[0].size();
    Matrice result(rowFirst, vector<double>(columnSecond));
    double total = 0;
    for (int i = 0; i < rowFirst; i++)
        for (int j = 0; j < columnSecond; j++) {
            total = 0;
            for (int k = 0; k < columnFirst; k++) {
                total += m1[i][k] * m2[k][j];
            }
                
            result[i][j] = total;
    }
    return result;
    
}


/*Creates orthonormal matrix called M*/
Matrice createOrthonormalMatrice(const Vec3 &u, const Vec3 &v, const Vec3 &w) {
    Matrice orthonormal { {u.x,u.y,u.z,0},
                          {v.x,v.y,v.z,0},
                          {w.x,w.y,w.z,0},
                          {0,0,0,1.0}
                        };
    return orthonormal;
}

/*Creates inverse orthonormal matrix called M-1*/
Matrice createInverseOrthonormalMatrice(const Vec3 &u, const Vec3 &v, const Vec3 &w) {
    Matrice inverseOrthonormal { {u.x,v.x,w.x,0},
                                 {u.y,v.y,w.y,0},
                                 {u.z,v.z,w.z,0},
                                 {0,0,0,1.0}
                               };
    return inverseOrthonormal;
}

/*Creates rotate matrice, rotation around arbitrary axis Rx(theta)*/
Matrice createRotationMatrice(const Rotation &r) {
    Matrice rotationX { {1.0,0,0,0},
                        {0,cos(r.angle*PI/180.0),(-1.0)*sin(r.angle*PI/180.0),0},
                        {0,sin(r.angle*PI/180.0),cos(r.angle*PI/180.0),0},
                        {0,0,0,1.0}
                      };

    Vec3 u;
    u.x = r.ux;
    u.y = r.uy;
    u.z = r.uz;
    u = normalizeVec3(u);

    Vec3 v;
    if(!(r.ux == 0 && r.uy == 0)){
        v.x = -u.y;
        v.y = u.x;
        v.z = 0;
    }else{
        v.x = r.uz;
        v.y = 0;
        v.z = 0;
    }
    v = normalizeVec3(v);
    
    Vec3 w = crossProductVec3(u, v);
    w = normalizeVec3(w);

    
    Matrice M = createOrthonormalMatrice(u,v,w);
    Matrice inverseM = createInverseOrthonormalMatrice(u,v,w);

    return multiplyMatrices(inverseM, multiplyMatrices(rotationX, M));
}


/*Creates the translation matrice with respect to the translation values*/
Matrice createTranslationMatrice(const Translation &tr) {
    Matrice translationMatrice { {1.0,0,0,tr.tx},
                                 {0,1.0,0,tr.ty},
                                 {0,0,1.0,tr.tz},
                                 {0,0,0,1.0}
                               };

    return translationMatrice;
}

/*Creates the scaling matrice with respect to the scaling values*/
Matrice createScalingMatrice(const Scaling &sc) {
    Matrice scalingMatrice { {sc.sx,0,0,0},
                             {0,sc.sy,0,0},
                             {0,0,sc.sz,0},
                             {0,0,0,1.0}
                           };
    return scalingMatrice;
}

/*Executes modeling transformations, returns Modeling matrice.*/
Matrice executeModelingTransformations(int numberOfTransformations, char* transformationTypes, int* transformationIDs) {

    Matrice modelingMatrice {{1.0,0,0,0},
                             {0,1.0,0,0},
                             {0,0,1.0,0},
                             {0,0,0,1.0}
                            };

    for (int i = 0; i<numberOfTransformations; i++){
         
        if(transformationTypes[i] == 'r'){
            Matrice rotation = createRotationMatrice(rotations[transformationIDs[i]]);
            modelingMatrice = multiplyMatrices(rotation, modelingMatrice);

        }else if(transformationTypes[i] == 's'){
            Matrice scaling = createScalingMatrice(scalings[transformationIDs[i]]);
            modelingMatrice = multiplyMatrices(scaling, modelingMatrice);

        }else if(transformationTypes[i] == 't'){
            Matrice translation = createTranslationMatrice(translations[transformationIDs[i]]);
            modelingMatrice = multiplyMatrices(translation, modelingMatrice);

        }
    }
    return modelingMatrice;
}



/*Calculates the composite matrice viewport * perspective * camera*/
Matrice calculatePerAndCamMatrice(const Camera &cam) {
    /*Camera transformation*/
     Matrice rotateCamera  { {cam.u.x, cam.u.y, cam.u.z,0},
                            {cam.v.x, cam.v.y, cam.v.z,0},
                            {cam.w.x, cam.w.y, cam.w.z,0},
                            {0,0,0,1.0}
                          };

    Matrice translateCamera { {1.0,0,0,(-1.0)*cam.pos.x},
                              {0,1.0,0,(-1.0)*cam.pos.y},
                              {0,0,1.0,(-1.0)*cam.pos.z},
                              {0,0,0,1.0}
                            };


    Matrice cameraTransformation = multiplyMatrices(rotateCamera, translateCamera);


    /*Perspective Projection*/
    Matrice orthographic { {2.0/(cam.r-cam.l), 0, 0, (-1.0)*((cam.r+cam.l)/(cam.r-cam.l))},
                           {0, 2.0/(cam.t-cam.b), 0, (-1.0)*((cam.t+cam.b)/(cam.t-cam.b))}, 
                           {0, 0, -2.0/(cam.f-cam.n), (-1.0)*((cam.f+cam.n)/(cam.f-cam.n))},
                           {0,0,0,1.0}
                         };

    Matrice p2o { {cam.n, 0, 0, 0},
                  {0, cam.n, 0, 0},
                  {0, 0, cam.f+cam.n, cam.f*cam.n},
                  {0,0,-1.0,0}
                };

    Matrice perspectiveProjection = multiplyMatrices(orthographic, p2o);

    return multiplyMatrices(perspectiveProjection, cameraTransformation);

}


/*Finds minimum of three numbers*/
double minimum(const double &x0, const double &x1, const double &x2) {
    return min(x0, min(x1, x2));
}

/*Finds maximum of three numbers*/
double maximum(const double &x0, const double &x1, const double &x2) {
    return max(x0, max(x1, x2));
}


/*Find magnitude of a vertice*/
double magnitude(const Vec3 &a) {
    return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}


/*Converts matrice to Vec3*/
Vec3 matriceToVec3(const Matrice &a) {
    Vec3 result;
    result.x = a[0][0];
    result.y = a[1][0];
    result.z = a[2][0];
    return result;
}

/*Finds surface normal of a triangle*/
Vec3 findSurfaceNormal(const Matrice &m1, const Matrice &m2, const Matrice &m3) {
    Vec3 a = matriceToVec3(m1);
    Vec3 b = matriceToVec3(m2);
    Vec3 c = matriceToVec3(m3);
    Vec3 cross = crossProductVec3(subtractVec3(b,a), subtractVec3(c,a));
    return normalizeVec3(cross);
}

/*Finds center of the triangle*/
Vec3 findCenterOfTriangle(const Matrice &m1, const Matrice &m2, const Matrice &m3) {
    Vec3 a = matriceToVec3(m1);
    Vec3 b = matriceToVec3(m2);
    Vec3 c = matriceToVec3(m3);
    Vec3 center;
    center.x = (a.x + b.x + c.x) / 3.0;
    center.y = (a.y + b.y + c.y) / 3.0;
    center.z = (a.z + b.z + c.z) / 3.0;
    return center;
}

