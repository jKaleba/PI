#include <stdio.h>
#include <math.h>

#define SIZE 40

void read_vector(double x[], int n) {
    for(int i = 0; i < n; ++i) {
        scanf("%lf", x++);
    }
}

void print_vector(double x[], int n) {
    for(int i = 0; i < n; ++i) {
        printf("%.4f ", x[i]);
    }
    printf("\n");
}

void read_mat(double A[][SIZE], int m, int n) {
    for(int i = 0; i < m; ++i) {
        for(int j = 0; j < n; ++j) {
            scanf("%lf", &A[i][j]);
        }
    }
}

void print_mat(double A[][SIZE], int m, int n) {
    for(int i = 0; i < m; ++i) {
        for(int j = 0; j < n; ++j) {
            printf("%.4f ", A[i][j]);
        }
        printf("\n");
    }
}

// 1. Calculate matrix product, AB = A X B
// A[m][p], B[p][n], AB[m][n]
void mat_product(double A[][SIZE], double B[][SIZE], double AB[][SIZE], int m, int p, int n) {
    double sum;
    for(int resultRow = 0; resultRow < m; resultRow++) {
        for(int resultCol = 0; resultCol < n; resultCol++) {
            sum = 0;
            for(int i = 0; i < p; i++) {
                sum += A[resultRow][i] * B[i][resultCol];
            }
            AB[resultRow][resultCol] = sum;
        }
    }
}

// 2. Matrix triangulation and determinant calculation - simplified version
// (no rows' swaps). If A[i][i] == 0, function returns NAN.
// Function may change A matrix elements.
double gauss_simplified(double A[][SIZE], int n) {

    double determinant = 1;
    for(int diagonal = 0; diagonal < n; diagonal++) {
        for(int row = diagonal + 1; row < n; row++) {
            double ratio = A[row][diagonal] / A[diagonal][diagonal];
            for(int column = diagonal; column < n; column++) {
                A[row][column] -= ratio * A[diagonal][column];
            }
        }
        determinant *= A[diagonal][diagonal];
    }
    return determinant != 0 ? determinant : NAN;
}

// 3. Matrix triangulation, determinant calculation, and Ax = b solving - extended version
// (Swap the rows so that the row with the largest, leftmost nonzero entry is on top. While
// swapping the rows use index vector - do not copy entire rows.)
// If max A[i][i] < eps, function returns 0.
// If det != 0 && b != NULL && x != NULL then vector x should contain solution of Ax = b.

int mainRow(double A[][SIZE], const int indices[], int diagonal, int n) {
    int max_idx = indices[diagonal];
    for(int i = diagonal + 1; i < n; i++) {
        if(fabs(A[indices[i]][diagonal]) > fabs(A[max_idx][diagonal])) {
            max_idx = i;
        }
    }
    return max_idx;
}

void swap(int indices[], int i, int j) {
    int t = indices[i];
    indices[i] = indices[j];
    indices[j] = t;
}

double gauss(double A[][SIZE], double b[], double x[], const int n, const double eps) {
    double determinant = 1.0;

    int indices[n];
    for(int i = 0; i < n; i++) {
        indices[i] = i;
    }

    for(int diagonal = 0; diagonal < n; diagonal++) {

        int main = mainRow(A, indices, diagonal, n);
        if(main != indices[diagonal]) {
            swap(indices, main, diagonal);
            determinant *= -1;
        }

        if(fabs(A[indices[diagonal]][diagonal]) < eps) {
            return 0.0;
        }

        for(int row = diagonal + 1; row < n; row++) {
            double ratio = A[indices[row]][diagonal] / A[indices[diagonal]][diagonal];
            for(int column = diagonal; column < n; column++) {
                A[indices[row]][column] -= ratio * A[indices[diagonal]][column];
            }
            b[indices[row]] -= ratio * b[indices[diagonal]];
        }

        determinant *= A[indices[diagonal]][diagonal];
    }

    for(int diagonal = n - 1; diagonal >= 0; diagonal--) {
        for(int row = diagonal - 1; row >= 0; row--) {
            double ratio = A[indices[row]][diagonal] / A[indices[diagonal]][diagonal];
            A[indices[row]][diagonal] -= ratio * A[indices[diagonal]][diagonal];
            b[indices[row]] -= ratio * b[indices[diagonal]];
        }
    }

    for(int i = 0; i < n; i++) {
        x[i] = b[indices[i]] / A[indices[i]][i];
    }

    return determinant;
}

void unityMatrix(double B[][SIZE], int n) {
    for(int i = 0; i < n; i++) {
        B[i][i] = 1.0;
        for(int j = 0; j < n && j != i; j++) {
            B[i][j] = .0;
        }
    }
}

void print(double A[][SIZE], double B[][SIZE], int n, const int indices[]) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            printf("%f ", A[indices[i]][j]);
        }
        printf("| ");
        for(int j = 0; j < n; j++) {
            printf("%f ", B[indices[i]][j]);
        }
        printf("\n");
    }
}

void swapRows(double B[][SIZE], int i, int j, int n) {
    for(int k = 0; k < n; k++) {
        double temp = B[i][k];
        B[i][k] = B[j][k];
        B[j][k] = temp;
    }
}

void restore(double B[][SIZE], int indices[], int n) {
    for(int i = 0; i < n; i++) {
        if(i == indices[i]) {
            continue;
        }

        swapRows(B, i, indices[i], n);
        int j = i + 1;
        while(indices[j] != i) {
            j++;
        }
        swap(indices, i, j);
    }
}

// 4. Returns the determinant; B contains the inverse of A (if det(A) != 0)
// If max A[i][i] < eps, function returns 0.
double matrix_inv(double A[][SIZE], double B[][SIZE], int n, double eps) {

    unityMatrix(B, n);
    double determinant = 1.0;
    int indices[n];
    for(int i = 0; i < n; i++) {
        indices[i] = i;
    }

    for(int diagonal = 0; diagonal < n; diagonal++) {
        int main = mainRow(A, indices, diagonal, n);
        if(main != indices[diagonal]) {
            swap(indices, main, diagonal);
            determinant *= -1;
        }

        if(fabs(A[indices[diagonal]][diagonal]) < eps) {
            return 0.0;
        }

        for(int row = diagonal + 1; row < n; row++) {
            double ratio = A[indices[row]][diagonal] / A[indices[diagonal]][diagonal];
            for(int column = diagonal; column < n; column++) {
                A[indices[row]][column] -= ratio * A[indices[diagonal]][column];
            }
            for(int column = 0; column < n; column++) {
                B[indices[row]][column] -= ratio * B[indices[diagonal]][column];
            }
        }
        determinant *= A[indices[diagonal]][diagonal];
    }

    for(int diagonal = n - 1; diagonal >= 0; diagonal--) {
        for(int row = diagonal - 1; row >= 0; row--) {
            double ratio = A[indices[row]][diagonal] / A[indices[diagonal]][diagonal];
            A[indices[row]][diagonal] -= ratio * A[indices[diagonal]][diagonal];
            for(int col = 0; col < n; col++) {
                B[indices[row]][col] -= ratio * B[indices[diagonal]][col];
            }
        }
    }

    for(int i = 0; i < n; i++) {
        double div = A[indices[i]][i];
        for(int j = 0; j < n; j++) {
            B[indices[i]][j] /= div;
        }
    }
    restore(B, indices, n);

    return determinant;
}

int main(void) {

    double A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
    double b[SIZE], x[SIZE], det, eps = 1.e-13;

    int to_do;
    int m, n, p;

    scanf ("%d", &to_do);

    switch (to_do) {
        case 1:
            scanf("%d %d %d", &m, &p, &n);
            read_mat(A, m, p);
            read_mat(B, p, n);
            mat_product(A, B, C, m, p, n);
            print_mat(C, m, n);
            break;
        case 2:
            scanf("%d", &n);
            read_mat(A, n, n);
            printf("%.4f\n", gauss_simplified(A, n));
            break;
        case 3:
            scanf("%d", &n);
            read_mat(A,n, n);
            read_vector(b, n);
            det = gauss(A, b, x, n, eps);
            printf("%.4f\n", det);
            if (det) print_vector(x, n);
            break;
        case 4:
            scanf("%d", &n);
            read_mat(A, n, n);
            det = matrix_inv(A, B, n, eps);
            printf("%.4f\n", det);
            if (det) print_mat(B, n, n);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}
