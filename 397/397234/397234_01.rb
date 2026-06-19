# a(n) = a(n-4) + a(n-5) + a(n-7).
def b(n) 
  ary = [1, 0, 0, 0, 1, 1, 0]
  (7..n).each{|i| ary << ary[i-4] + ary[i-5] + ary[i-7]}
  ary
end

# a(n) = b(2*n+5) = A397233(2*n+5)
def a(n)
  a = b(2*n+5)
  (0..n).map{|i| a[2*i+5]}
end

n = 43
p a(n)