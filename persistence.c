#include "moku.h"

/* FILE SPECIFICATION
This section uses a C-like syntax to describe how the file is laid out

struct MokuFile {
    food_info foods[];
}

struct food_info {
    utf8_info name;
    utf8_info unit;
    food_type type;
    ingredient_info|meal_info info;
}

enum food_type (uint8) {
    ingredient = 1;
    meal = 2;
}

struct ingredient_info {
    float32 price;
    float32 calories;
    float32 carbs;
    float32 fat;
    float32 protein;
    float32 fiber;
}

struct meal_info {
    byte num_ingredients;
    meal_ingredient ingredients[num_ingredients];
}

struct meal_ingredient {
    int32 id;
    float32 quantity;
}

struct utf8_info {
    uint8 size;
    char content[size];
}

*/

void write_string(char *str, FILE *file)
{
    fputc(strlen(str), file);
    fputs(str, file);
}

int food_write(union Food *this, FILE *file)
{
    write_string(this->header.name, file);
    write_string(this->header.unit, file);
    fputc(this->header.type, file);
    switch (this->header.type)
    {
    case FT_Ingredient:
        printf("%d", sizeof(this->ingredient.nutrients));
        fwrite(&this->ingredient.nutrients, sizeof(this->ingredient.nutrients), 1, file);
        return 0;
    case FT_Meal:
        fputc(this->meal.ingredients_count, file);
        fwrite(&this->meal.ingredients, sizeof(*this->meal.ingredients), this->meal.ingredients_count, file);
        return 0;
    default:
        return -1;
    }
}

char *read_string(FILE *file)
{
    int len = fgetc(file);
    char *buf = malloc(len + 1);
    for (int i = 0; i < len; i++)
    {
        buf[i] = fgetc(file);
    }
    buf[len] = 0;
    return buf;
}

int food_read(union Food *food, FILE *file)
{
    char *name = read_string(file);
    char *unit = read_string(file);
    if (name == NULL || unit == NULL)
        return 0;
    food->header.name = name;
    food->header.unit = unit;
    food->header.type = (enum FoodType)fgetc(file);
    switch (food->header.type)
    {
    case FT_Ingredient:
        fread(&food->ingredient.nutrients, sizeof(food->ingredient.nutrients), 1, file);
        return 1;
    case FT_Meal:
        food->meal.ingredients_capacity = fgetc(file);
        food->meal.ingredients_count = food->meal.ingredients_capacity;
        food->meal.ingredients = malloc(sizeof(*food->meal.ingredients) * food->meal.ingredients_count);
        fread(food->meal.ingredients, sizeof(*food->meal.ingredients), food->meal.ingredients_count, file);
        return 1;
    default:
        return -1;
    }
}

struct Pantry pantry_read(FILE *file)
{
    struct Pantry pantry = pantry_new();
    union Food food;
    while (food_read(&food, file) == 1)
    {
        pantry_push(&pantry, food);
    }
    return pantry;
}