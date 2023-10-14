require 'prime'

# Cf. A344080
# tau(n^k)
def tau(n, k = 1)
  n.prime_division.inject(1){|s, i| s * (k * i[1] + 1)}
end

# a[0]=0
def tau_ary(n, k = 1)
  [0] + (1..n).map{|i| tau(i, k)}
end

def A(k, n)
  t_ary = tau_ary(n)
  ary = []
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += k ** (t_ary[j] - 1) if i % j == 0
    }
    ary << s
  }
  ary
end

n = 10000
ary = A(2, n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}