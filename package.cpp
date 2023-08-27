#include "package.h"

Package::Package()
{

}

int Package::package(QStringList files, QString destination)
{
//    qDebug()<<files;
//    qDebug()<<destination;
    //目标tar文件
    QFile tar(destination);
    bool success = tar.open(QFile::WriteOnly);
    if (!success) return 1;
    //根路径
    auto root = QFileInfo(files[0]).path();
    for (auto& file : files) {
        if (QFileInfo(file).isFile()) {//如果是文件
            auto relativePath = QString(file).replace(root, "");
            relativePathLength = relativePath.length();
            //写入相对路径
            tar.write((const char*)&relativePathLength, sizeof (relativePathLength));
            tar.write(relativePath.toStdString().c_str());
            fileLength = QFileInfo(file).size();
            isFileDictionary = 0;
            //写入是否为文件
            tar.write((const char*)&isFileDictionary, sizeof (isFileDictionary));
            //写入文件长度目录
            tar.write((const char*)&fileLength, sizeof (fileLength));
            QFile data(file);
            bool success = data.open(QFile::ReadOnly);
            if (!success) return 2;
            tar.write(data.readAll());
            data.close();
        } else { //如果是目录
            QDirIterator iter(file, QDirIterator::Subdirectories);
            while (iter.hasNext()) {
                iter.next();
                QFileInfo info = iter.fileInfo();
                if (info.fileName() == "..") continue;
                if (info.isDir()) {//如果是文件夹
                    auto relativePath = QString(info.absoluteFilePath()).replace(root, "");
                    relativePathLength = relativePath.length();
                    //写入相对路径
                    tar.write((const char*)&relativePathLength, sizeof (relativePathLength));
                    tar.write(relativePath.toStdString().c_str());
                    fileLength = 0;
                    isFileDictionary = 1;
                    //写入文件长度目录
                    tar.write((const char*)&isFileDictionary, sizeof (isFileDictionary));
                    //写入文件长度
                    tar.write((const char*)&fileLength, sizeof (fileLength));
                } else {//如果是文件
                    auto relativePath = QString(info.absoluteFilePath()).replace(root, "");
                    relativePathLength = relativePath.length();
                    //写入相对路径
                    tar.write((const char*)&relativePathLength, sizeof (relativePathLength));
                    tar.write(relativePath.toStdString().c_str());
                    fileLength = info.size();
                    isFileDictionary = 0;
                    //写入文件长度目录
                    tar.write((const char*)&isFileDictionary, sizeof (isFileDictionary));
                    //写入文件长度
                    tar.write((const char*)&fileLength, sizeof (fileLength));
                    QFile data(info.absoluteFilePath());
                    bool success = data.open(QFile::ReadOnly);
                    if (!success) return 2;
                    tar.write(data.readAll());
                    data.close();
                }
            }
        }
    }

    tar.close();
    return 0;
}





