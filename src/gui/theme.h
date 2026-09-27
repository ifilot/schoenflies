#ifndef GUI_THEME_H
#define GUI_THEME_H

class QApplication;
class QSettings;

namespace Theme {
// An explicit application palette keeps native OS theme changes from mixing
// light backgrounds with dark-mode text. Fusion renders all palette roles.
enum class Mode { Dark, Light };
void apply(QApplication& application, Mode mode = Mode::Dark);
Mode load(QSettings& settings);
bool save(QSettings& settings, Mode mode);
void restore(QApplication& application);
}

#endif
