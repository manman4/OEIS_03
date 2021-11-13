require "big"

def a(n)
  return 0 if n == 0
  k = 1
  (1..n).each{|i| k *= i}
  max = 0
  (1..n).to_a.each_permutation{|i|
    m = 0.to_big_i
    (1..n).each{|j| m += k * j // i[j - 1]}
    if m % k == 0
      max = m // k if max < m // k
    end
  }
  max
end

def a349257(n)
  (0..n).map{|i| a(i)}
end

p a349257(10)
