require 'matrix'
require 'prime'

def power(a, n, mod)
  return Matrix.I(a.row_size) if n == 0
  m = power(a, n >> 1, mod)
  m = (m * m).map{|i| i % mod}
  return m if n & 1 == 0
  (m * a).map{|i| i % mod}
end

def f(n)
  v = Vector.elements([1, 0])
  a = Matrix[*[[2, 1], [1, 0]]]
  mod = 10 ** 20
  # power(a, n, mod) * vÇÃê¨ï™ÇÕmodñ¢ñû
  ((power(a, n - 1, n) * v)[0] + 1) % n
end

def A(n)
  ary = []
  i = 0
  while ary.size < n
    i += 1
    if !i.prime? && i > 1
      ary << i if f(i) == 0
    end
  end
  ary
end

n = 50
ary = A(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
