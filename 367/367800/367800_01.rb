# a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/8)} a(k) * a(n-1-8*k).
def A(k, n)
  ary = [1]
  (1..n).each{|i|
    ary << (0..(i - 1) / k).inject(0){|s, j| s + ary[j] * ary[i - 1 - k * j]}
  }
  ary
end

n = 10000
ary = A(8, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}