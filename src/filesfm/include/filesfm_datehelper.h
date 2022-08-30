#pragma once

#include <Windows.h>
#include <string>
#include <list>
#include <sstream>

using namespace std;

namespace FilesFM {
    class DateHelper {
        public:
        static string GetBuildDate()
        {
            string buildDate;

            try {
                stringstream ss(__DATE__);
                string day, monthName, year;

                ss >> monthName >> day >> year;

                string months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

                string month = "0";

                for ( int i = 0; i < 12; i++ )
                {
                    if ( months[i].compare(monthName) == 0 )
                    {
                        month = to_string(i + 1);
                        break;
                    }
                }

                buildDate = day + "." + month + "." + year;
            }
            catch(const std::exception& ex)
            {
  
            }

            return buildDate;
        }
    };
}