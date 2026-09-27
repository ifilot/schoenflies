#include <boost/test/unit_test.hpp>
#include <QApplication>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include "../src/gui/practice_widget.h"

namespace {
struct ApplicationFixture {
    int argc = 1;
    char name[16] = "practice-test";
    char* argv[2] = {name, nullptr};
    QApplication application{argc, argv};
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
