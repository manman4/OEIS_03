# Cf. https://manchanr6.blogspot.com/2017/05/170516.html

def s(f_ary, g_ary, n)
  s = 0
  (1..n).each{|i| s += i * f_ary[i] * g_ary[i] ** (n / i) if n % i == 0}
  s
end

def A(ary, n)
  a_ary = [1]
  a = [0] + (1..n).map{|i| ary.inject(0){|s, j| s + s(j[0], j[1], i)}}
  (1..n).each{|i| a_ary << (1..i).inject(0){|s, j| s + a[j] * a_ary[-j]} / i}
  a_ary
end

n = 5100
ary1 = Array.new(n + 1, -1)
ary2 = Array.new(n + 1,  1)
ary3 = (0..n).map{|i|  i}

# Product_{k>0} (1 - x^k)/(1 - k * x^k)
ary = A([[ary1, ary2], [ary2, ary3]], n)

(0..5000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
