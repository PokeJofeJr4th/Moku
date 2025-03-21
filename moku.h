#ifndef _MOKU_H_DEFINED
#define _MOKU_H_DEFINED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE (32)

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
struct Meal meal_new();
struct Ingredient ingredient_new();

int meal_push(struct Meal *this, int food_id, float amount);

#endif