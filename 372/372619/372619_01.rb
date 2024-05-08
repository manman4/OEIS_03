def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

# T(n,k) = Sum_{j=1..n} phi(k*j)
def T(n, k)
  (1..n).inject(0){|s, i| s + phi(k * i)} / phi(k)
end

def A(n)
  a = []
  (1..n).each{|i|
    a << (1..i).map{|j| T(j, i - j + 1)}
  }
  a.flatten
end

n = 140
ary = A(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
