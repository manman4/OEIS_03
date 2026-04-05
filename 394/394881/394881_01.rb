# a(0) = -1, a(1) = a(2) = a(3) = a(4) = a(5) = 0; a(n) = (n-1)/2 * a(n-1) - a(n-6) - ((n-1)/2 * b(n-1) - b(n-2) + b(n-3) - b(n-4) + b(n-5) - b(n-6)), where b(n) = Sum_{k=1..n} a(k)*a(n-k).
def a(n)
  a = [-1, 0, 0, 0, 0, 0]
  b = [ ]
  (0..5).each{|i| b << (1..i - 1).inject(0){|s, k| s + a[k] * a[i - 1 - k]}}
  (6..n).each{|i| 
    b << (1..i - 1).inject(0){|s, k| s + a[k] * a[i - 1 - k]}
    a << (i - 1) / 2r * a[-1] - a[-6] - ((i - 1) / 2r * b[-1] - b[-2] + b[-3] - b[-4] + b[-5] - b[-6])
  }
  a
end

n = 600
ary = a(n)
# b = [ ]
(0..n).each{|i|
  j = ary[i]
  break if j.denominator > 1
  j = j.to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
  # b << j
}
# p b