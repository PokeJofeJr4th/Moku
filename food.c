#include "food.h"

struct Pantry;

struct Nutrition nutrition_new()
{
    struct Nutrition nutrition;
    nutrition.calories = 0;
    nutrition.carbs = 0;
    nutrition.fat = 0;
    nutrition.fiber = 0;
    nutrition.price = 0;
    nutrition.protein = 0;
    return nutrition;
}

struct Ingredient ingredient_new()
{
    struct Ingredient this;
    this.head.type = FT_Ingredient;
    this.nutrients = nutrition_new();
    return this;
}

struct Meal meal_new()
{
    struct Meal this;
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

union Food *pantry_find(struct Pantry *pantry, char *name, int *index)
{
    for (int i = 0; i < pantry->size; i++)
    {
        if (strcmp(name, pantry->items[i].header.name) == 0)
        {
            if (index != NULL)
                *index = i;
            return &pantry->items[i];
        }
    }
    return NULL;
}

union Food *pantry_search(struct Pantry *pantry, char *name, int *index)
{
    for (int i = 0; i < pantry->size; i++)
    {
        if (substr(name, pantry->items[i].header.name) == 0)
        {
            if (index != NULL)
                *index = i;
            return &pantry->items[i];
        }
    }
    return NULL;
}

void visit_food(union Food *this, struct Pantry *pantry, float multiplier, struct Nutrition *nutrients)
{
    switch (this->header.type)
    {
    case FT_Ingredient:
        nutrients->price += this->ingredient.nutrients.price * multiplier;
        nutrients->calories += this->ingredient.nutrients.calories * multiplier;
        nutrients->carbs += this->ingredient.nutrients.carbs * multiplier;
        nutrients->fat += this->ingredient.nutrients.fat * multiplier;
        nutrients->protein += this->ingredient.nutrients.protein * multiplier;
        nutrients->fiber += this->ingredient.nutrients.fiber * multiplier;
        return;
    case FT_Meal:
        for (int i = 0; i < this->meal.ingredients_count; i++)
        {
            visit_food(
                &pantry->items[this->meal.ingredients[i].food_id], pantry, multiplier * this->meal.ingredients[i].amount, nutrients);
        }
        return;
    }
}

void print_food_short(union Food *this, struct Pantry *pantry)
{
    struct Nutrition nutrition = nutrition_new();
    visit_food(this, pantry, 1.0, &nutrition);
    printf("%-16s %-8s $%-5.2f %-4.0f %-5.2fg %-5.2fg %-5.2fg %-5.2fg\n",
           this->header.name,
           this->header.unit, nutrition.price, nutrition.calories, nutrition.carbs, nutrition.fat, nutrition.protein, nutrition.fiber);
}

void print_food_long(union Food *this, struct Pantry *pantry)
{
    struct Nutrition nutrition = nutrition_new();
    switch (this->header.type)
    {
    case FT_Meal:
        printf("                                Price   Cal   Carbs   Fat     Prot   Fiber\n");
        for (int i = 0; i < this->meal.ingredients_count; i++)
        {
            nutrition = nutrition_new();
            union Food *food = &pantry->items[this->meal.ingredients[i].food_id];
            visit_food(food, pantry, this->meal.ingredients[i].amount, &nutrition);
            printf("%-5.2f %-8s %-16s $%-6.2f %-5.0f %-6.2fg %-6.2fg %-6.2fg %-6.2fg\n",
                   this->meal.ingredients[i].amount,
                   food->header.unit,
                   food->header.name,
                   nutrition.price, nutrition.calories, nutrition.carbs, nutrition.fat, nutrition.protein, nutrition.fiber);
        }
        nutrition = nutrition_new();
        visit_food(this, pantry, 1.0, &nutrition);
        printf("\n               Total            $%-6.2f %-5.0f %-6.2fg %-6.2fg %-6.2fg %-6.2fg\n",
               nutrition.price, nutrition.calories, nutrition.carbs, nutrition.fat, nutrition.protein, nutrition.fiber);
        return;
    case FT_Ingredient:
        printf("Name             Unit     Cost    Cal   Carbs   Fat     Prot     Fiber\n");
        visit_food(this, pantry, 1.0, &nutrition);
        printf("%-16s %-8s $%-6.2f %-5.0f %-6.2fg %-6.2fg %-6.2fg %-6.2fg\n",
               this->header.name,
               this->header.unit, nutrition.price, nutrition.calories, nutrition.carbs, nutrition.fat, nutrition.protein, nutrition.fiber);
    }
}

void print_pantry_header()
{
    printf("Name             Unit     Cost   Cal  Carbs  Fat    Prot   Fiber\n");
}

void print_pantry(struct Pantry *pantry)
{
    print_pantry_header();
    for (int i = 0; i < pantry->size; i++)
    {
        print_food_short(&pantry->items[i], pantry);
    }
}
