/*
 exceptions.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef M8R_EXCEPTIONS_H
#define M8R_EXCEPTIONS_H

#include <exception>
#include <string>

namespace m8r {

class MindForgerException : public std::exception
{
private:
    const std::string message;

public:
    explicit MindForgerException(const std::string& message) throw()
        : message(message) {}

    const char* what() const throw() { return message.c_str(); }
};

/**
 * @brief MindForger user exception.
 *
 * User exception is caused by user configuration errors and can be fixed by users.
 * This needs a very clear explanation of what the user did wrong and how to fix it.
 */
class MindForgerUserException : public MindForgerException
{
    explicit MindForgerUserException(const std::string& message) throw()
        : MindForgerException(message) {}
};

/**
 * @brief MindForger runtime exception.
 *
 * Runtime exception is caused by the system configuration/system state error and
 * can be handled by a fallback or user action.
 */
class MindForgerRuntimeException : public MindForgerException
{
    explicit MindForgerRuntimeException(const std::string& message) throw()
        : MindForgerException(message) {}
};

/**
 * @brief MindForger error.
 *
 * Errors should not happen and can only be fixed by programmers ~ is an implementation
 * error. MindForger error should *never* be caught.
 */
class MindForgerError: public MindForgerException
{
    explicit MindForgerError(const std::string& message) throw()
        : MindForgerException(message) {}
};

}

#endif // M8R_EXCEPTIONS
