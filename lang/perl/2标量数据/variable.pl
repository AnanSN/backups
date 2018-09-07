#!/usr/bin/env perl

# 换行
print "hello world\n";

# 不换行
print "The answer is ";
print 6 * 7; # 直接计算
print ".\n";

# 列表
print "The answer is ", 7 * 8, ".\n";

# 变量
$meal = "brontosaurus steak";
$barney = "fred ate a $meal";
$barney1 = 'fred ate a' .$meal. "\n";

#print $meal
#print $barney
print $barney1
