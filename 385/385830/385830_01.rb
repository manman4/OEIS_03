# a(0) = 1; a(n) = Sum_{k=0..n-1} (1 + k^2) * a(k) * a(n-1-k).
def A(n)
  ary = [1]
  (1..n).each{|i|
    ary << (0..i-1).inject(0){|s, k| s + (1 + k * k) * ary[k] * ary[i - 1 - k]}
  }
  ary
end

n = 300
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}