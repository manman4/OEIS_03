def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def Jtilde(k, n)
  return 0 if k == 0
  return (2r ** n * f(n)) ** 2 / f(2 * n + 1) if k == 1
  if n == 0
    return 1 if k == 2
    return 0
  end
  if n == 1
    return 3r / 4 if k == 2
    return 1      if k == 3 || k == 4
    return 0
  end
  ((8r * n * n - 8 * n + 3) * Jtilde(k, n - 1) - 4 * (n - 1) ** 2 * Jtilde(k, n - 2) + 4 * Jtilde(k - 2, n - 1)) / (4 * n * n)
end

def A326748(n)
  (0..n).map{|i| (1..2 * i + 2).map{|j| Jtilde(j, i).denominator}}.flatten
end

n = 15
ary = A326748(n)
(0..ary.size - 1).each{|i|
  print i
  print ' '
  puts ary[i]
}
