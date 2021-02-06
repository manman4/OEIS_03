def A(m, n)
  ary = []
  while n != 0
    ary << n % m
    n /= m
  end
  ary.uniq!
  ary.size
end

def B(n)
  k = 2
  while A(n, f(k)) != n
    k += 1
  end
  k
end

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

n = 500
(2..n).each{|i|
  print i
  print ' '
  puts B(i)
}
