def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  ary = [1]
  (1..n).each{|i|
    ary << (0..(i - 1) / k).inject(0){|s, j| s + (j + 1r / k) ** j * f(k * j) / f(j) * ncr(i - 1, k * j) * ary[-1 - k * j]}.to_i
  }
  ary
end

n = 500
ary = A(3, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
