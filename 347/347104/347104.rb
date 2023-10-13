require 'prime'

def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def A347104(n)
  s = 0
  (1..n).each{|i|
    if i.prime?
      s += i * phi(n / i) if n % i == 0
    end
  }
  s
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A347104(i)
}