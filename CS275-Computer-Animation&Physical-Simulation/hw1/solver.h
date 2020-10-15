#ifndef SOLVER_H_
#define SOLVER_H_
#include <eigen3/Eigen/Dense>
#include <vector>

class Solver
{
public:
    Solver(int* xs, float* ys, int size, int frames_per_gap=50);
    Solver(std::vector<int> xs, std::vector<float> ys, int frames_per_gap=50);

    void  interploate(void);
    float getNextValue(void);

private:
    std::vector<int>    x;
    std::vector<float>  y;
    std::vector<float>  res;
    std::vector<float>  a, b, c, d;
    int                 frames_per_gap;
    int                 idx;
};

#endif