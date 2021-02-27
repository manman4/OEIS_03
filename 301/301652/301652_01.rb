def A(n)
  return 0 if n == 0
  cnt = 1
  s = 1
  ary = [0, 1]
  while n > s
    cnt += 1
    s *= cnt
    ary << s
  end
  s_ary = []
  while cnt > 0
    k = ary[cnt]
    l = n / k
    n = n % k
    s_ary << l
    cnt -= 1
  end
  s_ary.shift if s_ary[0] == 0
  s_ary.reverse
end

ary = (0..2000).map{|i| A(i)}.flatten!
(0..ary.size - 1).each{|i|
  print i
  print ' '
  puts ary[i]
}
