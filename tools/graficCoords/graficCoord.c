#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

float omegam, omegav;

float func_dtdacube(float a);
float rombint(float (*f)(float), float a, float b, float tol);

int main(int argc, char *argv[]) {
    char fname[800];
    float *slab;
    int np1, np2, np3;
    float dx, x1o, x2o, x3o, astart, h0;
    float ainv, adot, tmp1, growth, tmp, fupper, flower, growthVel, vFact;
    int i, k;
    FILE *fp1, *fp2;
    int rec;

    if (argc < 2) {
        printf("Usage: grafic_coord velocity_file\n");
        return 1;
    }

    strcpy(fname, argv[1]);

    fp1 = fopen(fname, "rb");
    if (!fp1) {
        printf("Error: Cannot open input file %s\n", fname);
        return 1;
    }

    // Read header
    
    fread(&rec, sizeof(int), 1, fp1);
    fread(&np1, sizeof(int), 1, fp1);
    fread(&np2, sizeof(int), 1, fp1);
    fread(&np3, sizeof(int), 1, fp1);
    fread(&dx, sizeof(float), 1, fp1);
    fread(&x1o, sizeof(float), 1, fp1);
    fread(&x2o, sizeof(float), 1, fp1);
    fread(&x3o, sizeof(float), 1, fp1);
    fread(&astart, sizeof(float), 1, fp1);
    fread(&omegam, sizeof(float), 1, fp1);
    fread(&omegav, sizeof(float), 1, fp1);
    fread(&h0, sizeof(float), 1, fp1);
    fread(&rec, sizeof(int), 1, fp1);

    // Create output filename and open output file
    memcpy(fname+strlen(fname)-4, "pos", 3);
    fp2 = fopen(fname, "wb");
    if (!fp2) {
        printf("Error: Cannot open output file %s\n", fname);
        fclose(fp1);
        return 1;
    }

    // Write header to output
    fwrite(&rec, sizeof(int), 1, fp2);
    fwrite(&np1, sizeof(int), 1, fp2);
    fwrite(&np2, sizeof(int), 1, fp2);
    fwrite(&np3, sizeof(int), 1, fp2);
    fwrite(&dx, sizeof(float), 1, fp2);
    fwrite(&x1o, sizeof(float), 1, fp2);
    fwrite(&x2o, sizeof(float), 1, fp2);
    fwrite(&x3o, sizeof(float), 1, fp2);
    fwrite(&astart, sizeof(float), 1, fp2);
    fwrite(&omegam, sizeof(float), 1, fp2);
    fwrite(&omegav, sizeof(float), 1, fp2);
    fwrite(&h0, sizeof(float), 1, fp2);
    fwrite(&rec, sizeof(int), 1, fp2);

    // Calculate velocity factor
    ainv = 1.0 / astart;
    adot = sqrt(omegam * (ainv - 1.0) + omegav * (astart * astart - 1.0) + 1.0);
    tmp1 = rombint(func_dtdacube, 1e-6, astart, 1e-5);
    
    growth = 2.5 * omegam * adot * tmp1 / astart;
    tmp = 1.0 - omegam - omegav;
    fupper = 2.5 * omegam / growth - 1.5 * omegam * ainv - tmp;
    flower = omegam * ainv + omegav * astart * astart + tmp;
    growthVel = fupper / flower;

    vFact = 1.0 / (adot * 100.0 * growthVel);

    printf("vFact: %f\n", vFact);
    printf("growthVel: %f\n", growthVel);
    printf("growth: %f\n", growth);

    slab = (float *)malloc(np1 * np2 * sizeof(float));

    // process slabs
    for (k = 0; k < np3; k++) {
        fread(&rec, sizeof(int), 1, fp1);
        fread(slab, sizeof(float), np1 * np2, fp1);
        fread(&rec, sizeof(int), 1, fp1);
        
        for (i = 0; i < np1 * np2; i++) {
            slab[i] *= vFact;
        }
        
        // GRAFIC requires only offsets in *pos files, but if the full coordinates needed:
        /*
        for (i = 0; i < np1; i++) {
            for (j = 0; j < np2; j++) {
                int idx = i * np2 + j;
                if (ax == 'x') slab[idx] = slab[idx] + dx * (i + 0.5);
                if (ax == 'y') slab[idx] = slab[idx] + dx * (j + 0.5);
                if (ax == 'z') slab[idx] = slab[idx] + dx * (k + 0.5);
            }
        }
        */
        
        fwrite(&rec, sizeof(int), 1, fp2);
        fwrite(slab, sizeof(float), np1 * np2, fp2);
        fwrite(&rec, sizeof(int), 1, fp2);
    }


    free(slab);
    fclose(fp1);
    fclose(fp2);

    return 0;
}

float func_dtdacube(float a) {
    return 1.0 / powf(omegam * (1.0 / a - 1.0) + omegav * (a * a - 1.0) + 1.0, 1.5);
}

float rombint(float (*f)(float), float a, float b, float tol) {
    const int MAXITER = 30;
    const int MAXJ = 5;
    float g[MAXJ + 1];
    float h, gmax, error, g0, fourj, g1;
    int nint, i, j, k, jmax;
    
    h = 0.5 * (b - a);
    gmax = h * (f(a) + f(b));
    g[0] = gmax;
    nint = 1;
    error = 1.0e20;
    i = 0;
    
    while (1) {
        i++;
        if (i > MAXITER || (i > 5 && fabs(error) < tol)) {
            break;
        }
        
        g0 = 0.0;
        for (k = 0; k < nint; k++) {
            g0 += f(a + (k + k + 1) * h);
        }
        g0 = 0.5 * g[0] + h * g0;
        h = 0.5 * h;
        nint += nint;
        jmax = (i < MAXJ) ? i : MAXJ;
        fourj = 1.0;
        
        for (j = 0; j < jmax; j++) {
            fourj = 4.0 * fourj;
            g1 = g0 + (g0 - g[j]) / (fourj - 1.0);
            g[j] = g0;
            g0 = g1;
        }
        
        if (fabs(g0) > tol) {
            error = 1.0 - gmax / g0;
        } else {
            error = gmax;
        }
        gmax = g0;
        g[jmax] = g0;
    }
    
    if (i > MAXITER && fabs(error) > tol) {
        printf("Rombint failed to converge; integral, error= %f, %f\n", g0, error);
    }
    
    return g0;
}
