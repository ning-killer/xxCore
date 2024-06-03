#include "EmxCore.hpp"
#include "OutlineOsd.h"

using namespace Emx;

// 计算字符串的字符数
double CountCharacters(const std::string& str) {
    double count = 0;
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        if (c <= 127) {
            i += 1; // ASCII字符
            count += 0.5; // 英文字符算0.5个中文字符
        } else if (c >= 192 && c <= 247) {
            size_t charLength = (c <= 223) ? 2 : ((c <= 239) ? 3 : 4);
            i += charLength; // UTF-8字符
            count += 1; // 中文字符算1个中文字符
        } else {
            // 不支持的UTF-8编码
            return 0;
        }
    }
    return count;
}

void FilterText(std::vector<std::string>& text) {
    std::vector<std::vector<std::string>::iterator> elementsToRemove;
    int countbranch = 0;
    for (auto element = text.begin(); element != text.end(); ++element) {
        countbranch++;
        if (countbranch > 9) {
            elementsToRemove.push_back(element);
            continue;
        }
        if (CountCharacters(*element) >= 24) {
            double charCount = 0.5; // 初始值为0.5，算上第一个英文字符的一半
            size_t index = 0;
            size_t length = 0;
            while (charCount < 24 && index < (*element).length()) {
                unsigned char c = static_cast<unsigned char>((*element)[index]);
                size_t charLength = 1; // 默认为单字节字符
                if (c <= 127) {
                    charLength = 1;
                } else if (c >= 192 && c <= 247) {
                    charLength = (c <= 223) ? 2 : ((c <= 239) ? 3 : 4);
                }
                if (charCount + 0.5 <= 24) {
                    index += charLength;
                    charCount += (c <= 127) ? 0.5 : 1; // 英文字符算0.5个中文字符，中文字符算1个中文字符
                    length += charLength;
                } else {
                    break; // 超过24个字符，停止添加
                }
            }
            *element = (*element).substr(0, index);
        }
    }
    // 在循环之外执行擦除操作
    for (auto element : elementsToRemove) {
        text.erase(element);
    }
}

void SplitText(const std::string &src
    , std::vector<std::string> &obj) {
    size_t start = 0;
    while (start < src.size()) {
        size_t end = src.find("\\n", start);
        if (end == std::string::npos) {
            end = src.size();  
        }
        std::string substr = src.substr(start, end - start);
        if (!substr.empty()) {
            obj.push_back(substr);
        }
        if (end != src.size()) {
            obj.push_back("\\n"); 
        }
        start = end + 2;  
    }
}

void FilterSpecialCh(char *text, OutlineOsd *outline) {
    std::string input = std::string(text);
    std::string result;
    for (size_t i = 0; i < input.length(); ++i) {
        unsigned char a = input[i];
        int ch_in = 0;
        size_t charLength = 1;
        if (0xE == (a >> 4)) {
            unsigned char b = input[i + 1];
            unsigned char c = input[i + 2];
            unsigned char ch = static_cast<unsigned char>(input[i]);
            if (ch <= 127) {
                charLength = 1;
            } else if (ch >= 192 && c <= 247) {
                charLength = (ch <= 223) ? 2 : ((ch <= 239) ? 3 : 4);
            }
            ch_in = ((a & 0x0F) << 12) | ((b & 0x3F) << 6) | (c & 0x3F);
            if (!outline->IsExistChar(ch_in)) {
                emxlogd("is specialCh\n");
                result += "*";
                i += (charLength - 1);
                continue;
            }
        }
        std::string tmp = input.substr(i, charLength);
        result += tmp;
        i += (charLength - 1);
    }
    emxlogd("result: %s\n", result.c_str());
    memset(text, 0, strlen(text));
    memcpy(text, result.data(), result.size());
}

void FilterText(std::string &text) {
    emxlogd("run\n");
    if (CountCharacters(text) >= 24) {
        double charCount = 0; // 初始值为0.5，算上第一个英文字符的一半
        int index = 0;
        while (charCount < 24.0 && index < (int)text.length()) {
            unsigned char c = static_cast<unsigned char>(text[index]);
            int charLength = 1; // 默认为单字节字符
            if (c <= 127) {
                charLength = 1;
            } else if (c >= 192 && c <= 247) {
                charLength = (c <= 223) ? 2 : ((c <= 239) ? 3 : 4);
            }
            if (charCount <= 24.0) {
                index += charLength;
                charCount += (c <= 127) ? 0.5 : 1; // 英文字符算0.5个中文字符，中文字符算1个中文字符
                if (charCount == 24.5) {
                    index -= charLength;
                }
            } else {
                break; // 超过24个字符，停止添加
            }
        }
        text = text.substr(0, index);
    }
}

int main(int argc, char *argv[]) {
    std::string text1 = "123456789123456789123456789123456789123456789123456789";
    std::string text2 = "何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁";
    std::string text3 = "1何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁何宁";
    emxlogd("text1: %s\n", text1.c_str());
    FilterText(text1);
    emxlogd("FilterText1: %s\n", text1.c_str());
    emxlogd("text2: %s\n", text2.c_str());
    FilterText(text2);
    emxlogd("FilterText2: %s\n", text2.c_str());
    emxlogd("text3: %s\n", text3.c_str());
    FilterText(text3);
    emxlogd("FilterText3: %s\n", text3.c_str());
    return 0;
}