# Let H(n,r) = Sum_{k=1..n} 1/k^r. 
def h(n, r)
  (1..n).inject(0){|s, k| s + 1r / k**r}
end

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a(n) = (n!)^3 * (H(n,3)^3 - 3*H(n,3)*H(n,6) + 2*H(n,9))/6 where H(n,r) = Sum_{k=1..n} 1/k^r.
def a(n)
  f(n)**3 * (h(n, 3)**3 - 3*h(n, 3)*h(n, 6) + 2*h(n, 9)) / 6
end

n = 300
(0..n).each{|i|
  j = a(i).to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
