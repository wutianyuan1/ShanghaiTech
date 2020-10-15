#include "solver.h"
#include <iostream>

Solver::Solver(int* xs, float* ys, int size, int frames_per_gap_)
{
    this->idx = 0;
    (this->x).resize(size);
    (this->y).resize(size);
    for (int i = 0; i < size; i++)
    {
        this->x[i] = xs[i];
        this->y[i] = ys[i];
    }
    this->frames_per_gap = frames_per_gap_;
}

Solver::Solver(std::vector<int> xs, std::vector<float> ys, int frames_per_gap_)
{
    this->idx = 0;
    this->x = std::vector<int>(xs);
    this->y = std::vector<float>(ys);
    this->frames_per_gap = frames_per_gap_;
}

void Solver::interploate()
{
    size_t key_frame_cnt = (this->x).size();
    this->res.resize((key_frame_cnt - 1) * frames_per_gap + key_frame_cnt);
    Eigen::MatrixXf mat(key_frame_cnt + 1, key_frame_cnt + 1);
    mat.setZero();
    mat(0, 1) = 1.0;
    mat(key_frame_cnt, key_frame_cnt) = 1.0;
    for (size_t i = 1; i < key_frame_cnt; i++)
    {
        float h_prev = x[i] - x[i-1];
        float h_curr = x[i+1] - x[i]; 
        mat(i, i - 1) = h_prev;
        mat(i, i)     = 2 * (h_curr + h_prev);
        mat(i, i + 1) = h_curr;
    }

    Eigen::VectorXf vec(key_frame_cnt + 1);
    vec.setZero();
    for (size_t i = 1; i < key_frame_cnt; i++)
    {
        float h_prev = x[i] - x[i-1];
        float h_curr = x[i+1] - x[i]; 
        vec(i) = ((y[i+1] - y[i]) / h_curr) - ((y[i] - y[i-1]) / h_prev);
    }
    vec = vec*6;
    Eigen::VectorXf m(key_frame_cnt + 1);
    m = mat.lu().solve(vec);
    (this->a).resize(key_frame_cnt);
    (this->b).resize(key_frame_cnt);
    (this->c).resize(key_frame_cnt);
    (this->d).resize(key_frame_cnt);
    m[0] = 0; m[key_frame_cnt] = 0;
    for (size_t i = 0; i < key_frame_cnt; i++)
    {
        float h_i = x[i+1] - x[i];
        a[i] = y[i];
        b[i] = ((y[i+1] - y[i]) / h_i) - (h_i * m[i] / 2) - (h_i * (m[i+1] - m[i])) / 6;
        c[i] = m[i]/2;
        d[i] = (m[i+1] - m[i]) / (6 * h_i);
    }
}

float Solver::getNextValue()
{
    int group_id = idx / frames_per_gap, self_id = idx % frames_per_gap;
    int x_i = x[group_id];
    float xx = group_id + (float)self_id / (float) frames_per_gap;
    idx++;
    return a[group_id] + b[group_id] * (xx - x_i) + c[group_id] * (xx - x_i) * (xx - x_i) + \
           d[group_id] * (xx - x_i) * (xx - x_i) * (xx - x_i);
}
