# author Cadmium

import re
from collections import Counter

fo = open("text.txt", "r+")
test_pr = fo.read()
#将大写字母都转换为小写字幕
test_low = test_pr.lower()
#获取单词表
c = Counter(re.split('\W', test_low))
#提取字典中的数字并排序
c_list = c.values()
c_list.sort()
#输出最大的十个数字
print c_list[-10:]