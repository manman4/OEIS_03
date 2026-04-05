# a(0) = 1, a(1) = a(2) = a(3) = 0;  a(n) = n*a(n-1) + a(n-2) + a(n-3) + a(n-4) + Sum_{k=1..n-1} a(k)*a(n-1-k).
def a(n)
  a = [1, 0, 0, 0]
  (4..n).each{|i| 
    a << i * a[-1] + a[-2] + a[-3] + a[-4] + (1..i - 1).inject(0){|s, k| s + a[k] * a[i - 1 - k]}
  }
  a
end

n = 600
ary = a(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
