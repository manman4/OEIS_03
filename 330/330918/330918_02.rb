a = 0
ary = [0]
(1..10000).each{|i|
  if a - i > 0 && !ary.include?(a - i)
    a -= i
  else
    a += i
  end
  ary << a
}

n = 10 ** 4
(0..n).each{|i| 
  print i
  print " "
  puts ary[0..i].select{|j| j > i}.size
}
