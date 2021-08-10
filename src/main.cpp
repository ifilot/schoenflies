/**
 * Schoenflies
 * Copyright (c) 2021 Luuk Kempen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "main.h"

int main(int argc, char** argv) {
    std::cout << "Hello, World!" << std::endl;
    std::cout << PROGRAM_NAME ": " PROGRAM_DESC ", version " PROGRAM_VERSION << std::endl;

    QApplication app(argc, argv);

    QPushButton hello("Hello, World!", 0);

    hello.show();

    return app.exec();
}
