def s(k, n)
  s = 0
  (1..n).each{|i| s += i if n % i == 0 && i % k == 0}
  s
end

def A(ary, n)
  a_ary = [1]
  a = [0] + (1..n).map{|i| ary.inject(0){|s, j| s += j[1] * s(j[0], i)}}
  (1..n).each{|i| a_ary << (1..i).inject(0){|s, j| s - a[j] * a_ary[-j]} / i}
  a_ary
end

n = 50000
ary = A([[2, 1], [1, -1]], n)
s = 1
b=[]
(0..n).each{|i|
  j = ary[i]
  if j > s
     s *= 2
  elsif j == s
     puts i
     b <<i
  end
}
p b
