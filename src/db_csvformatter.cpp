#include "db_csvformatter.hpp"

std::string DeepBreathCSVFormatter::headers;
unsigned int DeepBreathCSVFormatter::num_row_items;
unsigned int DeepBreathCSVFormatter::cur_item_index;

plog::util::nstring DeepBreathCSVFormatter::header()
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(headers);
}

plog::util::nstring DeepBreathCSVFormatter::format(const plog::Record& record)
{
    plog::util::nostringstream ss;
    plog::util::nstring message = record.getMessage();

    if (cur_item_index > 0) {
        ss << ",";
    }
    ss << message;

    if (cur_item_index == num_row_items - 1) {
        ss << PLOG_NSTR("\n");
    }
    cur_item_index = (cur_item_index + 1) % num_row_items;
    return ss.str();
}

void DeepBreathCSVFormatter::set_headers(std::string headers) {
    DeepBreathCSVFormatter::headers = headers + "\n";
}

void DeepBreathCSVFormatter::set_num_row_items(unsigned int num_row_items) {
    DeepBreathCSVFormatter::cur_item_index = 0;
    DeepBreathCSVFormatter::num_row_items = num_row_items;
}