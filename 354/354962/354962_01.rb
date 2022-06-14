def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  ary = [1]
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s + (-1) ** (j + 1) * (k + 1) ** (j * (i - j)) * ncr(i, j) * ary[-j]}}
  ary[-1]
end

def A354962(n)
  (0..n).map{|i| A(i, i)}
end

n = 50
ary = A354962(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}  