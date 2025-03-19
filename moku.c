// gcc -c -g rsv/rsv.c
// gcc -g moku.c rsv.o -o moku.exe

#include "moku.h"

#include "food.c"

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

    struct Ingredient ingredient = ingredient_new(name);

    puts("Calories:");
    fscanf(stdin, "%f", &input_value);
    ingredient.calories = input_value;

    puts("Carbs (g):");
    fscanf(stdin, "%f", &input_value);
    ingredient.carbs = input_value;

    puts("Protein (g):");
    fscanf(stdin, "%f", &input_value);
    ingredient.protein = input_value;

    puts("Cost ($):");
    fscanf(stdin, "%f", &input_value);
    ingredient.price = input_value;

    return pantry_push(pantry, ingredient_to_food(ingredient));
}

int main()
{
    FILE *file = fopen("moku.rsv", "r");
    struct Pantry pantry = pantry_from_rsv(file);
    fclose(file);

    char buf[BUFSIZE + 1];

    while (1)
    {
        fgets(buf, BUFSIZE, stdin);
        if (strcmp(buf, "ingredient\n") == 0)
        {
            add_ingredient(&pantry);
        }
        else if (strcmp(buf, "help\n") == 0)
        {
            printf("Moku\ningredient - add an ingredient\nhelp - display this message\nquit - exit the program\nls - list ingredients and meals in pantry\n");
        }
        else if (strcmp(buf, "quit\n") == 0)
        {
            file = fopen("moku.rsv", "w");
            for (int i = 0; i < pantry.size; i++)
            {
                food_rsv_write(&pantry.items[i], file);
            }
            fclose(file);
            break;
        }
        else if (strcmp(buf, "ls\n") == 0)
        {
            print_pantry(&pantry);
        }
    }
}
