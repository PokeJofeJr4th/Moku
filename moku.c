// gcc -g moku.c -Wall -o moku.exe

#include "moku.h"

#include "food.c"
#include "persistence.c"

int add_ingredient(struct Pantry *pantry)
{
    float input_value;
    // get the name
    puts("Ingredient name:");
    char buf[BUFSIZE + 1];
    fgets(buf, BUFSIZE, stdin);
    // cut off at a newline
    for (char *i = buf; *i; i++)
    {
        if (*i == '\n')
        {
            *i = 0;
            break;
        }
    }
    char *name = malloc(strlen(buf) + 1);
    strcpy(name, buf);

    // get the unit
    puts("Unit of measure:");
    fgets(buf, BUFSIZE, stdin);
    // cut off at a newline
    for (char *i = buf; *i; i++)
    {
        if (*i == '\n')
        {
            *i = 0;
            break;
        }
    }
    char *unit = malloc(strlen(buf) + 1);
    strcpy(unit, buf);

    struct Ingredient ingredient = ingredient_new(name, unit);

    puts("Cost ($):");
    fscanf(stdin, "%f", &input_value);
    ingredient.nutrients.price = input_value;

    puts("Calories:");
    fscanf(stdin, "%f", &input_value);
    ingredient.nutrients.calories = input_value;

    puts("Carbs (g):");
    fscanf(stdin, "%f", &input_value);
    ingredient.nutrients.carbs = input_value;

    puts("Fat (g):");
    fscanf(stdin, "%g", &input_value);
    ingredient.nutrients.fat = input_value;

    puts("Protein (g):");
    fscanf(stdin, "%f", &input_value);
    ingredient.nutrients.protein = input_value;

    puts("Fiber (g):");
    fscanf(stdin, "%g", &input_value);
    ingredient.nutrients.fiber = input_value;

    return pantry_push(pantry, ingredient_to_food(ingredient));
}

int main()
{
    FILE *file = fopen("pantry.moku", "r");
    struct Pantry pantry;
    if (file == NULL)
    {
        pantry = pantry_new();
    }
    else
    {
        pantry = pantry_read(file);
        fclose(file);
    }

    char buf[BUFSIZE + 1];

    while (1)
    {
        fgets(buf, BUFSIZE, stdin);
        if (strcmp(buf, "ingredient\n") == 0 || strcmp(buf, "i\n") == 0)
        {
            add_ingredient(&pantry);
        }
        else if (strcmp(buf, "help\n") == 0)
        {
            printf("Moku\ningredient - add an ingredient\nhelp - display this message\nquit - exit the program\nls - list ingredients and meals in pantry\n");
        }
        else if (strcmp(buf, "quit\n") == 0 || strcmp(buf, "q\n") == 0)
        {
            printf("Save progress? (Y/n)");
            fgets(buf, BUFSIZE, stdin);
            if (strcmp(buf, "n\n") == 0)
            {
                return 0;
            }
            file = fopen("pantry.moku", "w");
            for (int i = 0; i < pantry.size; i++)
            {
                food_write(&pantry.items[i], file);
            }
            fclose(file);
            return 0;
        }
        else if (strcmp(buf, "ls\n") == 0)
        {
            print_pantry(&pantry);
        }
        else
        {
            printf("Invalid command; use `help` to show options\n");
        }
    }
}
