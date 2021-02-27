def s(k, n)
  s = 0
  (1..n).each{|i| s += (n / i) * k ** (i - 1) * (1 - (-1) ** (i % 2)) if n % i == 0}
  s
end

def A(k, n)
  ary = [1]
  a = (0..n).map{|i| s(k, i)}
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s - a[j] * ary[-j]} / i.to_r}
  ary
end

n = 3000
ary = A(2, n)
(0..n).each{|i|
  j = ary[i].to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
