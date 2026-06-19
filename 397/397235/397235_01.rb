# a(n) = a(n-2) + a(n-5) + a(n-6).
def b(n) 
  ary = [1, 0, 1, 0, 1, 1]
  (6..n).each{|i| ary << ary[i-2] + ary[i-5] + ary[i-6]}
  ary
end

# a(n) = b(3*n+4) = A079955(3*n+4)
def a(n)
  a = b(3*n+4)
  (0..n).map{|i| a[3*i+4]}
end

n = 31
p a(n)