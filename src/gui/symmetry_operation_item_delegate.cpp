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

#include "symmetry_operation_item_delegate.h"

/**
 * @brief Renders the delegate using the given painter and style option_in
 * for the item specified by index.
 *
 * @param painter
 * @param option_in
 * @param index
 */
void SymmetryOperationItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option_in, const QModelIndex& index) const {
    QStyleOptionViewItem option = option_in;
    initStyleOption(&option, index);

    if (option.text.isEmpty()) {
        // not handled by this function
        QStyledItemDelegate::paint(painter, option_in, index);
        return;
    }

    QStyle *style = option.widget ? option.widget->style() : QApplication::style();

    QTextOption text_option;
    text_option.setWrapMode(option.features & QStyleOptionViewItem::ViewItemFeature::WrapText ? QTextOption::WrapMode::WordWrap : QTextOption::WrapMode::ManualWrap);
    text_option.setTextDirection(option.direction);

    QTextDocument doc;
    doc.setDefaultTextOption(text_option);
    doc.setHtml(option.text);
    doc.setDefaultFont(option.font);
    doc.setDocumentMargin(2);
    doc.setTextWidth(option.rect.width());
    doc.adjustSize();

    if (doc.size().width() > option.rect.width()) {
        // elide text
        QTextCursor cursor(&doc);
        cursor.movePosition(QTextCursor::MoveOperation::End);

        const QString ellipsis = "\u2026";
        QFontMetrics metric(option.font);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        int ellipsis_width = metric.horizontalAdvance(ellipsis);
#else
        int ellipsis_width = metric.width(ellipsis);
#endif

        while (doc.size().width() > option.rect.width() - ellipsis_width) {
            cursor.deletePreviousChar();
            doc.adjustSize();
        }

        cursor.insertText(ellipsis);
    }

    // paint item without text (takes care of painting e.g. highlight)
    option.text = QString();
    style->drawControl(QStyle::ControlElement::CE_ItemViewItem, &option, painter, option_in.widget);

    // determine where to render the text (alignment)
    QRect text_rect = style->subElementRect(QStyle::SubElement::SE_ItemViewItemText, &option);
    QSize document_size(doc.size().width(), doc.size().height());  // QSizeF -> QSize
    QRect layout_rect = QStyle::alignedRect(Qt::LayoutDirection::LayoutDirectionAuto, option.displayAlignment, document_size, text_rect);

    painter->save();

    // translate painter to origin of layout rectangle to render text correctly
    painter->translate(layout_rect.topLeft());
    doc.drawContents(painter, text_rect.translated(-text_rect.topLeft()));

    painter->restore();

    // draw button if data exists in the button role
    if (index.data(ItemDataRole::ButtonRole).toBool()) {
        QStyleOptionButton button;
        button.rect = this->button_rect(option_in);
        button.text = "\u25b6";
        button.state = QStyle::StateFlag::State_Enabled;

        QApplication::style()->drawControl(QStyle::ControlElement::CE_PushButton, &button, painter);
    }
}

/**
 * @brief Returns the size needed by the delegate to display the item
 * specified by index, taking into account the style information provided
 * by option_in.
 *
 * @param option_in
 * @param index
 * @return QSize
 */
QSize SymmetryOperationItemDelegate::sizeHint(const QStyleOptionViewItem& option_in, const QModelIndex& index) const {
    QStyleOptionViewItem option = option_in;
    initStyleOption(&option, index);

    if (option.text.isEmpty()) {
        // not handled by this function
        return QStyledItemDelegate::sizeHint(option_in, index);
    }

    QTextDocument doc;
    doc.setHtml(option.text);
    doc.setTextWidth(option.rect.width());
    doc.setDefaultFont(option.font);
    doc.setDocumentMargin(2);

    return QSize(doc.idealWidth(), doc.size().height());
}

/**
 * @brief When editing of an item starts, this function is called with the
 * event that triggered the editing, the model, the index of the item, and
 * the option_in used for rendering the item.
 *
 * @param event
 * @param model
 * @param option_in
 * @param index
 * @return true if event is handled
 */
bool SymmetryOperationItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option_in, const QModelIndex& index) {
    if (index.data(ItemDataRole::ButtonRole).toBool() && event->type() == QEvent::Type::MouseButtonRelease) {
        QMouseEvent *e = (QMouseEvent *) event;
        QRect button_rect = this->button_rect(option_in);

        if (e->x() >= button_rect.left() && e->x() <= button_rect.right() &&
            e->y() >= button_rect.top() && e->y() <= button_rect.bottom()) {
            model->setData(index, true, ItemDataRole::ButtonClickedRole);
        }
    }

    return false;  // pass through event
}

/**
 * @brief Get rectangle of button
 *
 * @param option_in
 * @return QRect
 */
QRect SymmetryOperationItemDelegate::button_rect(const QStyleOptionViewItem& option_in) const {
    int x, y, w, h;

    x = option_in.rect.right() - option_in.rect.height();
    y = option_in.rect.top();
    w = option_in.rect.height();
    h = option_in.rect.height();

    return QRect(x, y, w, h);
}
