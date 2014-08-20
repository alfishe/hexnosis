/**
 * Copyright (c) 2014 Ralph Janke. All rights reserved.
 *
 * Author: Ralph Janke hexnosis@jankeconsulting.ca
 */

#include "hexnosiswindow.h"
#include "hexfilemodel.h"
#include <QDebug>

HexFileModel::HexFileModel(QObject *parent, QFile *newfile) :
//    QStandardItemModel(parent),
  QAbstractTableModel(parent),
  file(newfile),
  tablewidth(16)
{
}

HexFileModel::~HexFileModel()
{
    delete file;
    filebuffer.clear();
}

int HexFileModel::rowCount(const QModelIndex & /* parent */) const
{
//    TODO: store count in class variable
    if((filebuffer.size() / tablewidth) * tablewidth == filebuffer.size())
        return filebuffer.size() / tablewidth;
    else
        return (filebuffer.size() / tablewidth) + 1;
}

int HexFileModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
//    if(parent.row() < rowCount())
        return tablewidth;
//    else
//        return filebuffer.size() - ((filebuffer.size() / tablewidth) * tablewidth) - 1;
}

QVariant HexFileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole))
        return QVariant();
    if (index.row()*columnCount()+index.column() >= filebuffer.size())
        return QVariant();
    return filebuffer.at(index.row()*columnCount()+index.column());
}

QByteArray HexFileModel::data( int length, const QModelIndex &index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole))
        return QByteArray();
    if (index.row()*columnCount()+index.column() >= filebuffer.size())
        return QByteArray();
    int len = filebuffer.size() - (index.row()*columnCount()+index.column());
    if(len < length)
        length = len;
    QByteArray data = QByteArray();
    data.resize(length);
    for (int i=0; i<length; i++)
        data[i] = filebuffer.at(index.row()*columnCount()+index.column()+i);
    return data;
}

bool HexFileModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid() && role == Qt::EditRole) {
        filebuffer.replace(index.row()*columnCount()+index.column(), 1, value.toByteArray());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool HexFileModel::setData(const QModelIndex &index, const QByteArray &value, int role)
{
    QModelIndex index2;
    if ((index.column() + value.size()) < columnCount()) {
        index2 = QAbstractItemModel::createIndex(index.row(), index.column()+value.size()-1);
    }
    else {
        int row = (index.row() + (value.size() / tablewidth)) + 1;
        int column = (index.column()+value.size()-1) % tablewidth;
        index2 = QAbstractItemModel::createIndex(row, column);
    }

    if(index.isValid() && role == Qt::EditRole) {
        filebuffer.replace(index.row()*columnCount()+index.column(), value.size(), value);
        emit dataChanged(index, index2);
        return true;
    }
    return false;
}

Qt::ItemFlags HexFileModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    //    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool HexFileModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(value);
    Q_UNUSED(role);
//  Headers are automatically set, cannot be set from outside
    return true;
}

QVariant HexFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
//        TODO: Should representation of header be located in Delegate?
        if(orientation == Qt::Horizontal) {
            return QString("%1").arg(QByteArray(1, section).toHex().toUpper().data()[1]);
        }
        if(orientation == Qt::Vertical) {
            return QString("%1h").arg(QByteArray(3, 0).setNum(section * tablewidth, 16).toUpper().data());
        }
    }
    return QVariant();
}

void HexFileModel::loadDatafromFile(int length, int offset)
{
    Q_UNUSED(length);
    Q_UNUSED(offset);
    if(file) {
        filebuffer = file->readAll();

        file->close();
    }
}

bool HexFileModel::saveDatatoFile(QString file_name, int length, int offset)
{
    Q_UNUSED(length);
    Q_UNUSED(offset);

    if (!file_name.isEmpty()) {
        file = new QFile(file_name);

        if (file->open(QIODevice::WriteOnly)) {
            file->write(filebuffer);
            file->close();
            return true;
        }
    }
    return false;
}

void HexFileModel::createBuffer(int len, char fillchar)
{
    emit beginResetModel();
    filebuffer = QByteArray(len, fillchar);
    emit endResetModel();
}

void HexFileModel::setColumnHeaders()
{
    for(int i=0; i<16; i++) {
//        QStandardItem *item = new QStandardItem(QString().number(i,16).toUpper());
        //setHorizontalHeaderItem(i, item);
        //setHorizontalHeaderItem(i+16, item);
    }
}

QString HexFileModel::fileInfo()
{
    QString info = QString();
    QFileInfo fi(*file);
    info.append(tr("Name: %1\n").arg(fi.fileName()));
    info.append(tr("Path: %1\n").arg(fi.path()));
    info.append(tr("Size: %1\n").arg(filebuffer.size()));
    info.append(tr("Size: %1\n").arg(fi.size()));
    return info;
}

void HexFileModel::updateCursorInfo(QModelIndex current, QModelIndex previous)
{
    Q_UNUSED(previous);
    if(current.isValid())
        HexnosisWindow::updateCursorInfo(current.row()*columnCount()+current.column(), data(current, Qt::DisplayRole).toInt());
    else
        HexnosisWindow::clearCursorInfo();
}

