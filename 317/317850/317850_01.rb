def f(k, m, a)
  (a ** k + 1) % (10 ** m)
end

def A(k, m, n)
  s = 0
  a = m
  while s < n
    s += 1
    (0..9).each{|i|
      a0 = i * 10 ** s + a
      2.times{a0 = f(k, s + 1, a0)}
      if a0 == (i * 10 ** s + a)
        a = a0
        break
      end
    }
  end
  ary = a.to_s.split('').map(&:to_i)
  ary.reverse + [0] * (n + 1 - ary.size)
end

m = 5010
ary = A(7, 3, m)
(0..5000).each{|i|
  j = ary[i]
  print i
  print ' '
  puts j
}
