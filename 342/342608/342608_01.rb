require 'prime'

def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def A(n)
  (1..n).inject(0){|s, i| s + phi(n / (i.gcd(n))) ** (n + n / (i.gcd(n)) - 1)}
end

n = 300
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  # FORMURLA確認
  if i.prime?
    k = 1 + (i - 1) ** (2 * i)
    break if j != k
  end
  print i
  print ' '
  puts j
}
