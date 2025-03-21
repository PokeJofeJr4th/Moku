// gcc -g moku.c rsv.o -Wall -o moku.exe

#include "moku.h"

#include "food.c"
#include "persistence.c"

void get_line(char *buf)
{
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
}

int add_ingredient(struct Pantry *pantry)
{
    float input_value;
    char buf[BUFSIZE];

    // get the name
    puts("Ingredient name:");
    get_line(buf);
    char *name = strdup(buf);

    // get the unit
    puts("Unit of measure:");
    get_line(buf);
    char *unit = strdup(buf);

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
    fscanf(stdin, "%f", &input_value);
    ingredient.nutrients.fiber = input_value;

    return pantry_push(pantry, ingredient_to_food(ingredient));
}

void edit_pantry(char *name, struct Pantry *pantry)
{
    char buf[BUFSIZE];
    float input_value;
    union Food *food = pantry_search(pantry, name);

    if (food == NULL)
    {
        printf("Couldn't find food \"%s\"", name);
        return;
    }

    puts("Ingredient name: (empty to cancel)");
    get_line(buf);
    if (buf[0] != 0)
    {
        free(food->header.name);
        food->header.name = strdup(buf);
    }

    puts("Ingredient unit: (empty to cancel)");
    get_line(buf);
    if (buf[0] != 0)
    {
        free(food->header.unit);
        food->header.unit = strdup(buf);
    }

    switch (food->header.type)
    {
    case FT_Ingredient:
        puts("Price: (empty to cancel)");
        get_line(buf);
        if (buf[0] != 0)
        {
            sscanf(buf, "%f", &input_value);
            food->ingredient.nutrients.price = input_value;
        }
        puts("Calories: (empty to cancel)");
        get_line(buf);
        if (buf[0] != 0)
        {
            sscanf(buf, "%f", &input_value);
            food->ingredient.nutrients.calories = input_value;
        }
        puts("Carbs: (empty to cancel)");
        get_line(buf);
        if (buf[0] != 0)
        {
            sscanf(buf, "%f", &input_value);
            food->ingredient.nutrients.carbs = input_value;
        }
        puts("Fat: (empty to cancel)");
        get_line(buf);
        if (buf[0] != 0)
        {
            sscanf(buf, "%f", &input_value);
            food->ingredient.nutrients.fat = input_value;
        }
        puts("Protein: (empty to cancel)");
        get_line(buf);
        if (buf[0] != 0)
        {
            sscanf(buf, "%f", &input_value);
            food->ingredient.nutrients.protein = input_value;
        }
        puts("Fiber: (empty to cancel)");
        get_line(buf);
        if (buf[0] != 0)
        {
            sscanf(buf, "%f", &input_value);
            food->ingredient.nutrients.fiber = input_value;
        }
        break;
    case FT_Meal:
        break;
    }
}

int main()
{
    FILE *file = fopen("moku.rsv", "r");
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

    char buf[BUFSIZE];

    while (1)
    {
        fgets(buf, BUFSIZE, stdin);
        for (int i = 0; i < BUFSIZE; i++)
        {
            if (buf[i] == '\n')
            {
                buf[i] = 0;
                break;
            }
        }
        if (strcmp(buf, "ingredient") == 0 || strcmp(buf, "i") == 0)
        {
            add_ingredient(&pantry);
        }
        else if (strcmp(buf, "help") == 0)
        {
            printf("Moku\ningredient - add an ingredient\nhelp - display this message\nquit - exit the program\nls - list ingredients and meals in pantry\n");
        }
        else if (strcmp(buf, "quit") == 0 || strcmp(buf, "q") == 0)
        {
            printf("Save progress? (Y/n)");
            fgets(buf, BUFSIZE, stdin);
            if (strcmp(buf, "n\n") == 0)
            {
                return 0;
            }
            file = fopen("moku.rsv", "w");
            for (int i = 0; i < pantry.size; i++)
            {
                food_write(&pantry.items[i], file);
            }
            fclose(file);
            return 0;
        }
        else if (strcmp(buf, "ls") == 0)
        {
            print_pantry(&pantry);
        }
        else if (strncmp(buf, "e ", 2) == 0)
        {
            edit_pantry(buf + 2, &pantry);
        }
        else if (strncmp(buf, "edit ", 5) == 0)
        {
            edit_pantry(buf + 5, &pantry);
        }
        else
        {
            printf("Invalid command; use `help` to show options\n");
        }
    }
}
