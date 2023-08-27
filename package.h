#ifndef PACKAGE_H
#define PACKAGE_H
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>


class Package
{
private:

    int relativePathLength;//文件相对路径长度
    int fileLength;//文件长度
    bool isFileDictionary;//是否为文件


public:
    Package();
    int package(QStringList files, QString destination);
};

#endif // PACKAGE_H
