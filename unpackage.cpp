#include "unpackage.h"

Unpackage::Unpackage()
{

}

int Unpackage::unPackage(QString tarFilename, QString destination) {
    QFile tar(tarFilename);
    bool success = tar.open(QFile::ReadOnly);
    if (!success) return 1;

    QString relativePath;

    while (tar.read((char*)&relativePathLength, sizeof (relativePathLength))) {
        char* _relativePath = new char[relativePathLength + 1];
        tar.read(_relativePath, relativePathLength);
        _relativePath[relativePathLength] = '\0';
        relativePath = QString::fromStdString(std::string(_relativePath));
        delete[] _relativePath;
        qDebug() << relativePath;
        tar.read((char*)&isFileDictionary, sizeof (isFileDictionary));
        tar.read((char*)&fileLength, sizeof (fileLength));
        qDebug() << fileLength;
        if (!isFileDictionary) {
            QFile data(destination + "/" + relativePath);
            bool success = data.open(QFile::WriteOnly);
            if (!success) return 2;
            if (fileLength) {
                char* content = new char[fileLength];
                tar.read(content, fileLength);
                data.write(content, fileLength);
                delete[] content;
            } else {
                data.write("");
            }
            data.close();
        } else {
            QDir dir;
            if (QFileInfo(destination + "/" + relativePath).exists()) continue;
            bool success = dir.mkdir(destination + "/" + relativePath);
            if (!success) return 3;
        }
    }
    return 0;
}


