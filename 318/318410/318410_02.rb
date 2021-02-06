def f(k, m, a)
  (a ** k + 1) % (10 ** m)
end

def A(k, m, n)
  sa = 0
  a = m
  while sa < n
    sa += 1
    (0..9).each{|i|
      a0 = i * 10 ** sa + a
      10.times{a0 = f(k, sa + 1, a0)}
      if a0 == (i * 10 ** sa + a)
        a = a0
        break
      end
    }
  end
  ary = a.to_s.split('').map(&:to_i)
  ary.reverse + [0] * (n + 1 - ary.size)
end

n = 5010
ary = A(9, 9, n)
(0..5000).each{|i|
  j = ary[i]
  print i
  print ' '
  puts j
}
