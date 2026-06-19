# a(n) = a(n-4) + a(n-5) + a(n-7).
def a(n) 
  ary = [1, 0, 0, 0, 1, 1, 0]
  (7..n).each{|i| ary << ary[i-4] + ary[i-5] + ary[i-7]}
  ary
end

n = 59
p a(n)