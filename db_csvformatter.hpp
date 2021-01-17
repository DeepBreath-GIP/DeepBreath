#pragma once

#include <plog/Record.h>
#include <plog/Util.h>
#include <plog/Log.h>
#include <iomanip>
#include <sstream>
#include <list>
#include <map>
#include <codecvt>

#define DBLOG PLOG_INFO

class DeepBreathCSVFormatter
{
public:

    template<typename T>
    static T row_adder(T row_value) {
        return row_value;
    }

    template<typename T, typename... RowValues>
    static T row_adder(T row_value, RowValues... row_values) {
        return row_value + "," + row_adder(row_values...);
    }

    static plog::util::nstring header();

    static plog::util::nstring format(const plog::Record& record);
    static void set_headers(std::string headers);
    static void set_num_row_items(unsigned int num_row_items);

private:
    static std::string headers;
    static unsigned int num_row_items;
    static unsigned int cur_item_index;
};
