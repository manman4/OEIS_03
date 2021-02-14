def A(n)
  a = [0]
  while a.size <= n
    b = []
    a.each{|i|
      if i == 0
        b << 0
        b << 1
      elsif i == 1
        b << 0
        b << 2
      elsif i == 2
        b << 0
        b << 3
      else
        b << 0
      end
    }
    a = b
  end
  a[0..n]
end
def B(n)
  a = A(200000)
  i = 0
  ary = []
  while ary.size < n
    ary << i if a[i] == 3
    i += 1
  end
  ary
end

n = 10000
ary = B(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
