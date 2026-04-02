# a(0) = a(1) = a(2) = 1; a(n) = a(n-1) + (n-2)*a(n-3).
def A394799(n)
  a = [1, 1, 1]
  (3..n).each{|i|
    a << a[-1] + (i - 2) * a[-3]
  }
  a
end

# a(n) = A394799(n) - Sum_{k=0..n-1} a(k) * A394799(n-1-k).
def A(n)
  ary = A394799(n)
  a = []
  (0..n).each{|i| a << ary[i] - (0..i-1).inject(0){|s, k| s + a[k] * ary[i-1-k]}}
  a
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