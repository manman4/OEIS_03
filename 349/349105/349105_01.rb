def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  ary = [1]
  (1..n).each{|i|
    ary << (0..(i - 1) / 2).inject(0){|s, j| s + (j + 1) * ncr(i, 2 * j + 1) * ary[-2 * j - 1]}
  }
  ary
end

n = 600
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}