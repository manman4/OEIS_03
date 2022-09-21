def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A216857(n)
  (1..n).inject(0){|s, i| s + i ** (n - 1) * ncr(n, i)}
end

def A(n)
  a = (0..n).map{|i| A216857(i)}
  ary = [1]
  (1..n).each{|i|
    ary << (1..i).inject(0){|s, j| s + (-1) ** (j - 1) * a[j] * ncr(i - 1, j - 1) * ary[-j]}
  }
  ary
end

n = 400
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}