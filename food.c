#include "moku.h"

struct Pantry;

enum FoodType
{
    FT_Ingredient,
    FT_Meal
};

struct FoodHeader
{
    enum FoodType type;
    /// @brief owned string describing the name of the meal
    char *name;
    /// @brief owned string describing the unit used in the meal
    char *unit;
};

struct Ingredient
{
    struct FoodHeader head;
    /// @brief price in USD of one unit of the ingredient
    float price;
    /// @brief kCal in one unit of the ingredient
    float calories;
    /// @brief g of protein in one unit of the ingredient
    float protein;
    /// @brief g of carbs in one unit of the ingredient
    float carbs;
};

struct Ingredient ingredient_new(char *name, char *unit)
{
    struct Ingredient this;
    this.head.name = name;
    this.head.unit = unit;
    this.head.type = FT_Ingredient;
    this.price = 0;
    this.calories = 0;
    this.protein = 0;
    this.carbs = 0;
    return this;
}

float ingredient_calories(struct Ingredient *this)
{
    return this->calories;
}

float ingredient_protein(struct Ingredient *this)
{
    return this->protein;
}

float ingredient_carbs(struct Ingredient *this)
{
    return this->carbs;
}

float ingredient_price(struct Ingredient *this)
{
    return this->price;
}

struct Meal
{
    struct FoodHeader head;
    /// @brief number of foods stored in the ingredients array
    int ingredients_count;
    /// @brief number of ingredients the current array can hold
    int ingredients_capacity;
    /// @brief ingredients and amounts contained in the meal item
    struct
    {
        /// @brief index of the type of food included in the meal
        int food_id;
        /// @brief amount of food included, based on that food's units
        float amount;
    } *ingredients;
};

struct Meal meal_new(char *name, char *unit)
{
    struct Meal this;
    this.head.name = name;
    this.head.unit = unit;
    this.head.type = FT_Meal;
    this.ingredients_count = 0;
    this.ingredients_capacity = 1;
    this.ingredients = malloc(sizeof(*this.ingredients));
    return this;
}

int meal_push(struct Meal *this, int food_id, float amount)
{
    if (this->ingredients_capacity == this->ingredients_count)
    {
        this->ingredients = realloc(this->ingredients, sizeof(*this->ingredients) * this->ingredients_capacity * 2);
        if (!this->ingredients)
            return -1;
        this->ingredients_capacity *= 2;
    }
    this->ingredients[this->ingredients_count].food_id = food_id;
    this->ingredients[this->ingredients_count].amount = amount;
    return this->ingredients_count++;
}

union Food
{
    struct Ingredient ingredient;
    struct Meal meal;
    struct FoodHeader header;
};

union Food ingredient_to_food(struct Ingredient i)
{
    union Food f;
    f.ingredient = i;
    return f;
}

union Food meal_to_food(struct Meal i)
{
    union Food f;
    f.meal = i;
    return f;
}

struct Pantry
{
    int size;
    int capacity;
    union Food *items;
};

struct Pantry pantry_new()
{
    struct Pantry this;
    this.size = 0;
    this.capacity = 1;
    this.items = malloc(sizeof(*this.items));
    return this;
}

int pantry_push(struct Pantry *this, union Food item)
{
    if (this->size == this->capacity)
    {
        this->items = realloc(this->items, sizeof(*(this->items)) * this->capacity * 2);
        if (!this->items)
            return -1;
        this->capacity *= 2;
    }
    this->items[this->size] = item;
    return this->size++;
}

union Food *pantry_get(struct Pantry *this, int index)
{
    if (index >= this->capacity)
    {
        return NULL;
    }
    return &this->items[index];
}

float visit_food(union Food *this, struct Pantry *pantry, float (*visitor)(struct Ingredient *))
{
    switch (this->header.type)
    {
    case FT_Ingredient:
        return visitor(&this->ingredient);
    case FT_Meal:
        float total = 0;
        for (int i = 0; i < this->meal.ingredients_count; i++)
        {
            total += visit_food(pantry_get(pantry, this->meal.ingredients[i].food_id), pantry, visitor) * this->meal.ingredients[i].amount;
        }

        return total;
    }
    return 0;
}

float food_calories(union Food *this, struct Pantry *pantry)
{
    return visit_food(this, pantry, ingredient_calories);
}

float food_carbs(union Food *this, struct Pantry *pantry)
{
    return visit_food(this, pantry, ingredient_carbs);
}

float food_protein(union Food *this, struct Pantry *pantry)
{
    return visit_food(this, pantry, ingredient_protein);
}

float food_price(union Food *this, struct Pantry *pantry)
{
    return visit_food(this, pantry, ingredient_price);
}

void print_food(union Food *this, struct Pantry *pantry)
{
    printf("%-16s %-8s $%-5.2f %-4.0f %-5.2fg %-5.2fg\n",
           this->header.name,
           this->header.unit,
           food_price(this, pantry),
           food_calories(this, pantry),
           food_carbs(this, pantry),
           food_protein(this, pantry));
}

void print_pantry(struct Pantry *pantry)
{
    printf("Name             Unit     Cost   Cal  Carbs  Protein\n");
    for (int i = 0; i < pantry->size; i++)
    {
        print_food(pantry_get(pantry, i), pantry);
    }
}

int food_write(union Food *this, FILE *file)
{
    int end_marker = -1;
    fputs(this->header.name, file);
    fputc(0, file);
    fputs(this->header.unit, file);
    fputc(0, file);
    switch (this->header.type)
    {
    case FT_Ingredient:
        fputc(1, file);
        fwrite(&this->ingredient.calories, sizeof(this->ingredient.calories), 1, file);
        fwrite(&this->ingredient.carbs, sizeof(this->ingredient.carbs), 1, file);
        fwrite(&this->ingredient.protein, sizeof(this->ingredient.protein), 1, file);
        fwrite(&this->ingredient.price, sizeof(this->ingredient.price), 1, file);
        return 0;
    case FT_Meal:
        fputc(2, file);
        fwrite(&this->meal.ingredients, sizeof(*this->meal.ingredients), this->meal.ingredients_count, file);
        fwrite(&end_marker, sizeof(*this->meal.ingredients), 1, file);
        return 0;
    default:
        return -1;
    }
}

char *read_string(FILE *file)
{
    char buf[BUFSIZE];
    int c;
    for (int i = 0; i < BUFSIZE; i++)
    {
        c = fgetc(file);
        if (c == EOF)
        {
            return NULL;
        }
        if (i < BUFSIZE)
        {
            buf[i] = c;
        }
        if (c == 0)
        {
            break;
        }
    }
    buf[BUFSIZE - 1] = 0;
    return strdup(buf);
}

int food_read(union Food *food, FILE *file)
{
    int meal_id;
    float meal_amount;
    char *name = read_string(file);
    char *unit = read_string(file);
    if (name == NULL || unit == NULL)
        return 0;
    food->header.name = name;
    food->header.unit = unit;
    switch (fgetc(file))
    {
    case 1:
        food->header.type = FT_Ingredient;
        fread(&food->ingredient.calories, sizeof(food->ingredient.calories), 1, file);
        fread(&food->ingredient.carbs, sizeof(food->ingredient.carbs), 1, file);
        fread(&food->ingredient.protein, sizeof(food->ingredient.protein), 1, file);
        fread(&food->ingredient.price, sizeof(food->ingredient.price), 1, file);
        return 1;
    case 2:
        food->header.type = FT_Meal;
        fread(&meal_id, sizeof(int), 1, file);
        while (meal_id != -1)
        {
            fread(&meal_amount, sizeof(float), 1, file);
            meal_push(&food->meal, meal_id, meal_amount);
            fread(&meal_id, sizeof(int), 1, file);
        }
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
