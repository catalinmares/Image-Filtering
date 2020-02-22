#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MASTER 0

void usage(int argc, char* argv[]) 
{    
    printf("Usage: ./%s <image_in> <image_out> <filter1> <filter2> ... <filterX>\n", argv[0]);
}

float** getFilterMatrix(char* filter)
{
    float** filterMatrix = (float**) calloc(3, sizeof(float*));

    for (int i = 0; i < 3; i++) {
        filterMatrix[i] = (float*) calloc(3, sizeof(float));
    }

    if (strcmp(filter, "smooth") == 0) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                filterMatrix[i][j] = (float) 1 / 9;
            }
        }
    } else if (strcmp(filter, "blur") == 0) {
        filterMatrix[0][0] = (float) 1 / 16;
        filterMatrix[0][1] = (float) 2 / 16;
        filterMatrix[0][2] = (float) 1 / 16;
        filterMatrix[1][0] = (float) 2 / 16;
        filterMatrix[1][1] = (float) 4 / 16;
        filterMatrix[1][2] = (float) 2 / 16;
        filterMatrix[2][0] = (float) 1 / 16;
        filterMatrix[2][1] = (float) 2 / 16;
        filterMatrix[2][2] = (float) 1 / 16;
    } else if (strcmp(filter, "sharpen") == 0) {
        filterMatrix[0][0] = 0; 
        filterMatrix[0][1] = (float) -2 / 3;
        filterMatrix[0][2] = 0;
        filterMatrix[1][0] = (float) -2 / 3;
        filterMatrix[1][1] = (float) 11 / 3;
        filterMatrix[1][2] = (float) -2 / 3;
        filterMatrix[2][0] = 0;
        filterMatrix[2][1] = (float) -2 / 3;
        filterMatrix[2][2] = 0;
    } else if (strcmp(filter, "mean") == 0) {
        filterMatrix[0][0] = -1;
        filterMatrix[0][1] = -1;
        filterMatrix[0][2] = -1;
        filterMatrix[1][0] = -1;
        filterMatrix[1][1] = 9;
        filterMatrix[1][2] = -1;
        filterMatrix[2][0] = -1;
        filterMatrix[2][1] = -1;
        filterMatrix[2][2] = -1;
    } else if (strcmp(filter, "emboss") == 0) {
        filterMatrix[0][0] = 0;
        filterMatrix[0][1] = 1;
        filterMatrix[0][2] = 0;
        filterMatrix[1][0] = 0;
        filterMatrix[1][1] = 0;
        filterMatrix[1][2] = 0;
        filterMatrix[2][0] = 0;
        filterMatrix[2][1] = -1;
        filterMatrix[2][2] = 0;
    }

    return filterMatrix;
}

unsigned char** applyBWFilter(unsigned char** image, int start, int stop, int width, char* filter)
{
    unsigned char** filteredImage = (unsigned char**) calloc(stop - start + 3, sizeof(unsigned char*));

    for (int i = 0; i < stop - start + 3; i++) {
        filteredImage[i] = (unsigned char*) calloc(width + 2, sizeof(unsigned char));
    }

    float** filterMatrix = getFilterMatrix(filter);

    for (int i = start; i <= stop; i++) {
        for (int j = 1; j <= width; j++) {
            int pixelValue = filterMatrix[2][2] * image[i - start][j - 1] +
                            filterMatrix[2][1] * image[i - start][j] +
                            filterMatrix[2][0] * image[i - start][j + 1] +
                            filterMatrix[1][2] * image[i - start + 1][j - 1] +
                            filterMatrix[1][1] * image[i - start + 1][j] +
                            filterMatrix[1][0] * image[i - start + 1][j + 1] +
                            filterMatrix[0][2] * image[i - start + 2][j - 1] +
                            filterMatrix[0][1] * image[i - start + 2][j] +
                            filterMatrix[0][0] * image[i - start + 2][j + 1];

            if (pixelValue < 0) {
                pixelValue = 0;
            } else if (pixelValue > 255) {
                pixelValue = 255;
            }

            filteredImage[i - start + 1][j] = (unsigned char) pixelValue;
        }
    }

    for (int i = 0; i < stop - start + 3; i++) {
        free(image[i]);
    }

    free(image);

    for (int i = 0; i < 3; i++) {
        free(filterMatrix[i]);
    }

    free(filterMatrix);

    return filteredImage;
}

unsigned char*** applyColoredFilter(unsigned char*** image, int start, int stop, int width, char* filter)
{
    unsigned char*** filteredImage = (unsigned char***) calloc(stop - start + 3, sizeof(unsigned char**));

    for (int i = 0; i < stop - start + 3; i++) {
        filteredImage[i] = (unsigned char**) calloc(width + 2, sizeof(unsigned char*));

        for (int j = 0; j < width + 2; j++) {
            filteredImage[i][j] = (unsigned char*) calloc(3, sizeof(unsigned char));
        }
    }

    float** filterMatrix = getFilterMatrix(filter);

    for (int i = start; i <= stop; i++) {
        for (int j = 1; j <= width; j++) {
            for (int k = 0; k < 3; k++) {
                int pixelValue = filterMatrix[2][2] * image[i - start][j - 1][k] +
                                filterMatrix[2][1] * image[i - start][j][k] +
                                filterMatrix[2][0] * image[i - start][j + 1][k] +
                                filterMatrix[1][2] * image[i - start + 1][j - 1][k] +
                                filterMatrix[1][1] * image[i - start + 1][j][k] +
                                filterMatrix[1][0] * image[i - start + 1][j + 1][k] +
                                filterMatrix[0][2] * image[i - start + 2][j - 1][k] +
                                filterMatrix[0][1] * image[i - start + 2][j][k] +
                                filterMatrix[0][0] * image[i - start + 2][j + 1][k];

                if (pixelValue < 0) {
                    pixelValue = 0;
                } else if (pixelValue > 255) {
                    pixelValue = 255;
                }

                filteredImage[i - start + 1][j][k] = (unsigned char) pixelValue;
            }
        } 
    }

    for (int i = 0; i < stop - start + 3; i++) {
        for (int j = 0; j < width + 2; j++) {
            free(image[i][j]);
        }

        free(image[i]);
    }

    free(image);

    for (int i = 0; i < 3; i++) {
        free(filterMatrix[i]);
    }

    free(filterMatrix);

    return filteredImage;
}

void main(int argc, char* argv[])
{
    int nProcesses, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    if (argc < 4) {
        usage(argc, argv);
        return;
    }

    int P;
    int width, height;
    int maxValue;

    int filterCount;
    char** filters;

    int start, stop;

    unsigned char** bwImage;
    unsigned char*** coloredImage;

    unsigned char** partialBWImage;
    unsigned char*** partialColoredImage;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(hostname, &len);
    
    // CITIREA IMAGINII DE CATRE MASTER
    if (rank == MASTER) {
        FILE* imageIn = fopen(argv[1], "r");

        if (imageIn == NULL) {
            printf("There was an error opening the given image.\n");

            MPI_Finalize();
            return;
        }

        fscanf(imageIn, "P%d\n", &P);

        fscanf(imageIn, "# Created by GIMP version 2.10.14 PNM plug-in\n");

        fscanf(imageIn, "%d %d\n", &width, &height);

        fscanf(imageIn, "%d\n", &maxValue);

        if (P == 5) {
            bwImage = (unsigned char**) calloc(height + 2, sizeof(unsigned char*));

            for (int i = 0; i < height + 2; i++) {
                bwImage[i] = (unsigned char*) calloc(width + 2, sizeof(unsigned char));
            }

            for (int i = 1; i <= height; i++) {
                for (int j = 1; j <= width; j++) {
                    fscanf(imageIn, "%c", &bwImage[i][j]);
                }
            }
        } else {
            coloredImage = (unsigned char***) calloc(height + 2, sizeof(unsigned char**));

            for (int i = 0; i < height + 2; i++) {
                coloredImage[i] = (unsigned char**) calloc(width + 2, sizeof(unsigned char*));

                for (int j = 0; j < width + 2; j++) {
                    coloredImage[i][j] = (unsigned char*) calloc(3, sizeof(unsigned char));
                }
            }

            for (int i = 1; i <= height; i++) {
                for (int j = 1; j <= width; j++) {
                    for (int k = 0; k < 3; k++) {
                        fscanf(imageIn, "%c", &coloredImage[i][j][k]);
                    }
                }
            }
        }

        fclose(imageIn);

        filterCount = argc - 3;
    }

    // TRIMITEREA PARAMETRILOR IMAGINII CATRE CELELALTE PROCESE
    MPI_Bcast(&P, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&width, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&maxValue, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&filterCount, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    start = ceil(rank * height / nProcesses) + 1;
    stop = fmin(ceil((rank + 1) * height / nProcesses), height);

    if (stop == height - 1) {
        stop = height;
    }

    if (P == 5) {
        partialBWImage = (unsigned char**) calloc(stop - start + 3, sizeof(unsigned char*));

        for (int i = 0; i < stop - start + 3; i++) {
            partialBWImage[i] = (unsigned char*) calloc(width + 2, sizeof(unsigned char));

            if (rank == MASTER) {
                for (int j = 0; j < width + 2; j++) {
                    partialBWImage[i][j] = bwImage[i][j];
                }
            }
        }
    } else {
        partialColoredImage = (unsigned char***) calloc(stop - start + 3, sizeof(unsigned char**));

        for (int i = 0; i < stop - start + 3; i++) {
            partialColoredImage[i] = (unsigned char**) calloc(width + 2, sizeof(unsigned char*));

            for (int j = 0; j < width + 2; j++) {
                partialColoredImage[i][j] = (unsigned char*) calloc(3, sizeof(unsigned char));

                if (rank == MASTER) {
                    for (int k = 0; k < 3; k++) {
                        partialColoredImage[i][j][k] = coloredImage[i][j][k];
                    }
                }
            }
        }
    }

    filters = (char**) malloc(filterCount * sizeof(char*));

    // TRIMITEREA FILTRELOR DE LA MASTER LA CELELALTE PROCESE
    if (rank == MASTER) {
        for (int i = 0; i < filterCount; i++) {
            filters[i] = (char*) malloc((strlen(argv[3 + i]) + 1) * sizeof(char));
            strcpy(filters[i], argv[3 + i]);
        }

        for (int i = 1; i < nProcesses; i++) {
            for (int j = 0; j < filterCount; j++) {
                int filterLen = strlen(filters[j]) + 1;
                MPI_Send(&filterLen, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(filters[j], filterLen, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            }
        }
    } else {
        for (int j = 0; j < filterCount; j++) {
            int filterLen;

            MPI_Recv(&filterLen, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            filters[j] = (char*) malloc(filterLen * sizeof(char));

            MPI_Recv(filters[j], filterLen, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // Distribuie imaginea intre procese
    if (rank == MASTER) {
        for (int i = 1; i < nProcesses; i++) {
            int startP = ceil(i * height / nProcesses) + 1;
            int stopP = fmin(ceil((i + 1) * height / nProcesses), height);

            if (stopP == height - 1) {
                stopP = height;
            }

            if (P == 5) {
                for (int j = startP; j <= stopP; j++) {
                    MPI_Send(bwImage[j], width + 2, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
                }
            } else {
                for (int j = startP; j <= stopP; j++) {
                    for (int k = 0; k < width + 2; k++) {
                        MPI_Send(coloredImage[j][k], 3, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
                    }
                }
            }
        }
    } else {
        if (P == 5) {
            for (int j = start; j <= stop; j++) {
                MPI_Recv(partialBWImage[j - start + 1], width + 2, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        } else {
            for (int j = start; j <= stop; j++) {
                for (int k = 0; k < width + 2; k++) {
                    MPI_Recv(partialColoredImage[j - start + 1][k], 3, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
            }
        }
    }

    // APLICAREA SECVENTIALA A FILTRELOR
    for (int n = 0; n < filterCount; n++) {
        // 1. Trimite marginile fiecarui proces
        if (rank == MASTER) {
            // MASTER-ul isi actualizeaza marginile propriei portiuni de imagine
            if (P == 5) {
                for (int i = 0; i < width + 2; i++) {
                    partialBWImage[0][i] = bwImage[start - 1][i];
                    partialBWImage[stop - start + 2][i] = bwImage[stop + 1][i];
                }
            } else {
                for (int i = 0; i < width + 2; i++) {
                    for (int k = 0; k < 3; k++) {
                        partialColoredImage[0][i][k] = coloredImage[start - 1][i][k];
                        partialColoredImage[stop - start + 2][i][k] = coloredImage[stop + 1][i][k];
                    }
                }
            }

            // MASTER-ul trimite celorlalte procese marginile actualizate pentru portiunile lor de imagine
            for (int i = 1; i < nProcesses; i++) {
                int startP = ceil(i * height / nProcesses) + 1;
                int stopP = fmin(ceil((i + 1) * height / nProcesses), height);

                if (stopP == height - 1) {
                    stopP = height;
                }

                if (P == 5) {
                    MPI_Send(bwImage[startP - 1], width + 2, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
                    MPI_Send(bwImage[stopP + 1], width + 2, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
                } else {
                    for (int k = 0; k < width + 2; k++) {
                        MPI_Send(coloredImage[startP - 1][k], 3, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
                    }

                    for (int k = 0; k < width + 2; k++) {
                        MPI_Send(coloredImage[stopP + 1][k], 3, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
                    }
                }
            } 
        } else {
            if (P == 5) {
                MPI_Recv(partialBWImage[0], width + 2, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(partialBWImage[stop - start + 2], width + 2, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else {
                for (int k = 0; k < width + 2; k++) {
                    MPI_Recv(partialColoredImage[0][k], 3, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }

                for (int k = 0; k < width + 2; k++) {
                    MPI_Recv(partialColoredImage[stop - start + 2][k], 3, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
            }
        }

        // 2. Aplica filtrul
        if (P == 5) {
            partialBWImage = applyBWFilter(partialBWImage, start, stop, width, filters[n]);
        } else {
            partialColoredImage = applyColoredFilter(partialColoredImage, start, stop, width, filters[n]);
        }

        // 3. Actualizare margini intre procese

        // Daca e ultimul filtru se va trimite direct
        // toata bucata de imagine, nu doar marginile
        if (n == filterCount - 1) {
        	break;
        }

        // MASTER PRIMESTE MARGINILE BUCATILOR DE IMAGINE DE LA PROCESE
        if (rank == MASTER) {
            for (int j = 1; j <= width; j++) {
                if (P == 5) {
                    bwImage[start][j] = partialBWImage[1][j];
                    bwImage[stop][j] = partialBWImage[stop - start + 1][j];
                } else {
                    for (int k = 0; k < 3; k++) {
                        coloredImage[start][j][k] = partialColoredImage[1][j][k];
                        coloredImage[stop][j][k] = partialColoredImage[stop - start + 1][j][k];
                    }
                }
            }

            for (int i = 1; i < nProcesses; i++) {
                int startP = ceil(i * height / nProcesses) + 1;
                int stopP = fmin(ceil((i + 1) * height / nProcesses), height);

                if (stopP == height - 1) {
                    stopP = height;
                }

                if (P == 5) {
                    MPI_Recv(bwImage[startP], width + 2, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(bwImage[stopP], width + 2, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                } else {
                    for (int k = 0; k < width + 2; k++) {
                        MPI_Recv(coloredImage[startP][k], 3, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        MPI_Recv(coloredImage[stopP][k], 3, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    }
                }
            }

        // PROCESELE TRIMIT MARGINILE BUCATILOR DE IMAGINE CATRE MASTER
        } else {
            if (P == 5) {
                MPI_Send(partialBWImage[1], width + 2, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD);
                MPI_Send(partialBWImage[stop - start + 1], width + 2, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD);
            } else {
                for (int k = 0; k < width + 2; k++) {
                    MPI_Send(partialColoredImage[1][k], 3, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD);
                    MPI_Send(partialColoredImage[stop - start + 1][k], 3, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD);
                }
            }
        }
    }

    // MASTER PRIMESTE BUCATILE DE IMAGINE DE LA PROCESE
    if (rank == MASTER) {
    	for (int i = start; i <= stop; i++) {
            for (int j = 1; j <= width; j++) {
                if (P == 5) {
                    bwImage[i][j] = partialBWImage[i - start + 1][j];
                } else {
                    for (int k = 0; k < 3; k++) {
                        coloredImage[i][j][k] = partialColoredImage[i - start + 1][j][k];
                    }
                }
            }
        }

        for (int i = 1; i < nProcesses; i++) {
            int startP = ceil(i * height / nProcesses) + 1;
            int stopP = fmin(ceil((i + 1) * height / nProcesses), height);

            if (stopP == height - 1) {
                stopP = height;
            }

            if (P == 5) {
                for (int j = startP; j <= stopP; j++) {
                    MPI_Recv(bwImage[j], width + 2, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
            } else {
                for (int j = startP; j <= stopP; j++) {
                    for (int k = 0; k < width + 2; k++) {
                        MPI_Recv(coloredImage[j][k], 3, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    }
                }
            }
        }

    // PROCESELE TRIMIT BUCATILE DE IMAGINE CATRE MASTER
    } else {
        if (P == 5) {
            for (int j = start; j <= stop; j++) {
                MPI_Send(partialBWImage[j - start + 1], width + 2, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD);
            }
        } else {
            for (int j = start; j <= stop; j++) {
                for (int k = 0; k < width + 2; k++) {
                    MPI_Send(partialColoredImage[j - start + 1][k], 3, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD);
                }
            }
        }
    }

    // ELIBERAREA MEMORIEI
    if (P == 5) {
        for (int i = 0; i < stop - start + 3; i++) {
            free(partialBWImage[i]);
        }

        free(partialBWImage);
    } else {
        for (int i = 0; i < stop - start + 3; i++) {
            for (int j = 0; j < width + 2; j++) {
                free(partialColoredImage[i][j]);
            }

            free(partialColoredImage[i]);
        }

        free(partialColoredImage);
    }

    for (int i = 0; i < filterCount; i++) {
        free(filters[i]);
    }

    free(filters);

    // SCRIEREA IMAGINII FINALE IN FISIER
    if (rank == MASTER) {
        FILE* imageOut = fopen(argv[2], "w");

        fprintf(imageOut, "P%d\n", P);
        fprintf(imageOut, "%d %d\n", width, height);
        fprintf(imageOut, "%d\n", maxValue);

        if (P == 5) {
            for (int i = 1; i <= height; i++) {
                for (int j = 1; j <= width; j++) {
                    fprintf(imageOut, "%c", bwImage[i][j]);
                }
            }

            for (int i = 0; i < height + 2; i++) {
                free(bwImage[i]);
            }

            free(bwImage);
        } else {
            for (int i = 1; i <= height; i++) {
                for (int j = 1; j <= width; j++) {
                    for (int k = 0; k < 3; k++) {
                        fprintf(imageOut, "%c", coloredImage[i][j][k]);
                    }
                }
            }

            for (int i = 0; i < height + 2; i++) {
                for (int j = 0; j < width + 2; j++) {
                    free(coloredImage[i][j]);
                }

                free(coloredImage[i]);
            }

            free(coloredImage);
        }

        fclose(imageOut);
    }
    
    MPI_Finalize();

}