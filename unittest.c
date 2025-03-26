#include "unittest.h"

#include "food.h"

int test_main()
{
    struct Pantry pantry = pantry_new();
    ASSERT(pantry.size == 0, "Pantry is empty");

    struct Ingredient beans = ingredient_new();
    beans.head.name = "Black Beans";
    beans.head.unit = "Can";

    ASSERT(beans.nutrients.calories == 0, "Beans have no calories");

    beans.nutrients.calories = 400;

    pantry_push(&pantry, ingredient_to_food(beans));

    ASSERT(pantry.size == 1, "Pantry has one item");
    ASSERT(pantry.items[0].header.type == FT_Ingredient, "Pantry's first item is an ingredient");
    ASSERT_STR(pantry.items[0].header.name, "Black Beans", "Pantry's first item is Black Beans");

    struct Meal soup = meal_new();
    soup.head.name = "Black Bean Soup";
    soup.head.unit = "Bowl";

    ASSERT(soup.ingredients_count == 0, "There are no ingredients yet");

    meal_push(&soup, 0, 2.0);

    ASSERT(soup.ingredients_count == 1, "There is now one ingredient");
    ASSERT(soup.ingredients[0].amount == 2.0, "The ingredient has the right quantity");
    ASSERT(soup.ingredients[0].food_id == 0, "The new ingredient has the right ID");

    pantry_push(&pantry, meal_to_food(soup));

    ASSERT(pantry.size == 2, "Pantry has two items now");
    ASSERT(pantry.items[1].header.type == FT_Meal, "Pantry's second item is a meal");
    ASSERT_STR(pantry.items[1].header.name, "Black Bean Soup", "Meal has the correct name");

    return 0;
}
