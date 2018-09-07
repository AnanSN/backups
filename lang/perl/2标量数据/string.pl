#!/usr/bin/env perl

# 单引号内的字符串直接量
print 'fred';
print "\n";
print 'barney';
print "\n";
print '';
print "\n";
print 'Don\'t let an apostrophe end this string permaturely!';
print "\n";
print 'the last characters is a backslash: \\';
print "\n";
print 'hello\n'; # 单引号内的\n不换行
print "\n";
print 'hello
there';
print "\n";
print '\'\\'; # 只有在单引号里\后根单引号或者\才表示转义
print "\n";

# 双引号内的字符串直接量
print "barney"; # 同单引号
print "\n";
print "hello world\n"; # 可以接\n换行符
print "\n";
print "The last characters of this string is a quote mark: \""; # quote mark 引号
print "\n";
print "coke\tsprite"; # \t制表符
print "\n";
print "\x{2668}"; # Unicode中名为HOT SPRINGS的字符的代码点(code point)
print "\n";

# 双引号内字符串的反斜线转义
# \n 换行
# \r 回车
# \t 水平制表符
# \f 换页符
# \b 退格 
# \a 系统响铃
# \e Esc (ASCII编码的转义字符)
print "\e";
print "\n";
# \007 八进制表示的ASCII值，007表示系统响铃
# \x7f 十六进制表示的ASCII值，7f表示删除键的控制码
# \x{2744} 雪花形状
# \cC Ctrl键的代码，表示按下Ctrl+C键的返回码
print "\cC";
print "\n";
# \\ 反斜线
# \" 双引号
# \l 将下一个字母转为小写的
print "\lA";
print "\n";
# \L 将它后面所有的字母都转为小写的，直到\E为止
# \u 将下一个字母转为大写的
# \U 将它后面所有的字母都转为大写的，直到\E为止
# \Q 相当于把它到\E之间的非单词(non word)字符加上反斜线转义
print "\Q!@#$%^&*()_+abcdefg123456<>?:{}''\E";
print "\n";
# \E 结束\L \U \Q的作用范围

# 字符串操作符
# 连接操作符 .
print "hello" . "world"; # = "helloworld"
print "\n";
print "hello" . " " . "world"; # = 'hello world'
print "\n";
print 'hello world' . "\n"; # = "hello world \n"
# 重复操作符 x
print "fred" x 3;
print "\n";
print "barney" x (4+1);
print "\n";
print 5 x 4.8; # 本质上就是"5"x4，所以得5555，5被转换成字符串"5"
               # 4.8会取整为4，小于1取整时，会变为长度为零的空字符串
print "\n";

# 数字与字符串之间的自动转换

