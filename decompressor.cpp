#include "decompressor.h"

Decompressor::Decompressor()
{

}
int Decompressor::decompress(string sourcePath, string destinationPath, string pw = "") {
    /**打开源文件**/
    if (sourcePath.substr(sourcePath.find_last_of(".") + 1) != "bak")
        return 1; // 源文件扩展名不是bak
    ifstream inFile;
    inFile.open(sourcePath, ios::in | ios::binary);
    if (!inFile)
        return 2; // 打开源文件失败
    /**密码校验**/
    unsigned char uchar;
    int pw_len = pw.length();
    inFile.read((char*)&uchar, sizeof(char));
    int zeroNum = uchar;
    if (zeroNum >= 8) {
        zeroNum -= 8;
        char crMD5_c[33];
        inFile.get(crMD5_c, 33);
        string crMD5 = string(crMD5_c, 32);
        //cout<<"d:crMD5="<<crMD5<<endl;
        string pwMD5 = MD5().getMD5(pw);
        //cout<<"d:pwMD5="<<pwMD5<<endl;
        if (crMD5 != pwMD5) {
            inFile.close();
            return 6; // 密码错误
        }
    }
    /**打开目标文件**/
    ofstream outFile;
    qDebug() << QString::fromStdString(sourcePath);
    sourcePath = QFileInfo(QString::fromStdString(sourcePath)).fileName().toStdString();
    string newFileName = destinationPath + sourcePath.substr(0, sourcePath.find_last_of(".")) + ".tar";
    qDebug() << QString::fromStdString(newFileName);
    outFile.open(newFileName, ios::out | ios::binary);
    if (!outFile) {
        inFile.close();
        return 3; // 打开目标文件失败
    }
    /**读出频率表**/
    unsigned long long freq;
    map<unsigned char, unsigned long long> freqMap;
    int i = 0;
    for (i = 0; i < 256; i++) {
        inFile.read((char*)&freq, sizeof(freq));
        if (freq) {
            freqMap[i] = freq;
        }
    }
    if (i != 256)
        return 4; // 文件过短，频率表不完整
    /**建立词频小顶堆**/
    priority_queue<haffNode*, vector<haffNode*>, compare> freqHeap;
    map<unsigned char, unsigned long long>::reverse_iterator it;
    for (it = freqMap.rbegin(); it != freqMap.rend(); it++) {
        haffNode* pn = new (haffNode);
        pn->freq = it->second;
        pn->uchar = it->first;
        pn->left = pn->right = 0;
        freqHeap.push(pn);
    }
    /**构建哈夫曼树**/
    while (freqHeap.size() > 1) {
        haffNode* pn1 = freqHeap.top();
        freqHeap.pop();
        haffNode* pn2 = freqHeap.top();
        freqHeap.pop();
        haffNode* pn = new (haffNode);
        pn->freq = pn1->freq + pn2->freq;
        pn->left = pn1;
        pn->right = pn2;
        freqHeap.push(pn);
    }
    haffNode* root = freqHeap.top();
    codeMap.clear();
    /**读出主体，用哈夫曼树树解码**/
    haffNode* decodePointer = root;
    string buf, now;
    inFile.read((char*)&uchar, sizeof(unsigned char));
    int pw_index = 0;
    if (pw_len) {
        uchar ^= pw[pw_index++];
        pw_index %= pw_len;
    }
    bitset<8> bs = uchar;
    buf = bs.to_string();
    while (inFile.read((char*)&uchar, sizeof(unsigned char))) {
        if (pw_len) {
            uchar ^= pw[pw_index++];
            pw_index %= pw_len;
        }
        bitset<8> bs = uchar;
        now = buf;
        buf = bs.to_string();
        for (char i = 0; i < 8; i++) {
            if (now[i] == '0') {
                if (!decodePointer->left)
                    return 7; // 解码错误
                decodePointer = decodePointer->left;
            } else {
                if (!decodePointer->right)
                    return 7; // 解码错误
                decodePointer = decodePointer->right;
            }
            if (!(decodePointer->left || decodePointer->right)) {
                //cout<<int(decodePointer->uchar)<<" ";
                outFile.write((char*) & (decodePointer->uchar), sizeof(decodePointer->uchar));
                decodePointer = root;
            }
        }
    }
    //最后一字节
    now = buf;
    for (char i = 0; i < (8 - zeroNum) % 8; i++) {
        if (now[i] == '0') {
            if (!decodePointer->left)
                return 7; // 解码错误
            decodePointer = decodePointer->left;
        } else {
            if (!decodePointer->right)
                return 7; // 解码错误
            decodePointer = decodePointer->right;
        }
        if (!(decodePointer->left || decodePointer->right)) {
            outFile.write((char*) & (decodePointer->uchar), sizeof(unsigned char));
            decodePointer = root;
        }
    }

    inFile.close();
    outFile.close();
    if (decodePointer == root) return 0; // 正常执行
    return 5; // 文件结尾不完整
}
