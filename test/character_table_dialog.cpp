/**
 * Schoenflies
 * Copyright (c) 2022 Luuk Kempen
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

#include <boost/test/unit_test.hpp>
#include <QApplication>
#include <QLabel>
#include <QSettings>
#include <QStringList>
#include <QTableView>
#include <QTemporaryDir>
#include "../src/gui/character_table_dialog.h"
#include "../src/gui/settings_dialog.h"

namespace {
struct ApplicationFixture {
    int argc = 1;
    char name[24] = "character-table-test";
    char* argv[2] = {name, nullptr};
    QApplication application{argc, argv};
};

QStringList row(QTableView* view, int index) {
    QStringList texts;
    for (int j = 0; j < view->model()->columnCount(); ++j) {
        texts << view->model()->index(index, j).data().toString();
    }
    return texts;
}

QLabel* complex_characters_note(CharacterTableDialog& dialog) {
    for (auto label : dialog.findChildren<QLabel*>()) {
        if (label->text().contains("complex-conjugate pair")) return label;
    }
    return nullptr;
}
}

BOOST_FIXTURE_TEST_SUITE(character_table_dialog, ApplicationFixture)

BOOST_AUTO_TEST_CASE(S4_shows_split_classes_and_complex_characters) {
    CharacterTableDialog dialog;
    dialog.show_point_group(PointGroupLabel(PointGroupLabel::Class::S, 4));
    auto view = dialog.findChild<QTableView*>();
    BOOST_REQUIRE(view != nullptr);

    BOOST_TEST(view->model()->rowCount() == 4);
    BOOST_TEST(view->model()->columnCount() == 4);
    BOOST_TEST(view->model()->headerData(2, Qt::Vertical).toString().contains("E<sub>+</sub>"));
    BOOST_TEST(view->model()->headerData(3, Qt::Horizontal).toString().contains("<i>S</i><sub>4</sub><sup>3</sup>"));
    BOOST_TEST(row(view, 2) == QStringList({"1", "i", "−1", "−i"}));
    BOOST_TEST(row(view, 3) == QStringList({"1", "−i", "−1", "i"}));

    QLabel* note = complex_characters_note(dialog);
    BOOST_REQUIRE(note != nullptr);
    BOOST_TEST(!note->isHidden());
}

BOOST_AUTO_TEST_CASE(C3_shows_exponential_characters) {
    CharacterTableDialog dialog;
    dialog.show_point_group(PointGroupLabel(PointGroupLabel::Class::C, 3));
    auto view = dialog.findChild<QTableView*>();
    BOOST_REQUIRE(view != nullptr);

    BOOST_TEST(row(view, 1) == QStringList({"1", "exp(2πi/3)", "exp(−2πi/3)"}));
    BOOST_TEST(row(view, 2) == QStringList({"1", "exp(−2πi/3)", "exp(2πi/3)"}));
}

BOOST_AUTO_TEST_CASE(real_tables_are_unchanged) {
    CharacterTableDialog dialog;
    dialog.show_point_group(PointGroupLabel(PointGroupLabel::Class::Cv, 3));
    auto view = dialog.findChild<QTableView*>();
    BOOST_REQUIRE(view != nullptr);

    BOOST_TEST(view->model()->columnCount() == 3);
    BOOST_TEST(row(view, 2) == QStringList({"2", "−1", "0"}));
    BOOST_TEST(complex_characters_note(dialog)->isHidden());
}

BOOST_AUTO_TEST_CASE(include_complex_irreps_setting_persists_and_defaults_off) {
    QTemporaryDir directory;
    BOOST_REQUIRE(directory.isValid());
    const auto file = directory.filePath("settings.ini");
    {
        QSettings settings(file, QSettings::IniFormat);
        BOOST_TEST(!SettingsDialog::load_include_complex_irreps(settings));
        BOOST_REQUIRE(SettingsDialog::save_include_complex_irreps(settings, true));
    }
    {
        QSettings settings(file, QSettings::IniFormat);
        BOOST_TEST(SettingsDialog::load_include_complex_irreps(settings));
        BOOST_REQUIRE(SettingsDialog::save_include_complex_irreps(settings, false));
    }
    QSettings settings(file, QSettings::IniFormat);
    BOOST_TEST(!SettingsDialog::load_include_complex_irreps(settings));
}

BOOST_AUTO_TEST_SUITE_END();
