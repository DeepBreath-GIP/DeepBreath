#pragma once

#include <plog/Record.h>
#include <plog/Util.h>
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

    static plog::util::nstring header()
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(headers);
    }

    static plog::util::nstring format(const plog::Record& record)
    {
 /*       tm t;
        useUtcTime ? util::gmtime_s(&t, &record.getTime().time) : util::localtime_s(&t, &record.getTime().time);*/

        plog::util::nostringstream ss;
        //ss << t.tm_year + 1900 << PLOG_NSTR("/") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("/") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(";");
        //ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3) << static_cast<int> (record.getTime().millitm) << PLOG_NSTR(";");
        //ss << severityToString(record.getSeverity()) << PLOG_NSTR(";");
        //ss << record.getTid() << PLOG_NSTR(";");
        //ss << record.getObject() << PLOG_NSTR(";");
        //ss << record.getFunc() << PLOG_NSTR("@") << record.getLine() << PLOG_NSTR(";");

        plog::util::nstring message = record.getMessage();
        ss << message;
        ss << PLOG_NSTR("\n");

        return ss.str();
    }

    static void set_headers(std::string headers) {
        DeepBreathCSVFormatter::headers = headers;
    }

private:
    static std::string headers;
};


class DeepBreathCSVRecord1 {
public:
    std::string row_value1;
    std::string row_value2;
    std::string row_value3;
    std::string to_string() {
        return DeepBreathCSVFormatter::row_adder(row_value1, row_value2, row_value3);
    }
};