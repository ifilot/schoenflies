#include <boost/test/unit_test.hpp>
#include <QApplication>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <cmath>
#include <algorithm>
#include <QSettings>
#include <QTemporaryDir>
#include "../src/gui/practice_flowchart_diagram.h"
#include "../src/gui/practice_widget.h"
#include "../src/gui/theme.h"

namespace {
struct ApplicationFixture {
    int argc = 1;
    char name[16] = "practice-test";
    char* argv[2] = {name, nullptr};
    QApplication application{argc, argv};
    ApplicationFixture() { Theme::apply(application); }
};

std::shared_ptr<Symmetry> load(const char* name) {
    return std::make_shared<Symmetry>(std::make_shared<Structure>(
        std::string(":/assets/structures/") + name + ".xyz"));
}

void answer(PracticeFlowchartWidget* chart, const std::vector<int>& answers) {
    for (int value : answers) {
        auto cards = chart->findChildren<PracticeFlowchartStepWidget*>();
        auto card = cards.back();
        if (auto combo = card->findChild<QComboBox*>()) {
            combo->setCurrentIndex(value - 2);
            BOOST_REQUIRE(QMetaObject::invokeMethod(card, "handle_choose_answer"));
        } else {
            BOOST_REQUIRE(QMetaObject::invokeMethod(card, "handle_no_yes_answer", Q_ARG(int, value)));
        }
    }
    auto card = chart->findChildren<PracticeFlowchartStepWidget*>().back();
    BOOST_REQUIRE(QMetaObject::invokeMethod(card, "handle_check_answer"));
    bool correct = false;
    for (auto label : card->findChildren<QLabel*>()) {
        correct |= label->text().contains("Correct — point group determined!");
    }
    BOOST_TEST(correct);
}
}

BOOST_FIXTURE_TEST_SUITE(practice_widget, ApplicationFixture)

BOOST_AUTO_TEST_CASE(theme_preference_survives_settings_recreation) {
    QTemporaryDir directory;
    BOOST_REQUIRE(directory.isValid());
    const auto file = directory.filePath("settings.ini");
    {
        QSettings settings(file, QSettings::IniFormat);
        BOOST_CHECK(Theme::load(settings) == Theme::Mode::Dark);
        BOOST_REQUIRE(Theme::save(settings, Theme::Mode::Light));
    }
    {
        QSettings settings(file, QSettings::IniFormat);
        BOOST_CHECK(Theme::load(settings) == Theme::Mode::Light);
        BOOST_REQUIRE(Theme::save(settings, Theme::Mode::Dark));
    }
    QSettings settings(file, QSettings::IniFormat);
    BOOST_CHECK(Theme::load(settings) == Theme::Mode::Dark);
    settings.setValue("appearance/theme", "invalid");
    BOOST_CHECK(Theme::load(settings) == Theme::Mode::Dark);
}

BOOST_AUTO_TEST_CASE(live_theme_switch_preserves_exercise_and_updates_feedback) {
    PracticeWidget widget(nullptr);
    PracticeFlowchartDiagram diagram;
    widget.create_practice_structure(load("methane"));
    widget.start_current_structure_flowchart();
    auto chart = widget.findChild<PracticeFlowchartWidget*>();
    answer(chart, {0, 1, 0});
    diagram.set_path({"start", "higher_order", "high_sym", "Td"});
    const auto path = diagram.get_path();
    const auto cards = chart->findChildren<PracticeFlowchartStepWidget*>();
    auto feedback = chart->findChild<QLabel*>("correctFeedback");
    BOOST_REQUIRE(feedback);
    for (auto mode : {Theme::Mode::Light, Theme::Mode::Dark, Theme::Mode::Light}) {
        Theme::apply(application, mode);
        application.processEvents();
        feedback->ensurePolished();
        BOOST_TEST(diagram.get_path() == path);
        BOOST_TEST(chart->findChildren<PracticeFlowchartStepWidget*>() == cards);
        BOOST_CHECK(diagram.palette().color(QPalette::Window) == application.palette().color(QPalette::Window));
        BOOST_CHECK(feedback->palette().color(QPalette::WindowText) ==
                   QColor(mode == Theme::Mode::Light ? "#17653b" : "#80e0b0"));
        for (auto button : chart->findChildren<QPushButton*>()) {
            button->ensurePolished();
            const auto palette = button->palette();
            BOOST_CHECK(palette.color(QPalette::ButtonText) != palette.color(QPalette::Button));
        }
    }
}

BOOST_AUTO_TEST_CASE(both_themes_have_readable_controls) {
    // Reproduce issue #3's system palette: white foregrounds before applying
    // the app theme. Check the actual styled controls, not just theme constants.
    QPalette system_palette;
    system_palette.setColor(QPalette::ButtonText, Qt::white);
    system_palette.setColor(QPalette::Text, Qt::white);
    application.setPalette(system_palette);
    auto check_theme = [&](Theme::Mode mode) {
    Theme::apply(application, mode);
    PracticeWidget widget(nullptr);
    widget.create_practice_structure(load("borane"));
    widget.start_current_structure_flowchart();
    auto chart = widget.findChild<PracticeFlowchartWidget*>();
    chart->handle_answer(0, 0);
    chart->handle_answer(1, 0);
    chart->handle_answer(2, 1);
    widget.ensurePolished();
    auto luminance = [](QColor color) {
        auto linear = [](double c) { return c <= .04045 ? c / 12.92 : std::pow((c + .055) / 1.055, 2.4); };
        return .2126 * linear(color.redF()) + .7152 * linear(color.greenF()) + .0722 * linear(color.blueF());
    };
    for (auto button : chart->findChildren<QPushButton*>()) {
        button->ensurePolished();
        double text = luminance(button->palette().color(QPalette::ButtonText));
        double background = luminance(button->palette().color(QPalette::Button));
        BOOST_TEST((std::max(text, background) + .05) / (std::min(text, background) + .05) >= 4.5);
    }
    auto combo = chart->findChild<QComboBox*>();
    BOOST_REQUIRE(combo);
    combo->ensurePolished();
    const auto text = luminance(combo->palette().color(QPalette::Text));
    const auto background = luminance(combo->palette().color(QPalette::Base));
    BOOST_TEST((std::max(text, background) + .05) / (std::min(text, background) + .05) >= 4.5);
    const auto palette = application.palette();
    BOOST_TEST((luminance(palette.color(QPalette::HighlightedText)) + .05) /
               (luminance(palette.color(QPalette::Highlight)) + .05) >= 4.5);
    };
    check_theme(Theme::Mode::Dark);
    check_theme(Theme::Mode::Light);
}

BOOST_AUTO_TEST_CASE(manual_load_resets_completed_route) {
    PracticeWidget widget(nullptr);
    widget.create_practice_structure(load("borane"));
    widget.start_current_structure_flowchart();
    auto chart = widget.findChild<PracticeFlowchartWidget*>();
    answer(chart, {0, 0, 1, 3, 1, 1});
    std::vector<std::string> route;
    QObject::connect(&widget, &PracticeWidget::flowchart_route_changed,
                     [&route](const auto& path) { route = path; });
    widget.create_practice_structure(load("methane"));
    BOOST_REQUIRE_EQUAL(route.size(), 1);
    BOOST_TEST(route.front() == "start");
    answer(chart, {0, 1, 0});
    BOOST_TEST(route.back() == "Td");
}

BOOST_AUTO_TEST_CASE(manual_load_resets_partial_route) {
    PracticeWidget widget(nullptr);
    widget.create_practice_structure(load("cis-1,2-dichloroethene"));
    widget.start_current_structure_flowchart();
    auto chart = widget.findChild<PracticeFlowchartWidget*>();
    chart->handle_answer(0, 0);
    std::vector<std::string> route;
    QObject::connect(&widget, &PracticeWidget::flowchart_route_changed,
                     [&route](const auto& path) { route = path; });
    widget.create_practice_structure(load("trans-1,2-dichloroethene"));
    BOOST_REQUIRE_EQUAL(route.size(), 1);
    BOOST_TEST(route.front() == "start");
    answer(chart, {0, 0, 1, 2, 0, 1});
    BOOST_TEST(route.back() == "Cnh");
}

BOOST_AUTO_TEST_CASE(random_next_exercise_still_requests_new_molecules) {
    PracticeWidget widget(nullptr);
    widget.set_library(std::make_shared<Library>());
    auto config = widget.findChild<PracticeConfigWidget*>();
    for (auto box : config->findChildren<QCheckBox*>()) {
        box->setChecked(box->text() == "Point group determination");
    }
    int requests = 0;
    QObject::connect(&widget, &PracticeWidget::request_new_structure, [&]() {
        ++requests;
        widget.create_practice_structure(load(requests == 1 ? "borane" : "methane"));
    });
    config->findChild<QPushButton*>()->click();
    auto chart = widget.findChild<PracticeFlowchartWidget*>();
    answer(chart, {0, 0, 1, 3, 1, 1});
    BOOST_REQUIRE(QMetaObject::invokeMethod(&widget, "create_exercise"));
    BOOST_TEST(requests == 2);
    answer(chart, {0, 1, 0});
    BOOST_REQUIRE(QMetaObject::invokeMethod(&widget, "create_exercise"));
    BOOST_TEST(requests == 3);

    // A manual load during random practice becomes a route for that molecule;
    // restarting it must not silently replace the user's selected molecule.
    widget.create_practice_structure(load("trans-1,2-dichloroethene"));
    answer(chart, {0, 0, 1, 2, 0, 1});
    BOOST_REQUIRE(QMetaObject::invokeMethod(&widget, "create_exercise"));
    BOOST_TEST(requests == 3);
    answer(chart, {0, 0, 1, 2, 0, 1});
}

BOOST_AUTO_TEST_SUITE_END()
