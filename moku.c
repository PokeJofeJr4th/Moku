// gcc -g moku.c rsv.o -Wall -o moku.exe

#include "moku.h"

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

int get_float(char *buf, float *out)
{
    float a, b;
    char op;
    get_line(buf);
    if (*buf == 0)
        return 0;
    // scuffed expression parsing
    if (sscanf(buf, "%f%c%f", &a, &op, &b) == 3)
    {
        switch (op)
        {
        case '*':
            *out = a * b;
            return 1;
        case '/':
            *out = a / b;
            return 1;
        default:
            return 0;
        }
    }
    else if (sscanf(buf, "%f", out) == 1)
        return 1;
    else
        return 0;
}

int add_ingredient(struct Pantry *pantry)
{
    char buf[BUFSIZE];
    struct Ingredient ingredient = ingredient_new();

    // get the name
    puts("Ingredient name:");
    get_line(buf);
    ingredient.head.name = strdup(buf);

    // get the unit
    puts("Unit of measure:");
    get_line(buf);
    ingredient.head.unit = strdup(buf);

    puts("Cost ($):");
    while (!get_float(buf, &ingredient.nutrients.price))
        ;

    puts("Calories:");
    while (!get_float(buf, &ingredient.nutrients.calories))
        ;

    puts("Carbs (g):");
    while (!get_float(buf, &ingredient.nutrients.carbs))
        ;

    puts("Fat (g):");
    while (!get_float(buf, &ingredient.nutrients.fat))
        ;

    puts("Protein (g):");
    while (!get_float(buf, &ingredient.nutrients.protein))
        ;

    puts("Fiber (g):");
    while (!get_float(buf, &ingredient.nutrients.fiber))
        ;

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
        union Food *ingredient = pantry_find(pantry, buf, &input_id);
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
    struct Meal meal = meal_new();

    // get the name
    puts("Meal name:");
    get_line(buf);
    meal.head.name = strdup(buf);

    // get the unit
    puts("Unit of measure:");
    get_line(buf);
    meal.head.unit = strdup(buf);

    input_meal_ingredients(pantry, &meal);

    return pantry_push(pantry, meal_to_food(meal));
}

void remove_ingredient_from_meal(struct Meal *meal, int id)
{
    for (int i = id + 1; i < meal->ingredients_count; i++)
    {
        meal->ingredients[i - 1] = meal->ingredients[i];
    }
    meal->ingredients_count--;
}

void edit_pantry(char *name, struct Pantry *pantry)
{
    char buf[BUFSIZE];
    union Food *food = pantry_find(pantry, name, NULL);

    if (food == NULL)
    {
        printf("Couldn't find food \"%s\"\n", name);
        return;
    }

    puts("Item name: (empty to cancel)");
    get_line(buf);
    if (buf[0] != 0)
    {
        free(food->header.name);
        food->header.name = strdup(buf);
    }

    puts("Item unit: (empty to cancel)");
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
        get_float(buf, &food->ingredient.nutrients.price);
        puts("Calories: (empty to cancel)");
        get_float(buf, &food->ingredient.nutrients.calories);
        puts("Carbs: (empty to cancel)");
        get_float(buf, &food->ingredient.nutrients.carbs);
        puts("Fat: (empty to cancel)");
        get_float(buf, &food->ingredient.nutrients.fat);
        puts("Protein: (empty to cancel)");
        get_float(buf, &food->ingredient.nutrients.protein);
        puts("Fiber: (empty to cancel)");
        get_float(buf, &food->ingredient.nutrients.fiber);
        break;
    case FT_Meal:
        for (int i = 0; i < food->meal.ingredients_count; i++)
        {
            union Food *current_ingredient = &pantry->items[food->meal.ingredients[i].food_id];
            printf("%s quantity (%s): (empty to cancel)\n", current_ingredient->header.name, current_ingredient->header.unit);
            get_float(buf, &food->meal.ingredients[i].amount);
            if (food->meal.ingredients[i].amount == 0)
            {
                remove_ingredient_from_meal(&food->meal, i--);
            }
        }
        input_meal_ingredients(pantry, &food->meal);
        break;
    }
}

void search_pantry(char *name, struct Pantry *pantry)
{
    print_pantry_header();
    for (int i = 0; i < pantry->size; i++)
    {
        union Food *food = &pantry->items[i];
        if (substr(name, food->header.name))
        {
            print_food_short(food, pantry);
        }
    }
}

void remove_food(char *food_name, struct Pantry *pantry)
{
    char buf[BUFSIZE];
    int food_id;
    union Food *food = pantry_find(pantry, food_name, &food_id);
    if (food == NULL)
    {
        printf("Couldn't find food \"%s\"\n", food_name);
        if (pantry_search(pantry, food_name, NULL) != NULL)
        {
            puts("Did you mean one of these?");
            search_pantry(food_name, pantry);
        }
        return;
    }
    int num_refs = 0;
    for (int i = 0; i < pantry->size; i++)
    {
        union Food *current = &pantry->items[i];
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
        union Food *current = &pantry->items[i];
        if (current->header.type != FT_Meal)
            continue;
        struct Meal *meal = &current->meal;
        for (int j = 0; j < meal->ingredients_count; j++)
        {
            if (meal->ingredients[j].food_id == food_id)
            {
                remove_ingredient_from_meal(meal, j--);
            }
            else if (meal->ingredients[j].food_id > food_id)
            {
                meal->ingredients[j].food_id--;
            }
        }
    }
    free(food->header.name);
    free(food->header.unit);
    if (food->header.type == FT_Meal)
    {
        free(food->meal.ingredients);
    }
    for (int i = food_id + 1; i < pantry->size; i++)
    {
        pantry->items[i - 1] = pantry->items[i];
    }
    pantry->size--;
}

void print_food(char *name, struct Pantry *pantry)
{
    union Food *food = pantry_find(pantry, name, NULL);
    if (food == NULL)
    {
        printf("Couldn't find food \"%s\"\n", name);
        return;
    }

    print_food_long(food, pantry);
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
        printf("> ");
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
            printf("Moku\ningredient(i) - add an ingredient\nmeal(m) - add a meal\nhelp - display this message\nquit(q) - exit the program\nls - list ingredients and meals in pantry\nedit(e) <name> - edit a meal or ingredient\nremove(rm) <name> - remove an ingredient or meal\nprint(p) <name> - print a detailed view of an ingredient or meal\n");
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
        else if (strncmp(buf, "p ", 2) == 0)
        {
            print_food(buf + 2, &pantry);
        }
        else if (strncmp(buf, "print ", 6) == 0)
        {
            print_food(buf + 6, &pantry);
        }
        else if (strncmp(buf, "s ", 2) == 0)
        {
            search_pantry(buf + 2, &pantry);
        }
        else if (strncmp(buf, "search ", 7) == 0)
        {
            search_pantry(buf + 7, &pantry);
        }
        else
        {
            printf("Invalid command; use `help` to show options\n");
        }
    }
}
