def A(n)
  f_ary = [1]
  a, b = 1, 2
  while f_ary[-1] < 10 ** 8
    a, b = b, a + b
    f_ary << a
  end
  a = [1]
  ary = [1]
  n.times{|i|
    a = [0] + a + [0]
    b = []
    (0..i + 1).each{|j|
      k = a[j] + a[j + 1]
      if f_ary.include?(k)
        b << k
      else
        b << 1
      end
    }
    a = b
    ary += a
  }
  ary
end

n = 139
ary = A(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
