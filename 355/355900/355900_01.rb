n = 100000
a, b = 1, 1
c = 1
d = 0
ary = []
(1..n).each{|i|
  c = a.gcd(b)
  a, b = b, c + (a + b) / c
  break if c.to_s.size > 1000
  if c > d
    d = c
    ary << i
    puts i
  end
}
p ary