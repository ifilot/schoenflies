#include "settings_dialog.h"
#include "theme.h"
#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Settings"));
    auto layout = new QVBoxLayout(this);
    auto form = new QFormLayout;
    auto theme = new QComboBox(this);
    theme->setObjectName("themeSelection");
    theme->addItem(tr("Dark"));
    theme->addItem(tr("Light"));
    QSettings settings("Schoenflies", "Schoenflies");
    theme->setCurrentIndex(Theme::load(settings) == Theme::Mode::Light ? 1 : 0);
    form->addRow(tr("&Theme:"), theme);
    layout->addLayout(form);
    auto note = new QLabel(tr("Changes apply immediately and are saved for the next session."), this);
    note->setWordWrap(true);
    layout->addWidget(note);
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
    connect(theme, &QComboBox::currentIndexChanged, this, [this](int index) {
        const auto mode = index == 1 ? Theme::Mode::Light : Theme::Mode::Dark;
        Theme::apply(*qApp, mode);
        QSettings settings("Schoenflies", "Schoenflies");
        if (!Theme::save(settings, mode))
            QMessageBox::warning(this, tr("Settings"), tr("The theme was applied, but the preference could not be saved."));
    });
    setMinimumWidth(380);
}
