def A(n)
  ary = [1, 1]
  ary1 = [1]
  last = 1
  (3..n).each{|i|
    cnt = 1
    while ary1.include?(cnt * last)
      cnt += 1
    end
    ary << cnt
    ary1 << cnt * last
    last = cnt
  }
  ary
end

n = 10000
ary = [0] + A(n)
n = 100
(1..n).each{|i|
  print i
  print ' '
  puts ary.index(i)
}

