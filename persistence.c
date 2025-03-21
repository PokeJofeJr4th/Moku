#include "moku.h"

#include <stdlib.h>

#include "rsv.h"

void write_string(char *str, FILE *file)
{
    fputc(strlen(str), file);
    fputs(str, file);
}

int food_write(union Food *this, FILE *file)
{
    rsv_write_field(this->header.name, file);
    rsv_write_field(this->header.unit, file);
    switch (this->header.type)
    {
    case FT_Ingredient:
        rsv_write_field("I", file);
        rsv_fmt_field(file, "%.2f", this->ingredient.nutrients.price);
        rsv_fmt_field(file, "%.2f", this->ingredient.nutrients.calories);
        rsv_fmt_field(file, "%.2f", this->ingredient.nutrients.carbs);
        rsv_fmt_field(file, "%.2f", this->ingredient.nutrients.fat);
        rsv_fmt_field(file, "%.2f", this->ingredient.nutrients.protein);
        rsv_fmt_field(file, "%.2f", this->ingredient.nutrients.fiber);
        break;
    case FT_Meal:
        rsv_write_field("M", file);
        for (int i = 0; i < this->meal.ingredients_count; i++)
        {
            rsv_fmt_field(file, "%i", this->meal.ingredients[i].food_id);
            rsv_fmt_field(file, "%f", this->meal.ingredients[i].amount);
        }
        break;
    default:
        return -1;
    }
    return rsv_finish_row(file);
}

void food_read(union Food *food, struct RsvRow *row)
{
    char *food_type = row->fields[2];
    if (strcmp(food_type, "I") == 0)
    {
        food->header.type = FT_Ingredient;
        sscanf(row->fields[3], "%f", &food->ingredient.nutrients.price);
        sscanf(row->fields[4], "%f", &food->ingredient.nutrients.calories);
        sscanf(row->fields[5], "%f", &food->ingredient.nutrients.carbs);
        sscanf(row->fields[6], "%f", &food->ingredient.nutrients.fat);
        sscanf(row->fields[7], "%f", &food->ingredient.nutrients.protein);
        sscanf(row->fields[8], "%f", &food->ingredient.nutrients.fiber);
    }
    else if (strcmp(food_type, "M") == 0)
    {
        food->meal = meal_new();
        int id;
        float amount;
        for (int i = 3; i < row->num_fields; i += 2)
        {
            sscanf(row->fields[i], "%i", &id);
            sscanf(row->fields[i + 1], "%f", &amount);
            meal_push(&food->meal, id, amount);
        }
    }
    food->header.name = strdup(row->fields[0]);
    food->header.unit = strdup(row->fields[1]);
}

struct Pantry pantry_read(FILE *file)
{
    struct Pantry pantry = pantry_new();
    union Food food;
    struct RsvTable *table = rsv_read_table(file);
    for (int i = 0; i < table->num_rows; i++)
    {
        food_read(&food, table->rows[i]);
        pantry_push(&pantry, food);
    }
    free(table);
    return pantry;
}