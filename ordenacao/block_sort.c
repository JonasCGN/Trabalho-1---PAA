#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct st_args
{
  char *path;
  int size_arr;
  int size_block;
  int show_values_sorted;
} args;


// Pode usar qualquer algoritmo de ordenacao
void sortBlock(int *block, int size)
{
  for (int i = 0; i < size - 1; i++)
    for (int j = 0; j < size - i - 1; j++)
      if (block[j] > block[j + 1])
      {
        int temp = block[j];
        block[j] = block[j + 1];
        block[j + 1] = temp;
      }
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
  {
    free(blocks[i]);
  }

  free(blocks);
  free(blockSizes);
  free(blockIndices);

  *sortedSize = size;
  return result;
}

void get_values_from_args(
    int argc, char *argv[], args *arguments)
{
  if (argc < 5)
  {
    printf("Usage: %s <path> <size_arr> <size_block> <show_values_sorted>\n", argv[0]);
    exit(1);
  }

  arguments->path = argv[1];
  arguments->size_arr = atoi(argv[2]);
  arguments->size_block = atoi(argv[3]);
  arguments->show_values_sorted = atoi(argv[4]);

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
  int sortedSize = 0;
  int *sortedArr = blockSort(arr, arguments.size_arr, arguments.size_block, &sortedSize);
  end = clock();

  if (arguments.show_values_sorted)
  {
    printf("Sorted array:\n");
    for (int i = 0; i < sortedSize; i++)
      printf("%d ", sortedArr[i]);
    printf("\n");
  }

  double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
  printf("%s %f\n", arguments.path, time_spent);
  
  free(arr);
  free(sortedArr);
  return 0;
}