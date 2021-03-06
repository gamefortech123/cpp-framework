/*
 *
 * Copyright 2018 Guolian Zhang.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once 

#include <map>
#include <string>
#include <vector>
#include <base/base.h>
#include <logs/logs.hpp>
#include <reflect/reflect.h>

namespace sql {

typedef std::map<std::string, std::string> SQL_ROW;
typedef std::vector<std::map<std::string, std::string>> SQL_ROWS;

// Database base class.
class SqlBase {
public:
    SqlBase() {}
    virtual ~SqlBase() {}

    // Database initialization, implemented by derived classes.
    //
    // Param db_port is database port.
    // Param db_host is database host.
    // Param db_user is database user.
    // Param db_pass is database user's password.
    // Param db_name is database name.
    virtual bool Init(int db_port,
                      const std::string& db_host,
                      const std::string& db_user,
                      const std::string& db_pass,
                      const std::string& db_name) = 0;

    // Database execution statement operation, like insert, update,
    // implemented by derived classes.
    virtual bool Exec(const std::string& sql) = 0;

    // Query database single record.
    // Parse all members of the object through the reflect library 
    // and assign values through database field names and member types
    template<class T>
    bool QueryRow(const std::string& sql,
                  T& result) {
        SQL_ROW row;
        if (!query_row(sql, row)) {
            return false;
        }

        auto members = REFLECT_MEMBERS(&result);
        for (size_t i = 0; i < members.size(); i++) {
            if (reflect::TypeIsInt8(members[i].type)) {
                *(int8_t*)members[i].value = base::StringToInt8(row[members[i].name]);
            } else if (reflect::TypeIsInt(members[i].type)) {
                *(int*)members[i].value = base::StringToInt(row[members[i].name]);
            } else if (reflect::TypeIsInt64(members[i].type)) {
                *(int64_t*)members[i].value = base::StringToInt64(row[members[i].name]);
            } else if (reflect::TypeIsUInt8(members[i].type)) {
                *(uint8_t*)members[i].value = base::StringToUInt8(row[members[i].name]);
            } else if (reflect::TypeIsUInt(members[i].type)) {
                *(uint*)members[i].value = base::StringToUInt(row[members[i].name]);
            } else if (reflect::TypeIsUInt64(members[i].type)) {
                *(uint64_t*)members[i].value = base::StringToUInt64(row[members[i].name]);
            } else if (reflect::TypeIsFloat(members[i].type)) {
                *(float*)members[i].value = base::StringToFloat(row[members[i].name]);
            } else if (reflect::TypeIsDouble(members[i].type)) {
                *(double*)members[i].value = base::StringToDouble(row[members[i].name]);
            } else if (reflect::TypeIsBool(members[i].type)) {
                *(bool*)members[i].value = base::StringToBool(row[members[i].name]);
            } else if (reflect::TypeIsString(members[i].type)) {
                 *(std::string*)members[i].value = row[members[i].name];
            } else {
                LOG_ERROR("Sql QueryRow unknow type %s", members[i].type.c_str());
            }    
        }

        return true;
    }

    // Query multiple records in the database.
    // Parse all members of the object through the reflect library 
    // and assign values through database field names and member types
    template<class T>
    bool QueryRows(const std::string& sql,
                   std::vector<T>& results) {
        SQL_ROWS rows;
        if (!query_rows(sql, rows)) {
            return false;
        }

        for (size_t i = 0; i < rows.size(); i++) {
            T result;
            auto members = REFLECT_MEMBERS(&result);
            for (size_t j = 0; j < members.size(); j++) {
                if (reflect::TypeIsInt8(members[j].type)) {
                    *(int8_t*)members[j].value = base::StringToInt8(rows[i][members[j].name]);
                } else if (reflect::TypeIsInt(members[j].type)) {
                    *(int*)members[j].value = base::StringToInt(rows[i][members[j].name]);
                } else if (reflect::TypeIsInt64(members[j].type)) {
                    *(int64_t*)members[j].value = base::StringToInt64(rows[i][members[j].name]);
                } else if (reflect::TypeIsUInt8(members[j].type)) {
                    *(uint8_t*)members[j].value = base::StringToUInt8(rows[i][members[j].name]);
                } else if (reflect::TypeIsUInt(members[j].type)) {
                    *(uint*)members[j].value = base::StringToUInt(rows[i][members[j].name]);
                } else if (reflect::TypeIsUInt64(members[j].type)) {
                    *(uint64_t*)members[j].value = base::StringToUInt64(rows[i][members[j].name]);
                } else if (reflect::TypeIsFloat(members[j].type)) {
                    *(float*)members[j].value = base::StringToFloat(rows[i][members[j].name]);
                } else if (reflect::TypeIsDouble(members[j].type)) {
                    *(double*)members[j].value = base::StringToDouble(rows[i][members[j].name]);
                } else if (reflect::TypeIsBool(members[j].type)) {
                    *(bool*)members[j].value = base::StringToBool(rows[i][members[j].name]);;
                } else if (reflect::TypeIsString(members[j].type)) {
                    *(std::string*)members[j].value = rows[i][members[j].name];
                } else {
                    LOG_ERROR("Sql QueryRows unknow type %s", members[j].type.c_str());
                }    
            }

            results.push_back(result);
        }
        return true;
    }

private:
    // The concrete implementation of the query, 
    // implemented by a derived class.
    virtual bool query_row(const std::string& sql,
                           SQL_ROW& row) = 0;
    virtual bool query_rows(const std::string& sql,
                            SQL_ROWS& row) = 0;
};

}; // namespace sql