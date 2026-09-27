#include "theme.h"
#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include <QStyle>
#include <QSettings>

void Theme::apply(QApplication& application, Mode mode) {
    if (application.style()->objectName().compare("fusion", Qt::CaseInsensitive) != 0)
        application.setStyle(QStyleFactory::create("Fusion"));
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#20252d"));
    palette.setColor(QPalette::WindowText, QColor("#edf2f7"));
    palette.setColor(QPalette::Base, QColor("#171c23"));
    palette.setColor(QPalette::AlternateBase, QColor("#292f39"));
    palette.setColor(QPalette::Text, QColor("#edf2f7"));
    palette.setColor(QPalette::Button, QColor("#343e4c"));
    palette.setColor(QPalette::ButtonText, QColor("#edf2f7"));
    palette.setColor(QPalette::BrightText, Qt::white);
    palette.setColor(QPalette::Light, QColor("#607086"));
    palette.setColor(QPalette::Midlight, QColor("#465368"));
    palette.setColor(QPalette::Mid, QColor("#465368"));
    palette.setColor(QPalette::Dark, QColor("#11151b"));
    palette.setColor(QPalette::Shadow, Qt::black);
    palette.setColor(QPalette::Highlight, QColor("#24659b"));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Link, QColor("#80caff"));
    palette.setColor(QPalette::LinkVisited, QColor("#c9afff"));
    palette.setColor(QPalette::ToolTipBase, QColor("#343e4c"));
    palette.setColor(QPalette::ToolTipText, QColor("#edf2f7"));
    palette.setColor(QPalette::PlaceholderText, QColor("#aab7c8"));
    for (auto role : {QPalette::WindowText, QPalette::Text, QPalette::ButtonText})
        palette.setColor(QPalette::Disabled, role, QColor("#8995a6"));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor("#343e4c"));
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor("#8995a6"));
    if (mode == Mode::Light) {
        palette = application.style()->standardPalette();
        palette.setColor(QPalette::Window, QColor("#f3f5f8"));
        palette.setColor(QPalette::WindowText, QColor("#20242b"));
        palette.setColor(QPalette::Base, Qt::white);
        palette.setColor(QPalette::AlternateBase, QColor("#e8edf3"));
        palette.setColor(QPalette::Text, QColor("#20242b"));
        palette.setColor(QPalette::Button, QColor("#e1e7ef"));
        palette.setColor(QPalette::ButtonText, QColor("#20242b"));
        palette.setColor(QPalette::Midlight, QColor("#d4dce7"));
        palette.setColor(QPalette::Mid, QColor("#a4afbd"));
        palette.setColor(QPalette::Highlight, QColor("#24659b"));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        palette.setColor(QPalette::Link, QColor("#005ea8"));
        palette.setColor(QPalette::LinkVisited, QColor("#7040a0"));
        palette.setColor(QPalette::ToolTipBase, QColor("#ffffff"));
        palette.setColor(QPalette::ToolTipText, QColor("#20242b"));
        palette.setColor(QPalette::PlaceholderText, QColor("#586575"));
        for (auto role : {QPalette::WindowText, QPalette::Text, QPalette::ButtonText})
            palette.setColor(QPalette::Disabled, role, QColor("#687585"));
    }
    application.setPalette(palette);
    application.setStyleSheet(
        "QToolTip { color: palette(tool-tip-text); background: palette(tool-tip-base);"
        " border: 1px solid palette(mid); padding: 4px; }"
        "QPushButton:checked { background: palette(highlight); color: palette(highlighted-text); }"
        + QString("QLabel#correctFeedback { color: %1; font-weight: 700; padding: 6px; }"
                  "QLabel#incorrectFeedback { color: %2; font-weight: 700; padding: 6px; }")
          .arg(mode == Mode::Dark ? "#80e0b0" : "#17653b",
               mode == Mode::Dark ? "#ffd080" : "#854500"));
}

Theme::Mode Theme::load(QSettings& settings) {
    return settings.value("appearance/theme", "dark").toString() == "light" ? Mode::Light : Mode::Dark;
}

bool Theme::save(QSettings& settings, Mode mode) {
    settings.setValue("appearance/theme", mode == Mode::Light ? "light" : "dark");
    settings.sync();
    return settings.status() == QSettings::NoError;
}

void Theme::restore(QApplication& application) {
    QSettings settings("Schoenflies", "Schoenflies");
    apply(application, load(settings));
}
