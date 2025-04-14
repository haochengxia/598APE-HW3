#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <sys/time.h>

float tdiff(struct timeval *start, struct timeval *end) {
    return (end->tv_sec-start->tv_sec) + 1e-6*(end->tv_usec-start->tv_usec);
  }

struct Planet {
   double mass;
   double x;
   double y;
   double vx;
   double vy;
};

struct QuadTreeNode {
   double centerX, centerY;  
   double size;           
   double totalMass;        
   double centerOfMassX;  
   double centerOfMassY;    
   bool isLeaf;             
   Planet* planet;          
   QuadTreeNode* children[4];
   
   QuadTreeNode(double cx, double cy, double s) : 
       centerX(cx), centerY(cy), size(s), totalMass(0),
       centerOfMassX(0), centerOfMassY(0), isLeaf(true), planet(nullptr) {
       for (int i = 0; i < 4; i++) {
           children[i] = nullptr;
       }
   }
   
   ~QuadTreeNode() {
       for (int i = 0; i < 4; i++) {
           if (children[i]) delete children[i];
       }
   }
};

unsigned long long seed = 100;

unsigned long long randomU64() {
  seed ^= (seed << 21);
  seed ^= (seed >> 35);
  seed ^= (seed << 4);
  return seed;
}

double randomDouble() {
   unsigned long long next = randomU64();
   next >>= (64 - 26);
   unsigned long long next2 = randomU64();
   next2 >>= (64 - 26);
   return ((next << 27) + next2) / (double)(1LL << 53);
}


int nplanets;
int timesteps;
double dt;
double G;


int getQuadrant(QuadTreeNode* node, double x, double y) {
    if (x < node->centerX) {
        return (y < node->centerY) ? 0 : 2;
    } else {
        return (y < node->centerY) ? 1 : 3;
    }
}

void insertPlanet(QuadTreeNode* node, Planet* planet) {
    if (node->isLeaf && node->planet == nullptr) {
        node->planet = planet;
        return;
    }
    
    if (node->isLeaf && node->planet != nullptr) {
        Planet* oldPlanet = node->planet;
        node->planet = nullptr;
        node->isLeaf = false;
        
        // create four children
        double qsize = node->size / 2;
        node->children[0] = new QuadTreeNode(node->centerX - qsize/2, node->centerY - qsize/2, qsize); // 左下
        node->children[1] = new QuadTreeNode(node->centerX + qsize/2, node->centerY - qsize/2, qsize); // 右下
        node->children[2] = new QuadTreeNode(node->centerX - qsize/2, node->centerY + qsize/2, qsize); // 左上
        node->children[3] = new QuadTreeNode(node->centerX + qsize/2, node->centerY + qsize/2, qsize); // 右上
        
        // insert the old planet into the appropriate child
        insertPlanet(node, oldPlanet);
    }
    
    // determine which child to insert the new planet into
    int quadrant = getQuadrant(node, planet->x, planet->y);
    insertPlanet(node->children[quadrant], planet);
}

void calculateCenterOfMass(QuadTreeNode* node) {
    if (node == nullptr) return;
    
    if (node->isLeaf && node->planet != nullptr) {
        node->totalMass = node->planet->mass;
        node->centerOfMassX = node->planet->x;
        node->centerOfMassY = node->planet->y;
        return;
    }
    
    node->totalMass = 0;
    node->centerOfMassX = 0;
    node->centerOfMassY = 0;
    
    for (int i = 0; i < 4; i++) {
        if (node->children[i] != nullptr) {
            calculateCenterOfMass(node->children[i]);
            node->totalMass += node->children[i]->totalMass;
            node->centerOfMassX += node->children[i]->centerOfMassX * node->children[i]->totalMass;
            node->centerOfMassY += node->children[i]->centerOfMassY * node->children[i]->totalMass;
        }
    }
    
    if (node->totalMass > 0) {
        node->centerOfMassX /= node->totalMass;
        node->centerOfMassY /= node->totalMass;
    }
}

QuadTreeNode* buildQuadTree(Planet* planets, int numPlanets, double centerX, double centerY, double size) {
    QuadTreeNode* root = new QuadTreeNode(centerX, centerY, size);
    
    // insert all planets into the quadtree
    for (int i = 0; i < numPlanets; i++) {
        insertPlanet(root, &planets[i]);
    }
    
    // calculate the center of mass for each node
    calculateCenterOfMass(root);
    
    return root;
}

void calculateForces(Planet* planet, QuadTreeNode* node, double theta) {
    if (node == nullptr || (node->isLeaf && node->planet == planet)) {
        return;
    }
    
    double dx = node->centerOfMassX - planet->x;
    double dy = node->centerOfMassY - planet->y;
    double distSqr = dx*dx + dy*dy + 0.0001;
    double distance = sqrt(distSqr);
    
    // if the node is far enough away, use the center of mass
    if (node->isLeaf || (node->size / distance < theta)) {
        // use the center of mass for force calculation
        double invDist = G * planet->mass * node->totalMass / (distSqr * distance);
        planet->vx += dt * dx * invDist;
        planet->vy += dt * dy * invDist;
    } else {
        // if the node is not a leaf, recursively calculate forces for its children
        for (int i = 0; i < 4; i++) {
            if (node->children[i] != nullptr) {
                calculateForces(planet, node->children[i], theta);
            }
        }
    }
}

// Barnes-Hut
Planet* next_barnes_hut(Planet* planets) {
    Planet* nextplanets = (Planet*)malloc(sizeof(Planet) * nplanets);
    
    // copy
    #pragma omp parallel for
    for (int i = 0; i < nplanets; i++) {
        nextplanets[i] = planets[i];
    }
    
    // determine the bounding box of all planets
    double minX = planets[0].x, maxX = planets[0].x;
    double minY = planets[0].y, maxY = planets[0].y;
    
    for (int i = 1; i < nplanets; i++) {
        if (planets[i].x < minX) minX = planets[i].x;
        if (planets[i].x > maxX) maxX = planets[i].x;
        if (planets[i].y < minY) minY = planets[i].y;
        if (planets[i].y > maxY) maxY = planets[i].y;
    }
    
    // calculate the center and size of the bounding box
    double centerX = (minX + maxX) / 2;
    double centerY = (minY + maxY) / 2;
    double size = std::max(maxX - minX, maxY - minY) * 1.1; // add some padding
    
    // create the quadtree
    QuadTreeNode* root = buildQuadTree(planets, nplanets, centerX, centerY, size);
    
    // use the quadtree to calculate forces
    #pragma omp parallel for
    for (int i = 0; i < nplanets; i++) {
        calculateForces(&nextplanets[i], root, 0.5); // theta = 0.5
        nextplanets[i].x += dt * nextplanets[i].vx;
        nextplanets[i].y += dt * nextplanets[i].vy;
    }
    
    // clean up
    delete root;
    free(planets);
    
    return nextplanets;
}

// original
#ifdef ENBALE_ALLOCAETED_NEXT
Planet* next(Planet* planets, Planet* nextplanets) {
#else
Planet* next(Planet* planets) {
   Planet* nextplanets = (Planet*)malloc(sizeof(Planet) * nplanets);
#endif
#ifdef ENABLE_OPENMP
#pragma omp parallel for
#endif
   for (int i=0; i<nplanets; i++) {
      nextplanets[i].vx = planets[i].vx;
      nextplanets[i].vy = planets[i].vy;
      nextplanets[i].mass = planets[i].mass;
      nextplanets[i].x = planets[i].x;
      nextplanets[i].y = planets[i].y;
   }

#ifdef ENABLE_OPENMP
#pragma omp parallel for
#endif
   for (int i=0; i<nplanets; i++) {
      for (int j=0; j<nplanets; j++) {
         if (i == j) continue;
         double dx = planets[j].x - planets[i].x;
         double dy = planets[j].y - planets[i].y;
         double distSqr = dx*dx + dy*dy + 0.0001;
         double invDist = planets[i].mass * planets[j].mass / sqrt(distSqr);
         double invDist3 = invDist * invDist * invDist;
         nextplanets[i].vx += dt * dx * invDist3;
         nextplanets[i].vy += dt * dy * invDist3;
      }
      nextplanets[i].x += dt * nextplanets[i].vx;
      nextplanets[i].y += dt * nextplanets[i].vy;
   }
#ifdef ENBALE_ALLOCAETED_NEXT
   return 0;
#else
   free(planets);
   return nextplanets;
#endif
}

int main(int argc, const char** argv) {
    if (argc < 2) {
        printf("Usage: %s <nplanets> <timesteps>\n", argv[0]);
        return 1;
     }
    
    nplanets = atoi(argv[1]);
    timesteps = atoi(argv[2]);
    dt = 0.001;
    G = 6.6743;
    
#ifdef ENBALE_BARNES_HUT
    // Use Barnes-Hut algorithm if enabled
    bool use_barnes_hut = true;
#else
    bool use_barnes_hut = false;
#endif

    // if (use_barnes_hut) {
    //     printf("Using Barnes-Hut algorithm for simulation\n");
    // } else {
    //     printf("Using traditional O(n^2) algorithm for simulation\n");
    // }
    
    Planet* planets = (Planet*)malloc(sizeof(Planet) * nplanets);
#ifdef ENBALE_ALLOCAETED_NEXT
    Planet* next_planets = (Planet*)malloc(sizeof(Planet) * nplanets);
#endif
    
    for (int i=0; i<nplanets; i++) {
        planets[i].mass = randomDouble() * 10 + 0.2;
        planets[i].x = (randomDouble() - 0.5) * 100 * pow(1 + nplanets, 0.4);
        planets[i].y = (randomDouble() - 0.5) * 100 * pow(1 + nplanets, 0.4);
        planets[i].vx = randomDouble() * 5 - 2.5;
        planets[i].vy = randomDouble() * 5 - 2.5;
    }
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    for (int i=0; i<timesteps; i++) {
        if (use_barnes_hut) {
            planets = next_barnes_hut(planets);
        } else {
#ifdef ENBALE_ALLOCAETED_NEXT
            next(planets, next_planets);
            Planet* temp = planets;
            planets = next_planets;
            next_planets = temp;
#else
            planets = next(planets);
#endif
        }
    }
    
    gettimeofday(&end, NULL);
    printf("Total time to run simulation %0.6f seconds, final location %f %f\n", 
           tdiff(&start, &end), planets[nplanets-1].x, planets[nplanets-1].y);
    
    free(planets);
#ifdef ENBALE_ALLOCAETED_NEXT
    if (!use_barnes_hut) {
        free(next_planets);
    }
#endif
    
    return 0;
}