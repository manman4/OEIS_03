require 'prime'

def pow(a, m, mod)
  return 1 % mod if m == 0
  k = pow(a, m >> 1, mod)
  k *= k
  return k % mod if m & 1 == 0
  return k * a % mod
end

def A(n)
  s = 1
  flag = false
  while !flag
    s += 1
    flag = true
    (1..n - 1).each{|i|
      if i != pow(i, s, n)
        flag = false
        break
      end
    }
  end
  s
end

n = 10000
ary = []
i = 2
while ary.size < n - 1
  ary << A(i) if i.prime_division.to_a.all?{|j| j[1] == 1}
  i += 1
end
(2..n).each{|i|
  print i
  print ' '
  puts ary[i - 2]
}
