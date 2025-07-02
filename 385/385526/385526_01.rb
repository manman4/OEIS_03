def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end
def A(q, n)
  ary = [1]
  (1..n).each{|i| ary << (0..i - 1).inject(0){|s, j| s + (j + 1) * q ** j * ncr(i - 1, j) * ary[j] * ary[i - 1 - j]}}
  ary
end
def A385526(n)
  A(3, n)
end
p A385526(30) 