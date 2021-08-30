require 'prime'

def A(n)
  s = 0
  (1..n).each{|i|
    j = i ** i
    break if j > n
    s += j * (n / j - (n - 1) / j)
  }
  s
end

def A347399(n)
  (1..n).map{|i| A(i ** i)}
end

p A347399(18)
# If p is prime, a(p) = 1 + p^p.
Prime.take(100).each{|i|
 if A(i ** i) - (i ** i + 1) != 0
  puts "out!"
 end
}