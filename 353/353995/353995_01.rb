def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  (0..n - 2).inject(0){|s, i| s + f(i) * ncr(n, i + 1)}
end

def B(n)
  a_ary = (0..n).map{|i| A(i)}
  ary = [1]
  (1..n).each{|i|
    ary << (1..i).inject(0){|s, j| s + a_ary[j] * ncr(i - 1, j - 1) * ary[-j]}
  }
  ary
end

n = 500
ary = B(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}