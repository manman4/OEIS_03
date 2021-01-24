def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end
def A(k, n)
  ary = [1]
  (1..n).each{|i|
    ary << (- f(i - 1) * (1..[i, k].min).inject(0){|s, j| s + ary[i - j] / f(i - j).to_r}).to_i
  }
  ary
end

m = 1000
n = 500
ary = A(4, m)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
