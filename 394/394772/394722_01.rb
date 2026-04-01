# a(0) = 1; a(n) = (n-1) * Sum_{k=0..n-1} a(k) * a(n-1-k) + Sum_{i, j, k>=0 and i+j+k=n-1} a(i) * a(j) * a(k).
def A(n)
  ary = [1]
  (1..n).each{|i|
    ary << (i - 1) * (0..i - 1).inject(0){|s, k| s + ary[k] * ary[i - 1 - k]} + (0..i - 1).inject(0){|s, j| s + (0..i - 1 - j).inject(0){|t, k| t + ary[j] * ary[k] * ary[i - 1 - j - k]}}
  }
  ary
end

n = 420
ary = A(n)
(0..n).each{|i| 
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}