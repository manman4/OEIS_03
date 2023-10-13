require 'prime'

def A(n)
  (1..n).inject(0){|s, i| s + i ** i.gcd(n)}
end

n = 100
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  # FORMURLA確認
  if i.prime?
    k = (i - 1) * i / 2 + i ** i
    break if j != k
  end
  print i
  print ' '
  puts j
}
