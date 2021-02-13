require 'prime'

def A(n)
  a = (0..n).to_a
  k = 2
  while (a.inject(:+) - n) % k != 0 || k.prime?
    k += 1
    (1..n).each{|i| a[i] *= i}
  end
  k
end

n = 500
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
