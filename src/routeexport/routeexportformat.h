/*****************************************************************************
* Copyright 2015-2020 Alexander Barthel alex@littlenavmap.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef LNM_ROUTEEXPORTFORMAT_H
#define LNM_ROUTEEXPORTFORMAT_H

#include "routeexport/routeexportflags.h"

#include <QApplication>

class RouteExport;

/*
 * Defines all aspects of an file format to be exported from either menu or automatic
 * export as defined in multiexport dialog.
 *
 * Can be serialized to a data stream or variant.
 */
class RouteExportFormat
{
  Q_DECLARE_TR_FUNCTIONS(RouteExportFormat)

public:
  /* See field documentation for a description for parameters */
  RouteExportFormat(rexp::RouteExportFormatType typeParam, rexp::RouteExportFormatFlags flagsParam,
                    const QString& formatParam, const QString& categoryParam, const QString& commentParam)
    : type(typeParam), format(formatParam), category(categoryParam), comment(commentParam), flags(flagsParam)
  {
  }

  RouteExportFormat()
  {

  }

  /* Call the assigned export function with this as a parameter */
  bool callExport() const
  {
    return func(*this);
  }

  /* Get format enum */
  rexp::RouteExportFormatType getType() const
  {
    return type;
  }

  /* Get format enum as interger */
  int getTypeAsInt() const
  {
    return type;
  }

  /* Get file extension or filename if isFile is true */
  const QString& getFormat() const
  {
    return format;
  }

  /* Get a filter string for the file dialog. Usually "(*.format)". */
  QString getFilter() const;

  const QString& getComment() const
  {
    return comment;
  }

  /* Get category like "Simulator" or "Aircraft". */
  const QString& getCategory() const
  {
    return category;
  }

  /* Get default path. Some are updated by change of simulator if depends on simulator path */
  const QString& getDefaultPath() const
  {
    return defaultPath;
  }

  /* Get assigned path or default if assinged is empty. Directory or file if isFile is true */
  const QString& getPathOrDefault() const
  {
    return path.isEmpty() ? defaultPath : path;
  }

  /* Get assigned path. Directory or file if isFile is true*/
  const QString& getPath() const
  {
    return path;
  }

  rexp::RouteExportFormatFlags getFlags() const
  {
    return flags;
  }

  bool isSelected() const
  {
    return flags.testFlag(rexp::SELECTED);
  }

  /* Format is cloned and selected for manual export from file menu */
  bool isManual() const
  {
    return flags.testFlag(rexp::MANUAL);
  }

  bool isExportToFile() const
  {
    return flags.testFlag(rexp::FILE);
  }

  /* Format is cloned and selected for manual export from file dialog. Forces file dialog to be shown. */
  bool isFileDialog() const
  {
    return flags.testFlag(rexp::FILEDIALOG);
  }

  /* Export in bulk/multi mode */
  bool isMulti() const
  {
    return !isFileDialog() && !isManual();
  }

  /* Assign data from an incomplete format object which was loaded from settings. */
  void copyLoadedData(RouteExportFormat& other) const;

  /* Create a clone with manual flag to indicate call from menu items */
  RouteExportFormat copyForManualSave() const
  {
    RouteExportFormat other;
    other = *this;
    other.flags.setFlag(rexp::MANUAL);
    return other;
  }

  /* Create a clone with force file dialog flag to indicate call from multi export dialog. */
  RouteExportFormat copyForManualSaveFileDialog() const
  {
    RouteExportFormat other;
    other = *this;
    other.flags.setFlag(rexp::FILEDIALOG);
    return other;
  }

  /* true if directoy or file exists */
  bool isPathValid(QString *errorMessage = nullptr) const;

private:
  friend QDataStream& operator>>(QDataStream& dataStream, RouteExportFormat& obj);
  friend QDataStream& operator<<(QDataStream& dataStream, const RouteExportFormat& obj);

  friend class RouteExportFormatMap;

  rexp::RouteExportFormatType type;

  QString format, category, comment, defaultPath, path;

  /* Callback function for export */
  std::function<bool(const RouteExportFormat&)> func;
  rexp::RouteExportFormatFlags flags = rexp::NONE;
};

Q_DECLARE_METATYPE(RouteExportFormat);

QDataStream& operator>>(QDataStream& dataStream, RouteExportFormat& obj);
QDataStream& operator<<(QDataStream& dataStream, const RouteExportFormat& obj);

/* ======================================================================= */

/*
 * A map of type enum to format objects. Can be serialized to a data stream or variant.
 */
class RouteExportFormatMap
  : public QMap<rexp::RouteExportFormatType, RouteExportFormat>
{
  Q_DECLARE_TR_FUNCTIONS(RouteExportFormatMap)

public:
  /* Save and load to/from settings file */
  void saveState();
  void restoreState();

  /* true if there are any selected export formats in the list.*/
  bool hasSelected() const;

  /* Get all formats that are selected for multiexport */
  QVector<RouteExportFormat> getSelected() const;

  /* Clear user selected path and use default again */
  void clearPath(rexp::RouteExportFormatType type)
  {
    (*this)[type].path.clear();
  }

  void updatePath(rexp::RouteExportFormatType type, const QString& path);

  /* Enable or disable for multiexport */
  void setSelected(rexp::RouteExportFormatType type, bool selected)
  {
    (*this)[type].flags.setFlag(rexp::SELECTED, selected);
  }

  /* Get a clone with flag indicating manually saving */
  RouteExportFormat getForManualSave(rexp::RouteExportFormatType type) const
  {
    return value(type).copyForManualSave();
  }

  /* Update all callbacks */
  void initCallbacks(RouteExport *routeExport);

  /* Update default paths depending on selected flight simulator */
  void updateDefaultPaths();

private:
  /* Initialize format map */
  void init();

};

Q_DECLARE_METATYPE(RouteExportFormatMap);

QDataStream& operator>>(QDataStream& dataStream, RouteExportFormatMap& obj);
QDataStream& operator<<(QDataStream& dataStream, const RouteExportFormatMap& obj);

#endif // LNM_ROUTEEXPORTFORMAT_H
