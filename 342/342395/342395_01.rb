require 'prime'

def A(n)
  (1..n).inject(0){|s, i| s + i ** (n / (i.gcd(n)))}
end

def A121706(n)
  (1..n - 1).inject(0){|s, i| s + i ** n}
end

n = 100
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  # FORMURLA確認
  if i.prime?
    k = A121706(i) + i
    break if j != k
  end
  print i
  print ' '
  puts j
}
