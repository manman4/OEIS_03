require 'prime'

def A(n)
  ary = [0]
  s = 0
  (1..n).each{|i| 
    s += 1 if i.prime?
    ary << s
  }
  ary
end

def B(n)
  ary = A(10 ** 7)
  a = [0, 1]
  b = [0, 1]
  s = 0
  t = 1
  (2..n).each{|i|
    s += a[i - 1]
    j = ary[i] - ary[s]
    a << j
    t += j
    b << t
  }
  b
end
n = 20000
ary = B(n)
p (1..n).map{|i| i - ary[i]}
p (1..n).select{|i| i - ary[i] == 2}
# (1..n).each{|i|
#   j = ary[i]
#   break if j.to_s.size > 1000
#   print i
#   print ' '
#   puts j
# }
