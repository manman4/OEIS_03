def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  ary = [1]
  (1..n).each{|i| 
    ary << (0..(i - 1) / 3).inject(0){|s, j| s + (-1) ** j * ncr(i - 2 * j, j) * ary[-1 - 3 * j]}
  }
  ary
end

n = 1000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}