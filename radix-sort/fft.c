#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Comp {
    /* comp of the form: a + bi */
    double a, b;
} Comp;

Comp comp_create(double a, double b) {
    Comp res;
    res.a = a;
    res.b = b;
    return res;
}

Comp comp_add(Comp c1, Comp c2) {
    Comp res = c1;
    res.a += c2.a;
    res.b += c2.b;
    return res;
}

Comp comp_sub(Comp c1, Comp c2) {
    Comp res = c1;
    res.a -= c2.a;
    res.b -= c2.b;
    return res;
}

Comp comp_mul(Comp c1, Comp c2) {
    Comp res;
    res.a = c1.a * c2.a - c1.b * c2.b;
    res.b = c1.b * c2.a + c1.a * c2.b;
    return res;
}


void comp_print(Comp comp) {
}

/* const double PI = acos(-1); */
#define PI 3.141592653589793
#define SQR(x) ((x) * (x))

/* Calculate e^(ix) */
Comp comp_euler(double x) {
    Comp res;
    res.a = cos(x);
    res.b = sin(x);
    return res;
}

#define comp_mul_self(c, c2) \
do { \
    double ca = c->a; \
    c->a = ca * c2->a - c->b * c2->b; \
    c->b = c->b * c2->a + ca * c2->b; \
} while (0)

void fft(const Comp *sig, Comp *f, int s, int n, int inv) {
    int i, hn = n >> 1;
    Comp ep = comp_euler((inv ? PI : -PI) / (double)hn), ei;
    Comp *pi = &ei, *pp = &ep;
    if (!hn) *f = *sig;
    else
    {
        fft(sig, f, s << 1, hn, inv);
        fft(sig + s, f + hn, s << 1, hn, inv);
        pi->a = 1;
        pi->b = 0;
        for (i = 0; i < hn; i++)
        {
            Comp even = f[i], *pe = f + i, *po = pe + hn;
            comp_mul_self(po, pi);
            pe->a += po->a;
            pe->b += po->b;
            po->a = even.a - po->a;
            po->b = even.b - po->b;
            comp_mul_self(pi, pp);
        }
    }
}

void print_result(const Comp *sig, const Comp *sig0, int n) {
    int i;
    double err = 0;
    for (i = 0; i < n; i++)
    {
        Comp t = sig0[i];
        t.a /= n;
        t.b /= n;
        comp_print(t);
        t = comp_sub(t, sig[i]);
        err += t.a * t.a + t.b * t.b;
    }
    printf("Error Squared = %.6f\n", err);
}


void test_fft(const Comp *sig, Comp *f, Comp *sig0, int n) {
    int i;
    puts("## Cooley–Tukey FFT ##");
    fft(sig, f, 1, n, 0);
    puts("----------------------");
    fft(f, sig0, 1, n, 1);
    puts("######################");
}

int main() {
    int n, i, k;
    Comp *sig, *f, *sig0;
    scanf("%d", &k);
    n = 1 << k;
    sig = (Comp *)malloc(sizeof(Comp) * (size_t)n);
    sig0 = (Comp *)malloc(sizeof(Comp) * (size_t)n);
    f = (Comp *)malloc(sizeof(Comp) * (size_t)n);
    for (i = 0; i < n; i++)
    {
        sig[i].a = rand() % 10;
        sig[i].b = 0;
    }
    puts("## Original Signal ##");
    puts("#####################");
    test_fft(sig, f, sig0, n);

    return 0;
}
