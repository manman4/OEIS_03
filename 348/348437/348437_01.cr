def a(n)
  ary = [1, 1]
  ary1 = [1]
  last = 1
  (3..n).each{|i|
    cnt = 1
    while ary1.includes?(cnt * last)
      cnt += 1
    end
    ary << cnt
    ary1 << cnt * last
    last = cnt
  }
  ary
end

n = 70000
ary = [0] + a(n)
n = 500
(1..n).each{|i|
  print i
  print ' '
  puts ary.index(i)
}

