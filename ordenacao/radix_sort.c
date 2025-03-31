#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct st_args
{
  char *path;
  int size_arr;
  int show_values_sorted;
} args;

void radixsort(int vetor[], int tamanho)
{
  int i;
  int *b;
  int maior = vetor[0];
  int exp = 1;

  b = (int *)calloc(tamanho, sizeof(int));

  for (i = 0; i < tamanho; i++)
  {
    if (vetor[i] > maior)
      maior = vetor[i];
  }

  while (maior / exp > 0)
  {
    int bucket[10] = {0};
    for (i = 0; i < tamanho; i++)
      bucket[(vetor[i] / exp) % 10]++;
    for (i = 1; i < 10; i++)
      bucket[i] += bucket[i - 1];
    for (i = tamanho - 1; i >= 0; i--)
      b[--bucket[(vetor[i] / exp) % 10]] = vetor[i];
    for (i = 0; i < tamanho; i++)
      vetor[i] = b[i];
    exp *= 10;
  }

  free(b);
}

void get_values_from_args(
    int argc, char *argv[], args *arguments)
{
  if (argc < 4)
  {
    printf("Usage: %s <path> <size_arr>  <show_values_sorted>\n", argv[0]);
    exit(1);
  }

  arguments->path = argv[1];
  arguments->size_arr = atoi(argv[2]);
  arguments->show_values_sorted = atoi(argv[3]);

  if (arguments->show_values_sorted != 0 && arguments->show_values_sorted != 1)
  {
    printf("Invalid value for show_values_sorted. Use 0 or 1.\n");
    exit(1);
  }

}

void get_numbers_from_file(char *path, int size, int **arr)
{
  FILE *file = fopen(path, "r");
  if (file == NULL)
  {
    printf("Error opening file: %s\n", path);
    exit(1);
  }

  *arr = (int *)malloc(size * sizeof(int));
  for (int i = 0; i < size; i++)
  {
    if (fscanf(file, "%d", &(*arr)[i]) != 1)
    {
      printf("Error reading number from file\n");
      free(*arr);
      fclose(file);
      exit(1);
    }
  }

  fclose(file);
}

int main(int argc, char *argv[])
{
  args arguments;
  int *arr = NULL;

  get_values_from_args(argc, argv, &arguments);
  get_numbers_from_file(arguments.path, arguments.size_arr, &arr);

  clock_t start, end;
  start = clock();
  radixsort(arr, arguments.size_arr);
  end = clock();

  if (arguments.show_values_sorted)
  {
    for (int i = 0; i < arguments.size_arr; i++)
      printf("%d ", arr[i]);
    printf("\n");
  }

  double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("%s %f\n", arguments.path, time_taken);
  
  free(arr);
  return 0;
}