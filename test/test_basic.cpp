/**
 * @file main.cpp
 * @brief Exemple basique d'utilisation de PreciseTime
 * @example Test_Basic.ino
 * @version 1.0.0
 * @date 2025
 * 
 * Copyright (C) 2025 Fo170
 * 
 * Ce programme est un logiciel libre ; vous pouvez le redistribuer
 * et/ou le modifier selon les termes de la GNU General Public License
 * telle que publiée par la Free Software Foundation ; soit la version 3
 * de la Licence, soit (à votre choix) toute version ultérieure.
 */

#include <Arduino.h>
#include <unity.h>
#include <PreciseTime.h>

void test_initialization() {
    TEST_ASSERT_FALSE(PreciseTime::isInitialized());
    PreciseTime::begin();
    TEST_ASSERT_TRUE(PreciseTime::isInitialized());
}

void test_time_increases() {
    uint64_t t1 = PreciseTime::getMicroseconds();
    delay(10);
    uint64_t t2 = PreciseTime::getMicroseconds();
    TEST_ASSERT_GREATER_THAN(t1, t2);
}

void test_reset_function() {
    uint64_t t1 = PreciseTime::getMicroseconds();
    delay(50);
    PreciseTime::reset();
    uint64_t t2 = PreciseTime::getMicroseconds();
    TEST_ASSERT_LESS_THAN(t2, 1000); // Should be near 0 after reset
}

void test_formatted_string() {
    String formatted = PreciseTime::getFormattedString();
    TEST_ASSERT_TRUE(formatted.length() > 0);
    // Format should be HH:MM:SS or X days, HH:MM:SS
    TEST_ASSERT_TRUE(formatted.indexOf(':') > 0);
}

void test_overflow_calculation() {
    double years = PreciseTime::getOverflowYears();
    TEST_ASSERT_TRUE(years > 500000); // Should be around 584,942 years
}

void setup() {
    delay(2000); // Give time for serial monitor
    
    UNITY_BEGIN();
    
    RUN_TEST(test_initialization);
    RUN_TEST(test_time_increases);
    RUN_TEST(test_reset_function);
    RUN_TEST(test_formatted_string);
    RUN_TEST(test_overflow_calculation);
    
    UNITY_END();
}

void loop() {
    // Empty
}