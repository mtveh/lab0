#include <stdio.h>

double get_daily_rate(int category)
{
    switch (category) {
        case 1:
            return 70.0;
        case 2:
            return 110.0;
        case 3:
            return 180.0;
        default:
            return 0.0;
    }
}

double calc_base(int days, double daily_rate, double distance)
{
    return days * daily_rate + distance * 0.25;
}

double calc_young_fee(int age, double base)
{
    if (age < 25) {
        return 0.15 * base;
    }
    else {
        return 0.0;
    }
}

double calc_fuel_fee(double missing_fuel)
{
    return missing_fuel * 2.2;
}

double calc_insurance_fee(int days, int insurance)
{
    return 18.0 * days * insurance;
}

int get_status(int age, int experience, int category, int insurance, double missing_fuel)
{
    if (age < 21 || experience < 1) {
        return 0;
    }
    else if (category == 3 && insurance == 0) {
        return 1;
    }
    else if (missing_fuel > 0.0) {
        return 2;
    }
    else {
        return 3;
    }
}

int is_input_valid(int days, double distance, int category, int age,
                   int experience, double missing_fuel, int insurance)
{
    if (days < 1 || days > 365) {
        printf("Ошибка: количество дней должно быть от 1 до 365.\n");
        return 0;
    }
    if (distance < 0.0 || distance > 100000.0) {
        printf("Ошибка: пробег должен быть от 0 до 100000 км.\n");
        return 0;
    }
    if (category < 1 || category > 3) {
        printf("Ошибка: категория должна быть 1, 2 или 3.\n");
        return 0;
    }
    if (age < 16 || age > 100) {
        printf("Ошибка: возраст должен быть от 16 до 100 лет.\n");
        return 0;
    }
    if (experience < 0 || experience > age - 16) {
        printf("Ошибка: стаж не может быть отрицательным или больше возраста минус 16.\n");
        return 0;
    }
    if (missing_fuel < 0.0 || missing_fuel > 100.0) {
        printf("Ошибка: недостающее топливо должно быть от 0 до 100 л.\n");
        return 0;
    }
    if (insurance != 0 && insurance != 1) {
        printf("Ошибка: страховка — только 0 или 1.\n");
        return 0;
    }
    return 1;
}

void print_status(int status)
{
    switch (status) {
        case 0:
            printf("DENIED\n");
            break;
        case 1:
            printf("HIGH_DEPOSIT\n");
            break;
        case 2:
            printf("FUEL_CHARGE\n");
            break;
        case 3:
            printf("ACCEPTED\n");
            break;
        default:
            printf("UNKNOWN\n");
            break;
    }
}

void print_report(double base, double young_fee, double fuel_fee,
                  double insurance_fee, double total, int status)
{
    printf("\nБазовая стоимость:      %.2f BYN\n", base);
    printf("Надбавка молодому:      %.2f BYN\n", young_fee);
    printf("Плата за топливо:       %.2f BYN\n", fuel_fee);
    printf("Плата за страховку:     %.2f BYN\n", insurance_fee);
    printf("Итого:                  %.2f BYN\n", total);
    printf("Статус: ");
    print_status(status);
}

void print_age_scenario(int scenario_age, int experience, int category, int insurance,
                        double missing_fuel, double base, double fuel_fee, double insurance_fee)
{
    double young_fee = calc_young_fee(scenario_age, base);
    double total = base + young_fee + fuel_fee + insurance_fee;
    int status = get_status(scenario_age, experience, category, insurance, missing_fuel);

    printf("Возраст %d: надбавка %.2f, итого %.2f, статус ", scenario_age, young_fee, total);
    print_status(status);
}

/* Тестовая копия calc_young_fee с намеренным дефектом (<= вместо <).
   Используется только в мутационном тесте, в основном расчёте не вызывается. */
double calc_young_fee_mutant(int age, double base)
{
    if (age <= 25) {
        return 0.15 * base;
    }
    else {
        return 0.0;
    }
}

int main(void)
{
    int days, category, age, experience, insurance;
    double distance, missing_fuel;

    printf("Введите через пробел: дни, пробег (км), категорию (1-3), возраст, стаж,\n");
    printf("недостающее топливо (л), страховку (1 - да, 0 - нет):\n");
    if (scanf("%d %lf %d %d %d %lf %d", &days, &distance, &category, &age,
              &experience, &missing_fuel, &insurance) != 7) {
        printf("Ошибка ввода.\n");
        return 1;
    }

    if (!is_input_valid(days, distance, category, age, experience, missing_fuel, insurance)) {
        return 1;
    }

    double daily_rate = get_daily_rate(category);
    double base = calc_base(days, daily_rate, distance);
    double young_fee = calc_young_fee(age, base);
    double fuel_fee = calc_fuel_fee(missing_fuel);
    double insurance_fee = calc_insurance_fee(days, insurance);
    double total = base + young_fee + fuel_fee + insurance_fee;
    int status = get_status(age, experience, category, insurance, missing_fuel);

    print_report(base, young_fee, fuel_fee, insurance_fee, total, status);

    printf("\nСценарии по возрасту (остальные данные без изменений):\n");
    print_age_scenario(age, experience, category, insurance, missing_fuel, base, fuel_fee, insurance_fee);
    print_age_scenario(20, experience, category, insurance, missing_fuel, base, fuel_fee, insurance_fee);
    print_age_scenario(26, experience, category, insurance, missing_fuel, base, fuel_fee, insurance_fee);

    printf("\nСамопроверка:\n");

    if (calc_young_fee(25, 100.0) == 0.0) {
        printf("PASS: с 25 лет надбавки нет\n");
    }
    else {
        printf("FAIL: с 25 лет надбавки нет\n");
    }

    if (calc_young_fee(24, 100.0) > 14.99 && calc_young_fee(24, 100.0) < 15.01) {
        printf("PASS: в 24 года надбавка равна 15%% от base\n");
    }
    else {
        printf("FAIL: в 24 года надбавка равна 15%% от base\n");
    }

    if (calc_insurance_fee(10, 0) == 0.0) {
        printf("PASS: без страховки плата за неё равна 0\n");
    }
    else {
        printf("FAIL: без страховки плата за неё равна 0\n");
    }

    if (get_status(20, 1, 3, 0, 5.0) == 0) {
        printf("PASS: DENIED имеет высший приоритет\n");
    }
    else {
        printf("FAIL: DENIED имеет высший приоритет\n");
    }

    if (get_status(30, 5, 3, 0, 5.0) == 1) {
        printf("PASS: HIGH_DEPOSIT важнее FUEL_CHARGE\n");
    }
    else {
        printf("FAIL: HIGH_DEPOSIT важнее FUEL_CHARGE\n");
    }

    if (get_daily_rate(1) < get_daily_rate(2) && get_daily_rate(2) < get_daily_rate(3)) {
        printf("PASS: чем выше категория, тем дороже сутки\n");
    }
    else {
        printf("FAIL: чем выше категория, тем дороже сутки\n");
    }

    printf("\nМутационный тест (calc_young_fee с <= вместо <):\n");
    double mutant_fee = calc_young_fee_mutant(25, 100.0);
    if (mutant_fee == 0.0) {
        printf("Мутация НЕ обнаружена: мутант при age = 25 вернул %.2f\n", mutant_fee);
    }
    else {
        printf("Мутант при age = 25 вернул %.2f вместо 0.00\n", mutant_fee);
        printf("Мутация обнаружена: свойство «с 25 лет надбавки нет» нарушено\n");
    }

    return 0;
}
