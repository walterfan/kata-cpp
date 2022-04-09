作为一个专业的程序员，你写了一段程序，完成了一个功能，似乎达到了用户的要求，其实你心里也没底。
做一个简单的测试, 跑了一个主要流程，基本的功能应该是满足需求的,但是性能呢？

我们需要对程序的性能进行度量，确保性能是满足用户需求的。如果性能不理想，必需要找到瓶颈在哪里。

最近在读狄更斯的小说《双城记》英文原版，既是学习英语，也是领略大师的行文。同时，我想知道大师喜欢用什么词，所以写了一个小程序来统计这部小说的词汇。让我们度量一下这个小程序的性能，看看瓶颈在哪里， 有什么好的方法。

# 程序示例

## 需求

## 设计


我想看看
* 大师在这本书中使用了多少词汇
* 大师用的最多的20个单词
* "think" 这个单词用了多少次
* "think" 排在最常使用率排名的多少位

用C++ 来写一段小程序，取名为 WordBank , 基本功能就是

1. 获取总的单词数量： `getTotalWordCount();`
2. 获取不重复的单词总数：`getUniqueWordCount);`
3. 打印若干个使用频率最高的单词：`printTop(int n) `
3. 获取某个单词的使用频度排序号： `getWordRank(const std::string& name);`
4. 获取某个单词的使用数量：`getWordCount(const std::string& name);`

然后, 使用 `std::map<std::string, int> m_mapWords;` 来保存单词的个数，使用 `std::vector<std::pair<std::string, int>> m_vecWords;` 来保存排过序的单词使用率排名。

```
#ifndef __WORD_BANK_H__
#define __WORD_BANK_H__

#include <cstdint>
#include <string>
#include <map>
#include <set>
#include <vector>

class WordBank
{
public:
	WordBank(const std::string& words_file);

	int getWordRank(const std::string& name) const;

	int getWordCount(const std::string& name) const;

	void sortWords();

	void printTop(int n) const;

	int getTotalWordCount() const;

	int getUniqueWordCount() const;

private:

	bool hasWord(const std::string& word) const;

	void increaseWordCount(const std::string& word);

	void addWord(const std::string& word);

	std::map<std::string, int> m_mapWords;

	std::vector<std::pair<std::string, int>> m_vecWords; 

	uint32_t m_wordCount;

};

std::string convertString(const std::string& word);

#endif
```

## 实现

```
#include "WordBank.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

bool comparePair(pair<string, int>& a, 
         pair<string, int>& b) 
{ 
    return a.second > b.second; 
} 

WordBank::WordBank(const string & words_file): m_wordCount(0) {
	ifstream is(words_file.data());
	if (!is) {
		throw invalid_argument("unable open file");
	}
	string word;
	while (is >> word) {
		m_wordCount ++;
		if (!hasWord(word)) {
			addWord(word);
		}else {
			increaseWordCount(word);
		}
	}
}

int WordBank::getWordRank(const std::string& word) const {
	
	int count = getWordCount(word);
	if (count == 0) {
		return 0;
	}
	
	int rank = 1;
	for (const auto&[key, value] : this->m_mapWords) {
		if (value > count) {
			rank++;
		}
	}
	return rank;
}

int WordBank::getWordCount(const std::string& word) const {
	string str = convertString(word.data());
	auto it = m_mapWords.find(str);
	
	if (it != m_mapWords.end()) {
		return it->second;
	}
	return 0;
}


int WordBank::getTotalWordCount() const {
	return m_wordCount;
}

int WordBank::getUniqueWordCount() const {
	return m_mapWords.size();
}

bool WordBank::hasWord(const string & word) const {
	string str = convertString(word.data());
	auto it = m_mapWords.find(str);
	
	if (it != m_mapWords.end()) {
		return true;
	}
	return false;
}
void WordBank::increaseWordCount(const string & word) {
	string str = convertString(word.data());
	auto it = m_mapWords.find(str);

	if (it != m_mapWords.end()) {
		it->second++;
	}
}
void WordBank::addWord(const string & word) {
	
	string str = convertString(word);
	if (str.empty()) {
		return;
	}
	//cout << "insert " << word  << " -> " << str << endl;
	m_mapWords.insert(make_pair(str, 1));
}

void WordBank::sortWords() {
   
    for (auto& pair : m_mapWords ) { 
        m_vecWords.push_back(pair); 
    } 
  
    // Sort using comparator function 
    sort(m_vecWords.begin(), m_vecWords.end(), comparePair); 
  

} 

void WordBank::printTop(int n) const {
	for (auto pair : m_vecWords) {
		cout << pair.first << ": " << pair.second << endl;
		if (--n <= 0) {
			break;
		}
	}

}

string convertString(const string & word) {
	string str("");
	for (size_t i = 0; i < word.length(); ++i) {
		uint8_t ch = word[i];
		if(ch > 0 && ch < 255 && isalpha(ch)) {
			str.push_back(tolower(ch));
		}
	}
	return str;
}
````

# 测试

```
#include "WordBank.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <boost/timer/timer.hpp>


using namespace std;
using namespace boost::timer;

int main(int argc, char *argv[])
{
	string word_search = "rtp";
	string word_file = "rfc3550.txt";
	int topN = 10;
	if (argc > 1) {
		word_search = argv[1];
		cout << "Make statistics for word count and rank: " <<convertString(word_search) << endl;
	} else {
		cout << "usage: " << argv[0] << "<search_word> <input_file> <topN>" << endl;
		cout << "example: " << argv[0] << " " << word_search << " " << word_file << " " << topN <<endl;
	}
	if (argc > 2) {
		word_file = argv[2];
		cout << "Read file: " << word_file << endl;
	}
	if (argc > 3) {
		topN = atoi(argv[3]);
	}
	
	try {
		boost::timer::auto_cpu_timer timer;
		
        WordBank wordbank(word_file);
        wordbank.sortWords();
		std::cout << "wordbank total word count: " << wordbank.getTotalWordCount() << endl;
        std::cout << "wordbank unique word count: " << wordbank.getUniqueWordCount() << endl;
        cout << word_search << "'s count=" << wordbank.getWordCount(word_search) << endl;
        cout << word_search  << "'s rank=" << wordbank.getWordRank(word_search) << endl;
        cout << "--- top "  << topN << " ---" << endl;
        wordbank.printTop(topN);

	} catch(const invalid_argument& e) {
        cerr << "Caught exeption: " <<e.what() <<endl;

    }
    return 0;

}

```

执行结果如下, 双城记总词数为139021 ，共使用了 10984 个单词，最常使用的词是 "the"， 而 “think” 这个词用了 120 次，排名 150 位。

```
./bin/wordbankdemo think A-Tale-of-Two-Cities.txt 20
Make statistics for word count and rank: think
Read file: A-Tale-of-Two-Cities.txt
wordbank total word count: 139021
wordbank unique word count: 10984
think's count=120
think's rank=150
--- top 20 ---
the: 8202
and: 4998
of: 4137
to: 3545
a: 2981
in: 2642
it: 2016
his: 2005
i: 1917
that: 1904
he: 1833
was: 1765
you: 1460
with: 1354
had: 1297
as: 1148
at: 1045
her: 1038
for: 973
him: 965
 0.079933s wall, 0.060000s user + 0.000000s system = 0.060000s CPU (75.1%)
```

# 度量性能
使用 boost 库的 auto_cpu_timer 来计时

# 剖析性能
使用著名的 valgrind 软件所附带的 callgrind 的剖析性能瓶颈在哪里
``` 
sudo apt install valgrind
valgrind --tool=callgrind ./bin/wordbankdemo
sudo apt-get install python3 graphviz

# 在 Debian/Ubuntu 安装 python3 和 graphviz
apt-get install python3 graphviz

# 在 RedHat/Fedora  安装 python3 和 graphviz
yum install python3 graphviz

# 再安装 gprof2dot
pip install gprof2dot

# 生成有向图
gprof2dot -f callgrind -n10 -s callgrind.out.816 > valgrind.dot
dot -Tpng valgrind.dot -o valgrind.png
```

![](https://upload-images.jianshu.io/upload_images/1598924-6e1079c264c70129.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

从上图可以看出,其大部分时间花在 WordBank 的构造方法,其中 `hasWord`, `increaseWordCount` 这两个方法花了大部分时间, 其中 `convertString` 和map 的红黑树的 `find` 方法又占据了大部分时间, 仔细看看这两个方法的调用,其实我们能够做出优化

```
WordBank::WordBank(const string & words_file): m_wordCount(0) {
	ifstream is(words_file.data());
	if (!is) {
		throw invalid_argument("unable open file");
	}
	string word;
	while (is >> word) {
		m_wordCount ++;
		if (!hasWord(word)) {
			addWord(word);
		}else {
			increaseWordCount(word);
		}
	}
}
```

1) word 可以事先转化 `convertString` 方法不需要重复调用
2) `hasWord` 和` increaseWordCount` 不需要重复查找, 可以把迭代器作为额外参数传入, 或者把 map 的 value 的引用传出来


其实这个测试有点问题，对于 getWordRank 的调用次数不够，反应不了它的问题，性能测试需要大量高频的调用并度量。改成二分查找会更好。


试着改一下,再重复上述的度量过程, 你能清晰地看出来改进的效果。

代码链接参见 ../exam

* WordBank.h
* WordBank.cpp
* WordBankDemo.cpp

