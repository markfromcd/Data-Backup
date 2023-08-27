#ifndef UNPACKAGE_H
#define UNPACKAGE_H
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>


class Unpackage
{

private:
    int relativePathLength;//文件相对路径长度
    int fileLength;//文件长度
    bool isFileDictionary;//是否为文件
public:
    Unpackage();
    int unPackage(QString tarFilename, QString destination);
};
#endif // UNPACKAGE_H
