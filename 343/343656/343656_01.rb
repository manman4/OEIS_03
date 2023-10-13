require 'prime'

def d(n)
  return 1 if n == 1
  n.prime_division.inject(1){|s, i| s * (i[1] + 1)}
end

def T(n, k)
  d(n ** k)
end

def A343656(n)
  ary = []
  (1..n).each{|i|
    (1..i).each{|j|
      ary << T(j, i - j)
    }
  }
  ary
end

n = 140
ary = A343656(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
