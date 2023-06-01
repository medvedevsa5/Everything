#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define BYTE_COUNT 8
#define MAX_INPUT_LENGTH 256

#define VIRUS_COUNT 3

// структура, в которой хранится вся информация о вирусе.
typedef struct
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

    if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) // fgets, чтобы предотвратить buffer overflow.
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

    int readResult = readVirusDatabase(virusesFile, viruses); // ПРОВЕРИТЬ ОШИБКИ
    if (readResult == 1)
    {
        printf("Error first argument readVirusDatabase() is NULL.");
        return 4;
    }
    else if (readResult == 2)
    {
        printf("Error second argument readVirusDatabase() is NULL.");
        return 5;
    }
    else if (readResult == 3)
    {
        printf("Error reading virus name.");
        return 6;
    }
    else if (readResult == 4)
    {
        printf("Error reading virus properities.");
        return 7;
    }

    // закрытие файла
    if (fclose(virusesFile) != 0)
    {
        printf("Error closing viruses database");
        return 8;
    }

    if (printf("Enter the paths to the executable files one by one. Stop by entering Ctrl+Z.\n") < 0)
    {
        printf("Error writing to stdout");
        return 9;
    }

    if (fflush(stdin) != 0)
    {
        printf("Error flushing stdin");
        return 10;
    }

    while (fgets(input, MAX_INPUT_LENGTH, stdin) != NULL)
    {
        input[strcspn(input, "\n")] = '\0';

        FILE* executable;
        executable = fopen(input, "rb");
        if (executable == NULL)
        {
            printf("Error opening the executable.");
            return 11;
        }

        bool isFileExecutable = false;
        int isExecutableResult = isExecutable(executable, &isFileExecutable); // ПРОВЕРИТЬ ОШИБКИ
        if (isExecutableResult == 1)
        {
            printf("Error first argument isExecutable() is NULL.");
            return 12;
        }
        if (isExecutableResult == 2)
        {
            printf("Error second argument isExecutable() is NULL.");
            return 13;
        }
        if (isExecutableResult == 3)
        {
            printf("Error getting file pointer position in isExecutable().");
            return 14;
        }
        if (isExecutableResult == 4)
        {
            printf("Error moving pointer to beginning of file in isExecutable().");
            return 15;
        }
        if (isExecutableResult == 5)
        {
            printf("Error reading first two bytes in isExecutable().");
            return 16;
        }
        if (isExecutableResult == 6)
        {
            printf("Error returning file pointer to the previous position in isExecutable().");
            return 17;
        }
        if (isFileExecutable == false)
        {
            printf("The file is not executable.");
            return 18;
        }

        int virusCount = 0;
        for (size_t i = 0; i < VIRUS_COUNT; i++)
        {
            bool virused = false;
            int hasVirusResult = hasVirus(executable, viruses[i].offset, &viruses[i].signature, &virused); // ПРОВЕРИТЬ ОШИБКИ
            if (hasVirusResult == 1)
            {
                printf("Error first argument hasVirus() is NULL.");
                return 19;
            }
            if (hasVirusResult == 2)
            {
                printf("Error second argument hasVirus() is below zero.");
                return 20;
            }
            if (hasVirusResult == 3)
            {
                printf("Error third argument hasVirus() is NULL.");
                return 21;
            }
            if (hasVirusResult == 4)
            {
                printf("Error fourth argument hasVirus() is NULL.");
                return 22;
            }
            if (hasVirusResult == 5)
            {
                printf("Error seeking pointer to offset in hasVirus().");
                return 23;
            }
            if (hasVirusResult == 6)
            {
                printf("Error reading bytes from file in hasVirus().");
                return 24;
            }
            if (virused == true)
            {
                if (printf("File has %s virus!\n", viruses[i].name) < 0)
                {
                    printf("Error writing to stdout");
                    return 25;
                }
                ++virusCount;
            }
        }
        if (virusCount == 0)
        {
            if (printf("File has no viruses.\n") < 0)
            {
                printf("Error writing to stdout");
                return 26;
            }
        }

        if (fclose(executable) != 0)
        {
            printf("Error closing the executable");
            return 27;
        }
        if (fflush(stdin) != 0)
        {
            printf("Error flushing stdin");
            return 28;
        }
    }
    if (ferror(stdin) != 0) // ЗДЕСЬ ОСТОРОЖНО! Я не проверяю excecutable на NULL,
                       // потому что http://www.c-cpp.ru/content/fgets. Читай возвращаемое значение
    {
        printf("Error reading executables from stdin");
        return 29;
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
    if (filePosition == -1L)
    {
        return 3;
    }

    if (fseek(file, 0, SEEK_SET) != 0)
    {
        return 4;
    }

    unsigned char buffer[3];
    buffer[2] = '\0';

    if (fread(&buffer, 1, 2, file) != 2)
    {
        return 5;
    }

    if (fseek(file, filePosition, SEEK_SET) != 0)
    {
        return 6;
    }

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
        if (fgets(&virusArray[i].name, MAX_INPUT_LENGTH, file) == NULL)
        {
            return 3;
        }
        virusArray[i].name[strcspn(virusArray[i].name, "\n")] = '\0';
        int writtenCount = fscanf(
            file, "%x %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx\n",
            &virusArray[i].offset, &virusArray[i].signature[0], &virusArray[i].signature[1],
            &virusArray[i].signature[2], &virusArray[i].signature[3], &virusArray[i].signature[4],
            &virusArray[i].signature[5], &virusArray[i].signature[6], &virusArray[i].signature[7]
        );
        if (ferror(file) || (writtenCount != BYTE_COUNT + 1))
        {
            return 4;
        }
    }
    return 0;
}