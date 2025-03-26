#ifndef FOOD_H_
#define FOOD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum FoodType
{
    FT_Ingredient = 1,
    FT_Meal = 2
};

struct Nutrition
{
    /// @brief price in USD of one unit of the ingredient
    float price;
    /// @brief kCal in one unit of the ingredient
    float calories;
    /// @brief g of carbs in one unit of the ingredient
    float carbs;
    float fat;
    /// @brief g of protein in one unit of the ingredient
    float protein;
    float fiber;
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
    struct Nutrition nutrients;
};

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

union Food
{
    struct Ingredient ingredient;
    struct Meal meal;
    struct FoodHeader header;
};

struct Pantry
{
    int size;
    int capacity;
    union Food *items;
};

struct Pantry pantry_new();

struct Pantry pantry_new();
int pantry_push(struct Pantry *this, union Food item);
union Food *pantry_find(struct Pantry *pantry, char *name, int *index);
union Food *pantry_search(struct Pantry *pantry, char *name, int *index);
void print_pantry_header();
void print_pantry(struct Pantry *pantry);

struct Meal meal_new();

int meal_push(struct Meal *this, int food_id, float amount);
union Food meal_to_food(struct Meal i);

struct Ingredient ingredient_new();

union Food ingredient_to_food(struct Ingredient i);
void visit_food(union Food *this, struct Pantry *pantry, float multiplier, struct Nutrition *nutrients);
void print_food_short(union Food *this, struct Pantry *pantry);
void print_food_long(union Food *this, struct Pantry *pantry);

int substr(const char *needle, const char *haystack);
#endif