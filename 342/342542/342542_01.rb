require 'prime'

def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def A(n)
  (1..n).inject(0){|s, i| s + phi(i.gcd(n)) ** (n / (i.gcd(n)) - 1)}
end

n = 5000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  # FORMURLA確認
  if i.prime?
    k = i
    break if j != k
  end
  print i
  print ' '
  puts j
}
