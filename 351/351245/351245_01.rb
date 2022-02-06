require 'prime'
def A(k, n)
  n.prime_division.map{|x| x[0]}.inject(0){|s, i| s + (n / i) ** k}
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(5, i)
}

