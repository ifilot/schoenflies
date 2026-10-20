#include "settings_dialog.h"
#include "theme.h"
#include <QApplication>
#include <QCheckBox>
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
    auto complex_irreps = new QCheckBox(tr("Include point groups with E+/E\u2212 pairs"), this);
    complex_irreps->setObjectName("includeComplexIrreps");
    complex_irreps->setToolTip(tr("Include molecules whose point group has complex-conjugate pairs of irreducible "
                                  "representations (E<sub>+</sub> and E<sub>\u2212</sub>, e.g. <i>C</i><sub>3</sub>, "
                                  "<i>C</i><sub>4h</sub> and <i>S</i><sub>4</sub>) in irreducible representation "
                                  "exercises. Applying the little orthogonality theorem then requires the complex "
                                  "conjugate of the characters. Takes effect when a practice session is started."));
    complex_irreps->setChecked(load_include_complex_irreps(settings));
    form->addRow(tr("&Irreps practice:"), complex_irreps);
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
    connect(complex_irreps, &QCheckBox::toggled, this, [this](bool checked) {
        QSettings settings("Schoenflies", "Schoenflies");
        if (!save_include_complex_irreps(settings, checked))
            QMessageBox::warning(this, tr("Settings"), tr("The practice preference could not be saved."));
    });
    setMinimumWidth(380);
}

bool SettingsDialog::load_include_complex_irreps(QSettings& settings) {
    return settings.value("practice/include_complex_irreps", false).toBool();
}

bool SettingsDialog::save_include_complex_irreps(QSettings& settings, bool include) {
    settings.setValue("practice/include_complex_irreps", include);
    settings.sync();
    return settings.status() == QSettings::NoError;
}
