def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  (0..n).map{|i| (0..i).inject(0){|s, j| s + ncr(2 * j, j) ** 3 * ncr(2 * i - 2 * j, i - j) * 2 ** (4 * i - 4 * j)}}
end

def B(n)
  a, b = 1, 40
  ary = [1]
  i = 0
  while i < n
    i += 1
    a, b = b, (8 * (2 * i + 1) * (8 * i * i + 8 * i + 5) * b - 4096 * i ** 3 * a) / (i + 1) ** 3
    ary << a
  end
  ary
end

def A2(n)
  (0..n).map{|i| (0..i).inject(0){|s, j| s + (ncr(2 * j, j) ** 2 * ncr(2 * i - 2 * j, i - j) ** 2)}}
end

n = 30
p A(n)
p B(n)
p A2(n)
