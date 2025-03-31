#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct st_args
{
  char *path;
  int size_arr;
  int size_block;
  int show_values_sorted;
  int qtd_testes;
} args;

void sortBlock(int vetor[], int tamanho)
{
  int *b = (int *)calloc(tamanho, sizeof(int));
  int maior = vetor[0];
  int exp = 1;

  for (int i = 0; i < tamanho; i++)
  {
    if (vetor[i] > maior)
      maior = vetor[i];
  }

  while (maior / exp > 0)
  {
    int bucket[10] = {0};
    for (int i = 0; i < tamanho; i++)
      bucket[(vetor[i] / exp) % 10]++;
    for (int i = 1; i < 10; i++)
      bucket[i] += bucket[i - 1];
    for (int i = tamanho - 1; i >= 0; i--)
      b[--bucket[(vetor[i] / exp) % 10]] = vetor[i];
    for (int i = 0; i < tamanho; i++)
      vetor[i] = b[i];
    exp *= 10;
  }
  free(b);
}

int *blockSort(int *arr, int size, int blockSize, int *sortedSize)
{
  int numBlocks = (size + blockSize - 1) / blockSize;
  int **blocks = (int **)malloc(numBlocks * sizeof(int *));
  int *blockSizes = (int *)malloc(numBlocks * sizeof(int));
  int resultIndex = 0;

  for (int i = 0; i < size; i += blockSize)
  {
    int currentBlockSize = (i + blockSize <= size) ? blockSize : size - i;
    blocks[resultIndex] = (int *)malloc(currentBlockSize * sizeof(int));
    blockSizes[resultIndex] = currentBlockSize;

    for (int j = 0; j < currentBlockSize; j++)
      blocks[resultIndex][j] = arr[i + j];

    sortBlock(blocks[resultIndex], currentBlockSize);
    resultIndex++;
  }

  int *result = (int *)malloc(size * sizeof(int));
  int *blockIndices = (int *)calloc(numBlocks, sizeof(int));

  for (int i = 0; i < size; i++)
  {
    int minIdx = -1;
    int minValue = __INT_MAX__;

    for (int j = 0; j < numBlocks; j++)
      if (blockIndices[j] < blockSizes[j] && blocks[j][blockIndices[j]] < minValue)
      {
        minValue = blocks[j][blockIndices[j]];
        minIdx = j;
      }

    result[i] = minValue;
    blockIndices[minIdx]++;
  }

  for (int i = 0; i < numBlocks; i++)
    free(blocks[i]);
  free(blocks);
  free(blockSizes);
  free(blockIndices);

  *sortedSize = size;
  return result;
}

void get_values_from_args(int argc, char *argv[], args *arguments)
{
  if (argc < 6)
  {
    printf("Usage: %s <path> <size_arr> <size_block> <show_values_sorted> <qtd_testes>\n", argv[0]);
    exit(1);
  }

  arguments->path = argv[1];
  arguments->size_arr = atoi(argv[2]);
  arguments->size_block = atoi(argv[3]);
  arguments->show_values_sorted = atoi(argv[4]);
  arguments->qtd_testes = atoi(argv[5]);

  if (arguments->size_arr <= 0 || arguments->size_block <= 0)
  {
    printf("Invalid size of array or block\n");
    exit(1);
  }

  if (arguments->show_values_sorted != 0 && arguments->show_values_sorted != 1)
  {
    printf("Invalid value for show_values_sorted. Use 0 or 1.\n");
    exit(1);
  }

  if (arguments->size_block > arguments->size_arr)
  {
    printf("Block size cannot be greater than array size\n");
    exit(1);
  }

  if (arguments->qtd_testes <= 0)
  {
    printf("Invalid number of tests\n");
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

int validar_ordem(int *arr, int size)
{
  for (int i = 0; i < size - 1; i++)
    if (arr[i] > arr[i + 1])
      return 0;
  return 1;
}

int main(int argc, char *argv[])
{
  args arguments;
  int *arr = NULL;

  get_values_from_args(argc, argv, &arguments);
  get_numbers_from_file(arguments.path, arguments.size_arr, &arr);

  clock_t media = 0;
  for (int i = 0; i < arguments.qtd_testes; i++)
  {
    int *temp_arr = (int *)malloc(arguments.size_arr * sizeof(int));
    memcpy(temp_arr, arr, arguments.size_arr * sizeof(int));

    clock_t start = clock();
    int sortedSize = 0;
    int *sortedArr = blockSort(temp_arr, arguments.size_arr, arguments.size_block, &sortedSize);
    clock_t end = clock();

    media += (end - start);
    
    free(sortedArr);
    free(temp_arr);
  }

  media /= arguments.qtd_testes;
  double time_spent = (double)media / CLOCKS_PER_SEC;
  printf("%s %f\n", arguments.path, time_spent);

  free(arr);
  return 0;
}