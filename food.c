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

struct Ingredient ingredient_new(char *name)
{
    struct Ingredient this;
    this.head.name = name;
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

struct Meal meal_new(char *name)
{
    struct Meal this;
    this.head.name = name;
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
    printf("%-16s $%-5.2f %-4.0f %-5.2fg %-5.2fg\n",
           this->header.name,
           food_price(this, pantry),
           food_calories(this, pantry),
           food_carbs(this, pantry),
           food_protein(this, pantry));
}

void print_pantry(struct Pantry *pantry)
{
    printf("Name             Cost   Cal  Carbs  Protein\n");
    for (int i = 0; i < pantry->size; i++)
    {
        print_food(pantry_get(pantry, i), pantry);
    }
}

void food_rsv_write(union Food *this, FILE *file)
{
    char buf[BUFSIZE + 1];
    rsv_write_field(this->header.name, file);
    switch (this->header.type)
    {
    case FT_Ingredient:
        rsv_write_field("Ingredient", file);
        sprintf_s(buf, BUFSIZE, "%f", this->ingredient.calories);
        // puts(buf);
        rsv_write_field(buf, file);
        sprintf_s(buf, BUFSIZE, "%f", this->ingredient.carbs);
        // puts(buf);
        rsv_write_field(buf, file);
        sprintf_s(buf, BUFSIZE, "%f", this->ingredient.protein);
        // puts(buf);
        rsv_write_field(buf, file);
        sprintf_s(buf, BUFSIZE, "%f", this->ingredient.price);
        // puts(buf);
        rsv_write_field(buf, file);
        break;
    case FT_Meal:
        rsv_write_field("Meal", file);
        for (int i = 0; i < this->meal.ingredients_count; i++)
        {
            sprintf_s(buf, BUFSIZE, "%i", this->meal.ingredients->food_id);
            // puts(buf);
            rsv_write_field(buf, file);
            sprintf_s(buf, BUFSIZE, "%f", this->meal.ingredients->amount);
            // puts(buf);
            rsv_write_field(buf, file);
        }
        break;
    }
    rsv_finish_row(file);
}

int food_from_rsv(struct RsvRow *row, union Food *out)
{
    if (row->num_fields < 3)
    {
        return -1;
    }
    if (strcmp(row->fields[1], "Meal") == 0)
    {
        struct Meal m = meal_new(strdup(row->fields[0]));
        for (int i = 2; i < row->num_fields; i += 2)
        {
            meal_push(&m, atoi(row->fields[i]), atof(row->fields[i + 1]));
        }
        *out = meal_to_food(m);
    }
    else if (strcmp(row->fields[1], "Ingredient") == 0)
    {
        struct Ingredient i = ingredient_new(strdup(row->fields[0]));
        if (row->num_fields != 6)
        {
            return -1;
        }
        i.calories = atof(row->fields[2]);
        i.carbs = atof(row->fields[3]);
        i.protein = atof(row->fields[4]);
        i.price = atof(row->fields[5]);
        *out = ingredient_to_food(i);
    }
    else
    {
        return -1;
    }
    return 0;
}

struct Pantry pantry_from_rsv(FILE *file)
{
    struct Pantry pantry = pantry_new();
    struct RsvRow *row;
    union Food food;
    while (1)
    {
        row = rsv_read_row(file);
        if (row == NULL)
            break;
        food_from_rsv(row, &food);
        pantry_push(&pantry, food);
        free(row);
    }
    return pantry;
}
