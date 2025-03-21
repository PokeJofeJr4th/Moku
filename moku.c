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

void get_float(char *buf, float *out)
{
    get_line(buf);
    sscanf(buf, "%f", out);
}

int add_ingredient(struct Pantry *pantry)
{
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
    get_float(buf, &ingredient.nutrients.price);

    puts("Calories:");
    get_float(buf, &ingredient.nutrients.calories);

    puts("Carbs (g):");
    get_float(buf, &ingredient.nutrients.carbs);

    puts("Fat (g):");
    get_float(buf, &ingredient.nutrients.fat);

    puts("Protein (g):");
    get_float(buf, &ingredient.nutrients.protein);

    puts("Fiber (g):");
    get_float(buf, &ingredient.nutrients.fiber);

    return pantry_push(pantry, ingredient_to_food(ingredient));
}

void input_meal_ingredients(struct Pantry *pantry, struct Meal *meal)
{
    int input_id;
    float input_value;
    char buf[BUFSIZE];
    while (1)
    {
        puts("Ingredient name: (empty to finish)");
        get_line(buf);
        if (buf[0] == 0)
            break;
        union Food *ingredient = pantry_search(pantry, buf, &input_id);
        if (ingredient == NULL)
        {
            printf("Couldn't find ingredint \"%s\"\n", buf);
            continue;
        }

        printf("%s amount (%s): ", ingredient->header.name, ingredient->header.unit);
        get_float(buf, &input_value);
        meal_push(meal, input_id, input_value);
    }
}

int add_meal(struct Pantry *pantry)
{
    char buf[BUFSIZE];

    // get the name
    puts("Meal name:");
    get_line(buf);
    char *name = strdup(buf);

    // get the unit
    puts("Unit of measure:");
    get_line(buf);
    char *unit = strdup(buf);

    struct Meal meal = meal_new(name, unit);

    input_meal_ingredients(pantry, &meal);

    return pantry_push(pantry, meal_to_food(meal));
}

void edit_pantry(char *name, struct Pantry *pantry)
{
    char buf[BUFSIZE];
    float input_value;
    union Food *food = pantry_search(pantry, name, NULL);

    if (food == NULL)
    {
        printf("Couldn't find food \"%s\"\n", name);
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
            if (sscanf(buf, "%f", &input_value))
                food->ingredient.nutrients.price = input_value;
        }
        puts("Calories: (empty to cancel)");
        get_line(buf);
        if (buf[0] != 0)
        {
            if (sscanf(buf, "%f", &input_value))
                food->ingredient.nutrients.calories = input_value;
        }
        puts("Carbs: (empty to cancel)");
        get_line(buf);
        if (buf[0] != 0)
        {
            if (sscanf(buf, "%f", &input_value))
                food->ingredient.nutrients.carbs = input_value;
        }
        puts("Fat: (empty to cancel)");
        get_line(buf);
        if (buf[0] != 0)
        {
            if (sscanf(buf, "%f", &input_value))
                food->ingredient.nutrients.fat = input_value;
        }
        puts("Protein: (empty to cancel)");
        get_line(buf);
        if (buf[0] != 0)
        {
            if (sscanf(buf, "%f", &input_value))
                food->ingredient.nutrients.protein = input_value;
        }
        puts("Fiber: (empty to cancel)");
        get_line(buf);
        if (buf[0] != 0)
        {
            if (sscanf(buf, "%f", &input_value))
                food->ingredient.nutrients.fiber = input_value;
        }
        break;
    case FT_Meal:
        for (int i = 0; i < food->meal.ingredients_count; i++)
        {
            union Food *current_ingredient = pantry_get(pantry, food->meal.ingredients[i].food_id);
            printf("%s quantity (%s): (empty to cancel)\n", current_ingredient->header.name, current_ingredient->header.unit);
            get_line(buf);
            if (buf[0] != 0)
            {
                if (sscanf(buf, "%f", &input_value))
                    food->meal.ingredients[i].amount = input_value;
            }
        }
        input_meal_ingredients(pantry, &food->meal);
        break;
    }
}

void remove_food(char *food_name, struct Pantry *pantry)
{
    char buf[BUFSIZE];
    int food_id;
    union Food *food = pantry_search(pantry, food_name, &food_id);
    if (food == NULL)
    {
        printf("Couldn't find food \"%s\"\n", food_name);
        return;
    }
    int num_refs = 0;
    for (int i = 0; i < pantry->size; i++)
    {
        union Food *current = pantry_get(pantry, i);
        if (current->header.type != FT_Meal)
            continue;
        for (int j = 0; j < current->meal.ingredients_count; j++)
        {
            if (current->meal.ingredients[j].food_id == food_id)
                num_refs++;
        }
    }
    printf("Food is used by %i meal(s). Confirm deletion? (y/N) ", num_refs);
    get_line(buf);
    if (strcmp(buf, "y") != 0)
    {
        puts("Canceled");
        return;
    }
    for (int i = 0; i < pantry->size; i++)
    {
        union Food *current = pantry_get(pantry, i);
        if (current->header.type != FT_Meal)
            continue;
        struct Meal *meal = &current->meal;
        for (int j = 0; j < meal->ingredients_count; j++)
        {
            if (meal->ingredients[j].food_id == food_id)
            {
                for (int k = j + 1; k < meal->ingredients_count; k++)
                {
                    meal->ingredients[k - 1] = meal->ingredients[k];
                }
                meal->ingredients_count--;
            }
            else if (meal->ingredients[j].food_id > food_id)
            {
                meal->ingredients[j].food_id--;
            }
        }
    }
    for (int i = food_id + 1; i < pantry->size; i++)
    {
        pantry->items[i - 1] = pantry->items[i];
    }
    pantry->size--;
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
        get_line(buf);
        if (strcmp(buf, "ingredient") == 0 || strcmp(buf, "i") == 0)
        {
            add_ingredient(&pantry);
        }
        else if (strcmp(buf, "meal") == 0 || strcmp(buf, "m") == 0)
        {
            add_meal(&pantry);
        }
        else if (strcmp(buf, "help") == 0)
        {
            printf("Moku\ningredient - add an ingredient\nhelp - display this message\nquit - exit the program\nls - list ingredients and meals in pantry\n");
        }
        else if (strcmp(buf, "quit") == 0 || strcmp(buf, "q") == 0)
        {
            printf("Save changes? (Y/n) ");
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
        else if (strncmp(buf, "rm ", 3) == 0)
        {
            remove_food(buf + 3, &pantry);
        }
        else if (strncmp(buf, "remove ", 7) == 0)
        {
            remove_food(buf + 7, &pantry);
        }
        else
        {
            printf("Invalid command; use `help` to show options\n");
        }
    }
}
