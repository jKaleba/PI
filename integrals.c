#include <stdio.h>
#include <math.h>

#define RECURS_LEVEL_MAX  10
#define N_MAX             10

// pointer to function of one variable
typedef double (*Func1vFp)(double);

// example functions of one variable
double f_poly(double x) { // polynomial a[0] + a[1]x + ... + a[n]x^n
    return 2 * pow(x, 5) - 4 * pow(x, 4) + 3.5 * pow(x, 2) + 1.35 * pow(x, 1) - 6.25;
}

double f_rat(double x) {
    return 1 / (pow(x - 0.5, 2) + 0.01);
}

double f_exp(double x) {
    return 2 * x * pow(M_E, -1.5 * x) - 1;
}

double f_trig(double x) {
    return x * tan(x);
}

// Quadratures

// rectangle rule, leftpoint
double quad_rect_left(Func1vFp f1, double a, double b, double n) {
    double h = (b - a) / n, sum = 0;

    for(int i = 0; i < n; i++) {
        sum += f1(a + i * h);
    }

    return h * sum;
}

// rectangle rule, rightpoint
double quad_rect_right(Func1vFp f1, double a, double b, double n) {
    double h = (b - a) / n, sum = 0;

    for(int i = 0; i < n; i++) {
        sum += f1(a + (i + 1) * h);
    }

    return h * sum;
}

// rectangle rule, midpoint
double quad_rect_mid(Func1vFp f1, double a, double b, double n) {
    double h = (b - a) / n, sum = 0;

    for(int i = 0; i < n; i++) {
        sum += f1(a + ((i + 0.5) * h));
    }

    return h * sum;
}

// trapezoidal rule
double quad_trap(Func1vFp func, double a, double b, double n) {
    double h = (b - a) / n, sum = 0;

    double left = func(a), right;

    for(int i = 0; i < n; i++) {
        right = func(a + (i + 1) * h);
        sum += (left + right);
        left = right;
    }

    return h / 2 * sum;
}

// Simpson's rule
double quad_simpson(Func1vFp f, double a, double b, double n) {
    double h = (b - a) / n, sum = 0, c;

    double left = f(a), middle, right;

    for(int i = 0; i < n; i++) {
        middle = f(a + (i + 0.5) * h);
        right = f(a + (i + 1) * h);
        sum += (left + 4 * middle + right);
        left = right;
    }

    return h / 6 * sum;
}

// pointer to quadrature function
typedef double (*QuadratureFp)(Func1vFp , double, double, double);

// array of pointers to integrand functions
Func1vFp func_tab[] = { f_poly, f_rat, f_trig, f_exp };

// array of pointers to quadrature functions
QuadratureFp quad_tab[] = {
        quad_rect_left, quad_rect_right, quad_rect_mid, quad_trap, quad_simpson
};

// calls 'quad_no' quadrature function for 'fun_no' integrand function
// on interval [a, b] and n subintervals
double quad_select(int fun_no, int quad_no, double a, double b, int n) {
    return quad_tab[quad_no](func_tab[fun_no], a, b, n);
}


// adaptive algorithm
double recurs(Func1vFp f, double a, double b, double S, double delta, QuadratureFp quad, int level) {
    double S1, S2, c;
    c = (a + b) / 2;
    S1 = quad(f, a, c, 1);
    S2 = quad(f, c, b, 1);

    if(fabs(S1 + S2 - S) > delta) {
        if(level  > RECURS_LEVEL_MAX) {
            return NAN;
        }

        return recurs(f, a, c, S1, delta / 2, quad, level + 1) + recurs(f, c, b, S2, delta / 2, quad, level + 1);
    }

    return S1 + S2;
}

// initialization for adaptive algorithm
double init_recurs(Func1vFp f, double a, double b, double delta, QuadratureFp quad) {
    return recurs(f, a, b, quad(f, a, b, 1), delta, quad, 1);
}

// double integrals

// pointer to function of two variables
typedef double (*Func2vFp)(double, double);

double func2v_2(double x, double y) {
    return 2 - x*x - y*y*y;
}

// sample functions to define the normal domain

double lower_bound2(double x) {
    return 0.7*exp(-2*x*x);
}
double upper_bound2(double x) {
    return sin(10*x);
}

// rectangle rule (leftpoint) - double integral over rectangular domain
double dbl_integr(Func2vFp f, double x1, double x2, int nx, double y1, double y2, int ny) {
    double hx = (x2 - x1) / nx, hy = (y2 - y1) / ny, sum = 0;
    for(int i = 0; i < nx; i++) {
        for(int j = 0; j < ny; j++) {
            sum += f(x1 + i * hx, y1 + j * hy);
        }
    }

    return hx * hy * sum;
}

// rectangle rule (midpoint) - double integral over normal domain with respect to the x-axis
double dbl_integr_normal_1(Func2vFp f, double x1, double x2, int nx, double hy, Func1vFp fg, Func1vFp fh) {
    double hx = (x2 - x1) / nx, sum = 0, xi, gi, hi;
    int ny;

    for(int i = 0; i < nx; i++) {

        xi = x1 + (i + 0.5) * hx;
        gi = fg(xi);
        hi = fh(xi);
        ny = ceil((hi - gi) / hy);

        for(int j = 0; j < ny; j++) {
            if(gi + (j + 0.5) * hy <= hi) {
                sum += f(xi, gi + (j + 0.5) * hy);
            }
        }
    }

    return hx * hy * sum;
}

// rectangle rule (leftpoint) - double integral over multiple normal
// domains with respect to the x-axis
double dbl_integr_normal_n(Func2vFp f, double x1, double x2, int nx, double y1, double y2, int ny, Func1vFp fg, Func1vFp fh)  {
    double hx = (x2 - x1) / nx, hy = (y2 - y1) / ny, sum = 0, xi, gi, hi, yi;

    xi = x1;
    for(int i = 0; i < nx; i++) {
        gi = fg(xi);
        hi = fh(xi);
        double upBound = fmin(y2, hi);
        double downBound = fmax(y1, gi);

        int currNy = ceil((upBound - downBound) / hy);
        double currHy = (upBound - downBound) / currNy;

        yi = fmax(y1, gi);
        for (int j = 0; j < currNy; j++) {
            sum += currHy * f(xi, yi);
            yi += currHy;
        }
        xi += hx;
    }
    return hx * sum;
}

// multiple quadratures

typedef double (*FuncNvFp)(const double*, int);
typedef int (*BoundNvFp)(const double*, int);

// sample function of three variables
double func3v(const double v[], int n) {
    return v[0] - v[1] + 2*v[2];
}

// sample predicate in 3D
int bound3v(const double v[], int n) { // a cylinder
    return v[0] > 0 && v[0] < 0.5 && v[1]*v[1] + (v[2]-1)*(v[2]-1) < 1;
}

// sample function of n variables
double funcNv(const double v[], int n) {
    double fv = 1.;
    for (int i = 1; i < n; ++i) {
        fv += sin(i*v[i]);
    }
    return fv;
}
// sample n-dimensional predicate (n-dim hypersphere)
int boundNv(const double v[], int n) {
    double r = 0.0;
    for (int i = 0; i < n; ++i) r += (v[i]-1)*(v[i]-1);
    return r <= 1.;
}

// multiple integrals over a cuboid with predicate (if boundary != NULL)
// rectangular rule (rightpoint)
double trpl_quad_rect(FuncNvFp f, double variable_lim[][2], const int tn[], BoundNvFp boundary) {
    double x1 = variable_lim[0][0], x2 = variable_lim[0][1],
           y1 = variable_lim[1][0], y2 = variable_lim[1][1],
           z1 = variable_lim[2][0], z2 = variable_lim[2][1];

    double dx = (x2 - x1) / tn[0], dy = (y2 - y1) / tn[1], dz = (z2 - z1) / tn[2], sum = 0.0;
    double xi = x1, yi = y1, zi = z1;

    for(int i = 0; i < tn[0]; i++) {
        xi += dx;
        for(int j = 0; j < tn[1]; j++) {
            yi += dy;
            for(int k = 0; k < tn[2]; k++) {
                zi += dz;
                double v[] = {xi, yi, zi};
                if(boundary != NULL && !boundary(v, 3)) {
                    continue;
                }
                sum += f(v, tn[2]);
            }
            zi = z1;
        }
        yi = y1;
    }

    return dx * dy * dz * sum;
}

// multiple integrals over a n-dim hypercuboid with predicate (if boundary != NULL)
// rectangular rule (midpoint)
void recur_quad_rect_mid(double *p_sum, FuncNvFp f, int variable_no, double t_variable[], double variable_lim[][2], const int tn[], int level, BoundNvFp boundary) {
    double x1 = variable_lim[level][0], x2 = variable_lim[level][1], nx = tn[level], dx = (x2 - x1) / nx;
    double xi = x1 + dx / 2;
    double fx;

    if(level == variable_no - 1) {
        for(int i = 0; i < tn[level]; i++) {
            t_variable[level] = xi;
            if(boundary != NULL && !boundary(t_variable, variable_no)) {
                xi += dx;
                continue;
            }

            fx = f(t_variable, variable_no);
            for(int lvl = 0; lvl < variable_no; lvl++) {
                fx *= (variable_lim[lvl][1] - variable_lim[lvl][0]) / tn[lvl];
            }

            *p_sum += fx;
            xi += dx;
        }
    }
    else {
        for(int i = 0; i < tn[level]; i++) {
            t_variable[level] = xi;
            recur_quad_rect_mid(p_sum, f, variable_no, t_variable, variable_lim, tn, level + 1, boundary);
            xi += dx;
        }
    }
}

int main(void) {
    int to_do, n, nx, ny, integrand_fun_no, method_no, flag;
    int no_funcs = sizeof(func_tab) / sizeof(Func1vFp);
    int no_quads = sizeof(quad_tab) / sizeof(QuadratureFp);
    double a, b, x1, x2, y1, y2, hy, sum, delta;
    double t_variable[N_MAX], variable_lim[N_MAX][2];
    int tn[N_MAX];

    scanf("%d", &to_do);
    switch (to_do) {
        case 1: // loop over quadratures and integrands
            scanf("%lf %lf %d", &a, &b, &n);
            for(int q = 0; q < no_quads; ++q) {
                for(int f = 0; f < no_funcs; ++f) {
                    printf("%.5f ",quad_select(f, q, a, b, n));
                }
                printf("\n");
            }
            break;
        case 2: // adaptive algorithm
            scanf("%d %d",&integrand_fun_no,&method_no);
            scanf("%lf %lf %lf", &a, &b, &delta);
            printf("%.5f\n", init_recurs(func_tab[integrand_fun_no],a,b,delta,quad_tab[method_no]));
            break;
        case 3: // double integral over a rectangle
            scanf("%lf %lf %d", &x1, &x2, &nx);
            scanf("%lf %lf %d", &y1, &y2, &ny);
            printf("%.5f\n", dbl_integr(func2v_2, x1, x2, nx, y1, y2, ny));
            break;
        case 4: // double integral over normal domain
            scanf("%lf %lf %d", &x1, &x2, &nx);
            scanf("%lf", &hy);
            printf("%.5f\n", dbl_integr_normal_1(func2v_2, x1, x2, nx, hy, lower_bound2, upper_bound2));
            break;
        case 5: // double integral over multiple normal domains
            scanf("%lf %lf %d", &x1, &x2, &nx);
            scanf("%lf %lf %d", &y1, &y2, &ny);
            printf("%.5f\n",dbl_integr_normal_n(func2v_2, x1, x2, nx, y1, y2, ny, lower_bound2, upper_bound2));
            break;
        case 6: // triple integral over a cuboid
            scanf("%lf %lf %d", &variable_lim[0][0], &variable_lim[0][1], tn);
            scanf("%lf %lf %d", &variable_lim[1][0], &variable_lim[1][1], tn+1);
            scanf("%lf %lf %d", &variable_lim[2][0], &variable_lim[2][1], tn+2);
            scanf("%d", &flag);
            printf("%.5f\n", trpl_quad_rect(func3v, variable_lim, tn, flag ? bound3v : NULL));
            break;
        case 7: // multiple integral over hyper-cuboid
            scanf("%d", &n);
            if(n > N_MAX) break;
            for(int i = 0; i < n; ++i) {
                scanf("%lf %lf %d", &variable_lim[i][0], &variable_lim[i][1], tn+i);
            }
            scanf("%d", &flag);
            sum = 0.;
            recur_quad_rect_mid(&sum, funcNv, n, t_variable, variable_lim, tn, 0, flag ? boundNv : NULL);
            printf("%.5f\n", sum);
            break;
        default:
            printf("Nothing to do for %d\n", to_do);
            break;
    }
    return 0;
}

