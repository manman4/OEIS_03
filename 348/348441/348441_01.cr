def a(n)
  ary = [1, 1]
  ary1 = [1]
  last = 1
  s, t = 1, 1
  print 1
  print ' '
  puts s
  (3..n).each{|i|
    cnt = 1
    while ary1.includes?(cnt * last)
      cnt += 1
    end
    ary << cnt
    ary1 << cnt * last
    last = cnt
    if last > t
      t = last
      s += 1
      print s
      print ' '
      puts i
    end
  }
  ary
end

n = 100000
a(n)
