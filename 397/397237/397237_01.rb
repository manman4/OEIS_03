# a(n) = a(n-3) + a(n-4) + 2*a(n-5) + a(n-7).
def b(n) 
  ary = [1, 0, 0, 1, 1, 2, 1]
  (7..n).each{|i| ary << ary[i-3] + ary[i-4] + 2*ary[i-5] + ary[i-7]}
  ary
end

# a(n) = b(2*n+3) = A397236(2*n+3)
def a(n)
  a = b(2*n+3)
  (0..n).map{|i| a[2*i+3]}
end

n = 33
p a(n)