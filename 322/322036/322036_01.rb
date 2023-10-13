require 'prime'

def A(n)
  n.prime_division.map{|i| [i[0]] * i[1]}.flatten
end

def B(n)
  return 0 if n == 1
  s = 1
  sum = 0
  A(n).each{|i|
    s /= i.to_r
    sum += s
  }
  sum
end

n = 2 ** 14
(1..n).each{|i|
  j = B(i)
  k = j.denominator - j.numerator
  break if k.to_s.size > 1000
  print i
  print ' '
  puts k
}
