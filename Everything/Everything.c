#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define BYTE_COUNT 8
#define MAX_INPUT_LENGTH 256

#define VIRUS_COUNT 3

// структура, в которой хранится вся информация о вирусе.
typedef struct _VirusData
{
    char name[MAX_INPUT_LENGTH + 1]; // +1 байт для символа завершения строки \0
    unsigned char signature[BYTE_COUNT + 1];
    int offset;
} VirusData;

int readVirusDatabase(FILE* file, VirusData* virusArray);
int isExecutable(FILE* file, bool* result);
int hasVirus(FILE* file, int offset, unsigned char* signature, bool* result);

int main(void)
{
    char input[MAX_INPUT_LENGTH + 1];
    input[MAX_INPUT_LENGTH] = '\0';

    // считывание базы из файла
    FILE* virusesFile;
    if (printf("Enter a database path.\n") < 0)
    {
        printf("Error writing to stdout.");
        return 1;
    }

    if (fgets(&input, MAX_INPUT_LENGTH, stdin) == NULL) // fgets, чтобы предотвратить buffer overflow.
    {
        printf("Error reading from stdin.");
        return 2;
    }
    input[strcspn(input, "\n")] = 0;

    virusesFile = fopen(input, "r");
    if (virusesFile == NULL)
    {
        printf("Error opening the database.");
        return 3;
    }

    VirusData viruses[VIRUS_COUNT];
    for (size_t i = 0; i < VIRUS_COUNT; i++)
    {
        viruses[i].name[MAX_INPUT_LENGTH] = '\0';
        viruses[i].signature[BYTE_COUNT] = '\0';
        viruses[i].offset = 0;
    }

    int readResult = readVirusDatabase(virusesFile, &viruses); // ОБРАБОТАТЬ ОШИБКИ
    if (readResult != 0)
    {
        printf("Error reading the database");
        return 4;
    }

    // закрытие файла
    if (fclose(virusesFile) != 0)
    {
        printf("Error closing viruses database");
        return 5;
    };

    if (printf("Enter the paths to the executable files one by one. Stop by entering Ctrl+Z.\n") < 0)
    {
        printf("Error writing to stdout");
        return 6;
    }

    if (fflush(stdin) != 0)
    {
        printf("Error flushing stdin");
        return 12;
    };

    while (fgets(&input, MAX_INPUT_LENGTH, stdin) != NULL)
    {
        input[strcspn(input, "\n")] = 0;

        FILE* executable;
        executable = fopen(input, "rb");
        if (executable == NULL)
        {
            printf("Error opening the executable.");
            return 7;
        }

        bool isFileExecutable = false;
        int isExecutableResult = isExecutable(executable, &isFileExecutable); // ОБРАБОТАТЬ ОШИБКИ
        if (isFileExecutable == false)
        {
            printf("The file is not executable.");
            return 8;
        }

        int virusCount = 0;
        for (size_t i = 0; i < VIRUS_COUNT; i++)
        {
            bool virused = false;
            int hasVirusResult = hasVirus(executable, viruses[i].offset, &viruses[i].signature, &virused); // ОБРАБОТАТЬ ОШИБКИ
            if (virused == true)
            {
                if (printf("File has %s virus!\n", viruses[i].name) < 0)
                {
                    printf("Error writing to stdout");
                    return 9;
                }
                ++virusCount;
            }
        }
        if (virusCount == 0)
        {
            if (printf("File has no viruses.\n") < 0)
            {
                printf("Error writing to stdout");
                return 10;
            }
        }

        if (fclose(executable) != 0)
        {
            printf("Error closing the executable");
            return 11;
        };
        if (fflush(stdin) != 0)
        {
            printf("Error flushing stdin");
            return 12;
        };
    }
    if (ferror(stdin)) // ЗДЕСЬ ОСТОРОЖНО! Я не проверяю excecutable на NULL,
                       // потому что http://www.c-cpp.ru/content/fgets. Читай возвращаемое значение
    {
        printf("Error reading executables from stdin");
        return 11;
    }

    return 0;
}

int hasVirus(FILE* file, int offset, unsigned char* signature, bool* result)
{
    if (file == NULL)
    {
        return 1;
    }
    if (offset < 0)
    {
        return 2;
    }
    if (signature == NULL)
    {
        return 3;
    }
    if (result == NULL) 
    {
        return 4;
    }

    unsigned char buffer = 0;

    if (fseek(file, offset, SEEK_SET) != 0)
    {
        return 5;
    }

    for (int i = 0; i < BYTE_COUNT; i++)
    {
        buffer = fgetc(file);
        if (buffer == EOF)
        {
            return 6;
        }
        if (buffer != signature[i])
        {
            *result = false;
            return 0; 
        }
    }

    *result = true;
    return 0;
}

int isExecutable(FILE* file, bool* result)
{
    if (file == NULL)
    {
        return 1;
    }
    if (result == NULL)
    {
        return 2;
    }

    int filePosition = ftell(file);
    if (filePosition == 1L)
    {
        return 3;
    }

    unsigned char buffer[3];
    buffer[2] = '\0';

    if (fread(&buffer, 1, 2, file) != 2)
    {
        return 4;
    }

    if (fseek(file, filePosition, SEEK_SET) != 0)
    {
        return 5;
    };

    bool isEqual = (strcmp(buffer, "MZ") == 0);

    if (isEqual == true)
    {
        *result = true;
    }
    else
    {
        *result = false;
    }

    return 0;

}

int readVirusDatabase(FILE* file, VirusData* virusArray)
{
    if (file == NULL)
    {
        return 1;
    }
    if (virusArray == NULL)
    {
        return 2;
    }
    for (size_t i = 0; i < VIRUS_COUNT; i++)
    {
        int writtenCount = fscanf(
            file, "%s %x %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx",
            &virusArray[i].name, &virusArray[i].offset, &virusArray[i].signature[0], &virusArray[i].signature[1],
            &virusArray[i].signature[2], &virusArray[i].signature[3], &virusArray[i].signature[4],
            &virusArray[i].signature[5], &virusArray[i].signature[6], &virusArray[i].signature[7]
        );
        if (ferror(file) || (writtenCount != BYTE_COUNT + 2))
        {
            return 3;
        }
    }
    return 0;
}